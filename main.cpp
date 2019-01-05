#include "mainwindow.h"
#include <QApplication>
#include <QUrl>
#include <QDir>
#include <iostream>

int main(int argc, char *argv[]) {
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);
    QDir::setCurrent(qApp->applicationDirPath());
    QCoreApplication::setApplicationName("gap browser gui");
    QCoreApplication::setApplicationVersion("0.0.1");

    MainWindow w(QUrl("https://www.adorama.com"));
    if (!w.startServer(3000)) {
        std::cerr << "unable to start listen server!\n";
        return -1;
    }
    w.resize(1024, 768);
    w.show();

    return a.exec();
}
