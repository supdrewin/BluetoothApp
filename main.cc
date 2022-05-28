#include <QApplication>

#include "Bluetooth.hh"

int main(int argc, char** argv)
{
    QApplication application(argc, argv);

    Bluetooth window;

#if ANDROID
    window.showMaximized();
#else
    window.show();
#endif

    return application.exec();
}
