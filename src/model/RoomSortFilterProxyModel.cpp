#include "RoomSortFilterProxyModel.h"

#include <QAbstractItemModel>
#include <QString>

RoomSortFilterProxyModel::RoomSortFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    setDynamicSortFilter(true);
    setSortCaseSensitivity(Qt::CaseInsensitive);
    setFilterCaseSensitivity(Qt::CaseInsensitive);
}

void RoomSortFilterProxyModel::setSearchColumn(int column)
{
    if (m_searchColumn == column) {
        return;
    }

    m_searchColumn = column;
    invalidateFilter();
}

int RoomSortFilterProxyModel::searchColumn() const
{
    return m_searchColumn;
}

void RoomSortFilterProxyModel::setSearchText(const QString &text)
{
    const QString normalizedText = text.trimmed();

    if (m_searchText == normalizedText) {
        return;
    }

    m_searchText = normalizedText;
    invalidateFilter();
}

QString RoomSortFilterProxyModel::searchText() const
{
    return m_searchText;
}

void RoomSortFilterProxyModel::clearFilter()
{
    m_searchText.clear();
    m_searchColumn = -1;
    invalidateFilter();
}

bool RoomSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    if (m_searchText.isEmpty()) {
        return true;
    }

    const QAbstractItemModel *model = sourceModel();

    if (model == nullptr) {
        return false;
    }

    if (m_searchColumn >= 0) {
        const QModelIndex index = model->index(sourceRow, m_searchColumn, sourceParent);
        const QString value = model->data(index, Qt::DisplayRole).toString();

        return value.contains(m_searchText, filterCaseSensitivity());
    }

    for (int column = 0; column < model->columnCount(sourceParent); ++column) {
        const QModelIndex index = model->index(sourceRow, column, sourceParent);
        const QString value = model->data(index, Qt::DisplayRole).toString();

        if (value.contains(m_searchText, filterCaseSensitivity())) {
            return true;
        }
    }

    return false;
}

bool RoomSortFilterProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    const QVariant leftValue = sourceModel()->data(left, Qt::EditRole);
    const QVariant rightValue = sourceModel()->data(right, Qt::EditRole);

    bool leftIsNumber = false;
    bool rightIsNumber = false;

    const double leftNumber = leftValue.toDouble(&leftIsNumber);
    const double rightNumber = rightValue.toDouble(&rightIsNumber);

    if (leftIsNumber && rightIsNumber) {
        return leftNumber < rightNumber;
    }

    const QString leftText = leftValue.toString();
    const QString rightText = rightValue.toString();

    return QString::compare(leftText, rightText, sortCaseSensitivity()) < 0;
}
