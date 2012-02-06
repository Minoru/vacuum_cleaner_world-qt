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
    void on_selectMapFileButton_clicked();

    void on_okButton_clicked();

    void on_cancelButton_clicked();

private:
    Ui::FormNewMap *ui;
signals:
    //send data to main window
    void SendData(QString fileName, int lifeTime, int testCase);
};

#endif // FORMNEWMAP_H
