#ifndef FORMNEWMAP_H
#define FORMNEWMAP_H

#include <QWidget>

namespace Ui {
    class FormNewMap;
}

class FormNewMap : public QWidget
{
    Q_OBJECT

public:
    explicit FormNewMap(QWidget *parent = 0);
    ~FormNewMap();

private slots:
    void on_toolButton_clicked();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::FormNewMap *ui;
signals:
    void SendData(QString fileName, int lifeTime, int testCase);        //send data to main window
};

#endif // FORMNEWMAP_H
