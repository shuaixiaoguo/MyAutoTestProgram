#ifndef CONTROLTOOLBOXDOCK_H
#define CONTROLTOOLBOXDOCK_H

#include <QDockWidget>
#include <QToolBox>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QList>
#include <QPair>
#include "Common/dragwidget.h"

class ControlToolBoxDock : public QDockWidget
{
    Q_OBJECT
public:
    explicit ControlToolBoxDock(QWidget *parent = nullptr);

private:
    QToolBox *m_toolBox;
    void initUi();
    void initToolBox();
    QWidget* createGroup(const QString &title,
                         const QList<QPair<QString, QString>> &items);

signals:
    void controlDragStarted(const QString &controlType);
};

#endif // CONTROLTOOLBOXDOCK_H
