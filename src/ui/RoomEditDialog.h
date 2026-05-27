#pragma once

#include "core/room.h"

#include <QDate>
#include <QDialog>

namespace Ui {
class RoomEditDialog;
}

/**
 * @brief Dialog for adding and editing one office room record.
 *
 * The dialog validates mandatory text fields, uses QIntValidator for floor and
 * capacity, QDoubleValidator for area and provides a QDateEdit for the last
 * renovation date. The current Room data structure does not store the date, so
 * the date is kept as dialog data for future storage extension.
 */
class RoomEditDialog final : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief Creates a room editing dialog.
     * @param parent Parent widget.
     */
    explicit RoomEditDialog(QWidget *parent = nullptr);

    /**
     * @brief Destroys the room editing dialog.
     */
    ~RoomEditDialog() override;

    /**
     * @brief Sets room data to edit.
     * @param room Room data.
     */
    void setRoom(const Room &room);

    /**
     * @brief Returns room data from the dialog fields.
     * @return Room data.
     */
    Room room() const;

    /**
     * @brief Sets the last renovation date shown in the dialog.
     * @param date Last renovation date.
     */
    void setLastRenovationDate(const QDate &date);

    /**
     * @brief Returns selected last renovation date.
     * @return Last renovation date.
     */
    QDate lastRenovationDate() const;

protected:
    /**
     * @brief Validates fields before accepting the dialog.
     */
    void accept() override;

private:
    /**
     * @brief Configures validators for numeric input fields.
     */
    void setupValidators();

    /**
     * @brief Checks all mandatory fields.
     * @return True if mandatory fields are filled correctly.
     */
    bool validateInput() const;

    Ui::RoomEditDialog *ui;
};
