#ifndef RUNLOGDOCK_H
#define RUNLOGDOCK_H

#include <QDockWidget>
#include <QTextEdit>
#include <QTimer>
#include <QQueue>
#include <QMutex>
#include <QPair>

class RunLogDock : public QDockWidget
{
    Q_OBJECT
public:
    explicit RunLogDock(QWidget *parent = nullptr);

    void appendLog(int level, const QString &message);
    void clearLog();

private:
    QTextEdit *m_logEdit;
    QTimer *m_flushTimer;
    QQueue<QPair<int, QString>> m_msgQueue;
    QMutex m_queueMutex;

    static const int MAX_LOG_LINES = 5000;
    static const int FLUSH_INTERVAL_MS = 100;

    void initUi();
    void flushQueue();

private slots:
    void onFlushTimeout();
};

#endif // RUNLOGDOCK_H
