#ifndef POPULATION_H
#define POPULATION_H

#include <QtCore/QList>
#include <chromosome.h>

class QString;

class Population : public QList<Chromosome*>
{
public:
    Population() =default;
    void initialize(int size, Chromosome::Type type,
               int ncapacitors, int nresistors, bool random);
    virtual ~Population();

    void setMaxQuality(float q)
    {m_maxQuality = q;}
    float maxQuality() const
    {return m_maxQuality;}

    void setAverageQuality(float q)
    {m_avrgQuality = q;}
    float averageQuality() const
    {return m_avrgQuality;}


    void setMaxFitness(float f)
    {m_maxFitness = f;}
    float maxFitness() const
    {return m_maxFitness;}

    void setAverageFitness(float f)
    {m_avrgFitness = f;}
    float averageFitness() const
    {return m_avrgFitness;}

    void setDiversity(float d)
    {m_diversity = d;}
    float diversity() const
    {return m_diversity;}
private:
    float m_maxQuality;
    float m_avrgQuality;
    float m_maxFitness;
    float m_avrgFitness;
    float m_diversity;
};
#endif /* POPULATION_H */
