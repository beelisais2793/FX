#include "NodeGraphicsObject.hpp"
#include <QPainter>
#include <QDebug>

// ----------------------------------------------------------------------------
NodeGraphicsObject::NodeGraphicsObject(QGraphicsItem* parent) :
    QGraphicsObject(parent)
{
}

// ----------------------------------------------------------------------------
void NodeGraphicsObject::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    const unsigned char color = 40;

    QPen pen;
    pen.setColor(QColor(color, color, color));

    QBrush brush;
    brush.setColor(QColor(color, color, color));
    brush.setStyle(Qt::SolidPattern);

    painter->setPen(pen);
    painter->setBrush(brush);
    painter->drawEllipse(QPointF(0, 0), 5, 5);
}

// ----------------------------------------------------------------------------
QRectF NodeGraphicsObject::boundingRect() const
{
    return QRectF(-5, -5, 10, 10);
}

// ----------------------------------------------------------------------------
void NodeGraphicsObject::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    qDebug() << "press";
}

// ----------------------------------------------------------------------------
void NodeGraphicsObject::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    qDebug() << "release";
}
