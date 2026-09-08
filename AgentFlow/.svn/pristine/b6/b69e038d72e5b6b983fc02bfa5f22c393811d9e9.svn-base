#include "mymdisubwindow.h"
#include <QCloseEvent>
#include <QLabel>
#include <QVBoxLayout>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QPainter>

MyMdiSubWindow::MyMdiSubWindow(QWidget *parent)
    : QMdiSubWindow(parent)
    , m_widgetType("default")
{
    setAttribute(Qt::WA_DeleteOnClose);

    // 占位内容：一个带标签的可视区域
    QWidget *content = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(content);
    layout->setContentsMargins(2, 2, 2, 2);

    QLabel *label = new QLabel("工作区窗口", content);
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet("QLabel { color: #6b7a90; font-size: 11pt; }");
    layout->addWidget(label);

    // 可视画布占位
    QGraphicsView *view = new QGraphicsView(content);
    QGraphicsScene *scene = new QGraphicsScene(view);
    scene->setSceneRect(0, 0, 800, 600);
    view->setScene(scene);
    view->setRenderHint(QPainter::Antialiasing);
    view->setStyleSheet("QGraphicsView { background: #fafbfc; border: 1px solid #e2e8f0; }");
    layout->addWidget(view);

    setWidget(content);
}

void MyMdiSubWindow::setWidgetType(const QString &type)
{
    m_widgetType = type;
    setWindowTitle(type);
}

void MyMdiSubWindow::closeEvent(QCloseEvent *event)
{
    QMdiSubWindow::closeEvent(event);
}
