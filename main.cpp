#include "login.h"
#include "cards.h"
#include <QApplication>
#include <QResource>
#include <QThreadPool>

int main(int argc, char* argv[])
{
    // 设置线程池最大线程数
    QThreadPool::globalInstance()->setMaxThreadCount(8);
    QApplication a(argc, argv);
    qRegisterMetaType<Cards>("Cards&");
    QResource::registerResource("./resource.rcc");
    Login w;
    w.show();
    return a.exec();
}
