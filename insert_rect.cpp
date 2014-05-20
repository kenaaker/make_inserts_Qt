#include "insert_rect.h"
#include <QDebug>

insert_rect::insert_rect(QGraphicsRectItem *parent) :
    QGraphicsRectItem(parent) {

}

insert_rect::insert_rect(const QRectF &rect, QGraphicsItem *parent, geom_angle *geom, QListWidgetItem * l_item) :
    QGraphicsRectItem(rect, parent) {
    image_geom = geom;
    list_item = l_item;
}

insert_rect::insert_rect(qreal x, qreal y, qreal w, qreal h, QGraphicsItem *parent) :
    QGraphicsRectItem(x, y, w, h, parent) {

}

QVariant insert_rect::itemChange(GraphicsItemChange change, const QVariant &value) {
    bool selected_rect;
    switch (change) {
        case QGraphicsRectItem::ItemSelectedChange:
            selected_rect = value.toBool();
            list_item->setSelected(true);
            break;
        case QGraphicsRectItem::ItemPositionChange:
            /* Change the corresponding text */ {
                QPoint new_pos = value.toPoint();
                image_geom->geom_where = new_pos;
                list_item->setData(Qt::DisplayRole, QVariant(image_geom->text()));
            }
            break;
        default:
            ;
    } /* end switch */
    return value;
}
