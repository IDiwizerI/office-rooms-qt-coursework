#pragma once

#include <QAbstractTableModel>
#include <QVector>

#include "core/room.h"

/**
 * @brief Table model for office room records.
 *
 * The model stores room data in QVector<Room> and provides access to it
 * through Qt's Model/View architecture.
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

    /**
     * @brief Returns the number of rows.
     * @param parent Parent model index.
     * @return Row count.
     */
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    /**
     * @brief Returns the number of columns.
     * @param parent Parent model index.
     * @return Column count.
     */
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    /**
     * @brief Returns data for a model index.
     * @param index Model index.
     * @param role Data role.
     * @return Cell data.
     */
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    /**
     * @brief Sets data for a model index.
     * @param index Model index.
     * @param value New value.
     * @param role Data role.
     * @return True if data was changed.
     */
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    /**
     * @brief Returns header data.
     * @param section Header section.
     * @param orientation Header orientation.
     * @param role Data role.
     * @return Header value.
     */
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    /**
     * @brief Returns item flags for a model index.
     * @param index Model index.
     * @return Item flags.
     */
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    /**
     * @brief Replaces all model data.
     * @param rooms New room list.
     */
    void setRooms(const QVector<Room> &rooms);

    /**
     * @brief Returns all rooms.
     * @return Constant reference to room list.
     */
    const QVector<Room> &rooms() const;

    /**
     * @brief Returns room by row.
     * @param row Row index.
     * @return Room object or a default Room for an invalid row.
     */
    Room roomAt(int row) const;

    /**
     * @brief Adds a new room.
     * @param room Room to add.
     */
    void addRoom(const Room &room);

    /**
     * @brief Updates an existing room.
     * @param row Row index.
     * @param room New room data.
     * @return True if room was updated.
     */
    bool updateRoom(int row, const Room &room);

    /**
     * @brief Removes a room.
     * @param row Row index.
     * @return True if room was removed.
     */
    bool removeRoom(int row);

private:
    /**
     * @brief Checks whether row index is valid.
     * @param row Row index.
     * @return True if row exists.
     */
    bool isValidRow(int row) const;

    QVector<Room> m_rooms;
};
