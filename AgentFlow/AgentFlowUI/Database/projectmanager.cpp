#include "projectmanager.h"
#include "dbmanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDatabase>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVariant>
#include <QDateTime>
#include <QSettings>
#include <QDebug>

// 只过滤 AgentFlow 工程库（有 ui_config 表才算）
static QStringList filterAgentFlowDatabases(const QStringList &raw, DbManager &db, const DbConfig &baseCfg)
{
    QStringList result;
    for (const QString &name : raw) {
        // 跳过系统库
        if (name.compare("information_schema", Qt::CaseInsensitive) == 0 ||
            name.compare("mysql", Qt::CaseInsensitive) == 0 ||
            name.compare("performance_schema", Qt::CaseInsensitive) == 0 ||
            name.compare("sys", Qt::CaseInsensitive) == 0)
            continue;

        // 检查是否有 ui_config 表
        DbConfig cfg = baseCfg;
        cfg.dbName = name;
        if (!db.connectToDatabase(cfg, nullptr))
            continue;

        QSqlQuery query(db.database());
        query.prepare("SELECT COUNT(*) FROM information_schema.tables "
                      "WHERE table_schema = ? AND table_name = 'ui_config'");
        query.addBindValue(name);
        if (!query.exec() || !query.next())
            continue;
        if (query.value(0).toInt() == 0)
            continue; // 没有 ui_config 表，跳过

        result << name;
    }
    return result;
}

ProjectManager::ProjectManager(QObject *parent)
    : QObject(parent)
    , m_projectOpen(false)
{
}

bool ProjectManager::createProject(const QString &projectName,
                                   const QString &dbName,
                                   const QString &description,
                                   QString *err)
{
    DbManager &db = DbManager::instance();
    DbConfig cfg = db.loadConfig();

    // 1. 连接到 MySQL 服务器
    if (!db.connectToServer(cfg.host, cfg.port, cfg.userName, cfg.password, err))
        return false;

    // 2. 检查数据库是否已存在
    bool ok = false;
    bool exists = db.databaseExists(dbName, &ok);
    if (!ok) {
        if (err) *err = "Failed to check database existence";
        return false;
    }
    if (exists) {
        if (err) *err = QString("Database '%1' already exists").arg(dbName);
        return false;
    }

    // 3. 创建数据库
    if (!db.createDatabase(dbName, err))
        return false;

    // 4. 切换到新数据库
    cfg.dbName = dbName;
    if (!db.connectToDatabase(cfg, err))
        return false;

    // 5. 创建 ui_config 表
    if (!createUiConfigTable(err))
        return false;

    // 6. 写入工程元信息
    QString now = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    insertConfig("project", "project_name", projectName, "工程显示名称", err);
    insertConfig("project", "description", description, "工程描述", err);
    insertConfig("project", "created_at", now, "创建时间", err);
    insertConfig("project", "created_by", cfg.userName, "创建者", err);

    // 保存当前工程信息
    m_projectName = projectName;
    m_dbName = dbName;
    m_description = description;
    m_projectOpen = true;

    // 保存到 QSettings 以便下次打开
    QSettings settings("AgentFlow", "TestConfig");
    settings.setValue("DB/Name", dbName);
    settings.setValue("Project/Name", projectName);
    settings.setValue("Project/Description", description);

    emit projectOpened(projectName);
    return true;
}

bool ProjectManager::openProject(const QString &dbName,
                                 QString *err)
{
    DbManager &db = DbManager::instance();
    DbConfig cfg = db.loadConfig();
    cfg.dbName = dbName;

    // 1. 连接到数据库
    if (!db.connectToDatabase(cfg, err))
        return false;

    // 2. 检查 ui_config 表是否存在
    QSqlDatabase sqldb = db.database();
    QSqlQuery query(sqldb);
    query.prepare("SELECT COUNT(*) FROM information_schema.tables "
                  "WHERE table_schema = ? AND table_name = 'ui_config'");
    query.addBindValue(dbName);
    if (!query.exec() || !query.next()) {
        if (err) *err = "Failed to check ui_config table: " + query.lastError().text();
        return false;
    }
    if (query.value(0).toInt() == 0) {
        // 表不存在，创建它
        if (!createUiConfigTable(err))
            return false;
    }

    // 3. 加载工程元信息
    m_dbName = dbName;
    if (!loadProjectMeta(err))
        return false;

    m_projectOpen = true;

    // 保存到 QSettings
    QSettings settings("AgentFlow", "TestConfig");
    settings.setValue("DB/Name", dbName);
    settings.setValue("Project/Name", m_projectName);
    settings.setValue("Project/Description", m_description);

    emit projectOpened(m_projectName);
    return true;
}

bool ProjectManager::saveProject(const QVariantMap &configData, QString *err)
{
    if (!m_projectOpen) {
        if (err) *err = "No project is open";
        return false;
    }

    // 清空旧配置
    if (!clearUiConfig(err))
        return false;

    // 逐条写入配置
    for (auto it = configData.begin(); it != configData.end(); ++it) {
        QString key = it.key();
        QString value;
        QVariant val = it.value();

        // 复杂类型用 JSON 存储
        switch (val.type()) {
        case QVariant::Map:
        case QVariant::List: {
            QJsonDocument doc = QJsonDocument::fromVariant(val);
            value = QString::fromUtf8(doc.toJson(QJsonDocument::Compact));
            break;
        }
        default:
            value = val.toString();
            break;
        }

        if (!insertConfig("ui_config", key, value, QString(), err))
            return false;
    }

    // 更新保存时间
    insertConfig("project", "saved_at",
                 QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"),
                 "最后保存时间");

    emit projectSaved();
    return true;
}

void ProjectManager::closeProject()
{
    if (!m_projectOpen)
        return;

    m_projectOpen = false;
    m_projectName.clear();
    m_dbName.clear();
    m_description.clear();

    DbManager::instance().disconnect();
    emit projectClosed();
}

bool ProjectManager::updateProject(const QString &newName,
                                   const QString &newDescription,
                                   QString *err)
{
    if (!m_projectOpen) {
        if (err) *err = "No project is open";
        return false;
    }

    // 更新 project_name 和 description
    if (!insertConfig("project", "project_name", newName, "工程显示名称", err))
        return false;
    if (!insertConfig("project", "description", newDescription, "工程描述", err))
        return false;

    // 更新 saved_at
    insertConfig("project", "saved_at",
                 QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"),
                 "最后保存时间");

    // 同步内存中的状态
    m_projectName = newName;
    m_description = newDescription;

    // 同步 QSettings
    QSettings settings("AgentFlow", "TestConfig");
    settings.setValue("Project/Name", newName);
    settings.setValue("Project/Description", newDescription);

    emit projectSaved();
    return true;
}

bool ProjectManager::deleteProject(const QString &dbName, QString *err)
{
    DbManager &db = DbManager::instance();
    DbConfig cfg = db.loadConfig();

    // 1. 先连接到服务器（不指定数据库）
    if (!db.connectToServer(cfg.host, cfg.port, cfg.userName, cfg.password, err))
        return false;

    // 2. 检查数据库是否存在
    bool ok = false;
    bool exists = db.databaseExists(dbName, &ok);
    if (!ok) {
        if (err) *err = "Failed to check database existence";
        return false;
    }
    if (!exists) {
        if (err) *err = QString("Database '%1' does not exist").arg(dbName);
        return false;
    }

    // 3. DROP DATABASE
    QString sql = QString("DROP DATABASE `%1`").arg(dbName);
    if (!db.executeSql(sql, err))
        return false;

    // 4. 如果删除的是当前打开的工程，关闭它
    if (m_projectOpen && m_dbName == dbName) {
        closeProject();
    }

    return true;
}

QStringList ProjectManager::listProjects(QString *err)
{
    DbManager &db = DbManager::instance();
    DbConfig cfg = db.loadConfig();

    if (!db.connectToServer(cfg.host, cfg.port, cfg.userName, cfg.password, err))
        return QStringList();

    QSqlQuery query(db.database());
    if (!query.exec("SHOW DATABASES")) {
        if (err) *err = query.lastError().text();
        return QStringList();
    }

    QStringList databases;
    while (query.next())
        databases << query.value(0).toString();

    // 过滤：只保留有 ui_config 表的数据库（即 AgentFlow 工程）
    return filterAgentFlowDatabases(databases, db, cfg);
}

bool ProjectManager::createUiConfigTable(QString *err)
{
    QString sql =
        "CREATE TABLE IF NOT EXISTS `ui_config` ("
        "  `id` INT NOT NULL AUTO_INCREMENT,"
        "  `config_group` VARCHAR(64) NOT NULL DEFAULT 'ui_config',"
        "  `config_key` VARCHAR(128) NOT NULL,"
        "  `config_value` TEXT,"
        "  `description` VARCHAR(255) DEFAULT '',"
        "  `created_at` DATETIME,"
        "  `updated_at` DATETIME,"
        "  PRIMARY KEY (`id`),"
        "  UNIQUE KEY `uk_group_key` (`config_group`, `config_key`)"
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4";

    if (!DbManager::instance().executeSql(sql, err))
        return false;

    // 兼容旧表：如果 description 列不存在则添加
    QSqlQuery checkQuery(DbManager::instance().database());
    checkQuery.prepare("SELECT COUNT(*) FROM information_schema.columns "
                        "WHERE table_schema = DATABASE() "
                        "AND table_name = 'ui_config' "
                        "AND column_name = 'description'");
    if (checkQuery.exec() && checkQuery.next() && checkQuery.value(0).toInt() == 0) {
        DbManager::instance().executeSql(
            "ALTER TABLE `ui_config` ADD COLUMN `description` VARCHAR(255) DEFAULT '' AFTER `config_value`");
    }

    return true;
}

QVariantMap ProjectManager::loadUiConfig(QString *err)
{
    QVariantMap result;
    if (!m_projectOpen) {
        if (err) *err = "No project is open";
        return result;
    }

    QSqlQuery query(DbManager::instance().database());
    if (!query.exec("SELECT config_group, config_key, config_value FROM ui_config")) {
        if (err) *err = query.lastError().text();
        return result;
    }

    while (query.next()) {
        QString group = query.value(0).toString();
        QString key = query.value(1).toString();
        QString value = query.value(2).toString();

        result[QString("%1/%2").arg(group, key)] = value;
    }

    return result;
}

bool ProjectManager::clearUiConfig(QString *err)
{
    return DbManager::instance().executeSql(
        "DELETE FROM ui_config WHERE config_group = 'ui_config'", err);
}

bool ProjectManager::insertConfig(const QString &group,
                                  const QString &key,
                                  const QString &value,
                                  const QString &desc,
                                  QString *err)
{
    QSqlDatabase db = DbManager::instance().database();
    if (!db.isOpen()) {
        if (err) *err = "Database not connected";
        return false;
    }

    QSqlQuery query(db);
    query.prepare("INSERT INTO ui_config (config_group, config_key, config_value, description, created_at, updated_at) "
                  "VALUES (?, ?, ?, ?, NOW(), NOW()) "
                  "ON DUPLICATE KEY UPDATE config_value = VALUES(config_value), description = VALUES(description), updated_at = NOW()");
    query.addBindValue(group);
    query.addBindValue(key);
    query.addBindValue(value);
    query.addBindValue(desc);

    if (!query.exec()) {
        if (err) *err = query.lastError().text();
        return false;
    }
    return true;
}

bool ProjectManager::loadProjectMeta(QString *err)
{
    QSqlQuery query(DbManager::instance().database());
    query.prepare("SELECT config_key, config_value FROM ui_config WHERE config_group = 'project'");
    if (!query.exec()) {
        if (err) *err = query.lastError().text();
        return false;
    }

    while (query.next()) {
        QString key = query.value(0).toString();
        QString val = query.value(1).toString();

        if (key == "project_name")
            m_projectName = val;
        else if (key == "description")
            m_description = val;
    }

    // 如果没有工程名，用数据库名兜底
    if (m_projectName.isEmpty())
        m_projectName = m_dbName;

    return true;
}
