#ifndef MODIFYPROJECTDIALOG_H
#define MODIFYPROJECTDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QLabel>
#include <QPushButton>

class ModifyProjectDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ModifyProjectDialog(const QString &projectName,
                                  const QString &description,
                                  QWidget *parent = nullptr);

    QString projectName() const;
    QString description() const;

private slots:
    void onConfirm();

private:
    QLineEdit *m_editName;
    QTextEdit *m_editDesc;
    QLabel    *m_labelDbName;
};

#endif // MODIFYPROJECTDIALOG_H
