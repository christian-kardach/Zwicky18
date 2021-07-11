#include "previewscenewidget.h"

PreviewSceneWidget::PreviewSceneWidget(QWidget *parent) : QWidget(parent)
{
    m_view = new QGraphicsView(this);
    m_scene = new PreviewSceneBase(this);
    // m_view->setDragMode(QGraphicsView::ScrollHandDrag);
    m_view->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    m_scene->setBackgroundBrush(QBrush(QColor(0,0,0)));
    //m_scene->setSceneRect(QRectF(0, 0, this->parentWidget()->geometry().width()-100, this->parentWidget()->geometry().height()-100));

    m_view->viewport()->installEventFilter(this);
    m_view->setMouseTracking(true);
    _modifiers = Qt::ControlModifier;
    _zoom_factor_base = 1.0015;

    m_view->setScene(m_scene);
    //m_view->setFixedWidth(3000);
   // m_view->setFixedHeight(3000);
    //m_view->centerOn(1000, 550);


    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
    m_view->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);

    centerInView();
    updateHistogram();

    //m_view->fitInView(m_scene->itemsBoundingRect() ,Qt::KeepAspectRatio);
}

void PreviewSceneWidget::resizeEvent(QResizeEvent* event)
{
    // qDebug() << this->parentWidget()->geometry().width();
    //m_scene->setSceneRect(QRectF(0, 0, this->parentWidget()->geometry().width()-100, this->parentWidget()->geometry().height()-100));
    //m_view->setFixedWidth(this->parentWidget()->geometry().width());
    //m_view->setFixedHeight(this->parentWidget()->geometry().height());

     //m_view->scale(0.75, 0.75);
    //DrawableSceneWidget::resizeEvent(event);
}

void PreviewSceneWidget::updateFrame(QImage frame)
{
    //QGraphicsPixmapItem* frameItem = new QGraphicsPixmapItem(QPixmap::fromImage(frame));
    m_scene->m_item->setPixmap(QPixmap::fromImage(frame));
    //m_view->fitInView(m_scene->sceneRect(), Qt::KeepAspectRatio);
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

void PreviewSceneWidget::centerInView()
{
    //m_view->ensureVisible(m_scene->sceneRect());
    //m_view->fitInView(m_scene->sceneRect(), Qt::KeepAspectRatio);
    //m_view->fitInView(m_scene->itemsBoundingRect(), Qt::KeepAspectRatio);
}

void PreviewSceneWidget::updateHistogram()
{
    QGraphicsRectItem* item1 = new QGraphicsRectItem(0,m_scene->sceneRect().height()-100,100,100);
    item1->setBrush(QBrush(Qt::red));
    m_scene->addItem(item1);
}

void PreviewSceneWidget::gentle_zoom(double factor) {
  m_view->scale(factor, factor);
  m_view->centerOn(target_scene_pos);
  QPointF delta_viewport_pos = target_viewport_pos - QPointF(m_view->viewport()->width() / 2.0,
                                                             m_view->viewport()->height() / 2.0);
  QPointF viewport_center = m_view->mapFromScene(target_scene_pos) - delta_viewport_pos;
  m_view->centerOn(m_view->mapToScene(viewport_center.toPoint()));
  emit zoomed();
}

void PreviewSceneWidget::set_modifiers(Qt::KeyboardModifiers modifiers) {
  _modifiers = modifiers;

}

void PreviewSceneWidget::set_zoom_factor_base(double value) {
  _zoom_factor_base = value;
}

bool PreviewSceneWidget::eventFilter(QObject *object, QEvent *event) {

    if (event->type() == QEvent::MouseMove) {
    QMouseEvent* mouse_event = static_cast<QMouseEvent*>(event);
    QPointF delta = target_viewport_pos - mouse_event->pos();
    if (qAbs(delta.x()) > 5 || qAbs(delta.y()) > 5) {
      target_viewport_pos = mouse_event->pos();
      target_scene_pos = m_view->mapToScene(mouse_event->pos());
    }
  } else if (event->type() == QEvent::Wheel) {
    QWheelEvent* wheel_event = (QWheelEvent*)(event);
    if (QApplication::keyboardModifiers() == _modifiers) {
        double angle = wheel_event->angleDelta().y();
        double factor = qPow(_zoom_factor_base, angle);
        gentle_zoom(factor);
        return true;
        /*
      if (wheel_event->orientation() == Qt::Vertical) {
        double angle = wheel_event->angleDelta().y();
        double factor = qPow(_zoom_factor_base, angle);
        gentle_zoom(factor);
        return true;
      }
      */
    }
  }
  Q_UNUSED(object)
  return false;
}
