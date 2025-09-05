#include "loading.h"
#include "login.h"
#include "cards.h"
#include <QApplication>
#include <QResource>
int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    qRegisterMetaType<Cards>("Cards&");
    QResource::registerResource("./resource.rcc");
    // Loading w;
    // w.show();
    Login w;
    w.show();
    return a.exec();
}
