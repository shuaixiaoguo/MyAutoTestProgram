#include "dbmanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QSettings>
#include <QVariant>
#include <QMutex>
#include <QMutexLocker>

const QString DbManager::CONNECTION_NAME = "AgentFlowDbConn";

DbManager& DbManager::instance()
{
    static DbManager s_instance;
    return s_instance;
}

DbManager::~DbManager()
{
    disconnect();
}

DbConfig DbManager::loadConfig() const
{
    QSettings settings("AgentFlow", "TestConfig");
    DbConfig cfg;
    cfg.connectionName = settings.value("DB/ConnectionName", "默认连接").toString();
    cfg.host = settings.value("DB/Host", "127.0.0.1").toString();
    cfg.port = settings.value("DB/Port", 3306).toInt();
    cfg.userName = settings.value("DB/User", "root").toString();
    cfg.password = settings.value("DB/Password", "").toString();
    cfg.dbName = settings.value("DB/Name", "").toString();
    return cfg;
}

void DbManager::saveConfig(const DbConfig &cfg)
{
    QSettings settings("AgentFlow", "TestConfig");
    settings.setValue("DB/ConnectionName", cfg.connectionName);
    settings.setValue("DB/Host", cfg.host);
    settings.setValue("DB/Port", cfg.port);
    settings.setValue("DB/User", cfg.userName);
    settings.setValue("DB/Password", cfg.password);
    settings.setValue("DB/Name", cfg.dbName);
}

bool DbManager::isMysqlDriverAvailable() const
{
    return QSqlDatabase::isDriverAvailable("QMYSQL");
}

QStringList DbManager::availableDrivers() const
{
    return QSqlDatabase::drivers();
}

bool DbManager::hasLocalConfig() const
{
    QSettings settings("AgentFlow", "TestConfig");
    return settings.contains("DB/Host") && settings.contains("DB/User");
}

bool DbManager::autoConnect(QString *err)
{
    if (!hasLocalConfig()) {
        if (err) *err = "本地无数据库配置";
        return false;
    }

    DbConfig cfg = loadConfig();

    // 如果有数据库名，直接连接到数据库
    if (!cfg.dbName.isEmpty()) {
        return connectToDatabase(cfg, err);
    }

    // 否则只连接到服务器
    return connectToServer(cfg.host, cfg.port, cfg.userName, cfg.password, err);
}

bool DbManager::connectToServer(const QString &host, int port,
                                const QString &user, const QString &pwd,
                                QString *err)
{
    // 检查 MySQL 驱动是否可用
    if (!isMysqlDriverAvailable()) {
        if (err) {
            *err = QString("MySQL 驱动未加载!\n\n"
                "可用驱动: %1\n\n"
                "解决方法:\n"
                "1. 编译 Qt MySQL 驱动 (qsqlmysql.dll)\n"
                "   cd C:\\Qt\\Qt5.12.9\\5.12.9\\Src\\qtbase\\src\\plugins\\sqldrivers\\mysql\n"
                "   qmake \"INCLUDEPATH+=C:\\Program Files\\MySQL\\MySQL Server 5.5\\include\" "
                "\"LIBS+=-L\\\"C:\\Program Files\\MySQL\\MySQL Server 5.5\\lib\\\" -llibmysql\" mysql.pro\n"
                "   nmake\n"
                "2. 将编译出的 qsqlmysql.dll 复制到:\n"
                "   C:\\Qt\\Qt5.12.9\\5.12.9\\msvc2017_64\\plugins\\sqldrivers\\\n"
                "3. 将 libmysql.dll 复制到 exe 同目录或系统 PATH 中"
                ).arg(availableDrivers().join(", "));
        }
        return false;
    }

    // 先断开旧连接
    disconnect();

    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL", CONNECTION_NAME);
    db.setHostName(host);
    db.setPort(port);
    db.setUserName(user);
    db.setPassword(pwd);

    if (!db.open()) {
        if (err) *err = db.lastError().text();
        return false;
    }
    return true;
}

bool DbManager::connectToDatabase(const DbConfig &cfg, QString *err)
{
    disconnect();

    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL", CONNECTION_NAME);
    db.setHostName(cfg.host);
    db.setPort(cfg.port);
    db.setUserName(cfg.userName);
    db.setPassword(cfg.password);
    db.setDatabaseName(cfg.dbName);

    if (!db.open()) {
        if (err) *err = db.lastError().text();
        return false;
    }
    return true;
}

bool DbManager::createDatabase(const QString &dbName, QString *err)
{
    QSqlDatabase db = QSqlDatabase::database(CONNECTION_NAME, false);
    if (!db.isOpen()) {
        if (err) *err = "Not connected to server";
        return false;
    }

    QSqlQuery query(db);
    QString sql = QString("CREATE DATABASE IF NOT EXISTS `%1` "
                          "DEFAULT CHARACTER SET utf8mb4 "
                          "COLLATE utf8mb4_general_ci").arg(dbName);
    if (!query.exec(sql)) {
        if (err) *err = query.lastError().text();
        return false;
    }
    return true;
}

bool DbManager::databaseExists(const QString &dbName, bool *ok)
{
    QSqlDatabase db = QSqlDatabase::database(CONNECTION_NAME, false);
    if (!db.isOpen()) {
        if (ok) *ok = false;
        return false;
    }

    QSqlQuery query(db);
    query.prepare("SELECT SCHEMA_NAME FROM INFORMATION_SCHEMA.SCHEMATA "
                  "WHERE SCHEMA_NAME = ?");
    query.addBindValue(dbName);
    if (!query.exec()) {
        if (ok) *ok = false;
        return false;
    }
    if (ok) *ok = true;
    return query.next();
}

bool DbManager::executeSql(const QString &sql, QString *err)
{
    QSqlDatabase db = QSqlDatabase::database(CONNECTION_NAME, false);
    if (!db.isOpen()) {
        if (err) *err = "Not connected to database";
        return false;
    }

    QSqlQuery query(db);
    if (!query.exec(sql)) {
        if (err) *err = query.lastError().text();
        return false;
    }
    return true;
}

bool DbManager::isConnected() const
{
    QSqlDatabase db = QSqlDatabase::database(CONNECTION_NAME, false);
    return db.isOpen();
}

QString DbManager::currentDatabase() const
{
    QSqlDatabase db = QSqlDatabase::database(CONNECTION_NAME, false);
    return db.isOpen() ? db.databaseName() : QString();
}

void DbManager::disconnect()
{
    // 必须在独立作用域内关闭db，确保QSqlDatabase对象销毁后再removeDatabase
    {
        QSqlDatabase db = QSqlDatabase::database(CONNECTION_NAME, false);
        if (db.isValid()) {
            if (db.isOpen())
                db.close();
        }
    }
    if (QSqlDatabase::contains(CONNECTION_NAME))
        QSqlDatabase::removeDatabase(CONNECTION_NAME);
}

QSqlDatabase DbManager::database()
{
    return QSqlDatabase::database(CONNECTION_NAME, false);
}
