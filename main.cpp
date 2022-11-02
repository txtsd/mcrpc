#include <QCoreApplication>
#include <QTimer>

#include "mcrpc.h"


int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("mcrpc");

    mcrpc instance;

    QTimer::singleShot(0, &instance, &mcrpc::run);

    return QCoreApplication::exec();
}
