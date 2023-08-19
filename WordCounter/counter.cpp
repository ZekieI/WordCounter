#include "counter.h"
#include "ui_counter.h"
#include <iostream>

void assignButton(QPushButton* button, QString image, int size)
{

    QPixmap pixmap(":/resources/img/" + image);
    QIcon ButtonIcon(pixmap);
    button->setIcon(ButtonIcon);
    button->setIconSize(QSize(size, size));
    button->setStyleSheet("border:0; background:transparent;");
}

Counter::Counter(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Counter)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::WindowType::FramelessWindowHint);
    this->setFixedSize(250,75);

    recording = false;
    in_word = true;
    count = 0;


    assignButton(this->ui->rec_button, "rec-button.png", 60);
    assignButton(this->ui->menu_button, "menu.png", 40);
    assignButton(this->ui->quit_button, "quit.png", 20);

}

Counter::~Counter()
{
    delete ui;
}

void Counter::increment()
{
    if(recording)
        count++;
    refresh();

}

void Counter::refresh()
{
    this->ui->counter_label->setText(QString::number(count));
}

void Counter::on_rec_button_clicked()
{
    if(recording)
        assignButton(this->ui->rec_button, "rec-button.png", 60);
    else
        assignButton(this->ui->rec_button, "pause.png", 40);
    recording = !recording;

}

void Counter::on_menu_button_clicked()
{
    hist->WriteHist();
    hist->LoadHist();
    hist->show();
}

void Counter::mousePressEvent(QMouseEvent *event) {
    cur_pos = event->globalPosition().toPoint();
}


void Counter::on_quit_button_clicked()
{
    this->close();
    hist->close();
}

void Counter::mouseMoveEvent(QMouseEvent *event) {
    new_pos = QPoint(event->globalPosition().toPoint() - cur_pos);
    move(x() + new_pos.x(), y() + new_pos.y());
    cur_pos = event->globalPosition().toPoint();
}
