#include "tfp/util/Util.hpp"

#include <QLayout>
#include <QSpacerItem>
#include <QWidget>
#include <QStandardPaths>

#include <qwt_plot_textlabel.h>


namespace tfp {

// ----------------------------------------------------------------------------
void Util::clearLayout(QLayout* layout)
{
    QLayoutItem* child;
    while((child = layout->takeAt(0)) != NULL)
    {
        if(child->layout() != NULL)
            child->layout()->deleteLater();
        if(child->widget() != NULL)
            child->widget()->deleteLater();
    }
}

// ----------------------------------------------------------------------------
QwtPlotTextLabel* Util::createLoadingTextLabel()
{
    QFont font;
    font.setPointSize(48);
    font.setBold(true);

    QwtText text;
    text.setText("Loading...");
    text.setFont(font);
    text.setColor(QColor(160, 160, 160));

    QwtPlotTextLabel* label = new QwtPlotTextLabel;
    label->setText(text);

    return label;
}

// ----------------------------------------------------------------------------
std::size_t Util::combineHashes(std::size_t lhs, std::size_t rhs)
{
    lhs^= rhs + 0x9e3779b9 + (lhs << 6) + (lhs >> 2);
    return lhs;
}

// ----------------------------------------------------------------------------
QDir Util::getConfigDir()
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if(QDir(path).exists() == false)
        QDir().mkdir(path);

    return QDir(path);
}

} // namespace tfp
