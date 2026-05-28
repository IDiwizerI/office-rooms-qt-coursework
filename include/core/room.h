#pragma once

#include <QDate>
#include <QString>

/**
 * @brief Describes one office room record stored in the text database.
 */
struct Room
{
    QString id;
    QString building;
    int floor = 0;
    QString roomNumber;
    QString roomType;
    double area = 0.0;
    int capacity = 0;
    QString department;
    QString responsiblePerson;
    QString condition;
    QDate lastRenovationDate = QDate::currentDate();
    QString notes;
};
