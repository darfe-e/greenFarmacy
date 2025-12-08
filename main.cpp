// main.cpp
#include "qt_classes/mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Устанавливаем стиль приложения
    app.setStyle("Fusion");

    MainWindow window;
    window.show();

    return app.exec();
}
