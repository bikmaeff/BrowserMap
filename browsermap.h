#ifndef BROWSERMAP_H
#define BROWSERMAP_H

#include <QFrame>
#include <QToolBar>
#include <QWebFrame>
#include <QWebView>

class QAction;
class QWebPage;
class QProgressBar;
class QUrl;
class QLabel;
class QSpinBox;

class BrowserMap : public QFrame
{
    Q_OBJECT
public:
    explicit BrowserMap(const QString &url=QString(),
                        QWebPage *webPage=0, QWidget *parent=0,
                        Qt::WindowFlags flags=0);

signals:
    void urlChanged(const QUrl &url);
    void loadFinished(bool ok);

public slots:
    void load(const QString &url);
    void setHtml(const QString &html);
    void showToolBar(bool on) { c_tool_bar->setVisible(on); }
    void enableAction(bool enable);

private slots:
    void zoomOut();
    void zoomIn();
    void setUrl();
    void setZoomFactor(int zoom);
    void popUpHistoryMenu();
    void loadFinish(bool ok);
    void urlChange(const QUrl &url);

protected:
    void focusInEvent(QFocusEvent*) { c_web_view->setFocus(); }

private:
    void createActions();
    void createToolBar();
    void createLayout();
    void createConnections();

    QAction *c_zoom_out_action;
    QAction *c_zoom_in_action;
    QAction *c_set_url_action;
    QAction *c_history_action;

    QWebView *c_web_view;
    QToolBar *c_tool_bar;
    QSpinBox *c_zoom_spin_box;
    QProgressBar *c_progress_bar;
    QLabel *c_progress_label;
};

#endif // BROWSERMAP_H
