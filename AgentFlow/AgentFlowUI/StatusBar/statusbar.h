#ifndef STATUSBAR_H
#define STATUSBAR_H

#include <QStatusBar>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QQueue>
#include <QStringList>

class StatusBar : public QStatusBar
{
    Q_OBJECT
public:
    explicit StatusBar(QWidget *parent = nullptr);

    // 状态灯颜色枚举
    enum StatusColor {
        StatusGray  = 0,
        StatusGreen = 1,
        StatusRed   = 2
    };

    void setMessage(const QString &msg);
    void updateStatus(const QStringList &status);
    void updateButtonIcon(QPushButton *btn, bool ok, const QString &label);
    // 设置DB状态灯
    void setDbStatus(int state, const QString &text);

private:
    void initUi();

    QLabel      *m_msgLabel;
    QPushButton *m_btnMode;
    QPushButton *m_btnAgent;
    QPushButton *m_btnDB;
    QPushButton *m_btnServer;

private slots:
    void onButtonPressed();
};

#endif // STATUSBAR_H
