#ifndef DNDQLISTWIDGET_H
#define DNDQLISTWIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QMouseEvent>
#include <QApplication>

class DnDQListWidget : public QListWidget {
    Q_OBJECT
public:
    explicit DnDQListWidget(QWidget *parent = 0);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);

signals:
    void files_dropped(QStringList *files);

public slots:

private:
    void DnDstartDrag(void);
    QPoint startPos;

};

#endif // DNDQLISTWIDGET_H
