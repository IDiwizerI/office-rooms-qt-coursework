#include "ChartDialog.h"
#include "ui_ChartDialog.h"

#include <QBarCategoryAxis>
#include <QBarSeries>
#include <QBarSet>
#include <QChart>
#include <QChartView>
#include <QMap>
#include <QValueAxis>
#include <QVBoxLayout>

namespace {
constexpr int TypeChartIndex = 0;
constexpr int DepartmentAreaChartIndex = 1;

QString normalizedCategory(const QString &value, const QString &fallback)
{
    const QString trimmedValue = value.trimmed();
    return trimmedValue.isEmpty() ? fallback : trimmedValue;
}
}

ChartDialog::ChartDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ChartDialog)
    , m_chartView(new QChartView(this))
{
    ui->setupUi(this);
    setWindowTitle(tr("Chart"));

    ui->chartModeComboBox->addItem(tr("Room count by type"), TypeChartIndex);
    ui->chartModeComboBox->addItem(tr("Area by department"), DepartmentAreaChartIndex);

    ui->chartLayout->addWidget(m_chartView);
    m_chartView->setRenderHint(QPainter::Antialiasing);

    connect(ui->chartModeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ChartDialog::rebuildChart);
}

ChartDialog::~ChartDialog()
{
    delete ui;
}

void ChartDialog::setRooms(const QVector<Room> &rooms)
{
    m_rooms = rooms;
    rebuildChart();
}

void ChartDialog::rebuildChart()
{
    const int mode = ui->chartModeComboBox->currentData().toInt();

    if (mode == DepartmentAreaChartIndex) {
        buildDepartmentAreaChart();
        return;
    }

    buildTypeChart();
}

void ChartDialog::buildTypeChart()
{
    QMap<QString, int> countByType;
    for (const Room &room : m_rooms) {
        countByType[normalizedCategory(room.roomType, tr("Unknown"))] += 1;
    }

    auto *set = new QBarSet(tr("Rooms"));
    QStringList categories;
    int maxValue = 0;

    for (auto iterator = countByType.cbegin(); iterator != countByType.cend(); ++iterator) {
        categories.append(iterator.key());
        *set << iterator.value();
        maxValue = qMax(maxValue, iterator.value());
    }

    auto *series = new QBarSeries;
    series->append(set);

    auto *chart = new QChart;
    chart->addSeries(series);
    chart->setTitle(tr("Room count by type"));
    chart->setAnimationOptions(QChart::SeriesAnimations);

    auto *axisX = new QBarCategoryAxis;
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    auto *axisY = new QValueAxis;
    axisY->setRange(0, qMax(1, maxValue));
    axisY->setLabelFormat("%d");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    m_chartView->setChart(chart);
}

void ChartDialog::buildDepartmentAreaChart()
{
    QMap<QString, double> areaByDepartment;
    for (const Room &room : m_rooms) {
        areaByDepartment[normalizedCategory(room.department, tr("Unknown"))] += room.area;
    }

    auto *set = new QBarSet(tr("Area"));
    QStringList categories;
    double maxValue = 0.0;

    for (auto iterator = areaByDepartment.cbegin(); iterator != areaByDepartment.cend(); ++iterator) {
        categories.append(iterator.key());
        *set << iterator.value();
        maxValue = qMax(maxValue, iterator.value());
    }

    auto *series = new QBarSeries;
    series->append(set);

    auto *chart = new QChart;
    chart->addSeries(series);
    chart->setTitle(tr("Area by department"));
    chart->setAnimationOptions(QChart::SeriesAnimations);

    auto *axisX = new QBarCategoryAxis;
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    auto *axisY = new QValueAxis;
    axisY->setRange(0.0, qMax(1.0, maxValue));
    axisY->setLabelFormat("%.2f");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    m_chartView->setChart(chart);
}
