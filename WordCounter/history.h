#ifndef HISTORY_H
#define HISTORY_H

#include <QWidget>
#include "counter.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class History;
}
QT_END_NAMESPACE

class Counter;
class History : public QWidget
{
    Q_OBJECT

public:
    explicit History(QWidget *parent = nullptr);
    Counter* counter;
    void LoadHist();
    void WriteHist();
    ~History();

private slots:
    void on_pushButton_clicked();

private:
    Ui::History *ui;
    QList<std::tuple<std::string, int>> history;
};

#endif // HISTORY_H
