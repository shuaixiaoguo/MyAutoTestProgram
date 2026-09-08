#include "basetableview.h"
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QScrollBar>
#include <QApplication>
#include <QPalette>
#include <QEvent>

BaseTableView::BaseTableView(QWidget *parent)
    : QAbstractScrollArea(parent)
    , m_rowHeight(28)
    , m_headerHeight(32)
    , m_currentRow(-1)
    , m_hoverRow(-1)
{
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    setFrameShape(QFrame::StyledPanel);
    setFrameShadow(QFrame::Sunken);

    // 默认颜色
    m_headerBg = QColor("#f0f0f0");
    m_headerText = QColor("#333333");
    m_rowBgEven = QColor("#ffffff");
    m_rowBgOdd = QColor("#fafafa");
    m_rowBgHover = QColor("#e8f0fe");
    m_rowBgSelected = QColor("#1677ff");
    m_textColor = QColor("#333333");
    m_selectedTextColor = QColor("#ffffff");
    m_gridColor = QColor("#e5e7eb");

    m_headerFont = font();
    m_headerFont.setBold(true);
    m_cellFont = font();

    // 初始化滚动条
    verticalScrollBar()->setRange(0, 0);
    horizontalScrollBar()->setRange(0, 0);
}

BaseTableView::~BaseTableView()
{
}

// ==================== 列管理 ====================

void BaseTableView::addColumn(const QString &title, int width, Qt::Alignment align)
{
    m_columns.append(TableColumnInfo(title, width, align));
    updateScrollBars();
    viewport()->update();
}

void BaseTableView::setColumnCount(int count)
{
    m_columns.resize(count);
    updateScrollBars();
    viewport()->update();
}

void BaseTableView::setColumnWidth(int col, int width)
{
    if (col >= 0 && col < m_columns.size()) {
        m_columns[col].width = width;
        updateScrollBars();
        viewport()->update();
    }
}

int BaseTableView::columnWidth(int col) const
{
    if (col >= 0 && col < m_columns.size())
        return m_columns[col].width;
    return 0;
}

void BaseTableView::setColumnTitle(int col, const QString &title)
{
    if (col >= 0 && col < m_columns.size()) {
        m_columns[col].title = title;
        viewport()->update();
    }
}

QString BaseTableView::columnTitle(int col) const
{
    if (col >= 0 && col < m_columns.size())
        return m_columns[col].title;
    return QString();
}

// ==================== 行数据管理 ====================

void BaseTableView::appendRow(const QVector<QVariant> &rowData)
{
    m_rows.append(rowData);
    m_userDatas.append(QVariant());
    updateScrollBars();
    viewport()->update();
}

void BaseTableView::insertRow(int row, const QVector<QVariant> &rowData)
{
    if (row < 0) row = 0;
    if (row > m_rows.size()) row = m_rows.size();
    m_rows.insert(row, rowData);
    m_userDatas.insert(row, QVariant());
    updateScrollBars();
    viewport()->update();
}

void BaseTableView::removeRow(int row)
{
    if (row >= 0 && row < m_rows.size()) {
        m_rows.remove(row);
        m_userDatas.remove(row);
        if (m_currentRow >= m_rows.size())
            m_currentRow = m_rows.size() - 1;
        updateScrollBars();
        viewport()->update();
    }
}

void BaseTableView::removeRows(int row, int count)
{
    if (row < 0 || count <= 0 || row >= m_rows.size())
        return;
    int end = qMin(row + count, m_rows.size());
    m_rows.remove(row, end - row);
    m_userDatas.remove(row, end - row);
    if (m_currentRow >= m_rows.size())
        m_currentRow = m_rows.size() - 1;
    updateScrollBars();
    viewport()->update();
}

void BaseTableView::clear()
{
    m_rows.clear();
    m_userDatas.clear();
    m_currentRow = -1;
    m_hoverRow = -1;
    updateScrollBars();
    viewport()->update();
}

void BaseTableView::setData(int row, int col, const QVariant &value)
{
    if (row >= 0 && row < m_rows.size() && col >= 0 && col < m_columns.size()) {
        if (col >= m_rows[row].size())
            m_rows[row].resize(m_columns.size());
        m_rows[row][col] = value;
        viewport()->update();
    }
}

QVariant BaseTableView::data(int row, int col) const
{
    if (row >= 0 && row < m_rows.size() && col >= 0 && col < m_rows[row].size())
        return m_rows[row][col];
    return QVariant();
}

void BaseTableView::setRowData(int row, const QVector<QVariant> &rowData)
{
    if (row >= 0 && row < m_rows.size()) {
        m_rows[row] = rowData;
        viewport()->update();
    }
}

QVector<QVariant> BaseTableView::rowData(int row) const
{
    if (row >= 0 && row < m_rows.size())
        return m_rows[row];
    return QVector<QVariant>();
}

void BaseTableView::setDataList(const QVector<QVector<QVariant>> &rows)
{
    m_rows = rows;
    m_userDatas.fill(QVariant(), rows.size());
    m_currentRow = -1;
    updateScrollBars();
    viewport()->update();
}

int BaseTableView::findRow(int col, const QVariant &value) const
{
    for (int i = 0; i < m_rows.size(); ++i) {
        if (col >= 0 && col < m_rows[i].size() && m_rows[i][col] == value)
            return i;
    }
    return -1;
}

void BaseTableView::setCurrentRow(int row)
{
    if (row >= m_rows.size())
        row = m_rows.size() - 1;
    if (row < 0)
        row = -1;

    if (m_currentRow != row) {
        m_currentRow = row;
        ensureRowVisible(row);
        viewport()->update();
        emit currentRowChanged(row);
    }
}

void BaseTableView::selectRow(int row)
{
    setCurrentRow(row);
}

void BaseTableView::setRowHeight(int height)
{
    m_rowHeight = height;
    updateScrollBars();
    viewport()->update();
}

void BaseTableView::setHeaderHeight(int height)
{
    m_headerHeight = height;
    viewport()->update();
}

void BaseTableView::setRowUserData(int row, const QVariant &userData)
{
    if (row >= 0 && row < m_userDatas.size())
        m_userDatas[row] = userData;
}

QVariant BaseTableView::rowUserData(int row) const
{
    if (row >= 0 && row < m_userDatas.size())
        return m_userDatas[row];
    return QVariant();
}

// ==================== 样式设置 ====================

void BaseTableView::setRowColors(const QColor &evenColor, const QColor &oddColor)
{
    m_rowBgEven = evenColor;
    m_rowBgOdd = oddColor;
    viewport()->update();
}

void BaseTableView::setHeaderColor(const QColor &bg, const QColor &text)
{
    m_headerBg = bg;
    m_headerText = text;
    viewport()->update();
}

void BaseTableView::setSelectedColor(const QColor &bg, const QColor &text)
{
    m_rowBgSelected = bg;
    m_selectedTextColor = text;
    viewport()->update();
}

void BaseTableView::setHoverColor(const QColor &color)
{
    m_rowBgHover = color;
    viewport()->update();
}

void BaseTableView::setGridColor(const QColor &color)
{
    m_gridColor = color;
    viewport()->update();
}

void BaseTableView::setHeaderFont(const QFont &font)
{
    m_headerFont = font;
    viewport()->update();
}

void BaseTableView::setCellFont(const QFont &font)
{
    m_cellFont = font;
    viewport()->update();
}

void BaseTableView::setTextColor(const QColor &color)
{
    m_textColor = color;
    viewport()->update();
}

// ==================== 事件处理 ====================

void BaseTableView::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(viewport());
    painter.setRenderHint(QPainter::Antialiasing, false);

    // 画背景
    painter.fillRect(viewport()->rect(), m_rowBgEven);

    // 画表头
    drawHeader(painter);

    // 画行
    drawRows(painter);
}

void BaseTableView::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)
    updateScrollBars();
    viewport()->update();
}

void BaseTableView::mousePressEvent(QMouseEvent *event)
{
    int row = rowAt(event->pos());

    if (event->button() == Qt::LeftButton) {
        if (row >= 0) {
            setCurrentRow(row);
            emit rowClicked(row);
        }
    } else if (event->button() == Qt::RightButton) {
        if (row >= 0) {
            setCurrentRow(row);
            emit rowRightClicked(row, event->globalPos());
        }
    }
    QAbstractScrollArea::mousePressEvent(event);
}

void BaseTableView::mouseMoveEvent(QMouseEvent *event)
{
    int row = rowAt(event->pos());
    if (row != m_hoverRow) {
        m_hoverRow = row;
        viewport()->update();
    }
    QAbstractScrollArea::mouseMoveEvent(event);
}

void BaseTableView::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        int row = rowAt(event->pos());
        if (row >= 0) {
            setCurrentRow(row);
            emit rowDoubleClicked(row);
        }
    }
    QAbstractScrollArea::mouseDoubleClickEvent(event);
}

void BaseTableView::wheelEvent(QWheelEvent *event)
{
    int delta = event->angleDelta().y();
    int val = verticalScrollBar()->value();
    int step = m_rowHeight * 3;
    if (delta > 0)
        verticalScrollBar()->setValue(qMax(0, val - step));
    else
        verticalScrollBar()->setValue(qMin(verticalScrollBar()->maximum(), val + step));

    event->accept();
}

void BaseTableView::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Up:
        if (m_currentRow > 0)
            setCurrentRow(m_currentRow - 1);
        break;
    case Qt::Key_Down:
        if (m_currentRow < m_rows.size() - 1)
            setCurrentRow(m_currentRow + 1);
        break;
    case Qt::Key_Return:
    case Qt::Key_Enter:
        if (m_currentRow >= 0)
            emit rowDoubleClicked(m_currentRow);
        break;
    default:
        break;
    }
    QAbstractScrollArea::keyPressEvent(event);
}

void BaseTableView::leaveEvent(QEvent *event)
{
    Q_UNUSED(event)
    if (m_hoverRow >= 0) {
        m_hoverRow = -1;
        viewport()->update();
    }
}

// ==================== 内部方法 ====================

void BaseTableView::updateScrollBars()
{
    int contentHeight = m_rows.size() * m_rowHeight;
    int visibleH = viewport()->height() - m_headerHeight;

    if (contentHeight <= visibleH) {
        verticalScrollBar()->setRange(0, 0);
    } else {
        verticalScrollBar()->setRange(0, contentHeight - visibleH);
        verticalScrollBar()->setPageStep(visibleH);
    }

    // 水平滚动条
    int totalWidth = 0;
    for (const auto &col : m_columns)
        totalWidth += col.width;

    if (totalWidth <= viewport()->width()) {
        horizontalScrollBar()->setRange(0, 0);
    } else {
        horizontalScrollBar()->setRange(0, totalWidth - viewport()->width());
        horizontalScrollBar()->setPageStep(viewport()->width());
    }
}

int BaseTableView::visibleRowCount() const
{
    int visibleH = viewport()->height() - m_headerHeight;
    if (m_rowHeight <= 0) return 0;
    return (visibleH + m_rowHeight - 1) / m_rowHeight + 1;
}

int BaseTableView::rowAt(const QPoint &pos) const
{
    int y = pos.y() - m_headerHeight + verticalScrollBar()->value();
    if (y < 0) return -1;
    if (m_rowHeight <= 0) return -1;
    int row = y / m_rowHeight;
    if (row >= m_rows.size()) return -1;
    return row;
}

void BaseTableView::ensureRowVisible(int row)
{
    if (row < 0) return;

    int y = row * m_rowHeight;
    int visibleH = viewport()->height() - m_headerHeight;
    int scrollVal = verticalScrollBar()->value();

    if (y < scrollVal) {
        verticalScrollBar()->setValue(y);
    } else if (y + m_rowHeight > scrollVal + visibleH) {
        verticalScrollBar()->setValue(y + m_rowHeight - visibleH);
    }
}

void BaseTableView::drawHeader(QPainter &painter)
{
    QRect headerRect(0, 0, viewport()->width(), m_headerHeight);
    painter.fillRect(headerRect, m_headerBg);

    painter.setPen(m_gridColor);
    painter.drawLine(0, m_headerHeight - 1, viewport()->width(), m_headerHeight - 1);

    int x = -horizontalScrollBar()->value();
    painter.setPen(m_headerText);
    painter.setFont(m_headerFont);

    for (int i = 0; i < m_columns.size(); ++i) {
        if (!m_columns[i].visible) continue;

        QRect cellRect(x, 0, m_columns[i].width, m_headerHeight);
        painter.drawText(cellRect, m_columns[i].alignment, m_columns[i].title);

        // 列分隔线
        painter.setPen(m_gridColor);
        painter.drawLine(x + m_columns[i].width - 1, 0,
                         x + m_columns[i].width - 1, m_headerHeight - 1);
        painter.setPen(m_headerText);

        x += m_columns[i].width;
    }
}

void BaseTableView::drawRows(QPainter &painter)
{
    int scrollY = verticalScrollBar()->value();
    int visibleH = viewport()->height() - m_headerHeight;
    int startRow = scrollY / m_rowHeight;
    int endRow = qMin(startRow + visibleRowCount() + 1, m_rows.size());

    int y = m_headerHeight + startRow * m_rowHeight - scrollY;

    for (int row = startRow; row < endRow; ++row) {
        drawRow(painter, row, y);
        y += m_rowHeight;
    }
}

void BaseTableView::drawRow(QPainter &painter, int row, int y)
{
    // 背景
    QColor bgColor;
    if (row == m_currentRow)
        bgColor = m_rowBgSelected;
    else if (row == m_hoverRow)
        bgColor = m_rowBgHover;
    else
        bgColor = (row % 2 == 0) ? m_rowBgEven : m_rowBgOdd;

    painter.fillRect(0, y, viewport()->width(), m_rowHeight, bgColor);

    // 底部网格线
    painter.setPen(m_gridColor);
    painter.drawLine(0, y + m_rowHeight - 1, viewport()->width(), y + m_rowHeight - 1);

    // 单元格
    int x = -horizontalScrollBar()->value();
    for (int col = 0; col < m_columns.size(); ++col) {
        if (!m_columns[col].visible) continue;

        QRect cellRect(x, y, m_columns[col].width, m_rowHeight);
        drawCell(painter, row, col, cellRect);

        // 列分隔线
        painter.setPen(m_gridColor);
        painter.drawLine(x + m_columns[col].width - 1, y,
                         x + m_columns[col].width - 1, y + m_rowHeight - 1);

        x += m_columns[col].width;
    }
}

void BaseTableView::drawCell(QPainter &painter, int row, int col, const QRect &rect)
{
    QString text;
    if (row < m_rows.size() && col < m_rows[row].size())
        text = m_rows[row][col].toString();

    QColor textColor = (row == m_currentRow) ? m_selectedTextColor : m_textColor;
    painter.setPen(textColor);
    painter.setFont(m_cellFont);

    // 文字内边距
    QRect textRect = rect.adjusted(8, 0, -8, 0);
    painter.drawText(textRect, m_columns[col].alignment | Qt::AlignVCenter,
                     fontMetrics().elidedText(text, Qt::ElideRight, textRect.width()));
}
