#include "paramconfigdialog.h"
#include "paramtree.h"
#include "parampropertypanel.h"
#include "nodepropertypanel.h"
#include "paramdatabase.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QLabel>
#include <QToolBar>
#include <QMessageBox>
#include <QApplication>
#include <QStyle>

ParamConfigDialog::ParamConfigDialog(QWidget *parent)
    : QDialog(parent)
    , m_paramTree(nullptr)
    , m_propertyStack(nullptr)
    , m_paramPropertyPanel(nullptr)
    , m_nodePropertyPanel(nullptr)
    , m_infoStack(nullptr)
{
    setWindowTitle("参数配置");
    setMinimumSize(1200, 700);

    buildUi();
    buildToolBar();
    buildInfoPanels();

    ParamDatabase db;
    QString err;
    if (!db.createTables(&err)) {
        QMessageBox::warning(this, "警告", "创建参数表失败: " + err);
    }

    m_paramTree->loadFromDatabase();
}

void ParamConfigDialog::buildUi()
{
    QSplitter *mainSplitter = new QSplitter(Qt::Horizontal, this);

    // --- 左侧：垂直分割器（上方树视图，下方属性面板） ---
    QSplitter *leftSplitter = new QSplitter(Qt::Vertical, mainSplitter);

    m_paramTree = new ParamTree(leftSplitter);

    m_propertyStack = new QStackedWidget(leftSplitter);

    // 参数级属性面板
    m_paramPropertyPanel = new ParamPropertyPanel(m_propertyStack);
    m_propertyStack->addWidget(m_paramPropertyPanel);

    // 通用节点属性面板
    m_nodePropertyPanel = new NodePropertyPanel(m_propertyStack);
    m_propertyStack->addWidget(m_nodePropertyPanel);

    // 空白占位
    m_emptyWidget = new QWidget(m_propertyStack);
    QVBoxLayout *emptyLayout = new QVBoxLayout(m_emptyWidget);
    QLabel *emptyHint = new QLabel("请选择一个节点查看属性", m_emptyWidget);
    emptyHint->setAlignment(Qt::AlignCenter);
    emptyHint->setStyleSheet("QLabel { color: #9ca3af; font-size: 10pt; }");
    emptyLayout->addWidget(emptyHint, 1);
    m_propertyStack->addWidget(m_emptyWidget);

    leftSplitter->addWidget(m_paramTree);
    leftSplitter->addWidget(m_propertyStack);
    leftSplitter->setStretchFactor(0, 3);
    leftSplitter->setStretchFactor(1, 2);
    leftSplitter->setSizes({400, 250});

    // --- 右侧：信息面板 ---
    m_infoStack = new QStackedWidget(mainSplitter);

    m_infoTopology = new QWidget(m_infoStack);
    QVBoxLayout *topoLayout = new QVBoxLayout(m_infoTopology);
    QLabel *topoLabel = new QLabel("拓扑图\n\n（设备/接口级显示连接关系）", m_infoTopology);
    topoLabel->setAlignment(Qt::AlignCenter);
    topoLabel->setStyleSheet("QLabel { color: #9ca3af; font-size: 10pt; }");
    topoLayout->addWidget(topoLabel, 1);
    m_infoStack->addWidget(m_infoTopology);

    m_infoDatablock = new QWidget(m_infoStack);
    QVBoxLayout *dbLayout = new QVBoxLayout(m_infoDatablock);
    m_infoTitle = new QLabel("数据块信息", m_infoDatablock);
    m_infoTitle->setStyleSheet("QLabel { font-size: 11pt; font-weight: bold; }");
    dbLayout->addWidget(m_infoTitle);
    QLabel *dbHint = new QLabel("选中数据块查看详情", m_infoDatablock);
    dbHint->setAlignment(Qt::AlignCenter);
    dbHint->setStyleSheet("QLabel { color: #9ca3af; }");
    dbLayout->addWidget(dbHint, 1);
    m_infoStack->addWidget(m_infoDatablock);

    m_infoParameter = new QWidget(m_infoStack);
    QVBoxLayout *paramInfoLayout = new QVBoxLayout(m_infoParameter);
    QLabel *paramInfoLabel = new QLabel("参数详情\n\n（参数位结构、枚举值、公式预览）", m_infoParameter);
    paramInfoLabel->setAlignment(Qt::AlignCenter);
    paramInfoLabel->setStyleSheet("QLabel { color: #9ca3af; font-size: 10pt; }");
    paramInfoLayout->addWidget(paramInfoLabel, 1);
    m_infoStack->addWidget(m_infoParameter);

    mainSplitter->addWidget(leftSplitter);
    mainSplitter->addWidget(m_infoStack);
    mainSplitter->setStretchFactor(0, 2);
    mainSplitter->setStretchFactor(1, 3);
    mainSplitter->setSizes({400, 600});

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(mainSplitter, 1);
    mainLayout->setContentsMargins(6, 6, 6, 6);

    connect(m_paramTree, &ParamTree::nodeSelected,
            this, &ParamConfigDialog::onTreeNodeSelected);
    connect(m_paramPropertyPanel, &ParamPropertyPanel::parameterChanged,
            this, &ParamConfigDialog::onParameterChanged);
}

void ParamConfigDialog::buildToolBar()
{
    QToolBar *toolBar = new QToolBar(this);
    toolBar->setMovable(false);
    toolBar->setIconSize(QSize(18, 18));

    QStyle *style = QApplication::style();

    m_actImport = toolBar->addAction(
        style->standardIcon(QStyle::SP_DialogOpenButton), "导入Excel");
    m_actExport = toolBar->addAction(
        style->standardIcon(QStyle::SP_DialogSaveButton), "导出Excel");
    toolBar->addSeparator();
    toolBar->addAction(
        style->standardIcon(QStyle::SP_BrowserReload), "刷新",
        m_paramTree, &ParamTree::loadFromDatabase);

    connect(m_actImport, &QAction::triggered, this, &ParamConfigDialog::onImport);
    connect(m_actExport, &QAction::triggered, this, &ParamConfigDialog::onExport);

    QVBoxLayout *mainLayout = qobject_cast<QVBoxLayout*>(layout());
    if (mainLayout)
        mainLayout->insertWidget(0, toolBar);
}

void ParamConfigDialog::buildInfoPanels()
{
}

void ParamConfigDialog::onTreeNodeSelected(const TreeNode &node)
{
    switchPropertyPanel(node);
    switchInfoPanel(node);
}

void ParamConfigDialog::switchPropertyPanel(const TreeNode &node)
{
    if (node.type == NodeParameter) {
        ParamDatabase db;
        QString err;
        QVector<ParamParameter> params = db.listParameters(node.parentId, &err);
        for (const ParamParameter &p : params) {
            if (p.id == node.id) {
                m_paramPropertyPanel->loadParameter(p);
                break;
            }
        }
        m_propertyStack->setCurrentWidget(m_paramPropertyPanel);
    } else if (node.type >= NodeCategory && node.type <= NodeDatablock) {
        m_nodePropertyPanel->loadNode(node);
        m_propertyStack->setCurrentWidget(m_nodePropertyPanel);
    } else {
        m_paramPropertyPanel->clearForm();
        m_propertyStack->setCurrentWidget(m_emptyWidget);
    }
}

void ParamConfigDialog::switchInfoPanel(const TreeNode &node)
{
    switch (node.type) {
    case NodeDevice:
    case NodeInterface:
        m_infoStack->setCurrentWidget(m_infoTopology);
        break;
    case NodeDatablock:
        m_infoStack->setCurrentWidget(m_infoDatablock);
        m_infoTitle->setText("数据块: " + node.name);
        break;
    case NodeParameter:
        m_infoStack->setCurrentWidget(m_infoParameter);
        break;
    default:
        m_infoStack->setCurrentWidget(m_infoTopology);
        break;
    }
}

void ParamConfigDialog::onParameterChanged(int paramId)
{
    Q_UNUSED(paramId)
    m_paramTree->loadFromDatabase();
}

void ParamConfigDialog::onImport()
{
    QMessageBox::information(this, "提示", "Excel导入功能开发中...");
}

void ParamConfigDialog::onExport()
{
    QMessageBox::information(this, "提示", "Excel导出功能开发中...");
}
