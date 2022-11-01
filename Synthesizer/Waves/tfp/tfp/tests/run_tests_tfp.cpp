#include "tfp/tfp.hpp"
#include <QApplication>

int main(int argc, char** argv)
{
    QApplication application(argc, argv);

    tfp_init(&argc, argv);
    int result = tfp_run_tests(&argc, argv);
    tfp_deinit();

    return result;
}
