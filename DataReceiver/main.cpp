#pragma execution_character_set("utf-8")

#include "DataReceiver.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DataReceiver w;
    w.show();
    return a.exec();
}
