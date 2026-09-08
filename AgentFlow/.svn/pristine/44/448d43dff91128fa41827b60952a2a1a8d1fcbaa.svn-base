#include "parampropertypanel.h"
#include "paramdatabase.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QHeaderView>
#include <QScrollArea>

ParamPropertyPanel::ParamPropertyPanel(QWidget *parent)
    : QWidget(parent)
    , m_paramId(0)
{
    buildForm();
    clearForm();
}

void ParamPropertyPanel::buildForm()
{
    // 滚动区域（属性多，支持滚动）
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    QWidget *content = new QWidget(scrollArea);
    QVBoxLayout *contentLayout = new QVBoxLayout(content);
    contentLayout->setContentsMargins(8, 8, 8, 8);
    contentLayout->setSpacing(8);

    // --- 基本信息 ---
    QGroupBox *boxBasic = new QGroupBox("基本信息", content);
    QFormLayout *formBasic = new QFormLayout(boxBasic);
    formBasic->setSpacing(6);

    // 节点类型（只读）
    QLineEdit *editNodeType = new QLineEdit("参数", boxBasic);
    editNodeType->setReadOnly(true);
    editNodeType->setStyleSheet("QLineEdit { color: #666; background: #f0f0f0; }");

    m_editName = new QLineEdit(boxBasic);
    m_editCode = new QLineEdit(boxBasic);
    m_cmbType = new QComboBox(boxBasic);
    m_cmbType->addItems({"uint8", "uint16", "uint32", "uint64",
                         "int8", "int16", "int32", "int64",
                         "float", "double", "bool", "enum"});

    formBasic->addRow("节点类型:", editNodeType);
    formBasic->addRow("参数名称:", m_editName);
    formBasic->addRow("参数代号:", m_editCode);
    formBasic->addRow("参数类型:", m_cmbType);

    contentLayout->addWidget(boxBasic);

    // --- 位信息 ---
    QGroupBox *boxBit = new QGroupBox("位信息", content);
    QFormLayout *formBit = new QFormLayout(boxBit);
    formBit->setSpacing(6);

    m_spinStartBit = new QSpinBox(boxBit);
    m_spinStartBit->setRange(0, 10000);
    m_spinBitWidth = new QSpinBox(boxBit);
    m_spinBitWidth->setRange(1, 1024);
    m_cmbByteOrder = new QComboBox(boxBit);
    m_cmbByteOrder->addItem("大端 (Big Endian)", 1);
    m_cmbByteOrder->addItem("小端 (Little Endian)", 2);

    formBit->addRow("起始位:", m_spinStartBit);
    formBit->addRow("位宽:", m_spinBitWidth);
    formBit->addRow("字节序:", m_cmbByteOrder);

    contentLayout->addWidget(boxBit);

    // --- 数值属性 ---
    QGroupBox *boxValue = new QGroupBox("数值属性", content);
    QFormLayout *formValue = new QFormLayout(boxValue);
    formValue->setSpacing(6);

    m_editUnit = new QLineEdit(boxValue);
    m_spinPrecision = new QSpinBox(boxValue);
    m_spinPrecision->setRange(0, 10);
    m_editDefault = new QLineEdit(boxValue);
    m_editMin = new QLineEdit(boxValue);
    m_editMax = new QLineEdit(boxValue);

    formValue->addRow("单位:", m_editUnit);
    formValue->addRow("精度(小数位):", m_spinPrecision);
    formValue->addRow("默认值:", m_editDefault);
    formValue->addRow("最小值:", m_editMin);
    formValue->addRow("最大值:", m_editMax);

    contentLayout->addWidget(boxValue);

    // --- 计算公式 ---
    QGroupBox *boxFormula = new QGroupBox("计算公式", content);
    QVBoxLayout *formulaLayout = new QVBoxLayout(boxFormula);
    m_editFormula = new QTextEdit(boxFormula);
    m_editFormula->setMaximumHeight(60);
    m_editFormula->setPlaceholderText("如：raw * 0.1 + 273.15");
    formulaLayout->addWidget(m_editFormula);
    contentLayout->addWidget(boxFormula);

    // --- 枚举定义 ---
    QGroupBox *boxEnum = new QGroupBox("枚举定义", content);
    QVBoxLayout *enumLayout = new QVBoxLayout(boxEnum);

    m_tableEnum = new QTableWidget(0, 2, boxEnum);
    m_tableEnum->setHorizontalHeaderLabels({"值", "描述"});
    m_tableEnum->horizontalHeader()->setStretchLastSection(true);
    m_tableEnum->verticalHeader()->setVisible(false);
    m_tableEnum->setMaximumHeight(120);

    QHBoxLayout *enumBtnLayout = new QHBoxLayout;
    m_btnAddEnum = new QPushButton("添加", boxEnum);
    m_btnRemoveEnum = new QPushButton("删除", boxEnum);
    enumBtnLayout->addWidget(m_btnAddEnum);
    enumBtnLayout->addWidget(m_btnRemoveEnum);
    enumBtnLayout->addStretch();

    enumLayout->addWidget(m_tableEnum);
    enumLayout->addLayout(enumBtnLayout);
    contentLayout->addWidget(boxEnum);

    // --- 描述 ---
    QGroupBox *boxDesc = new QGroupBox("描述", content);
    QVBoxLayout *descLayout = new QVBoxLayout(boxDesc);
    m_editDesc = new QTextEdit(boxDesc);
    m_editDesc->setMaximumHeight(60);
    descLayout->addWidget(m_editDesc);
    contentLayout->addWidget(boxDesc);

    // --- 保存按钮 ---
    m_btnSave = new QPushButton("保存修改", content);
    m_btnSave->setMinimumHeight(32);
    contentLayout->addWidget(m_btnSave);

    contentLayout->addStretch();

    scrollArea->setWidget(content);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(scrollArea);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    connect(m_btnSave, &QPushButton::clicked,
            this, &ParamPropertyPanel::onSaveClicked);
    connect(m_btnAddEnum, &QPushButton::clicked,
            this, &ParamPropertyPanel::onAddEnumItem);
    connect(m_btnRemoveEnum, &QPushButton::clicked,
            this, &ParamPropertyPanel::onRemoveEnumItem);
}

void ParamPropertyPanel::loadParameter(const ParamParameter &param)
{
    m_paramId = param.id;

    m_editName->setText(param.name);
    m_editCode->setText(param.code);

    int typeIdx = m_cmbType->findText(param.paramType);
    m_cmbType->setCurrentIndex(typeIdx >= 0 ? typeIdx : 0);

    m_spinStartBit->setValue(param.startBit);
    m_spinBitWidth->setValue(param.bitWidth);
    m_cmbByteOrder->setCurrentIndex(param.byteOrder == 2 ? 1 : 0);

    m_editUnit->setText(param.unit);
    m_spinPrecision->setValue(param.precision);
    m_editDefault->setText(param.defaultValue);
    m_editMin->setText(param.minValue);
    m_editMax->setText(param.maxValue);

    m_editFormula->setPlainText(param.formula);
    m_editDesc->setPlainText(param.description);

    // 加载枚举
    m_tableEnum->setRowCount(0);
    if (!param.enumOptions.isEmpty()) {
        QJsonDocument doc = QJsonDocument::fromJson(param.enumOptions.toUtf8());
        if (doc.isArray()) {
            QJsonArray arr = doc.array();
            for (const QJsonValue &v : arr) {
                QJsonObject obj = v.toObject();
                int row = m_tableEnum->rowCount();
                m_tableEnum->insertRow(row);
                m_tableEnum->setItem(row, 0, new QTableWidgetItem(obj["value"].toString()));
                m_tableEnum->setItem(row, 1, new QTableWidgetItem(obj["desc"].toString()));
            }
        }
    }

    m_btnSave->setEnabled(true);
}

void ParamPropertyPanel::clearForm()
{
    m_paramId = 0;

    m_editName->clear();
    m_editCode->clear();
    m_cmbType->setCurrentIndex(0);
    m_spinStartBit->setValue(0);
    m_spinBitWidth->setValue(8);
    m_cmbByteOrder->setCurrentIndex(0);
    m_editUnit->clear();
    m_spinPrecision->setValue(0);
    m_editDefault->clear();
    m_editMin->clear();
    m_editMax->clear();
    m_editFormula->clear();
    m_editDesc->clear();
    m_tableEnum->setRowCount(0);

    m_btnSave->setEnabled(false);
}

ParamParameter ParamPropertyPanel::getFromForm() const
{
    ParamParameter p;
    p.id = m_paramId;
    p.name = m_editName->text().trimmed();
    p.code = m_editCode->text().trimmed();
    p.paramType = m_cmbType->currentText();
    p.startBit = m_spinStartBit->value();
    p.bitWidth = m_spinBitWidth->value();
    p.byteOrder = m_cmbByteOrder->currentData().toInt();
    p.unit = m_editUnit->text().trimmed();
    p.precision = m_spinPrecision->value();
    p.defaultValue = m_editDefault->text().trimmed();
    p.minValue = m_editMin->text().trimmed();
    p.maxValue = m_editMax->text().trimmed();
    p.formula = m_editFormula->toPlainText().trimmed();
    p.description = m_editDesc->toPlainText().trimmed();

    // 枚举转JSON
    QJsonArray arr;
    for (int i = 0; i < m_tableEnum->rowCount(); i++) {
        QJsonObject obj;
        QTableWidgetItem *item0 = m_tableEnum->item(i, 0);
        QTableWidgetItem *item1 = m_tableEnum->item(i, 1);
        obj["value"] = item0 ? item0->text() : "";
        obj["desc"] = item1 ? item1->text() : "";
        arr.append(obj);
    }
    p.enumOptions = QString::fromUtf8(QJsonDocument(arr).toJson(QJsonDocument::Compact));

    return p;
}

void ParamPropertyPanel::onSaveClicked()
{
    if (m_paramId <= 0) return;

    if (m_editName->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "提示", "参数名称不能为空");
        return;
    }
    if (m_editCode->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "提示", "参数代号不能为空");
        return;
    }

    ParamDatabase db;
    QString err;
    ParamParameter param = getFromForm();
    if (db.updateParameter(param, &err)) {
        QMessageBox::information(this, "成功", "参数保存成功");
        emit parameterChanged(m_paramId);
    } else {
        QMessageBox::critical(this, "错误", "保存失败: " + err);
    }
}

void ParamPropertyPanel::onAddEnumItem()
{
    int row = m_tableEnum->rowCount();
    m_tableEnum->insertRow(row);
    m_tableEnum->setItem(row, 0, new QTableWidgetItem("0"));
    m_tableEnum->setItem(row, 1, new QTableWidgetItem("枚举项"));
}

void ParamPropertyPanel::onRemoveEnumItem()
{
    int row = m_tableEnum->currentRow();
    if (row >= 0)
        m_tableEnum->removeRow(row);
}
