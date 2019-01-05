#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include "ui_cookiewidget.h"
#include "ui_cookiedialog.h"
#include <QNetworkCookie>
#include <QMainWindow>
#include "remote_command_listener.h"

class QWebEngineCookieStore;

class CookieDialog : public QDialog, public Ui_CookieDialog
{
    Q_OBJECT
public:
    CookieDialog(const QNetworkCookie &cookie, QWidget *parent = nullptr);
    CookieDialog(QWidget *parent = 0);
    QNetworkCookie cookie();
};

class CookieWidget : public QWidget,  public Ui_CookieWidget
{
    Q_OBJECT
public:
    CookieWidget(const QNetworkCookie &cookie, QWidget *parent = nullptr);
    void setHighlighted(bool enabled);
signals:
    void deleteClicked();
    void viewClicked();
};

class MainWindow : public QMainWindow, public Ui_MainWindow
{
    Q_OBJECT

    enum class ScriptType {
        Custom,
        Element,
        Click
    };

public:
    explicit MainWindow(const QUrl &url);
    bool startServer(uint _portNumber);
    ~MainWindow();
signals:
    void commandCompleted(const QString& data);
private slots:
    void handleCookieAdded(const QNetworkCookie &cookie);
    void handleDeleteAllClicked();
    void handleNewClicked();
    void handleUrlClicked();
    void executeCommand(const ClientCommand& command);

    void finishLoading(bool ok);

private:
    bool containsCookie(const QNetworkCookie &cookie);
    // Browser commands list
    void gotoPage(const QString& commandData);
    void createPdf(const QString& commandData);
    void executeScript(const QString& commandData, ScriptType scriptType);
    // --------------------


    QWebEngineCookieStore *m_store;
    QVector<QNetworkCookie> m_cookies;
    QVBoxLayout *m_layout;
    RemoteCommandListener* commandListener;

    QString jQuery;
};

#endif // MAINWINDOW_H
