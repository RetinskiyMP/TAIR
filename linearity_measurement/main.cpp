#include "gui.h"
#include "handler.h"

#include <QApplication>
#include <QTcpSocket>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qRegisterMetaType<QAbstractSocket::SocketError>();
    qRegisterMetaType<QList<QPointF>>();
    qRegisterMetaType<QList<QString>>();

    GUI window;
    window.show();

    return a.exec();
}
