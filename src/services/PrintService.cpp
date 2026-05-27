#include "PrintService.h"

#include <QAbstractItemModel>
#include <QDateTime>
#include <QPainter>
#include <QPrintPreviewDialog>
#include <QPrinter>
#include <QTextDocument>

PrintService::PrintService(QObject *parent)
    : QObject(parent)
{
}

void PrintService::printModel(QAbstractItemModel *model, QWidget *parent, const QString &title) const
{
    if (model == nullptr) {
        return;
    }

    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog previewDialog(&printer, parent);
    previewDialog.setWindowTitle(tr("Print preview"));

    connect(&previewDialog, &QPrintPreviewDialog::paintRequested, this, [model, title](QPrinter *targetPrinter) {
        QString html;
        html += QStringLiteral("<html><head><meta charset='utf-8'></head><body>");
        html += QStringLiteral("<h2>%1</h2>").arg(title.toHtmlEscaped());
        html += QStringLiteral("<p>%1</p>").arg(QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd HH:mm")).toHtmlEscaped());
        html += QStringLiteral("<table border='1' cellspacing='0' cellpadding='3' width='100%'>");
        html += QStringLiteral("<tr>");

        for (int column = 0; column < model->columnCount(); ++column) {
            html += QStringLiteral("<th>%1</th>").arg(model->headerData(column, Qt::Horizontal).toString().toHtmlEscaped());
        }
        html += QStringLiteral("</tr>");

        for (int row = 0; row < model->rowCount(); ++row) {
            html += QStringLiteral("<tr>");
            for (int column = 0; column < model->columnCount(); ++column) {
                const QModelIndex index = model->index(row, column);
                html += QStringLiteral("<td>%1</td>").arg(model->data(index, Qt::DisplayRole).toString().toHtmlEscaped());
            }
            html += QStringLiteral("</tr>");
        }

        html += QStringLiteral("</table></body></html>");

        QTextDocument document;
        document.setHtml(html);
        document.print(targetPrinter);
    });

    previewDialog.exec();
}
