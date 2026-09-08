#ifndef PARAMTREE_H
#define PARAMTREE_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QTreeView>
#include <QStandardItemModel>
#include <QStandardItem>

enum NodeType {
    NodeRoot = 0,
    NodeCategory = 1,
    NodeBus = 2,
    NodeDevice = 3,
    NodeInterface = 4,
    NodeRoute = 5,
    NodeDatablock = 6,
    NodeParameter = 7
};

struct TreeNode {
    int id = 0;
    int parentId = 0;
    int level = 0;
    QString name;
    QString code;
    NodeType type = NodeRoot;
    QVariant userData;
    TreeNode() {}
};

class ParamTree : public QWidget
{
    Q_OBJECT
public:
    explicit ParamTree(QWidget *parent = nullptr);

    void loadFromDatabase();
    void reloadChildren(int parentId, NodeType parentType);
    TreeNode currentNode() const;

signals:
    void nodeSelected(const TreeNode &node);

private slots:
    void onSearchTextChanged(const QString &text);
    void onItemClicked(const QModelIndex &index);
    void onItemRightClicked(const QPoint &pos);

    void onAddCategory();
    void onAddBus();
    void onAddDevice();
    void onAddInterface();
    void onAddRoute();
    void onAddDatablock();
    void onAddParameter();
    void onDeleteNode();
    void onRenameNode();

private:
    QTreeView *m_tree;
    QStandardItemModel *m_model;
    QLineEdit  *m_searchEdit;
    QPushButton *m_btnAdd;
    QPushButton *m_btnDelete;

    TreeNode m_contextNode;

    // 角色
    enum DataRole {
        RoleNodeId = Qt::UserRole + 1,
        RoleParentId = Qt::UserRole + 2,
        RoleNodeType = Qt::UserRole + 3,
        RoleLevel = Qt::UserRole + 4,
        RoleCode = Qt::UserRole + 5,
        RoleUserData = Qt::UserRole + 6,
    };

    void loadChildNodes(QStandardItem *parentItem, int parentId, NodeType parentType, int level);
    QStandardItem* createItem(const QString &name, const TreeNode &nodeInfo);
    TreeNode itemToNode(QStandardItem *item) const;
    QStandardItem* findItemById(int id, QStandardItem *parent = nullptr) const;
};

#endif // PARAMTREE_H
