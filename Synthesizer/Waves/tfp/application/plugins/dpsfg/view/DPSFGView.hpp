#pragma once

#include "tfp/config.hpp"
#include <QGraphicsView>

class DPSFGScene;
class ConnectionGraphicsObject;

class DPSFGView : public QGraphicsView
{
  Q_OBJECT

public:
    explicit DPSFGView(DPSFGScene* scene);

protected:
    virtual void mousePressEvent(QMouseEvent* event) OVERRIDE;
    virtual void mouseReleaseEvent(QMouseEvent* event) OVERRIDE;
    virtual void mouseMoveEvent(QMouseEvent* event) OVERRIDE;
    virtual void drawBackground(QPainter* painter, const QRectF& r) OVERRIDE;

private:
    void spawnNode(const QPointF& pos);
    void activateConnection(const QPointF& pos);
    void updateActiveConnection(const QPointF& endPoint);
    void finaliseConnection(const QPointF& endPoint, bool trashIt);
    void drawGrid(QPainter* painter, qreal gridStep);

private:
    DPSFGScene* scene_;
    QPointF lastMiddleClickPosition_;
    QPointF lastRightClickPosition_;
    ConnectionGraphicsObject* activeConnection_;
};
