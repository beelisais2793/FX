#include "tfp/models/Translation.hpp"
#include <QtGlobal>

namespace tfp {

const char* ID_APP_TITLE = QT_TRID_NOOP("Transfer Function Plotter");
const char* ID_DESCRIPTION_ALLOWVARIABLEEXPONENTS = QT_TRID_NOOP("Allows variable expressions to appear in the exponents of polynomials in transfer functions. Only integer exponents are supported.\n\nDecreases performance if such exponents are present, because the expression tree has to be rebuilt every time a variable changes.");
const char* ID_DESCRIPTION_ANALYTICALEXPONENTS = QT_TRID_NOOP("Separates the exponential expressions (e^(-as)) from polynimial expressions (s^a), and then tries to find the inverse Laplace transform analytically. Simple time delays are solvable, but anything more complicated will fail.");
const char* ID_DESCRIPTION_EXPONENTIALEXPANSION = QT_TRID_NOOP("If finding an analytical solution to an exponential expression fails, this option will approximate all exponential expressions (e^s) using a series expansion (1 + s^1/1! + s^2/2! + s^3/3! + ...)");
const char* ID_DESCRIPTION_EXPANSIONCOUNT = QT_TRID_NOOP("How many expansions should be performed.");
const char* ID_DESCRIPTION_OPTIMALDATAPOINTSMODE = QT_TRID_NOOP("Make an estimate of how many data points of resolution are required based on the fastest time-constant in the transfer function.\n\nThe estimated value is clamped between the following minimum and maximum values.");
const char* ID_DESCRIPTION_FIXEDDATAPOINTSMODE = QT_TRID_NOOP("Use a fixed number of data points when plotting signals.");
const char* ID_DESCRIPTION_NONE = QT_TRID_NOOP("No description available.");

}
