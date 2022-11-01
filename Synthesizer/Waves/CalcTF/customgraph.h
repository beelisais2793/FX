#ifndef CUSTOMGRAPH_H
#define CUSTOMGRAPH_H

#include "qcustomplot.h"

#include <QString>



class CustomGraph
{

public:
    CustomGraph();

    QString XAxisLabel() const;
    void setXAxisLabel(const QString &XAxisLabel);

    QString YAxisLabel() const;
    void setYAxisLabel(const QString &YAxisLabel);

    void setPlot(QCustomPlot *plot);

    QCustomPlot *plot() const;

    virtual void plotGraph();

private:
    QString m_XAxisLabel;
    QString m_YAxisLabel;
    QCustomPlot *m_plot;
};

#endif // CUSTOMGRAPH_H
