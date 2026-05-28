#include "DocumentWidget.h"
#include "ui_DocumentWidget.h"

#include "ChartDialog.h"
#include "PrintService.h"
#include "RoomEditDialog.h"
#include "RoomSortFilterProxyModel.h"
#include "RoomTableModel.h"
#include "SettingsManager.h"
#include "core/room.h"

#include <QAbstractItemModel>
#include <QComboBox>
#include <QFile>
#include <QFileInfo>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>
#include <QTableView>
#include <QTextStream>

#include <algorithm>
#include <functional>

namespace {
constexpr int AllColumnsValue = -1;
constexpr int RequiredColumnCount = 10;
constexpr auto ColumnSettingsKey = "roomsTable";

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
    restoreColumnWidths();

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
    saveColumnWidths();
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

    SettingsManager().addRecentFile(filePath);
    setFilePath(filePath);
    setModified(false);
    return true;
}

void DocumentWidget::saveColumnWidths() const
{
    SettingsManager().saveColumnWidths(ui->roomsTableView, QStringLiteral(ColumnSettingsKey));
}

void DocumentWidget::restoreColumnWidths()
{
    SettingsManager().restoreColumnWidths(ui->roomsTableView, QStringLiteral(ColumnSettingsKey));
}

void DocumentWidget::addRoom()
{
    RoomEditDialog dialog(this);
    dialog.setWindowTitle(tr("Add room"));

    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    m_model->addRoom(dialog.room());
}

void DocumentWidget::editSelectedRoom()
{
    const int sourceRow = currentSourceRow();
    if (sourceRow < 0) {
        QMessageBox::information(this, tr("Edit room"), tr("Select a room to edit."));
        return;
    }

    RoomEditDialog dialog(this);
    dialog.setWindowTitle(tr("Edit room"));
    dialog.setRoom(m_model->roomAt(sourceRow));

    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    m_model->updateRoom(sourceRow, dialog.room());
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
    PrintService printService(this);
    printService.printModel(m_proxyModel, this, displayName());
}

void DocumentWidget::showChart()
{
    ChartDialog dialog(this);
    dialog.setRooms(m_model->rooms());
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
    ui->roomsTableView->setDragEnabled(true);
    ui->roomsTableView->setAcceptDrops(true);
    ui->roomsTableView->setDropIndicatorShown(true);
    ui->roomsTableView->setDragDropMode(QAbstractItemView::DragDrop);
    ui->roomsTableView->setDefaultDropAction(Qt::CopyAction);
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

int DocumentWidget::currentSourceRow() const
{
    const QModelIndex proxyIndex = ui->roomsTableView->currentIndex();
    if (!proxyIndex.isValid()) {
        return -1;
    }

    return m_proxyModel->mapToSource(proxyIndex).row();
}
