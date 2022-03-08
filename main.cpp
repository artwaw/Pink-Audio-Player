#include "Player.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QCoreApplication::setApplicationVersion("0.10.2");
    QCoreApplication::setOrganizationName("TrollNet");
    QCoreApplication::setOrganizationDomain("trollnet.com.pl");
    QCoreApplication::setApplicationName("Pink Audio Player");
    QApplication a(argc, argv);
    Player w;
    w.show();
    return a.exec();
}
