#ifndef MYMDISUBWINDOW_H
#define MYMDISUBWINDOW_H

#include <QMdiSubWindow>
#include <QLabel>
#include <QVBoxLayout>
#include <QGraphicsView>
#include <QGraphicsScene>

class MyMdiSubWindow : public QMdiSubWindow
{
    Q_OBJECT
public:
    explicit MyMdiSubWindow(QWidget *parent = nullptr);

    void setWidgetType(const QString &type);
    QString widgetType() const { return m_widgetType; }

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    QString m_widgetType;
};

#endif // MYMDISUBWINDOW_H
