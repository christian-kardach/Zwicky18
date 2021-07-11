#ifndef PREVIEWSCENEWIDGET_H
#define PREVIEWSCENEWIDGET_H

#include <QObject>
#include <QWidget>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QApplication>
#include <QScrollBar>
#include <qmath.h>

#include "previewscenebase.h"

class PreviewSceneWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PreviewSceneWidget(QWidget *parent = nullptr);

    void gentle_zoom(double factor);
    void set_modifiers(Qt::KeyboardModifiers modifiers);
    void set_zoom_factor_base(double value);

    void updateHistogram();

public slots:
    void updateFrame(QImage frame);
    void centerInView();

private:
    QGraphicsView *m_view;
    PreviewSceneBase *m_scene;

    QGraphicsPixmapItem *frameItem;
    void resizeEvent(QResizeEvent* event) override;
    bool eventFilter(QObject* object, QEvent* event) override;


    Qt::KeyboardModifiers _modifiers;
    double _zoom_factor_base;
    QPointF target_scene_pos, target_viewport_pos;


signals:
    void zoomed();
};

#endif // PREVIEWSCENEWIDGET_H
