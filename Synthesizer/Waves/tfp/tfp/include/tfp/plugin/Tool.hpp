#pragma once

#include "tfp/config.hpp"
#include "tfp/models/System.hpp"
#include "tfp/util/Reference.hpp"
#include <QWidget>

namespace tfp {

class System;

/*!
 * @brief Represents a continuous dynamic system.
 *
 * The numerator and denominator objects can be publicly accessed (numerator_
 * and denominator_). When doing so, you need to manually emit either
 * parametersChanged() (if you only modified existing values) or
 * structureChanged() (if you added or removed roots).
 */
class TFP_PUBLIC_API Tool : public QWidget,
                            public SystemListener
{
public:
    explicit Tool(QWidget* parent=NULL);
    ~Tool();

    virtual void setSystem(System* system);

protected:
    virtual void onSetSystem() = 0;
    virtual void replot() = 0;
    virtual void autoScale() = 0;

protected:
    Reference<System> system_;
};

}
