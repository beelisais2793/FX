#ifndef ROOTLOCUS_H
#define ROOTLOCUS_H

#include "intervaltf.h"
#include "transferfunction.h"

#include <memory>
class RootLocus
{
public:
    RootLocus();    

    QVector<std::shared_ptr<Root>> poleR() const;

    QVector<std::shared_ptr<Root>> zeroR() const;

    QVector<std::shared_ptr<Root>> locus() const;

    double K() const;    

    QList<QVector<std::shared_ptr<Root>> > locus1() const;
    void setLocus1(const QList<QVector<std::shared_ptr<Root>> > &locus1);

    std::shared_ptr<IntervalTF> tf() const;
    void setTf(const std::shared_ptr<IntervalTF> &tf);
    void calculateLocus();

private:
    std::shared_ptr<IntervalTF> m_tf;
    QVector<std::shared_ptr<Root>> m_poleR;
    QVector<std::shared_ptr<Root>> m_zeroR;
    QVector<std::shared_ptr<Root>> m_locus;
    QList<QVector<std::shared_ptr<Root>>> m_locus1;



    double m_K;
    double calculateDelta(QVector<std::shared_ptr<Root>> &rootList1, QVector<std::shared_ptr<Root>> &rootList2);
};

#endif // ROOTLOCUS_H
