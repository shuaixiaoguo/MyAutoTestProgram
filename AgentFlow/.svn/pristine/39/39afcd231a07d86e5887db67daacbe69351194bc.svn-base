#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAction>
#include <QMenu>
#include <QToolBar>

class IcdTreeViewDock;
class ControlToolBoxDock;
class PropertyPanelDock;
class FindResultDock;
class RunLogDock;
class CanvasTabWidget;
class StatusBar;
class ProjectManager;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    // DockWidget 指针
    IcdTreeViewDock     *m_icdTreeViewDock;
    ControlToolBoxDock  *m_controlToolBoxDock;
    PropertyPanelDock   *m_propertyPanelDock;
    FindResultDock      *m_findResultDock;
    RunLogDock          *m_runLogDock;

    // 中央区域
    CanvasTabWidget *m_canvasTab;

    // 状态栏
    StatusBar *m_statusBar;

    // 工程管理
    ProjectManager *m_projectManager;

    // 菜单
    QMenu *m_menuFile;
    QMenu *m_menuEdit;
    QMenu *m_menuView;
    QMenu *m_menuProject;
    QMenu *m_menuSettings;
    QMenu *m_menuHelp;

    // 工具栏
    QToolBar *m_mainToolBar;

    // 工具栏动作
    QAction *m_actNewPanel;
    QAction *m_actNewWindow;
    QAction *m_actStartTest;
    QAction *m_actStopTest;
    QAction *m_actSave;
    QAction *m_actParamConfig;

    // 视图菜单动作
    QAction *m_actViewIcdTree;
    QAction *m_actViewToolbox;
    QAction *m_actViewProperty;
    QAction *m_actViewFindResult;
    QAction *m_actViewLog;

    // 文件菜单动作
    QAction *m_actNewProject;
    QAction *m_actOpenProject;
    QAction *m_actModifyProject;
    QAction *m_actExit;

    // 编辑菜单动作
    QAction *m_actUndo;
    QAction *m_actRedo;

    // 设置菜单动作
    QAction *m_actDbSettings;
    QAction *m_actNewConn;
    QAction *m_actOpenConn;
    QAction *m_actCloseConn;

    // 工程菜单动作
    QAction *m_actProjectSettings;

    // 初始化方法（在 mainwindow_init.cpp 中实现）
    void initMenuBar();
    void initToolBar();
    void initDockWidgets();
    void initStatusBar();
    void initConnections();
    void restoreLayout();

    // 动作创建（在 mainwindow_menu.cpp 中实现）
    void createActions();
    void onNewPanel();
    void onNewWindow();
    void onStartTest();
    void onStopTest();
    void onParamConfig();

    // 工程管理槽函数
    void onNewProject();
    void onOpenProject();
    void onSaveProject();
    void onModifyProject();
    void onDbSettings();

    // 连接管理槽函数
    void onNewConnection();
    void onOpenConnection();
    void onCloseConnection();
    void updateConnActions();

    // 视图切换槽函数
    void onViewIcdTree(bool visible);
    void onViewToolbox(bool visible);
    void onViewProperty(bool visible);
    void onViewFindResult(bool visible);
    void onViewLog(bool visible);

    // 退出
    void onExit();

    // 更新窗口标题
    void updateWindowTitle();

    // 界面配置变更标记
    bool m_isModified;

    // 标记工程已修改（保存按钮变亮）
    void markModified();
    // 清除修改标记（保存后调用）
    void clearModified();

    // 启动时自动连接数据库
    void autoConnectDatabase();

    // 更新数据库状态灯
    void updateDbStatus();

protected:
    void closeEvent(QCloseEvent *event) override;
};

#endif // MAINWINDOW_H
