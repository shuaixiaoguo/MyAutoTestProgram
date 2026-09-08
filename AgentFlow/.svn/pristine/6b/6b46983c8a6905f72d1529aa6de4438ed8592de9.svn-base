#include "runlogdock.h"
#include "Common/commondef.h"
#include <QDateTime>
#include <QScrollBar>
#include <QTextCursor>

RunLogDock::RunLogDock(QWidget *parent)
    : QDockWidget("运行日志", parent)
{
    setObjectName("RunLogDock");
    initUi();

    m_flushTimer = new QTimer(this);
    m_flushTimer->setInterval(FLUSH_INTERVAL_MS);
    connect(m_flushTimer, &QTimer::timeout,
            this, &RunLogDock::onFlushTimeout);
    m_flushTimer->start();

    appendLog(LogInfo, "程序启动");
}

void RunLogDock::initUi()
{
    setFeatures(QDockWidget::DockWidgetMovable
              | QDockWidget::DockWidgetFloatable);
    setMinimumHeight(100);

    m_logEdit = new QTextEdit(this);
    m_logEdit->setReadOnly(true);
    m_logEdit->setLineWrapMode(QTextEdit::NoWrap);
    m_logEdit->setStyleSheet(
        "QTextEdit { background: #1e293b; color: #e2e8f0; "
        "font-family: Consolas, 'Courier New', monospace; font-size: 10pt; "
        "border: 1px solid #e2e8f0; }");

    setWidget(m_logEdit);
}

void RunLogDock::appendLog(int level, const QString &message)
{
    m_queueMutex.lock();
    m_msgQueue.enqueue(qMakePair(level, message));
    m_queueMutex.unlock();
}

void RunLogDock::flushQueue()
{
    if (m_msgQueue.isEmpty())
        return;

    QString html;
    m_queueMutex.lock();
    while (!m_msgQueue.isEmpty()) {
        auto item = m_msgQueue.dequeue();
        int level = item.first;
        QString msg = item.second;
        QString time = QDateTime::currentDateTime()
                          .toString("yyyy-MM-dd hh:mm:ss");
        QString levelStr;
        QString color;

        switch (level) {
        case LogDebug:    levelStr = "DEBUG";   color = "#94a3b8"; break;
        case LogInfo:     levelStr = "INFO";    color = "#38bdf8"; break;
        case LogWarning:  levelStr = "WARNING"; color = "#fbbf24"; break;
        case LogError:    levelStr = "ERROR";   color = "#f87171"; break;
        case LogCritical: levelStr = "CRITICAL";color = "#ef4444"; break;
        default:          levelStr = "INFO";    color = "#38bdf8"; break;
        }

        html += QString("<div style='color:%1'>[%2] [%3] %4</div>")
                    .arg(color, time, levelStr, msg);
    }
    m_queueMutex.unlock();

    // 自动滚动到底部
    QScrollBar *bar = m_logEdit->verticalScrollBar();
    bool atBottom = bar->value() >= bar->maximum() - 10;
    m_logEdit->append(html);
    if (atBottom)
        bar->setValue(bar->maximum());
}

void RunLogDock::onFlushTimeout()
{
    flushQueue();
}

void RunLogDock::clearLog()
{
    m_logEdit->clear();
}
