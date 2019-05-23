#include "mainwindow.h"
#include <QApplication>
#include <QtCore/QDir>
#include <QtCore/QString>
#include <QtCore/QCommandLineParser>
#include <iostream>
#include <QtGlobal>
#include <QNetworkProxy>

void initEnvVars(int port);


int main(int argc, char *argv[]) {

    QNetworkProxy proxy;
    proxy.setType(QNetworkProxy::Socks5Proxy);
    proxy.setHostName("us-wa.proxymesh.com");
    proxy.setPort(31280);
    proxy.setUser("gapcrawler");
    proxy.setPassword("gapcrawler");
    QNetworkProxy::setApplicationProxy(proxy);

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);



    QDir::setCurrent(qApp->applicationDirPath());
    QCoreApplication::setApplicationName("Gap Browser gui");
    QCoreApplication::setApplicationVersion("0.0.1");

    QCommandLineParser parser;
    parser.setApplicationDescription("Simple automation browser");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("port","Listening port");
    parser.process(app);
    int port = parser.positionalArguments().at(1).toInt();

    initEnvVars(port);

    qputenv("QTWEBENGINE_REMOTE_DEBUGGING","9230");
    MainWindow w;
    if (!w.startServer(port)) {
        std::cerr << "unable to start listen server!\n";
        return -1;
    }
    w.resize(1024, 768);
    w.show();

    return app.exec();
}

void initEnvVars(int port) {
    //qputenv("GAPI_BROWSER_PORT", QString::number(port));
}
