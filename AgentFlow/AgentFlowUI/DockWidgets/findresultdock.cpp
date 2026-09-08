#include "findresultdock.h"
#include <QHeaderView>
#include <QTableWidgetItem>

FindResultDock::FindResultDock(QWidget *parent)
    : QDockWidget("参数查找结果", parent)
{
    setObjectName("FindResultDock");
    initUi();
    initTable();
}

void FindResultDock::initUi()
{
    setFeatures(QDockWidget::DockWidgetMovable
              | QDockWidget::DockWidgetFloatable);
    setMinimumHeight(100);
}

void FindResultDock::initTable()
{
    m_table = new QTableWidget(this);
    m_table->setColumnCount(5);
    m_table->setHorizontalHeaderLabels(
        QStringList() << "序号" << "参数名" << "值" << "单位" << "来源");
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setAlternatingRowColors(true);
    setWidget(m_table);

    connect(m_table, &QTableWidget::cellClicked,
            this, &FindResultDock::onItemClicked);

    // Mock 数据
    setResults({
        {{"序号", 1}, {"参数名", "U1"}, {"值", "220"}, {"单位", "V"}, {"来源", "ICD"}},
        {{"序号", 2}, {"参数名", "I1"}, {"值", "5.2"}, {"单位", "A"}, {"来源", "ICD"}},
        {{"序号", 3}, {"参数名", "F1"}, {"值", "50"},  {"单位", "Hz"}, {"来源", "ICD"}},
    });
}

void FindResultDock::setResults(const QList<QVariantMap> &results)
{
    m_table->setRowCount(results.size());
    for (int r = 0; r < results.size(); ++r) {
        const QVariantMap &row = results[r];
        m_table->setItem(r, 0, new QTableWidgetItem(row.value("序号").toString()));
        m_table->setItem(r, 1, new QTableWidgetItem(row.value("参数名").toString()));
        m_table->setItem(r, 2, new QTableWidgetItem(row.value("值").toString()));
        m_table->setItem(r, 3, new QTableWidgetItem(row.value("单位").toString()));
        m_table->setItem(r, 4, new QTableWidgetItem(row.value("来源").toString()));
    }
}

void FindResultDock::clearResults()
{
    m_table->setRowCount(0);
}

void FindResultDock::onItemClicked(int row, int /*col*/)
{
    if (row < 0 || row >= m_table->rowCount())
        return;

    QVariantMap data;
    for (int c = 0; c < m_table->columnCount(); ++c) {
        QString header = m_table->horizontalHeaderItem(c)->text();
        QString value = m_table->item(row, c) ? m_table->item(row, c)->text() : "";
        data[header] = value;
    }
    emit resultSelected(data);
}
