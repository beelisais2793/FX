#include "jshelper.h"

JsHelper::JsHelper()
{

}

void JsHelper::displayEquation(QString equation)
{
    emit showEquation(equation);
}
