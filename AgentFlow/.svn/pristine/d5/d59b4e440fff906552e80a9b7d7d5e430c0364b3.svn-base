#include "mainwindow.h"
#include "icdtreeviewdock.h"
#include "controltoolboxdock.h"
#include "propertypaneldock.h"
#include "findresultdock.h"
#include "runlogdock.h"
#include "canvastabwidget.h"
#include "statusbar.h"
#include "projectmanager.h"
#include "commondef.h"
#include <QSettings>
#include <QMenuBar>
#include <QToolBar>
#include <QMdiSubWindow>

// 创建并布局所有 DockWidget
void MainWindow::initDockWidgets()
{
    // ① ICD接口数据库树 — 左侧
    m_icdTreeViewDock = new IcdTreeViewDock(this);
    addDockWidget(Qt::LeftDockWidgetArea, m_icdTreeViewDock);

    // ② 控件工具箱 — 左侧（在①右侧）
    m_controlToolBoxDock = new ControlToolBoxDock(this);
    addDockWidget(Qt::LeftDockWidgetArea, m_controlToolBoxDock);
    splitDockWidget(m_icdTreeViewDock, m_controlToolBoxDock, Qt::Horizontal);

    // ③ 属性编辑面板 — 右侧
    m_propertyPanelDock = new PropertyPanelDock(this);
    addDockWidget(Qt::RightDockWidgetArea, m_propertyPanelDock);

    // ④ 参数查找结果 — 底部左半
    m_findResultDock = new FindResultDock(this);
    addDockWidget(Qt::BottomDockWidgetArea, m_findResultDock);

    // ⑤ 运行日志 — 底部右半（与④水平排列）
    m_runLogDock = new RunLogDock(this);
    addDockWidget(Qt::BottomDockWidgetArea, m_runLogDock);
    splitDockWidget(m_findResultDock, m_runLogDock, Qt::Horizontal);

    // 底部两个面板各占一半宽度
    resizeDocks({m_findResultDock, m_runLogDock},
                {300, 500}, Qt::Horizontal);
}

// 创建菜单栏
void MainWindow::initMenuBar()
{
    QMenuBar *mb = menuBar();

    // 文件菜单
    m_menuFile = mb->addMenu("文件");
    m_menuFile->addAction(m_actNewProject);
    m_menuFile->addAction(m_actOpenProject);
    m_menuFile->addAction(m_actSave);
    m_menuFile->addAction(m_actModifyProject);
    m_menuFile->addSeparator();
    m_menuFile->addAction(m_actExit);

    // 编辑菜单
    m_menuEdit = mb->addMenu("编辑");
    m_menuEdit->addAction(m_actUndo);
    m_menuEdit->addAction(m_actRedo);
    m_menuEdit->addSeparator();
    m_menuEdit->addAction("剪切");
    m_menuEdit->addAction("复制");
    m_menuEdit->addAction("粘贴");

    // 视图菜单
    m_menuView = mb->addMenu("视图");
    m_menuView->addAction(m_actViewIcdTree);
    m_menuView->addAction(m_actViewToolbox);
    m_menuView->addAction(m_actViewProperty);
    m_menuView->addAction(m_actViewFindResult);
    m_menuView->addAction(m_actViewLog);

    // 工程菜单
    m_menuProject = mb->addMenu("工程");
    m_menuProject->addAction(m_actNewPanel);
    m_menuProject->addAction(m_actNewWindow);
    m_menuProject->addSeparator();
    m_menuProject->addAction(m_actStartTest);
    m_menuProject->addAction(m_actStopTest);
    m_menuProject->addSeparator();
    m_menuProject->addAction(m_actParamConfig);

    // 设置菜单
    m_menuSettings = mb->addMenu("设置");
    m_menuSettings->addAction(m_actNewConn);
    m_menuSettings->addAction(m_actOpenConn);
    m_menuSettings->addAction(m_actCloseConn);
    m_menuSettings->addSeparator();
    m_menuSettings->addAction(m_actDbSettings);

    // 帮助菜单
    m_menuHelp = mb->addMenu("帮助");
    m_menuHelp->addAction("关于");
}

// 创建工具栏
void MainWindow::initToolBar()
{
    m_mainToolBar = addToolBar("主工具栏");
    m_mainToolBar->setObjectName("MainToolBar");
    m_mainToolBar->setMovable(false);
    m_mainToolBar->setIconSize(QSize(20, 20));
    m_mainToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    m_mainToolBar->addAction(m_actSave);
    m_mainToolBar->addSeparator();
    m_mainToolBar->addAction(m_actNewPanel);
    m_mainToolBar->addAction(m_actNewWindow);
    m_mainToolBar->addSeparator();
    m_mainToolBar->addAction(m_actStartTest);
    m_mainToolBar->addAction(m_actStopTest);
    m_mainToolBar->addSeparator();
    m_mainToolBar->addAction(m_actParamConfig);
}

// 初始化状态栏
void MainWindow::initStatusBar()
{
    m_statusBar = new StatusBar(this);
    setStatusBar(m_statusBar);

    m_statusBar->updateStatus({
        "脱机模式",
        "Agent: 未知",
        "DB: 未连接",
        "Server: 未启动"
    });
}

// 信号槽连接
void MainWindow::initConnections()
{
    // ① ICD树选中 → ③ 属性面板加载
    connect(m_icdTreeViewDock, &IcdTreeViewDock::icdNodeSelected,
            m_propertyPanelDock, &PropertyPanelDock::setTargetObject);

    // ⑥ 画布窗体切换 → ③ 属性面板
    connect(m_canvasTab, &CanvasTabWidget::windowChanged,
            this, [this](QMdiSubWindow *window){
        if (window)
            m_propertyPanelDock->setTargetObject(
                window->windowTitle(), "MDI子窗口");
    });

    // ④ 查找结果选中 → ③ 属性面板
    connect(m_findResultDock, &FindResultDock::resultSelected,
            m_propertyPanelDock, [this](const QVariantMap &data){
        m_propertyPanelDock->setPropertyData(data);
    });

    // 工具栏动作
    connect(m_actNewPanel, &QAction::triggered, this, &MainWindow::onNewPanel);
    connect(m_actNewWindow, &QAction::triggered, this, &MainWindow::onNewWindow);
    connect(m_actStartTest, &QAction::triggered, this, &MainWindow::onStartTest);
    connect(m_actStopTest, &QAction::triggered, this, &MainWindow::onStopTest);
    connect(m_actParamConfig, &QAction::triggered, this, &MainWindow::onParamConfig);
    connect(m_actSave, &QAction::triggered, this, &MainWindow::onSaveProject);

    // 工程管理动作
    connect(m_actNewProject, &QAction::triggered, this, &MainWindow::onNewProject);
    connect(m_actOpenProject, &QAction::triggered, this, &MainWindow::onOpenProject);
    connect(m_actModifyProject, &QAction::triggered, this, &MainWindow::onModifyProject);
    connect(m_actDbSettings, &QAction::triggered, this, &MainWindow::onDbSettings);

    // 连接管理
    connect(m_actNewConn, &QAction::triggered, this, &MainWindow::onNewConnection);
    connect(m_actOpenConn, &QAction::triggered, this, &MainWindow::onOpenConnection);
    connect(m_actCloseConn, &QAction::triggered, this, &MainWindow::onCloseConnection);

    // 视图切换
    connect(m_actViewIcdTree, &QAction::toggled,
            this, &MainWindow::onViewIcdTree);
    connect(m_actViewToolbox, &QAction::toggled,
            this, &MainWindow::onViewToolbox);
    connect(m_actViewProperty, &QAction::toggled,
            this, &MainWindow::onViewProperty);
    connect(m_actViewFindResult, &QAction::toggled,
            this, &MainWindow::onViewFindResult);
    connect(m_actViewLog, &QAction::toggled,
            this, &MainWindow::onViewLog);

    // 退出
    connect(m_actExit, &QAction::triggered, this, &MainWindow::onExit);

    // 工程管理信号
    connect(m_projectManager, &ProjectManager::projectOpened,
            this, [this](const QString &name){
        updateWindowTitle();
        m_runLogDock->appendLog(LogInfo, QString("工程已打开: %1").arg(name));
        m_statusBar->updateStatus({
            "脱机模式",
            "Agent: 未知",
            "DB: 已连接",
            "Server: 未启动"
        });
    });
    connect(m_projectManager, &ProjectManager::projectClosed,
            this, [this](){
        updateWindowTitle();
        m_runLogDock->appendLog(LogInfo, "工程已关闭");
        m_statusBar->updateStatus({
            "脱机模式",
            "Agent: 未知",
            "DB: 未连接",
            "Server: 未启动"
        });
    });
    connect(m_projectManager, &ProjectManager::projectSaved,
            this, [this](){
        m_runLogDock->appendLog(LogInfo, "工程已保存");
    });
    connect(m_projectManager, &ProjectManager::projectError,
            this, [this](const QString &msg){
        m_runLogDock->appendLog(LogError, msg);
    });
}

// 恢复布局
void MainWindow::restoreLayout()
{
    QSettings settings("AgentFlow", "TestConfig");
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());

    m_icdTreeViewDock->setVisible(settings.value("IcdTreeViewDock", true).toBool());
    m_controlToolBoxDock->setVisible(settings.value("ControlToolBoxDock", true).toBool());
    m_propertyPanelDock->setVisible(settings.value("PropertyPanelDock", true).toBool());
    m_findResultDock->setVisible(settings.value("FindResultDock", true).toBool());
    m_runLogDock->setVisible(settings.value("RunLogDock", true).toBool());

    int index = settings.value("CurrentPanelIndex", 0).toInt();
    if (index >= 0 && index < m_canvasTab->count())
        m_canvasTab->setCurrentIndex(index);
}
