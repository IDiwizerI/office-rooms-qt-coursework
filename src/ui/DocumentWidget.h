#pragma once

#include <QModelIndex>
#include <QPoint>
#include <QString>
#include <QWidget>

class QEvent;
class RoomTableModel;
class RoomSortFilterProxyModel;
class QTableView;

namespace Ui {
class DocumentWidget;
}

/**
 * @brief Widget for one opened room database file.
 *
 * The widget owns its own RoomTableModel and RoomSortFilterProxyModel,
 * displays data in QTableView, supports table editing actions, context menu,
 * sorting, filtering and drag-and-drop.
 */
class DocumentWidget final : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Creates a document widget.
     * @param parent Parent widget.
     */
    explicit DocumentWidget(QWidget *parent = nullptr);

    /**
     * @brief Destroys the document widget and saves table column widths.
     */
    ~DocumentWidget() override;

    /**
     * @brief Returns the current file path.
     * @return Current file path or empty string for a new unsaved document.
     */
    QString filePath() const;

    /**
     * @brief Returns a title suitable for a tab caption.
     * @return File name or Untitled with an optional modified marker.
     */
    QString displayName() const;

    /**
     * @brief Checks whether the document has unsaved changes.
     * @return True if document data was changed after the last save or load.
     */
    bool isModified() const;

    /**
     * @brief Returns the table view used by this document.
     * @return Pointer to the document table view.
     */
    QTableView *tableView() const;

    /**
     * @brief Loads room data from a text file.
     * @param filePath Source database file path.
     * @param errorMessage Optional output parameter for an error message.
     * @return True if the file was loaded successfully.
     */
    bool loadFromFile(const QString &filePath, QString *errorMessage = nullptr);

    /**
     * @brief Saves room data to the current file path.
     * @param errorMessage Optional output parameter for an error message.
     * @return True if the document was saved successfully.
     */
    bool save(QString *errorMessage = nullptr);

    /**
     * @brief Saves room data to a selected file path.
     * @param filePath Target database file path.
     * @param errorMessage Optional output parameter for an error message.
     * @return True if the document was saved successfully.
     */
    bool saveAs(const QString &filePath, QString *errorMessage = nullptr);

    /**
     * @brief Saves table column widths to QSettings.
     */
    void saveColumnWidths() const;

    /**
     * @brief Restores table column widths from QSettings.
     */
    void restoreColumnWidths();

public slots:
    /** @brief Adds a new room record. */
    void addRoom();

    /** @brief Edits the selected room record. */
    void editSelectedRoom();

    /** @brief Deletes selected room records. */
    void deleteSelectedRooms();

    /** @brief Opens print preview for the document table. */
    void printDocument();

    /** @brief Shows chart dialog for this document. */
    void showChart();

    /** @brief Sets focus to the search input field. */
    void focusSearch();

    /** @brief Clears current search filter. */
    void clearSearch();

signals:
    /**
     * @brief Emitted when the file path changes.
     * @param filePath New document file path.
     */
    void filePathChanged(const QString &filePath);

    /**
     * @brief Emitted when the modified state changes.
     * @param modified New modified state.
     */
    void modifiedChanged(bool modified);

    /** @brief Emitted when tab title must be updated. */
    void titleChanged();

protected:
    /**
     * @brief Handles language change events.
     * @param event Qt event.
     */
    void changeEvent(QEvent *event) override;

private slots:
    /**
     * @brief Updates proxy search column from the combo box.
     * @param index Combo box item index.
     */
    void updateSearchColumn(int index);

    /**
     * @brief Updates proxy search text from the line edit.
     * @param text Search text.
     */
    void updateSearchText(const QString &text);

    /**
     * @brief Shows the context menu for the table view.
     * @param position Context menu position in table viewport coordinates.
     */
    void showContextMenu(const QPoint &position);

    /**
     * @brief Edits a row after double click.
     * @param index Clicked proxy model index.
     */
    void handleDoubleClick(const QModelIndex &index);

    /** @brief Marks the document as modified after model changes. */
    void markModified();

private:
    /** @brief Configures table view and model connections. */
    void setupTable();

    /** @brief Fills the search column combo box. */
    void setupSearchColumns();

    /** @brief Retranslates UI and search column captions after language changes. */
    void retranslateDocumentUi();

    /**
     * @brief Updates file path and emits related signals.
     * @param filePath New document file path.
     */
    void setFilePath(const QString &filePath);

    /**
     * @brief Updates modified state and emits related signals.
     * @param modified New modified state.
     */
    void setModified(bool modified);

    /**
     * @brief Returns the selected source model row.
     * @return Source model row or -1 if current selection is invalid.
     */
    int currentSourceRow() const;

    Ui::DocumentWidget *ui;
    RoomTableModel *m_model;
    RoomSortFilterProxyModel *m_proxyModel;
    QString m_filePath;
    bool m_modified = false;
    bool m_loading = false;
};
