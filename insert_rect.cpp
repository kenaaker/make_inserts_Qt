#include "insert_rect.h"
#include <QDebug>

insert_rect::insert_rect(QGraphicsRectItem *parent) :
    QGraphicsRectItem(parent) {

}

insert_rect::insert_rect(const QRectF &rect, QGraphicsItem *parent, const geom_angle *geom) :
    QGraphicsRectItem(rect, parent) {
    image_geom = geom;
}

insert_rect::insert_rect(qreal x, qreal y, qreal w, qreal h, QGraphicsItem *parent) :
    QGraphicsRectItem(x, y, w, h, parent) {

}

QVariant insert_rect::itemChange(GraphicsItemChange change, const QVariant &value) {
    bool selected_rect;
    switch (change) {
        case QGraphicsRectItem::ItemSelectedChange:
            selected_rect = value.toBool();
            qDebug() << "Item Selection Changed. Item is " << image_geom->text();
            if (selected_rect) {
                qDebug() << "Selected";
            } else {
                qDebug() << "not Selected";
            } /* endif */
            break;
        case QGraphicsRectItem::ItemPositionChange:
            qDebug() << "Rectangle position changed";
            break;
        default:
            ;
    } /* end switch */
    return value;
}
