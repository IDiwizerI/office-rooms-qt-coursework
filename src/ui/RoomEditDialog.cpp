#include "RoomEditDialog.h"
#include "ui_RoomEditDialog.h"

#include <QDoubleValidator>
#include <QIntValidator>
#include <QMessageBox>

RoomEditDialog::RoomEditDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RoomEditDialog)
{
    ui->setupUi(this);
    setupValidators();
    ui->lastRenovationDateEdit->setDate(QDate::currentDate());
}

RoomEditDialog::~RoomEditDialog()
{
    delete ui;
}

void RoomEditDialog::setRoom(const Room &room)
{
    ui->idLineEdit->setText(room.id);
    ui->buildingLineEdit->setText(room.building);
    ui->floorLineEdit->setText(QString::number(room.floor));
    ui->numberLineEdit->setText(room.roomNumber);
    ui->typeLineEdit->setText(room.roomType);
    ui->areaLineEdit->setText(QString::number(room.area, 'f', 2));
    ui->capacityLineEdit->setText(QString::number(room.capacity));
    ui->departmentLineEdit->setText(room.department);
    ui->responsibleLineEdit->setText(room.responsiblePerson);
    ui->conditionLineEdit->setText(room.condition);
    ui->lastRenovationDateEdit->setDate(room.lastRenovationDate.isValid() ? room.lastRenovationDate : QDate::currentDate());
    ui->notesPlainTextEdit->setPlainText(room.notes);
}

Room RoomEditDialog::room() const
{
    Room room;
    room.id = ui->idLineEdit->text().trimmed();
    room.building = ui->buildingLineEdit->text().trimmed();
    room.floor = ui->floorLineEdit->text().toInt();
    room.roomNumber = ui->numberLineEdit->text().trimmed();
    room.roomType = ui->typeLineEdit->text().trimmed();
    room.area = ui->areaLineEdit->text().toDouble();
    room.capacity = ui->capacityLineEdit->text().toInt();
    room.department = ui->departmentLineEdit->text().trimmed();
    room.responsiblePerson = ui->responsibleLineEdit->text().trimmed();
    room.condition = ui->conditionLineEdit->text().trimmed();
    room.lastRenovationDate = ui->lastRenovationDateEdit->date();
    room.notes = ui->notesPlainTextEdit->toPlainText().trimmed();
    return room;
}

void RoomEditDialog::setLastRenovationDate(const QDate &date)
{
    ui->lastRenovationDateEdit->setDate(date.isValid() ? date : QDate::currentDate());
}

QDate RoomEditDialog::lastRenovationDate() const
{
    return ui->lastRenovationDateEdit->date();
}

void RoomEditDialog::accept()
{
    if (!validateInput()) {
        return;
    }

    QDialog::accept();
}

void RoomEditDialog::setupValidators()
{
    ui->floorLineEdit->setValidator(new QIntValidator(0, 200, this));
    ui->capacityLineEdit->setValidator(new QIntValidator(0, 10000, this));

    auto *areaValidator = new QDoubleValidator(0.0, 1000000.0, 2, this);
    areaValidator->setNotation(QDoubleValidator::StandardNotation);
    ui->areaLineEdit->setValidator(areaValidator);
}

bool RoomEditDialog::validateInput() const
{
    if (ui->idLineEdit->text().trimmed().isEmpty()
        || ui->buildingLineEdit->text().trimmed().isEmpty()
        || ui->floorLineEdit->text().trimmed().isEmpty()
        || ui->numberLineEdit->text().trimmed().isEmpty()
        || ui->typeLineEdit->text().trimmed().isEmpty()
        || ui->areaLineEdit->text().trimmed().isEmpty()
        || ui->capacityLineEdit->text().trimmed().isEmpty()
        || ui->departmentLineEdit->text().trimmed().isEmpty()
        || ui->responsibleLineEdit->text().trimmed().isEmpty()
        || ui->conditionLineEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(
            const_cast<RoomEditDialog *>(this),
            tr("Invalid data"),
            tr("Fill all required fields: ID, building, floor, room number, room type, area, capacity, department, responsible person and condition.")
        );
        return false;
    }

    bool floorOk = false;
    bool areaOk = false;
    bool capacityOk = false;
    const int floor = ui->floorLineEdit->text().toInt(&floorOk);
    const double area = ui->areaLineEdit->text().toDouble(&areaOk);
    const int capacity = ui->capacityLineEdit->text().toInt(&capacityOk);

    if (!floorOk || !areaOk || !capacityOk || floor < 0 || area < 0.0 || capacity < 0) {
        QMessageBox::warning(
            const_cast<RoomEditDialog *>(this),
            tr("Invalid data"),
            tr("Floor, area and capacity must be non-negative numbers.")
        );
        return false;
    }

    return true;
}
