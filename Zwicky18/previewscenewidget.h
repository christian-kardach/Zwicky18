#ifndef PREVIEWSCENEWIDGET_H
#define PREVIEWSCENEWIDGET_H

#include <QObject>
#include <QWidget>
#include <QGraphicsView>

#include "previewscenebase.h"

class PreviewSceneWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PreviewSceneWidget(QWidget *parent = nullptr);

    QGraphicsView *m_view;
    PreviewSceneBase *m_scene;


    void updateHistogram();

public slots:
    void updateFrame(QImage frame);

private:
    QGraphicsPixmapItem *frameItem;
    void resizeEvent(QResizeEvent* event) override;

    signals:

};

#endif // PREVIEWSCENEWIDGET_H
