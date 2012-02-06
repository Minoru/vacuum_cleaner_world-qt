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
        //ui->doOneRunButton->setEnabled(true);
        //ui->doAllRunsButton->setEnabled(true);
        //ui->nextRunButton->setEnabled(true);
        //ui->displayButton->setEnabled(true);

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
                /* We need to display amount of dirt in each cell somehow.
                 * GLUT version of GUI greyed cells depending on dirtinnes, so
                 * do we, although we would use quite more sophisticated
                 * algorithm. It is based on a few facts:
                 * *  one run consists of 'lifeTime' steps;
                 * *  at each step, we can increase dirtiness of each cell by
                 *    one;
                 * *  probability of dirtiness to increase is
                 *    'dirtyProbability'.
                 * Now here's how we'll put those facts to work. We can be
                 * absolutely sure that cell won't ever be dirtier than
                 * 'lifeTime', because that's how much possibilies we've got to
                 * increase cell's dirtiness. Statistically, dirtiness of any
                 * cell can't be more that 'lifetime' * 'dirtyProbability'.
                 * 
                 * Given all that, the following algorithm follows naturally.
                 */
                int dirt = w->getWorld().at(i)->at(j);
                float dirtProb = w->getDirtyProbability();
                int dirtColor = 255;
                if(dirt / dirtProb < lifeTime)
                    dirtColor *= (lifeTime - dirt / dirtProb) / lifeTime;

                pen->setColor(QColor(dirtColor, dirtColor, dirtColor));
                brush->setColor(QColor(dirtColor, dirtColor, dirtColor));
            }

            scene->addRect(i * RECTANGLE_SIZE, j * RECTANGLE_SIZE,
                RECTANGLE_SIZE, RECTANGLE_SIZE, *pen, *brush);

            delete pen;
            delete brush;
        }
    }

    QVector<QPoint> triangle;
    QColor color(0, 255, 0);
    if (w->isJustBumped())
        color.setRgb(255, 0, 0);

    //FIXME: this holy shit must be simpified
    Agent::actions action = w->getLastAgentAction();
    QPen pen(color);
    QBrush brush(color);
    int posX = w->getAgentPosX() * RECTANGLE_SIZE,
        posY = w->getAgentPosY() * RECTANGLE_SIZE;
    switch (action)
    {
        case Agent::idle:
            /* Don't need to do anything - all the colors are set up already */
        break;

        case Agent::suck:
            /* box should be yellow */
            pen.setColor(QColor(255, 255, 0));
            brush.setColor(QColor(255, 255, 0));
        break;

        case Agent::moveUp:
            triangle.push_back(QPoint(posX + RECTANGLE_SIZE / 2,
                                      posY + RECTANGLE_SIZE / 10));
            triangle.push_back(QPoint(posX + RECTANGLE_SIZE / 10,
                                      posY + RECTANGLE_SIZE * 9 / 10));
            triangle.push_back(QPoint(posX + RECTANGLE_SIZE * 9 / 10,
                                      posY + RECTANGLE_SIZE * 9 / 10));

        break;

        case Agent::moveDown:
            triangle.push_back(QPoint(posX + RECTANGLE_SIZE / 2,
                                      posY + RECTANGLE_SIZE * 9 / 10));
            triangle.push_back(QPoint(posX + RECTANGLE_SIZE / 10,
                                      posY + RECTANGLE_SIZE / 10));
            triangle.push_back(QPoint(posX + RECTANGLE_SIZE * 9 / 10,
                                      posY + RECTANGLE_SIZE / 10));
        break;

        case Agent::moveLeft:
            triangle.push_back(QPoint(posX + RECTANGLE_SIZE / 10,
                                      posY + RECTANGLE_SIZE / 2));
            triangle.push_back(QPoint(posX + RECTANGLE_SIZE * 9 / 10,
                                      posY + RECTANGLE_SIZE / 10));
            triangle.push_back(QPoint(posX + RECTANGLE_SIZE * 9 / 10,
                                      posY + RECTANGLE_SIZE * 9 / 10));
        break;

        case Agent::moveRight:
            triangle.push_back(QPoint(posX + RECTANGLE_SIZE * 9 / 10,
                                      posY + RECTANGLE_SIZE / 2));
            triangle.push_back(QPoint(posX + RECTANGLE_SIZE / 10,
                                      posY + RECTANGLE_SIZE / 10));
            triangle.push_back(QPoint(posX + RECTANGLE_SIZE / 10,
                                      posY + RECTANGLE_SIZE * 9 / 10));
        break;

        default:
            //do nothing, cuz this situation cannot happen :)
        break;
    }

    if(action == Agent::idle || action == Agent::suck)
        scene->addRect(posX + RECTANGLE_SIZE / 10,
                       posY + RECTANGLE_SIZE / 10,
                       RECTANGLE_SIZE * 4 / 5, RECTANGLE_SIZE * 4 / 5,
                       pen, brush);
    else
        scene->addPolygon(QPolygon(triangle), pen, brush);

    //TODO: scale map if it's bigger than graphicsView's size
}

void MainWindow::on_doOneStepButton_clicked()
{
    w->doOneStep();

    DrawMap();

    if (w->getCurrentTime() >= lifeTime)
    {
        w->resetMap();
        ui->doOneStepButton->setEnabled(false);
    }
}
