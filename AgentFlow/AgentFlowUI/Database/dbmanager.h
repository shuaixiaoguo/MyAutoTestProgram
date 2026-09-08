#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QString>
#include <QStringList>

struct DbConfig {
    QString connectionName;  // 连接名（可自定义，如 "sql"、"生产环境"）
    QString host;
    int     port;
    QString userName;
    QString password;
    QString dbName;

    DbConfig()
        : connectionName("默认连接")
        , host("127.0.0.1")
        , port(3306)
        , userName("root")
        , password("")
        , dbName("")
    {}
};

class DbManager : public QObject
{
    Q_OBJECT
public:
    static DbManager& instance();

    // 加载/保存数据库设置
    DbConfig loadConfig() const;
    void saveConfig(const DbConfig &cfg);

    // 检查MySQL驱动是否可用
    bool isMysqlDriverAvailable() const;
    // 获取可用驱动列表
    QStringList availableDrivers() const;

    // 检查本地是否有已保存的数据库配置
    bool hasLocalConfig() const;
    // 尝试用本地配置自动连接数据库，返回是否成功
    bool autoConnect(QString *err = nullptr);

    // 连接到 MySQL 服务器（不指定数据库）
    bool connectToServer(const QString &host, int port,
                         const QString &user, const QString &pwd,
                         QString *err = nullptr);

    // 连接到指定数据库
    bool connectToDatabase(const DbConfig &cfg, QString *err = nullptr);

    // 创建新数据库
    bool createDatabase(const QString &dbName, QString *err = nullptr);

    // 检查数据库是否存在
    bool databaseExists(const QString &dbName, bool *ok = nullptr);

    // 执行 SQL
    bool executeSql(const QString &sql, QString *err = nullptr);

    // 是否已连接
    bool isConnected() const;

    // 当前数据库名
    QString currentDatabase() const;

    // 断开连接
    void disconnect();

    // 获取连接对象
    QSqlDatabase database();

private:
    DbManager() = default;
    ~DbManager();
    DbManager(const DbManager&) = delete;
    DbManager& operator=(const DbManager&) = delete;

    static const QString CONNECTION_NAME;
};

#endif // DBMANAGER_H
