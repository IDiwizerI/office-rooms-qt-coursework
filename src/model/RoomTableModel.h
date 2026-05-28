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
     * @brief Table column indexes matching the ORDB text database format.
     */
    enum Column {
        IdColumn = 0,
        BuildingColumn,
        FloorColumn,
        RoomNumberColumn,
        RoomTypeColumn,
        AreaColumn,
        CapacityColumn,
        DepartmentColumn,
        ResponsiblePersonColumn,
        ConditionColumn,
        LastRenovationDateColumn,
        NotesColumn,
        ColumnCount
    };

    /**
     * @brief Creates a room table model.
     * @param parent Parent QObject.
     */
    explicit RoomTableModel(QObject *parent = nullptr);

    /**
     * @brief Returns the number of rows in the model.
     * @param parent Parent index for hierarchical models; must be invalid for this table model.
     * @return Number of room records.
     */
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    /**
     * @brief Returns the number of columns in the model.
     * @param parent Parent index for hierarchical models; must be invalid for this table model.
     * @return Number of table columns.
     */
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    /**
     * @brief Returns data for a model index and role.
     * @param index Model index.
     * @param role Qt data role.
     * @return Cell value, alignment value or invalid QVariant for unsupported roles.
     */
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    /**
     * @brief Sets data for a model index.
     * @param index Model index to update.
     * @param value New cell value.
     * @param role Qt data role.
     * @return True if the value was accepted and stored.
     */
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    /**
     * @brief Returns header text for rows and columns.
     * @param section Header section index.
     * @param orientation Header orientation.
     * @param role Qt data role.
     * @return Header value or invalid QVariant for unsupported roles.
     */
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    /**
     * @brief Returns item flags for a model index.
     * @param index Model index.
     * @return Enabled item flags including edit, drag and drop support.
     */
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    /**
     * @brief Returns MIME types supported by the model.
     * @return List containing text/csv and text/plain MIME types.
     */
    QStringList mimeTypes() const override;

    /**
     * @brief Creates MIME data for selected indexes.
     * @param indexes Selected model indexes.
     * @return New MIME data object owned by the caller.
     */
    QMimeData *mimeData(const QModelIndexList &indexes) const override;

    /**
     * @brief Drops MIME data into this model.
     * @param data Dropped MIME data.
     * @param action Requested drop action.
     * @param row Target row or -1.
     * @param column Target column or -1.
     * @param parent Parent index for the drop target.
     * @return True if records were inserted successfully.
     */
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override;

    /**
     * @brief Returns supported drag actions.
     * @return Copy and move drag actions.
     */
    Qt::DropActions supportedDragActions() const override;

    /**
     * @brief Returns supported drop actions.
     * @return Copy and move drop actions.
     */
    Qt::DropActions supportedDropActions() const override;

    /**
     * @brief Replaces all model data.
     * @param rooms New room list.
     */
    void setRooms(const QVector<Room> &rooms);

    /**
     * @brief Returns all rooms stored in the model.
     * @return Constant reference to the room container.
     */
    const QVector<Room> &rooms() const;

    /**
     * @brief Returns room data by row.
     * @param row Source model row.
     * @return Room record or default Room for invalid row.
     */
    Room roomAt(int row) const;

    /**
     * @brief Adds a room to the end of the model.
     * @param room Room record to add.
     */
    void addRoom(const Room &room);

    /**
     * @brief Updates an existing room.
     * @param row Source model row.
     * @param room New room record.
     * @return True if the row exists and was updated.
     */
    bool updateRoom(int row, const Room &room);

    /**
     * @brief Removes an existing room.
     * @param row Source model row.
     * @return True if the row exists and was removed.
     */
    bool removeRoom(int row);

private:
    /**
     * @brief Checks whether row index is valid.
     * @param row Source model row.
     * @return True if row exists.
     */
    bool isValidRow(int row) const;

    /**
     * @brief Converts a room record to CSV line.
     * @param room Room record to convert.
     * @return CSV line with semicolon-separated fields.
     */
    QString roomToCsvLine(const Room &room) const;

    /**
     * @brief Converts CSV text to room records.
     * @param text CSV text with one or more room records.
     * @return Parsed room records.
     */
    QVector<Room> roomsFromCsv(const QString &text) const;

    QVector<Room> m_rooms;
};
