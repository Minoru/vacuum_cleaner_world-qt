#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "formnewmap.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    scene = new QGraphicsScene();
    ui->graphicsView->setScene(scene);
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
    w = new World(fileName.toLocal8Bit().constData());

    if (w->getErrorMessage().isEmpty())
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
        QMessageBox::critical(this, tr("Error!"), w->getErrorMessage());

        return false;
    }
}

void MainWindow::DrawMap()
{
    scene->clear();

    for (int i = 0; i < w->getWorldHeight(); i++)
    {
        for (int j = 0; j < w->getWorldWidth(); j++)
        {
            QPen *pen = new QPen(QColor(130, 50, 10));
            QBrush *brush = new QBrush(QColor(130, 50, 10));

            if (w->getWorld().at(i)->at(j) != -1)
            {
                int color = 255 * (lifeTime - w->getWorld().at(i)->at(j)) / lifeTime;

                pen->setColor(QColor(color, color, color));
                brush->setColor(QColor(color, color, color));
            }

            scene->addRect(i * RECTANGLE_SIZE, j * RECTANGLE_SIZE, RECTANGLE_SIZE, RECTANGLE_SIZE, *pen, *brush);

            delete pen;
            delete brush;
        }
    }

    //TODO: draw a vacuum cleaner (simple green triangle)
    //TODO: scale map if it's bigger than graphicsView's size
}
