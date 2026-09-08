#include "canvastabwidget.h"
#include "mymdisubwindow.h"
#include <QMessageBox>
#include <QApplication>

CanvasTabWidget::CanvasTabWidget(QWidget *parent)
    : QTabWidget(parent)
    , m_panelCounter(0)
{
    initUi();
}

void CanvasTabWidget::initUi()
{
    setTabsClosable(true);
    setMovable(true);
    setTabShape(QTabWidget::Triangular);

    connect(this, &QTabWidget::currentChanged,
            this, &CanvasTabWidget::onTabChanged);
    connect(this, &QTabWidget::tabCloseRequested,
            this, &CanvasTabWidget::onTabCloseRequested);
}

void CanvasTabWidget::newPanel(const QString &name)
{
    m_panelCounter++;

    QString title = name;
    if (title.isEmpty())
        title = QString("新建面板%1").arg(m_panelCounter);

    QMdiArea *area = new QMdiArea(this);
    area->setObjectName("QMdiArea");
    area->setBackground(QColor(0xfa, 0xfb, 0xfc));
    area->setActivationOrder(QMdiArea::CreationOrder);
    area->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    area->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    int index = addTab(area, title);
    setCurrentIndex(index);

    // 在新面板中默认创建一个子窗口
    newWindow();
}

void CanvasTabWidget::deletePanel(int index)
{
    if (index < 0)
        index = currentIndex();
    if (index < 0 || index >= count())
        return;

    if (QMessageBox::question(this, "确认删除",
            QString("确认删除面板「%1」?").arg(tabText(index)),
            QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes)
        return;

    QWidget *w = widget(index);
    if (w) {
        QMdiArea *area = qobject_cast<QMdiArea*>(w);
        if (area) {
            QList<QMdiSubWindow*> subs = area->subWindowList();
            foreach (QMdiSubWindow *sub, subs)
                sub->close();
            area->deleteLater();
        }
    }
    removeTab(index);
}

void CanvasTabWidget::newWindow()
{
    QMdiArea *area = currentMdiArea();
    if (!area) {
        // 如果没有面板，先创建一个
        newPanel();
        return;
    }

    MyMdiSubWindow *sw = new MyMdiSubWindow(area);
    sw->setWidgetType(QString("窗口%1").arg(area->subWindowList().size() + 1));
    sw->resize(400, 300);
    area->addSubWindow(sw);
    sw->show();
    area->tileSubWindows();
}

void CanvasTabWidget::deleteWindow()
{
    QMdiArea *area = currentMdiArea();
    if (!area)
        return;

    QMdiSubWindow *sub = area->currentSubWindow();
    if (sub) {
        sub->close();
    }
}

void CanvasTabWidget::autoLayout()
{
    QMdiArea *area = currentMdiArea();
    if (area)
        area->tileSubWindows();
}

void CanvasTabWidget::cascadeLayout()
{
    QMdiArea *area = currentMdiArea();
    if (area)
        area->cascadeSubWindows();
}

QMdiArea* CanvasTabWidget::currentMdiArea()
{
    QWidget *w = currentWidget();
    return qobject_cast<QMdiArea*>(w);
}

QMdiArea* CanvasTabWidget::mdiArea(int index)
{
    if (index < 0 || index >= count())
        return nullptr;
    QWidget *w = widget(index);
    return qobject_cast<QMdiArea*>(w);
}

void CanvasTabWidget::addMdiSubWindow(const QString &title)
{
    QMdiArea *area = currentMdiArea();
    if (!area) {
        newPanel(title);
        return;
    }

    MyMdiSubWindow *sw = new MyMdiSubWindow(area);
    sw->setWidgetType(title);
    sw->resize(400, 300);
    area->addSubWindow(sw);
    sw->show();
    area->tileSubWindows();
}

void CanvasTabWidget::onTabChanged(int index)
{
    if (index < 0)
        return;

    QMdiArea *area = currentMdiArea();
    if (area) {
        QMdiSubWindow *sub = area->currentSubWindow();
        emit windowChanged(sub);
    }
    emit panelChanged(index);
}

void CanvasTabWidget::onTabCloseRequested(int index)
{
    deletePanel(index);
}
