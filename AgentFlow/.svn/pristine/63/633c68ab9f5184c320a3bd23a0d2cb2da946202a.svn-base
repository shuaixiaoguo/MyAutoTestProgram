#include "propertypaneldock.h"

PropertyPanelDock::PropertyPanelDock(QWidget *parent)
    : QDockWidget("属性编辑面板", parent)
{
    setObjectName("PropertyPanelDock");
    initUi();
}

void PropertyPanelDock::initUi()
{
    setFeatures(QDockWidget::DockWidgetMovable
              | QDockWidget::DockWidgetFloatable);
    setMinimumWidth(200);

    m_content = new QWidget(this);
    m_mainLayout = new QVBoxLayout(m_content);
    m_mainLayout->setContentsMargins(6, 6, 6, 6);
    m_mainLayout->setSpacing(6);

    m_titleLabel = new QLabel("未选中对象", m_content);
    m_titleLabel->setStyleSheet(
        "QLabel { font-weight: bold; color: #2563eb; "
        "padding: 4px; background: #eff6ff; border-radius: 4px; }");
    m_mainLayout->addWidget(m_titleLabel);

    // 表单容器
    QScrollArea *scrollArea = new QScrollArea(m_content);
    scrollArea->setWidgetResizable(true);
    QWidget *formWidget = new QWidget(scrollArea);
    m_formLayout = new QFormLayout(formWidget);
    m_formLayout->setSpacing(6);
    m_formLayout->setLabelAlignment(Qt::AlignRight);
    scrollArea->setWidget(formWidget);
    m_mainLayout->addWidget(scrollArea, 1);

    m_mainLayout->addStretch();

    setWidget(m_content);
}

void PropertyPanelDock::setTargetObject(const QString &objId, const QString &objType)
{
    m_titleLabel->setText(QString("%1: %2").arg(objType, objId));

    QVariantMap mockData;
    mockData["编码"] = objId;
    mockData["名称"] = objType;
    mockData["类型"] = "模拟量";
    mockData["状态"] = "启用";
    setPropertyData(mockData);
}

void PropertyPanelDock::setPropertyData(const QVariantMap &data)
{
    // 清除旧字段
    QLayoutItem *item;
    while ((item = m_formLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    buildPropertyFields(data);
}

void PropertyPanelDock::buildPropertyFields(const QVariantMap &data)
{
    for (auto it = data.begin(); it != data.end(); ++it) {
        const QString &key = it.key();
        const QVariant &val = it.value();

        QLabel *label = new QLabel(key + ":", this);

        if (val.type() == QVariant::Bool) {
            QComboBox *combo = new QComboBox(this);
            combo->addItem("是", true);
            combo->addItem("否", false);
            combo->setCurrentIndex(val.toBool() ? 0 : 1);
            connect(combo, QOverload<int>::of(&QComboBox::currentIndexChanged),
                [this, key, combo](int) {
                    emit propertyChanged(key, combo->currentData());
                });
            m_formLayout->addRow(label, combo);
        } else {
            QLineEdit *edit = new QLineEdit(val.toString(), this);
            connect(edit, &QLineEdit::textChanged,
                [this, key](const QString &text) {
                    emit propertyChanged(key, text);
                });
            m_formLayout->addRow(label, edit);
        }
    }
}

void PropertyPanelDock::clear()
{
    m_titleLabel->setText("未选中对象");
    QLayoutItem *item;
    while ((item = m_formLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
}

void PropertyPanelDock::onFieldChanged(const QString &propName, const QVariant &value)
{
    emit propertyChanged(propName, value);
}
