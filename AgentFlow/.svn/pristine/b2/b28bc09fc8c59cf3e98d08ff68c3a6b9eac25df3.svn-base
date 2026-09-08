#ifndef PARAMDATABASE_H
#define PARAMDATABASE_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QVariantMap>

// 参数结构体
struct ParamCategory {
    int id = 0;
    QString name;
    QString description;
    int sortOrder = 0;
};

struct ParamBus {
    int id = 0;
    int categoryId = 0;
    QString name;
    QString code;
    QString busType;
    QString protocolVersion;
    QString description;
};

struct ParamDevice {
    int id = 0;
    int busId = 0;
    QString name;
    QString code;
    QString deviceType;
    QString description;
};

struct ParamInterface {
    int id = 0;
    int deviceId = 0;
    QString name;
    QString code;
    QString ifType;
    QString adapterName;
    QString adapterIp;
    int port = 0;
    int baudrate = 0;
    QString deviceAddress;
    QString description;
};

struct ParamRoute {
    int id = 0;
    int interfaceId = 0;
    QString name;
    int direction = 1;    // 1:发送 2:接收 3:双向
    int processMode = 1;  // 1:周期 2:事件 3:DMA 4:中断
    int periodMs = 100;
    QString description;
};

struct ParamDatablock {
    int id = 0;
    int routeId = 0;
    QString name;
    QString code;
    int dataLength = 0;
    QString triggerMode;
    QString description;
    int sortOrder = 0;
};

struct ParamParameter {
    int id = 0;
    int datablockId = 0;
    QString name;
    QString code;
    QString paramType;  // uint8/uint16/uint32/int8/int16/int32/float/double/bool/enum
    int startBit = 0;
    int bitWidth = 8;
    int byteOrder = 1;  // 1:大端 2:小端
    QString unit;
    int precision = 0;
    QString defaultValue;
    QString minValue;
    QString maxValue;
    QString formula;
    QString enumOptions;  // JSON数组
    QString description;
    int sortOrder = 0;
};

class ParamDatabase : public QObject
{
    Q_OBJECT
public:
    explicit ParamDatabase(QObject *parent = nullptr);

    // --- 建表 ---
    bool createTables(QString *err = nullptr);
    bool tableExists(const QString &tableName);
    bool nameExistsForType(const QString &tableName, const QString &name, int excludeId = 0);
    int countByParent(const QString &table, const QString &parentColumn, int parentId);

    // --- 总线类别 ---
    QVector<ParamCategory> listCategories(QString *err = nullptr);
    int addCategory(const ParamCategory &cat, QString *err = nullptr);
    bool updateCategory(const ParamCategory &cat, QString *err = nullptr);
    bool deleteCategory(int id, QString *err = nullptr);

    // --- 总线 ---
    QVector<ParamBus> listBuses(int categoryId, QString *err = nullptr);
    int addBus(const ParamBus &bus, QString *err = nullptr);
    bool updateBus(const ParamBus &bus, QString *err = nullptr);
    bool deleteBus(int id, QString *err = nullptr);

    // --- 设备 ---
    QVector<ParamDevice> listDevices(int busId, QString *err = nullptr);
    int addDevice(const ParamDevice &dev, QString *err = nullptr);
    bool updateDevice(const ParamDevice &dev, QString *err = nullptr);
    bool deleteDevice(int id, QString *err = nullptr);

    // --- 接口 ---
    QVector<ParamInterface> listInterfaces(int deviceId, QString *err = nullptr);
    int addInterface(const ParamInterface &iface, QString *err = nullptr);
    bool updateInterface(const ParamInterface &iface, QString *err = nullptr);
    bool deleteInterface(int id, QString *err = nullptr);

    // --- 路由 ---
    QVector<ParamRoute> listRoutes(int interfaceId, QString *err = nullptr);
    int addRoute(const ParamRoute &route, QString *err = nullptr);
    bool updateRoute(const ParamRoute &route, QString *err = nullptr);
    bool deleteRoute(int id, QString *err = nullptr);

    // --- 数据块 ---
    QVector<ParamDatablock> listDatablocks(int routeId, QString *err = nullptr);
    int addDatablock(const ParamDatablock &blk, QString *err = nullptr);
    bool updateDatablock(const ParamDatablock &blk, QString *err = nullptr);
    bool deleteDatablock(int id, QString *err = nullptr);

    // --- 参数 ---
    QVector<ParamParameter> listParameters(int datablockId, QString *err = nullptr);
    int addParameter(const ParamParameter &param, QString *err = nullptr);
    bool updateParameter(const ParamParameter &param, QString *err = nullptr);
    bool deleteParameter(int id, QString *err = nullptr);

    // --- 工具 ---
    static QString now(); // 当前时间字符串
};

#endif // PARAMDATABASE_H
