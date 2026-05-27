#include "DocumentWidget.h"
#include "ui_DocumentWidget.h"

#include "RoomSortFilterProxyModel.h"
#include "RoomTableModel.h"
#include "core/room.h"

#include <QAbstractItemModel>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFile>
#include <QFileInfo>
#include <QFormLayout>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QLineEdit>
#include <QMap>
#include <QMenu>
#include <QMessageBox>
#include <QPainter>
#include <QPlainTextEdit>
#include <QPrintDialog>
#include <QPrinter>
#include <QPushButton>
#include <QTableView>
#include <QTextStream>
#include <QVBoxLayout>

#include <algorithm>
#include <functional>

namespace {
constexpr int AllColumnsValue = -1;
constexpr int RequiredColumnCount = 10;

QString escapeField(QString value)
{
    value.replace('\\', "\\\\");
    value.replace(';', "\\;");
    value.replace('\n', " ");
    value.replace('\r', " ");
    return value;
}

QStringList splitLine(const QString &line)
{
    QStringList result;
    QString current;
    bool escaped = false;

    for (const QChar character : line) {
        if (escaped) {
            current.append(character);
            escaped = false;
        } else if (character == '\\') {
            escaped = true;
        } else if (character == ';') {
            result.append(current);
            current.clear();
        } else {
            current.append(character);
        }
    }

    result.append(current);
    return result;
}

Room roomFromFields(const QStringList &fields)
{
    Room room;
    room.id = fields.value(RoomTableModel::IdColumn).trimmed();
    room.building = fields.value(RoomTableModel::BuildingColumn).trimmed();
    room.floor = fields.value(RoomTableModel::FloorColumn).toInt();
    room.number = fields.value(RoomTableModel::NumberColumn).trimmed();
    room.type = fields.value(RoomTableModel::TypeColumn).trimmed();
    room.area = fields.value(RoomTableModel::AreaColumn).toDouble();
    room.department = fields.value(RoomTableModel::DepartmentColumn).trimmed();
    room.workplaces = fields.value(RoomTableModel::WorkplacesColumn).toInt();
    room.status = fields.value(RoomTableModel::StatusColumn).trimmed();
    room.responsible = fields.value(RoomTableModel::ResponsibleColumn).trimmed();
    return room;
}

QString roomToLine(const Room &room)
{
    return QStringList{
        escapeField(room.id),
        escapeField(room.building),
        QString::number(room.floor),
        escapeField(room.number),
        escapeField(room.type),
        QString::number(room.area, 'f', 2),
        escapeField(room.department),
        QString::number(room.workplaces),
        escapeField(room.status),
        escapeField(room.responsible)
    }.join(';');
}

void addLineEdit(QFormLayout *layout, QList<QLineEdit *> *fields, const QString &label, const QString &value)
{
    auto *lineEdit = new QLineEdit(value);
    layout->addRow(label, lineEdit);
    fields->append(lineEdit);
}
} // namespace

DocumentWidget::DocumentWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DocumentWidget)
    , m_model(new RoomTableModel(this))
    , m_proxyModel(new RoomSortFilterProxyModel(this))
{
    ui->setupUi(this);
    setupTable();
    setupSearchColumns();

    connect(ui->searchColumnComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &DocumentWidget::updateSearchColumn);
    connect(ui->searchLineEdit, &QLineEdit::textChanged, this, &DocumentWidget::updateSearchText);
    connect(ui->clearSearchButton, &QPushButton::clicked, this, &DocumentWidget::clearSearch);

    connect(m_model, &QAbstractItemModel::dataChanged, this, &DocumentWidget::markModified);
    connect(m_model, &QAbstractItemModel::rowsInserted, this, &DocumentWidget::markModified);
    connect(m_model, &QAbstractItemModel::rowsRemoved, this, &DocumentWidget::markModified);
    connect(m_model, &QAbstractItemModel::modelReset, this, &DocumentWidget::markModified);
}

DocumentWidget::~DocumentWidget()
{
    delete ui;
}

QString DocumentWidget::filePath() const
{
    return m_filePath;
}

QString DocumentWidget::displayName() const
{
    const QString baseName = m_filePath.isEmpty() ? tr("Untitled") : QFileInfo(m_filePath).fileName();
    return m_modified ? baseName + '*' : baseName;
}

bool DocumentWidget::isModified() const
{
    return m_modified;
}

QTableView *DocumentWidget::tableView() const
{
    return ui->roomsTableView;
}

bool DocumentWidget::loadFromFile(const QString &filePath, QString *errorMessage)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        if (errorMessage != nullptr) {
            *errorMessage = tr("Cannot open file for reading.");
        }
        return false;
    }

    QTextStream input(&file);
    input.setEncoding(QStringConverter::Utf8);

    const QString signature = input.readLine().trimmed();
    if (signature != QStringLiteral("ROOMS_OFFICE_DB;VERSION=1")) {
        if (errorMessage != nullptr) {
            *errorMessage = tr("The selected file is not an office rooms database.");
        }
        return false;
    }

    const QString columns = input.readLine().trimmed();
    if (!columns.startsWith(QStringLiteral("COLUMNS="))) {
        if (errorMessage != nullptr) {
            *errorMessage = tr("The database header is damaged.");
        }
        return false;
    }

    QVector<Room> rooms;
    int lineNumber = 2;

    while (!input.atEnd()) {
        ++lineNumber;
        const QString line = input.readLine().trimmed();
        if (line.isEmpty()) {
            continue;
        }

        const QStringList fields = splitLine(line);
        if (fields.size() != RequiredColumnCount) {
            if (errorMessage != nullptr) {
                *errorMessage = tr("Invalid field count at line %1.").arg(lineNumber);
            }
            return false;
        }

        rooms.append(roomFromFields(fields));
    }

    m_loading = true;
    m_model->setRooms(rooms);
    m_loading = false;

    setFilePath(filePath);
    setModified(false);
    return true;
}

bool DocumentWidget::save(QString *errorMessage)
{
    if (m_filePath.isEmpty()) {
        if (errorMessage != nullptr) {
            *errorMessage = tr("File path is empty. Use Save As.");
        }
        return false;
    }

    return saveAs(m_filePath, errorMessage);
}

bool DocumentWidget::saveAs(const QString &filePath, QString *errorMessage)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        if (errorMessage != nullptr) {
            *errorMessage = tr("Cannot open file for writing.");
        }
        return false;
    }

    QTextStream output(&file);
    output.setEncoding(QStringConverter::Utf8);
    output << "ROOMS_OFFICE_DB;VERSION=1\n";
    output << "COLUMNS=id;building;floor;number;type;area;department;workplaces;status;responsible\n";

    for (const Room &room : m_model->rooms()) {
        output << roomToLine(room) << '\n';
    }

    setFilePath(filePath);
    setModified(false);
    return true;
}

void DocumentWidget::addRoom()
{
    Room room;
    if (!editRoomWithDialog(&room, tr("Add room"))) {
        return;
    }

    m_model->addRoom(room);
}

void DocumentWidget::editSelectedRoom()
{
    const int sourceRow = currentSourceRow();
    if (sourceRow < 0) {
        QMessageBox::information(this, tr("Edit room"), tr("Select a room to edit."));
        return;
    }

    Room room = m_model->roomAt(sourceRow);
    if (!editRoomWithDialog(&room, tr("Edit room"))) {
        return;
    }

    m_model->updateRoom(sourceRow, room);
}

void DocumentWidget::deleteSelectedRooms()
{
    const QModelIndexList selectedRows = ui->roomsTableView->selectionModel()->selectedRows();
    if (selectedRows.isEmpty()) {
        QMessageBox::information(this, tr("Delete rooms"), tr("Select rooms to delete."));
        return;
    }

    if (QMessageBox::question(this, tr("Delete rooms"), tr("Delete selected rooms?")) != QMessageBox::Yes) {
        return;
    }

    QList<int> sourceRows;
    for (const QModelIndex &proxyIndex : selectedRows) {
        sourceRows.append(m_proxyModel->mapToSource(proxyIndex).row());
    }
    std::sort(sourceRows.begin(), sourceRows.end(), std::greater<int>());

    for (const int row : sourceRows) {
        m_model->removeRoom(row);
    }
}

void DocumentWidget::printDocument()
{
    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog dialog(&printer, this);
    dialog.setWindowTitle(tr("Print"));

    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    QPainter painter(&printer);
    ui->roomsTableView->render(&painter);
}

void DocumentWidget::showChart()
{
    QMap<QString, double> areaByType;
    for (const Room &room : m_model->rooms()) {
        const QString type = room.type.trimmed().isEmpty() ? tr("Unknown") : room.type.trimmed();
        areaByType[type] += room.area;
    }

    QString chartText;
    double maxArea = 0.0;
    for (auto iterator = areaByType.cbegin(); iterator != areaByType.cend(); ++iterator) {
        maxArea = std::max(maxArea, iterator.value());
    }

    for (auto iterator = areaByType.cbegin(); iterator != areaByType.cend(); ++iterator) {
        const int barLength = maxArea > 0.0 ? static_cast<int>((iterator.value() / maxArea) * 40.0) : 0;
        chartText += QStringLiteral("%1 | %2 %3\n")
                         .arg(iterator.key(), -20)
                         .arg(QString(barLength, QChar(0x2588)))
                         .arg(iterator.value(), 0, 'f', 2);
    }

    if (chartText.isEmpty()) {
        chartText = tr("No data for chart.");
    }

    QDialog dialog(this);
    dialog.setWindowTitle(tr("Chart"));
    auto *layout = new QVBoxLayout(&dialog);
    auto *textEdit = new QPlainTextEdit(chartText);
    textEdit->setReadOnly(true);
    layout->addWidget(textEdit);
    dialog.resize(700, 400);
    dialog.exec();
}

void DocumentWidget::focusSearch()
{
    ui->searchLineEdit->setFocus();
    ui->searchLineEdit->selectAll();
}

void DocumentWidget::clearSearch()
{
    ui->searchLineEdit->clear();
    ui->searchColumnComboBox->setCurrentIndex(0);
    m_proxyModel->clearFilter();
}

void DocumentWidget::updateSearchColumn(int index)
{
    m_proxyModel->setSearchColumn(ui->searchColumnComboBox->itemData(index).toInt());
}

void DocumentWidget::updateSearchText(const QString &text)
{
    m_proxyModel->setSearchText(text);
}

void DocumentWidget::showContextMenu(const QPoint &position)
{
    QMenu menu(this);
    menu.addAction(tr("Add"), this, &DocumentWidget::addRoom);
    menu.addAction(tr("Edit"), this, &DocumentWidget::editSelectedRoom);
    menu.addAction(tr("Delete"), this, &DocumentWidget::deleteSelectedRooms);
    menu.addSeparator();
    menu.addAction(tr("Print"), this, &DocumentWidget::printDocument);
    menu.addAction(tr("Chart"), this, &DocumentWidget::showChart);
    menu.exec(ui->roomsTableView->viewport()->mapToGlobal(position));
}

void DocumentWidget::handleDoubleClick(const QModelIndex &index)
{
    if (index.isValid()) {
        editSelectedRoom();
    }
}

void DocumentWidget::markModified()
{
    if (!m_loading) {
        setModified(true);
    }
}

void DocumentWidget::setupTable()
{
    m_proxyModel->setSourceModel(m_model);
    ui->roomsTableView->setModel(m_proxyModel);
    ui->roomsTableView->setSortingEnabled(true);
    ui->roomsTableView->horizontalHeader()->setStretchLastSection(true);
    ui->roomsTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    ui->roomsTableView->verticalHeader()->setVisible(false);
    ui->roomsTableView->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui->roomsTableView, &QTableView::customContextMenuRequested, this, &DocumentWidget::showContextMenu);
    connect(ui->roomsTableView, &QTableView::doubleClicked, this, &DocumentWidget::handleDoubleClick);
}

void DocumentWidget::setupSearchColumns()
{
    ui->searchColumnComboBox->addItem(tr("All columns"), AllColumnsValue);
    for (int column = 0; column < RoomTableModel::ColumnCount; ++column) {
        ui->searchColumnComboBox->addItem(m_model->headerData(column, Qt::Horizontal).toString(), column);
    }
}

void DocumentWidget::setFilePath(const QString &filePath)
{
    if (m_filePath == filePath) {
        return;
    }

    m_filePath = filePath;
    emit filePathChanged(m_filePath);
    emit titleChanged();
}

void DocumentWidget::setModified(bool modified)
{
    if (m_modified == modified) {
        return;
    }

    m_modified = modified;
    emit modifiedChanged(m_modified);
    emit titleChanged();
}

bool DocumentWidget::editRoomWithDialog(Room *room, const QString &title)
{
    QDialog dialog(this);
    dialog.setWindowTitle(title);

    auto *layout = new QVBoxLayout(&dialog);
    auto *formLayout = new QFormLayout;
    QList<QLineEdit *> fields;

    addLineEdit(formLayout, &fields, tr("ID"), room->id);
    addLineEdit(formLayout, &fields, tr("Building"), room->building);
    addLineEdit(formLayout, &fields, tr("Floor"), QString::number(room->floor));
    addLineEdit(formLayout, &fields, tr("Room number"), room->number);
    addLineEdit(formLayout, &fields, tr("Type"), room->type);
    addLineEdit(formLayout, &fields, tr("Area"), QString::number(room->area, 'f', 2));
    addLineEdit(formLayout, &fields, tr("Department"), room->department);
    addLineEdit(formLayout, &fields, tr("Workplaces"), QString::number(room->workplaces));
    addLineEdit(formLayout, &fields, tr("Status"), room->status);
    addLineEdit(formLayout, &fields, tr("Responsible"), room->responsible);

    layout->addLayout(formLayout);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout->addWidget(buttons);
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() != QDialog::Accepted) {
        return false;
    }

    bool floorOk = false;
    bool areaOk = false;
    bool workplacesOk = false;
    const int floor = fields.at(2)->text().toInt(&floorOk);
    const double area = fields.at(5)->text().toDouble(&areaOk);
    const int workplaces = fields.at(7)->text().toInt(&workplacesOk);

    if (!floorOk || !areaOk || !workplacesOk || floor < 0 || area < 0.0 || workplaces < 0) {
        QMessageBox::warning(this, tr("Invalid data"), tr("Floor, area and workplaces must be non-negative numbers."));
        return false;
    }

    room->id = fields.at(0)->text().trimmed();
    room->building = fields.at(1)->text().trimmed();
    room->floor = floor;
    room->number = fields.at(3)->text().trimmed();
    room->type = fields.at(4)->text().trimmed();
    room->area = area;
    room->department = fields.at(6)->text().trimmed();
    room->workplaces = workplaces;
    room->status = fields.at(8)->text().trimmed();
    room->responsible = fields.at(9)->text().trimmed();

    return true;
}

int DocumentWidget::currentSourceRow() const
{
    const QModelIndex proxyIndex = ui->roomsTableView->currentIndex();
    if (!proxyIndex.isValid()) {
        return -1;
    }

    return m_proxyModel->mapToSource(proxyIndex).row();
}
