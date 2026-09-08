#ifndef PARAMCONFIGDIALOG_H
#define PARAMCONFIGDIALOG_H

#include <QDialog>
#include <QStackedWidget>
#include <QLabel>
#include <QAction>
#include "paramtree.h"

class ParamTree;
class ParamPropertyPanel;
class NodePropertyPanel;

class ParamConfigDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ParamConfigDialog(QWidget *parent = nullptr);

private slots:
    void onTreeNodeSelected(const TreeNode &node);
    void onParameterChanged(int paramId);
    void onImport();
    void onExport();

private:
    ParamTree *m_paramTree;

    QStackedWidget *m_propertyStack;
    ParamPropertyPanel *m_paramPropertyPanel;
    NodePropertyPanel *m_nodePropertyPanel;
    QWidget *m_emptyWidget;

    QStackedWidget *m_infoStack;
    QWidget *m_infoTopology;
    QWidget *m_infoDatablock;
    QWidget *m_infoParameter;
    QLabel  *m_infoTitle;

    QAction *m_actImport;
    QAction *m_actExport;

    void buildUi();
    void buildToolBar();
    void buildInfoPanels();

    void switchPropertyPanel(const TreeNode &node);
    void switchInfoPanel(const TreeNode &node);
};

#endif // PARAMCONFIGDIALOG_H
