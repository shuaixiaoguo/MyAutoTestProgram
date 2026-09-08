#ifndef BASETABLEVIEW_H
#define BASETABLEVIEW_H

#include <QAbstractScrollArea>
#include <QWidget>
#include <QVector>
#include <QVariant>
#include <QString>
#include <QColor>
#include <QFont>

// 列定义
struct TableColumnInfo
{
    QString title;          // 列标题
    int width;              // 列宽（像素）
    Qt::Alignment alignment; // 对齐方式
    bool visible;           // 是否可见

    TableColumnInfo()
        : width(100), alignment(Qt::AlignLeft | Qt::AlignVCenter), visible(true) {}
    TableColumnInfo(const QString &t, int w, Qt::Alignment a = Qt::AlignLeft | Qt::AlignVCenter)
        : title(t), width(w), alignment(a), visible(true) {}
};

// 单元格样式
struct TableCellStyle
{
    QColor bgColor;
    QColor textColor;
    QFont font;
};

class BaseTableView : public QAbstractScrollArea
{
    Q_OBJECT
public:
    explicit BaseTableView(QWidget *parent = nullptr);
    ~BaseTableView() override;

    // === 列管理 ===
    void addColumn(const QString &title, int width, Qt::Alignment align = Qt::AlignLeft | Qt::AlignVCenter);
    void setColumnCount(int count);
    int columnCount() const { return m_columns.size(); }
    void setColumnWidth(int col, int width);
    int columnWidth(int col) const;
    void setColumnTitle(int col, const QString &title);
    QString columnTitle(int col) const;

    // === 行数据管理（增删改查） ===
    int rowCount() const { return m_rows.size(); }

    // 增加一行
    void appendRow(const QVector<QVariant> &rowData);
    void insertRow(int row, const QVector<QVariant> &rowData);

    // 删除行
    void removeRow(int row);
    void removeRows(int row, int count);
    void clear();

    // 修改单元格
    void setData(int row, int col, const QVariant &value);
    QVariant data(int row, int col) const;

    // 修改整行
    void setRowData(int row, const QVector<QVariant> &rowData);
    QVector<QVariant> rowData(int row) const;

    // 批量设置数据
    void setDataList(const QVector<QVector<QVariant>> &rows);

    // 查找
    int findRow(int col, const QVariant &value) const;

    // === 选中 ===
    int currentRow() const { return m_currentRow; }
    void setCurrentRow(int row);
    void selectRow(int row);

    // === 行高 ===
    void setRowHeight(int height);
    int rowHeight() const { return m_rowHeight; }

    // === 表头高度 ===
    void setHeaderHeight(int height);
    int headerHeight() const { return m_headerHeight; }

    // === 用户数据（每一行存一个 userData，类似于 QListWidgetItem::data(Qt::UserRole)） ===
    void setRowUserData(int row, const QVariant &userData);
    QVariant rowUserData(int row) const;

    // === 样式设置 ===
    // 设置单行/双行背景色（斑马纹效果，防止看串行）
    void setRowColors(const QColor &evenColor, const QColor &oddColor);
    // 设置表头背景和文字色
    void setHeaderColor(const QColor &bg, const QColor &text);
    // 设置选中行背景和文字色
    void setSelectedColor(const QColor &bg, const QColor &text);
    // 设置悬停行背景色
    void setHoverColor(const QColor &color);
    // 设置网格线颜色
    void setGridColor(const QColor &color);
    // 设置表头字体
    void setHeaderFont(const QFont &font);
    // 设置单元格字体
    void setCellFont(const QFont &font);
    // 设置文字颜色
    void setTextColor(const QColor &color);

signals:
    void rowClicked(int row);
    void rowDoubleClicked(int row);
    void rowRightClicked(int row, const QPoint &globalPos);
    void currentRowChanged(int row);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    void updateScrollBars();
    int visibleRowCount() const;
    int rowAt(const QPoint &pos) const;
    void ensureRowVisible(int row);
    void drawHeader(QPainter &painter);
    void drawRows(QPainter &painter);
    void drawRow(QPainter &painter, int row, int y);
    void drawCell(QPainter &painter, int row, int col, const QRect &rect);

    QVector<TableColumnInfo> m_columns;     // 列定义
    QVector<QVector<QVariant>> m_rows;      // 行数据（二维数组）
    QVector<QVariant> m_userDatas;          // 每行的用户数据

    int m_rowHeight;        // 行高
    int m_headerHeight;     // 表头高度
    int m_currentRow;       // 当前选中行
    int m_hoverRow;         // 鼠标悬停行

    // 颜色配置
    QColor m_headerBg;
    QColor m_headerText;
    QColor m_rowBgEven;
    QColor m_rowBgOdd;
    QColor m_rowBgHover;
    QColor m_rowBgSelected;
    QColor m_textColor;
    QColor m_selectedTextColor;
    QColor m_gridColor;

    QFont m_headerFont;
    QFont m_cellFont;
};

#endif // BASETABLEVIEW_H
