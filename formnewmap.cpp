#include "formnewmap.h"
#include "ui_formnewmap.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDesktopWidget>

FormNewMap::FormNewMap(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormNewMap)
{
    ui->setupUi(this);
    //Put the dialog in the screen center
    const QRect screen = QApplication::desktop()->screenGeometry();
    this->move( screen.center() - this->rect().center() );
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

void FormNewMap::on_lifetimeEdit_returnPressed()
{
    on_okButton_clicked();
}

void FormNewMap::on_mapFilenameEdit_returnPressed()
{
    on_selectMapFileButton_clicked();
}

void FormNewMap::on_testCaseEdit_returnPressed()
{
    on_okButton_clicked();
}
