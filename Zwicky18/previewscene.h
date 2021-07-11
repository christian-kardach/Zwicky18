#ifndef PREVIEWSCENE_H
#define PREVIEWSCENE_H

#include <QObject>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>

class PreviewScene : public QObject
{
    Q_OBJECT
public:
    explicit PreviewScene(QGraphicsView* view);
    void gentle_zoom(double factor);
    void set_modifiers(Qt::KeyboardModifiers modifiers);
    void set_zoom_factor_base(double value);

private:
    QGraphicsView* _view;
    QGraphicsScene* _scene;

    QPixmap* _image;
    QGraphicsPixmapItem* _imageItem = nullptr;

    Qt::KeyboardModifiers _modifiers;
    double _zoom_factor_base;
    QPointF target_scene_pos, target_viewport_pos;
    bool eventFilter(QObject* object, QEvent* event);


public slots:
    void updateFrame(QImage frame);

signals:
  void zoomed();

};

#endif // PREVIEWSCENE_H
