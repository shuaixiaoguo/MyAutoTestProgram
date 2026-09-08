#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantMap>
#include <functional>

class ProjectManager : public QObject
{
    Q_OBJECT
public:
    explicit ProjectManager(QObject *parent = nullptr);

    // 新建工程：用用户指定的数据库名创建
    bool createProject(const QString &projectName,
                       const QString &dbName,
                       const QString &description,
                       QString *err = nullptr);

    // 打开工程：连接到已有数据库，加载配置
    bool openProject(const QString &dbName,
                     QString *err = nullptr);

    // 保存工程：将界面配置写入数据库
    bool saveProject(const QVariantMap &configData,
                     QString *err = nullptr);

    // 修改工程信息（工程名、描述），同步更新 updated_at
    bool updateProject(const QString &newName,
                       const QString &newDescription,
                       QString *err = nullptr);

    // 删除工程：DROP DATABASE（危险操作，调用前请二次确认）
    bool deleteProject(const QString &dbName,
                       QString *err = nullptr);

    // 关闭工程
    void closeProject();

    // 工程是否已打开
    bool isProjectOpen() const { return m_projectOpen; }

    // 获取当前工程名（显示名）
    QString currentProjectName() const { return m_projectName; }

    // 获取当前数据库名
    QString currentDbName() const { return m_dbName; }

    // 获取当前工程描述
    QString currentDescription() const { return m_description; }

    // 获取已有工程列表（数据库列表）
    QStringList listProjects(QString *err = nullptr);

    // 创建ui_config表
    bool createUiConfigTable(QString *err = nullptr);

signals:
    void projectOpened(const QString &name);
    void projectClosed();
    void projectSaved();
    void projectError(const QString &msg);

private:
    bool m_projectOpen;
    QString m_projectName;   // 工程显示名
    QString m_dbName;        // 数据库名
    QString m_description;  // 工程描述

    // 加载界面配置
    QVariantMap loadUiConfig(QString *err = nullptr);

    // 清空ui_config表
    bool clearUiConfig(QString *err = nullptr);

    // 插入一条配置记录
    bool insertConfig(const QString &group,
                      const QString &key,
                      const QString &value,
                      const QString &desc = QString(),
                      QString *err = nullptr);

    // 加载工程元信息
    bool loadProjectMeta(QString *err = nullptr);
};

#endif // PROJECTMANAGER_H
