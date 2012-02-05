#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "world.h"
#include <QGraphicsScene>


namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_quitButton_clicked();

    void on_selectMapButton_clicked();

    void on_doOneStepButton_clicked();

public slots:
    void onNewMapData(QString filename, int lifetime, int testcase);

private:
    Ui::MainWindow *ui;

    bool LoadMap();
    void DrawMap();

    QString fileName;       //.map file
    int lifeTime;
    int testCase;

    World *w;
    QGraphicsScene* scene;  //graphics scene

    static const int RECTANGLE_SIZE = 40;
};

#endif // MAINWINDOW_H
