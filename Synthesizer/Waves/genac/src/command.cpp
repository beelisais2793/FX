#include "command.h"
#include "config.h"
#include "Analyzer/circuit.h"
#include "Analyzer/netlist.h"
#include "Analyzer/analyzer.h"
#include "Analyzer/pretty.h"
#include "Synthesizer/geneticsynthesizer.h"
#include "dbg.h"
#include <QtCore/QCoreApplication>
#include <QtCore/QCommandLineParser>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QTimer>
#include <QtCore/QTextStream>
#include <QtCore/QRegExp>

static int analyze(const QString& netlistFile, const QString& libraryDirectory)
{
    QTextStream console(stdout);
    Netlist netlist;
    QFile file(netlistFile);

   if(file.open(QIODevice::ReadOnly | QIODevice::Text)) {
           QTextStream stream(&file);
           netlist.parse(stream, libraryDirectory);
    } else {
       qDebug() << "Cannot read" << netlistFile;
       return -1;
    }

    Circuit *circuit = &netlist.circuit();
    Analyzer analyzer(circuit);
    analyzer.solve();
    QList<Analyzer::TransferFunction> results = analyzer.calcTFs(
                netlist.transferFunctions());
    QString output;
    for(int i = 0; i < results.size(); ++i){
        output += pretty(results.at(i));
        output += "\n";
    }
    console << output;
    console.flush();
    return 0;
}

static int genetic_synthesize(const QString& nreq, const QString& dreq,
            const QString& active, int psize, int generations,
            const QString& selection, const QString& xover,
            float xp, float mp, int cc, int rc, bool adaptivem,
             const QString& outputdir)
{
    GeneticSynthesizer gs;
    gs.run(nreq, dreq, active, psize, generations, selection,
           xover, xp, mp, cc, rc, adaptivem, outputdir);
    return 0;
}

void buildCommandLineParser(QCommandLineParser* parser, const QCoreApplication& app)
{
    QCoreApplication::setApplicationName(GENAC);
    QCoreApplication::setApplicationVersion(GENAC_VERSION);
    parser->setApplicationDescription(GENAC_DESCRIPTION);
    parser->addHelpOption();
    parser->addVersionOption();

    parser->addPositionalArgument("command",
                    QCoreApplication::translate("main", "analyze | genetic-synthesize"));

    parser->parse(QCoreApplication::arguments());
    const QStringList args = parser->positionalArguments();
    const QString command = args.isEmpty() ? QString() : args.first();
    if (command == "genetic-synthesize") {
        parser->clearPositionalArguments();
        parser->addPositionalArgument("genetic-synthesize",
                QCoreApplication::translate("main", "Generate a circuit which meets given requirements"));
        parser->addOptions({
            {"numerator",
                QCoreApplication::translate("main", "Search for binary numerator <requirements> (e.g. 001) Should match denominator in size."),
                QCoreApplication::translate("main", "requirements")},
            {"denominator",
                QCoreApplication::translate("main", "Search for binary denominator <requirements> (e.g. 111) Should match numerator in size."),
                QCoreApplication::translate("main", "requirements")},
            {"ncapacitors",
                QCoreApplication::translate("main", "Specify the number of capacitors."),
                QCoreApplication::translate("main", "number")},
            {"nresistors",
                QCoreApplication::translate("main", "Specify the number of resistors."),
                QCoreApplication::translate("main", "number")},
            {"active-element",
                QCoreApplication::translate("main", "Use given active element <type> OPAMP | OTRA | CCII | CFOA. Default OPAMP."),
                QCoreApplication::translate("main", "type")},
            {"population-size",
                QCoreApplication::translate("main", "Population size in a generation."),
                QCoreApplication::translate("main", "number")},
            {"ngenerations",
                QCoreApplication::translate("main", "The number of generations"),
                QCoreApplication::translate("main", "number")},
            {"selection-type",
                QCoreApplication::translate("main", "Perform selection with a given <type> ROULETTEWHEEL | TOURNAMENT | RANK. Default RANK."),
                QCoreApplication::translate("main", "type")},
            {"crossover-type",
                QCoreApplication::translate("main", "Perform crossover with a given <type> ONEPOINT | TWOPOINT | UNIFORM | SINGLEGENE | MULTIGENE. Default MULTIGENE."),
                QCoreApplication::translate("main", "type")},
            {"crossover-probability",
                QCoreApplication::translate("main", "Crossover probability."),
                QCoreApplication::translate("main", "probability")},
            {"mutation-probability",
                QCoreApplication::translate("main", "Mutation probability. If unspecified, uses adaptive mutation probability."),
                QCoreApplication::translate("main", "probability")},
            {"outputdir",
                QCoreApplication::translate("main", "Output directory save synthesis files. Default is present working directory."),
                QCoreApplication::translate("main", "directory")},
            });
        parser->process(app);
    } else if (command == "analyze") {
        parser->clearPositionalArguments();
        parser->addPositionalArgument("analyze",
        QCoreApplication::translate("main", "Perform a symbolic circuit analysis on a given netlist"));

        parser->addOptions({
            {{"n", "netlist-file"},
                QCoreApplication::translate("main", "Analyze circuit given in input <netlist-file>."),
                QCoreApplication::translate("main", "netlist-file")},
            {{"l", "library-directory"},
                QCoreApplication::translate("main", "Use library entries in <directory>."),
                QCoreApplication::translate("main", "directory")},
            });
        parser->process(app);
    }
}

int runCommand(QCommandLineParser& parser, QCoreApplication& app)
{
    const QStringList args = parser.positionalArguments();
    if (args.isEmpty()) {
        parser.showHelp(-1);
    }
    if (args[0] == "analyze") {
        const QString netlistFile = parser.value("netlist-file");
        const QString libraryDirectory = parser.value("library-directory");
        if (netlistFile.isEmpty()) {
            parser.showHelp(-1);
        }
        return analyze(netlistFile, libraryDirectory);
    } else if (args[0] == "genetic-synthesize") {
        bool ok = false;
        const QString nreq = parser.value("numerator");
        const QString dreq = parser.value("denominator");
        QString active = parser.value("active-element");
        QString selection = parser.value("selection-type");
        QString xover = parser.value("crossover-type");
        QString outputdir = parser.value("outputdir");

        float xp = parser.value("crossover-probability").toFloat(&ok);
        if (!ok) {
            parser.showHelp(-1);
        }
        float mp = parser.value("mutation-probability").toFloat(&ok);
        bool adaptivem = false;

        if (!ok) {
            adaptivem = true;
        }

        int rc = parser.value("nresistors").toInt(&ok);
        if (!ok || rc <= 0) {
            parser.showHelp(-1);
        }

        int cc = parser.value("ncapacitors").toInt(&ok);
        if (!ok || cc <= 0) {
            parser.showHelp(-1);
        }

        int psize = parser.value("population-size").toInt(&ok);
        if (!ok || psize <= 0) {
            parser.showHelp(-1);
        }

        int generations = parser.value("ngenerations").toInt(&ok);
        if (!ok || generations <= 0) {
            parser.showHelp(-1);
        }

        if (nreq.size() != dreq.size()) {
            parser.showHelp(-1);
        }

        QRegExp rx("[01]+");
        if (!rx.exactMatch(nreq) || !rx.exactMatch(dreq)) {
            parser.showHelp(-1);
        }

        if (active.isEmpty()) {
            active = "OPAMP";
        }

        if (selection.isEmpty()) {
            selection = "RANK";
        }

        if (xover.isEmpty()) {
            xover = "MULTIGENE";
        }

        if (outputdir.isEmpty()) {
            outputdir = ".";
        }

        return genetic_synthesize(nreq, dreq, active, psize,
                generations, selection, xover, xp, mp, cc, rc,
                adaptivem, outputdir);
    } else {
        parser.showHelp(-1);
    }
    return app.exec();
}
