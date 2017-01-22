#include <QApplication>
#include <QWebSettings>
#include <QString>
#include <QDialog>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QVBoxLayout>

#include "browsermap.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    app.setApplicationName(app.trUtf8("Просмотр карты местности", "Browser Map"));

    QWebSettings *webSettings = QWebSettings::globalSettings();
    webSettings->setAttribute(QWebSettings::AutoLoadImages, true);
    webSettings->setAttribute(QWebSettings::JavascriptEnabled, true);
    webSettings->setAttribute(QWebSettings::PluginsEnabled, true);
    webSettings->setAttribute(QWebSettings::ZoomTextOnly, true);
    webSettings->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);

    QString url("www.google.ru/maps");
    if (argc > 1)
        url = argv[1];
    QDialog dialog;
    BrowserMap *browser = new BrowserMap(url);
    if (argc > 2) browser->showToolBar(false); // For testing; don't quote
    QDialogButtonBox *buttonBox = new QDialogButtonBox;

    QPushButton *quitButton = buttonBox->addButton(
            app.translate("main", "&Quit"),
            QDialogButtonBox::AcceptRole);
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(browser, 1);
    layout->addWidget(buttonBox);
    dialog.setLayout(layout);
    QObject::connect(quitButton, SIGNAL(clicked()),
                     &dialog, SLOT(accept()));
    dialog.setWindowTitle(app.applicationName());
    dialog.setWindowFlags(Qt::WindowSystemMenuHint);
    dialog.show();
    return app.exec();

}
