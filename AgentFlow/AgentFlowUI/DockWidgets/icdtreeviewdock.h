#ifndef ICDTREEVIEWDOCK_H
#define ICDTREEVIEWDOCK_H

#include <QDockWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>

class IcdTreeViewDock : public QDockWidget
{
    Q_OBJECT
public:
    explicit IcdTreeViewDock(QWidget *parent = nullptr);

private:
    QTreeWidget *m_tree;
    void initTree();
    void initUi();

private slots:
    void onItemClicked(QTreeWidgetItem *item, int column);
    void onItemDoubleClicked(QTreeWidgetItem *item, int column);

signals:
    void icdNodeSelected(const QString &nodeId, const QString &nodeType);
};

#endif // ICDTREEVIEWDOCK_H
