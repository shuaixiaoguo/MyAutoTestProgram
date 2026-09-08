#ifndef NEWPROJECTDIALOG_H
#define NEWPROJECTDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QLabel>
#include <QPushButton>

class BaseTableView;

class NewProjectDialog : public QDialog
{
    Q_OBJECT
public:
    explicit NewProjectDialog(QWidget *parent = nullptr);

    QString projectName() const;
    QString description() const;
    QString dbName() const;

    // 结果码
    enum ResultCode {
        ResultCreate = QDialog::Accepted,       // 新建工程
        ResultOpen   = QDialog::Accepted + 1    // 打开工程
    };

    void refreshProjectList();

private slots:
    void onNewProject();
    void onOpenProject();
    void onCancel();
    void onTableDoubleClicked(int row);
    void onTableRightClicked(int row, const QPoint &globalPos);
    void onDeleteProject();
    void onRefreshList();

private:
    BaseTableView *m_projectTable;
    QLabel      *m_labelStatus;
    QPushButton *m_btnNew;
    QPushButton *m_btnOpen;
    QPushButton *m_btnCancel;

    QString m_projectName;
    QString m_dbName;
    QString m_description;
};

#endif // NEWPROJECTDIALOG_H
