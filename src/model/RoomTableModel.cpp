#include "RoomTableModel.h"

#include <QMimeData>
#include <QLocale>
#include <QRegularExpression>
#include <QSet>
#include <QStringList>

namespace {
constexpr auto CsvMimeType = "text/csv";

QString escapeCsvField(QString value)
{
    const bool mustQuote = value.contains(';') || value.contains('"') || value.contains('\n') || value.contains('\r');
    value.replace('"', "\"\"");
    return mustQuote ? QStringLiteral("\"%1\"").arg(value) : value;
}

QStringList splitCsvLine(const QString &line)
{
    QStringList fields;
    QString current;
    bool quoted = false;

    for (int i = 0; i < line.size(); ++i) {
        const QChar character = line.at(i);
        if (character == '"') {
            if (quoted && i + 1 < line.size() && line.at(i + 1) == '"') {
                current.append('"');
                ++i;
            } else {
                quoted = !quoted;
            }
        } else if (character == ';' && !quoted) {
            fields.append(current);
            current.clear();
        } else {
            current.append(character);
        }
    }

    fields.append(current);
    return fields;
}
}

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
        return Qt::ItemIsDropEnabled;
    }

    return Qt::ItemIsSelectable
           | Qt::ItemIsEnabled
           | Qt::ItemIsEditable
           | Qt::ItemIsDragEnabled
           | Qt::ItemIsDropEnabled;
}

QStringList RoomTableModel::mimeTypes() const
{
    return {QStringLiteral(CsvMimeType), QStringLiteral("text/plain")};
}

QMimeData *RoomTableModel::mimeData(const QModelIndexList &indexes) const
{
    auto *mimeData = new QMimeData;
    QSet<int> uniqueRows;
    for (const QModelIndex &index : indexes) {
        if (index.isValid()) {
            uniqueRows.insert(index.row());
        }
    }

    QList<int> rows = uniqueRows.values();
    std::sort(rows.begin(), rows.end());

    QStringList lines;
    for (const int row : rows) {
        if (isValidRow(row)) {
            lines.append(roomToCsvLine(m_rooms.at(row)));
        }
    }

    const QString text = lines.join('\n');
    mimeData->setData(QStringLiteral(CsvMimeType), text.toUtf8());
    mimeData->setText(text);
    return mimeData;
}

bool RoomTableModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    Q_UNUSED(column)

    if (data == nullptr || action == Qt::IgnoreAction) {
        return true;
    }

    const QString text = data->hasFormat(QStringLiteral(CsvMimeType))
                         ? QString::fromUtf8(data->data(QStringLiteral(CsvMimeType)))
                         : data->text();
    const QVector<Room> roomsToInsert = roomsFromCsv(text);
    if (roomsToInsert.isEmpty()) {
        return false;
    }

    int insertRow = row;
    if (insertRow < 0 && parent.isValid()) {
        insertRow = parent.row();
    }
    if (insertRow < 0 || insertRow > m_rooms.size()) {
        insertRow = m_rooms.size();
    }

    beginInsertRows(QModelIndex(), insertRow, insertRow + roomsToInsert.size() - 1);
    for (int offset = 0; offset < roomsToInsert.size(); ++offset) {
        m_rooms.insert(insertRow + offset, roomsToInsert.at(offset));
    }
    endInsertRows();

    return true;
}

Qt::DropActions RoomTableModel::supportedDragActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}

Qt::DropActions RoomTableModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
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

QString RoomTableModel::roomToCsvLine(const Room &room) const
{
    return QStringList{
        escapeCsvField(room.id),
        escapeCsvField(room.building),
        QString::number(room.floor),
        escapeCsvField(room.number),
        escapeCsvField(room.type),
        QString::number(room.area, 'f', 2),
        escapeCsvField(room.department),
        QString::number(room.workplaces),
        escapeCsvField(room.status),
        escapeCsvField(room.responsible)
    }.join(';');
}

QVector<Room> RoomTableModel::roomsFromCsv(const QString &text) const
{
    QVector<Room> result;
    const QStringList lines = text.split(QRegularExpression(QStringLiteral("[\\r\\n]+")), Qt::SkipEmptyParts);

    for (const QString &line : lines) {
        const QStringList fields = splitCsvLine(line);
        if (fields.size() != ColumnCount) {
            continue;
        }

        Room room;
        room.id = fields.at(IdColumn).trimmed();
        room.building = fields.at(BuildingColumn).trimmed();
        room.floor = fields.at(FloorColumn).toInt();
        room.number = fields.at(NumberColumn).trimmed();
        room.type = fields.at(TypeColumn).trimmed();
        room.area = fields.at(AreaColumn).toDouble();
        room.department = fields.at(DepartmentColumn).trimmed();
        room.workplaces = fields.at(WorkplacesColumn).toInt();
        room.status = fields.at(StatusColumn).trimmed();
        room.responsible = fields.at(ResponsibleColumn).trimmed();
        result.append(room);
    }

    return result;
}
