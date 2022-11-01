#include "DPSFGView.hpp"
#include "DPSFGScene.hpp"
#include "NodeGraphicsObject.hpp"
#include "ConnectionGraphicsObject.hpp"
#include <QDebug>
#include <QPainter>
#include <QMouseEvent>
#include <cmath>

// ----------------------------------------------------------------------------
DPSFGView::DPSFGView(DPSFGScene* scene) :
    QGraphicsView(scene),
    scene_(scene),
    activeConnection_(NULL)
{
    setSceneRect(QRectF(0, 0, 1, 1));
}

// ----------------------------------------------------------------------------
void DPSFGView::mousePressEvent(QMouseEvent* event)
{
    QGraphicsView::mousePressEvent(event);

    QPointF pos = mapToScene(event->pos());

    if (event->button() == Qt::MiddleButton)
        lastMiddleClickPosition_ = pos;

    if (event->button() == Qt::RightButton)
    {
        lastRightClickPosition_ = pos;
        if (scene_->mouseGrabberItem() != NULL)
            activateConnection(pos);
    }
}

// ----------------------------------------------------------------------------
void DPSFGView::mouseReleaseEvent(QMouseEvent* event)
{
    QGraphicsView::mouseReleaseEvent(event);
    if (event->button() == Qt::RightButton)
    {
        QPointF pos = mapToScene(event->pos());

        float dx = pos.x() - lastRightClickPosition_.x();
        float dy = pos.y() - lastRightClickPosition_.y();
        if (dx*dx + dy*dy < 50)
            spawnNode(pos);

        finaliseConnection(pos, true);
    }
}

// ----------------------------------------------------------------------------
void DPSFGView::mouseMoveEvent(QMouseEvent* event)
{
    QGraphicsView::mouseMoveEvent(event);
    QPointF pos = mapToScene(event->pos());

    if (scene_->mouseGrabberItem() == NULL && event->buttons() & Qt::MiddleButton)
    {
        QPointF diff = lastMiddleClickPosition_ - pos;
        setSceneRect(sceneRect().translated(diff.x(), diff.y()));
    }

    updateActiveConnection(pos);
}

// ----------------------------------------------------------------------------
void DPSFGView::spawnNode(const QPointF& pos)
{
    QGraphicsItem* item = new NodeGraphicsObject;
    item->setPos(pos);
    scene_->addItem(item);
}

// ----------------------------------------------------------------------------
void DPSFGView::activateConnection(const QPointF& pos)
{
    activeConnection_ = new ConnectionGraphicsObject;
    activeConnection_->setPos(pos);
    scene_->addItem(activeConnection_);
}

// ----------------------------------------------------------------------------
void DPSFGView::updateActiveConnection(const QPointF& endPoint)
{
    if (activeConnection_ != NULL)
        activeConnection_->setEndPoint(endPoint - activeConnection_->pos());
}

// ----------------------------------------------------------------------------
void DPSFGView::finaliseConnection(const QPointF& endPoint, bool trashIt)
{
    if (activeConnection_ == NULL)
        return;

    if (trashIt == true)
        delete activeConnection_;

    activeConnection_ = NULL;
}

// ----------------------------------------------------------------------------
void DPSFGView::drawBackground(QPainter* painter, const QRectF& r)
{
    const unsigned char light = 230;
    const unsigned char dark = 210;

    QPen pen;
    pen.setWidth(1.0);
    pen.setColor(QColor(light, light, light));
    pen.setStyle(Qt::DashLine);
    painter->setPen(pen);
    drawGrid(painter, 15);

    pen.setColor(QColor(dark, dark, dark));
    pen.setStyle(Qt::SolidLine);
    painter->setPen(pen);
    drawGrid(painter, 150);
}

// ----------------------------------------------------------------------------
void DPSFGView::drawGrid(QPainter* painter, qreal gridStep)
{
    QRect windowRect = rect();
    QPointF topLeft = mapToScene(windowRect.topLeft());
    QPointF bottomRight = mapToScene(windowRect.bottomRight());

    double left   = std::floor(topLeft.x()     / gridStep);
    double right  = std::floor(bottomRight.x() / gridStep + 1.0);
    double top    = std::floor(topLeft.y()     / gridStep);
    double bottom = std::floor(bottomRight.y() / gridStep + 1.0);

    for (int i = int(left); i < int(right); ++i)
    {
        QLineF line(i * gridStep, bottom * gridStep, i * gridStep, top * gridStep);
        painter->drawLine(line);
    }

    // Horizontal lines
    for (int i = int(top); i < int(bottom); ++i)
    {
        QLineF line(left * gridStep, i * gridStep, right * gridStep, i * gridStep);
        painter->drawLine(line);
    }
}
