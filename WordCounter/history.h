#ifndef HISTORY_H
#define HISTORY_H

#include <QWidget>
#include <QtCharts>
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
    void LoadChart();
    void WriteHist();
    ~History();

private slots:
    void on_reloadButton_clicked();

private:
    void deleteChart();
    Ui::History *ui;
    QList<std::tuple<std::string, int>> history;
    QChartView *chartView;
};

#endif // HISTORY_H
