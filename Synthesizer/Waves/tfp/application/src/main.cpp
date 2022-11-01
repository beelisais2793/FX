#include "tfp/tfp.hpp"
#include "tfp/views/MainWindow.hpp"
#include <QApplication>


// ----------------------------------------------------------------------------
int main(int argc, char** argv)
{
    // QApplication must exist before tfp_init() call.
    QApplication application(argc, argv);

    tfp_init(&argc, argv);

    tfp::MainWindow mainWindow;
    mainWindow.show();
    int result = application.exec();

    tfp_deinit();

    return result;
}
