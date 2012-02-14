#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "formnewmap.h"
#include <QMessageBox>
#include <QTimer>

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
    {
        DrawMap();

        QRectF sceneRect = ui->graphicsView->sceneRect();
        ui->graphicsView->fitInView(sceneRect, Qt::KeepAspectRatio);
    }
}

bool MainWindow::LoadMap()
{
    /* FIXME: maybe toUtf8() is a better choise */
    w = new World(fileName.toLocal8Bit().constData());

    if (w->getErrorMessage().isEmpty())
    {
        currentRun = 1;
        totalDirtyDegree = 0;
        totalConsumedEnergy = 0;

        ui->doOneStepButton->setEnabled(true);
        ui->doOneRunButton->setEnabled(true);
        ui->doAllRunsButton->setEnabled(true);
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
                int dirtColor;
                if(dirt / dirtProb < lifeTime)
                    dirtColor = 255 * (lifeTime - dirt / dirtProb) / lifeTime;
                else
                    dirtColor = 0;

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

    RefreshStats();
}

void MainWindow::RefreshStats()
{
    if (w != NULL && w->getErrorMessage().isEmpty())
    {
        QString stats = QString("<table width=\"100%\"><tr>") +
                        tr("<td>Run %1, time step %2</td>") +
                        tr("<td>Completed runs: %3</td></tr><tr>") +
                        tr("<td>Action: %4</td>") +
                        tr("<td>Total dirty degree: %5</td></tr><tr>") +
                        tr("<td>Dirty degree: %6</td>") +
                        tr("<td>Total consumed energy: %7</td></tr><tr>") +
                        tr("<td>Consumed energy: %8</td>") +
                        tr("<td>Average dirty degree: %9</td></tr><tr>") +
                        tr("<td></td><td>Average consumed energy: %10</td>") +
                        QString("</tr></table>");

        QString currentAction;
        switch (w->getLastAgentAction())
        {
            case Agent::moveUp:
                currentAction = "Up";
            break;
            case Agent::moveDown:
                currentAction = "Down";
            break;
            case Agent::moveLeft:
                currentAction = "Left";
            break;
            case Agent::moveRight:
                currentAction = "Right";
            break;
            case Agent::suck:
                currentAction = "Suck";
            break;
            default:
                currentAction = "Idle";
            break;
        }

        currentAction.append((w->isJustBumped())?(QString(" Bump!")):(NULL));

        int completedRuns = currentRun;
        if(w->getCurrentTime() >= lifeTime) {
            completedRuns--;
        }

        double avgDirtyDegree = 0.0, avgConsumedEnergy = 0.0;
        if(completedRuns != 0) {
            avgDirtyDegree = totalDirtyDegree / completedRuns;
            avgConsumedEnergy = totalConsumedEnergy / completedRuns;
        }
        ui->statsLabel->setText(stats.
                                arg(currentRun).
                                arg(w->getCurrentTime()).
                                arg(completedRuns).
                                arg(currentAction).
                                arg(totalDirtyDegree, 10, 'f', 0).
                                arg(w->getDirtyDegree()).
                                arg(totalConsumedEnergy, 10, 'f', 0).
                                arg(w->getConsumedEnergy()).
                                arg(avgDirtyDegree, 7, 'f', 3).
                                arg(avgConsumedEnergy, 7, 'f', 3)
                                );
    }
    else
    {
        ui->statsLabel->setText("Map not loaded...");
    }
}

void MainWindow::on_doOneStepButton_clicked()
{
    w->doOneStep();
    ManageSituation();
}

void MainWindow::ManageSituation()
{
    if (w->getCurrentTime() >= lifeTime)
    {
        totalDirtyDegree += w->getDirtyDegree();
        totalConsumedEnergy += w->getConsumedEnergy();

        DrawMap();

        w->resetMap();
        ui->doOneStepButton->setEnabled(false);
        ui->doOneRunButton->setEnabled(false);
        ui->doAllRunsButton->setEnabled(false);
        ui->displayButton->setEnabled(false);

        if (currentRun >= testCase)
            ui->nextRunButton->setEnabled(false);
        else
            ui->nextRunButton->setEnabled(true);
    }
    else
        DrawMap();
}

void MainWindow::on_doOneRunButton_clicked()
{
    while (w->getCurrentTime() < lifeTime)
    {
        w->doOneStep();
    }

    ManageSituation();
}

void MainWindow::on_nextRunButton_clicked()
{
    currentRun++;
    DrawMap();
    ui->doOneStepButton->setEnabled(true);
    ui->doOneRunButton->setEnabled(true);
    ui->nextRunButton->setEnabled(false);
    ui->doAllRunsButton->setEnabled(true);
    ui->displayButton->setEnabled(true);
}

void MainWindow::on_doAllRunsButton_clicked()
{
    do
    {
        while (w->getCurrentTime() < lifeTime)
        {
            w->doOneStep();
        }

        totalDirtyDegree += w->getDirtyDegree();
        totalConsumedEnergy += w->getConsumedEnergy();

        if (currentRun < testCase)
        {
            w->resetMap();
        }
        currentRun++;

    } while (currentRun <= testCase);

    currentRun--;

    ui->doOneStepButton->setEnabled(false);
    ui->doOneRunButton->setEnabled(false);
    ui->nextRunButton->setEnabled(false);
    ui->doAllRunsButton->setEnabled(false);
    ui->displayButton->setEnabled(false);

    DrawMap();
}

void MainWindow::on_displayButton_clicked()
{
    int pause = ui->timeEdit->text().toInt();
    int steps = ui->stepsEdit->text().toInt();

    if (pause < 20 || pause > 10000)
    {
        QMessageBox::critical(this, tr("Error!"),
            tr("Time for one step must be over 0.02 and less than 10 secs"));
    }
    else if (steps < 1 || steps > lifeTime - w->getCurrentTime())
    {
        QMessageBox::critical(this, tr("Error!"),
            tr("Number of steps must be positive and less than remaining life time"));
    }
    else
    {
        ui->doOneStepButton->setEnabled(false);
        ui->doOneRunButton->setEnabled(false);
        ui->doAllRunsButton->setEnabled(false);
        ui->nextRunButton->setEnabled(false);
        ui->selectMapButton->setEnabled(false);
        ui->displayButton->setEnabled(false);

        int i = 0;
        while (w->getCurrentTime() < lifeTime && i < steps)
        {
            QTimer::singleShot((i+1) * pause, this, SLOT(onDrawOneStep()));

            i++;
        }

        QTimer::singleShot((i+1) * pause, this, SLOT(onRestore()));
    }
}

// Draw single step (required for visualization)
void MainWindow::onDrawOneStep()
{
    w->doOneStep();
    DrawMap();
}

// Enable buttons after ending of visualization
void MainWindow::onRestore()
{
    ui->doOneStepButton->setEnabled(true);
    ui->doOneRunButton->setEnabled(true);
    ui->doAllRunsButton->setEnabled(true);
    ui->selectMapButton->setEnabled(true);
    ui->displayButton->setEnabled(true);

    ManageSituation();
}

void MainWindow::resizeEvent(QResizeEvent * event)
{
    QRectF sceneRect = ui->graphicsView->sceneRect();
    ui->graphicsView->fitInView(sceneRect, Qt::KeepAspectRatio);
}

