#pragma once

#include <QModelIndex>
#include <QPoint>
#include <QString>
#include <QWidget>

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
 * sorting and filtering by a selected column.
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
     * @brief Destroys the document widget.
     */
    ~DocumentWidget() override;

    /**
     * @brief Returns the current file path.
     * @return File path or an empty string for a new document.
     */
    QString filePath() const;

    /**
     * @brief Returns a title suitable for a tab caption.
     * @return Display name with modified marker.
     */
    QString displayName() const;

    /**
     * @brief Checks whether the document has unsaved changes.
     * @return True if document data was changed.
     */
    bool isModified() const;

    /**
     * @brief Returns the table view used by this document.
     * @return Table view pointer.
     */
    QTableView *tableView() const;

    /**
     * @brief Loads room data from a text file.
     * @param filePath Path to the room database file.
     * @param errorMessage Optional error message output.
     * @return True if the file was loaded.
     */
    bool loadFromFile(const QString &filePath, QString *errorMessage = nullptr);

    /**
     * @brief Saves room data to the current file path.
     * @param errorMessage Optional error message output.
     * @return True if the document was saved.
     */
    bool save(QString *errorMessage = nullptr);

    /**
     * @brief Saves room data to a selected file path.
     * @param filePath Target file path.
     * @param errorMessage Optional error message output.
     * @return True if the document was saved.
     */
    bool saveAs(const QString &filePath, QString *errorMessage = nullptr);

public slots:
    /** @brief Adds a new room record. */
    void addRoom();

    /** @brief Edits the selected room record. */
    void editSelectedRoom();

    /** @brief Deletes selected room records. */
    void deleteSelectedRooms();

    /** @brief Opens print dialog for the document table. */
    void printDocument();

    /** @brief Shows chart dialog for this document. */
    void showChart();

    /** @brief Sets focus to the search input field. */
    void focusSearch();

    /** @brief Clears current search filter. */
    void clearSearch();

signals:
    /** @brief Emitted when the file path changes. */
    void filePathChanged(const QString &filePath);

    /** @brief Emitted when the modified state changes. */
    void modifiedChanged(bool modified);

    /** @brief Emitted when tab title must be updated. */
    void titleChanged();

private slots:
    /** @brief Updates proxy search column from the combo box. */
    void updateSearchColumn(int index);

    /** @brief Updates proxy search text from the line edit. */
    void updateSearchText(const QString &text);

    /** @brief Shows the context menu for the table view. */
    void showContextMenu(const QPoint &position);

    /** @brief Edits a row after double click. */
    void handleDoubleClick(const QModelIndex &index);

    /** @brief Marks the document as modified after model changes. */
    void markModified();

private:
    /** @brief Configures table view and model connections. */
    void setupTable();

    /** @brief Fills the search column combo box. */
    void setupSearchColumns();

    /** @brief Updates file path and emits related signals. */
    void setFilePath(const QString &filePath);

    /** @brief Updates modified state and emits related signals. */
    void setModified(bool modified);

    /** @brief Returns the selected source model row. */
    int currentSourceRow() const;

    Ui::DocumentWidget *ui;
    RoomTableModel *m_model;
    RoomSortFilterProxyModel *m_proxyModel;
    QString m_filePath;
    bool m_modified = false;
    bool m_loading = false;
};
