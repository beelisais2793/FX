
#define _USE_MATH_DEFINES

#include "transferfunction.h"
#include "Polynomial.h"
#include "errorstrings.h"
#include "root.h"

#include <QList>
#include <QStringBuilder>
#include <QVector>
#include <QDebug>
#include <complex>
#include <math.h>

TransferFunction::TransferFunction()
{    
    initTS();
}

TransferFunction::TransferFunction(double scalar)
{
    initTS();
    setZerosPoly(QString("%0").arg(scalar));
    setPolesPoly(QString("%0").arg(scalar));
}

TransferFunction::TransferFunction(TransferFunction *tf0 )
{
    initTS();
    setTF(tf0->getZeroVectorStr(),tf0->getPolesVectorStr());
}

TransferFunction::TransferFunction(const Polynomial &zeroP, const Polynomial &poleP)
{
    QString zeroStr = getPolynomialVectorStr(zeroP);
    QString poleStr = getPolynomialVectorStr(poleP);

    initTS();
    setTF(zeroStr,poleStr);
}

TransferFunction::TransferFunction(const QString &zeroStr, const QString &poleStr)
{
    initTS();
    setTF(zeroStr,poleStr);
}

TransferFunction::TransferFunction(const std::string &zeroStr, const std::string &poleStr)
{
    setZerosPoly(QString(zeroStr.c_str()));
    setPolesPoly(QString(poleStr.c_str()));
}

void TransferFunction::initTS()
{
    m_zerosPoly = std::make_shared<Polynomial>();
    m_polesPoly = std::make_shared<Polynomial>();
}

void TransferFunction::setTF(const Polynomial &zeroP, const Polynomial &poleP)
{
    QString zeroStr = getPolynomialVectorStr(zeroP);
    QString poleStr = getPolynomialVectorStr(poleP);

    setTF(zeroStr,poleStr);
}

void TransferFunction::setTF(const QString &zeroStr, const QString &poleStr)
{
    setZerosPoly(zeroStr);
    setPolesPoly(poleStr);
}

TransferFunction operator *(const TransferFunction &tr0, const TransferFunction &tr1)
{

    auto tr0_p = tr0.polesPoly();
    auto tr0_z = tr0.zerosPoly();

    auto tr1_p = tr1.polesPoly();
    auto tr1_z = tr1.zerosPoly();

    Polynomial res_z = (*tr0_z) * (*tr1_z) ;
    Polynomial res_p = (*tr0_p) * (*tr1_p) ;

    TransferFunction res(res_z,res_p);

    return res;

}

TransferFunction operator /(const TransferFunction &tr0, const TransferFunction &tr1)
{
    auto tr0_p = tr0.polesPoly();
    auto tr0_z = tr0.zerosPoly();

    auto tr1_p = tr1.polesPoly();
    auto tr1_z = tr1.zerosPoly();

    Polynomial res_z = (*tr0_z) * (*tr1_p) ;
    Polynomial res_p = (*tr0_p) * (*tr1_z) ;

    TransferFunction res(res_z,res_p);

    return res;

}

TransferFunction operator -(const TransferFunction &tr0, const TransferFunction &tr1)
{
    auto tr0_p = tr0.polesPoly();
    auto tr0_z = tr0.zerosPoly();

    auto tr1_p = tr1.polesPoly();
    auto tr1_z = tr1.zerosPoly();

    Polynomial res_z = ((*tr0_z) * (*tr1_p)) - ((*tr1_z)*(*tr0_p));
    Polynomial res_p = (*tr0_p) * (*tr1_p) ;

    TransferFunction res(res_z,res_p);

    return res;
}

TransferFunction operator +(const TransferFunction &tr0, const TransferFunction &tr1)
{
    auto tr0_p = tr0.polesPoly();
    auto tr0_z = tr0.zerosPoly();

    auto tr1_p = tr1.polesPoly();
    auto tr1_z = tr1.zerosPoly();

    Polynomial res_z = ((*tr0_z) * (*tr1_p)) + ((*tr1_z)*(*tr0_p));
    Polynomial res_p = (*tr0_p) * (*tr1_p) ;

    TransferFunction res(res_z,res_p);

    return res;
}


std::shared_ptr<Polynomial> TransferFunction::zerosPoly() const
{
    return m_zerosPoly;
}

void TransferFunction::setZerosPoly(std::shared_ptr<Polynomial> &zerosPoly)
{
    m_zerosPoly = zerosPoly;
    //simplifyTF();
}

QString TransferFunction::setZerosPoly(const QString &polyStr,QString *errString)
{
    auto p = m_zerosPoly;    
    //dumpValue(QString("In function setZeroPoly BEFORE edit"),p);
    QString res = setPolynomialFomStr(polyStr,p,errString);   
    //qDebug()<<" =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=";
    //dumpValue(QString("In function setZeroPoly AFTER edit"),p);
    m_zerosPoly = p ;
    //simplify polynomial
    //simplifyTF();
    return res;
}

std::shared_ptr<Polynomial> TransferFunction::polesPoly() const
{
    return m_polesPoly;
}

void TransferFunction::setPolesPoly(std::shared_ptr<Polynomial> &polesPoly)
{
    m_polesPoly = polesPoly;
    //simplifyTF();
}

QString TransferFunction::setPolesPoly(const QString &polyStr,QString *errString)
{
    auto p = m_polesPoly;
    //dumpValue(QString("In function setPolesPoly BEFORE edit"),p);
    QString res = setPolynomialFomStr(polyStr,p,errString);
    //qDebug()<<" =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= \n";
    //dumpValue(QString("In function setPolesPoly AFTER edit"),p);
    m_polesPoly = p;
    //simplifyTF();
    return res;
}

QString TransferFunction::getPolesVectorStr()
{
    auto p = m_polesPoly;
    QString pStr;
    pStr = getPolynomialVectorStr(p);
    return pStr;
}

QString TransferFunction::getZeroVectorStr()
{
    auto p = m_zerosPoly;
    QString pStr;
    pStr = getPolynomialVectorStr(p);
    return pStr;
}

QString TransferFunction::setPolynomialFomStr(const QString &polyStr, std::shared_ptr<Polynomial> &p, QString *errString)
{
    QStringList coeff_str_list = polyStr.split(' ');
    QString coeff_str;
    QList<double> coeff_double_list;
    double coeff;
    bool ok,pass = true;
    foreach (coeff_str , coeff_str_list) {
        coeff = coeff_str.toDouble(&ok);
        if (ok){
            coeff_double_list.append(coeff);
        }
        else{
            if (errString != nullptr && coeff_str != ""){
                (*errString) += QString(ErrorStrings::NOT_A_NUMBER).arg(coeff_str) + " \n";
            }
            pass = false;
        }
    }
    if (!pass && errString != nullptr) {
        return (*errString);
    }

    //qDebug() << "coefficients qlist " <<coeff_double_list;

    std::vector<double> coeff_double_vector = coeff_double_list.toVector().toStdVector();

    //qDebug() << "coefficients std vector size" << coeff_double_vector.size();

    //dumpValue(QString("In function setPolyFromStr Before assignment of p"),p);

    p = std::make_shared<Polynomial>();

    p->SetCoefficients(coeff_double_vector.data(),coeff_double_vector.size() - 1 );

    //dumpValue(QString("In function setPolyFromStr AFTER assignment of p"),p);

    return getPolynomialVectorStr(p);
}

QString TransferFunction::getPolynomialEquation(const std::shared_ptr<Polynomial> &p, const QString &plane)
{
    QString polyStr = "(";
    QStringList terms;

    if (p->Degree() == 0) return QString("%0").arg((*p)[0]);
    for(int i = p->Degree() ; i >= 0 ; i--){
       double c = (*p)[i];
       QString term;

       if (c == 0) {
           continue;
       }else{
        //c not equal to zero
           if (c == 1 ){
               //c equals 1 and i > 1  s^2, s^3, s^4
               if (i > 1){
                    term = QString("%0^%1").arg(plane).arg(i);
               }
               if (i == 1){
                   term = QString("%0").arg(plane);
               }
               if (i == 0){
                   term = QString("%0").arg("1");
               }
           }else{
               if (i > 1){
                    term = QString("%0%1^%2").arg(c).arg(plane).arg(i);
               }
               if (i == 1){
                   term = QString("%0%1").arg(c).arg(plane);
               }
               if (i == 0){
                   term = QString("%0").arg(c);
               }
           }
       }

       if (c > 0 && i != p->Degree()){
           term = QString("+%0").arg(term);
       }

       polyStr += term;
    }

    polyStr += ") ";

    return polyStr;
}

QString TransferFunction::getPolynomialVectorStr(const std::shared_ptr<Polynomial> &p)
{
    return getPolynomialVectorStr(*p);
}

QString TransferFunction::getPolynomialVectorStr(const Polynomial &p)
{
    //return string with coeficients in ascending order
    QString polystr="";
    if (p.Degree() == -1) return "";
    if (p.Degree() == 0) return QString("%0").arg(p[0]);
    for (int i = 0 ; i <= p.Degree(); i++){
        double c = p[i];
        QString term = QString("%0").arg(c);

        polystr += term + " ";
    }
    return polystr;
}



double TransferFunction::getRootAt(QVector<double> &vect, int i)
{
    if (i < 0 || i >= vect.size()){
        return 0;
    }else
        return vect[i];
}

QString TransferFunction::getTfEquation(const QString &plane)
{
    //Return string in this form
    //"`W(s) = (a1 * s ^3 + a2 * s ^ 2 + a3 * s + a4)/(b1 * s ^3 + b2 * s ^ 2 + b3 * s + b4)`"

    QString zeroStr = getPolynomialEquation(m_zerosPoly,plane);
    QString poleStr = getPolynomialEquation(m_polesPoly,plane);

    //if (zeroStr == "") return
    QString tf_eq = QString("W(%0) = %1 / %2").arg(plane)
                                              .arg(zeroStr)
                                              .arg(poleStr);
    return tf_eq;
}

QVector<std::shared_ptr<Root>> TransferFunction::getRootsClosedLoop(const double K)
{
    Polynomial *D = m_polesPoly.get();
    Polynomial *N = m_zerosPoly.get();
    QVector<std::shared_ptr<Root>> rList;

    Polynomial R = (*D) + K*(*N);

    rList = getRoots(R);

    return rList;
}

QVector<std::shared_ptr<Root>> TransferFunction::getRootsClosedLoop(const bool max_K) const
{
    Polynomial R;
    Polynomial *D = m_polesPoly.get();
    Polynomial *N = m_zerosPoly.get();
    QVector<std::shared_ptr<Root>> rList;

    if (max_K) R = (*N);
    else R = (*D);

    rList = getRoots(R);

    return rList;
}

QVector<std::shared_ptr<Root>> TransferFunction::getRootsClosedLoop1() const
{
    return getRootsClosedLoop(getMaxK());
}

QVector<std::shared_ptr<Root>> TransferFunction::getRootsClosedLoop(Polynomial &N,Polynomial &D,
                                                                   const double K) const
{
    QVector<std::shared_ptr<Root>> rList;

    Polynomial R = D + K * N;

    rList = getRoots(R);

    return rList;
}

QVector<std::shared_ptr<Root>> TransferFunction::getRootLocus() const
{
    Polynomial N = *m_zerosPoly;
    Polynomial D = *m_polesPoly;
    double K_max;
    QList<QVector<std::shared_ptr<Root>>> locus;
    QVector<std::shared_ptr<Root>>result;

    result = getRootLocus(N,D,K_max,locus);

    return result;
}

QVector<std::shared_ptr<Root>> TransferFunction::getRootLocus(Polynomial &N,Polynomial &D,double & K_max,
                                                            QList<QVector<std::shared_ptr<Root>>> &locus) const
{
    auto poleRoots = getRootsClosedLoop(N,D,0);


    //poleRoots.append(intersectRoots);

    int Max_Points = 400;
    double cut_off = 5;
    double step = 0.1;
    double k=0;

    for (int i = 0; i < poleRoots.count(); i++){
        QVector<std::shared_ptr<Root>> segment;
        segment.append(poleRoots[i]);
        locus.append(segment);
    }
    int count = 0;
    for (;;){
        if (count > 350) step = 10;
        k = step* count++;
        auto roots = getRootsClosedLoop(N,D,k);
        QVector<std::shared_ptr<Root>> rlist;
        rlist.clear();
        for(int i = 0; i < locus.count(); i++){
            auto seg = locus[i];
            rlist.append(seg[seg.count()-1]);
        }

        for(int i = 0; i < roots.count(); i++){
            auto root = roots[i]->complexRoot();
            if (std::abs(root.real()) > cut_off) continue;
            int idx = getClosestRoot(rlist, root);
            if (idx > -1 && idx < rlist.count()){
                locus[idx].append(roots[i]);
            }
        }

        if (count >  Max_Points) break;
    }

    QVector<std::shared_ptr<Root>> result;
    for(int i = 0; i < locus.count(); i++){
        for (int j = 0; j < locus[i].count(); j++){
            result.append(locus[i][j]);
        }
    }
    return result;
}

int TransferFunction::getClosestRoot(QVector<std::shared_ptr<Root>> &roots, std::complex<double> &root) const
{
    int index = -1;
    double diff = 0, diff_prev = 0;
    for(int i = 0; i < roots.count(); i++){
        diff_prev = diff;
        auto r1 = roots[i]->complexRoot();
        auto r2 = root;
        diff = norm(r2 - r1);
        //qDebug()<<"diff : "<< diff;
        if (i == 0 ) index = i;
        if (i > 0 && diff < diff_prev) index = i;
    }

    return index;
}

std::complex<double> TransferFunction::evaluateComplex(const Polynomial &P, std::complex<double> p)
{
    double pr,pi;
    P.EvaluateComplex(p.real(),p.imag(),pr,pi);

    return std::complex<double>(pr,pi);
}

unsigned int TransferFunction::factorial(unsigned int n)
{
    unsigned int ret = 1;
    for (unsigned int i = 1; i <=n; i++)
        ret *=i;
    return ret;
}

QVector<std::shared_ptr<Root>> TransferFunction::getRoots(Polynomial &P) const{
    QVector<std::shared_ptr<Root>> rList;

    QVector<double> real_vect;
    QVector<double>imag_vect;

    real_vect.resize(P.Degree());
    imag_vect.resize(P.Degree());
    rList.resize(P.Degree());

    int num_of_roots = 0;

    if (P.Degree() == 0) return rList;

    P.FindRoots(&real_vect[0],&imag_vect[0],&num_of_roots);

    for(int i = 0 ; i < num_of_roots; i++){
        auto r = std::make_shared<Root>(real_vect[i],imag_vect[i]);
        //if (std::abs(r->real()) > 3 ) continue;
        rList[i] = r;
    }

    return rList;
}

bool TransferFunction::isEmpty()
{
    if (m_polesPoly->Degree() == -1 || m_zerosPoly->Degree() == -1){
        return true;
    }
    else return false;
}
