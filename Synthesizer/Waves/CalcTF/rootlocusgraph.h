#ifndef ROOTLOCUSGRAPH_H
#define ROOTLOCUSGRAPH_H

#include "customgraph.h"
#include "intervaltf.h"
#include "rootlocus.h"
#include <memory>
#include "qcustomplot.h"



class RootLocusGraph : public CustomGraph
{
public:
    RootLocusGraph(std::shared_ptr<IntervalTF> &tf);
    void plotGraph() override;
private:
    void plotRootLocus();
    std::shared_ptr<RootLocus> m_locus;
    void getXYvectors(const QVector<std::shared_ptr<Root>> &roots, QVector<double> &x, QVector<double> &y);
    void plotRootLocus1();
    void split_locus_by_quadrants(QVector<double> &x, QVector<double> &y, QList<QVector<double> > &quadrants);
};

#endif // ROOTLOCUSGRAPH_H
