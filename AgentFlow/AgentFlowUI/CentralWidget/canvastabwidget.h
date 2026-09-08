#ifndef CANVASTABWIDGET_H
#define CANVASTABWIDGET_H

#include <QTabWidget>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QString>

class CanvasTabWidget : public QTabWidget
{
    Q_OBJECT
public:
    explicit CanvasTabWidget(QWidget *parent = nullptr);

    void newPanel(const QString &name = QString());
    void deletePanel(int index = -1);
    void newWindow();
    void deleteWindow();
    void autoLayout();
    void cascadeLayout();

    QMdiArea* currentMdiArea();
    QMdiArea* mdiArea(int index);
    int panelCount() const { return count(); }

    // 在当前面板添加MDI子窗口（带标题）
    void addMdiSubWindow(const QString &title);

private:
    void initUi();
    int m_panelCounter;

private slots:
    void onTabChanged(int index);
    void onTabCloseRequested(int index);

signals:
    void panelChanged(int index);
    void windowChanged(QMdiSubWindow *window);
};

#endif // CANVASTABWIDGET_H
