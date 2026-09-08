#ifndef PARAMPROPERTYPANEL_H
#define PARAMPROPERTYPANEL_H

#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QTextEdit>
#include <QTableWidget>
#include <QLabel>
#include <QFormLayout>
#include <QPushButton>

struct ParamParameter;

class ParamPropertyPanel : public QWidget
{
    Q_OBJECT
public:
    explicit ParamPropertyPanel(QWidget *parent = nullptr);

    // 加载参数到表单
    void loadParameter(const ParamParameter &param);
    // 清空表单
    void clearForm();

signals:
    void parameterChanged(int paramId);

private slots:
    void onSaveClicked();
    void onAddEnumItem();
    void onRemoveEnumItem();

private:
    // 参数ID
    int m_paramId;

    // 表单控件
    QLineEdit   *m_editName;
    QLineEdit   *m_editCode;
    QComboBox   *m_cmbType;
    QSpinBox    *m_spinStartBit;
    QSpinBox    *m_spinBitWidth;
    QComboBox   *m_cmbByteOrder;
    QLineEdit   *m_editUnit;
    QSpinBox    *m_spinPrecision;
    QLineEdit   *m_editDefault;
    QLineEdit   *m_editMin;
    QLineEdit   *m_editMax;
    QTextEdit   *m_editFormula;
    QTableWidget *m_tableEnum;
    QTextEdit   *m_editDesc;
    QPushButton *m_btnSave;
    QPushButton *m_btnAddEnum;
    QPushButton *m_btnRemoveEnum;

    // 构建表单
    void buildForm();

    // 从表单读取数据
    ParamParameter getFromForm() const;
};

#endif // PARAMPROPERTYPANEL_H
