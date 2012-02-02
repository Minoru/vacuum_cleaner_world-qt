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
void FormNewMap::on_toolButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select map file"), "map", tr("Map files (*.map)"));
    ui->lineEdit_3->setText(fileName);
}

//OK
void FormNewMap::on_pushButton_clicked()
{
    int lifeTime = ui->lineEdit->text().toInt();
    int testCase = ui->lineEdit_2->text().toInt();
    QString fileName = ui->lineEdit_3->text();

    if (lifeTime > 0 && testCase > 0 && QFile::exists(fileName))
    {
        emit this->SendData(fileName, lifeTime, testCase);          //send SIGNAL to main window
        this->close();
    }
    else
        QMessageBox::information(this, tr("Error!"), tr("Invalid value(s)"));
}

//Cancel => close window
void FormNewMap::on_pushButton_2_clicked()
{
    this->close();
}
