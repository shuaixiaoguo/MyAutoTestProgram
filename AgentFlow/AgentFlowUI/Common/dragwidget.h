#ifndef DRAGWIDGET_H
#define DRAGWIDGET_H

#include <QLabel>
#include <QMouseEvent>
#include <QDrag>
#include <QMimeData>
#include <QPixmap>

class DragWidget : public QLabel
{
    Q_OBJECT
public:
    explicit DragWidget(const QString &text, const QString &dragType, QWidget *parent = nullptr)
        : QLabel(text, parent), m_dragType(dragType)
    {
        setAlignment(Qt::AlignCenter);
        setCursor(Qt::PointingHandCursor);
        setMinimumHeight(28);
        setProperty("dragType", true);
    }

    QString dragType() const { return m_dragType; }

protected:
    void mousePressEvent(QMouseEvent *event) override
    {
        if (event->button() == Qt::LeftButton) {
            m_dragStartPos = event->pos();
        }
        QLabel::mousePressEvent(event);
    }

    void mouseMoveEvent(QMouseEvent *event) override
    {
        if (!(event->buttons() & Qt::LeftButton))
            return;
        if ((event->pos() - m_dragStartPos).manhattanLength() < 5)
            return;

        QDrag *drag = new QDrag(this);
        QMimeData *mimeData = new QMimeData;
        mimeData->setText(m_dragType);
        mimeData->setData("application/x-dragwidget-type", m_dragType.toUtf8());
        drag->setMimeData(mimeData);

        QPixmap pixmap(size());
        render(&pixmap);
        drag->setPixmap(pixmap);
        drag->setHotSpot(event->pos());

        drag->exec(Qt::CopyAction);
        QLabel::mouseMoveEvent(event);
    }

private:
    QString m_dragType;
    QPoint m_dragStartPos;
};

#endif // DRAGWIDGET_H
