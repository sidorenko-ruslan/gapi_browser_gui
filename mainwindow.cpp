#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QWebEngineCookieStore>
#include <QWebEngineProfile>
#include <QWebEnginePage>
#include <QFile>

CookieDialog::CookieDialog(const QNetworkCookie &cookie, QWidget *parent): QDialog(parent) {
    setupUi(this);
    m_nameLineEdit->setText(cookie.name());
    m_domainLineEdit->setText(cookie.domain());
    m_valueLineEdit->setText(cookie.value());
    m_pathLineEdit->setText(cookie.path());
    m_dateEdit->setDate(cookie.expirationDate().date());
    m_isSecureComboBox->addItem(cookie.isSecure() ? tr("yes") : tr("no"));
    m_isHttpOnlyComboBox->addItem(cookie.isHttpOnly() ? tr("yes") : tr("no"));
    m_addButton->setVisible(false);
    m_cancelButton->setText(tr("Close"));
}

CookieDialog::CookieDialog(QWidget *parent): QDialog(parent) {
    setupUi(this);
    m_nameLineEdit->setReadOnly(false);
    m_domainLineEdit->setReadOnly(false);
    m_valueLineEdit->setReadOnly(false);
    m_pathLineEdit->setReadOnly(false);
    m_dateEdit->setReadOnly(false);
    m_dateEdit->setDate(QDateTime::currentDateTime().addYears(1).date());
    m_isSecureComboBox->addItem(tr("no"));
    m_isSecureComboBox->addItem(tr("yes"));
    m_isHttpOnlyComboBox->addItem(tr("no"));
    m_isHttpOnlyComboBox->addItem(tr("yes"));
}

QNetworkCookie CookieDialog::cookie() {
    QNetworkCookie cookie;
    cookie.setDomain(m_domainLineEdit->text());
    cookie.setName(m_nameLineEdit->text().toLatin1());
    cookie.setValue(m_valueLineEdit->text().toLatin1());
    cookie.setExpirationDate(QDateTime(m_dateEdit->date()));
    cookie.setPath(m_pathLineEdit->text());
    cookie.setSecure(m_isSecureComboBox->currentText() == tr("yes"));
    cookie.setHttpOnly(m_isHttpOnlyComboBox->currentText() == tr("yes"));
    return cookie;
}

CookieWidget::CookieWidget(const QNetworkCookie &cookie, QWidget *parent): QWidget(parent) {
    setupUi(this);
    setAutoFillBackground(true);
    m_nameLabel->setText(cookie.name());
    m_domainLabel->setText(cookie.domain());
    connect(m_viewButton, &QPushButton::clicked, this, &CookieWidget::viewClicked);
    connect(m_deleteButton, &QPushButton::clicked, this, &CookieWidget::deleteClicked);
}

void CookieWidget::setHighlighted(bool enabled) {
    QPalette p = palette();
    p.setColor(backgroundRole(), enabled ? QColor(0xF0, 0xF8, 0xFF) : Qt::white);
    setPalette(p);
}

MainWindow::MainWindow(const QUrl &url) :
    QMainWindow(), m_store(nullptr), m_layout(new QVBoxLayout),
    commandListener(new RemoteCommandListener(this)) {
    setAttribute(Qt::WA_DeleteOnClose, true);

    QFile file;
    file.setFileName(":/jquery.min.js");
    file.open(QIODevice::ReadOnly);
    jQuery = file.readAll();
    jQuery.append("\nvar qt = { 'jQuery': jQuery.noConflict(true) };");
    file.close();

    connect(webView,SIGNAL(loadFinished(bool)),this,SLOT(finishLoading()));
    connect(commandListener,SIGNAL(commandReceived(ClientCommand)),this,SLOT(executeCommand(ClientCommand)));
    connect(this,SIGNAL(commandCompleted(QString)),commandListener,SLOT(sendReply(QString)));

    setupUi(this);
    m_urlLineEdit->setText(url.toString());

    m_layout->addItem(new QSpacerItem(0,0, QSizePolicy::Minimum, QSizePolicy::Expanding));
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);

    QWidget *w = new QWidget();
    QPalette p = w->palette();
    p.setColor(widget->backgroundRole(), Qt::white);
    w->setPalette(p);
    w->setLayout(m_layout);

    m_scrollArea->setWidget(w);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    connect(m_urlButton, &QPushButton::clicked, this, &MainWindow::handleUrlClicked);
    connect(m_deleteAllButton, &QPushButton::clicked, this, &MainWindow::handleDeleteAllClicked);
    connect(m_newButton, &QPushButton::clicked, this, &MainWindow::handleNewClicked);

    m_store = webView->page()->profile()->cookieStore();
    connect(m_store, &QWebEngineCookieStore::cookieAdded, this, &MainWindow::handleCookieAdded);
    m_store->loadAllCookies();
    //webView->page()->load(url);
}

bool MainWindow::startServer(uint _portNumber) {
    return commandListener->start(_portNumber);
}

MainWindow::~MainWindow() {
    webView->page()->profile()->cookieStore()->deleteAllCookies();
    webView->page()->profile()->clearHttpCache();
}

bool MainWindow::containsCookie(const QNetworkCookie &cookie) {
    for (auto c: m_cookies) {
        if (c.hasSameIdentifier(cookie))
            return true;
    }
    return false;
}

void MainWindow::handleCookieAdded(const QNetworkCookie &cookie) {
    // only new cookies
    if (containsCookie(cookie))
        return;

    CookieWidget *widget = new CookieWidget(cookie);
    widget->setHighlighted(m_cookies.count() % 2);
    m_cookies.append(cookie);
    m_layout->insertWidget(0,widget);

    connect(widget, &CookieWidget::deleteClicked, [this, cookie, widget]() {
        m_store->deleteCookie(cookie);
        delete widget;
        m_cookies.removeOne(cookie);
        for (int i = 0; i < m_layout->count() - 1; i++) {
            // fix background colors
            auto widget = qobject_cast<CookieWidget*>(m_layout->itemAt(i)->widget());
            widget->setHighlighted(i % 2);
        }
    });

    connect(widget, &CookieWidget::viewClicked, [cookie]() {
        CookieDialog dialog(cookie);
        dialog.exec();
    });
}

void MainWindow::handleDeleteAllClicked() {
    m_store->deleteAllCookies();
    for (int i = m_layout->count() - 1; i >= 0; i--)
        delete m_layout->itemAt(i)->widget();
    m_cookies.clear();
}

void MainWindow::handleNewClicked() {
    CookieDialog dialog;
    if (dialog.exec() == QDialog::Accepted)
        m_store->setCookie(dialog.cookie());
}

void MainWindow::handleUrlClicked() {
    webView->page()->load(QUrl::fromUserInput(m_urlLineEdit->text()));
}


///////////////////////////////////////////////////////////////////////

void MainWindow::executeCommand(const ClientCommand& command) {
    switch (command.type) {
        case CommandType::Goto: {
            gotoPage(command.data);
            emit commandCompleted("successs");
            break;
        }
        case CommandType::ExecuteScript: {
            executeScript(command.data, ScriptType::Custom);
            break;
        }
        case CommandType::Element: {
            executeScript(command.data, ScriptType::Element);
            break;
        }
        case CommandType::Click: {
            executeScript(command.data, ScriptType::Click);
            break;
        }
        case CommandType::CreatePdf: {
            createPdf(command.data);
            break;
        }
        default: {
            break;
        }
    }
}

void MainWindow::gotoPage(const QString& commandData) {
    m_urlLineEdit->setText(commandData);
    webView->page()->load(QUrl(commandData));
}

void MainWindow::executeScript(const QString& commandData, ScriptType scriptType) {
    if (scriptType == ScriptType::Custom) {
        webView->page()->runJavaScript(commandData, 1, [this](const QVariant & v) {
            qDebug() << "JS result: " << v;
        });
    }
    else if (scriptType == ScriptType::Element) {
        webView->page()->runJavaScript(commandData, 1, [this](const QVariant & v) {
            qDebug() << "JS result: " << v;
        });
    }
    else if (scriptType == ScriptType::Click) {

    }
}

void MainWindow::createPdf(const QString& commandData) {
    webView->page()->printToPdf(commandData);
}

void MainWindow::finishLoading(bool ok) {
    if (ok) {
        webView->page()->runJavaScript(jQuery);
    }
}
