#include "paramdatabase.h"
#include "dbmanager.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>

ParamDatabase::ParamDatabase(QObject *parent)
    : QObject(parent)
{
}

QString ParamDatabase::now()
{
    return QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
}

bool ParamDatabase::tableExists(const QString &tableName)
{
    QSqlDatabase db = DbManager::instance().database();
    if (!db.isOpen()) return false;

    QSqlQuery query(db);
    query.prepare(QString("SHOW TABLES LIKE '%1'").arg(tableName));
    return query.exec() && query.next();
}

bool ParamDatabase::nameExistsForType(const QString &tableName, const QString &name, int excludeId)
{
    QSqlDatabase db = DbManager::instance().database();
    if (!db.isOpen()) return false;

    QSqlQuery query(db);
    if (excludeId > 0) {
        query.prepare(QString("SELECT id FROM `%1` WHERE name=? AND id<>?").arg(tableName));
        query.addBindValue(name);
        query.addBindValue(excludeId);
    } else {
        query.prepare(QString("SELECT id FROM `%1` WHERE name=?").arg(tableName));
        query.addBindValue(name);
    }
    return query.exec() && query.next();
}

int ParamDatabase::countByParent(const QString &table, const QString &parentColumn, int parentId)
{
    QSqlDatabase db = DbManager::instance().database();
    if (!db.isOpen()) return 0;

    QSqlQuery query(db);
    query.prepare(QString("SELECT COUNT(*) FROM `%1` WHERE `%2`=?").arg(table).arg(parentColumn));
    query.addBindValue(parentId);
    if (query.exec() && query.next())
        return query.value(0).toInt();
    return 0;
}

bool ParamDatabase::createTables(QString *err)
{
    QSqlDatabase db = DbManager::instance().database();
    if (!db.isOpen()) {
        if (err) *err = "数据库未连接";
        return false;
    }

    QSqlQuery query(db);
    QString sql;

    // 1. 总线类别
    if (!tableExists("bus_category")) {
        sql = "CREATE TABLE bus_category ("
              "id INT AUTO_INCREMENT PRIMARY KEY, "
              "name VARCHAR(64) NOT NULL, "
              "description TEXT, "
              "sort_order INT DEFAULT 0, "
              "created_at DATETIME, "
              "updated_at DATETIME)";
        if (!query.exec(sql)) {
            if (err) *err = "创建bus_category表失败: " + query.lastError().text();
            return false;
        }
    }

    // 2. 总线
    if (!tableExists("bus")) {
        sql = "CREATE TABLE bus ("
              "id INT AUTO_INCREMENT PRIMARY KEY, "
              "category_id INT NOT NULL, "
              "name VARCHAR(64) NOT NULL, "
              "code VARCHAR(64) NOT NULL, "
              "bus_type VARCHAR(32), "
              "protocol_version VARCHAR(32), "
              "description TEXT, "
              "created_at DATETIME, "
              "updated_at DATETIME, "
              "INDEX idx_category (category_id))";
        if (!query.exec(sql)) {
            if (err) *err = "创建bus表失败: " + query.lastError().text();
            return false;
        }
    }

    // 3. 设备
    if (!tableExists("bus_device")) {
        sql = "CREATE TABLE bus_device ("
              "id INT AUTO_INCREMENT PRIMARY KEY, "
              "bus_id INT NOT NULL, "
              "name VARCHAR(64) NOT NULL, "
              "code VARCHAR(64) NOT NULL, "
              "device_type VARCHAR(32), "
              "description TEXT, "
              "created_at DATETIME, "
              "updated_at DATETIME, "
              "INDEX idx_bus (bus_id))";
        if (!query.exec(sql)) {
            if (err) *err = "创建bus_device表失败: " + query.lastError().text();
            return false;
        }
    }

    // 4. 接口
    if (!tableExists("bus_interface")) {
        sql = "CREATE TABLE bus_interface ("
              "id INT AUTO_INCREMENT PRIMARY KEY, "
              "device_id INT NOT NULL, "
              "name VARCHAR(64) NOT NULL, "
              "code VARCHAR(64) NOT NULL, "
              "if_type VARCHAR(32), "
              "adapter_name VARCHAR(64), "
              "adapter_ip VARCHAR(64), "
              "port INT, "
              "baudrate INT, "
              "device_address VARCHAR(64), "
              "description TEXT, "
              "created_at DATETIME, "
              "updated_at DATETIME, "
              "INDEX idx_device (device_id))";
        if (!query.exec(sql)) {
            if (err) *err = "创建bus_interface表失败: " + query.lastError().text();
            return false;
        }
    }

    // 5. 路由
    if (!tableExists("bus_route")) {
        sql = "CREATE TABLE bus_route ("
              "id INT AUTO_INCREMENT PRIMARY KEY, "
              "interface_id INT NOT NULL, "
              "name VARCHAR(64) NOT NULL, "
              "direction TINYINT DEFAULT 1, "
              "process_mode TINYINT DEFAULT 1, "
              "period_ms INT DEFAULT 100, "
              "description TEXT, "
              "created_at DATETIME, "
              "updated_at DATETIME, "
              "INDEX idx_interface (interface_id))";
        if (!query.exec(sql)) {
            if (err) *err = "创建bus_route表失败: " + query.lastError().text();
            return false;
        }
    }

    // 6. 数据块
    if (!tableExists("bus_datablock")) {
        sql = "CREATE TABLE bus_datablock ("
              "id INT AUTO_INCREMENT PRIMARY KEY, "
              "route_id INT NOT NULL, "
              "name VARCHAR(64) NOT NULL, "
              "code VARCHAR(64) NOT NULL, "
              "data_length INT DEFAULT 0, "
              "trigger_mode VARCHAR(32), "
              "description TEXT, "
              "sort_order INT DEFAULT 0, "
              "created_at DATETIME, "
              "updated_at DATETIME, "
              "INDEX idx_route (route_id))";
        if (!query.exec(sql)) {
            if (err) *err = "创建bus_datablock表失败: " + query.lastError().text();
            return false;
        }
    }

    // 7. 参数
    if (!tableExists("bus_parameter")) {
        sql = "CREATE TABLE bus_parameter ("
              "id INT AUTO_INCREMENT PRIMARY KEY, "
              "datablock_id INT NOT NULL, "
              "name VARCHAR(64) NOT NULL, "
              "code VARCHAR(64) NOT NULL, "
              "param_type VARCHAR(16), "
              "start_bit INT DEFAULT 0, "
              "bit_width INT DEFAULT 8, "
              "byte_order TINYINT DEFAULT 1, "
              "unit VARCHAR(16), "
              "`precision` INT DEFAULT 0, "
              "default_value VARCHAR(64), "
              "min_value VARCHAR(64), "
              "max_value VARCHAR(64), "
              "formula TEXT, "
              "enum_options TEXT, "
              "description TEXT, "
              "sort_order INT DEFAULT 0, "
              "created_at DATETIME, "
              "updated_at DATETIME, "
              "INDEX idx_datablock (datablock_id))";
        if (!query.exec(sql)) {
            if (err) *err = "创建bus_parameter表失败: " + query.lastError().text();
            return false;
        }
    }

    return true;
}

// ============ 总线类别 ============

QVector<ParamCategory> ParamDatabase::listCategories(QString *err)
{
    QVector<ParamCategory> result;
    QSqlDatabase db = DbManager::instance().database();
    if (!db.isOpen()) {
        if (err) *err = "数据库未连接";
        return result;
    }

    QSqlQuery query(db);
    query.prepare("SELECT id, name, description, sort_order FROM bus_category "
                  "ORDER BY sort_order, id");
    if (!query.exec()) {
        if (err) *err = query.lastError().text();
        return result;
    }

    while (query.next()) {
        ParamCategory c;
        c.id = query.value(0).toInt();
        c.name = query.value(1).toString();
        c.description = query.value(2).toString();
        c.sortOrder = query.value(3).toInt();
        result.append(c);
    }
    return result;
}

int ParamDatabase::addCategory(const ParamCategory &cat, QString *err)
{
    QSqlDatabase db = DbManager::instance().database();
    if (!db.isOpen()) {
        if (err) *err = "数据库未连接";
        return -1;
    }

    if (!tableExists("bus_category")) {
        if (err) *err = "表 bus_category 不存在，请先创建参数表";
        return -1;
    }

    if (nameExistsForType("bus_category", cat.name)) {
        if (err) *err = "总线类别名称「" + cat.name + "」已存在";
        return -1;
    }

    QSqlQuery query(db);
    query.prepare("INSERT INTO bus_category (name, description, sort_order, created_at, updated_at) "
                  "VALUES (?, ?, ?, ?, ?)");
    query.addBindValue(cat.name);
    query.addBindValue(cat.description);
    query.addBindValue(cat.sortOrder);
    QString t = now();
    query.addBindValue(t);
    query.addBindValue(t);

    if (!query.exec()) {
        if (err) *err = query.lastError().text();
        return -1;
    }
    return query.lastInsertId().toInt();
}

bool ParamDatabase::updateCategory(const ParamCategory &cat, QString *err)
{
    QSqlDatabase db = DbManager::instance().database();
    if (!db.isOpen()) {
        if (err) *err = "数据库未连接";
        return false;
    }

    if (nameExistsForType("bus_category", cat.name, cat.id)) {
        if (err) *err = "总线类别名称「" + cat.name + "」已存在";
        return false;
    }

    QSqlQuery query(db);
    query.prepare("UPDATE bus_category SET name=?, description=?, sort_order=?, updated_at=? "
                  "WHERE id=?");
    query.addBindValue(cat.name);
    query.addBindValue(cat.description);
    query.addBindValue(cat.sortOrder);
    query.addBindValue(now());
    query.addBindValue(cat.id);

    if (!query.exec()) {
        if (err) *err = query.lastError().text();
        return false;
    }
    return true;
}

bool ParamDatabase::deleteCategory(int id, QString *err)
{
    QSqlDatabase db = DbManager::instance().database();
    if (!db.isOpen()) {
        if (err) *err = "数据库未连接";
        return false;
    }

    // 级联删除：先删子节点
    QVector<ParamBus> buses = listBuses(id, err);
    for (const ParamBus &bus : buses)
        deleteBus(bus.id, err);

    QSqlQuery query(db);
    query.prepare("DELETE FROM bus_category WHERE id=?");
    query.addBindValue(id);
    if (!query.exec()) {
        if (err) *err = query.lastError().text();
        return false;
    }
    return true;
}

// ============ 总线 ============

QVector<ParamBus> ParamDatabase::listBuses(int categoryId, QString *err)
{
    QVector<ParamBus> result;
    QSqlDatabase db = DbManager::instance().database();
    if (!db.isOpen()) {
        if (err) *err = "数据库未连接";
        return result;
    }

    QSqlQuery query(db);
    query.prepare("SELECT id, category_id, name, code, bus_type, protocol_version, description "
                  "FROM bus WHERE category_id=? ORDER BY id");
    query.addBindValue(categoryId);
    if (!query.exec()) {
        if (err) *err = query.lastError().text();
        return result;
    }

    while (query.next()) {
        ParamBus b;
        b.id = query.value(0).toInt();
        b.categoryId = query.value(1).toInt();
        b.name = query.value(2).toString();
        b.code = query.value(3).toString();
        b.busType = query.value(4).toString();
        b.protocolVersion = query.value(5).toString();
        b.description = query.value(6).toString();
        result.append(b);
    }
    return result;
}

int ParamDatabase::addBus(const ParamBus &bus, QString *err)
{
    QSqlDatabase db = DbManager::instance().database();
    if (!db.isOpen()) {
        if (err) *err = "数据库未连接";
        return -1;
    }

    if (!tableExists("bus")) {
        if (err) *err = "表 bus 不存在，请先创建参数表";
        return -1;
    }

    if (nameExistsForType("bus", bus.name)) {
        if (err) *err = "总线名称「" + bus.name + "」已存在";
        return -1;
    }

    QSqlQuery query(db);
    query.prepare("INSERT INTO bus (category_id, name, code, bus_type, protocol_version, "
                  "description, created_at, updated_at) "
                  "VALUES (?, ?, ?, ?, ?, ?, ?, ?)");
    query.addBindValue(bus.categoryId);
    query.addBindValue(bus.name);
    query.addBindValue(bus.code);
    query.addBindValue(bus.busType);
    query.addBindValue(bus.protocolVersion);
    query.addBindValue(bus.description);
    QString t = now();
    query.addBindValue(t);
    query.addBindValue(t);

    if (!query.exec()) {
        if (err) *err = query.lastError().text();
        return -1;
    }
    return query.lastInsertId().toInt();
}

bool ParamDatabase::updateBus(const ParamBus &bus, QString *err)
{
    QSqlDatabase db = DbManager::instance().database();
    if (!db.isOpen()) {
        if (err) *err = "数据库未连接";
        return false;
    }

    QSqlQuery query(db);
    query.prepare("UPDATE bus SET name=?, code=?, bus_type=?, protocol_version=?, "
                  "description=?, updated_at=? WHERE id=?");
    query.addBindValue(bus.name);
    query.addBindValue(bus.code);
    query.addBindValue(bus.busType);
    query.addBindValue(bus.protocolVersion);
    query.addBindValue(bus.description);
    query.addBindValue(now());
    query.addBindValue(bus.id);

    if (!query.exec()) {
        if (err) *err = query.lastError().text();
        return false;
    }
    return true;
}

bool ParamDatabase::deleteBus(int id, QString *err)
{
    QSqlDatabase db = DbManager::instance().database();
    if (!db.isOpen()) {
        if (err) *err = "数据库未连接";
        return false;
    }

    // 级联删除：先删子节点
    QVector<ParamDevice> devs = listDevices(id, err);
    for (const ParamDevice &dev : devs)
        deleteDevice(dev.id, err);

    QSqlQuery query(db);
    query.prepare("DELETE FROM bus WHERE id=?");
    query.addBindValue(id);
    if (!query.exec()) {
        if (err) *err = query.lastError().text();
        return false;
    }
    return true;
}

// ============ 设备 ============

QVector<ParamDevice> ParamDatabase::listDevices(int busId, QString *err)
{
    QVector<ParamDevice> result;
    QSqlDatabase db = DbManager::instance().database();
    if (!db.isOpen()) {
        if (err) *err = "数据库未连接";
        return result;
    }

    QSqlQuery query(db);
    query.prepare("SELECT id, bus_id, name, code, device_type, description "
                  "FROM bus_device WHERE bus_id=? ORDER BY id");
    query.addBindValue(busId);
    if (!query.exec()) {
        if (err) *err = query.lastError().text();
        return result;
    }

    while (query.next()) {
        ParamDevice d;
        d.id = query.value(0).toInt();
        d.busId = query.value(1).toInt();
        d.name = query.value(2).toString();
        d.code = query.value(3).toString();
        d.deviceType = query.value(4).toString();
        d.description = query.value(5).toString();
        result.append(d);
    }
    return result;
}

int ParamDatabase::addDevice(const ParamDevice &dev, QString *err)
{
    QSqlDatabase db = DbManager::instance().database();
    if (!db.isOpen()) {
        if (err) *err = "数据库未连接";
        return -1;
    }

    if (!tableExists("bus_device")) {
        if (err) *err = "表 bus_device 不存在，请先创建参数表";
        return -1;
    }

    if (nameExistsForType("bus_device", dev.name)) {
        if (err) *err = "设备名称「" + dev.name + "」已存在";
        return -1;
    }

    QSqlQuery query(db);
    query.prepare("INSERT INTO bus_device (bus_id, name, code, device_type, "
                  "description, created_at, updated_at) "
                  "VALUES (?, ?, ?, ?, ?, ?, ?)");
    query.addBindValue(dev.busId);
    query.addBindValue(dev.name);
    query.addBindValue(dev.code);
    query.addBindValue(dev.deviceType);
    query.addBindValue(dev.description);
    QString t = now();
    query.addBindValue(t);
    query.addBindValue(t);

    if (!query.exec()) {
        if (err) *err = query.lastError().text();
        return -1;
    }
    return query.lastInsertId().toInt();
}

bool ParamDatabase::updateDevice(const ParamDevice &dev, QString *err)
{
    QSqlDatabase db = DbManager::instance().database();
    if (!db.isOpen()) {
        if (err) *err = "数据库未连接";
        return false;
    }

    if (nameExistsForType("bus_device", dev.name, dev.id)) {
        if (err) *err = "设备名称「" + dev.name + "」已存在";
        return false;
    }

    QSqlQuery query(db);
    query.prepare("UPDATE bus_device SET name=?, code=?, device_type=?, "
                  "description=?, updated_at=? WHERE id=?");
    query.addBindValue(dev.name);
    query.addBindValue(dev.code);
    query.addBindValue(dev.deviceType);
    query.addBindValue(dev.description);
    query.addBindValue(now());
    query.addBindValue(dev.id);

    if (!query.exec()) {
        if (err) *err = query.lastError().text();
        return false;
    }
    return true;
}

bool ParamDatabase::deleteDevice(int id, QString *err)
{
    QSqlDatabase db = DbManager::instance().database();
    if (!db.isOpen()) {
        if (err) *err = "数据库未连接";
        return false;
    }

    // 级联删除：先删子节点
    QVector<ParamInterface> ifaces = listInterfaces(id, err);
    for (const ParamInterface &iface : ifaces)
        deleteInterface(iface.id, err);

    QSqlQuery query(db);
    query.prepare("DELETE FROM bus_device WHERE id=?");
    query.addBindValue(id);
    if (!query.exec()) {
        if (err) *err = query.lastError().text();
        return false;
    }
    return true;
}

// ============ 接口 ============

QVector<ParamInterface> ParamDatabase::listInterfaces(int deviceId, QString *err)
{
    QVector<ParamInterface> result;
    QSqlDatabase db = DbManager::instance().database();
    if (!db.isOpen()) {
        if (err) *err = "数据库未连接";
        return result;
    }

    QSqlQuery query(db);
    query.prepare("SELECT id, device_id, name, code, if_type, adapter_name, "
                  "adapter_ip, port, baudrate, device_address, description "
                  "FROM bus_interface WHERE device_id=? ORDER BY id");
    query.addBindValue(deviceId);
    if (!query.exec()) {
        if (err) *err = query.lastError().text();
        return result;
    }

    while (query.next()) {
        ParamInterface i;
        i.id = query.value(0).toInt();
        i.deviceId = query.value(1).toInt();
        i.name = query.value(2).toString();
        i.code = query.value(3).toString();
        i.ifType = query.value(4).toString();
        i.adapterName = query.value(5).toString();
        i.adapterIp = query.value(6).toString();
        i.port = query.value(7).toInt();
        i.baudrate = query.value(8).toInt();
        i.deviceAddress = query.value(9).toString();
        i.description = query.value(10).toString();
        result.append(i);
    }
    return result;
}

int ParamDatabase::addInterface(const ParamInterface &iface, QString *err)
{
    QSqlDatabase db = DbManager::instance().database();
    if (!db.isOpen()) {
        if (err) *err = "数据库未连接";
        return -1;
    }

    if (!tableExists("bus_interface")) {
        if (err) *err = "表 bus_interface 不存在，请先创建参数表";
        return -1;
    }

    if (nameExistsForType("bus_interface", iface.name)) {
        if (err) *err = "接口名称「" + iface.name + "」已存在";
        return -1;
    }

    QSqlQuery query(db);
    query.prepare("INSERT INTO bus_interface (device_id, name, code, if_type, adapter_name, "
                  "adapter_ip, port, baudrate, device_address, description, created_at, updated_at) "
                  "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
    query.addBindValue(iface.deviceId);
    query.addBindValue(iface.name);
    query.addBindValue(iface.code);
    query.addBindValue(iface.ifType);
    query.addBindValue(iface.adapterName);
    query.addBindValue(iface.adapterIp);
    query.addBindValue(iface.port);
    query.addBindValue(iface.baudrate);
    query.addBindValue(iface.deviceAddress);
    query.addBindValue(iface.description);
    QString t = now();
    query.addBindValue(t);
    query.addBindValue(t);

    if (!query.exec()) {
        if (err) *err = query.lastError().text();
        return -1;
    }
    return query.lastInsertId().toInt();
}

bool ParamDatabase::updateInterface(const ParamInterface &iface, QString *err)
{
    QSqlDatabase db = DbManager::instance().database();
    if (!db.isOpen()) {
        if (err) *err = "数据库未连接";
        return false;
    }

    QSqlQuery query(db);
    query.prepare("UPDATE bus_interface SET name=?, code=?, if_type=?, adapter_name=?, "
                  "adapter_ip=?, port=?, baudrate=?, device_address=?, "
                  "description=?, updated_at=? WHERE id=?");
    query.addBindValue(iface.name);
    query.addBindValue(iface.code);
    query.addBindValue(iface.ifType);
    query.addBindValue(iface.adapterName);
    query.addBindValue(iface.adapterIp);
    query.addBindValue(iface.port);
    query.addBindValue(iface.baudrate);
    query.addBindValue(iface.deviceAddress);
    query.addBindValue(iface.description);
    query.addBindValue(now());
    query.addBindValue(iface.id);

    if (!query.exec()) {
        if (err) *err = query.lastError().text();
        return false;
    }
    return true;
}

bool ParamDatabase::deleteInterface(int id, QString *err)
{
    QSqlDatabase db = DbManager::instance().database();
    if (!db.isOpen()) {
        if (err) *err = "数据库未连接";
        return false;
    }

    // 级联删除：先删子节点
    QVector<ParamRoute> routes = listRoutes(id, err);
    for (const ParamRoute &route : routes)
        deleteRoute(route.id, err);

    QSqlQuery query(db);
    query.prepare("DELETE FROM bus_interface WHERE id=?");
    query.addBindValue(id);
    if (!query.exec()) {
        if (err) *err = query.lastError().text();
        return false;
    }
    return true;
}

// ============ 路由 ============

QVector<ParamRoute> ParamDatabase::listRoutes(int interfaceId, QString *err)
{
    QVector<ParamRoute> result;
    QSqlDatabase db = DbManager::instance().database();
    if (!db.isOpen()) {
        if (err) *err = "数据库未连接";
        return result;
    }

    QSqlQuery query(db);
    query.prepare("SELECT id, interface_id, name, direction, process_mode, period_ms, description "
                  "FROM bus_route WHERE interface_id=? ORDER BY id");
    query.addBindValue(interfaceId);
    if (!query.exec()) {
        if (err) *err = query.lastError().text();
        return result;
    }

    while (query.next()) {
        ParamRoute r;
        r.id = query.value(0).toInt();
        r.interfaceId = query.value(1).toInt();
        r.name = query.value(2).toString();
        r.direction = query.value(3).toInt();
        r.processMode = query.value(4).toInt();
        r.periodMs = query.value(5).toInt();
        r.description = query.value(6).toString();
        result.append(r);
    }
    return result;
}

int ParamDatabase::addRoute(const ParamRoute &route, QString *err)
{
    QSqlDatabase db = DbManager::instance().database();
    if (!db.isOpen()) {
        if (err) *err = "数据库未连接";
        return -1;
    }

    if (!tableExists("bus_route")) {
        if (err) *err = "表 bus_route 不存在，请先创建参数表";
        return -1;
    }

    if (nameExistsForType("bus_route", route.name)) {
        if (err) *err = "路由名称「" + route.name + "」已存在";
        return -1;
    }

    QSqlQuery query(db);
    query.prepare("INSERT INTO bus_route (interface_id, name, direction, process_mode, "
                  "period_ms, description, created_at, updated_at) "
                  "VALUES (?, ?, ?, ?, ?, ?, ?, ?)");
    query.addBindValue(route.interfaceId);
    query.addBindValue(route.name);
    query.addBindValue(route.direction);
    query.addBindValue(route.processMode);
    query.addBindValue(route.periodMs);
    query.addBindValue(route.description);
    QString t = now();
    query.addBindValue(t);
    query.addBindValue(t);

    if (!query.exec()) {
        if (err) *err = query.lastError().text();
        return -1;
    }
    return query.lastInsertId().toInt();
}

bool ParamDatabase::updateRoute(const ParamRoute &route, QString *err)
{
    QSqlDatabase db = DbManager::instance().database();
    if (!db.isOpen()) {
        if (err) *err = "数据库未连接";
        return false;
    }

    if (nameExistsForType("bus_route", route.name, route.id)) {
        if (err) *err = "路由名称「" + route.name + "」已存在";
        return false;
    }

    QSqlQuery query(db);
    query.prepare("UPDATE bus_route SET name=?, direction=?, process_mode=?, "
                  "period_ms=?, description=?, updated_at=? WHERE id=?");
    query.addBindValue(route.name);
    query.addBindValue(route.direction);
    query.addBindValue(route.processMode);
    query.addBindValue(route.periodMs);
    query.addBindValue(route.description);
    query.addBindValue(now());
    query.addBindValue(route.id);

    if (!query.exec()) {
        if (err) *err = query.lastError().text();
        return false;
    }
    return true;
}

bool ParamDatabase::deleteRoute(int id, QString *err)
{
    QSqlDatabase db = DbManager::instance().database();
    if (!db.isOpen()) {
        if (err) *err = "数据库未连接";
        return false;
    }

    // 级联删除：先删子节点
    QVector<ParamDatablock> blocks = listDatablocks(id, err);
    for (const ParamDatablock &blk : blocks)
        deleteDatablock(blk.id, err);

    QSqlQuery query(db);
    query.prepare("DELETE FROM bus_route WHERE id=?");
    query.addBindValue(id);
    if (!query.exec()) {
        if (err) *err = query.lastError().text();
        return false;
    }
    return true;
}

// ============ 数据块 ============

QVector<ParamDatablock> ParamDatabase::listDatablocks(int routeId, QString *err)
{
    QVector<ParamDatablock> result;
    QSqlDatabase db = DbManager::instance().database();
    if (!db.isOpen()) {
        if (err) *err = "数据库未连接";
        return result;
    }

    QSqlQuery query(db);
    query.prepare("SELECT id, route_id, name, code, data_length, trigger_mode, "
                  "description, sort_order FROM bus_datablock WHERE route_id=? "
                  "ORDER BY sort_order, id");
    query.addBindValue(routeId);
    if (!query.exec()) {
        if (err) *err = query.lastError().text();
        return result;
    }

    while (query.next()) {
        ParamDatablock d;
        d.id = query.value(0).toInt();
        d.routeId = query.value(1).toInt();
        d.name = query.value(2).toString();
        d.code = query.value(3).toString();
        d.dataLength = query.value(4).toInt();
        d.triggerMode = query.value(5).toString();
        d.description = query.value(6).toString();
        d.sortOrder = query.value(7).toInt();
        result.append(d);
    }
    return result;
}

int ParamDatabase::addDatablock(const ParamDatablock &blk, QString *err)
{
    QSqlDatabase db = DbManager::instance().database();
    if (!db.isOpen()) {
        if (err) *err = "数据库未连接";
        return -1;
    }

    if (nameExistsForType("bus_datablock", blk.name)) {
        if (err) *err = "数据块名称「" + blk.name + "」已存在";
        return -1;
    }

    QSqlQuery query(db);
    query.prepare("INSERT INTO bus_datablock (route_id, name, code, data_length, "
                  "trigger_mode, description, sort_order, created_at, updated_at) "
                  "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)");
    query.addBindValue(blk.routeId);
    query.addBindValue(blk.name);
    query.addBindValue(blk.code);
    query.addBindValue(blk.dataLength);
    query.addBindValue(blk.triggerMode);
    query.addBindValue(blk.description);
    query.addBindValue(blk.sortOrder);
    QString t = now();
    query.addBindValue(t);
    query.addBindValue(t);

    if (!query.exec()) {
        if (err) *err = query.lastError().text();
        return -1;
    }
    return query.lastInsertId().toInt();
}

bool ParamDatabase::updateDatablock(const ParamDatablock &blk, QString *err)
{
    QSqlDatabase db = DbManager::instance().database();
    if (!db.isOpen()) {
        if (err) *err = "数据库未连接";
        return false;
    }

    if (nameExistsForType("bus_datablock", blk.name, blk.id)) {
        if (err) *err = "数据块名称「" + blk.name + "」已存在";
        return false;
    }

    QSqlQuery query(db);
    query.prepare("UPDATE bus_datablock SET name=?, code=?, data_length=?, "
                  "trigger_mode=?, description=?, sort_order=?, updated_at=? WHERE id=?");
    query.addBindValue(blk.name);
    query.addBindValue(blk.code);
    query.addBindValue(blk.dataLength);
    query.addBindValue(blk.triggerMode);
    query.addBindValue(blk.description);
    query.addBindValue(blk.sortOrder);
    query.addBindValue(now());
    query.addBindValue(blk.id);

    if (!query.exec()) {
        if (err) *err = query.lastError().text();
        return false;
    }
    return true;
}

bool ParamDatabase::deleteDatablock(int id, QString *err)
{
    QSqlDatabase db = DbManager::instance().database();
    if (!db.isOpen()) {
        if (err) *err = "数据库未连接";
        return false;
    }

    // 级联删除：先删子节点
    QVector<ParamParameter> params = listParameters(id, err);
    for (const ParamParameter &param : params)
        deleteParameter(param.id, err);

    QSqlQuery query(db);
    query.prepare("DELETE FROM bus_datablock WHERE id=?");
    query.addBindValue(id);
    if (!query.exec()) {
        if (err) *err = query.lastError().text();
        return false;
    }
    return true;
}

// ============ 参数 ============

QVector<ParamParameter> ParamDatabase::listParameters(int datablockId, QString *err)
{
    QVector<ParamParameter> result;
    QSqlDatabase db = DbManager::instance().database();
    if (!db.isOpen()) {
        if (err) *err = "数据库未连接";
        return result;
    }

    QSqlQuery query(db);
    query.prepare("SELECT id, datablock_id, name, code, param_type, start_bit, "
                  "bit_width, byte_order, unit, `precision`, default_value, min_value, "
                  "max_value, formula, enum_options, description, sort_order "
                  "FROM bus_parameter WHERE datablock_id=? "
                  "ORDER BY sort_order, id");
    query.addBindValue(datablockId);
    if (!query.exec()) {
        if (err) *err = query.lastError().text();
        return result;
    }

    while (query.next()) {
        ParamParameter p;
        p.id = query.value(0).toInt();
        p.datablockId = query.value(1).toInt();
        p.name = query.value(2).toString();
        p.code = query.value(3).toString();
        p.paramType = query.value(4).toString();
        p.startBit = query.value(5).toInt();
        p.bitWidth = query.value(6).toInt();
        p.byteOrder = query.value(7).toInt();
        p.unit = query.value(8).toString();
        p.precision = query.value(9).toInt();
        p.defaultValue = query.value(10).toString();
        p.minValue = query.value(11).toString();
        p.maxValue = query.value(12).toString();
        p.formula = query.value(13).toString();
        p.enumOptions = query.value(14).toString();
        p.description = query.value(15).toString();
        p.sortOrder = query.value(16).toInt();
        result.append(p);
    }
    return result;
}

int ParamDatabase::addParameter(const ParamParameter &param, QString *err)
{
    QSqlDatabase db = DbManager::instance().database();
    if (!db.isOpen()) {
        if (err) *err = "数据库未连接";
        return -1;
    }

    if (!tableExists("bus_parameter")) {
        if (err) *err = "表 bus_parameter 不存在，请先创建参数表";
        return -1;
    }

    if (nameExistsForType("bus_parameter", param.name)) {
        if (err) *err = "参数名称「" + param.name + "」已存在";
        return -1;
    }

    QSqlQuery query(db);
    query.prepare("INSERT INTO bus_parameter (datablock_id, name, code, param_type, "
                  "start_bit, bit_width, byte_order, unit, `precision`, default_value, "
                  "min_value, max_value, formula, enum_options, description, "
                  "sort_order, created_at, updated_at) "
                  "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
    query.addBindValue(param.datablockId);
    query.addBindValue(param.name);
    query.addBindValue(param.code);
    query.addBindValue(param.paramType);
    query.addBindValue(param.startBit);
    query.addBindValue(param.bitWidth);
    query.addBindValue(param.byteOrder);
    query.addBindValue(param.unit);
    query.addBindValue(param.precision);
    query.addBindValue(param.defaultValue);
    query.addBindValue(param.minValue);
    query.addBindValue(param.maxValue);
    query.addBindValue(param.formula);
    query.addBindValue(param.enumOptions);
    query.addBindValue(param.description);
    query.addBindValue(param.sortOrder);
    QString t = now();
    query.addBindValue(t);
    query.addBindValue(t);

    if (!query.exec()) {
        if (err) *err = query.lastError().text();
        return -1;
    }
    return query.lastInsertId().toInt();
}

bool ParamDatabase::updateParameter(const ParamParameter &param, QString *err)
{
    QSqlDatabase db = DbManager::instance().database();
    if (!db.isOpen()) {
        if (err) *err = "数据库未连接";
        return false;
    }

    if (nameExistsForType("bus_parameter", param.name, param.id)) {
        if (err) *err = "参数名称「" + param.name + "」已存在";
        return false;
    }

    QSqlQuery query(db);
    query.prepare("UPDATE bus_parameter SET name=?, code=?, param_type=?, "
                  "start_bit=?, bit_width=?, byte_order=?, unit=?, `precision`=?, "
                  "default_value=?, min_value=?, max_value=?, formula=?, "
                  "enum_options=?, description=?, sort_order=?, updated_at=? "
                  "WHERE id=?");
    query.addBindValue(param.name);
    query.addBindValue(param.code);
    query.addBindValue(param.paramType);
    query.addBindValue(param.startBit);
    query.addBindValue(param.bitWidth);
    query.addBindValue(param.byteOrder);
    query.addBindValue(param.unit);
    query.addBindValue(param.precision);
    query.addBindValue(param.defaultValue);
    query.addBindValue(param.minValue);
    query.addBindValue(param.maxValue);
    query.addBindValue(param.formula);
    query.addBindValue(param.enumOptions);
    query.addBindValue(param.description);
    query.addBindValue(param.sortOrder);
    query.addBindValue(now());
    query.addBindValue(param.id);

    if (!query.exec()) {
        if (err) *err = query.lastError().text();
        return false;
    }
    return true;
}

bool ParamDatabase::deleteParameter(int id, QString *err)
{
    QSqlDatabase db = DbManager::instance().database();
    if (!db.isOpen()) {
        if (err) *err = "数据库未连接";
        return false;
    }

    QSqlQuery query(db);
    query.prepare("DELETE FROM bus_parameter WHERE id=?");
    query.addBindValue(id);
    if (!query.exec()) {
        if (err) *err = query.lastError().text();
        return false;
    }
    return true;
}
