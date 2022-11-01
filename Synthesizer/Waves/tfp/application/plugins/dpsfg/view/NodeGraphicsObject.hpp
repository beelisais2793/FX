#pragma once

#include "tfp/config.hpp"
#include <QGraphicsObject>

class NodeGraphicsObject : public QGraphicsObject
{
public:
    explicit NodeGraphicsObject(QGraphicsItem* parent=NULL);

protected:
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget=NULL) OVERRIDE;
    virtual QRectF boundingRect() const OVERRIDE;

    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event) OVERRIDE;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) OVERRIDE;
};
