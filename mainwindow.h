#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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
    void on_pushButton_8_clicked();

    void on_pushButton_clicked();

public slots:
    void onNewMapData(QString filename, int lifetime, int testcase);

private:
    Ui::MainWindow *ui;

    QString fileName;       //.map file
    int lifeTime;
    int testCase;
};

#endif // MAINWINDOW_H
