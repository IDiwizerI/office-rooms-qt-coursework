#pragma once

#include <QSortFilterProxyModel>

/**
 * @brief Proxy model for sorting and filtering room table data.
 *
 * Supports filtering by selected column and sorting through QTableView headers.
 */
class RoomSortFilterProxyModel final : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    /**
     * @brief Creates a room sort/filter proxy model.
     * @param parent Parent QObject.
     */
    explicit RoomSortFilterProxyModel(QObject *parent = nullptr);

    /**
     * @brief Sets column used for searching.
     * @param column Source model column index. Use -1 to search in all columns.
     */
    void setSearchColumn(int column);

    /**
     * @brief Returns current search column.
     * @return Source model column index.
     */
    int searchColumn() const;

    /**
     * @brief Sets search text.
     * @param text Text used for filtering.
     */
    void setSearchText(const QString &text);

    /**
     * @brief Returns current search text.
     * @return Search text.
     */
    QString searchText() const;

    /**
     * @brief Clears current filter.
     */
    void clearFilter();

protected:
    /**
     * @brief Checks whether a source row should be shown.
     * @param sourceRow Source row.
     * @param sourceParent Source parent index.
     * @return True if row matches filter.
     */
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

    /**
     * @brief Compares two source indexes for sorting.
     * @param left Left source index.
     * @param right Right source index.
     * @return True if left value is less than right value.
     */
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;

private:
    int m_searchColumn = -1;
    QString m_searchText;
};
