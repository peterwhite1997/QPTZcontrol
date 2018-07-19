#include "widget.h"
#include <QApplication>



int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    Widget w;
    w.setWindowTitle("QPTZcontrol by Peter_White");
    w.show();
    w.ConsoleAppend("*****System Start*****\n");

    // 初始化SDK
    if(CLIENT_Init(NULL, 0))
    {
        w.ConsoleAppend("Initialize client SDK done\n");
        w.ConsoleAppend("SDK Version: ");
        w.ConsoleAppend(QString::number(CLIENT_GetSDKVersion()));
        w.ConsoleAppend("\n");

    }
    return a.exec();
}
