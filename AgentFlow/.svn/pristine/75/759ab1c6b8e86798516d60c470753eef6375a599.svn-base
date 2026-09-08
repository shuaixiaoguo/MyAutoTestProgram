#include "mainwindow.h"
#include "canvastabwidget.h"
#include "icdtreeviewdock.h"
#include "controltoolboxdock.h"
#include "propertypaneldock.h"
#include "findresultdock.h"
#include "runlogdock.h"
#include "statusbar.h"
#include "commondef.h"
#include "projectmanager.h"
#include "dbmanager.h"
#include "databasesettingsdialog.h"
#include "newprojectdialog.h"
#include "modifyprojectdialog.h"
#include "paramconfigdialog.h"
#include <QSqlDatabase>
#include <QMessageBox>
#include <QStyle>
#include <QApplication>
#include <QVariantMap>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

// 创建所有 QAction 对象
void MainWindow::createActions()
{
    QStyle *style = QApplication::style();

    // 工具栏动作 — 使用 Qt 内置标准图标
    m_actNewPanel = new QAction(
        style->standardIcon(QStyle::SP_FileDialogNewFolder), "新建面板", this);
    m_actNewPanel->setShortcut(QKeySequence("Ctrl+P"));
    m_actNewPanel->setToolTip("新建面板");

    m_actNewWindow = new QAction(
        style->standardIcon(QStyle::SP_FileDialogListView), "新建窗口", this);
    m_actNewWindow->setShortcut(QKeySequence("Ctrl+N"));
    m_actNewWindow->setToolTip("在当前面板中新建窗口");

    m_actStartTest = new QAction(
        style->standardIcon(QStyle::SP_MediaPlay), "开始测试", this);
    m_actStartTest->setShortcut(QKeySequence("F5"));
    m_actStartTest->setToolTip("开始测试");

    m_actStopTest = new QAction(
        style->standardIcon(QStyle::SP_MediaStop), "停止测试", this);
    m_actStopTest->setShortcut(QKeySequence("Shift+F5"));
    m_actStopTest->setToolTip("停止测试");
    m_actStopTest->setEnabled(false);

    m_actSave = new QAction(
        style->standardIcon(QStyle::SP_DialogSaveButton), "保存工程", this);
    m_actSave->setShortcut(QKeySequence::Save);
    m_actSave->setToolTip("保存工程配置");
    m_actSave->setEnabled(false);

    m_actParamConfig = new QAction(
        style->standardIcon(QStyle::SP_FileDialogDetailedView), "参数配置", this);
    m_actParamConfig->setToolTip("参数配置管理");

    // 文件菜单
    m_actNewProject = new QAction(
        style->standardIcon(QStyle::SP_FileDialogNewFolder), "新建工程...", this);
    m_actOpenProject = new QAction(
        style->standardIcon(QStyle::SP_DialogOpenButton), "打开工程...", this);
    m_actModifyProject = new QAction("修改工程...", this);
    m_actExit = new QAction("退出", this);
    m_actExit->setShortcut(QKeySequence("Ctrl+Q"));

    // 编辑菜单
    m_actUndo = new QAction("撤销", this);
    m_actUndo->setShortcut(QKeySequence::Undo);
    m_actRedo = new QAction("重做", this);
    m_actRedo->setShortcut(QKeySequence::Redo);

    // 视图菜单（控制各 Dock 面板显示/隐藏）
    m_actViewIcdTree = new QAction("ICD接口数据库", this);
    m_actViewIcdTree->setCheckable(true);
    m_actViewIcdTree->setChecked(true);

    m_actViewToolbox = new QAction("控件工具箱", this);
    m_actViewToolbox->setCheckable(true);
    m_actViewToolbox->setChecked(true);

    m_actViewProperty = new QAction("属性编辑面板", this);
    m_actViewProperty->setCheckable(true);
    m_actViewProperty->setChecked(true);

    m_actViewFindResult = new QAction("参数查找结果", this);
    m_actViewFindResult->setCheckable(true);
    m_actViewFindResult->setChecked(true);

    m_actViewLog = new QAction("运行日志", this);
    m_actViewLog->setCheckable(true);
    m_actViewLog->setChecked(true);

    // 设置菜单
    m_actDbSettings = new QAction("数据库基础设置...", this);
    m_actNewConn = new QAction("新建连接...", this);
    m_actOpenConn = new QAction("打开连接", this);
    m_actCloseConn = new QAction("关闭连接", this);
    m_actCloseConn->setEnabled(false);

    // 工程菜单
    m_actProjectSettings = new QAction("工程设置", this);
}

// 新建面板
void MainWindow::onNewPanel()
{
    m_canvasTab->newPanel("面板 " + QString::number(m_canvasTab->panelCount() + 1));
    m_runLogDock->appendLog(LogInfo, "新建面板");
    markModified();
}

// 新建窗口
void MainWindow::onNewWindow()
{
    m_canvasTab->addMdiSubWindow("窗口 " + QString::number(
        m_canvasTab->currentMdiArea() ?
            m_canvasTab->currentMdiArea()->subWindowList().size() + 1 : 1));
    m_runLogDock->appendLog(LogInfo, "新建窗口");
    markModified();
}

// 开始测试
void MainWindow::onStartTest()
{
    m_runLogDock->appendLog(LogInfo, "开始测试...");
    m_actStartTest->setEnabled(false);
    m_actStopTest->setEnabled(true);
}

// 停止测试
void MainWindow::onStopTest()
{
    m_runLogDock->appendLog(LogInfo, "停止测试");
    m_actStartTest->setEnabled(true);
    m_actStopTest->setEnabled(false);
}

// 参数配置
void MainWindow::onParamConfig()
{
    if (!DbManager::instance().isConnected() || !m_projectManager->isProjectOpen()) {
        QMessageBox::warning(this, "提示", "请先连接数据库并打开工程");
        return;
    }
    m_runLogDock->appendLog(LogInfo, "打开参数配置");
    ParamConfigDialog dlg(this);
    dlg.exec();
}

// 新建工程
void MainWindow::onNewProject()
{
    // 先检查数据库连接
    if (!DbManager::instance().isConnected()) {
        QMessageBox::warning(this, "提示",
            "数据库未连接，请先在 设置 > 新建连接 中配置并连接数据库");
        return;
    }

    NewProjectDialog dlg(this);
    int ret = dlg.exec();

    if (ret == NewProjectDialog::ResultCreate) {
        // 新建工程：对话框内部已经创建好了，这里只需更新界面状态
        updateWindowTitle();
        updateConnActions();
        m_actSave->setEnabled(true);
        m_runLogDock->appendLog(LogInfo,
            QString("工程创建成功 [工程名: %1, 数据库: %2]")
                .arg(m_projectManager->currentProjectName())
                .arg(m_projectManager->currentDbName()));
    } else if (ret == NewProjectDialog::ResultOpen) {
        // 打开已有工程
        QString dbName = dlg.dbName();
        if (dbName.isEmpty())
            return;

        QString err;
        if (m_projectManager->openProject(dbName, &err)) {
            updateWindowTitle();
            updateConnActions();
            m_actSave->setEnabled(false); // 打开后默认未修改，保存按钮灰
            m_runLogDock->appendLog(LogInfo,
                QString("工程已打开 [工程名: %1, 数据库: %2]")
                    .arg(m_projectManager->currentProjectName())
                    .arg(m_projectManager->currentDbName()));
        } else {
            QMessageBox::critical(this, "打开工程失败", err);
        }
    }
}

// 打开工程
void MainWindow::onOpenProject()
{
    // 先检查数据库连接
    if (!DbManager::instance().isConnected()) {
        QMessageBox::warning(this, "提示",
            "数据库未连接，请先在 设置 > 打开连接 中连接数据库");
        return;
    }

    NewProjectDialog dlg(this);
    int ret = dlg.exec();

    if (ret == NewProjectDialog::ResultCreate) {
        // 在打开工程界面点了新建工程（对话框内部已创建）
        updateWindowTitle();
        updateConnActions();
        m_actSave->setEnabled(true);
        m_runLogDock->appendLog(LogInfo,
            QString("工程创建成功 [工程名: %1, 数据库: %2]")
                .arg(m_projectManager->currentProjectName())
                .arg(m_projectManager->currentDbName()));
    } else if (ret == NewProjectDialog::ResultOpen) {
        // 打开已有工程
        QString dbName = dlg.dbName();
        if (dbName.isEmpty())
            return;

        QString err;
        if (m_projectManager->openProject(dbName, &err)) {
            updateWindowTitle();
            updateConnActions();
            m_actSave->setEnabled(false);
            m_runLogDock->appendLog(LogInfo,
                QString("工程已打开 [工程名: %1, 数据库: %2]")
                    .arg(m_projectManager->currentProjectName())
                    .arg(m_projectManager->currentDbName()));
        } else {
            QMessageBox::critical(this, "打开工程失败", err);
        }
    }
}

// 保存工程
void MainWindow::onSaveProject()
{
    if (!m_projectManager->isProjectOpen()) {
        QMessageBox::warning(this, "提示", "请先打开或新建工程");
        return;
    }

    // 收集当前界面配置
    QVariantMap configData;

    // 保存面板数量
    configData["panel_count"] = m_canvasTab->panelCount();

    // 保存当前面板索引
    configData["current_panel_index"] = m_canvasTab->currentIndex();

    // 保存每个面板的窗口数量和位置
    for (int i = 0; i < m_canvasTab->panelCount(); ++i) {
        QMdiArea *mdi = m_canvasTab->mdiArea(i);
        if (!mdi) continue;

        QJsonArray windows;
        QList<QMdiSubWindow *> subWindows = mdi->subWindowList();
        for (QMdiSubWindow *sub : subWindows) {
            QJsonObject win;
            win["title"] = sub->windowTitle();
            win["x"] = sub->x();
            win["y"] = sub->y();
            win["width"] = sub->width();
            win["height"] = sub->height();
            win["isMaximized"] = sub->isMaximized();
            windows.append(win);
        }

        configData[QString("panel_%1_windows").arg(i)] =
            QString::fromUtf8(QJsonDocument(windows).toJson(QJsonDocument::Compact));
    }

    // 保存 Dock 面板可见状态
    QJsonObject dockState;
    dockState["icd_tree_visible"] = m_icdTreeViewDock->isVisible();
    dockState["toolbox_visible"] = m_controlToolBoxDock->isVisible();
    dockState["property_visible"] = m_propertyPanelDock->isVisible();
    dockState["find_result_visible"] = m_findResultDock->isVisible();
    dockState["log_visible"] = m_runLogDock->isVisible();
    configData["dock_visibility"] =
        QString::fromUtf8(QJsonDocument(dockState).toJson(QJsonDocument::Compact));

    // 保存窗口几何信息
    QJsonObject geom;
    geom["width"] = width();
    geom["height"] = height();
    geom["x"] = x();
    geom["y"] = y();
    geom["isMaximized"] = isMaximized();
    configData["window_geometry"] =
        QString::fromUtf8(QJsonDocument(geom).toJson(QJsonDocument::Compact));

    // 保存到数据库
    QString err;
    if (m_projectManager->saveProject(configData, &err)) {
        m_runLogDock->appendLog(LogInfo, "工程保存成功");
        clearModified();
    } else {
        m_runLogDock->appendLog(LogError, "工程保存失败: " + err);
        QMessageBox::critical(this, "保存失败", err);
    }
}

// 修改工程
void MainWindow::onModifyProject()
{
    if (!m_projectManager->isProjectOpen()) {
        QMessageBox::warning(this, "提示", "请先打开工程");
        return;
    }

    ModifyProjectDialog dlg(
        m_projectManager->currentProjectName(),
        m_projectManager->currentDescription(),
        this);

    if (dlg.exec() == QDialog::Accepted) {
        QString err;
        if (m_projectManager->updateProject(
                dlg.projectName(), dlg.description(), &err)) {
            updateWindowTitle();
            m_runLogDock->appendLog(LogInfo, "工程信息已更新");
        } else {
            QMessageBox::critical(this, "修改失败", err);
        }
    }
}

// 数据库基础设置
void MainWindow::onDbSettings()
{
    DatabaseSettingsDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        // 保存后重新连接
        DbManager &db = DbManager::instance();
        DbConfig cfg = db.loadConfig();
        QString err;
        if (db.connectToServer(cfg.host, cfg.port, cfg.userName, cfg.password, &err)) {
            m_runLogDock->appendLog(LogInfo,
                QString("数据库连接成功 [%1@%2:%3]")
                    .arg(cfg.userName).arg(cfg.host).arg(cfg.port));
        } else {
            m_runLogDock->appendLog(LogError, "数据库连接失败: " + err);
        }
        updateConnActions();
        updateDbStatus();
    }
}

// 新建连接
void MainWindow::onNewConnection()
{
    DatabaseSettingsDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        DbManager &db = DbManager::instance();
        DbConfig cfg = db.loadConfig();
        QString err;
        if (db.connectToServer(cfg.host, cfg.port, cfg.userName, cfg.password, &err)) {
            m_runLogDock->appendLog(LogInfo,
                QString("新建连接成功 [连接名: %1, %2@%3:%4]")
                    .arg(cfg.connectionName)
                    .arg(cfg.userName).arg(cfg.host).arg(cfg.port));
        } else {
            m_runLogDock->appendLog(LogError, "连接失败: " + err);
        }
        updateConnActions();
        updateDbStatus();
    }
}

// 打开连接
void MainWindow::onOpenConnection()
{
    DbManager &db = DbManager::instance();
    if (!db.hasLocalConfig()) {
        QMessageBox::information(this, "提示",
            "没有找到本地数据库配置，请先点击 \"新建连接\" 进行配置");
        return;
    }

    DbConfig cfg = db.loadConfig();
    QString err;
    if (db.connectToServer(cfg.host, cfg.port, cfg.userName, cfg.password, &err)) {
        m_runLogDock->appendLog(LogInfo,
            QString("连接已打开 [%1@%2:%3]")
                .arg(cfg.userName).arg(cfg.host).arg(cfg.port));
    } else {
        m_runLogDock->appendLog(LogError, "连接失败: " + err);
    }
    updateConnActions();
    updateDbStatus();
}

// 关闭连接
void MainWindow::onCloseConnection()
{
    DbManager &db = DbManager::instance();
    QString name = db.loadConfig().connectionName;
    db.disconnect();

    m_runLogDock->appendLog(LogInfo,
        QString("连接已关闭 [连接名: %1]").arg(name));

    // 如果有打开的工程，同步关闭
    if (m_projectManager->isProjectOpen()) {
        m_projectManager->closeProject();
        updateWindowTitle();
        m_actSave->setEnabled(false);
    }

    updateConnActions();
    updateDbStatus();
}

// 退出
void MainWindow::onExit()
{
    close();
}

// 视图切换 - ICD树
void MainWindow::onViewIcdTree(bool visible)
{
    m_icdTreeViewDock->setVisible(visible);
    markModified();
}

// 视图切换 - 控件工具箱
void MainWindow::onViewToolbox(bool visible)
{
    m_controlToolBoxDock->setVisible(visible);
    markModified();
}

// 视图切换 - 属性面板
void MainWindow::onViewProperty(bool visible)
{
    m_propertyPanelDock->setVisible(visible);
    markModified();
}

// 视图切换 - 参数查找结果
void MainWindow::onViewFindResult(bool visible)
{
    m_findResultDock->setVisible(visible);
    markModified();
}

// 视图切换 - 运行日志
void MainWindow::onViewLog(bool visible)
{
    m_runLogDock->setVisible(visible);
    markModified();
}

// 标记修改
void MainWindow::markModified()
{
    if (m_isModified) return;
    m_isModified = true;
    m_actSave->setEnabled(m_projectManager->isProjectOpen());
    updateWindowTitle();
}

// 清除修改标记
void MainWindow::clearModified()
{
    if (!m_isModified) return;
    m_isModified = false;
    m_actSave->setEnabled(false);
    updateWindowTitle();
}

// 更新连接相关菜单项状态
void MainWindow::updateConnActions()
{
    bool connected = DbManager::instance().isConnected();
    bool hasConfig = DbManager::instance().hasLocalConfig();

    m_actCloseConn->setEnabled(connected);
    m_actOpenConn->setEnabled(!connected && hasConfig);

    // 更新视图菜单的勾选状态
    m_actViewIcdTree->setChecked(m_icdTreeViewDock->isVisible());
    m_actViewToolbox->setChecked(m_controlToolBoxDock->isVisible());
    m_actViewProperty->setChecked(m_propertyPanelDock->isVisible());
    m_actViewFindResult->setChecked(m_findResultDock->isVisible());
    m_actViewLog->setChecked(m_runLogDock->isVisible());
}

// 更新窗口标题
void MainWindow::updateWindowTitle()
{
    QString title = "AgentFlow";

    if (m_projectManager->isProjectOpen()) {
        QString name = m_projectManager->currentProjectName();
        QString desc = m_projectManager->currentDescription();
        if (!desc.isEmpty())
            title += QString(" - %1 [%2]").arg(name, desc);
        else
            title += " - " + name;
    }

    if (m_isModified)
        title += " *";

    setWindowTitle(title);
}

// 更新数据库状态灯
void MainWindow::updateDbStatus()
{
    if (!m_statusBar) return;

    DbManager &db = DbManager::instance();
    if (!db.hasLocalConfig()) {
        m_statusBar->setDbStatus(StatusBar::StatusGray, "DB: 未配置");
    } else if (db.isConnected()) {
        DbConfig cfg = db.loadConfig();
        m_statusBar->setDbStatus(StatusBar::StatusGreen,
            QString("DB: %1@%2").arg(cfg.userName).arg(cfg.host));
    } else {
        m_statusBar->setDbStatus(StatusBar::StatusRed, "DB: 连接失败");
    }
}

// 启动时自动连接数据库
void MainWindow::autoConnectDatabase()
{
    DbManager &db = DbManager::instance();

    if (!db.hasLocalConfig()) {
        m_runLogDock->appendLog(LogWarning, "未找到本地数据库配置，请在 设置 > 新建连接 中配置");
        updateDbStatus();
        updateConnActions();
        return;
    }

    // 检查 MySQL 驱动
    if (!QSqlDatabase::isDriverAvailable("QMYSQL")) {
        m_runLogDock->appendLog(LogError,
            "MySQL 驱动不可用 (QMYSQL)。请先编译并安装 qsqlmysql.dll 驱动。");
        updateDbStatus();
        updateConnActions();
        return;
    }

    DbConfig cfg = db.loadConfig();
    m_runLogDock->appendLog(LogInfo,
        QString("正在连接数据库 %1@%2:%3 ...")
            .arg(cfg.userName).arg(cfg.host).arg(cfg.port));

    QString err;
    if (db.connectToServer(cfg.host, cfg.port, cfg.userName, cfg.password, &err)) {
        m_runLogDock->appendLog(LogInfo,
            QString("数据库连接成功 [连接名: %1, 主机: %2:%3, 用户: %4]")
                .arg(cfg.connectionName).arg(cfg.host).arg(cfg.port).arg(cfg.userName));
    } else {
        m_runLogDock->appendLog(LogError, "数据库连接失败: " + err);
    }

    updateDbStatus();
    updateConnActions();
}
