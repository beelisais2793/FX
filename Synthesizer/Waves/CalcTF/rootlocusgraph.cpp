#include "rootlocusgraph.h"

RootLocusGraph::RootLocusGraph(std::shared_ptr<IntervalTF> &tf)
{
    m_locus = std::make_shared<RootLocus>();
    m_locus->setTf(tf);
}

void RootLocusGraph::plotGraph()
{
    m_locus->calculateLocus();
    plotRootLocus();
}

void RootLocusGraph::plotRootLocus()
{
    QCustomPlot *plot = this->plot();

    if (plot == nullptr) return;
    plot->clearGraphs();
    //draw locus
    QVector<double> x,y;
    getXYvectors(m_locus->locus(),x,y);
    QCPGraph *g = plot->addGraph();
    //QCPDataMap *data = getQCPDataMap(m_locus->locus());
    g->setData(x,y);
    g->setLineStyle(QCPGraph::lsNone);
    QCPScatterStyle locusStlye(QCPScatterStyle::ssCircle,Qt::black,2);
    g->setScatterStyle(locusStlye);
    //qDebug()<<"Number of locus poits to be plotted:"<<data->size();

    //Draw zeros   
    g = plot->addGraph();
    //data = getQCPDataMap(m_locus->zeroR());
    getXYvectors(m_locus->zeroR(),x,y);
    g->setData(x,y);
    g->setLineStyle(QCPGraph::lsNone);
    QCPScatterStyle zeroStlye(QCPScatterStyle::ssCrossCircle,Qt::black,2);
    g->setScatterStyle(zeroStlye);

    //Draw poles
    g = plot->addGraph();
    //data = getQCPDataMap(m_locus->poleR());
    getXYvectors(m_locus->poleR(),x,y);
    g->setData(x,y);
    g->setLineStyle(QCPGraph::lsNone);
    QCPScatterStyle poleStlye(QCPScatterStyle::ssCross,Qt::black,2);
    g->setScatterStyle(poleStlye);

    setXAxisLabel("Re");
    setYAxisLabel("Im");

    plot->rescaleAxes();
    plot->replot();    

}

void RootLocusGraph::plotRootLocus1()
{
    QCustomPlot *plot = this->plot();

    if (plot == nullptr) return;
    plot->clearGraphs();
    QCPGraph *g;
    QVector<double> x,y;
    //draw locus

    for (int i = 0; i < m_locus->locus1().count(); i++){
        getXYvectors(m_locus->locus1()[i],x,y);

        //Split to quadrants
        QList<QVector<double>> quads;
        split_locus_by_quadrants(x,y,quads);

        for (int i = 0; i < quads.size(); i+=2)
            {
                //Draw locus

                x = quads[i];
                y = quads[i+1];
                g = plot->addGraph();
                //QCPDataMap *data = getQCPDataMap(m_locus->locus());
                g->setData(x,y);
                g->setLineStyle(QCPGraph::lsLine);
                QCPScatterStyle locusStlye(QCPScatterStyle(QCPScatterStyle::ssDot,Qt::black,2));
                g->setScatterStyle(locusStlye);
                g->setPen(QPen(Qt::black,2));
            }

    }

    //qDebug()<<"Number of locus poits to be plotted:"<<data->size();

    //Draw zeros
    g = plot->addGraph();
    //data = getQCPDataMap(m_locus->zeroR());
    getXYvectors(m_locus->zeroR(),x,y);
    g->setData(x,y);
    g->setLineStyle(QCPGraph::lsNone);
    QCPScatterStyle zeroStlye(QCPScatterStyle::ssCircle,Qt::green,8);
    g->setScatterStyle(zeroStlye);

    //Draw poles
    g = plot->addGraph();
    //data = getQCPDataMap(m_locus->poleR());
    getXYvectors(m_locus->poleR(),x,y);
    g->setData(x,y);
    g->setLineStyle(QCPGraph::lsNone);
    QCPScatterStyle poleStlye(QCPScatterStyle::ssCross,Qt::red,8);
    g->setScatterStyle(poleStlye);

    setXAxisLabel("Re");
    setYAxisLabel("Im");

    plot->rescaleAxes();
    plot->replot();

}

void RootLocusGraph::getXYvectors(const QVector<std::shared_ptr<Root>> &roots, QVector<double> &x, QVector<double> &y){
    int size = roots.size();
    x.resize(size);
    //x.squeeze();
    y.resize(size);
    //y.squeeze();

    for (int i = 0; i < size ; i++){
        Root *r = roots[i].get();
        x[i] = r->real();
        y[i] = r->imaginary();
    }
}

void RootLocusGraph::split_locus_by_quadrants(QVector<double> &x, QVector<double> &y, QList<QVector<double> > &quadrants)
{
    if (x.size() != y.size()) return;
    QList<double> x_lst, y_lst;
    int i = 0;
    bool prev = true,curr = true;

    do{
       x_lst.clear();
       y_lst.clear();

       do
       {
           if ((i + 1) >= x.size()) {
               i++;
               break;
           }
//           if (x[i] != 0 && y[i] != 0)
//           {
//                x_lst.append(x[i]);
//                y_lst.append(y[i]);
//           }

           x_lst.append(x[i]);
           y_lst.append(y[i]);

           if ((x[i+1]- x[i]) > 0)
           {
               if (i == 0) prev = true;
               else prev = curr;
               curr = true;
           }
           else
           {
               if (i == 0) prev = false;
               else prev = curr;
               curr = false;
           }

           if (prev == curr) i++;
       }
       while ( prev == curr );

       prev = curr; //should be equal before next loop

       quadrants.push_back(x_lst.toVector());
       quadrants.push_back(y_lst.toVector());
    }
    while (i < x.size());
}
