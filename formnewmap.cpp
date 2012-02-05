#include "formnewmap.h"
#include "ui_formnewmap.h"
#include <QFileDialog>
#include <QMessageBox>

FormNewMap::FormNewMap(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormNewMap)
{
    ui->setupUi(this);
}

FormNewMap::~FormNewMap()
{
    delete ui;
}

//invokes "select file" dialog
void FormNewMap::on_selectMapFileButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
            tr("Select map file"), "map", tr("Map files (*.map)"));
    ui->mapFilenameEdit->setText(fileName);
}

//OK
void FormNewMap::on_okButton_clicked()
{
    int lifeTime = ui->lifetimeEdit->text().toInt();
    int testCase = ui->testCaseEdit->text().toInt();
    QString fileName = ui->mapFilenameEdit->text();

    if (lifeTime > 0 && testCase > 0 && QFile::exists(fileName))
    {
        //send SIGNAL to main window
        emit this->SendData(fileName, lifeTime, testCase);
        this->close();
    }
    else
        QMessageBox::information(this, tr("Error!"), tr("Invalid value(s)"));
}

//Cancel => close window
void FormNewMap::on_cancelButton_clicked()
{
    this->close();
}
