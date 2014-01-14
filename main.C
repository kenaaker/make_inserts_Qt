#include "make_image_inserts.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    make_image_inserts w;
    w.show();

    return a.exec();
}
