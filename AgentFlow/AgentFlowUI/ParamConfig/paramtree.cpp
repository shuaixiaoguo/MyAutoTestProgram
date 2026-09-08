#include "paramtree.h"
#include "paramdatabase.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMenu>
#include <QInputDialog>
#include <QMessageBox>
#include <QApplication>
#include <QStyle>
#include <QHeaderView>
#include <QIcon>

static QIcon iconForType(NodeType type)
{
    switch (type) {
    case NodeCategory:  return QIcon(":/icons/icons/category.svg");
    case NodeBus:       return QIcon(":/icons/icons/bus.svg");
    case NodeDevice:    return QIcon(":/icons/icons/device.svg");
    case NodeInterface: return QIcon(":/icons/icons/interface.svg");
    case NodeRoute:     return QIcon(":/icons/icons/route.svg");
    case NodeDatablock: return QIcon(":/icons/icons/datablock.svg");
    case NodeParameter: return QIcon(":/icons/icons/parameter.svg");
    default:            return QIcon();
    }
}

ParamTree::ParamTree(QWidget *parent)
    : QWidget(parent)
    , m_tree(nullptr)
    , m_model(nullptr)
    , m_searchEdit(nullptr)
{
    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setPlaceholderText("搜索参数...");
    m_searchEdit->setClearButtonEnabled(true);

    m_btnAdd = new QPushButton("新增", this);
    m_btnAdd->setIcon(QIcon(":/icons/icons/category.svg"));
    m_btnDelete = new QPushButton("删除", this);
    m_btnDelete->setIcon(QApplication::style()->standardIcon(QStyle::SP_TrashIcon));

    QHBoxLayout *toolBarLayout = new QHBoxLayout;
    toolBarLayout->setSpacing(4);
    toolBarLayout->addWidget(m_searchEdit, 1);
    toolBarLayout->addWidget(m_btnAdd);
    toolBarLayout->addWidget(m_btnDelete);
    toolBarLayout->setContentsMargins(0, 0, 0, 4);

    m_tree = new QTreeView(this);
    m_model = new QStandardItemModel(this);
    m_model->setHorizontalHeaderLabels({"名称"});
    m_tree->setModel(m_model);
    m_tree->setHeaderHidden(true);
    m_tree->setAlternatingRowColors(true);
    m_tree->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_tree->setContextMenuPolicy(Qt::CustomContextMenu);
    m_tree->setUniformRowHeights(true);
    m_tree->setIndentation(20);
    m_tree->setExpandsOnDoubleClick(true);
    m_tree->setAnimated(true);
    m_tree->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tree->setStyleSheet(
        "QTreeView {"
        "  background: #FAFBFC;"
        "  border: 1px solid #D0D7DE;"
        "  border-radius: 4px;"
        "  font-size: 10pt;"
        "}"
        "QTreeView::item {"
        "  padding: 3px 2px;"
        "  min-height: 22px;"
        "}"
        "QTreeView::item:has-children {"
        "  font-weight: bold;"
        "}"
        "QTreeView::item:selected {"
        "  background: #E1F0FF;"
        "  color: #0969DA;"
        "}"
        "QTreeView::item:!selected:hover {"
        "  background: #F0F4F8;"
        "}"
        "QTreeView::branch:has-children:open {"
        "  image: url(:/icons/icons/arrow_down.svg);"
        "}"
        "QTreeView::branch:has-children:!open {"
        "  image: url(:/icons/icons/arrow_right.svg);"
        "}"
        "QTreeView::branch:has-children:open:hover {"
        "  background: #E1E4E8;"
        "}"
        "QTreeView::branch:has-children:!open:hover {"
        "  background: #E1E4E8;"
        "}"
        "QLineEdit {"
        "  padding: 4px 6px;"
        "  border: 1px solid #D0D7DE;"
        "  border-radius: 3px;"
        "  font-size: 10pt;"
        "}"
        "QLineEdit:focus {"
        "  border: 1px solid #0969DA;"
        "}"
        "QPushButton {"
        "  padding: 4px 10px;"
        "  border: 1px solid #D0D7DE;"
        "  border-radius: 3px;"
        "  background: #F6F8FA;"
        "  font-size: 10pt;"
        "}"
        "QPushButton:hover {"
        "  background: #EAEEF2;"
        "  border-color: #0969DA;"
        "}"
        "QPushButton:pressed {"
        "  background: #D0D7DE;"
        "}"
    );

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(toolBarLayout);
    mainLayout->addWidget(m_tree, 1);
    mainLayout->setContentsMargins(6, 6, 6, 6);

    connect(m_searchEdit, &QLineEdit::textChanged,
            this, &ParamTree::onSearchTextChanged);
    connect(m_tree, &QTreeView::clicked,
            this, &ParamTree::onItemClicked);
    connect(m_tree, &QTreeView::customContextMenuRequested,
            this, &ParamTree::onItemRightClicked);
    connect(m_btnAdd, &QPushButton::clicked, this, [this]() {
        TreeNode n = currentNode();
        m_contextNode = n;
        switch (n.type) {
        case NodeRoot:      onAddCategory(); break;
        case NodeCategory:  onAddBus(); break;
        case NodeBus:       onAddDevice(); break;
        case NodeDevice:    onAddInterface(); break;
        case NodeInterface: onAddRoute(); break;
        case NodeRoute:     onAddDatablock(); break;
        case NodeDatablock: onAddParameter(); break;
        default: break;
        }
    });
    connect(m_btnDelete, &QPushButton::clicked,
            this, &ParamTree::onDeleteNode);
}

TreeNode ParamTree::currentNode() const
{
    QModelIndex idx = m_tree->currentIndex();
    if (!idx.isValid()) {
        TreeNode root;
        root.type = NodeRoot;
        root.level = 0;
        return root;
    }
    QStandardItem *item = m_model->itemFromIndex(idx);
    if (!item) {
        TreeNode root;
        root.type = NodeRoot;
        root.level = 0;
        return root;
    }
    return itemToNode(item);
}

QStandardItem* ParamTree::createItem(const QString &name, const TreeNode &nodeInfo)
{
    QStandardItem *item = new QStandardItem(name);
    item->setData(nodeInfo.id, RoleNodeId);
    item->setData(nodeInfo.parentId, RoleParentId);
    item->setData(nodeInfo.type, RoleNodeType);
    item->setData(nodeInfo.level, RoleLevel);
    item->setData(nodeInfo.code, RoleCode);
    item->setData(nodeInfo.userData, RoleUserData);
    item->setIcon(iconForType(nodeInfo.type));
    item->setEditable(false);
    return item;
}

TreeNode ParamTree::itemToNode(QStandardItem *item) const
{
    TreeNode node;
    node.id = item->data(RoleNodeId).toInt();
    node.parentId = item->data(RoleParentId).toInt();
    node.type = static_cast<NodeType>(item->data(RoleNodeType).toInt());
    node.level = item->data(RoleLevel).toInt();
    node.name = item->text();
    node.code = item->data(RoleCode).toString();
    node.userData = item->data(RoleUserData);
    return node;
}

QStandardItem* ParamTree::findItemById(int id, QStandardItem *parent) const
{
    QStandardItem *root = parent ? parent : m_model->invisibleRootItem();
    for (int i = 0; i < root->rowCount(); i++) {
        QStandardItem *child = root->child(i);
        if (child->data(RoleNodeId).toInt() == id)
            return child;
        QStandardItem *found = findItemById(id, child);
        if (found)
            return found;
    }
    return nullptr;
}

void ParamTree::loadFromDatabase()
{
    m_model->clear();
    m_model->setHorizontalHeaderLabels({"名称"});

    ParamDatabase db;
    QString err;

    QVector<ParamCategory> cats = db.listCategories(&err);
    if (!err.isEmpty()) return;

    QStandardItem *root = m_model->invisibleRootItem();
    for (const ParamCategory &cat : cats) {
        TreeNode nodeInfo;
        nodeInfo.id = cat.id;
        nodeInfo.parentId = 0;
        nodeInfo.level = 1;
        nodeInfo.name = cat.name;
        nodeInfo.code = cat.name;
        nodeInfo.type = NodeCategory;
        nodeInfo.userData = cat.description;

        QStandardItem *item = createItem(cat.name, nodeInfo);
        root->appendRow(item);
        loadChildNodes(item, cat.id, NodeCategory, 1);
    }

    m_tree->collapseAll();
}

void ParamTree::loadChildNodes(QStandardItem *parentItem, int parentId, NodeType parentType, int level)
{
    ParamDatabase db;
    QString err;

    switch (parentType) {
    case NodeCategory: {
        QVector<ParamBus> list = db.listBuses(parentId, &err);
        for (const ParamBus &item : list) {
            TreeNode nodeInfo;
            nodeInfo.id = item.id;
            nodeInfo.parentId = parentId;
            nodeInfo.level = level + 1;
            nodeInfo.name = item.name;
            nodeInfo.code = item.code;
            nodeInfo.type = NodeBus;
            nodeInfo.userData = item.description;
            QStandardItem *child = createItem(item.name, nodeInfo);
            parentItem->appendRow(child);
            loadChildNodes(child, item.id, NodeBus, level + 1);
        }
        break;
    }
    case NodeBus: {
        QVector<ParamDevice> list = db.listDevices(parentId, &err);
        for (const ParamDevice &item : list) {
            TreeNode nodeInfo;
            nodeInfo.id = item.id;
            nodeInfo.parentId = parentId;
            nodeInfo.level = level + 1;
            nodeInfo.name = item.name;
            nodeInfo.code = item.code;
            nodeInfo.type = NodeDevice;
            nodeInfo.userData = item.description;
            QStandardItem *child = createItem(item.name, nodeInfo);
            parentItem->appendRow(child);
            loadChildNodes(child, item.id, NodeDevice, level + 1);
        }
        break;
    }
    case NodeDevice: {
        QVector<ParamInterface> list = db.listInterfaces(parentId, &err);
        for (const ParamInterface &item : list) {
            TreeNode nodeInfo;
            nodeInfo.id = item.id;
            nodeInfo.parentId = parentId;
            nodeInfo.level = level + 1;
            nodeInfo.name = item.name;
            nodeInfo.code = item.code;
            nodeInfo.type = NodeInterface;
            nodeInfo.userData = item.description;
            QStandardItem *child = createItem(item.name, nodeInfo);
            parentItem->appendRow(child);
            loadChildNodes(child, item.id, NodeInterface, level + 1);
        }
        break;
    }
    case NodeInterface: {
        QVector<ParamRoute> list = db.listRoutes(parentId, &err);
        for (const ParamRoute &item : list) {
            TreeNode nodeInfo;
            nodeInfo.id = item.id;
            nodeInfo.parentId = parentId;
            nodeInfo.level = level + 1;
            nodeInfo.name = item.name;
            nodeInfo.code = QString::number(item.direction);
            nodeInfo.type = NodeRoute;
            nodeInfo.userData = item.description;
            QStandardItem *child = createItem(item.name, nodeInfo);
            parentItem->appendRow(child);
            loadChildNodes(child, item.id, NodeRoute, level + 1);
        }
        break;
    }
    case NodeRoute: {
        QVector<ParamDatablock> list = db.listDatablocks(parentId, &err);
        for (const ParamDatablock &item : list) {
            TreeNode nodeInfo;
            nodeInfo.id = item.id;
            nodeInfo.parentId = parentId;
            nodeInfo.level = level + 1;
            nodeInfo.name = item.name;
            nodeInfo.code = item.code;
            nodeInfo.type = NodeDatablock;
            nodeInfo.userData = item.description;
            QStandardItem *child = createItem(item.name, nodeInfo);
            parentItem->appendRow(child);
            loadChildNodes(child, item.id, NodeDatablock, level + 1);
        }
        break;
    }
    case NodeDatablock: {
        QVector<ParamParameter> list = db.listParameters(parentId, &err);
        for (const ParamParameter &item : list) {
            TreeNode nodeInfo;
            nodeInfo.id = item.id;
            nodeInfo.parentId = parentId;
            nodeInfo.level = level + 1;
            nodeInfo.name = item.name;
            nodeInfo.code = item.code;
            nodeInfo.type = NodeParameter;
            nodeInfo.userData = item.description;
            QStandardItem *child = createItem(item.name, nodeInfo);
            parentItem->appendRow(child);
        }
        break;
    }
    default:
        break;
    }
}

void ParamTree::reloadChildren(int parentId, NodeType parentType)
{
    Q_UNUSED(parentId)
    Q_UNUSED(parentType)
    loadFromDatabase();
}

// ---- 槽函数 ----

void ParamTree::onSearchTextChanged(const QString &text)
{
    Q_UNUSED(text)
}

void ParamTree::onItemClicked(const QModelIndex &index)
{
    Q_UNUSED(index)
    // 多选时发送空节点，属性面板显示空白
    int count = m_tree->selectionModel()->selectedIndexes().size();
    if (count > 1) {
        TreeNode empty;
        empty.type = NodeRoot;
        emit nodeSelected(empty);
        return;
    }
    // 单选
    QModelIndex idx = m_tree->currentIndex();
    if (!idx.isValid()) {
        TreeNode empty;
        empty.type = NodeRoot;
        emit nodeSelected(empty);
        return;
    }
    QStandardItem *item = m_model->itemFromIndex(idx);
    if (!item) {
        TreeNode empty;
        empty.type = NodeRoot;
        emit nodeSelected(empty);
        return;
    }
    emit nodeSelected(itemToNode(item));
}

void ParamTree::onItemRightClicked(const QPoint &pos)
{
    QModelIndex index = m_tree->indexAt(pos);
    QStandardItem *item = index.isValid() ? m_model->itemFromIndex(index) : nullptr;

    if (item) {
        m_contextNode = itemToNode(item);
    } else {
        m_contextNode = TreeNode();
        m_contextNode.type = NodeRoot;
    }

    QMenu menu(this);

    switch (m_contextNode.type) {
    case NodeRoot:
        menu.addAction("新增总线类别", this, &ParamTree::onAddCategory);
        break;
    case NodeCategory:
        menu.addAction("新增总线", this, &ParamTree::onAddBus);
        break;
    case NodeBus:
        menu.addAction("新增设备", this, &ParamTree::onAddDevice);
        break;
    case NodeDevice:
        menu.addAction("新增接口", this, &ParamTree::onAddInterface);
        break;
    case NodeInterface:
        menu.addAction("新增路由", this, &ParamTree::onAddRoute);
        break;
    case NodeRoute:
        menu.addAction("新增数据块", this, &ParamTree::onAddDatablock);
        break;
    case NodeDatablock:
        menu.addAction("新增参数", this, &ParamTree::onAddParameter);
        break;
    default:
        break;
    }

    menu.addSeparator();
    menu.addAction("重命名", this, &ParamTree::onRenameNode);
    menu.addAction("删除", this, &ParamTree::onDeleteNode);

    menu.exec(m_tree->viewport()->mapToGlobal(pos));
}

// ---- 新增 ----

void ParamTree::onAddCategory()
{
    bool ok;
    QString name = QInputDialog::getText(this, "新增总线类别",
        "类别名称:", QLineEdit::Normal, "", &ok);
    if (!ok || name.isEmpty()) return;

    ParamDatabase db;
    ParamCategory cat;
    cat.name = name;
    QString err;
    int id = db.addCategory(cat, &err);
    if (id <= 0) {
        QMessageBox::critical(this, "错误", "新增失败: " + err);
        return;
    }
    loadFromDatabase();
}

void ParamTree::onAddBus()
{
    bool ok;
    QString name = QInputDialog::getText(this, "新增总线",
        "总线名称:", QLineEdit::Normal, "", &ok);
    if (!ok || name.isEmpty()) return;

    ParamDatabase db;
    ParamBus bus;
    bus.categoryId = m_contextNode.id;
    bus.name = name;
    bus.code = name;
    QString err;
    int id = db.addBus(bus, &err);
    if (id <= 0) {
        QMessageBox::critical(this, "错误", "新增失败: " + err);
        return;
    }
    loadFromDatabase();
}

void ParamTree::onAddDevice()
{
    bool ok;
    QString name = QInputDialog::getText(this, "新增设备",
        "设备名称:", QLineEdit::Normal, "", &ok);
    if (!ok || name.isEmpty()) return;

    ParamDatabase db;
    ParamDevice dev;
    dev.busId = m_contextNode.id;
    dev.name = name;
    dev.code = name;
    QString err;
    int id = db.addDevice(dev, &err);
    if (id <= 0) {
        QMessageBox::critical(this, "错误",
            QString("新增设备失败:\n错误: %1\n总线ID=%2").arg(err).arg(dev.busId));
        return;
    }
    loadFromDatabase();
}

void ParamTree::onAddInterface()
{
    bool ok;
    QString name = QInputDialog::getText(this, "新增接口",
        "接口名称:", QLineEdit::Normal, "", &ok);
    if (!ok || name.isEmpty()) return;

    ParamDatabase db;
    ParamInterface iface;
    iface.deviceId = m_contextNode.id;
    iface.name = name;
    iface.code = name;
    QString err;
    int id = db.addInterface(iface, &err);
    if (id <= 0) {
        QMessageBox::critical(this, "错误",
            QString("新增接口失败:\n错误: %1\n设备ID=%2").arg(err).arg(iface.deviceId));
        return;
    }
    loadFromDatabase();
}

void ParamTree::onAddRoute()
{
    bool ok;
    QString name = QInputDialog::getText(this, "新增路由",
        "路由名称:", QLineEdit::Normal, "", &ok);
    if (!ok || name.isEmpty()) return;

    ParamDatabase db;
    ParamRoute route;
    route.interfaceId = m_contextNode.id;
    route.name = name;
    route.direction = 1;
    QString err;
    int id = db.addRoute(route, &err);
    if (id <= 0) {
        QMessageBox::critical(this, "错误",
            QString("新增路由失败:\n错误: %1\n接口ID=%2").arg(err).arg(route.interfaceId));
        return;
    }
    loadFromDatabase();
}

void ParamTree::onAddDatablock()
{
    bool ok;
    QString name = QInputDialog::getText(this, "新增数据块",
        "数据块名称:", QLineEdit::Normal, "", &ok);
    if (!ok || name.isEmpty()) return;

    ParamDatabase db;
    ParamDatablock blk;
    blk.routeId = m_contextNode.id;
    blk.name = name;
    blk.code = name;
    QString err;
    int id = db.addDatablock(blk, &err);
    if (id <= 0) {
        QMessageBox::critical(this, "错误",
            QString("新增数据块失败:\n错误: %1\n路由ID=%2").arg(err).arg(blk.routeId));
        return;
    }
    loadFromDatabase();
}

void ParamTree::onAddParameter()
{
    bool ok;
    QString name = QInputDialog::getText(this, "新增参数",
        "参数名称:", QLineEdit::Normal, "", &ok);
    if (!ok || name.isEmpty()) return;

    ParamDatabase db;
    ParamParameter param;
    param.datablockId = m_contextNode.id;
    param.name = name;
    param.code = name;
    param.paramType = "uint8";
    param.bitWidth = 8;
    QString err;
    int id = db.addParameter(param, &err);
    if (id <= 0) {
        QMessageBox::critical(this, "错误",
            QString("新增参数失败:\n错误: %1\n数据块ID=%2").arg(err).arg(param.datablockId));
        return;
    }
    loadFromDatabase();
}

// ---- 删除 ----

void ParamTree::onDeleteNode()
{
    QModelIndexList selected = m_tree->selectionModel()->selectedIndexes();

    if (selected.isEmpty()) {
        TreeNode n = currentNode();
        if (n.type == NodeRoot) return;
        selected << m_tree->currentIndex();
    }

    QStringList names;
    QList<TreeNode> nodes;
    for (const QModelIndex &idx : selected) {
        QStandardItem *item = m_model->itemFromIndex(idx);
        if (!item) continue;
        TreeNode n = itemToNode(item);
        if (n.type == NodeRoot) continue;
        nodes.append(n);
        names.append(n.name);
    }

    if (nodes.isEmpty()) return;

    QMessageBox::StandardButton ret = QMessageBox::question(
        this, "确认删除",
        QString("确定要删除以下 %1 个节点吗？\n\n%2\n\n其下所有子节点也将被删除，无法恢复！")
            .arg(nodes.size())
            .arg(names.join(", ")),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No);

    if (ret != QMessageBox::Yes) return;

    ParamDatabase db;
    QString err;
    int failCount = 0;

    for (const TreeNode &n : nodes) {
        bool ok = false;
        switch (n.type) {
        case NodeCategory:  ok = db.deleteCategory(n.id, &err); break;
        case NodeBus:       ok = db.deleteBus(n.id, &err); break;
        case NodeDevice:    ok = db.deleteDevice(n.id, &err); break;
        case NodeInterface: ok = db.deleteInterface(n.id, &err); break;
        case NodeRoute:     ok = db.deleteRoute(n.id, &err); break;
        case NodeDatablock: ok = db.deleteDatablock(n.id, &err); break;
        case NodeParameter: ok = db.deleteParameter(n.id, &err); break;
        default: break;
        }
        if (!ok) failCount++;
    }

    if (failCount > 0)
        QMessageBox::warning(this, "部分失败",
            QString("%1 个节点删除失败。最后错误: %2").arg(failCount).arg(err));

    // 清除选中状态，属性面板显示空白
    m_tree->clearSelection();
    emit nodeSelected(TreeNode());

    loadFromDatabase();
}

// ---- 重命名 ----

void ParamTree::onRenameNode()
{
    TreeNode n = currentNode();
    if (n.type == NodeRoot) return;

    bool ok;
    QString newName = QInputDialog::getText(this, "重命名",
        "新名称:", QLineEdit::Normal, n.name, &ok);
    if (!ok || newName.isEmpty() || newName == n.name) return;

    ParamDatabase db;
    QString err;
    bool ok2 = false;

    switch (n.type) {
    case NodeCategory: {
        ParamCategory cat;
        cat.id = n.id;
        cat.name = newName;
        cat.description = n.userData.toString();
        ok2 = db.updateCategory(cat, &err);
        break;
    }
    case NodeBus: {
        ParamBus bus;
        bus.id = n.id;
        bus.name = newName;
        bus.code = n.code;
        ok2 = db.updateBus(bus, &err);
        break;
    }
    case NodeDevice: {
        ParamDevice dev;
        dev.id = n.id;
        dev.name = newName;
        dev.code = n.code;
        ok2 = db.updateDevice(dev, &err);
        break;
    }
    case NodeInterface: {
        ParamInterface iface;
        iface.id = n.id;
        iface.name = newName;
        iface.code = n.code;
        ok2 = db.updateInterface(iface, &err);
        break;
    }
    case NodeRoute: {
        ParamRoute route;
        route.id = n.id;
        route.name = newName;
        ok2 = db.updateRoute(route, &err);
        break;
    }
    case NodeDatablock: {
        ParamDatablock blk;
        blk.id = n.id;
        blk.name = newName;
        blk.code = n.code;
        ok2 = db.updateDatablock(blk, &err);
        break;
    }
    case NodeParameter: {
        ParamParameter param;
        param.id = n.id;
        param.name = newName;
        param.code = n.code;
        ok2 = db.updateParameter(param, &err);
        break;
    }
    default: break;
    }

    if (!ok2) {
        QMessageBox::critical(this, "错误", "重命名失败: " + err);
        return;
    }
    loadFromDatabase();
}
