#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "formnewmap.h"
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
    w = new World(fileName.toLocal8Bit().constData(), this);

    if (w->isCreated())
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
    QGraphicsScene* scene = new QGraphicsScene();
    ui->graphicsView->setScene(scene);

    const int rectSize = 40;                    //TODO: move it to class header

    for (int i = 0; i < w->getWorldHeight(); i++)
    {
        for (int j = 0; j < w->getWorldWidth(); j++)
        {
            QPen pen(QColor(130, 50, 10));              //TODO: change pen color depending on it's road or dirty road
            QBrush brush(QColor(130, 50, 10));          //TODO: similar with brush color
            scene->addRect(i * rectSize, j * rectSize, rectSize, rectSize, pen, brush);
        }
    }

    //TODO: draw a vacuum cleaner (simple green triangle)
    //TODO: scale map if it's bigger than graphicsView's size
}
