#ifndef NODEPROPERTYPANEL_H
#define NODEPROPERTYPANEL_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include "paramtree.h"

class NodePropertyPanel : public QWidget
{
    Q_OBJECT
public:
    explicit NodePropertyPanel(QWidget *parent = nullptr);

    void loadNode(const TreeNode &node);
    void clearForm();

private slots:
    void onSaveClicked();

private:
    QTableWidget *m_table;
    QPushButton *m_btnSave;
    int m_nodeId;
    NodeType m_nodeType;

    void addRow(const QString &prop, const QString &value, bool editable = true);
    void addRowWithCombo(const QString &prop, const QStringList &options, int currentIndex);

    void setupCategory(int id);
    void setupBus(int id);
    void setupDevice(int id);
    void setupInterface(int id);
    void setupRoute(int id);
    void setupDatablock(int id);

    QString directionText(int dir) const;
    QString processModeText(int mode) const;
};

#endif // NODEPROPERTYPANEL_H
