#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "DocumentWidget.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QStatusBar>
#include <QTabWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupActions();
    updateActions();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::newDocument()
{
    auto *document = new DocumentWidget(this);
    addDocumentTab(document);
}

void MainWindow::openFiles()
{
    const QStringList filePaths = QFileDialog::getOpenFileNames(
        this,
        tr("Open room database"),
        QString(),
        tr("Room database (*.rooms);;Text files (*.txt);;All files (*.*)")
    );

    for (const QString &filePath : filePaths) {
        auto *document = new DocumentWidget(this);
        QString errorMessage;

        if (!document->loadFromFile(filePath, &errorMessage)) {
            document->deleteLater();
            QMessageBox::warning(this, tr("Open file"), errorMessage);
            continue;
        }

        addDocumentTab(document);
    }
}

void MainWindow::saveCurrentDocument()
{
    DocumentWidget *document = currentDocument();
    if (document == nullptr) {
        return;
    }

    if (document->filePath().isEmpty()) {
        saveDocumentAs(document);
        return;
    }

    QString errorMessage;
    if (!document->save(&errorMessage)) {
        QMessageBox::warning(this, tr("Save file"), errorMessage);
        return;
    }

    statusBar()->showMessage(tr("File saved"), 3000);
}

void MainWindow::saveCurrentDocumentAs()
{
    DocumentWidget *document = currentDocument();
    if (document == nullptr) {
        return;
    }

    saveDocumentAs(document);
}

void MainWindow::closeCurrentDocument()
{
    closeDocumentAt(ui->documentsTabWidget->currentIndex());
}

void MainWindow::closeDocumentAt(int index)
{
    if (index < 0) {
        return;
    }

    auto *document = qobject_cast<DocumentWidget *>(ui->documentsTabWidget->widget(index));
    if (document == nullptr) {
        return;
    }

    if (!maybeSave(document)) {
        return;
    }

    ui->documentsTabWidget->removeTab(index);
    document->deleteLater();
    updateActions();
}

void MainWindow::addRoom()
{
    if (DocumentWidget *document = currentDocument()) {
        document->addRoom();
    }
}

void MainWindow::editRoom()
{
    if (DocumentWidget *document = currentDocument()) {
        document->editSelectedRoom();
    }
}

void MainWindow::deleteRooms()
{
    if (DocumentWidget *document = currentDocument()) {
        document->deleteSelectedRooms();
    }
}

void MainWindow::printCurrentDocument()
{
    if (DocumentWidget *document = currentDocument()) {
        document->printDocument();
    }
}

void MainWindow::showCurrentChart()
{
    if (DocumentWidget *document = currentDocument()) {
        document->showChart();
    }
}

void MainWindow::focusSearch()
{
    if (DocumentWidget *document = currentDocument()) {
        document->focusSearch();
    }
}

void MainWindow::clearSearch()
{
    if (DocumentWidget *document = currentDocument()) {
        document->clearSearch();
    }
}

void MainWindow::showAboutDialog()
{
    QMessageBox::about(
        this,
        tr("About"),
        tr("Office Rooms\n\nCoursework: Office premises accounting software for a construction company.\n\nAuthor: Shavaliev Amir Ilmirovich\nInstitute: ICTMS\nCourse and group: II-101")
    );
}

void MainWindow::updateActions()
{
    const bool hasDocument = currentDocument() != nullptr;

    ui->actionSave->setEnabled(hasDocument);
    ui->actionSaveAs->setEnabled(hasDocument);
    ui->actionClose->setEnabled(hasDocument);
    ui->actionAdd->setEnabled(hasDocument);
    ui->actionEdit->setEnabled(hasDocument);
    ui->actionDelete->setEnabled(hasDocument);
    ui->actionPrint->setEnabled(hasDocument);
    ui->actionChart->setEnabled(hasDocument);
    ui->actionFind->setEnabled(hasDocument);
    ui->actionClearSearch->setEnabled(hasDocument);
}

void MainWindow::updateCurrentTabTitle()
{
    auto *document = qobject_cast<DocumentWidget *>(sender());
    if (document == nullptr) {
        document = currentDocument();
    }

    const int index = ui->documentsTabWidget->indexOf(document);
    if (index >= 0) {
        ui->documentsTabWidget->setTabText(index, document->displayName());
    }
}

void MainWindow::setupActions()
{
    connect(ui->actionNew, &QAction::triggered, this, &MainWindow::newDocument);
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::openFiles);
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::saveCurrentDocument);
    connect(ui->actionSaveAs, &QAction::triggered, this, &MainWindow::saveCurrentDocumentAs);
    connect(ui->actionClose, &QAction::triggered, this, &MainWindow::closeCurrentDocument);
    connect(ui->actionExit, &QAction::triggered, this, &QWidget::close);

    connect(ui->actionAdd, &QAction::triggered, this, &MainWindow::addRoom);
    connect(ui->actionEdit, &QAction::triggered, this, &MainWindow::editRoom);
    connect(ui->actionDelete, &QAction::triggered, this, &MainWindow::deleteRooms);

    connect(ui->actionPrint, &QAction::triggered, this, &MainWindow::printCurrentDocument);
    connect(ui->actionChart, &QAction::triggered, this, &MainWindow::showCurrentChart);
    connect(ui->actionFind, &QAction::triggered, this, &MainWindow::focusSearch);
    connect(ui->actionClearSearch, &QAction::triggered, this, &MainWindow::clearSearch);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::showAboutDialog);

    connect(ui->documentsTabWidget, &QTabWidget::currentChanged, this, &MainWindow::updateActions);
    connect(ui->documentsTabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::closeDocumentAt);

    connect(ui->actionEnglish, &QAction::triggered, this, [this]() {
        statusBar()->showMessage(tr("English language will be loaded by TranslationManager."), 3000);
    });
    connect(ui->actionRussian, &QAction::triggered, this, [this]() {
        statusBar()->showMessage(tr("Russian language will be loaded by TranslationManager."), 3000);
    });
    connect(ui->actionGerman, &QAction::triggered, this, [this]() {
        statusBar()->showMessage(tr("German language will be loaded by TranslationManager."), 3000);
    });
}

DocumentWidget *MainWindow::currentDocument() const
{
    return qobject_cast<DocumentWidget *>(ui->documentsTabWidget->currentWidget());
}

void MainWindow::addDocumentTab(DocumentWidget *document)
{
    const int index = ui->documentsTabWidget->addTab(document, document->displayName());
    ui->documentsTabWidget->setCurrentIndex(index);

    connect(document, &DocumentWidget::titleChanged, this, &MainWindow::updateCurrentTabTitle);
    connect(document, &DocumentWidget::modifiedChanged, this, &MainWindow::updateActions);
    updateActions();
}

bool MainWindow::maybeSave(DocumentWidget *document)
{
    if (document == nullptr || !document->isModified()) {
        return true;
    }

    const QMessageBox::StandardButton result = QMessageBox::question(
        this,
        tr("Unsaved changes"),
        tr("Save changes in %1?").arg(document->displayName()),
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel
    );

    if (result == QMessageBox::Cancel) {
        return false;
    }

    if (result == QMessageBox::Discard) {
        return true;
    }

    if (document->filePath().isEmpty()) {
        return saveDocumentAs(document);
    }

    QString errorMessage;
    if (!document->save(&errorMessage)) {
        QMessageBox::warning(this, tr("Save file"), errorMessage);
        return false;
    }

    return true;
}

bool MainWindow::saveDocumentAs(DocumentWidget *document)
{
    if (document == nullptr) {
        return false;
    }

    const QString filePath = QFileDialog::getSaveFileName(
        this,
        tr("Save room database"),
        document->filePath(),
        tr("Room database (*.rooms);;Text files (*.txt);;All files (*.*)")
    );

    if (filePath.isEmpty()) {
        return false;
    }

    QString errorMessage;
    if (!document->saveAs(filePath, &errorMessage)) {
        QMessageBox::warning(this, tr("Save file"), errorMessage);
        return false;
    }

    statusBar()->showMessage(tr("File saved"), 3000);
    return true;
}
