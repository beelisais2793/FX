#include "mainwindow.h"
#include "annotation.h"
#include <QApplication>
#include <QFontDatabase>
#include <QImage>
#include <memory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFontDatabase::addApplicationFont(":/DejaVuLGCSans-ExtraLight.ttf");
    QFontDatabase::addApplicationFont(":/DejaVuLGCSansCondensed.ttf");
    Annotation anno;
    QImage new_splash;
    if (new_splash.load(QStringLiteral(":/anno_splash.png"))){
        anno.reloadSplash(new_splash);
    }
    MainWindow w;
    //запуск аннотации
    anno.show();
    w.show();
    anno.activateWindow();
    return a.exec();
}
