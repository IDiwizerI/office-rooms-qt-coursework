#include "RoomTableModel.h"

#include <QDate>
#include <QMimeData>
#include <QLocale>
#include <QRegularExpression>
#include <QSet>
#include <QStringList>
#include <QVariant>

namespace {
const QString CsvMimeType = QStringLiteral("text/csv");
const QString DateFormat = QStringLiteral("yyyy-MM-dd");

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

QDate dateFromString(const QString &value)
{
    const QDate date = QDate::fromString(value.trimmed(), DateFormat);
    return date.isValid() ? date : QDate::currentDate();
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
        case CapacityColumn:
            return QVariant::fromValue(Qt::Alignment(Qt::AlignRight | Qt::AlignVCenter));
        default:
            return QVariant::fromValue(Qt::Alignment(Qt::AlignLeft | Qt::AlignVCenter));
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
    case RoomNumberColumn:
        return room.roomNumber;
    case RoomTypeColumn:
        return room.roomType;
    case AreaColumn:
        if (role == Qt::DisplayRole) {
            return QLocale().toString(room.area, 'f', 2);
        }
        return room.area;
    case CapacityColumn:
        return room.capacity;
    case DepartmentColumn:
        return room.department;
    case ResponsiblePersonColumn:
        return room.responsiblePerson;
    case ConditionColumn:
        return room.condition;
    case LastRenovationDateColumn:
        return room.lastRenovationDate.toString(DateFormat);
    case NotesColumn:
        return room.notes;
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
    case RoomNumberColumn:
        room.roomNumber = value.toString().trimmed();
        break;
    case RoomTypeColumn:
        room.roomType = value.toString().trimmed();
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
    case CapacityColumn: {
        bool ok = false;
        const int capacity = value.toInt(&ok);
        if (!ok || capacity < 0) {
            return false;
        }
        room.capacity = capacity;
        break;
    }
    case DepartmentColumn:
        room.department = value.toString().trimmed();
        break;
    case ResponsiblePersonColumn:
        room.responsiblePerson = value.toString().trimmed();
        break;
    case ConditionColumn:
        room.condition = value.toString().trimmed();
        break;
    case LastRenovationDateColumn:
        room.lastRenovationDate = dateFromString(value.toString());
        break;
    case NotesColumn:
        room.notes = value.toString().trimmed();
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
    case RoomNumberColumn:
        return tr("Room number");
    case RoomTypeColumn:
        return tr("Room type");
    case AreaColumn:
        return tr("Area");
    case CapacityColumn:
        return tr("Capacity");
    case DepartmentColumn:
        return tr("Department");
    case ResponsiblePersonColumn:
        return tr("Responsible person");
    case ConditionColumn:
        return tr("Condition");
    case LastRenovationDateColumn:
        return tr("Last renovation date");
    case NotesColumn:
        return tr("Notes");
    default:
        return {};
    }
}

void RoomTableModel::refreshHeaderTranslations()
{
    emit headerDataChanged(Qt::Horizontal, 0, ColumnCount - 1);
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
    return QStringList{CsvMimeType, QStringLiteral("text/plain")};
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
    mimeData->setData(CsvMimeType, text.toUtf8());
    mimeData->setText(text);
    return mimeData;
}

bool RoomTableModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    Q_UNUSED(column)

    if (data == nullptr || action == Qt::IgnoreAction) {
        return true;
    }

    const QString text = data->hasFormat(CsvMimeType)
                         ? QString::fromUtf8(data->data(CsvMimeType))
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
        escapeCsvField(room.roomNumber),
        escapeCsvField(room.roomType),
        QString::number(room.area, 'f', 2),
        QString::number(room.capacity),
        escapeCsvField(room.department),
        escapeCsvField(room.responsiblePerson),
        escapeCsvField(room.condition),
        room.lastRenovationDate.toString(DateFormat),
        escapeCsvField(room.notes)
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
        room.roomNumber = fields.at(RoomNumberColumn).trimmed();
        room.roomType = fields.at(RoomTypeColumn).trimmed();
        room.area = fields.at(AreaColumn).toDouble();
        room.capacity = fields.at(CapacityColumn).toInt();
        room.department = fields.at(DepartmentColumn).trimmed();
        room.responsiblePerson = fields.at(ResponsiblePersonColumn).trimmed();
        room.condition = fields.at(ConditionColumn).trimmed();
        room.lastRenovationDate = dateFromString(fields.at(LastRenovationDateColumn));
        room.notes = fields.at(NotesColumn).trimmed();
        result.append(room);
    }

    return result;
}
