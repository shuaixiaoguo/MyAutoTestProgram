#include "nodepropertypanel.h"
#include "paramdatabase.h"
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QComboBox>
#include <QHBoxLayout>
#include <QMessageBox>

NodePropertyPanel::NodePropertyPanel(QWidget *parent)
    : QWidget(parent)
    , m_table(nullptr)
    , m_btnSave(nullptr)
    , m_nodeId(0)
    , m_nodeType(NodeRoot)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(4, 4, 4, 4);
    layout->setSpacing(4);

    m_table = new QTableWidget(this);
    m_table->setColumnCount(2);
    m_table->setHorizontalHeaderLabels({"属性", "值"});
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_table->verticalHeader()->setVisible(false);
    m_table->setAlternatingRowColors(true);
    m_table->setSelectionBehavior(QAbstractItemView::SelectItems);
    m_table->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);

    m_btnSave = new QPushButton("保存修改", this);

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addStretch();
    btnLayout->addWidget(m_btnSave);

    layout->addWidget(m_table, 1);
    layout->addLayout(btnLayout);

    connect(m_btnSave, &QPushButton::clicked, this, &NodePropertyPanel::onSaveClicked);
}

void NodePropertyPanel::addRow(const QString &prop, const QString &value, bool editable)
{
    int row = m_table->rowCount();
    m_table->insertRow(row);

    QTableWidgetItem *propItem = new QTableWidgetItem(prop);
    propItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    propItem->setBackground(QColor(240, 240, 240));

    QTableWidgetItem *valItem = new QTableWidgetItem(value);
    if (!editable)
        valItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);

    m_table->setItem(row, 0, propItem);
    m_table->setItem(row, 1, valItem);
}

void NodePropertyPanel::addRowWithCombo(const QString &prop, const QStringList &options, int currentIndex)
{
    int row = m_table->rowCount();
    m_table->insertRow(row);

    QTableWidgetItem *propItem = new QTableWidgetItem(prop);
    propItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    propItem->setBackground(QColor(240, 240, 240));
    m_table->setItem(row, 0, propItem);

    QComboBox *combo = new QComboBox();
    combo->addItems(options);
    combo->setCurrentIndex(currentIndex);
    combo->setStyleSheet("QComboBox { border: none; }");
    m_table->setCellWidget(row, 1, combo);
}

void NodePropertyPanel::clearForm()
{
    m_table->setRowCount(0);
    m_nodeId = 0;
    m_nodeType = NodeRoot;
}

void NodePropertyPanel::loadNode(const TreeNode &node)
{
    clearForm();
    m_nodeId = node.id;
    m_nodeType = node.type;

    // 第一行：节点类型（只读）
    static const char *typeNames[] = {
        "根节点", "总线类别", "总线", "设备", "接口", "路由", "数据块", "参数"
    };
    int typeIdx = static_cast<int>(node.type);
    if (typeIdx >= 0 && typeIdx <= 7)
        addRow("节点类型", typeNames[typeIdx], false);
    else
        addRow("节点类型", "未知", false);

    switch (node.type) {
    case NodeCategory:  setupCategory(node.id); break;
    case NodeBus:       setupBus(node.id); break;
    case NodeDevice:    setupDevice(node.id); break;
    case NodeInterface: setupInterface(node.id); break;
    case NodeRoute:     setupRoute(node.id); break;
    case NodeDatablock: setupDatablock(node.id); break;
    default: break;
    }
}

// ---- 总线类别 ----
void NodePropertyPanel::setupCategory(int id)
{
    ParamDatabase db;
    QString err;
    QVector<ParamCategory> list = db.listCategories(&err);
    for (const ParamCategory &cat : list) {
        if (cat.id != id) continue;
        addRow("名称", cat.name);
        addRow("描述", cat.description);
        addRow("排序", QString::number(cat.sortOrder));
        break;
    }
}

// ---- 总线 ----
void NodePropertyPanel::setupBus(int id)
{
    ParamDatabase db;
    QString err;

    // 需要先找到 categoryId，用 listBuses 逐个找
    QVector<ParamCategory> cats = db.listCategories(&err);
    for (const ParamCategory &cat : cats) {
        QVector<ParamBus> buses = db.listBuses(cat.id, &err);
        for (const ParamBus &bus : buses) {
            if (bus.id != id) continue;
            addRow("名称", bus.name);
            addRow("描述", bus.description);
            addRow("总线类型", bus.busType);
            addRow("协议版本", bus.protocolVersion);
            addRow("最大数据块长度", "1024");
            return;
        }
    }
}

// ---- 设备 ----
void NodePropertyPanel::setupDevice(int id)
{
    ParamDatabase db;
    QString err;

    QVector<ParamCategory> cats = db.listCategories(&err);
    for (const ParamCategory &cat : cats) {
        QVector<ParamBus> buses = db.listBuses(cat.id, &err);
        for (const ParamBus &bus : buses) {
            QVector<ParamDevice> devs = db.listDevices(bus.id, &err);
            for (const ParamDevice &dev : devs) {
                if (dev.id != id) continue;
                addRow("名称", dev.name);
                addRow("描述", dev.description);
                addRow("设备类型", dev.deviceType);
                addRow("所属总线", bus.name, false);
                return;
            }
        }
    }
}

// ---- 接口 ----
void NodePropertyPanel::setupInterface(int id)
{
    ParamDatabase db;
    QString err;

    QVector<ParamCategory> cats = db.listCategories(&err);
    for (const ParamCategory &cat : cats) {
        QVector<ParamBus> buses = db.listBuses(cat.id, &err);
        for (const ParamBus &bus : buses) {
            QVector<ParamDevice> devs = db.listDevices(bus.id, &err);
            for (const ParamDevice &dev : devs) {
                QVector<ParamInterface> ifaces = db.listInterfaces(dev.id, &err);
                for (const ParamInterface &iface : ifaces) {
                    if (iface.id != id) continue;
                    addRow("名称", iface.name);
                    addRow("接口描述", iface.description);
                    addRow("适配器", iface.adapterName);
                    addRow("配置", "ProgId=Adapter." + iface.adapterName);
                    addRow("宿主IP", iface.adapterIp);
                    addRow("硬件地址", iface.deviceAddress);
                    addRowWithCombo("是否自动启动", {"否", "是"}, 0);
                    addRow("所属总线", bus.name, false);
                    return;
                }
            }
        }
    }
}

// ---- 路由 ----
void NodePropertyPanel::setupRoute(int id)
{
    ParamDatabase db;
    QString err;

    QVector<ParamCategory> cats = db.listCategories(&err);
    for (const ParamCategory &cat : cats) {
        QVector<ParamBus> buses = db.listBuses(cat.id, &err);
        for (const ParamBus &bus : buses) {
            QVector<ParamDevice> devs = db.listDevices(bus.id, &err);
            for (const ParamDevice &dev : devs) {
                QVector<ParamInterface> ifaces = db.listInterfaces(dev.id, &err);
                for (const ParamInterface &iface : ifaces) {
                    QVector<ParamRoute> routes = db.listRoutes(iface.id, &err);
                    for (const ParamRoute &route : routes) {
                        if (route.id != id) continue;
                        addRow("名称", route.name);
                        addRowWithCombo("方向", {"发送", "接收", "双向"}, route.direction - 1);
                        addRowWithCombo("处理方式", {"周期", "事件", "DMA", "中断"}, route.processMode - 1);
                        addRow("周期(ms)", QString::number(route.periodMs));
                        addRow("描述", route.description);
                        return;
                    }
                }
            }
        }
    }
}

// ---- 数据块 ----
void NodePropertyPanel::setupDatablock(int id)
{
    ParamDatabase db;
    QString err;

    QVector<ParamCategory> cats = db.listCategories(&err);
    for (const ParamCategory &cat : cats) {
        QVector<ParamBus> buses = db.listBuses(cat.id, &err);
        for (const ParamBus &bus : buses) {
            QVector<ParamDevice> devs = db.listDevices(bus.id, &err);
            for (const ParamDevice &dev : devs) {
                QVector<ParamInterface> ifaces = db.listInterfaces(dev.id, &err);
                for (const ParamInterface &iface : ifaces) {
                    QVector<ParamRoute> routes = db.listRoutes(iface.id, &err);
                    for (const ParamRoute &route : routes) {
                        QVector<ParamDatablock> blocks = db.listDatablocks(route.id, &err);
                        for (const ParamDatablock &blk : blocks) {
                            if (blk.id != id) continue;
                            addRow("块名称", blk.name);
                            addRowWithCombo("是否指定块长度", {"否", "是"}, blk.dataLength > 0 ? 1 : 0);
                            if (blk.dataLength > 0)
                                addRow("长度(字节)", QString::number(blk.dataLength));
                            else
                                addRow("长度(字节)", "0 (自动计算)");
                            addRow("触发模式", blk.triggerMode);
                            addRow("描述", blk.description);
                            return;
                        }
                    }
                }
            }
        }
    }
}

// ---- 保存 ----
void NodePropertyPanel::onSaveClicked()
{
    ParamDatabase db;
    QString err;

    switch (m_nodeType) {
    case NodeCategory: {
        ParamCategory cat;
        cat.id = m_nodeId;
        cat.name = m_table->item(0, 1)->text();
        cat.description = m_table->item(1, 1)->text();
        cat.sortOrder = m_table->item(2, 1)->text().toInt();
        if (!db.updateCategory(cat, &err))
            QMessageBox::critical(this, "错误", "保存失败: " + err);
        break;
    }
    case NodeBus: {
        ParamBus bus;
        bus.id = m_nodeId;
        bus.name = m_table->item(0, 1)->text();
        bus.description = m_table->item(1, 1)->text();
        bus.busType = m_table->item(2, 1)->text();
        bus.protocolVersion = m_table->item(3, 1)->text();
        if (!db.updateBus(bus, &err))
            QMessageBox::critical(this, "错误", "保存失败: " + err);
        break;
    }
    case NodeDevice: {
        ParamDevice dev;
        dev.id = m_nodeId;
        dev.name = m_table->item(0, 1)->text();
        dev.description = m_table->item(1, 1)->text();
        dev.deviceType = m_table->item(2, 1)->text();
        if (!db.updateDevice(dev, &err))
            QMessageBox::critical(this, "错误", "保存失败: " + err);
        break;
    }
    case NodeInterface: {
        ParamInterface iface;
        iface.id = m_nodeId;
        iface.name = m_table->item(0, 1)->text();
        iface.description = m_table->item(1, 1)->text();
        iface.adapterName = m_table->item(2, 1)->text();
        iface.adapterIp = m_table->item(4, 1)->text();
        iface.deviceAddress = m_table->item(5, 1)->text();
        if (!db.updateInterface(iface, &err))
            QMessageBox::critical(this, "错误", "保存失败: " + err);
        break;
    }
    case NodeRoute: {
        ParamRoute route;
        route.id = m_nodeId;
        route.name = m_table->item(0, 1)->text();
        QComboBox *cmbDir = qobject_cast<QComboBox*>(m_table->cellWidget(1, 1));
        if (cmbDir) route.direction = cmbDir->currentIndex() + 1;
        QComboBox *cmbProc = qobject_cast<QComboBox*>(m_table->cellWidget(2, 1));
        if (cmbProc) route.processMode = cmbProc->currentIndex() + 1;
        route.periodMs = m_table->item(3, 1)->text().toInt();
        route.description = m_table->item(4, 1)->text();
        if (!db.updateRoute(route, &err))
            QMessageBox::critical(this, "错误", "保存失败: " + err);
        break;
    }
    case NodeDatablock: {
        ParamDatablock blk;
        blk.id = m_nodeId;
        blk.name = m_table->item(0, 1)->text();
        QComboBox *cmbLen = qobject_cast<QComboBox*>(m_table->cellWidget(1, 1));
        bool hasLen = cmbLen && cmbLen->currentIndex() == 1;
        if (hasLen)
            blk.dataLength = m_table->item(2, 1)->text().toInt();
        else
            blk.dataLength = 0;
        blk.triggerMode = m_table->item(3, 1)->text();
        blk.description = m_table->item(4, 1)->text();
        if (!db.updateDatablock(blk, &err))
            QMessageBox::critical(this, "错误", "保存失败: " + err);
        break;
    }
    default:
        break;
    }
}

QString NodePropertyPanel::directionText(int dir) const
{
    switch (dir) {
    case 1: return "发送";
    case 2: return "接收";
    case 3: return "双向";
    default: return "未知";
    }
}

QString NodePropertyPanel::processModeText(int mode) const
{
    switch (mode) {
    case 1: return "周期";
    case 2: return "事件";
    case 3: return "DMA";
    case 4: return "中断";
    default: return "未知";
    }
}
