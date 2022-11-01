#ifndef PRETTY_H
#define PRETTY_H

#include "analyzer.h"
#include <QtCore/QString>

QString pretty(const GiNaC::ex& expr,bool complex = false,bool islhs = false);

QString pretty(const Equation& eq, bool complex = false);
#endif // PRETTY_H
