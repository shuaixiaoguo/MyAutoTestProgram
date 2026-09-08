#-------------------------------------------------
# AgentFlow.pro - Qt自动化测试配置软件
# Qt 5.12.9
#-------------------------------------------------

QT += core gui widgets sql

CONFIG += c++11

TARGET = AgentFlow
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

# 源文件
SOURCES += \
    main.cpp \
    MainWindow/mainwindow.cpp \
    MainWindow/mainwindow_init.cpp \
    MainWindow/mainwindow_menu.cpp \
    DockWidgets/icdtreeviewdock.cpp \
    DockWidgets/controltoolboxdock.cpp \
    DockWidgets/propertypaneldock.cpp \
    DockWidgets/findresultdock.cpp \
    DockWidgets/runlogdock.cpp \
    CentralWidget/canvastabwidget.cpp \
    CentralWidget/mymdisubwindow.cpp \
    StatusBar/statusbar.cpp \
    Common/dragwidget.cpp \
    Common/basetableview.cpp \
    Common/basetreeview.cpp \
    Database/dbmanager.cpp \
    Database/projectmanager.cpp \
    Database/paramdatabase.cpp \
    Dialogs/databasesettingsdialog.cpp \
    Dialogs/newprojectdialog.cpp \
    Dialogs/modifyprojectdialog.cpp \
    ParamConfig/paramconfigdialog.cpp \
    ParamConfig/paramtree.cpp \
    ParamConfig/parampropertypanel.cpp \
    ParamConfig/nodepropertypanel.cpp

# 头文件
HEADERS += \
    MainWindow/mainwindow.h \
    DockWidgets/icdtreeviewdock.h \
    DockWidgets/controltoolboxdock.h \
    DockWidgets/propertypaneldock.h \
    DockWidgets/findresultdock.h \
    DockWidgets/runlogdock.h \
    CentralWidget/canvastabwidget.h \
    CentralWidget/mymdisubwindow.h \
    StatusBar/statusbar.h \
    Common/commondef.h \
    Common/dragwidget.h \
    Common/basetableview.h \
    Common/basetreeview.h \
    Database/dbmanager.h \
    Database/projectmanager.h \
    Database/paramdatabase.h \
    Dialogs/databasesettingsdialog.h \
    Dialogs/newprojectdialog.h \
    Dialogs/modifyprojectdialog.h \
    ParamConfig/paramconfigdialog.h \
    ParamConfig/paramtree.h \
    ParamConfig/parampropertypanel.h \
    ParamConfig/nodepropertypanel.h

# 资源文件
RESOURCES += \
    Resources/resources.qrc

# Windows Per-Monitor DPI Manifest
win32 {
    RC_FILE = Resources/app.rc
    OTHER_FILES += Resources/app.manifest
}

# 包含路径
INCLUDEPATH += \
    $$PWD \
    $$PWD/MainWindow \
    $$PWD/DockWidgets \
    $$PWD/CentralWidget \
    $$PWD/StatusBar \
    $$PWD/Common \
    $$PWD/Database \
    $$PWD/Dialogs \
    $$PWD/ParamConfig

# 输出路径
win32:CONFIG(release, debug|release): DESTDIR = $$PWD/bin/release
else:win32:CONFIG(debug, debug|release): DESTDIR = $$PWD/bin/debug

# 构建后自动复制 dll/ 目录下所有动态库到输出路径
win32 {
    DLL_DIR = $$PWD/dll
    DLL_DEST = $$DESTDIR
    win32:CONFIG(debug, debug|release) {
        QMAKE_POST_LINK += xcopy /Y /D \"$$DLL_DIR\\*.dll\" \"$$DLL_DEST\\\" > nul 2>&1 &
    } else {
        QMAKE_POST_LINK += xcopy /Y /D \"$$DLL_DIR\\*.dll\" \"$$DLL_DEST\\\" > nul 2>&1 &
    }
}
