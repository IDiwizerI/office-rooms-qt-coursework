#include "RoomTableModel.h"

#include <QLocale>

RoomTableModel::RoomTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

int RoomTableModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return m_rooms.size();
}

int RoomTableModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return ColumnCount;
}

QVariant RoomTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || !isValidRow(index.row())) {
        return {};
    }

    const Room &room = m_rooms.at(index.row());

    if (role == Qt::TextAlignmentRole) {
        switch (index.column()) {
        case FloorColumn:
        case AreaColumn:
        case WorkplacesColumn:
            return Qt::AlignRight | Qt::AlignVCenter;
        default:
            return Qt::AlignLeft | Qt::AlignVCenter;
        }
    }

    if (role != Qt::DisplayRole && role != Qt::EditRole) {
        return {};
    }

    switch (index.column()) {
    case IdColumn:
        return room.id;

    case BuildingColumn:
        return room.building;

    case FloorColumn:
        return room.floor;

    case NumberColumn:
        return room.number;

    case TypeColumn:
        return room.type;

    case AreaColumn:
        if (role == Qt::DisplayRole) {
            return QLocale().toString(room.area, 'f', 2);
        }
        return room.area;

    case DepartmentColumn:
        return room.department;

    case WorkplacesColumn:
        return room.workplaces;

    case StatusColumn:
        return room.status;

    case ResponsibleColumn:
        return room.responsible;

    default:
        return {};
    }
}

bool RoomTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || !isValidRow(index.row()) || role != Qt::EditRole) {
        return false;
    }

    Room &room = m_rooms[index.row()];

    switch (index.column()) {
    case IdColumn:
        room.id = value.toString().trimmed();
        break;

    case BuildingColumn:
        room.building = value.toString().trimmed();
        break;

    case FloorColumn: {
        bool ok = false;
        const int floor = value.toInt(&ok);
        if (!ok || floor < 0) {
            return false;
        }
        room.floor = floor;
        break;
    }

    case NumberColumn:
        room.number = value.toString().trimmed();
        break;

    case TypeColumn:
        room.type = value.toString().trimmed();
        break;

    case AreaColumn: {
        bool ok = false;
        const double area = value.toDouble(&ok);
        if (!ok || area < 0.0) {
            return false;
        }
        room.area = area;
        break;
    }

    case DepartmentColumn:
        room.department = value.toString().trimmed();
        break;

    case WorkplacesColumn: {
        bool ok = false;
        const int workplaces = value.toInt(&ok);
        if (!ok || workplaces < 0) {
            return false;
        }
        room.workplaces = workplaces;
        break;
    }

    case StatusColumn:
        room.status = value.toString().trimmed();
        break;

    case ResponsibleColumn:
        room.responsible = value.toString().trimmed();
        break;

    default:
        return false;
    }

    emit dataChanged(index, index, {Qt::DisplayRole, Qt::EditRole});
    return true;
}

QVariant RoomTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return {};
    }

    if (orientation == Qt::Vertical) {
        return section + 1;
    }

    switch (section) {
    case IdColumn:
        return tr("ID");

    case BuildingColumn:
        return tr("Building");

    case FloorColumn:
        return tr("Floor");

    case NumberColumn:
        return tr("Room number");

    case TypeColumn:
        return tr("Type");

    case AreaColumn:
        return tr("Area");

    case DepartmentColumn:
        return tr("Department");

    case WorkplacesColumn:
        return tr("Workplaces");

    case StatusColumn:
        return tr("Status");

    case ResponsibleColumn:
        return tr("Responsible");

    default:
        return {};
    }
}

Qt::ItemFlags RoomTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }

    return Qt::ItemIsSelectable
           | Qt::ItemIsEnabled
           | Qt::ItemIsEditable
           | Qt::ItemIsDragEnabled;
}

void RoomTableModel::setRooms(const QVector<Room> &rooms)
{
    beginResetModel();
    m_rooms = rooms;
    endResetModel();
}

const QVector<Room> &RoomTableModel::rooms() const
{
    return m_rooms;
}

Room RoomTableModel::roomAt(int row) const
{
    if (!isValidRow(row)) {
        return {};
    }

    return m_rooms.at(row);
}

void RoomTableModel::addRoom(const Room &room)
{
    const int newRow = m_rooms.size();

    beginInsertRows(QModelIndex(), newRow, newRow);
    m_rooms.append(room);
    endInsertRows();
}

bool RoomTableModel::updateRoom(int row, const Room &room)
{
    if (!isValidRow(row)) {
        return false;
    }

    m_rooms[row] = room;

    const QModelIndex left = index(row, 0);
    const QModelIndex right = index(row, ColumnCount - 1);

    emit dataChanged(left, right, {Qt::DisplayRole, Qt::EditRole});

    return true;
}

bool RoomTableModel::removeRoom(int row)
{
    if (!isValidRow(row)) {
        return false;
    }

    beginRemoveRows(QModelIndex(), row, row);
    m_rooms.removeAt(row);
    endRemoveRows();

    return true;
}

bool RoomTableModel::isValidRow(int row) const
{
    return row >= 0 && row < m_rooms.size();
}
