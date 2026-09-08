#include "controltoolboxdock.h"
#include "Common/commondef.h"

ControlToolBoxDock::ControlToolBoxDock(QWidget *parent)
    : QDockWidget("控件工具箱", parent)
{
    setObjectName("ControlToolBoxDock");
    initUi();
    initToolBox();
}

void ControlToolBoxDock::initUi()
{
    setFeatures(QDockWidget::DockWidgetMovable
              | QDockWidget::DockWidgetFloatable);
    setMinimumWidth(140);
}

QWidget* ControlToolBoxDock::createGroup(const QString &title,
    const QList<QPair<QString, QString>> &items)
{
    QWidget *container = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(container);
    layout->setContentsMargins(4, 4, 4, 4);
    layout->setSpacing(4);

    for (const auto &item : items) {
        DragWidget *dw = new DragWidget(item.first, item.second, container);
        layout->addWidget(dw);
    }

    layout->addStretch();
    return container;
}

void ControlToolBoxDock::initToolBox()
{
    m_toolBox = new QToolBox(this);
    m_toolBox->setObjectName("ControlToolBox");

    // 基础控件
    m_toolBox->addItem(createGroup(ToolBoxGroup::BasicControl, {
        {"开始节点",   ControlType::StartNode},
        {"结束节点",   ControlType::EndNode},
        {"条件分支",   ControlType::ConditionBranch},
        {"循环节点",   ControlType::LoopNode},
        {"延时节点",   ControlType::DelayNode},
    }), ToolBoxGroup::BasicControl);

    // 指令控件
    m_toolBox->addItem(createGroup(ToolBoxGroup::InstructionCtrl, {
        {"发送命令",   ControlType::SendCommand},
        {"读取数据",   ControlType::ReadData},
        {"写寄存器",   ControlType::WriteRegister},
        {"读寄存器",   ControlType::ReadRegister},
        {"等待响应",   ControlType::WaitResponse},
    }), ToolBoxGroup::InstructionCtrl);

    // 数据块控件
    m_toolBox->addItem(createGroup(ToolBoxGroup::DataBlockCtrl, {
        {"模拟量输入", ControlType::AnalogInput},
        {"数字量输入", ControlType::DigitalInput},
        {"模拟量输出", ControlType::AnalogOutput},
        {"数字量输出", ControlType::DigitalOutput},
        {"通讯参数",   ControlType::CommParam},
    }), ToolBoxGroup::DataBlockCtrl);

    // 判读控件
    m_toolBox->addItem(createGroup(ToolBoxGroup::JudgeCtrl, {
        {"阈值判读",   ControlType::ThresholdJudge},
        {"范围判读",   ControlType::RangeJudge},
        {"趋势判读",   ControlType::TrendJudge},
        {"逻辑组合",   ControlType::LogicCombine},
    }), ToolBoxGroup::JudgeCtrl);

    // 显示控件
    m_toolBox->addItem(createGroup(ToolBoxGroup::DisplayCtrl, {
        {"数值显示",   ControlType::NumericDisplay},
        {"状态指示",   ControlType::StatusIndicator},
        {"曲线图",     ControlType::CurveChart},
        {"数据表格",   ControlType::DataTable},
    }), ToolBoxGroup::DisplayCtrl);

    setWidget(m_toolBox);
}
