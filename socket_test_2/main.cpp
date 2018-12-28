#include "dialog.h"
#include "threat.h"
#include "mythread.h"
#include <QApplication>
#include <QThread>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

   // myclient client("127.0.1.1", 5025);
   // MyThread thread;

    Threat client("127.0.1.1", 5025);
    dialog menu(&client);

    menu.show();

    return a.exec();
}
