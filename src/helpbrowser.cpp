#include "helpbrowser.h"
#include <QTabWidget>
#include <QSplitter>
#include <QHelpContentWidget>
#include <QHelpIndexWidget>


const QString HelpBrowser::defaultHelpPath = QString("docs/help.qhc");
const QString HelpBrowser::startPage = QString("qthelp://coursework.help/docs/index.html");

HelpBrowser::HelpBrowser(const QString& collectionPath, QWidget* parent): QTextBrowser(parent) {
    helpEngine = new QHelpEngine(collectionPath);
    helpEngine->setupData();
}

QVariant HelpBrowser::loadResource(int type, const QUrl &name) {
    if (name.scheme() == "qthelp")
        return QVariant(helpEngine->fileData(name));
    else
        return QTextBrowser::loadResource(type, name);
}

QHelpContentWidget* HelpBrowser::getContentWidget() {
    return helpEngine->contentWidget();
}

QHelpIndexWidget *HelpBrowser::getIndexWidget() {
    return helpEngine->indexWidget();
}

HelpBrowser::~HelpBrowser() {
    delete helpEngine;
}

HelpDockWidget::HelpDockWidget(QWidget* parent): QDockWidget(parent) {
    QSplitter* horizontalSplitter = new QSplitter(Qt::Horizontal, this);
    QTabWidget* tabWidget = new QTabWidget(horizontalSplitter);
    HelpBrowser* helpBrowser = new HelpBrowser(HelpBrowser::defaultHelpPath, horizontalSplitter);

    tabWidget->setMaximumWidth(320);
    tabWidget->addTab(helpBrowser->getContentWidget(), "Contents");
    tabWidget->addTab(helpBrowser->getIndexWidget(), "Index");
    horizontalSplitter->insertWidget(0, tabWidget);
    horizontalSplitter->insertWidget(1, helpBrowser);
    horizontalSplitter->hide();

    helpBrowser->setSource(QUrl(HelpBrowser::startPage));

    setWidget(horizontalSplitter);

    connect(helpBrowser->getContentWidget(), SIGNAL(linkActivated(const QUrl&)), helpBrowser, SLOT(setSource(const QUrl&)));
    connect(helpBrowser->getIndexWidget(), SIGNAL(linkActivated(const QUrl&, const QString&)), helpBrowser, SLOT(setSource(const QUrl&)));

    hide();
}
