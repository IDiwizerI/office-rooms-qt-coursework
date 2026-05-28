#pragma once

#include <QObject>
#include <QString>

class QAbstractItemModel;
class QWidget;

/**
 * @brief Provides table printing through QPrinter and QPrintPreviewDialog.
 */
class PrintService final : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Creates a print service.
     * @param parent Parent QObject.
     */
    explicit PrintService(QObject *parent = nullptr);

    /**
     * @brief Opens print preview and prints model contents.
     * @param model Source model to print.
     * @param parent Parent widget for print preview dialog.
     * @param title Document title.
     */
    void printModel(QAbstractItemModel *model, QWidget *parent, const QString &title) const;
};
