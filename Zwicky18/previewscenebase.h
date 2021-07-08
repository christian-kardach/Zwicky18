#ifndef PREVIEWSCENEBASE_H
#define PREVIEWSCENEBASE_H

#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>

class PreviewSceneBase : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit PreviewSceneBase(QObject *parent = nullptr);

    QPixmap *m_image;
    QGraphicsPixmapItem *m_item;

private:
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void drawLineTo(const QPointF &endPoint);

    bool m_modified;
    bool m_scribbling;
    int m_penWidth;
    QColor m_penColor;
    QPointF m_lastPoint;
    // QPixmap *m_image;

signals:
};

#endif // PREVIEWSCENEBASE_H
