#ifndef PROPERTYPANELDOCK_H
#define PROPERTYPANELDOCK_H

#include <QDockWidget>
#include <QScrollArea>
#include <QFormLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QVariant>
#include <QMap>

class PropertyPanelDock : public QDockWidget
{
    Q_OBJECT
public:
    explicit PropertyPanelDock(QWidget *parent = nullptr);

    void setTargetObject(const QString &objId, const QString &objType);
    void setPropertyData(const QVariantMap &data);
    void clear();

private:
    QWidget *m_content;
    QVBoxLayout *m_mainLayout;
    QLabel *m_titleLabel;
    QFormLayout *m_formLayout;

    void initUi();
    void buildPropertyFields(const QVariantMap &data);

private slots:
    void onFieldChanged(const QString &propName, const QVariant &value);

signals:
    void propertyChanged(const QString &propName, const QVariant &value);
};

#endif // PROPERTYPANELDOCK_H
