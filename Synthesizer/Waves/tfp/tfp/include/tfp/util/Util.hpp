#pragma once

#include "tfp/config.hpp"
#include <QDir>

class QLayout;
class QwtPlotTextLabel;

namespace tfp {

struct TFP_PUBLIC_API Util
{
    /*!
     * @brief Deletes all widgets, layouts, and spacer items from a layout,
     * effectively making it empty.
     */
    static void clearLayout(QLayout* layout);

    /*!
     * @brief Creates the "Loading..." text label for a Qwt plot. Since a few
     * parts of the program need these labels, the function for creating it
     * has been centralised here.
     */
    static QwtPlotTextLabel* createLoadingTextLabel();

    /*!
     * @brief Taken from boost::hash_combine. Combines two hash values into a
     * new hash value.
     */
    static std::size_t combineHashes(std::size_t lhs, std::size_t rhs);

    static QDir getConfigDir();
};

} // namespace tfp
