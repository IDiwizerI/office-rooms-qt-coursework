#include "ChartDialog.h"
#include "ui_ChartDialog.h"

#include <QBarCategoryAxis>
#include <QBarSeries>
#include <QBarSet>
#include <QChart>
#include <QChartView>
#include <QLegend>
#include <QMap>
#include <QPieSeries>
#include <QPieSlice>
#include <QValueAxis>
#include <QVBoxLayout>

namespace {
constexpr int TypeChartIndex = 0;
constexpr int DepartmentAreaChartIndex = 1;
constexpr int MaximumBarCategories = 10;

QString normalizedCategory(const QString &value, const QString &fallback)
{
    const QString trimmedValue = value.trimmed();
    return trimmedValue.isEmpty() ? fallback : trimmedValue;
}

QVector<QPair<QString, double>> sortedLimitedValues(const QMap<QString, double> &values, const QString &otherTitle)
{
    QVector<QPair<QString, double>> items;
    items.reserve(values.size());

    for (auto iterator = values.cbegin(); iterator != values.cend(); ++iterator) {
        items.append(qMakePair(iterator.key(), iterator.value()));
    }

    std::sort(items.begin(), items.end(), [](const auto &left, const auto &right) {
        if (qFuzzyCompare(left.second, right.second)) {
            return left.first < right.first;
        }
        return left.second > right.second;
    });

    if (items.size() <= MaximumBarCategories) {
        return items;
    }

    QVector<QPair<QString, double>> limited;
    double otherValue = 0.0;

    for (int index = 0; index < items.size(); ++index) {
        if (index < MaximumBarCategories - 1) {
            limited.append(items.at(index));
        } else {
            otherValue += items.at(index).second;
        }
    }

    limited.append(qMakePair(otherTitle, otherValue));
    return limited;
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
    QMap<QString, double> countByType;
    for (const Room &room : m_rooms) {
        countByType[normalizedCategory(room.roomType, tr("Unknown"))] += 1.0;
    }

    auto *series = new QPieSeries;
    const QVector<QPair<QString, double>> values = sortedLimitedValues(countByType, tr("Other"));

    for (const auto &item : values) {
        QPieSlice *slice = series->append(QStringLiteral("%1 (%2)").arg(item.first).arg(static_cast<int>(item.second)), item.second);
        slice->setLabelVisible(true);
        slice->setLabel(QStringLiteral("%1: %2").arg(item.first).arg(static_cast<int>(item.second)));
    }

    auto *chart = new QChart;
    chart->addSeries(series);
    chart->setTitle(tr("Room count by type"));
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->legend()->setAlignment(Qt::AlignRight);

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
    const QVector<QPair<QString, double>> values = sortedLimitedValues(areaByDepartment, tr("Other"));

    for (const auto &item : values) {
        categories.append(item.first);
        *set << item.second;
        maxValue = qMax(maxValue, item.second);
    }

    auto *series = new QBarSeries;
    series->append(set);

    auto *chart = new QChart;
    chart->addSeries(series);
    chart->setTitle(tr("Area by department"));
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->legend()->setAlignment(Qt::AlignBottom);

    auto *axisX = new QBarCategoryAxis;
    axisX->append(categories);
    axisX->setLabelsAngle(-45);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    auto *axisY = new QValueAxis;
    axisY->setRange(0.0, qMax(1.0, maxValue * 1.10));
    axisY->setLabelFormat("%.0f");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    m_chartView->setChart(chart);
}
