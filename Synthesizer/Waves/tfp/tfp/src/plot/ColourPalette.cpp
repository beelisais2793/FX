#include "tfp/plot/ColourPalette.hpp"

#include <QVector>


namespace tfp {

static QVector<QColor> generateColourPalette()
{
    QVector<QColor> colours;

    colours.push_back(QColor(0, 0, 255));
    colours.push_back(QColor(255, 0, 0));
    colours.push_back(QColor(40, 128, 0));
    colours.push_back(QColor(255, 128, 0));
    colours.push_back(QColor(0, 255, 255));
    colours.push_back(QColor(0, 128, 255));
    colours.push_back(QColor(255, 0, 255));

    return colours;
}

// ----------------------------------------------------------------------------
QColor ColourPalette::getColour(int index)
{
    static QVector<QColor> colours = generateColourPalette();

    while(index >= colours.size())
        index -= colours.size();
    return colours.at(index);
}

} // namespace tfp
