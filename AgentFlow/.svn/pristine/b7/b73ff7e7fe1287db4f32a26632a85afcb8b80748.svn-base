#include "databasesettingsdialog.h"
#include "Database/dbmanager.h"
#include <QFormLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSettings>

DatabaseSettingsDialog::DatabaseSettingsDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("数据库基础设置");
    setMinimumWidth(400);

    QFormLayout *formLayout = new QFormLayout;

    m_editConnName = new QLineEdit(this);
    m_editConnName->setPlaceholderText("如: sql、生产环境、测试环境");

    m_editHost = new QLineEdit(this);
    m_editHost->setPlaceholderText("127.0.0.1");

    m_spinPort = new QSpinBox(this);
    m_spinPort->setRange(1, 65535);
    m_spinPort->setValue(3306);

    m_editUser = new QLineEdit(this);
    m_editUser->setPlaceholderText("root");

    m_editPassword = new QLineEdit(this);
    m_editPassword->setEchoMode(QLineEdit::Password);
    m_editPassword->setPlaceholderText("输入密码");

    formLayout->addRow("连接名:", m_editConnName);
    formLayout->addRow("主机地址:", m_editHost);
    formLayout->addRow("端口:", m_spinPort);
    formLayout->addRow("用户名:", m_editUser);
    formLayout->addRow("密码:", m_editPassword);

    // 状态灯 + 状态文字
    m_labelStatusLight = new QLabel(this);
    m_labelStatusLight->setFixedSize(16, 16);
    m_labelStatusLight->setText("●");
    m_labelStatusLight->setStyleSheet("QLabel { font-size: 16pt; color: #9ca3af; }");

    m_labelStatus = new QLabel(this);
    m_labelStatus->setStyleSheet("QLabel { font-size: 9pt; }");

    QHBoxLayout *statusLayout = new QHBoxLayout;
    statusLayout->addWidget(m_labelStatusLight);
    statusLayout->addWidget(m_labelStatus);
    statusLayout->addStretch();

    m_btnTest = new QPushButton("测试连接", this);
    m_btnSave = new QPushButton("保存", this);
    m_btnCancel = new QPushButton("取消", this);

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addStretch();
    btnLayout->addWidget(m_btnTest);
    btnLayout->addWidget(m_btnSave);
    btnLayout->addWidget(m_btnCancel);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(statusLayout);
    mainLayout->addLayout(btnLayout);

    loadSettings();

    // 初始状态灯
    if (!DbManager::instance().hasLocalConfig()) {
        updateStatusLight(0);
        m_labelStatus->setText("本地无配置，请填写后保存");
    } else if (DbManager::instance().isConnected()) {
        updateStatusLight(1);
        m_labelStatus->setText("数据库已连接");
    } else {
        updateStatusLight(0);
        m_labelStatus->setText("未连接");
    }

    connect(m_btnTest, &QPushButton::clicked, this, &DatabaseSettingsDialog::onTestConnection);
    connect(m_btnSave, &QPushButton::clicked, this, &DatabaseSettingsDialog::onSave);
    connect(m_btnCancel, &QPushButton::clicked, this, &QDialog::reject);
}

QString DatabaseSettingsDialog::connectionName() const
{
    return m_editConnName->text().trimmed();
}

QString DatabaseSettingsDialog::host() const
{
    return m_editHost->text().trimmed();
}

int DatabaseSettingsDialog::port() const
{
    return m_spinPort->value();
}

QString DatabaseSettingsDialog::userName() const
{
    return m_editUser->text().trimmed();
}

QString DatabaseSettingsDialog::password() const
{
    return m_editPassword->text();
}

void DatabaseSettingsDialog::loadSettings()
{
    DbConfig cfg = DbManager::instance().loadConfig();
    m_editConnName->setText(cfg.connectionName);
    m_editHost->setText(cfg.host);
    m_spinPort->setValue(cfg.port);
    m_editUser->setText(cfg.userName);
    m_editPassword->setText(cfg.password);
}

void DatabaseSettingsDialog::updateStatusLight(int state)
{
    QString color;
    switch (state) {
    case 0:  color = "#9ca3af"; break;  // 灰色
    case 1:  color = "#16a34a"; break;  // 绿色
    case 2:  color = "#dc2626"; break;  // 红色
    default: color = "#9ca3af"; break;
    }
    m_labelStatusLight->setStyleSheet(
        QString("QLabel { font-size: 16pt; color: %1; }").arg(color));
}

void DatabaseSettingsDialog::onTestConnection()
{
    m_labelStatus->setStyleSheet("QLabel { color: #2563eb; font-size: 9pt; }");
    m_labelStatus->setText("正在测试连接...");
    updateStatusLight(0);

    // 先保存配置，确保后续操作能用
    DbConfig cfg;
    cfg.connectionName = connectionName();
    cfg.host = host();
    cfg.port = port();
    cfg.userName = userName();
    cfg.password = password();
    DbManager::instance().saveConfig(cfg);

    QString err;
    bool ok = DbManager::instance().connectToServer(
        host(), port(), userName(), password(), &err);

    if (ok) {
        updateStatusLight(1);
        m_labelStatus->setStyleSheet("QLabel { color: #16a34a; font-size: 9pt; }");
        m_labelStatus->setText("连接成功!");
        DbManager::instance().disconnect();
    } else {
        updateStatusLight(2);
        m_labelStatus->setStyleSheet("QLabel { color: #dc2626; font-size: 9pt; }");
        m_labelStatus->setText("连接失败: " + err);
    }
}

void DatabaseSettingsDialog::onSave()
{
    DbConfig cfg;
    cfg.connectionName = connectionName();
    cfg.host = host();
    cfg.port = port();
    cfg.userName = userName();
    cfg.password = password();
    DbManager::instance().saveConfig(cfg);
    accept();
}
