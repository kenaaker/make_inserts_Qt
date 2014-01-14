#ifndef MAKE_IMAGE_INSERTS_H
#define MAKE_IMAGE_INSERTS_H

#include <QMainWindow>
#include <QList>
#include <QListWidget>
#include <QFileDialog>
#include <QString>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QDir>
#include <QStringListIterator>
#include <QRect>
#include <sstream>
#include <QRegExp>

namespace Ui {
class make_image_inserts;
}

class geom_angle {
public:
    QRect geom;                 /* geometry location WxH+x+y */
    double rotation_degrees;    /* rotation of insertion image before insert */
    bool operator<(const geom_angle& rhs) const {
        if ((geom.width()*geom.height()) < (rhs.geom.width()*rhs.geom.height())) {
            return true;
        } else if ((geom.width()*geom.height()) > (rhs.geom.width()*rhs.geom.height())) {
            return false;
        } else {
            return (rotation_degrees < rhs.rotation_degrees);
        } /* endif */
    } /* operator< */
    bool operator==(const geom_angle& rhs) const {
        return ((geom == rhs.geom) &&
                (rotation_degrees == rhs.rotation_degrees));
    } /* operator== */
};

class make_image_inserts : public QMainWindow
{
    Q_OBJECT

public:
    explicit make_image_inserts(QWidget *parent = 0);
    ~make_image_inserts();

private slots:
    void on_actionQuit_triggered();

    void on_actionOpenTemplateImage_triggered();

    void on_actionOpenInsert_triggered();

    void on_actionInsert_images_into_template_triggered();

private:
    Ui::make_image_inserts *ui;
    QGraphicsScene template_scene;
    QGraphicsPixmapItem template_image;
    QString insert_dir;
    QString template_dir;
    QStringList insert_strings;
    QList<geom_angle> insert_geoms;
    QList<QImage> insert_images;
};

#endif // MAKE_IMAGE_INSERTS_H
