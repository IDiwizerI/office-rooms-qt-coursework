#pragma once

#include "core/room.h"

#include <QDialog>
#include <QPainter>
#include <QVector>

class QChartView;

namespace Ui {
class ChartDialog;
}

/**
 * @brief Dialog that builds charts based on room records.
 *
 * Supports two chart modes: room count by type and total area by department.
 */
class ChartDialog final : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief Creates a chart dialog.
     * @param parent Parent widget.
     */
    explicit ChartDialog(QWidget *parent = nullptr);

    /**
     * @brief Destroys the chart dialog.
     */
    ~ChartDialog() override;

    /**
     * @brief Sets room data used by the chart.
     * @param rooms Room records.
     */
    void setRooms(const QVector<Room> &rooms);

private slots:
    /**
     * @brief Rebuilds chart after mode change.
     */
    void rebuildChart();

private:
    /**
     * @brief Builds room count by room type chart.
     */
    void buildTypeChart();

    /**
     * @brief Builds total area by department chart.
     */
    void buildDepartmentAreaChart();

    Ui::ChartDialog *ui;
    QChartView *m_chartView;
    QVector<Room> m_rooms;
};
