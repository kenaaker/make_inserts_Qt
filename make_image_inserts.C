#include "make_image_inserts.h"
#include "ui_make_image_inserts.h"
#include <iostream>
using namespace std;

static inline int string_to_int(QString s)
{
    stringstream ss(s.toStdString());
    int x;
    ss >> x;
    return x;
}

static inline string int_to_string(int i)
{
    stringstream ss;
    ss << i;
    return ss.str();
}

make_image_inserts::make_image_inserts(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::make_image_inserts)
{
    ui->setupUi(this);
    ui->graphicsView->setScene(&template_scene);

}

make_image_inserts::~make_image_inserts()
{
    delete ui;
}

void make_image_inserts::on_actionQuit_triggered()
{
    exit(0);
}

const geom_angle *convert_str_to_geom(QString in_str) {
    QRegExp rx("[0-9]+\\x[0-9]+\\+[0-9]+\\+[0-9]+($|/[0-9]+$");
    geom_angle *new_geom = new geom_angle;
    enum geom_order { geom_width,
                      geom_height,
                      geom_xoffset,
                      geom_yoffset,
                      geom_rotation
                    };
    int width = 0;
    int height = 0;
    int xoffset = 0;
    int yoffset = 0;
    int rotation = 0;

    bool error = false;

    if (rx.indexIn(in_str) != -1) {
        QStringList list = rx.capturedTexts();
        QStringList::iterator it = list.begin();
        int item = geom_width;
        while (it != list.end()) {
            string_to_int(*it);
            switch(item) {
                case geom_width:
                    width = string_to_int(*it);
                    break;
                case geom_height:
                    height = string_to_int(*it);
                    break;
                case geom_xoffset:
                    xoffset = string_to_int(*it);
                    break;
                case geom_yoffset:
                    yoffset = string_to_int(*it);
                    break;
                case geom_rotation:
                    rotation = string_to_int(*it);
                    break;
                default:
                    error=true;
                    break;
            } /* endswitch */
            if (!error) {
                new_geom->geom = QRect(xoffset, yoffset, width, height);
                new_geom->rotation_degrees = rotation;
            } /* endif */
            ++it;
            ++item;
        } /* endwhile */
    } else {
        new_geom->geom = QRect(0, 0, 0, 0);
        new_geom->rotation_degrees = 0;
    } /* endif */
    return new_geom;
}

void make_image_inserts::on_actionOpenTemplateImage_triggered()
{
    // Remove the current image from the Template area and load in a new one.
    QFileDialog template_dialog;
    QString template_file_name;
    QStringList img_keys;

    template_file_name = template_dialog.getOpenFileName(this,
                                                         tr("Open template image file"), template_dir, tr("Images (*.png *.jpg *.gif)"));
    if (template_file_name != "") {
        template_dir = QFileInfo(template_file_name).path();
        QImage new_template(template_file_name);
        template_scene.addItem(new QGraphicsPixmapItem(QPixmap::fromImage(new_template)));
        img_keys = new_template.textKeys();
        QStringList::ConstIterator i;
        for (i=img_keys.constBegin(); i != img_keys.constEnd(); ++i) {
            QString this_key;
            QString this_value;
            this_key = *i;
            if (this_key.startsWith("insert_loc_")) {
                QListWidgetItem this_item;
                this_value = new_template.text(this_key);
                cout << "image text key= \"" << this_key.toAscii().constData() << "\" value=\"" << this_value.toAscii().constData() << "\"" << endl;
                insert_strings.push_back(this_value);
                insert_geoms.push_back(*convert_str_to_geom(this_value));
                this_item.setText(this_value);
                ui->insertion_points->addItem(this_value);
            }
        } /* endfor */
    } /* endif */
}

void make_image_inserts::on_actionOpenInsert_triggered()
{
    // Add a new insert image into the insert group.
    QFileDialog insert_dialog;
    QString insert_file_name;

    insert_file_name = insert_dialog.getOpenFileName(this,
         tr("Open insert image file"), insert_dir, tr("Images (*.png *.jpg *.gif)"));
    if (insert_file_name != "") {
        QImage new_insert_image(insert_file_name);
        insert_dir = QFileInfo(insert_file_name).path();
        insert_images.push_back(QImage(insert_file_name));
        QListWidgetItem *new_insert = new QListWidgetItem(QIcon(insert_file_name), NULL, ui->listWidget);
        new_insert->setData(Qt::UserRole, insert_images.count());
        ui->listWidget->insertItem(0, new_insert);
    } /* endif */
}

void make_image_inserts::on_actionInsert_images_into_template_triggered()
{
    QList<geom_angle>::iterator this_insert;
    for (this_insert = insert_geoms.begin(); this_insert != insert_geoms.end(); ++this_insert) {
        QList<QListWidgetItem *> current_selections(ui->listWidget->selectedItems());
        QListWidgetItem *current_item;
        current_item = *current_selections.begin();
        QImage insert = insert_images[current_item->data(Qt::UserRole).toUInt()];
        QImage scaled_insert;
    } /* endfor */
}
