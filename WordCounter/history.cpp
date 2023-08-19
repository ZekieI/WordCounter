#include "history.h"
#include "ui_history.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <QString>

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

    //std::cout << stringStream.str().c_str() << "\n";
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
    QString text_hist = "";
    while (!in.atEnd()) {
        QStringList line = in.readLine().split(u',');

        text_hist.append(line[0] + ": " + line[1] + " mots\n");

        std::string date = line[0].toStdString();
        int count = line[1].toInt();
        history.append(std::make_tuple(date, count));
    }
    file.close();

    this->ui->label->setText(text_hist);

    const char* last_histo = std::get<0>(history.back()).c_str();
    const char* today = getDate().c_str();
    if(std::strcmp(last_histo, today) == 0)
    {
        counter->count = std::get<1>(history.back());
        counter->refresh();
    }

}

void History::on_pushButton_clicked()
{
    WriteHist();
    LoadHist();
}

