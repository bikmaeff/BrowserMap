#include "browsermap.h"
#include <QAction>
#include <QFontMetrics>
#include <QInputDialog>
#include <QLabel>
#include <QMenu>
#include <QProgressBar>
#include <QSet>
#include <QSpinBox>
#include <QUrl>
#include <QVBoxLayout>
#include <QWebHistory>
#include <QWebPage>

namespace {
const int MaxHistoryMenuItems = 20;
const int MaxMenuWidth = 300;
const int ZoomStepSize = 5;
}

BrowserMap::BrowserMap(const QString &url, QWebPage *webPage, QWidget *parent,
                       Qt::WindowFlags flags) : QFrame(parent)
{
    setFrameStyle(QFrame::Box|QFrame::Raised);

    c_web_view = new QWebView;
    if(c_web_view)
        c_web_view->setPage(webPage);
    load(url);

    createActions();
    createToolBar();
    createLayout();
    createConnections();
}

void BrowserMap::createActions()
{
    c_zoom_out_action = new QAction(QIcon(":image/zoomout.png"),trUtf8("Zoom Out"),this);
    c_zoom_out_action->setShortcut(QKeySequence::ZoomOut);

    c_zoom_in_action = new QAction(QIcon(":image/zoomin.png"),trUtf8("Zoom In"), this);
    c_zoom_in_action->setShortcut(QKeySequence::ZoomIn);

    c_set_url_action = new QAction(QIcon(":image/seturl.png"),trUtf8("Введи URL..."),this);
    c_set_url_action->setShortcut(QKeySequence("F2"));

    c_history_action = new QAction(QIcon(":image/history.png"),trUtf8("История..."), this);

    QList<QAction*> actions;
    actions << c_web_view->pageAction(QWebPage::Back)
            << c_web_view->pageAction(QWebPage::Forward)
            << c_web_view->pageAction(QWebPage::Reload)
            << c_web_view->pageAction(QWebPage::Stop)
            << c_web_view->pageAction(QWebPage::InspectElement)
            << c_zoom_out_action << c_zoom_in_action<< c_set_url_action
            << c_history_action;

    c_web_view->addActions(actions);
    c_web_view->setContextMenuPolicy(Qt::ActionsContextMenu);
}

void BrowserMap::createToolBar()
{
    c_tool_bar = new QToolBar(trUtf8("Navigation"), this);
    c_tool_bar->addAction(c_web_view->pageAction(QWebPage::Back));
    c_tool_bar->addAction(c_web_view->pageAction(QWebPage::Forward));
    c_tool_bar->addAction(c_web_view->pageAction(QWebPage::Reload));
    c_tool_bar->addAction(c_web_view->pageAction(QWebPage::Stop));

    c_tool_bar->addSeparator();
    c_tool_bar->addAction(c_zoom_out_action);
    c_tool_bar->addAction(c_zoom_in_action);
    c_tool_bar->addSeparator();

    c_zoom_spin_box = new QSpinBox;
    c_zoom_spin_box->setSingleStep(ZoomStepSize);
    c_zoom_spin_box->setRange(25,250);
    c_zoom_spin_box->setValue(100);
    c_zoom_spin_box->setSuffix(trUtf8("%"));
    c_zoom_spin_box->setFocusPolicy(Qt::NoFocus);

    QLabel *zoomLabel = new QLabel(trUtf8("Zoom"));
    zoomLabel->setMargin(4);
    c_progress_bar = new QProgressBar;
    c_progress_label = new QLabel(trUtf8("Progress"));
    c_progress_label->setMargin(4);

    QWidget *zoomWidget = new QWidget;
    QVBoxLayout *zoomLayout = new QVBoxLayout;
    zoomLayout->addWidget(c_zoom_spin_box);
    zoomLayout->addWidget(zoomLabel);
    zoomLayout->setAlignment(zoomLabel, Qt::AlignCenter);
    zoomWidget->setLayout(zoomLayout);
    c_tool_bar->addWidget(zoomWidget);
    c_tool_bar->addSeparator();
    c_tool_bar->addAction(c_set_url_action);
    c_tool_bar->addAction(c_history_action);
    QWidget *progressWidget = new QWidget;
    QVBoxLayout *progressLayout = new QVBoxLayout;
    progressLayout->setAlignment(Qt::AlignCenter);
    progressLayout->addWidget(c_progress_bar);
    progressLayout->addWidget(c_progress_label);
    progressLayout->setAlignment(c_progress_label, Qt::AlignCenter);
    progressWidget->setLayout(progressLayout);
    c_tool_bar->addWidget(progressWidget);
}

void BrowserMap::createLayout()
{
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(c_web_view);
    layout->addWidget(c_tool_bar);
    setLayout(layout);
}

void BrowserMap::createConnections()
{
    connect(c_web_view, SIGNAL(loadProgress(int)), c_progress_bar, SLOT(setValue(int)));
    connect(c_web_view, SIGNAL(urlChanged(const QUrl&)), this, SLOT(urlChange(const QUrl&)));
    connect(c_web_view, SIGNAL(loadFinished(bool)), this, SLOT(loadFinish(bool)));
    connect(c_set_url_action, SIGNAL(triggered()), this, SLOT(setUrl()));
    connect(c_history_action, SIGNAL(triggered()), this, SLOT(popUpHistoryMenu()));
    connect(c_zoom_out_action, SIGNAL(triggered()), this, SLOT(zoomOut()));
    connect(c_zoom_in_action, SIGNAL(triggered()), this, SLOT(zoomIn()));
    connect(c_zoom_spin_box, SIGNAL(valueChanged(int)), this, SLOT(setZoomFactor(int)));
}


void BrowserMap::load(const QString &url)
{
    if (url.isEmpty())
        return;
    QString theUrl = url;
    if (!theUrl.contains("://"))
        theUrl.prepend("http://");
    c_web_view->load(theUrl);
}

void BrowserMap::setHtml(const QString &html)
{
    c_web_view->setHtml(html);
}

void BrowserMap::enableAction(bool enable)
{
    foreach (QAction *action, c_web_view->actions())
        action->setEnabled(enable);
    c_tool_bar->setEnabled(enable);
    c_web_view->setContextMenuPolicy(enable ? Qt::ActionsContextMenu
                                         : Qt::NoContextMenu);
}

void BrowserMap::zoomOut()
{
    c_zoom_spin_box->setValue(c_zoom_spin_box->value() - ZoomStepSize);
}

void BrowserMap::zoomIn()
{
    c_zoom_spin_box->setValue(c_zoom_spin_box->value() + ZoomStepSize);
}

void BrowserMap::setUrl()
{
    load(QInputDialog::getText(this, tr("Set URL"), tr("&URL:")));
}

void BrowserMap::setZoomFactor(int zoom)
{
    c_web_view->setZoomFactor(zoom / 100.0);
    //c_web_view->setTextSizeMultiplier(zoom / 100.0);
}

void BrowserMap::popUpHistoryMenu()
{
    QFontMetrics fontMetrics(font());
    QMenu menu;
    QSet<QUrl> uniqueUrls;
    QListIterator<QWebHistoryItem> i(c_web_view->history()->items());
    i.toBack();
    while (i.hasPrevious() &&
           uniqueUrls.count() < MaxHistoryMenuItems) {
        const QWebHistoryItem &item = i.previous();
        if (uniqueUrls.contains(item.url()))
            continue;
        uniqueUrls << item.url();
        QString title = fontMetrics.elidedText(item.title(),
                Qt::ElideRight, MaxMenuWidth);
        QAction *action = new QAction(item.icon(), title, &menu);
        action->setData(item.url());
        menu.addAction(action);
    }
}

void BrowserMap::loadFinish(bool ok)
{
    emit loadFinished(ok);
    c_progress_label->setText(ok ? trUtf8("Загружено") : trUtf8("Canceled"));
}

void BrowserMap::urlChange(const QUrl &url)
{
    emit urlChanged(url);
    c_progress_label->setText(trUtf8("Идет загрузка"));
}

