#include "ConnectionGraphicsObject.hpp"
#include <QPainter>
#include <QDebug>

// ----------------------------------------------------------------------------
ConnectionGraphicsObject::ConnectionGraphicsObject(QGraphicsItem* parent) :
    QGraphicsObject(parent)
{
}

// ----------------------------------------------------------------------------
void ConnectionGraphicsObject::setEndPoint(const QPointF& p)
{
    QPointF c1(p.x(), 0);
    QPointF c2(0, p.y());
    QPainterPath path;
    path.cubicTo(c1, c2, p);

    prepareGeometryChange();
    path_ = path;
}

// ----------------------------------------------------------------------------
void ConnectionGraphicsObject::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    const unsigned char color = 40;

    QPen pen;
    pen.setColor(QColor(color, color, color));

    painter->setPen(pen);
    painter->drawPath(path_);
}

// ----------------------------------------------------------------------------
QRectF ConnectionGraphicsObject::boundingRect() const
{
    return path_.boundingRect();
}
