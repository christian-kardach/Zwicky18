#include "previewscenewidget.h"

PreviewSceneWidget::PreviewSceneWidget(QWidget *parent) : QWidget(parent)
{
    m_view = new QGraphicsView(this);
    m_scene = new PreviewSceneBase(this);
    m_scene->setBackgroundBrush(QBrush(QColor(0,0,0)));
    m_view->setFixedWidth(1000);
    m_view->setFixedHeight(1000);
    m_view->centerOn(0, 0);
    m_view->setScene(m_scene);
}

void PreviewSceneWidget::resizeEvent(QResizeEvent* event)
{
    // qDebug() << this->parentWidget()->geometry().width();
    m_scene->setSceneRect(QRectF(0, 0, this->parentWidget()->geometry().width()-100, this->parentWidget()->geometry().height()-100));
    m_view->setFixedWidth(this->parentWidget()->geometry().width());
    m_view->setFixedHeight(this->parentWidget()->geometry().height());

    //DrawableSceneWidget::resizeEvent(event);
}

void PreviewSceneWidget::updateFrame(QImage frame)
{
    // frameItem = new QGraphicsPixmapItem(QPixmap::fromImage(frame));
    m_scene->m_item->setPixmap(QPixmap::fromImage(frame));
    /*
    m_scene->clear();
    //m_scene->setSceneRect(0, 0, frame.width(), frame.height());
    if(frame.height() > m_view->height() && frame.width()>m_view->width())
    {
        //m_view->ensureVisible(m_scene->sceneRect());
        //m_view->fitInView(m_scene->sceneRect(), Qt::KeepAspectRatio);
        //m_view->fitInView(m_scene->itemsBoundingRect() ,Qt::KeepAspectRatio);
    }
    m_scene->addItem(frameItem);
    updateHistogram();
    */
}

void PreviewSceneWidget::updateHistogram()
{
    QGraphicsRectItem* item1 = new QGraphicsRectItem(0,m_scene->sceneRect().height()-100,100,100);
    item1->setBrush(QBrush(Qt::red));
    m_scene->addItem(item1);
}
