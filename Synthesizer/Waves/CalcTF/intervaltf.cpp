#include "intervaltf.h"

#include <QStringList>
#include <QDebug>
#include <random>
#include <QtConcurrent>

IntervalTF::IntervalTF()
{

}

void IntervalTF::setN(const QString value)
{
    purseString(value, m_numerator);
    //generateTF(m_numerator,m_denomenator,m_tfList);
    //generateTF1(m_numerator,m_denomenator,m_tfList);
}

void IntervalTF::setD(const QString value)
{
    purseString(value, m_denomenator);
    //generateTF(m_numerator,m_denomenator,m_tfList);
    //generateTF1(m_numerator,m_denomenator,m_tfList);
}

QString IntervalTF::getN()
{
    return getStr(m_numerator);
}

QString IntervalTF::getD()
{
    return getStr(m_denomenator);
}

QString IntervalTF::getTfEquation(const QString &plane)
{
    QString nstr = getCoeffEq(m_numerator,plane);
    QString dstr = getCoeffEq(m_denomenator,plane);

    QString tfeq = QString("%0 / %1").arg(nstr).arg(dstr);

    return tfeq;
}

QVector<std::shared_ptr<Root>> IntervalTF::getRootsClosedLoop(const double K)
{
    QVector<std::shared_ptr<Root>> rlist;
    //generateTF(m_numerator,m_denomenator,m_tfList);
    //generateTF1(m_numerator,m_denomenator,m_tfList);

    for(int i = 0 ; i < m_tfList.count(); i++){
        auto r = m_tfList[i].getRootsClosedLoop(K);
        rlist.append(r);
    }
    return rlist;
}


QVector<std::shared_ptr<Root>> IntervalTF::getRootsClosedLoop(const bool max_K)
{
    QVector<std::shared_ptr<Root>> rlist;
    //generateTF(m_numerator,m_denomenator,m_tfList);
   // generateTF1(m_numerator,m_denomenator,m_tfList);

//    for(int i = 0 ; i < m_tfList.count(); i++){
//        auto r = m_tfList[i].getRootsClosedLoop(max_K);
//        rlist.append(r);
//    }
    for(int i = 0 ; i < m_tfList.count(); i++){
        m_tfList[i].setMaxK(max_K);
    }
    int max_size = m_tfList.count()*(m_tfList[0].polesPoly()->Degree());
    qDebug()<<"Max size: "<<max_size <<""<< rlist.capacity();
    rlist.reserve(max_size);

    int mapReduceTime = 0;
    QTime time;
    time.start();
    auto r = QtConcurrent::mappedReduced(m_tfList,mapFunction,reduceFunction);
    rlist = r.result();
    mapReduceTime = time.elapsed();
    qDebug() <<"Max K: "<<max_K<<" "<< "MapReduce" << mapReduceTime <<"size of result: "<<rlist.count();
    return rlist;
}



QVector<std::shared_ptr<Root>> IntervalTF::getRootLocus()
{
    QVector<std::shared_ptr<Root>> rlist;

    //generateTF(m_numerator,m_denomenator,m_tfList);
    //generateTF1(m_numerator,m_denomenator,m_tfList);
//    for(int i = 0 ; i < m_tfList.count(); i++){
//        auto r = m_tfList[i].getRootLocus();
//        rlist.append(r);
//    }
    int mapReduceTime = 0;
    QTime time;
    time.start();
    auto r = QtConcurrent::mappedReduced(m_tfList,mapLocusFunction,reduceLocusFunction);
    rlist = r.result();
    mapReduceTime = time.elapsed();
    qDebug() << "MapReduce" << mapReduceTime;
    return rlist;
}


QString IntervalTF::getCoeffEq(const QList<TfCoeff> &p, const QString &plane)
{
    QString polyStr;

    if (p.count() == 1) return QString("%0..%1").arg(p[0].lowerV()).arg(p[0].upperV());
    for(int i = p.count() - 1 ; i >= 0 ; i--){
       TfCoeff c = p[i];
       QString term;

       if (c.upperV() == 0.0 && c.lowerV() == 0.0) {
           continue;
       }else{
           QString range = "";
           if (c.lowerV() == c.upperV()) range = QString("(%0)").arg(c.lowerV());
           else range = QString("(%0..%1)").arg(c.lowerV()).arg(c.upperV());

           if (i > 1){
                term = QString("%0%1^%2").arg(range).arg(plane).arg(i);
           }
           if (i == 1){
               term = QString("%0%1").arg(range).arg(plane);
           }
           if (i == 0){
               term = QString("%0").arg(range);
           }
        }

       if (i != p.count() - 1){
           term = QString(" + %0").arg(term);
       }

       polyStr += term;
    }   

    return polyStr;
}

QString IntervalTF::getStr(const QList<TfCoeff> &c)
{
    QString res = "";
    for (int i = 0; i < c.count(); i ++){
        if (c[i].upperV() == c[i].lowerV())
        {
            res += QString("%0 ").arg(c[i].upperV());
        }else{
            res +=QString("%0..%1 ").arg(c[i].lowerV()).arg(c[i].upperV());
        }
    }

    return res;
}

void IntervalTF::purseString(const QString value, QList<TfCoeff> &coeffList)
{
    coeffList.clear();
    if (value.isEmpty()) return;
    QString res = value.trimmed();
    QStringList tokens = res.split(' ');    

    int i = 0;    
    //qDebug()<<"-=-----tokens------=-";
    while (i < tokens.count()){
        QString token = tokens[i++];
        //qDebug()<<token;
        QStringList numList = token.split("..");
        if (numList.count() == 1){
            QString num = numList[0].trimmed();
            TfCoeff coeff;
            coeff.setLowerV(num.toDouble());
            coeff.setUpperV(num.toDouble());
            coeffList.append(coeff);
        }
        if (numList.count() == 2){
            QString low = numList[0].trimmed();
            QString high = numList[1].trimmed();
            TfCoeff coeff;
            coeff.setLowerV(low.toDouble());
            coeff.setUpperV(high.toDouble());
            coeffList.append(coeff);
        }
    }
}

void IntervalTF::kharitonovP(const QList<TfCoeff> &c,
                                                std::vector<double> &v, const int n)
{
   std::vector<int> p1;
   std::vector<int> p2;
   std::vector<int> p3;
   std::vector<int> p4;
   int k;
   p1.push_back(0);
   p1.push_back(0);
   p1.push_back(1);
   p1.push_back(1);

   p2.push_back(0);
   p2.push_back(1);
   p2.push_back(1);
   p2.push_back(0);

   p3.push_back(1);
   p3.push_back(0);
   p3.push_back(0);
   p3.push_back(1);

   p4.push_back(1);
   p4.push_back(1);
   p4.push_back(0);
   p4.push_back(0);

   QList<std::vector<int>> p;
   p.push_back(p1);
   p.push_back(p2);
   p.push_back(p3);
   p.push_back(p4);

   v.assign(c.size(),0);

   for (unsigned int i = 0 ; i < v.size(); i++)
   {
       if (i > 3) k = i % 4;
       else k = i;
       if (p[n][k] == 1)
       {
           v[i] = c[i].upperV();
       }
       else
       {
           v[i] = c[i].lowerV();
       }
   }

}

void IntervalTF::generateTF()
{
    generateTF1(m_numerator,m_denomenator,m_tfList);
}


void IntervalTF::generateTF(const QList<TfCoeff> &N,const QList<TfCoeff> &D,
                            QVector<TransferFunction> &tfList)
{
    tfList.clear();
    if (N.count() == 0 || D.count() == 0) return;
    for (int i = 0; i < 4 ; i++){
        std::vector<double> vN;
        std::vector<double> vD;
        kharitonovP(N,vN,i);
        kharitonovP(D,vD,i);

        Polynomial pN = Polynomial(&vN[0],vN.size());
        Polynomial pD = Polynomial(&vD[0],vD.size());

        TransferFunction tf(pN,pD);
        tfList.append(tf);
    }
}

void IntervalTF::generateTF1(const QList<TfCoeff> &N, const QList<TfCoeff> &D, QVector<TransferFunction> &tfList)
{
    int max_tf = 1000000;
    tfList.resize(max_tf);

    if (N.count() == 0 || D.count() == 0) return;
//    for (int i = 0; i < max_tf ; i++){
//        TransferFunction tf;
//        tfList.append(tf);
//    }
    QTime time;
    time.start();
    QtConcurrent::map(tfList,mapTF(N,D));
    int fillVectTime = time.elapsed();
    qDebug() << "Fill vector with random values :" << fillVectTime;

    //qDebug()<<"Generated transfer functions";
}






