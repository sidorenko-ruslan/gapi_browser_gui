#ifndef WEBVIEW_H
#define WEBVIEW_H

#include <QWebEngineView>

class WebView : public QWebEngineView {
    Q_OBJECT
public:
    WebView(QWidget* parent = nullptr);
protected:
    void contextMenuEvent(QContextMenuEvent *event) override;

signals:
    void devToolsRequested(QWebEnginePage *source);
};


#endif // WEBVIEW_H
