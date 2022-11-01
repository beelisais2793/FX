#include "tfp/views/AdjustmentWidget.hpp"

#include <QLabel>
#include <QHBoxLayout>

namespace tfp {

// ----------------------------------------------------------------------------
AdjustmentWidget::AdjustmentWidget(QWidget* parent) :
    QWidget(parent),
    labelName_(NULL)
{
    setLayout(new QHBoxLayout);

    labelName_ = new QLabel;
    layout()->addWidget(labelName_);

    layout()->setContentsMargins(0, 0, 0, 0);
}

// ----------------------------------------------------------------------------
void AdjustmentWidget::setName(const QString& name)
{
    labelName_->setText(name);
}

} // namespace tfp
