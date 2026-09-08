#include "statusbar.h"
#include <QStyle>
#include <QApplication>

StatusBar::StatusBar(QWidget *parent)
    : QStatusBar(parent)
{
    initUi();
}

void StatusBar::initUi()
{
    setSizeGripEnabled(false);

    m_msgLabel = new QLabel(" 就绪 ");
    m_msgLabel->setStyleSheet("QLabel { color: #e2e8f0; padding: 0 8px; }");
    addWidget(m_msgLabel);

    QStyle *style = QApplication::style();

    m_btnMode = new QPushButton;
    m_btnMode->setToolTip("脱机模式");
    m_btnMode->setIcon(style->standardIcon(QStyle::SP_ComputerIcon));
    m_btnMode->setFlat(true);
    addPermanentWidget(m_btnMode);

    m_btnAgent = new QPushButton;
    m_btnAgent->setToolTip("Agent: 未知");
    m_btnAgent->setIcon(style->standardIcon(QStyle::SP_DriveHDIcon));
    m_btnAgent->setFlat(true);
    addPermanentWidget(m_btnAgent);

    m_btnDB = new QPushButton;
    m_btnDB->setToolTip("DB: 未配置");
    m_btnDB->setText("●");
    m_btnDB->setStyleSheet(
        "QPushButton { color: #9ca3af; font-size: 14pt; "
        "border: none; padding: 0 4px; }");
    m_btnDB->setFlat(true);
    addPermanentWidget(m_btnDB);

    m_btnServer = new QPushButton;
    m_btnServer->setToolTip("Server: 未启动");
    m_btnServer->setIcon(style->standardIcon(QStyle::SP_ComputerIcon));
    m_btnServer->setFlat(true);
    addPermanentWidget(m_btnServer);

    connect(m_btnMode, &QPushButton::pressed, this, &StatusBar::onButtonPressed);
    connect(m_btnAgent, &QPushButton::pressed, this, &StatusBar::onButtonPressed);
    connect(m_btnDB, &QPushButton::pressed, this, &StatusBar::onButtonPressed);
    connect(m_btnServer, &QPushButton::pressed, this, &StatusBar::onButtonPressed);
}

void StatusBar::setMessage(const QString &msg)
{
    m_msgLabel->setText(" " + msg + " ");
}

void StatusBar::updateButtonIcon(QPushButton *btn, bool ok, const QString &label)
{
    QStyle *style = QApplication::style();
    if (ok) {
        btn->setIcon(style->standardIcon(QStyle::SP_DialogApplyButton));
        btn->setToolTip(label + ": 正常");
    } else {
        btn->setIcon(style->standardIcon(QStyle::SP_DialogCancelButton));
        btn->setToolTip(label + ": 异常");
    }
}

void StatusBar::updateStatus(const QStringList &status)
{
    QStyle *style = QApplication::style();

    if (status.size() > 0) {
        m_btnMode->setToolTip(status[0]);
        m_btnMode->setIcon(style->standardIcon(
            status[0].contains("联机") ? QStyle::SP_MediaPlay
                                       : QStyle::SP_MediaStop));
    }
    if (status.size() > 1) {
        updateButtonIcon(m_btnAgent,
                        status[1].contains("正常"), "Agent");
    }
    if (status.size() > 2) {
        updateButtonIcon(m_btnDB,
                        status[2].contains("正常"), "DB");
    }
    if (status.size() > 3) {
        updateButtonIcon(m_btnServer,
                        status[3].contains("正常"), "Server");
    }
}

void StatusBar::onButtonPressed()
{
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (btn)
        setMessage(btn->toolTip());
}

void StatusBar::setDbStatus(int state, const QString &text)
{
    QString color;
    switch (state) {
    case 0:  color = "#9ca3af"; break;  // 灰色
    case 1:  color = "#16a34a"; break;  // 绿色
    case 2:  color = "#dc2626"; break;  // 红色
    default: color = "#9ca3af"; break;
    }
    m_btnDB->setStyleSheet(
        QString("QPushButton { color: %1; font-size: 14pt; "
                "border: none; padding: 0 4px; }").arg(color));
    m_btnDB->setToolTip(text);
}
