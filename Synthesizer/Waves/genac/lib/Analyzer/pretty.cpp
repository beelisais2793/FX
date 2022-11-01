#include "pretty.h"
#include <sstream>

static QString pretty(const GiNaC::ex & num, const GiNaC::ex & den, bool complex, bool islhs)
{
    std::stringstream stream_num;
    std::stringstream stream_den;
    stream_num << num;
    if(den.is_equal(1)){
        stream_num << "\n";
        return "*  "+ QString::fromStdString(stream_num.str());
    }
    stream_den << den;
    QString num_str = QString::fromStdString(stream_num.str());
    QString den_str = QString::fromStdString(stream_den.str());
    int max = 0;
    int min = 0;
    if(num_str.size() > den_str.size()){
        max = num_str.size();
        min = den_str.size();
    }
    else{
        max = den_str.size();
        min = num_str.size();
    }
    QString str(max+1,QChar('-'));
    if(islhs){
        str.append(" =");
    }
    str.append("\n");
    num_str.append("\n");
    den_str.append("\n");

    QString pre((max-min)/2, ' ');
    if(num_str.size() < den_str.size()){
        num_str.prepend(pre);
    }
    else{
        den_str.prepend(pre);
    }

    if(complex){
        den_str.replace(QChar('I'),QChar('j'));
        num_str.replace(QChar('I'),QChar('j'));
        //den.replace(QChar('w'),QChar(0x03c9)); // Does not work!
        //num.replace(QChar('w'),QChar(0x03c9)); //replace with small omega
    }

    return "*  " + num_str + "*  " + str + "*  " + den_str;
}

QString pretty(const Equation & eq, bool complex)
{
    return pretty(eq.lhsNum, eq.lhsDen, complex, true) +
           pretty(eq.rhsNum, eq.rhsDen, complex, false);
}

