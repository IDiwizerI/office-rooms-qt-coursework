#pragma once

#include <QString>

/**
 * @brief Describes one office room record.
 */
struct Room
{
    QString id;
    QString building;
    int floor = 0;
    QString number;
    QString type;
    double area = 0.0;
    QString department;
    int workplaces = 0;
    QString status;
    QString responsible;
};
