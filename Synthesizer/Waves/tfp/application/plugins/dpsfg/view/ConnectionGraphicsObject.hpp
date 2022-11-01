#pragma once

#include "tfp/config.hpp"
#include <QGraphicsObject>

class ConnectionGraphicsObject : public QGraphicsObject
{
public:
    explicit ConnectionGraphicsObject(QGraphicsItem* parent=NULL);

    void setEndPoint(const QPointF& p);

protected:
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget=NULL) OVERRIDE;
    virtual QRectF boundingRect() const OVERRIDE;

private:
    QPainterPath path_;
};
