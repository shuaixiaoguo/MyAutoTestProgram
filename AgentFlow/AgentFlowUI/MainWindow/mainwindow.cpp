#include "mainwindow.h"
#include "icdtreeviewdock.h"
#include "controltoolboxdock.h"
#include "propertypaneldock.h"
#include "findresultdock.h"
#include "runlogdock.h"
#include "canvastabwidget.h"
#include "statusbar.h"
#include "projectmanager.h"
#include "dbmanager.h"
#include "commondef.h"
#include <QCloseEvent>
#include <QSettings>
#include <QIcon>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_icdTreeViewDock(nullptr)
    , m_controlToolBoxDock(nullptr)
    , m_propertyPanelDock(nullptr)
    , m_findResultDock(nullptr)
    , m_runLogDock(nullptr)
    , m_canvasTab(nullptr)
    , m_statusBar(nullptr)
    , m_projectManager(new ProjectManager(this))
    , m_isModified(false)
{
    setWindowTitle("AgentFlow - 自动化测试配置软件");
    setWindowIcon(QIcon());
    setMinimumSize(1024, 600);

    createActions();

    m_canvasTab = new CanvasTabWidget(this);
    setCentralWidget(m_canvasTab);
    m_canvasTab->newPanel("测试面板1");

    initDockWidgets();

    initMenuBar();
    initToolBar();

    initStatusBar();

    initConnections();

    restoreLayout();

    // 启动时自动尝试连接数据库
    autoConnectDatabase();
}

MainWindow::~MainWindow()
{
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings("AgentFlow", "TestConfig");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());

    settings.setValue("IcdTreeViewDock",   m_icdTreeViewDock->isVisible());
    settings.setValue("ControlToolBoxDock", m_controlToolBoxDock->isVisible());
    settings.setValue("PropertyPanelDock",  m_propertyPanelDock->isVisible());
    settings.setValue("FindResultDock",    m_findResultDock->isVisible());
    settings.setValue("RunLogDock",         m_runLogDock->isVisible());

    settings.setValue("CurrentPanelIndex", m_canvasTab->currentIndex());

    if (m_projectManager->isProjectOpen())
        m_projectManager->closeProject();

    event->accept();
}
