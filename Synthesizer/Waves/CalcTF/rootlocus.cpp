#include "rootlocus.h"
#include <QDebug>

RootLocus::RootLocus()
{

}

QVector<std::shared_ptr<Root>> RootLocus::poleR() const
{
    return m_poleR;
}

QVector<std::shared_ptr<Root>> RootLocus::zeroR() const
{
    return m_zeroR;
}

QVector<std::shared_ptr<Root>> RootLocus::locus() const
{
    return m_locus;
}

double RootLocus::K() const
{
    return m_K;
}

QList<QVector<std::shared_ptr<Root>> > RootLocus::locus1() const
{
    return m_locus1;
}

void RootLocus::setLocus1(const QList<QVector<std::shared_ptr<Root>>> &locus1)
{
    m_locus1 = locus1;
}

std::shared_ptr<IntervalTF> RootLocus::tf() const
{
    return m_tf;
}

void RootLocus::setTf(const std::shared_ptr<IntervalTF> &tf)
{
    m_tf = tf;
    //calculateLocus();
}

double RootLocus::calculateDelta(QVector<std::shared_ptr<Root>> &rootList1 , QVector<std::shared_ptr<Root>> &rootList2)
{
    if (rootList1.count() != rootList2.count()) return 0;

    double delta = 0 ;
    double delta_r = 0;
    for (int i = 0; i < rootList1.count(); i++)
    {
        std::shared_ptr<Root> r1 = rootList1[i];
        std::shared_ptr<Root> r2 = rootList2[i];

        delta_r = std::abs(r1->magnitude() - r2->magnitude());
        if (delta < delta_r)
            delta = delta_r ;
    }

    return delta;

}



void RootLocus::calculateLocus()
{
    m_locus.clear();

    m_tf->generateTF();

    //QList<QVector<std::shared_ptr<Root>>> rlocus;
    //m_locus = m_tf->getRootLocus();


    m_poleR = m_tf->getRootsClosedLoop(false);
    m_zeroR = m_tf->getRootsClosedLoop(true);
}



