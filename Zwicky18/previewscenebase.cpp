#include "previewscenebase.h"

#include <QGraphicsSceneMouseEvent>
#include <QPainter>

PreviewSceneBase::PreviewSceneBase(QObject *parent)
    : QGraphicsScene(parent),
      m_modified(false)
{

    m_image = new QPixmap(1920, 1080);
    m_image->fill(Qt::gray);
    m_item = addPixmap(*m_image);
    m_item->setTransformationMode(Qt::SmoothTransformation);
    //this->addItem(m_item);
}

void PreviewSceneBase::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if ((event->buttons() & Qt::LeftButton) && m_scribbling) {
        drawLineTo(event->scenePos());
        event->accept();
    }
    else QGraphicsScene::mouseMoveEvent(event);
}

void PreviewSceneBase::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_lastPoint = event->scenePos();
        m_scribbling = true;
        event->accept();
    }
    else QGraphicsScene::mousePressEvent(event);
}

void PreviewSceneBase::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && m_scribbling) {
        drawLineTo(event->scenePos());
        m_scribbling = false;
        event->accept();
    }
    else QGraphicsScene::mouseReleaseEvent(event);
}

void PreviewSceneBase::drawLineTo(const QPointF &endPoint)
{
    QPainter painter(m_image);
    painter.setPen(QPen(m_penColor, m_penWidth, Qt::SolidLine, Qt::RoundCap,Qt::RoundJoin));
    painter.drawLine(m_lastPoint, endPoint);
    m_modified = true;
    m_lastPoint = endPoint;
    // m_item->setPixmap(*m_image);
}
