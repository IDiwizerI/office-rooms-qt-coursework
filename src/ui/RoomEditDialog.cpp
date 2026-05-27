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
    ui->numberLineEdit->setText(room.number);
    ui->typeLineEdit->setText(room.type);
    ui->areaLineEdit->setText(QString::number(room.area, 'f', 2));
    ui->departmentLineEdit->setText(room.department);
    ui->capacityLineEdit->setText(QString::number(room.workplaces));
    ui->statusLineEdit->setText(room.status);
    ui->responsibleLineEdit->setText(room.responsible);
}

Room RoomEditDialog::room() const
{
    Room room;
    room.id = ui->idLineEdit->text().trimmed();
    room.building = ui->buildingLineEdit->text().trimmed();
    room.floor = ui->floorLineEdit->text().toInt();
    room.number = ui->numberLineEdit->text().trimmed();
    room.type = ui->typeLineEdit->text().trimmed();
    room.area = ui->areaLineEdit->text().toDouble();
    room.department = ui->departmentLineEdit->text().trimmed();
    room.workplaces = ui->capacityLineEdit->text().toInt();
    room.status = ui->statusLineEdit->text().trimmed();
    room.responsible = ui->responsibleLineEdit->text().trimmed();
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
        || ui->departmentLineEdit->text().trimmed().isEmpty()
        || ui->capacityLineEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(
            const_cast<RoomEditDialog *>(this),
            tr("Invalid data"),
            tr("Fill all required fields: ID, building, floor, room number, type, area, department and capacity.")
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
