#include "tfp/models/System.hpp"
#include <QTreeWidgetItem>

namespace tfp {

// ----------------------------------------------------------------------------
System::System(const QString& name) :
    dataTree_(NULL),
    name_(name)
{
}

// ----------------------------------------------------------------------------
System::~System()
{
    setDataTree(NULL);
}

// ----------------------------------------------------------------------------
void System::notifyParametersChanged()
{
    CoefficientPolynomial<double> coeffNumerator = numerator().poly();
    CoefficientPolynomial<double> coeffDenominator = denominator().poly();
    QString buf;

#define UPDATE_CHILD_VALUES_COMPLEX(item, polynomial)                        \
    buf= "[";                                                                \
    for (int i = 0; i != polynomial.size(); ++i)                             \
    {                                                                        \
        QString num = QString::number(polynomial.root(i).real()) + "+" +     \
                      QString::number(polynomial.root(i).imag()) + "j";      \
        item->child(i)->setText(1, num);                                     \
        if (i > 0)                                                           \
            buf += ", ";                                                     \
        buf += num;                                                          \
    }                                                                        \
    buf += "]";                                                              \
    item->setText(1, buf);

#define UPDATE_CHILD_VALUES_REAL(item, polynomial)                           \
    buf= "[";                                                                \
    for (int i = 0; i != polynomial.size(); ++i)                             \
    {                                                                        \
        QString num = QString::number(polynomial.coefficient(i));            \
        item->child(i)->setText(1, num);                                     \
        if (i > 0)                                                           \
            buf += ", ";                                                     \
        buf += num;                                                          \
    }                                                                        \
    buf += "]";                                                              \
    item->setText(1, buf);

    factorItem_->setText(1, QString::number(numerator().factor() / denominator().factor()));
    UPDATE_CHILD_VALUES_COMPLEX(polesItem_, denominator());
    UPDATE_CHILD_VALUES_COMPLEX(zerosItem_, numerator());
    UPDATE_CHILD_VALUES_REAL(coeffBItem_, coeffNumerator);
    UPDATE_CHILD_VALUES_REAL(coeffAItem_, coeffDenominator);

    dispatcher.dispatch(&SystemListener::onSystemParametersChanged);
}

// ----------------------------------------------------------------------------
void System::notifyStructureChanged()
{
    CoefficientPolynomial<double> coeffNumerator = numerator().poly();
    CoefficientPolynomial<double> coeffDenominator = denominator().poly();
#define UPDATE_CHILD_COUNT(item, polynomial, name)                           \
    while (item->childCount() < polynomial.size())                           \
        item->addChild(new QTreeWidgetItem(QStringList(name + QString::number(item->childCount())))); \
    while (item->childCount() > polynomial.size())                           \
        delete item->takeChild(item->childCount() - 1);
    UPDATE_CHILD_COUNT(polesItem_, denominator(), "p");
    UPDATE_CHILD_COUNT(zerosItem_, numerator(), "z");
    UPDATE_CHILD_COUNT(coeffBItem_, coeffNumerator, "b");
    UPDATE_CHILD_COUNT(coeffAItem_, coeffDenominator, "a");

    dispatcher.dispatch(&SystemListener::onSystemStructureChanged);
}

// ----------------------------------------------------------------------------
void System::interestingFrequencyInterval(double* xStart, double* xEnd) const
{
    double minx = std::numeric_limits<double>::max();
    double maxx = 0;

    for (int i = 0; i != denominator().size(); ++i)
    {
        double mag = std::abs(denominator().root(i));
        if (maxx < mag)
            maxx = mag;
        if (minx > mag)
            minx = mag;
    }

    // In case of zero poles (because logarithmic plots depend on these ranges)
    if (minx == 0)
        minx = maxx;

    *xStart = minx * 0.01;
    *xEnd   = maxx * 100;
}

// ----------------------------------------------------------------------------
void System::interestingTimeInterval(double* xStart, double* xEnd) const
{
    double closestpole = std::numeric_limits<double>::max();

    for (int i = 0; i != denominator().size(); ++i)
    {
        double real = std::abs(denominator().root(i).real());
        if (closestpole > real)
            closestpole = real;
    }

    *xStart = 0;
    *xEnd   = 10 / closestpole;
}

// ----------------------------------------------------------------------------
const QString& System::name() const
{
    return name_;
}

// ----------------------------------------------------------------------------
void System::setDataTree(QTreeWidgetItem* tree)
{
    if (dataTree_ != NULL)
        delete dataTree_->takeChild(dataTree_->indexOfChild(tfItem_));

    dataTree_ = tree;

    if (dataTree_ == NULL)
        return;

    tfItem_ = new QTreeWidgetItem(dataTree_);
    tfItem_->setText(0, "Transfer Function");
    tfItem_->setExpanded(true);
    factorItem_ = new QTreeWidgetItem(tfItem_);
    factorItem_->setText(0, "Factor");
    polesItem_ = new QTreeWidgetItem(tfItem_);
    polesItem_->setText(0, "Poles");
    zerosItem_ = new QTreeWidgetItem(tfItem_);
    zerosItem_->setText(0, "Zeros");
    coeffBItem_ = new QTreeWidgetItem(tfItem_);
    coeffBItem_->setText(0, "Numerator");
    coeffAItem_ = new QTreeWidgetItem(tfItem_);
    coeffAItem_->setText(0, "Denominator");

    dataTree_->treeWidget()->resizeColumnToContents(0);
}

// ----------------------------------------------------------------------------
QTreeWidgetItem* System::dataTree()
{
    return dataTree_;
}

}
