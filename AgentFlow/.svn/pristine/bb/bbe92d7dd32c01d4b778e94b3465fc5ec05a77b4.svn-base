#ifndef FINDRESULTDOCK_H
#define FINDRESULTDOCK_H

#include <QDockWidget>
#include <QTableWidget>
#include <QVariant>
#include <QMap>
#include <QList>

class FindResultDock : public QDockWidget
{
    Q_OBJECT
public:
    explicit FindResultDock(QWidget *parent = nullptr);

    void setResults(const QList<QVariantMap> &results);
    void clearResults();

private:
    QTableWidget *m_table;
    void initUi();
    void initTable();

private slots:
    void onItemClicked(int row, int col);

signals:
    void resultSelected(const QVariantMap &itemData);
};

#endif // FINDRESULTDOCK_H
