#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QTimer>
#include "communictioanframe.h"
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
    void on_openButton_clicked();
    void Read_Data();
    void Read_Para();
    void Comm_TimerOut();
    void Check_Para();





    void on_ReadParaButton__clicked();

    void on_ReadParaButton_clicked();

    void on_CheckpushButton_clicked();

    void on_writeParaButton_clicked();

    void on_ClearpushButton_clicked();

    void on_BaudBox_activated(const QString &arg1);

private:
    Ui::MainWindow *ui;
    QSerialPort *serial;
    struct sSystemProtcolFrame Rxdata;
    struct sSystemProtcolFrame Txdata;
    QTimer *timer;
    QByteArray RxData;
};

#endif // MAINWINDOW_H
