#ifndef DATABASESETTINGSDIALOG_H
#define DATABASESETTINGSDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QSpinBox>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>

class DatabaseSettingsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit DatabaseSettingsDialog(QWidget *parent = nullptr);

    QString connectionName() const;
    QString host() const;
    int port() const;
    QString userName() const;
    QString password() const;

private slots:
    void onTestConnection();
    void onSave();

private:
    void loadSettings();
    void updateStatusLight(int state); // 0=灰 1=绿 2=红

    QLineEdit   *m_editConnName;
    QLineEdit   *m_editHost;
    QSpinBox    *m_spinPort;
    QLineEdit   *m_editUser;
    QLineEdit   *m_editPassword;
    QLabel      *m_labelStatusLight;  // 状态灯
    QLabel      *m_labelStatus;       // 状态文字
    QPushButton *m_btnTest;
    QPushButton *m_btnSave;
    QPushButton *m_btnCancel;
};

#endif // DATABASESETTINGSDIALOG_H
