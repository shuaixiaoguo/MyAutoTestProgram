#include "basetreeview.h"
#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QScrollBar>

BaseTreeView::BaseTreeView(QWidget *parent)
    : QAbstractScrollArea(parent)
    , m_rowHeight(28)
    , m_indent(20)
    , m_currentRow(-1)
    , m_hoverRow(-1)
    , m_scrollOffset(0)
    , m_colorEven("#ffffff")
    , m_colorOdd("#f5f7fa")
    , m_colorSelectedBg("#1677ff")
    , m_colorSelectedText("#ffffff")
    , m_colorHover("#e8f0fe")
    , m_colorText("#1f2937")
{
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    setFrameShape(QFrame::StyledPanel);

    // 添加虚拟根节点
    TreeNode root;
    root.id = 0;
    root.parentId = -1;
    root.level = 0;
    root.name = "Root";
    root.type = NodeRoot;
    root.expanded = true;
    root.hasChildren = false;
    root.childrenLoaded = true;
    m_nodes.append(root);

    updateScrollBars();
}

BaseTreeView::~BaseTreeView()
{
}

void BaseTreeView::setRootNode(const TreeNode &root)
{
    m_nodes.clear();
    TreeNode r = root;
    r.expanded = true;
    m_nodes.append(r);
    m_currentRow = -1;
    m_hoverRow = -1;
    m_scrollOffset = 0;
    updateScrollBars();
    viewport()->update();
}

void BaseTreeView::appendChild(int parentId, const TreeNode &node)
{
    // 找到父节点的位置，在其后面插入（保持深度遍历顺序）
    int parentIdx = findNodeById(parentId);
    if (parentIdx < 0) return;

    // 计算插入位置：父节点最后一个可见子节点的后面
    int insertIdx = parentIdx + 1;
    if (m_nodes[parentIdx].expanded) {
        // 父节点展开，跳过所有子节点找到最后一个
        int i = parentIdx + 1;
        while (i < m_nodes.size() && m_nodes[i].level > m_nodes[parentIdx].level) {
            i++;
        }
        insertIdx = i;
    }

    TreeNode n = node;
    n.parentId = parentId;
    m_nodes.insert(insertIdx, n);

    // 标记父节点有子节点
    m_nodes[parentIdx].hasChildren = true;

    updateScrollBars();
    viewport()->update();
}

void BaseTreeView::insertNode(int index, const TreeNode &node)
{
    if (index < 0 || index > m_nodes.size()) return;
    m_nodes.insert(index, node);
    updateScrollBars();
    viewport()->update();
}

void BaseTreeView::removeNode(int index)
{
    if (index <= 0 || index >= m_nodes.size()) return; // 不能删根节点

    int parentId = m_nodes[index].parentId;
    int level = m_nodes[index].level;

    // 同时删除所有子节点
    int endIdx = index + 1;
    while (endIdx < m_nodes.size() && m_nodes[endIdx].level > level) {
        endIdx++;
    }
    m_nodes.remove(index, endIdx - index);

    // 更新父节点的 hasChildren 状态
    int parentIdx = findNodeById(parentId);
    if (parentIdx >= 0) {
        bool hasChildren = false;
        for (int i = 0; i < m_nodes.size(); i++) {
            if (m_nodes[i].parentId == parentId) {
                hasChildren = true;
                break;
            }
        }
        m_nodes[parentIdx].hasChildren = hasChildren;
    }

    if (m_currentRow >= m_nodes.size())
        m_currentRow = m_nodes.size() - 1;

    updateScrollBars();
    viewport()->update();
}

void BaseTreeView::clear()
{
    TreeNode root = m_nodes.first();
    m_nodes.clear();
    m_nodes.append(root);
    m_currentRow = -1;
    m_hoverRow = -1;
    m_scrollOffset = 0;
    updateScrollBars();
    viewport()->update();
}

int BaseTreeView::nodeCount() const
{
    return m_nodes.size() - 1; // 去掉根节点
}

TreeNode BaseTreeView::nodeAt(int index) const
{
    if (index >= 0 && index < m_nodes.size())
        return m_nodes[index];
    return TreeNode();
}

TreeNode BaseTreeView::currentNode() const
{
    if (m_currentRow >= 0 && m_currentRow < m_nodes.size())
        return m_nodes[m_currentRow];
    return TreeNode();
}

int BaseTreeView::findNodeById(int id) const
{
    for (int i = 0; i < m_nodes.size(); i++) {
        if (m_nodes[i].id == id)
            return i;
    }
    return -1;
}

int BaseTreeView::findNodeByName(const QString &name) const
{
    for (int i = 0; i < m_nodes.size(); i++) {
        if (m_nodes[i].name == name)
            return i;
    }
    return -1;
}

void BaseTreeView::expandNode(int index)
{
    if (index < 0 || index >= m_nodes.size()) return;
    if (m_nodes[index].expanded) return;
    if (!m_nodes[index].hasChildren) return;

    m_nodes[index].expanded = true;
    updateScrollBars();
    viewport()->update();
    emit nodeExpanded(index, m_nodes[index]);
}

void BaseTreeView::collapseNode(int index)
{
    if (index < 0 || index >= m_nodes.size()) return;
    if (!m_nodes[index].expanded) return;

    m_nodes[index].expanded = false;
    updateScrollBars();
    viewport()->update();
    emit nodeCollapsed(index, m_nodes[index]);
}

void BaseTreeView::toggleExpand(int index)
{
    if (index < 0 || index >= m_nodes.size()) return;
    if (m_nodes[index].expanded)
        collapseNode(index);
    else
        expandNode(index);
}

void BaseTreeView::expandAll()
{
    for (int i = 0; i < m_nodes.size(); i++) {
        if (m_nodes[i].hasChildren)
            m_nodes[i].expanded = true;
    }
    updateScrollBars();
    viewport()->update();
}

void BaseTreeView::collapseAll()
{
    for (int i = 1; i < m_nodes.size(); i++) { // 根节点不折叠
        if (m_nodes[i].hasChildren)
            m_nodes[i].expanded = false;
    }
    // 根节点保持展开
    if (m_nodes.size() > 0)
        m_nodes[0].expanded = true;
    updateScrollBars();
    viewport()->update();
}

void BaseTreeView::setChildrenLoaded(int parentId, bool loaded)
{
    int idx = findNodeById(parentId);
    if (idx >= 0)
        m_nodes[idx].childrenLoaded = loaded;
}

void BaseTreeView::setHasChildren(int nodeId, bool has)
{
    int idx = findNodeById(nodeId);
    if (idx >= 0)
        m_nodes[idx].hasChildren = has;
}

void BaseTreeView::removeChildren(int parentId)
{
    int parentIdx = findNodeById(parentId);
    if (parentIdx < 0) return;

    int level = m_nodes[parentIdx].level;
    int startIdx = parentIdx + 1;
    int endIdx = startIdx;
    while (endIdx < m_nodes.size() && m_nodes[endIdx].level > level) {
        endIdx++;
    }
    if (endIdx > startIdx) {
        m_nodes.remove(startIdx, endIdx - startIdx);
    }

    m_nodes[parentIdx].hasChildren = false;
    m_nodes[parentIdx].childrenLoaded = false;
    m_nodes[parentIdx].expanded = false;

    updateScrollBars();
    viewport()->update();
}

void BaseTreeView::appendChildren(int parentId, const QVector<TreeNode> &children)
{
    if (children.isEmpty()) return;

    int parentIdx = findNodeById(parentId);
    if (parentIdx < 0) return;

    int parentLevel = m_nodes[parentIdx].level;
    int insertIdx = parentIdx + 1;

    // 跳过该父节点下所有已有的子节点（用parentId匹配，不用level）
    while (insertIdx < m_nodes.size()
           && m_nodes[insertIdx].level > parentLevel
           && m_nodes[insertIdx].parentId != parentId) {
        // 这是其他同级节点的子树，不跳过
        break;
    }
    // 真正跳过当前父节点已有的子节点
    while (insertIdx < m_nodes.size()
           && m_nodes[insertIdx].parentId == parentId) {
        insertIdx++;
    }
    // 也跳过子节点的子节点（孙节点）
    while (insertIdx < m_nodes.size()
           && m_nodes[insertIdx].level > parentLevel) {
        insertIdx++;
    }

    for (int i = children.size() - 1; i >= 0; i--) {
        TreeNode n = children[i];
        n.parentId = parentId;
        m_nodes.insert(insertIdx, n);
    }

    m_nodes[parentIdx].hasChildren = true;
    m_nodes[parentIdx].childrenLoaded = true;

    updateScrollBars();
    viewport()->update();
}

void BaseTreeView::setRowHeight(int height)
{
    m_rowHeight = height;
    updateScrollBars();
    viewport()->update();
}

void BaseTreeView::setIndent(int indent)
{
    m_indent = indent;
    viewport()->update();
}

void BaseTreeView::setRowColors(const QColor &even, const QColor &odd)
{
    m_colorEven = even;
    m_colorOdd = odd;
    viewport()->update();
}

void BaseTreeView::setSelectedColor(const QColor &bg, const QColor &text)
{
    m_colorSelectedBg = bg;
    m_colorSelectedText = text;
    viewport()->update();
}

void BaseTreeView::setHoverColor(const QColor &color)
{
    m_colorHover = color;
    viewport()->update();
}

void BaseTreeView::setTextColor(const QColor &color)
{
    m_colorText = color;
    viewport()->update();
}

void BaseTreeView::setCurrentRow(int row)
{
    if (row < 0 || row >= m_nodes.size()) return;
    m_currentRow = row;
    viewport()->update();
    emit currentChanged(row, m_nodes[row]);
}

// ---- 绘制 ----

void BaseTreeView::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(viewport());
    painter.setRenderHint(QPainter::Antialiasing, false);

    // 计算可见范围
    int viewH = viewport()->height();
    int firstRow = m_scrollOffset / m_rowHeight;
    int lastRow = firstRow + viewH / m_rowHeight + 1;

    // 计算可见的节点索引（考虑折叠的节点不显示）
    QVector<int> visibleNodeIdxs;
    int count = 0;
    for (int i = 0; i < m_nodes.size(); i++) {
        // 根节点不显示
        if (m_nodes[i].type == NodeRoot) continue;

        // 检查父节点是否展开
        int pId = m_nodes[i].parentId;
        bool visible = true;
        while (pId > 0) {
            int pIdx = findNodeById(pId);
            if (pIdx < 0 || !m_nodes[pIdx].expanded) {
                visible = false;
                break;
            }
            pId = m_nodes[pIdx].parentId;
        }

        if (visible) {
            if (count >= firstRow && count <= lastRow)
                visibleNodeIdxs.append(i);
            count++;
        }
    }

    // 绘制背景
    painter.fillRect(viewport()->rect(), m_colorEven);

    // 绘制可见行
    for (int i = 0; i < visibleNodeIdxs.size(); i++) {
        int nodeIdx = visibleNodeIdxs[i];
        int row = firstRow + i;
        int y = row * m_rowHeight - m_scrollOffset;

        bool isSelected = (nodeIdx == m_currentRow);
        bool isHovered = (nodeIdx == m_hoverRow);

        drawRow(painter, y, nodeIdx, isSelected, isHovered);
    }
}

void BaseTreeView::drawRow(QPainter &painter, int y, int nodeIdx,
                           bool isSelected, bool isHovered)
{
    const TreeNode &node = m_nodes[nodeIdx];
    int w = viewport()->width();
    int x = (node.level - 1) * m_indent; // level从1开始显示

    // 背景色
    if (isSelected) {
        painter.fillRect(0, y, w, m_rowHeight, m_colorSelectedBg);
        painter.setPen(m_colorSelectedText);
    } else if (isHovered) {
        painter.fillRect(0, y, w, m_rowHeight, m_colorHover);
        painter.setPen(m_colorText);
    } else {
        // 交替行色（按显示行号，不按数组索引）
        // 简单起见，用nodeIdx判断
        if (nodeIdx % 2 == 0)
            painter.fillRect(0, y, w, m_rowHeight, m_colorEven);
        else
            painter.fillRect(0, y, w, m_rowHeight, m_colorOdd);
        painter.setPen(m_colorText);
    }

    // 展开/折叠图标
    QRect iconRect(x, y, m_indent, m_rowHeight);
    drawExpandIcon(painter, iconRect, node.expanded, node.hasChildren);

    // 文本
    QRect textRect(x + m_indent, y, w - x - m_indent - 10, m_rowHeight);
    painter.drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, node.name);
}

void BaseTreeView::drawExpandIcon(QPainter &painter, const QRect &rect,
                                   bool expanded, bool hasChildren)
{
    if (!hasChildren) return;

    int cx = rect.center().x();
    int cy = rect.center().y();
    int size = 8;

    painter.save();
    painter.setPen(QPen(QColor("#6b7280"), 1));
    painter.setBrush(Qt::NoBrush);

    if (expanded) {
        // 展开：画 -
        painter.drawLine(cx - size/2, cy, cx + size/2, cy);
    } else {
        // 折叠：画 +
        painter.drawLine(cx - size/2, cy, cx + size/2, cy);
        painter.drawLine(cx, cy - size/2, cx, cy + size/2);
    }

    painter.restore();
}

// ---- 鼠标事件 ----

void BaseTreeView::mousePressEvent(QMouseEvent *event)
{
    int row = posToRow(event->pos());
    if (row < 0) return;

    // 计算点击的是第几个可见行，需要转换为节点索引
    // 重新计算可见行列表
    QVector<int> visibleIdxs;
    for (int i = 0; i < m_nodes.size(); i++) {
        if (m_nodes[i].type == NodeRoot) continue;
        int pId = m_nodes[i].parentId;
        bool visible = true;
        while (pId > 0) {
            int pIdx = findNodeById(pId);
            if (pIdx < 0 || !m_nodes[pIdx].expanded) {
                visible = false;
                break;
            }
            pId = m_nodes[pIdx].parentId;
        }
        if (visible)
            visibleIdxs.append(i);
    }

    if (row >= visibleIdxs.size()) return;
    int nodeIdx = visibleIdxs[row];

    if (event->button() == Qt::RightButton) {
        m_currentRow = nodeIdx;
        viewport()->update();
        emit nodeRightClicked(nodeIdx, m_nodes[nodeIdx], event->globalPos());
        return;
    }

    if (event->button() == Qt::LeftButton) {
        // 检查是否点击在展开图标上
        int level = m_nodes[nodeIdx].level;
        int iconX = (level - 1) * m_indent;
        int clickX = event->pos().x();
        if (clickX >= iconX && clickX < iconX + m_indent && m_nodes[nodeIdx].hasChildren) {
            toggleExpand(nodeIdx);
            return;
        }

        m_currentRow = nodeIdx;
        viewport()->update();
        emit nodeClicked(nodeIdx, m_nodes[nodeIdx]);
        emit currentChanged(nodeIdx, m_nodes[nodeIdx]);
    }
}

void BaseTreeView::mouseMoveEvent(QMouseEvent *event)
{
    int row = posToRow(event->pos());

    // 转换为节点索引
    QVector<int> visibleIdxs;
    for (int i = 0; i < m_nodes.size(); i++) {
        if (m_nodes[i].type == NodeRoot) continue;
        int pId = m_nodes[i].parentId;
        bool visible = true;
        while (pId > 0) {
            int pIdx = findNodeById(pId);
            if (pIdx < 0 || !m_nodes[pIdx].expanded) {
                visible = false;
                break;
            }
            pId = m_nodes[pIdx].parentId;
        }
        if (visible)
            visibleIdxs.append(i);
    }

    int hoverIdx = -1;
    if (row >= 0 && row < visibleIdxs.size())
        hoverIdx = visibleIdxs[row];

    if (hoverIdx != m_hoverRow) {
        m_hoverRow = hoverIdx;
        viewport()->update();
    }
}

void BaseTreeView::mouseDoubleClickEvent(QMouseEvent *event)
{
    int row = posToRow(event->pos());

    QVector<int> visibleIdxs;
    for (int i = 0; i < m_nodes.size(); i++) {
        if (m_nodes[i].type == NodeRoot) continue;
        int pId = m_nodes[i].parentId;
        bool visible = true;
        while (pId > 0) {
            int pIdx = findNodeById(pId);
            if (pIdx < 0 || !m_nodes[pIdx].expanded) {
                visible = false;
                break;
            }
            pId = m_nodes[pIdx].parentId;
        }
        if (visible)
            visibleIdxs.append(i);
    }

    if (row < 0 || row >= visibleIdxs.size()) return;
    int nodeIdx = visibleIdxs[row];

    m_currentRow = nodeIdx;
    viewport()->update();
    emit nodeDoubleClicked(nodeIdx, m_nodes[nodeIdx]);
}

void BaseTreeView::leaveEvent(QEvent *event)
{
    Q_UNUSED(event)
    if (m_hoverRow >= 0) {
        m_hoverRow = -1;
        viewport()->update();
    }
}

void BaseTreeView::wheelEvent(QWheelEvent *event)
{
    int delta = event->angleDelta().y();
    int maxScroll = qMax(0, visibleNodeCount() * m_rowHeight - viewport()->height());

    m_scrollOffset -= delta;
    if (m_scrollOffset < 0) m_scrollOffset = 0;
    if (m_scrollOffset > maxScroll) m_scrollOffset = maxScroll;

    verticalScrollBar()->setValue(m_scrollOffset);
    viewport()->update();
}

void BaseTreeView::keyPressEvent(QKeyEvent *event)
{
    if (m_nodes.size() <= 1) return; // 只有根节点

    // 计算可见节点列表
    QVector<int> visibleIdxs;
    for (int i = 0; i < m_nodes.size(); i++) {
        if (m_nodes[i].type == NodeRoot) continue;
        int pId = m_nodes[i].parentId;
        bool visible = true;
        while (pId > 0) {
            int pIdx = findNodeById(pId);
            if (pIdx < 0 || !m_nodes[pIdx].expanded) {
                visible = false;
                break;
            }
            pId = m_nodes[pIdx].parentId;
        }
        if (visible)
            visibleIdxs.append(i);
    }

    if (visibleIdxs.isEmpty()) return;

    // 找到当前选中在可见列表中的位置
    int visRow = -1;
    for (int i = 0; i < visibleIdxs.size(); i++) {
        if (visibleIdxs[i] == m_currentRow) {
            visRow = i;
            break;
        }
    }

    switch (event->key()) {
    case Qt::Key_Up:
        if (visRow > 0) {
            m_currentRow = visibleIdxs[visRow - 1];
            if (visRow * m_rowHeight < m_scrollOffset)
                m_scrollOffset = visRow * m_rowHeight;
        }
        break;
    case Qt::Key_Down:
        if (visRow < visibleIdxs.size() - 1) {
            m_currentRow = visibleIdxs[visRow + 1];
            int bottom = (visRow + 1) * m_rowHeight + m_rowHeight;
            if (bottom > m_scrollOffset + viewport()->height())
                m_scrollOffset = bottom - viewport()->height();
        }
        break;
    case Qt::Key_Left:
        if (m_currentRow >= 0 && m_nodes[m_currentRow].expanded)
            collapseNode(m_currentRow);
        break;
    case Qt::Key_Right:
        if (m_currentRow >= 0 && !m_nodes[m_currentRow].expanded
                && m_nodes[m_currentRow].hasChildren)
            expandNode(m_currentRow);
        break;
    case Qt::Key_Return:
    case Qt::Key_Enter:
        if (m_currentRow >= 0)
            emit nodeDoubleClicked(m_currentRow, m_nodes[m_currentRow]);
        break;
    default:
        break;
    }

    verticalScrollBar()->setValue(m_scrollOffset);
    viewport()->update();

    if (m_currentRow >= 0)
        emit currentChanged(m_currentRow, m_nodes[m_currentRow]);
}

void BaseTreeView::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)
    updateScrollBars();
}

// ---- 内部工具 ----

int BaseTreeView::visibleNodeCount() const
{
    int count = 0;
    for (int i = 0; i < m_nodes.size(); i++) {
        if (m_nodes[i].type == NodeRoot) continue;
        int pId = m_nodes[i].parentId;
        bool visible = true;
        while (pId > 0) {
            int pIdx = findNodeById(pId);
            if (pIdx < 0 || !m_nodes[pIdx].expanded) {
                visible = false;
                break;
            }
            pId = m_nodes[pIdx].parentId;
        }
        if (visible) count++;
    }
    return count;
}

void BaseTreeView::updateScrollBars()
{
    int total = visibleNodeCount() * m_rowHeight;
    int viewH = viewport()->height();

    verticalScrollBar()->setRange(0, qMax(0, total - viewH));
    verticalScrollBar()->setPageStep(viewH);
    verticalScrollBar()->setSingleStep(m_rowHeight);
}

int BaseTreeView::posToRow(const QPoint &pos) const
{
    if (pos.y() < 0 || pos.y() >= viewport()->height())
        return -1;
    return (pos.y() + m_scrollOffset) / m_rowHeight;
}

int BaseTreeView::rowToNodeIndex(int row) const
{
    Q_UNUSED(row)
    return -1; // 简化实现，需要时再补充
}

bool BaseTreeView::isOnExpandIcon(const QPoint &pos, int *rowIndex) const
{
    Q_UNUSED(pos)
    Q_UNUSED(rowIndex)
    return false; // 简化实现
}

void BaseTreeView::updateVisibleRows()
{
    // 预留接口
}
