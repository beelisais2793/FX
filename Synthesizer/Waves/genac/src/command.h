#ifndef COMMAND_H
#define COMMAND_H

class QCommandLineParser;
class QCoreApplication;

void buildCommandLineParser(QCommandLineParser* parser, const QCoreApplication& app);
int runCommand(QCommandLineParser& parser, QCoreApplication& app);

#endif /* COMMAND_H */
