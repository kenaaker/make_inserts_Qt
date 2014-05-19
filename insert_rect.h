#ifndef INSERT_RECT_H
#define INSERT_RECT_H

#include <QGraphicsRectItem>
#include "make_image_inserts.h"

class insert_rect : public QGraphicsRectItem {
public:
    explicit insert_rect(QGraphicsRectItem *parent = 0);
    explicit insert_rect(const QRectF &rect, QGraphicsItem *parent = 0, const geom_angle *geom = 0);
    explicit insert_rect(qreal x, qreal y, qreal w, qreal h, QGraphicsItem *parent = 0);

signals:

public slots:

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
private:
    const geom_angle *image_geom;     /* Insertion image geometry spec. */
};

#endif // INSERT_RECT_H
