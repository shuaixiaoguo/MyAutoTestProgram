#ifndef BASETREEVIEW_H
#define BASETREEVIEW_H

#include <QAbstractScrollArea>
#include <QVector>
#include <QString>
#include <QVariant>
#include <QModelIndex>

// 节点类型枚举（对应7级层级）
enum NodeType {
    NodeRoot = 0,       // 虚拟根节点（不显示）
    NodeCategory = 1,   // 总线类别
    NodeBus = 2,        // 总线
    NodeDevice = 3,     // 设备
    NodeInterface = 4,  // 接口
    NodeRoute = 5,      // 路由
    NodeDatablock = 6,  // 数据块
    NodeParameter = 7   // 参数
};

// 树节点数据结构
struct TreeNode {
    int       id = 0;           // 节点ID（数据库主键）
    int       parentId = 0;     // 父节点ID，0为根
    int       level = 0;        // 层级 0-7
    QString   name;             // 显示名称
    QString   code;             // 代号/标识
    NodeType  type = NodeRoot;  // 节点类型
    bool      expanded = false; // 是否展开
    bool      hasChildren = false; // 是否有子节点（懒加载用）
    bool      childrenLoaded = false; // 子节点是否已加载
    QVariant  userData;         // 附加数据

    TreeNode() {}
    TreeNode(int id_, int pid_, int level_, const QString &name_,
             const QString &code_ = QString(), NodeType type_ = NodeRoot)
        : id(id_), parentId(pid_), level(level_), name(name_)
        , code(code_), type(type_) {}
};

class BaseTreeView : public QAbstractScrollArea
{
    Q_OBJECT
public:
    explicit BaseTreeView(QWidget *parent = nullptr);
    ~BaseTreeView();

    // --- 数据操作 ---
    void setRootNode(const TreeNode &root);
    void appendChild(int parentId, const TreeNode &node);
    void insertNode(int index, const TreeNode &node);
    void removeNode(int index);
    void clear();

    int nodeCount() const;
    TreeNode nodeAt(int index) const;
    TreeNode currentNode() const;
    int currentIndex() const { return m_currentRow; }
    void setCurrentRow(int row);

    // 查找（按id）
    int findNodeById(int id) const;
    int findNodeByName(const QString &name) const;

    // --- 展开折叠 ---
    void expandNode(int index);
    void collapseNode(int index);
    void toggleExpand(int index);
    void expandAll();
    void collapseAll();

    // --- 子节点管理（懒加载用） ---
    void setChildrenLoaded(int parentId, bool loaded);
    void setHasChildren(int nodeId, bool has);
    void appendChildren(int parentId, const QVector<TreeNode> &children);
    void removeChildren(int parentId);

    // --- 样式设置 ---
    void setRowHeight(int height);
    void setIndent(int indent);
    void setRowColors(const QColor &even, const QColor &odd);
    void setSelectedColor(const QColor &bg, const QColor &text);
    void setHoverColor(const QColor &color);
    void setTextColor(const QColor &color);

signals:
    void nodeClicked(int index, const TreeNode &node);
    void nodeDoubleClicked(int index, const TreeNode &node);
    void nodeRightClicked(int index, const TreeNode &node, const QPoint &globalPos);
    void nodeExpanded(int index, const TreeNode &node);
    void nodeCollapsed(int index, const TreeNode &node);
    void currentChanged(int index, const TreeNode &node);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    // 计算可见行
    void updateVisibleRows();
    // 重新计算滚动条
    void updateScrollBars();
    // 行号 -> 节点索引
    int rowToNodeIndex(int row) const;
    // 坐标 -> 行号
    int posToRow(const QPoint &pos) const;
    // 坐标 -> 是否点击在展开图标上
    bool isOnExpandIcon(const QPoint &pos, int *rowIndex = nullptr) const;
    // 绘制一行
    void drawRow(QPainter &painter, int y, int nodeIdx, bool isSelected, bool isHovered);
    // 绘制展开图标
    void drawExpandIcon(QPainter &painter, const QRect &rect, bool expanded, bool hasChildren);
    // 可见节点数量
    int visibleNodeCount() const;

    // 扁平化的节点数组（按深度遍历顺序排列，用于快速索引）
    QVector<TreeNode> m_nodes;

    int m_rowHeight;
    int m_indent;           // 每层缩进像素
    int m_currentRow;
    int m_hoverRow;
    int m_scrollOffset;     // 垂直滚动偏移量（像素）

    // 颜色
    QColor m_colorEven;
    QColor m_colorOdd;
    QColor m_colorSelectedBg;
    QColor m_colorSelectedText;
    QColor m_colorHover;
    QColor m_colorText;
};

#endif // BASETREEVIEW_H
