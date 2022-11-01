#ifndef SYNTHESIZER
#define SYNTHESIZER

#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QSet>
#include <QtCore/QString>

class Estimator;
class Population;
class Chromosome;
class GeneticSynthesizer
{
public:
    enum SelectionType {
        RouletteWheel,
        Rank,
        Tournament
    };

    enum CrossoverType {
        OnePoint,
        TwoPoint,
        Uniform,
        SingleGene,
        MultiGene
    };

    GeneticSynthesizer();
    ~GeneticSynthesizer();
    void run(const QString& nreq, const QString& dreq,
             const QString& active, int psize, int generations,
             const QString& selection, const QString& xover,
             float xp, float mp, int cc, int rc, bool adaptivem,
             const QString& outputdir);
private:
    void select(Chromosome **parent1, Chromosome **parent2);
    void crossover(const Chromosome *parent1, const Chromosome *parent2,
                         Chromosome **child1, Chromosome **child2);
    void mutate(Chromosome *c);

    void save(const Chromosome *c, int gen);
    void record(int max_generations, int n);

    Estimator *m_estimator;
    SelectionType m_selectionType;
    CrossoverType m_crossoverType;
    Population *m_population;
    QFile m_recordFile;
    QFile m_saveFile;
    QTextStream m_recordStream;
    QTextStream m_saveStream;
    QSet<QString> m_bests;
};
#endif /* SYNTHESIZER */
