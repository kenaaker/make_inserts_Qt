#include "dndqlistwidget.h"
#include <QDrag>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QDebug>

DnDQListWidget::DnDQListWidget(QWidget *parent) : QListWidget(parent) {
    setAcceptDrops(true);
}

void DnDQListWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        startPos = event->pos();
    } /* endif */
    QListWidget::mousePressEvent(event);
}

void DnDQListWidget::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        int distance = (event->pos() - startPos).manhattanLength();
        if (distance >= QApplication::startDragDistance()) {
            DnDstartDrag();
        } /* endif */
    } /* endif */
}

void DnDQListWidget::dragEnterEvent(QDragEnterEvent *event) {
    event->acceptProposedAction();
}

void DnDQListWidget::dragMoveEvent(QDragMoveEvent *event) {
    event->acceptProposedAction();
}

void DnDQListWidget::dropEvent(QDropEvent *event) {
    const QMimeData *mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        QList<QUrl> urlList = mimeData->urls();
        QStringList new_files;
        for (int i = 0; i < urlList.size(); ++i) {
            QString url = urlList.at(i).toLocalFile();
            new_files.push_back(url);
        } /* endfor */
        emit files_dropped(&new_files);
    } /* endif */
    event->acceptProposedAction();
}

void DnDQListWidget::DnDstartDrag(void) {
    QListWidgetItem *item = currentItem();
    if (item) {
        QMimeData *mimeData = new QMimeData;
        mimeData->setText(item->text());
        QDrag *drag = new QDrag(this);
        drag->setMimeData(mimeData);
        drag->setPixmap(QPixmap(":/images/person.png"));
        if (drag->start(Qt::MoveAction) == Qt::MoveAction) {
            delete item;
        } /* endif */
    } /* endif */
}
