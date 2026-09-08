#include "icdtreeviewdock.h"

IcdTreeViewDock::IcdTreeViewDock(QWidget *parent)
    : QDockWidget("ICD接口数据库", parent)
{
    setObjectName("IcdTreeViewDock");
    initUi();
    initTree();
}

void IcdTreeViewDock::initUi()
{
    setFeatures(QDockWidget::DockWidgetMovable
              | QDockWidget::DockWidgetFloatable);
    setMinimumWidth(160);
}

void IcdTreeViewDock::initTree()
{
    m_tree = new QTreeWidget(this);
    m_tree->setHeaderHidden(true);
    m_tree->setExpandsOnDoubleClick(false);

    // Mock 数据
    QTreeWidgetItem *root = new QTreeWidgetItem(m_tree,
        QStringList() << "新建工程1");
    QFont boldFont;
    boldFont.setBold(true);
    root->setFont(0, boldFont);

    QTreeWidgetItem *defaultIcd = new QTreeWidgetItem(root,
        QStringList() << "默认ICD");
    defaultIcd->setData(0, Qt::UserRole, "default_icd");
    defaultIcd->setData(0, Qt::UserRole + 1, "icd_root");

    QTreeWidgetItem *device = new QTreeWidgetItem(root,
        QStringList() << "新增设备");
    device->setData(0, Qt::UserRole, "device_001");
    device->setData(0, Qt::UserRole + 1, "device");

    QStringList subDevices;
    subDevices << "遥信" << "遥测" << "遥控" << "遥调" << "告警显示" << "列表";
    foreach (const QString &name, subDevices) {
        QTreeWidgetItem *sub = new QTreeWidgetItem(device,
            QStringList() << name);
        sub->setData(0, Qt::UserRole, name);
        sub->setData(0, Qt::UserRole + 1, "sub_device");
    }

    QTreeWidgetItem *opPanel = new QTreeWidgetItem(root,
        QStringList() << "操作面板");
    opPanel->setData(0, Qt::UserRole, "op_panel");
    opPanel->setData(0, Qt::UserRole + 1, "op_panel");

    QTreeWidgetItem *testFolder = new QTreeWidgetItem(opPanel,
        QStringList() << "测试");
    testFolder->setData(0, Qt::UserRole, "test");
    testFolder->setData(0, Qt::UserRole + 1, "test_folder");

    QStringList testItems;
    testItems << "调试执行" << "基础信息" << "遥控输出";
    foreach (const QString &name, testItems) {
        QTreeWidgetItem *sub = new QTreeWidgetItem(testFolder,
            QStringList() << name);
        sub->setData(0, Qt::UserRole, name);
        sub->setData(0, Qt::UserRole + 1, "test_item");
    }

    m_tree->expandItem(root);
    m_tree->expandItem(device);
    m_tree->expandItem(opPanel);

    setWidget(m_tree);

    connect(m_tree, &QTreeWidget::itemClicked,
            this, &IcdTreeViewDock::onItemClicked);
    connect(m_tree, &QTreeWidget::itemDoubleClicked,
            this, &IcdTreeViewDock::onItemDoubleClicked);
}

void IcdTreeViewDock::onItemClicked(QTreeWidgetItem *item, int /*column*/)
{
    QString nodeId = item->data(0, Qt::UserRole).toString();
    QString nodeType = item->data(0, Qt::UserRole + 1).toString();
    if (!nodeId.isEmpty())
        emit icdNodeSelected(nodeId, nodeType);
}

void IcdTreeViewDock::onItemDoubleClicked(QTreeWidgetItem *item, int /*column*/)
{
    QString nodeId = item->data(0, Qt::UserRole).toString();
    QString nodeType = item->data(0, Qt::UserRole + 1).toString();
    if (!nodeId.isEmpty())
        emit icdNodeSelected(nodeId, nodeType);
}
