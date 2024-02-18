#include "history.h"
#include "ui_history.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <QString>
#include <QDebug>
#include <QtCharts>

std::string getDate()
{
    std::ostringstream stringStream;
    time_t time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    struct tm * date = std::localtime(&time);

    stringStream << std::setfill('0') << std::setw(2) << date->tm_mday << "/"
                 << std::setfill('0') << std::setw(2)<< date->tm_mon + 1 << "/"
                 << date->tm_year + 1900;
    return stringStream.str();
}

History::History(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::History)
{
    ui->setupUi(this);
    this->setWindowTitle("WordCounter");
}

History::~History()
{
    delete ui;
}

void History::deleteChart()
{
    if (chartView)
    {
        QLayout *layout = this->layout();
        if (layout) {
            layout->removeWidget(chartView);
        }
        delete chartView;
        chartView = nullptr;
    }
}

void History::WriteHist()
{
    std::ostringstream stringStream;
    bool written = false;

    std::string today = getDate();
    for(auto data : history)
    {

        if(std::strcmp(std::get<0>(data).c_str(), today.c_str()) == 0)
        {
            stringStream << std::get<0>(data) << "," << counter->count << "\n";
            written = true;
        }
        else
            stringStream << std::get<0>(data) << "," << std::get<1>(data) << "\n";
    }
    if (!written)
        stringStream << getDate() << "," << counter->count << "\n";
    QFile file("history.csv");
    if (file.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text))
    {
        file.write(stringStream.str().c_str());
    }
    file.close();
}
void History::LoadHist()
{
    history.clear();
    QFile file("history.csv");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    while (!in.atEnd()) {
        QStringList line = in.readLine().split(u',');
        std::string date = line[0].toStdString();
        int count = line[1].toInt();
        history.append(std::make_tuple(date, count));
    }
    file.close();

    const char* last_histo = std::get<0>(history.back()).c_str();
    //const char* today = getDate().c_str();
    if(std::strcmp(last_histo, getDate().c_str()) == 0)
    {
        counter->count = std::get<1>(history.back());
        counter->refresh();
    }

}

std::vector<std::string>* buid_dates(std::chrono::system_clock::time_point min, std::chrono::system_clock::time_point max)
{
    std::vector<std::string> *ret = new std::vector<std::string>;
    while (min < max)
    {
        std::time_t time = std::chrono::system_clock::to_time_t(min);
        std::tm* tm = std::gmtime(&time);
        int day = tm->tm_mday;
        int month = tm->tm_mon + 1;
        int year = tm->tm_year + 1900;
        std::stringstream ss;

        if (day < 10 && month < 10)
            ss << "0" << day << "/" << "0"<< month << "/" << year;
        else if (day < 10)
            ss << "0" << day << "/" << month << "/" << year;
        else if (month < 10)
            ss << day << "/" << "0"<< month << "/" << year;
        else
            ss << day << "/" << month << "/" << year;

        std::string fullDate = ss.str();
        ret->emplace_back(fullDate);
        min += std::chrono::hours(24);
    }
    return ret;
}

void History::LoadChart()
{
    LoadHist();
    deleteChart();
    QBarSet *set0 = new QBarSet("");
    QStringList categories {};

    auto current_date = std::chrono::system_clock::now();

    std::tm tm = {};
    std::istringstream ss(std::get<0>(history[0]));
    ss >> std::get_time(&tm, "%d/%m/%Y");
    std::time_t time = std::mktime(&tm);
    auto limit_date = std::chrono::system_clock::from_time_t(time);

    int spinnbr = this->ui->spinBox->value(); //get spinbox

    if (this->ui->weekbutton->isChecked())
        limit_date = current_date - std::chrono::hours(24 * 7 * spinnbr);

    if (this->ui->monthButton->isChecked())
        limit_date = current_date - std::chrono::hours(24 * 30 * spinnbr);

    if (this->ui->checkBox->isChecked())
    {
        auto dates = buid_dates(limit_date, current_date);
        int index = 0;
        for (const std::string& date : *dates) {
            const std::string& Hdate = std::get<0>(history[index]);
            const int& count = std::get<1>(history[index]);
            //std::cout << "'" << date <<"'" << Hdate << "'\n" << std::flush;
           if (Hdate ==  date)
            {
                *set0 << count;
                categories.append(QString::fromStdString(date));
                index ++;
            }
            else
            {
                *set0 << 0;
                categories.append(QString::fromStdString(date));
            }
        }

    }
    else
    {
        for (const auto& elem : history) {
            //get from tuple
            std::string date;
            int count;
            std::tie(date, count) = elem;

            //get date as timepoint
            std::tm tm = {};
            std::stringstream ss(date);
            ss >> std::get_time(&tm, "%d/%m/%Y");
            auto datePoint = std::chrono::system_clock::from_time_t(std::mktime(&tm));

            if(datePoint > limit_date)
            {
                *set0 << count;
                categories.append(QString::fromStdString(date));
            }
        }
    }


    QBarSeries *series = new QBarSeries();
    series->append(set0);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Historique");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    if (categories.length() < 50)
    {
        auto axisX = new QBarCategoryAxis;
        axisX->append(categories);
        chart->addAxis(axisX, Qt::AlignBottom);
        axisX->setLabelsAngle(60);
        series->attachAxis(axisX);
    }

    QValueAxis *axisY = new QValueAxis();
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
    axisY->setLabelFormat("%.0f");
    chart->legend()->setVisible(false);

    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    this->layout()->addWidget(chartView);
}

void History::on_reloadButton_clicked()
{
    WriteHist();
    LoadChart();
    LoadChart();
}
