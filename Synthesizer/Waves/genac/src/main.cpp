#include "command.h"
#include <QtCore/QCoreApplication>
#include <QtCore/QCommandLineParser>
#include <ctime>

int main(int argc, char* argv[])
{
    srand(time(0));
    QCoreApplication app(argc, argv);
    QCommandLineParser parser;
    buildCommandLineParser(&parser, app);
    return runCommand(parser, app);
}
