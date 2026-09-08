#include "newprojectdialog.h"
#include "Database/dbmanager.h"
#include "Database/projectmanager.h"
#include "basetableview.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QSqlQuery>
#include <QMenu>
#include <QMessageBox>
#include <QApplication>
#include <QStyle>
#include <QLineEdit>
#include <QTextEdit>
#include <QFormLayout>
#include <QDialogButtonBox>

NewProjectDialog::NewProjectDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("打开工程");
    setMinimumSize(640, 520);

    // --- 已有工程列表 ---
    QGroupBox *openGroup = new QGroupBox("工程列表", this);
    QVBoxLayout *openLayout = new QVBoxLayout(openGroup);

    // 工具栏：刷新 + 删除工程
    QHBoxLayout *toolBarLayout = new QHBoxLayout;
    QPushButton *btnRefresh = new QPushButton("刷新", this);
    btnRefresh->setIcon(QApplication::style()->standardIcon(QStyle::SP_BrowserReload));
    QPushButton *btnDelete = new QPushButton("删除工程", this);
    btnDelete->setIcon(QApplication::style()->standardIcon(QStyle::SP_TrashIcon));

    toolBarLayout->addWidget(btnRefresh);
    toolBarLayout->addWidget(btnDelete);
    toolBarLayout->addStretch();

    openLayout->addLayout(toolBarLayout);

    m_projectTable = new BaseTableView(this);
    m_projectTable->addColumn("数据库名", 200);
    m_projectTable->addColumn("工程名称", 200);
    m_projectTable->addColumn("描述", 200);
    m_projectTable->setRowHeight(28);
    m_projectTable->setMinimumHeight(200);
    // 单双行交替颜色（斑马纹，防止看串行）
    m_projectTable->setRowColors(QColor("#ffffff"), QColor("#f5f7fa"));
    m_projectTable->setSelectedColor(QColor("#1677ff"), QColor("#ffffff"));

    openLayout->addWidget(m_projectTable, 1);

    // --- 状态标签 ---
    m_labelStatus = new QLabel(this);
    m_labelStatus->setStyleSheet("QLabel { font-size: 9pt; }");

    // --- 底部按钮 ---
    m_btnNew = new QPushButton("新建工程", this);
    m_btnNew->setIcon(QApplication::style()->standardIcon(QStyle::SP_FileDialogNewFolder));
    m_btnCancel = new QPushButton("取消", this);
    m_btnCancel->setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogCancelButton));
    m_btnOpen = new QPushButton("打开", this);
    m_btnOpen->setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogOpenButton));
    m_btnOpen->setDefault(true);

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addWidget(m_btnNew);
    btnLayout->addStretch();
    btnLayout->addWidget(m_btnCancel);
    btnLayout->addWidget(m_btnOpen);

    // --- 主布局 ---
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(openGroup, 1);
    mainLayout->addWidget(m_labelStatus);
    mainLayout->addLayout(btnLayout);

    // 加载已有工程列表
    refreshProjectList();

    connect(m_btnNew, &QPushButton::clicked, this, &NewProjectDialog::onNewProject);
    connect(m_btnOpen, &QPushButton::clicked, this, &NewProjectDialog::onOpenProject);
    connect(m_btnCancel, &QPushButton::clicked, this, &NewProjectDialog::onCancel);
    connect(btnRefresh, &QPushButton::clicked, this, &NewProjectDialog::onRefreshList);
    connect(btnDelete, &QPushButton::clicked, this, &NewProjectDialog::onDeleteProject);
    connect(m_projectTable, &BaseTableView::rowDoubleClicked,
            this, &NewProjectDialog::onTableDoubleClicked);
    connect(m_projectTable, &BaseTableView::rowRightClicked,
            this, &NewProjectDialog::onTableRightClicked);
}

QString NewProjectDialog::projectName() const
{
    return m_projectName;
}

QString NewProjectDialog::description() const
{
    return m_description;
}

QString NewProjectDialog::dbName() const
{
    return m_dbName;
}

void NewProjectDialog::refreshProjectList()
{
    m_projectTable->clear();
    QString err;
    QStringList projects = ProjectManager().listProjects(&err);
    if (!err.isEmpty()) {
        m_labelStatus->setStyleSheet("QLabel { color: #dc2626; font-size: 9pt; }");
        m_labelStatus->setText("获取工程列表失败: " + err);
        return;
    }

    DbManager &db = DbManager::instance();
    DbConfig cfg = db.loadConfig();

    for (const QString &dbName : projects) {
        QString projName;
        QString projDesc;

        // 临时连接到该库，读取工程名和描述
        DbConfig projCfg = cfg;
        projCfg.dbName = dbName;

        if (db.connectToDatabase(projCfg, nullptr)) {
            QSqlQuery query(db.database());
            query.prepare("SELECT config_key, config_value FROM ui_config "
                          "WHERE config_group = 'project' "
                          "AND config_key IN ('project_name', 'description')");
            if (query.exec()) {
                while (query.next()) {
                    QString key = query.value(0).toString();
                    QString val = query.value(1).toString();
                    if (key == "project_name") projName = val;
                    else if (key == "description") projDesc = val;
                }
            }
        }

        QVector<QVariant> row;
        row << dbName
            << (projName.isEmpty() ? dbName : projName)
            << (projDesc.isEmpty() ? "无描述" : projDesc);

        m_projectTable->appendRow(row);
        m_projectTable->setRowUserData(m_projectTable->rowCount() - 1, dbName);
    }

    m_labelStatus->setStyleSheet("QLabel { color: #6b7280; font-size: 9pt; }");
    m_labelStatus->setText(QString("共 %1 个工程").arg(m_projectTable->rowCount()));
}

void NewProjectDialog::onNewProject()
{
    // 弹出新建工程对话框
    QDialog dlg(this);
    dlg.setWindowTitle("新建工程");
    dlg.setMinimumWidth(420);

    QFormLayout *form = new QFormLayout(&dlg);
    QLineEdit *editName = new QLineEdit(&dlg);
    editName->setPlaceholderText("如：测试流程A");
    QLineEdit *editDbName = new QLineEdit(&dlg);
    editDbName->setPlaceholderText("如：test_flow_a");
    QTextEdit *editDesc = new QTextEdit(&dlg);
    editDesc->setMaximumHeight(60);
    editDesc->setPlaceholderText("工程描述（可选）");

    form->addRow("工程名称:", editName);
    form->addRow("数据库名:", editDbName);
    form->addRow("描述:", editDesc);

    QDialogButtonBox *btnBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    btnBox->button(QDialogButtonBox::Ok)->setText("确定");
    btnBox->button(QDialogButtonBox::Cancel)->setText("取消");
    form->addRow(btnBox);

    connect(btnBox, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(btnBox, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() != QDialog::Accepted)
        return;

    QString name = editName->text().trimmed();
    QString dbName = editDbName->text().trimmed();
    QString desc = editDesc->toPlainText().trimmed();

    if (name.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入工程名称");
        return;
    }
    if (dbName.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入数据库名");
        return;
    }

    // 直接在对话框里创建工程
    ProjectManager pm;
    QString err;
    if (!pm.createProject(name, dbName, desc, &err)) {
        QMessageBox::critical(this, "新建失败", "创建工程失败:\n" + err);
        return;
    }

    // 记录结果
    m_projectName = name;
    m_dbName = dbName;
    m_description = desc;

    // 刷新列表
    refreshProjectList();

    // 选中新建的工程
    int row = m_projectTable->findRow(0, dbName);
    if (row >= 0)
        m_projectTable->setCurrentRow(row);

    m_labelStatus->setStyleSheet("QLabel { color: #16a34a; font-size: 9pt; }");
    m_labelStatus->setText(QString("工程 \"%1\" 创建成功").arg(name));
}

void NewProjectDialog::onOpenProject()
{
    int row = m_projectTable->currentRow();
    if (row < 0) {
        m_labelStatus->setStyleSheet("QLabel { color: #dc2626; font-size: 9pt; }");
        m_labelStatus->setText("请选择一个工程");
        return;
    }

    m_dbName = m_projectTable->rowUserData(row).toString();
    m_projectName = m_projectTable->data(row, 1).toString();
    done(ResultOpen);
}

void NewProjectDialog::onCancel()
{
    reject();
}

void NewProjectDialog::onTableDoubleClicked(int row)
{
    Q_UNUSED(row)
    onOpenProject();
}

void NewProjectDialog::onTableRightClicked(int row, const QPoint &globalPos)
{
    Q_UNUSED(row)
    QMenu menu(this);
    QAction *actNew = menu.addAction("新建工程");
    actNew->setIcon(QApplication::style()->standardIcon(QStyle::SP_FileDialogNewFolder));
    menu.addSeparator();
    QAction *actDelete = menu.addAction("删除工程");
    actDelete->setIcon(QApplication::style()->standardIcon(QStyle::SP_TrashIcon));

    QAction *selected = menu.exec(globalPos);
    if (selected == actNew) {
        onNewProject();
    } else if (selected == actDelete) {
        onDeleteProject();
    }
}

void NewProjectDialog::onDeleteProject()
{
    int row = m_projectTable->currentRow();
    if (row < 0) {
        m_labelStatus->setStyleSheet("QLabel { color: #dc2626; font-size: 9pt; }");
        m_labelStatus->setText("请先选择要删除的工程");
        return;
    }

    QString dbName = m_projectTable->rowUserData(row).toString();
    QString projName = m_projectTable->data(row, 1).toString();

    // 二次确认
    QString msg = QString("确定要删除工程 \"%1\" 吗？\n\n"
                          "数据库名: %2\n\n"
                          "删除后数据将永久丢失，无法恢复！")
                      .arg(projName, dbName);

    QMessageBox::StandardButton ret = QMessageBox::warning(
        this, "确认删除", msg,
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No);

    if (ret != QMessageBox::Yes)
        return;

    // 执行删除
    ProjectManager pm;
    QString err;
    if (!pm.deleteProject(dbName, &err)) {
        m_labelStatus->setStyleSheet("QLabel { color: #dc2626; font-size: 9pt; }");
        m_labelStatus->setText("删除失败: " + err);
        return;
    }

    m_labelStatus->setStyleSheet("QLabel { color: #16a34a; font-size: 9pt; }");
    m_labelStatus->setText(QString("工程 \"%1\" 已删除").arg(projName));

    // 刷新列表
    refreshProjectList();
}

void NewProjectDialog::onRefreshList()
{
    refreshProjectList();
}
