#include <QApplication>
#include <QStyleFactory>
#include <QScreen>
#include <QFile>
#include <QFont>
#include "MainWindow/mainwindow.h"

int main(int argc, char *argv[])
{
    // === 高DPI配置（必须在QApplication创建前设置）===
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    QApplication::setStyle(QStyleFactory::create("Fusion"));

    QApplication app(argc, argv);
    app.setApplicationName("AgentFlow");
    app.setOrganizationName("AgentFlow");

    // 设置全局默认字体（必须在QApplication创建之后调用）
    QFont appFont = app.font();
    appFont.setPointSize(9);
    app.setFont(appFont);

    // 加载QSS样式
    QFile qssFile(":/styles/styles.qss");
    if (qssFile.open(QFile::ReadOnly | QFile::Text)) {
        app.setStyleSheet(QString::fromUtf8(qssFile.readAll()));
        qssFile.close();
    }

    MainWindow w;
    w.showMaximized();

    return app.exec();
}
