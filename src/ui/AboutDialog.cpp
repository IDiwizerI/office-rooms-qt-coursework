#include "AboutDialog.h"
#include "ui_AboutDialog.h"

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    setupText();
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::setupText()
{
    setWindowTitle(tr("About"));
    ui->titleLabel->setText(tr("Office Rooms"));
    ui->courseworkLabel->setText(tr("Coursework: Office premises accounting software for a construction company."));
    ui->instituteTitleLabel->setText(tr("Institute:"));
    ui->instituteValueLabel->setText(tr("Institute of Digital Technologies for Modeling in Construction"));
    ui->courseTitleLabel->setText(tr("Course:"));
    ui->courseValueLabel->setText(tr("Second year"));
    ui->groupTitleLabel->setText(tr("Group:"));
    ui->groupValueLabel->setText(tr("ICTMS-II-101"));
    ui->authorTitleLabel->setText(tr("Author:"));
    ui->authorValueLabel->setText(tr("Shavaliev Amir Ilmirovich"));
}
