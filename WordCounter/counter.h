#ifndef COUNTER_H
#define COUNTER_H

#include <QWidget>
#include <QMouseEvent>
#include <QPoint>
#include <QtGui>
#include <QString>
#include "windows.h"
#include "history.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class Counter;
}
QT_END_NAMESPACE

class History;
class Counter : public QWidget
{
    Q_OBJECT

public:
    explicit Counter(QWidget *parent = nullptr);
    void increment();
    void refresh();
    bool in_word;
    int count;
    History* hist;

    ~Counter();

private slots:
    void on_rec_button_clicked();
    void on_menu_button_clicked();
    void on_quit_button_clicked();

private:
    Ui::Counter *ui;
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    QPoint cur_pos;
    QPoint new_pos;
    bool recording;
};

#endif // COUNTER_H
