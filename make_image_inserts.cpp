#include "make_image_inserts.h"
#include "ui_make_image_inserts.h"
#include <QMessageBox>
#include <QPainter>
#include <QImageWriter>
#include <QStringList>

using namespace std;

make_image_inserts::make_image_inserts(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::make_image_inserts)
{
    ui->setupUi(this);
    template_item = NULL;
    ui->graphicsView->setScene(&template_scene);
    ui->width->setInputMask("9999");
    ui->height->setInputMask("9999");
    ui->xoffset->setInputMask("9999");
    ui->yoffset->setInputMask("9999");
    ui->rotation->setInputMask("000");
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
    QRegExp rx("([0-9]+)x([0-9]+)\\+([0-9]+)\\+([0-9]+)(?:$|/([0-9]+)$)");
//    QRegExp rx("[0-9]+x[0-9]+");
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
    int regexp_rc;
    regexp_rc = rx.indexIn(in_str);
    if (regexp_rc != -1) {
        QStringList list = rx.capturedTexts();
        /* The first match is the whole string, so skip it */
        QStringList::iterator it = ++list.begin();
        int item = geom_width;
        while ((it != list.end()) && (*it != "")) {
            int converted_num = QString_to_int(*it);
            switch(item) {
                case geom_width:
                    width = converted_num;
                    break;
                case geom_height:
                    height = converted_num;
                    break;
                case geom_xoffset:
                    xoffset = converted_num;
                    break;
                case geom_yoffset:
                    yoffset = converted_num;
                    break;
                case geom_rotation:
                    rotation = converted_num;
                    break;
                default:
                    error=true;
                    break;
            } /* endswitch */
            ++it;
            ++item;
        } /* endwhile */
        if (!error) {
            new_geom->geom_size = QSize(width, height);
            new_geom->geom_where = QPoint(xoffset, yoffset);
            new_geom->rotation_degrees = rotation;
        } /* endif */
    } else {
        new_geom->geom_size = QSize(0, 0);
        new_geom->geom_where = QPoint(0, 0);
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
        template_image = QImage(template_file_name);
        result_image = QImage();
        if (template_item != NULL) {
            template_scene.removeItem(template_item);
            delete template_item;
            template_item = NULL;
        } /* endif */
        template_item = new QGraphicsPixmapItem(QPixmap::fromImage(template_image));
        template_scene.addItem(template_item);
        img_keys = template_image.textKeys();
        insert_strings.clear();
        insert_geoms.clear();
        ui->insertion_points->clear();
        QStringList::ConstIterator i;
        for (i=img_keys.constBegin(); i != img_keys.constEnd(); ++i) {
            QString this_key;
            QString this_value;
            this_key = *i;
            if (this_key.startsWith("insert_loc_")) {
                this_value = template_image.text(this_key);
                insert_strings.push_back(this_value);
                insert_geoms.push_back(*convert_str_to_geom(this_value));
                ui->insertion_points->addItem(this_value);
            }
        } /* endfor */
    } /* endif */
}
void make_image_inserts::make_image_insert_add_files(QStringList *new_files) {
    QStringList::ConstIterator it = new_files->begin();
    while (it != new_files->end()) {
        QString insert_file_name = *it;
        QImage new_insert_image(insert_file_name);
        insert_dir = QFileInfo(insert_file_name).path();
        insert_images.push_back(QImage(insert_file_name));
        QListWidgetItem *new_insert = new QListWidgetItem(QIcon(insert_file_name), NULL, ui->listWidget);
        new_insert->setData(Qt::UserRole, insert_images.count()-1);
        ui->listWidget->insertItem(0, new_insert);
        new_insert->setSelected(true);
        ++it;
    } /* endwhile */
}

void make_image_inserts::on_actionOpenInsert_triggered()
{
    // Add a new insert image into the insert group.
    QFileDialog insert_dialog;
    QStringList selected_file_names;
    QStringList insert_file_names;

    selected_file_names = insert_dialog.getOpenFileNames(this,
         tr("Select 1 or more insert image file(s)"), insert_dir, tr("Images (*.png *.jpg *.gif)"));
    if (!selected_file_names.isEmpty()) {
        insert_file_names = selected_file_names;
        make_image_insert_add_files(&insert_file_names);
    } /* endif */
}

static void copy_text_keys(QImage &out, QImage &in) {
    QStringList img_keys;
    img_keys = in.textKeys();

    QStringList::ConstIterator i;
    for (i=img_keys.constBegin(); i != img_keys.constEnd(); ++i) {
        QString this_key;
        QString this_value;
        this_key = *i;
        this_value = in.text(this_key);
        out.setText(this_key, this_value);
    } /* endfor */
} /* copy_text_keys */

static void set_inserts(QImage &out, QStringList &inserts) {

    for (int i=0; i < inserts.count(); ++i) {
        QString this_key = "insert_loc_" + int_to_QString(i);
        QString this_value;
        this_value = inserts[i];
        out.setText(this_key, this_value);
    } /* endfor */
} /* copy_text_keys */

void make_image_inserts::on_actionInsert_images_into_template_triggered()
{
    QList<geom_angle>::iterator this_insert;
    QImage tmp_img = template_image;
    QImage result(tmp_img.width(), tmp_img.height(), QImage::Format_ARGB32_Premultiplied);
    QPainter pt(&result);
    pt.drawImage(QPoint(0,0), tmp_img);
    pt.setBackgroundMode(Qt::TransparentMode);
    bool error = false;

    for (this_insert = insert_geoms.begin(); this_insert != insert_geoms.end(); ++this_insert) {
        QList<QListWidgetItem *> current_selections(ui->listWidget->selectedItems());
        if (current_selections.isEmpty()) {
            QMessageBox select_an_insert;
            select_an_insert.setText("Please select an image to insert");
            select_an_insert.exec();
            error = true;
            break;
        } else {
            QListWidgetItem *current_item;
            double rotation = this_insert->rotation_degrees;
            QMatrix rm;
            rm.rotate(rotation);
            QPoint where;
            current_item = *current_selections.begin();
            QImage insert = insert_images[current_item->data(Qt::UserRole).toUInt()].transformed(rm, Qt::SmoothTransformation);
            QImage scaled_insert(this_insert->geom_size, QImage::Format_ARGB32_Premultiplied);

            scaled_insert.fill(qRgba(0,0,0,0));
            scaled_insert = insert.scaled(this_insert->geom_size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            int inset_img_width = scaled_insert.width();
            int inset_img_height = scaled_insert.height();
            int geom_width = this_insert->geom_size.width();
            int geom_height = this_insert->geom_size.height();
            int center_x_offset = geom_width/2 - inset_img_width/2;
            int center_y_offset = geom_height/2 - inset_img_height/2;
            where = this_insert->geom_where;
            where.setX(where.x() + center_x_offset);
            where.setY(where.y() + center_y_offset);
            pt.drawImage(where, scaled_insert);
        } /* endif */
    } /* endfor */
    if (!error) {
        template_item->setPixmap(QPixmap::fromImage(result));
        copy_text_keys(result, template_image);
        result_image = result;
    } /* endif */
}

void make_image_inserts::on_actionSave_triggered()
{
    bool save_error = false;
    if (result_file_name.isEmpty()) {
        on_actionSave_As_triggered();
    } else {
        QImageWriter save_writer(result_file_name);
        /* This covers the case where the template file is being prepped */
        if (result_image.isNull()) {
            result_image = template_image;
        }
        set_inserts(result_image, insert_strings);
        save_error = !save_writer.write(result_image);
        if (save_error) {
            QMessageBox file_save_error;
            file_save_error.setText("Unable to save result file " + save_writer.errorString());
            file_save_error.exec();
        } /* endif */
    } /* endif */
} /* Save to existing result file */

void make_image_inserts::on_actionSave_As_triggered()
{
    QFileDialog save_result_dialog;
    QString file_name;
    bool save_error = false;

    file_name = save_result_dialog.getSaveFileName(this,
         tr("Save result image file"), template_dir, tr("Images (*.png *.jpg *.gif)"));
    if (file_name != "") {
        result_file_name = file_name;
        QImageWriter save_writer(result_file_name);
        /* This covers the case where the template file is being prepped */
        if (result_image.isNull()) {
            result_image = template_image;
        }
        set_inserts(result_image, insert_strings);
        save_error = !save_writer.write(result_image);
        if (save_error) {
            QMessageBox file_save_error;
            file_save_error.setText("Unable to save result file " + save_writer.errorString());
            file_save_error.exec();
        } /* endif */
    }

}

void make_image_inserts::on_add_insert_button_clicked()
{
    geom_angle this_geom;
    this_geom.geom_size = QSize(QString_to_int(ui->width->text()), QString_to_int(ui->height->text()));
    this_geom.geom_where =QPoint(QString_to_int(ui->xoffset->text()), QString_to_int(ui->yoffset->text()));
    this_geom.rotation_degrees = (QString_to_int(ui->rotation->text()));
    insert_geoms.push_back(this_geom);
    insert_strings.push_back(this_geom.text());
    ui->insertion_points->addItem(this_geom.text());
}

void make_image_inserts::on_delete_insert_button_clicked()
{
    QList<QListWidgetItem *> selected_inserts = ui->insertion_points->selectedItems();

    /* One level of un-do for the moment */
    if (!removed_inserts.isEmpty()) {
        qDeleteAll(removed_inserts);
    } /* endif */
    removed_inserts = selected_inserts;

    for (int i=0; i<selected_inserts.size(); i++) {
        insert_geoms.removeAt(ui->insertion_points->row(selected_inserts[i]));
        ui->insertion_points->takeItem(ui->insertion_points->row(selected_inserts[i]));
    } /* endfor */

} /* on_delete_insert_button_clicked */

void make_image_inserts::on_insertion_points_currentItemChanged(QListWidgetItem *current, QListWidgetItem *)
{
    geom_angle this_geom = insert_geoms.at(ui->insertion_points->row(current));
    ui->width->setText(QString::fromStdString(int_to_string(this_geom.geom_size.width())));
    ui->height->setText(QString::fromStdString(int_to_string(this_geom.geom_size.height())));
    ui->xoffset->setText(QString::fromStdString(int_to_string(this_geom.geom_where.x())));
    ui->yoffset->setText(QString::fromStdString(int_to_string(this_geom.geom_where.y())));
    ui->rotation->setText(QString::fromStdString(int_to_string(this_geom.rotation_degrees)));
}

void make_image_inserts::on_update_insert_button_clicked()
{
    QListWidgetItem *current;
    geom_angle this_geom;
    current = ui->insertion_points->currentItem();
    this_geom.geom_size = QSize(QString_to_int(ui->width->text()), QString_to_int(ui->height->text()));
    this_geom.geom_where =QPoint(QString_to_int(ui->xoffset->text()), QString_to_int(ui->yoffset->text()));
    this_geom.rotation_degrees = (QString_to_int(ui->rotation->text()));
    insert_geoms[ui->insertion_points->row(current)] = this_geom;
    current->setText(this_geom.text());
}
