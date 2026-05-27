#pragma once

#include <QMainWindow>

class DocumentWidget;

namespace Ui {
class MainWindow;
}

/**
 * @brief Main application window.
 *
 * Provides menu actions for multi-document work through QTabWidget.
 * Each tab contains its own DocumentWidget and its own RoomTableModel.
 */
class MainWindow final : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Creates the main window.
     * @param parent Parent widget.
     */
    explicit MainWindow(QWidget *parent = nullptr);

    /**
     * @brief Destroys the main window.
     */
    ~MainWindow() override;

private slots:
    /**
     * @brief Creates an empty document tab.
     */
    void newDocument();

    /**
     * @brief Opens one or more room database files.
     */
    void openFiles();

    /**
     * @brief Saves the current document.
     */
    void saveCurrentDocument();

    /**
     * @brief Saves the current document using a new file path.
     */
    void saveCurrentDocumentAs();

    /**
     * @brief Closes the current document tab.
     */
    void closeCurrentDocument();

    /**
     * @brief Closes the selected document tab.
     * @param index Tab index.
     */
    void closeDocumentAt(int index);

    /**
     * @brief Adds a room to the current document.
     */
    void addRoom();

    /**
     * @brief Edits selected room in the current document.
     */
    void editRoom();

    /**
     * @brief Deletes selected rooms in the current document.
     */
    void deleteRooms();

    /**
     * @brief Prints the current document.
     */
    void printCurrentDocument();

    /**
     * @brief Shows chart for the current document.
     */
    void showCurrentChart();

    /**
     * @brief Focuses search field in the current document.
     */
    void focusSearch();

    /**
     * @brief Clears search field in the current document.
     */
    void clearSearch();

    /**
     * @brief Shows the about dialog.
     */
    void showAboutDialog();

    /**
     * @brief Updates enabled state of actions.
     */
    void updateActions();

    /**
     * @brief Updates title of a changed tab.
     */
    void updateCurrentTabTitle();

private:
    /**
     * @brief Connects menu actions to slots.
     */
    void setupActions();

    /**
     * @brief Returns the active document widget.
     * @return Active document or nullptr if there is no tab.
     */
    DocumentWidget *currentDocument() const;

    /**
     * @brief Adds a document widget as a new tab.
     * @param document Document widget to add.
     */
    void addDocumentTab(DocumentWidget *document);

    /**
     * @brief Saves a document if it has unsaved changes.
     * @param document Document to check.
     * @return True if closing can continue.
     */
    bool maybeSave(DocumentWidget *document);

    /**
     * @brief Shows a save file dialog and saves the document.
     * @param document Document to save.
     * @return True if saved.
     */
    bool saveDocumentAs(DocumentWidget *document);

    Ui::MainWindow *ui;
};
