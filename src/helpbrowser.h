#ifndef HELPBROWSER_H
#define HELPBROWSER_H

#include <QTextBrowser>
#include <QHelpEngine>
#include <QDockWidget>


class HelpBrowser: public QTextBrowser {
    Q_OBJECT

public:
    static const QString defaultHelpPath;
    static const QString startPage;

    HelpBrowser(const QString& collectionPath, QWidget* parent = nullptr);

    QVariant loadResource(int type, const QUrl &name);
    QHelpContentWidget* getContentWidget();
    QHelpIndexWidget* getIndexWidget();

    ~HelpBrowser();

private:
    QHelpEngine* helpEngine = nullptr;
};


class HelpDockWidget: public QDockWidget {
    Q_OBJECT

public:
    HelpDockWidget(QWidget* parent = nullptr);

private:
    HelpBrowser* helpBrowser = nullptr;
};


#endif // HELPBROWSER_H
