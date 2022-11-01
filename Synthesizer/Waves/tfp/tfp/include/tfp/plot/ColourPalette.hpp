#pragma once

#include "tfp/config.hpp"

#include <QColor>


namespace tfp {

struct ColourPalette
{
    static QColor getColour(int index);
};

} // namespace tfp
