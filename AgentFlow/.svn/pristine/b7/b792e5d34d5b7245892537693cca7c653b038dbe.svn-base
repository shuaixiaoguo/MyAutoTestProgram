#include "modifyprojectdialog.h"
#include <QFormLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>

ModifyProjectDialog::ModifyProjectDialog(const QString &projectName,
                                          const QString &description,
                                          QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("修改工程信息");
    setMinimumSize(420, 250);

    QGroupBox *group = new QGroupBox("工程信息", this);
    QFormLayout *form = new QFormLayout(group);

    m_editName = new QLineEdit(this);
    m_editName->setText(projectName);

    m_editDesc = new QTextEdit(this);
    m_editDesc->setMaximumHeight(80);
    m_editDesc->setText(description);

    m_labelDbName = new QLabel(this);
    m_labelDbName->setStyleSheet("QLabel { color: #666; font-size: 9pt; }");

    form->addRow("工程名称:", m_editName);
    form->addRow("描述:", m_editDesc);

    QPushButton *btnOk = new QPushButton("确认修改", this);
    QPushButton *btnCancel = new QPushButton("取消", this);

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addStretch();
    btnLayout->addWidget(btnOk);
    btnLayout->addWidget(btnCancel);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(group);
    mainLayout->addLayout(btnLayout);

    connect(btnOk, &QPushButton::clicked, this, &ModifyProjectDialog::onConfirm);
    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);
}

QString ModifyProjectDialog::projectName() const
{
    return m_editName->text().trimmed();
}

QString ModifyProjectDialog::description() const
{
    return m_editDesc->toPlainText().trimmed();
}

void ModifyProjectDialog::onConfirm()
{
    if (m_editName->text().trimmed().isEmpty()) {
        return;
    }
    accept();
}
