#include "SettingsManager.h"

#include <QMainWindow>
#include <QSettings>
#include <QTableView>

namespace {
constexpr int MaxRecentFiles = 10;

QSettings createSettings()
{
    return QSettings(QStringLiteral("NIU MGSU"), QStringLiteral("OfficeRoomsCoursework"));
}
}

SettingsManager::SettingsManager() = default;

void SettingsManager::saveMainWindow(const QMainWindow *window) const
{
    if (window == nullptr) {
        return;
    }

    QSettings settings = createSettings();
    settings.setValue(QStringLiteral("mainWindow/geometry"), window->saveGeometry());
    settings.setValue(QStringLiteral("mainWindow/state"), window->saveState());
}

void SettingsManager::restoreMainWindow(QMainWindow *window) const
{
    if (window == nullptr) {
        return;
    }

    QSettings settings = createSettings();
    window->restoreGeometry(settings.value(QStringLiteral("mainWindow/geometry")).toByteArray());
    window->restoreState(settings.value(QStringLiteral("mainWindow/state")).toByteArray());
}

void SettingsManager::setLanguage(const QString &language) const
{
    QSettings settings = createSettings();
    settings.setValue(QStringLiteral("language/current"), language);
}

QString SettingsManager::language() const
{
    QSettings settings = createSettings();
    return settings.value(QStringLiteral("language/current"), QStringLiteral("en")).toString();
}

void SettingsManager::addRecentFile(const QString &filePath) const
{
    if (filePath.trimmed().isEmpty()) {
        return;
    }

    QSettings settings = createSettings();
    QStringList files = settings.value(QStringLiteral("files/recent")).toStringList();
    files.removeAll(filePath);
    files.prepend(filePath);

    while (files.size() > MaxRecentFiles) {
        files.removeLast();
    }

    settings.setValue(QStringLiteral("files/recent"), files);
}

QStringList SettingsManager::recentFiles() const
{
    QSettings settings = createSettings();
    return settings.value(QStringLiteral("files/recent")).toStringList();
}

void SettingsManager::saveColumnWidths(const QTableView *tableView, const QString &key) const
{
    if (tableView == nullptr || tableView->model() == nullptr || key.trimmed().isEmpty()) {
        return;
    }

    QVariantList widths;
    for (int column = 0; column < tableView->model()->columnCount(); ++column) {
        widths.append(tableView->columnWidth(column));
    }

    QSettings settings = createSettings();
    settings.setValue(QStringLiteral("tables/%1/columnWidths").arg(key), widths);
}

void SettingsManager::restoreColumnWidths(QTableView *tableView, const QString &key) const
{
    if (tableView == nullptr || tableView->model() == nullptr || key.trimmed().isEmpty()) {
        return;
    }

    QSettings settings = createSettings();
    const QVariantList widths = settings.value(QStringLiteral("tables/%1/columnWidths").arg(key)).toList();

    const int count = qMin(widths.size(), tableView->model()->columnCount());
    for (int column = 0; column < count; ++column) {
        const int width = widths.at(column).toInt();
        if (width > 0) {
            tableView->setColumnWidth(column, width);
        }
    }
}
