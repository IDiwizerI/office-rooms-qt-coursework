#pragma once

#include <QAbstractTableModel>
#include <QStringList>
#include <QVector>

#include "core/room.h"

class QMimeData;

/**
 * @brief Table model for office room records.
 *
 * The model stores room data in QVector<Room> and provides access to it
 * through Qt's Model/View architecture. It also supports drag-and-drop using
 * text/csv and text/plain data, so selected records can be moved between
 * documents and copied to external programs.
 */
class RoomTableModel final : public QAbstractTableModel
{
    Q_OBJECT

public:
    /**
     * @brief Table column indexes.
     */
    enum Column {
        IdColumn = 0,
        BuildingColumn,
        FloorColumn,
        NumberColumn,
        TypeColumn,
        AreaColumn,
        DepartmentColumn,
        WorkplacesColumn,
        StatusColumn,
        ResponsibleColumn,
        ColumnCount
    };

    /**
     * @brief Creates a room table model.
     * @param parent Parent QObject.
     */
    explicit RoomTableModel(QObject *parent = nullptr);

    /** @brief Returns the number of rows. */
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    /** @brief Returns the number of columns. */
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    /** @brief Returns data for a model index. */
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    /** @brief Sets data for a model index. */
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    /** @brief Returns header data. */
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    /** @brief Returns item flags for a model index. */
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    /** @brief Returns MIME types supported by the model. */
    QStringList mimeTypes() const override;

    /** @brief Creates MIME data for selected indexes. */
    QMimeData *mimeData(const QModelIndexList &indexes) const override;

    /** @brief Drops MIME data into this model. */
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override;

    /** @brief Returns supported drag actions. */
    Qt::DropActions supportedDragActions() const override;

    /** @brief Returns supported drop actions. */
    Qt::DropActions supportedDropActions() const override;

    /** @brief Replaces all model data. */
    void setRooms(const QVector<Room> &rooms);

    /** @brief Returns all rooms. */
    const QVector<Room> &rooms() const;

    /** @brief Returns room by row. */
    Room roomAt(int row) const;

    /** @brief Adds a new room. */
    void addRoom(const Room &room);

    /** @brief Updates an existing room. */
    bool updateRoom(int row, const Room &room);

    /** @brief Removes a room. */
    bool removeRoom(int row);

private:
    /** @brief Checks whether row index is valid. */
    bool isValidRow(int row) const;

    /** @brief Converts a room record to CSV line. */
    QString roomToCsvLine(const Room &room) const;

    /** @brief Converts CSV text to room records. */
    QVector<Room> roomsFromCsv(const QString &text) const;

    QVector<Room> m_rooms;
};
