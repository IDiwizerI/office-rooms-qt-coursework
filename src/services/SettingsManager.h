#pragma once

#include <QString>
#include <QStringList>

class QMainWindow;
class QTableView;

/**
 * @brief Provides persistent application settings through QSettings.
 *
 * Stores main window geometry, selected language, recent files and table column widths.
 */
class SettingsManager final
{
public:
    /**
     * @brief Creates a settings manager instance.
     */
    SettingsManager();

    /**
     * @brief Saves main window size and position.
     * @param window Main window to save.
     */
    void saveMainWindow(const QMainWindow *window) const;

    /**
     * @brief Restores main window size and position.
     * @param window Main window to restore.
     */
    void restoreMainWindow(QMainWindow *window) const;

    /**
     * @brief Saves selected interface language.
     * @param language Language code: en, ru or de.
     */
    void setLanguage(const QString &language) const;

    /**
     * @brief Returns selected interface language.
     * @return Language code.
     */
    QString language() const;

    /**
     * @brief Adds a file path to recent files.
     * @param filePath File path to add.
     */
    void addRecentFile(const QString &filePath) const;

    /**
     * @brief Returns recent file paths.
     * @return Recent file list.
     */
    QStringList recentFiles() const;

    /**
     * @brief Saves table column widths.
     * @param tableView Table view to read widths from.
     * @param key Settings key for this table.
     */
    void saveColumnWidths(const QTableView *tableView, const QString &key) const;

    /**
     * @brief Restores table column widths.
     * @param tableView Table view to update.
     * @param key Settings key for this table.
     */
    void restoreColumnWidths(QTableView *tableView, const QString &key) const;
};
