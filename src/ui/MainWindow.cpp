#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "AboutDialog.h"
#include "DocumentWidget.h"
#include "SettingsManager.h"
#include "TranslationManager.h"

#include <QApplication>
#include <QCloseEvent>
#include <QDir>
#include <QEvent>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QStatusBar>
#include <QTabWidget>

namespace {
QString databaseDirectory()
{
#ifdef OFFICE_ROOMS_DATA_DIR
    const QString configuredPath = QString::fromUtf8(OFFICE_ROOMS_DATA_DIR);
    if (QDir(configuredPath).exists()) {
        return configuredPath;
    }
#endif

    const QString applicationDataPath = QCoreApplication::applicationDirPath() + QDir::separator() + QStringLiteral("data");
    if (QDir(applicationDataPath).exists()) {
        return applicationDataPath;
    }

    const QString currentDataPath = QDir::currentPath() + QDir::separator() + QStringLiteral("data");
    if (QDir(currentDataPath).exists()) {
        return currentDataPath;
    }

    return QDir::homePath();
}
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_settingsManager(new SettingsManager)
    , m_translationManager(new TranslationManager(qobject_cast<QApplication *>(QApplication::instance()), this))
{
    ui->setupUi(this);
    setupActions();
    restoreSettings();
    updateActions();
}

MainWindow::~MainWindow()
{
    saveSettings();
    delete m_settingsManager;
    delete ui;
}

void MainWindow::changeEvent(QEvent *event)
{
    if (event != nullptr && event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
        updateActions();
        for (int index = 0; index < ui->documentsTabWidget->count(); ++index) {
            if (auto *document = qobject_cast<DocumentWidget *>(ui->documentsTabWidget->widget(index))) {
                ui->documentsTabWidget->setTabText(index, document->displayName());
            }
        }
    }

    QMainWindow::changeEvent(event);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    for (int index = ui->documentsTabWidget->count() - 1; index >= 0; --index) {
        auto *document = qobject_cast<DocumentWidget *>(ui->documentsTabWidget->widget(index));
        if (!maybeSave(document)) {
            event->ignore();
            return;
        }
    }

    saveSettings();
    QMainWindow::closeEvent(event);
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
        databaseDirectory(),
        tr("Office rooms database (*.ordb);;Room database (*.rooms);;Text files (*.txt);;All files (*.*)")
    );

    for (const QString &filePath : filePaths) {
        auto *document = new DocumentWidget(this);
        QString errorMessage;

        if (!document->loadFromFile(filePath, &errorMessage)) {
            document->deleteLater();
            QMessageBox::warning(this, tr("Open file"), errorMessage);
            continue;
        }

        m_settingsManager->addRecentFile(filePath);
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

    m_settingsManager->addRecentFile(document->filePath());
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

    document->saveColumnWidths();
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
    AboutDialog dialog(this);
    dialog.exec();
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

void MainWindow::switchToEnglish()
{
    switchLanguage(QStringLiteral("en"));
}

void MainWindow::switchToRussian()
{
    switchLanguage(QStringLiteral("ru"));
}

void MainWindow::switchToGerman()
{
    switchLanguage(QStringLiteral("de"));
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

    connect(ui->actionEnglish, &QAction::triggered, this, &MainWindow::switchToEnglish);
    connect(ui->actionRussian, &QAction::triggered, this, &MainWindow::switchToRussian);
    connect(ui->actionGerman, &QAction::triggered, this, &MainWindow::switchToGerman);
}

void MainWindow::restoreSettings()
{
    m_settingsManager->restoreMainWindow(this);
    switchLanguage(m_settingsManager->language());
}

void MainWindow::saveSettings() const
{
    m_settingsManager->saveMainWindow(this);
    if (DocumentWidget *document = currentDocument()) {
        document->saveColumnWidths();
    }
}

void MainWindow::switchLanguage(const QString &language)
{
    if (m_translationManager->switchLanguage(language)) {
        m_settingsManager->setLanguage(language);
        statusBar()->showMessage(tr("Language changed"), 3000);
    }
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

    m_settingsManager->addRecentFile(document->filePath());
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
        document->filePath().isEmpty() ? databaseDirectory() : document->filePath(),
        tr("Office rooms database (*.ordb);;Room database (*.rooms);;Text files (*.txt);;All files (*.*)")
    );

    if (filePath.isEmpty()) {
        return false;
    }

    QString errorMessage;
    if (!document->saveAs(filePath, &errorMessage)) {
        QMessageBox::warning(this, tr("Save file"), errorMessage);
        return false;
    }

    m_settingsManager->addRecentFile(filePath);
    statusBar()->showMessage(tr("File saved"), 3000);
    return true;
}
