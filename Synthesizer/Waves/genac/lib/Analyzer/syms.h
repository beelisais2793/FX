#ifndef SYMS_H
#define SYMS_H

#include <QtCore/QHash>
#include <QtCore/QString>
#include <ginac/ginac.h>

class Syms
{
public:

    Syms()
    {
        GiNaC::realsymbol symbl("_inf");
        m_syms.insert("_inf",symbl);
        GiNaC::realsymbol s("s");
        m_syms.insert("s",s);
    }

    void getSymbol(GiNaC::realsymbol& symbl, const QString& name)
    {
        if(m_syms.contains(name))
        {
            symbl = m_syms[name];
            return;
        }

        symbl.set_name(name.toStdString());
        m_syms.insert(name,symbl);
    }

    const GiNaC::realsymbol& getInf() const
    {
        return m_syms.find("_inf").value();
    }

    const GiNaC::realsymbol& getS() const
    {
        return m_syms.find("s").value();
    }


    void addSymbol(const GiNaC::realsymbol& sym)
    {
        m_syms.insert(QString::fromStdString(sym.get_name()),sym);
    }

private:
    QHash<QString,GiNaC::realsymbol> m_syms;
};

#endif // SYMS_H
