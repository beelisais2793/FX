#include "customgraph.h"

CustomGraph::CustomGraph()
{

}

QString CustomGraph::XAxisLabel() const
{
    return m_XAxisLabel;
}

void CustomGraph::setXAxisLabel(const QString &XAxisLabel)
{
    m_XAxisLabel = XAxisLabel;
    if (m_plot != nullptr){
        m_plot->xAxis->setLabel(m_XAxisLabel);
    }
}

QString CustomGraph::YAxisLabel() const
{
    return m_YAxisLabel;    
}

void CustomGraph::setYAxisLabel(const QString &YAxisLabel)
{
    m_YAxisLabel = YAxisLabel;
    if (m_plot !=nullptr){
        m_plot->yAxis->setLabel(m_YAxisLabel);
    }
}

void CustomGraph::setPlot(QCustomPlot *plot)
{
    m_plot = plot;
    if (m_plot != nullptr){
        m_plot->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom|QCP::iSelectPlottables);
    }
}

QCustomPlot *CustomGraph::plot() const
{
    return m_plot;
}

void CustomGraph::plotGraph()
{

}
