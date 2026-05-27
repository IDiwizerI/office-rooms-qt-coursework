#include "MainWindow.h"

#include <QApplication>

/**
 * @brief Application entry point.
 *
 * Creates and shows the main Qt Widgets application window.
 *
 * @param argc Argument count.
 * @param argv Argument values.
 * @return Application execution result code.
 */
int main(int argc, char *argv[])
{
    QApplication application(argc, argv);

    MainWindow window;
    window.show();

    return application.exec();
}
