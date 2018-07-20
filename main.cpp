#include "widget.h"
#include <QApplication>



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.setWindowTitle("QPTZcontrol by Peter_White & ZZ");
    w.show();
    w.ConsoleAppend("*****System Start*****\n");
    return a.exec();
}
