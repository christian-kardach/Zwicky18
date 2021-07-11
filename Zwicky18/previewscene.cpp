#include <QMouseEvent>
#include <QApplication>
#include <QScrollBar>
#include <qmath.h>
#include "previewscene.h"

PreviewScene::PreviewScene(QGraphicsView* view)
    : QObject(view), _view(view)
{
    _view->viewport()->installEventFilter(this);
    _view->setMouseTracking(true);
    _modifiers = Qt::ControlModifier;
    _zoom_factor_base = 1.0015;

    _view->setFixedWidth(1920);
    _view->setFixedHeight(1080);

    _scene = new QGraphicsScene(this);
    // _scene->setSceneRect()
    _scene->setBackgroundBrush(QBrush(QColor(0,0,0)));
    _view->setFixedWidth(1920);
    _view->setFixedHeight(1080);
    _view->centerOn(0, 0);
    _view->setScene(_scene);

    _image = new QPixmap(500, 500);
    _image->fill(Qt::darkGray);
    _imageItem = _scene->addPixmap(*_image);
}

void PreviewScene::updateFrame(QImage frame)
{
    //QGraphicsPixmapItem* frameItem = new QGraphicsPixmapItem(QPixmap::fromImage(frame));
    _imageItem->setPixmap(QPixmap::fromImage(frame));
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

void PreviewScene::gentle_zoom(double factor) {
  _view->scale(factor, factor);
  _view->centerOn(target_scene_pos);
  QPointF delta_viewport_pos = target_viewport_pos - QPointF(_view->viewport()->width() / 2.0,
                                                             _view->viewport()->height() / 2.0);
  QPointF viewport_center = _view->mapFromScene(target_scene_pos) - delta_viewport_pos;
  _view->centerOn(_view->mapToScene(viewport_center.toPoint()));
  emit zoomed();
}

void PreviewScene::set_modifiers(Qt::KeyboardModifiers modifiers) {
  _modifiers = modifiers;

}

void PreviewScene::set_zoom_factor_base(double value) {
  _zoom_factor_base = value;
}

bool PreviewScene::eventFilter(QObject *object, QEvent *event) {
  if (event->type() == QEvent::MouseMove) {
    QMouseEvent* mouse_event = static_cast<QMouseEvent*>(event);
    QPointF delta = target_viewport_pos - mouse_event->pos();
    if (qAbs(delta.x()) > 5 || qAbs(delta.y()) > 5) {
      target_viewport_pos = mouse_event->pos();
      target_scene_pos = _view->mapToScene(mouse_event->pos());
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
