#include <QApplication>
#include <QCoreApplication>
#include <QWidget>

/**
 * @brief Application entry point.
 *
 * Creates a minimal Qt Widgets application window.
 *
 * @param argc Argument count.
 * @param argv Argument values.
 * @return Application execution result code.
 */
int main(int argc, char *argv[])
{
    QApplication application(argc, argv);

    QWidget window;
    window.setWindowTitle(QCoreApplication::translate("main", "Office Rooms"));
    window.resize(1000, 600);
    window.show();

    return application.exec();
}
