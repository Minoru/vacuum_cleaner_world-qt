#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "formnewmap.h"
#include "world.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//Quit
void MainWindow::on_quitButton_clicked()
{
    this->close();
}

//Select map
void MainWindow::on_selectMapButton_clicked()
{
    FormNewMap* form = new FormNewMap(0);

    connect(form, SIGNAL(SendData(QString, int, int)),
                         this, SLOT(onNewMapData(QString, int, int)));

    form->show();
}

//SLOT, receives data from "select map" form
void MainWindow::onNewMapData(QString filename, int lifetime, int testcase)
{
    fileName = filename;
    lifeTime = lifetime;
    testCase = testcase;

    if (LoadMap())
        DrawMap();
}

bool MainWindow::LoadMap()
{
    /* FIXME: maybe toUtf8() is a better choise */
    World w(fileName.toLocal8Bit().constData(), this);

    if (w.isCreated())
    {
        ui->doOneStepButton->setEnabled(true);
        ui->doOneRunButton->setEnabled(true);
        ui->doAllRunsButton->setEnabled(true);
        ui->nextRunButton->setEnabled(true);
        ui->displayButton->setEnabled(true);

        return true;
    }
    else
    {
        //FIXME: isCreated() is anal strut
        //FIXME: it will be better to create "QString errorMessage" in "World" class and if it isn't empty output here an error

        return false;
    }
}

void MainWindow::DrawMap()
{
    //TODO: draw a map
}
