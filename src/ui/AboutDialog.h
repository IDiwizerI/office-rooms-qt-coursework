#pragma once

#include <QDialog>

namespace Ui {
class AboutDialog;
}

/**
 * @brief Dialog with coursework author information.
 */
class AboutDialog final : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief Creates the about dialog.
     * @param parent Parent widget.
     */
    explicit AboutDialog(QWidget *parent = nullptr);

    /**
     * @brief Destroys the about dialog.
     */
    ~AboutDialog() override;

private:
    /**
     * @brief Fills labels with translated text.
     */
    void setupText();

    Ui::AboutDialog *ui;
};
