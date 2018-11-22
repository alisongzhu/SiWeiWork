#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QTimer>
#include <QMessageBox>
#include <string.h>
#include <QString>
enum {
      ReadParaCmd=0x00000001,
      WriteParaCmd,
      CheckParaCmd,
      DevRestCmd,
      ReadDataCmd
 }CommandCode;
uint64_t ReadSize=0;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    foreach (const QSerialPortInfo &info,QSerialPortInfo::availablePorts())
    {
        QSerialPort serial;
        serial.setPort(info);
        if(serial.open(QIODevice::ReadWrite));
        {
            ui->PortBox->addItem(serial.portName());
        }

    }
    ui->ReadParaButton->setEnabled(false);
    ui->ReadParaButton->setEnabled(false);
    ui->writeParaButton->setEnabled(false);
    ui->ReadDatapushButton->setEnabled(false);
    ui->CheckpushButton->setEnabled(false);
    ui->SendpushButton->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_openButton_clicked()
{
    if(ui->openButton->text()==tr("打开串口"))
        {
            serial = new QSerialPort;
            //设置串口名
            serial->setPortName(ui->PortBox->currentText());
            //打开串口
            serial->open(QIODevice::ReadWrite);
            //设置波特率
            serial->setBaudRate(ui->BaudBox->currentText().toInt());
            //设置数据位数
            serial->setDataBits(QSerialPort::Data8);
             //设置奇偶校验
            serial->setParity(QSerialPort::NoParity);

            //设置停止位
            serial->setStopBits(QSerialPort::OneStop);

            //设置流控制
            serial->setFlowControl(QSerialPort::NoFlowControl);
            //关闭设置菜单使能
            ui->PortBox->setEnabled(false);
            ui->BaudBox->setEnabled(false);
            ui->ReadParaButton->setEnabled(true);
            ui->writeParaButton->setEnabled(false);
            ui->ReadDatapushButton->setEnabled(true);
            ui->CheckpushButton->setEnabled(true);
            ui->SendpushButton->setEnabled(true);
            ui->openButton->setText(tr("关闭串口"));
           // ui->sendButton->setEnabled(true);
            //连接信号槽
           // QObject::connect(serial, &QSerialPort::readyRead, this, &MainWindow::Read_Data);
        }
        else
        {
            //关闭串口
            serial->clear();
            serial->close();
            serial->deleteLater();
            //恢复设置使能
            ui->PortBox->setEnabled(true);
            ui->BaudBox->setEnabled(true);
            ui->openButton->setText(tr("打开串口"));
            ui->ReadParaButton->setEnabled(false);
            ui->ReadParaButton->setEnabled(false);
            ui->writeParaButton->setEnabled(false);
            ui->ReadDatapushButton->setEnabled(false);
            ui->CheckpushButton->setEnabled(false);
            ui->SendpushButton->setEnabled(false);
            //ui->sendButton->setEnabled(false);
        }

}
void MainWindow::Read_Data()
{
    QByteArray buf;
         buf = serial->readAll();
        if(!buf.isEmpty())
        {
            QString str ;
            str+=tr(buf);
            ui->textBrowser->clear();
            ui->textBrowser->append(str);
             ui->textBrowser->setText(buf.split(1).at(0));
        }
        buf.clear();

}

void MainWindow::Read_Para()
{

    timer->stop();
    QByteArray buf;
    buf=serial->readAll();


     ReadSize+=buf.size();
     RxData.append( buf);
     if(ReadSize>=sizeof(struct sSystemProtcolFrame))
     {
         memcpy((char*)&Rxdata,RxData,sizeof(struct sSystemProtcolFrame));
         QByteArray   temp;
         uint8_t Dat[12]={0};
//       temp.setRawData((char*)&Rxdata,25);
//       serial->write(temp,25);
         temp.clear();

         memcpy(Dat,Rxdata.destinationAddress,12);
         temp.setRawData((char*)Dat,12);
//         temp.append(QString::number(Rxdata.destinationAddress[0], 16));
//         temp.append(QString::number(Rxdata.destinationAddress[1] ,16));
//         temp.append(QString::number(Rxdata.destinationAddress[2] ,16));
         ui->destinationAddrlineEdit->clear();
         ui->destinationAddrlineEdit->setText(temp.toHex().data());
         temp.clear();
//         temp.append(QString::number(Rxdata.SourceAddress[0] , 16));
//         temp.append(QString::number(Rxdata.SourceAddress[1] , 16));
//         temp.append(QString::number(Rxdata.SourceAddress[2] , 16));
          memcpy((char*)Dat,(char*)Rxdata.SourceAddress,12);
          temp.setRawData((char*)Dat,12);
         ui->SourceAddrlineEdit->clear();
         ui->SourceAddrlineEdit->setText(temp.toHex().data());
         ui->textBrowser->append( RxData.toHex().data());
         RxData.clear();
         ReadSize=0;
         disconnect(serial, &QSerialPort::readyRead, this, &MainWindow::Read_Para);
     }
    ui->ReadParaButton->setEnabled(true);
    ui->writeParaButton->setEnabled(true);
    ui->ReadDatapushButton->setEnabled(true);
    ui->CheckpushButton->setEnabled(true);
    ui->SendpushButton->setEnabled(true);
    ui->textBrowser->append(QString::number(ReadSize, 10));


}
void MainWindow::Comm_TimerOut()
{
     QMessageBox *Mess=new QMessageBox(this);
     Mess->warning(this, "错误","通信超时，请重试");
     ReadSize=0;
     ui->ReadParaButton->setEnabled(true);
     ui->writeParaButton->setEnabled(true);
     ui->ReadDatapushButton->setEnabled(true);
     ui->CheckpushButton->setEnabled(true);
     ui->SendpushButton->setEnabled(true);
     disconnect(timer , &QTimer::timeout, this, &MainWindow::Comm_TimerOut);
     disconnect(serial, &QSerialPort::readyRead, this, &MainWindow::Read_Para);
     disconnect(serial, &QSerialPort::readyRead, this, &MainWindow::Check_Para);
//     if ( timer->isActive() )
//         timer->stop();

}




void MainWindow::on_ReadParaButton__clicked()
{
    memset(&Txdata.destinationAddress,0xFF,sizeof(Txdata.destinationAddress));
    memset(&Txdata.SourceAddress,0xFA,sizeof(Txdata.SourceAddress));
    Txdata.Command=ReadParaCmd;
   // memcpy(Txdata.destinationAddress,ReadPara,12);
    QByteArray   temp;
    temp.setRawData((char*)&Txdata,28);
    serial->write(temp,28);
    connect(serial, &QSerialPort::readyRead, this, &MainWindow::Read_Para);
    timer=new QTimer(this);
    timer->setSingleShot(true);
    timer->start(Comm_TimerOUT );
    connect(timer , &QTimer::timeout, this, &MainWindow::Comm_TimerOut);

}

void MainWindow::on_ReadParaButton_clicked()
{
    memset(&Txdata.destinationAddress,0xFF,sizeof(Txdata.destinationAddress));
    memset(&Txdata.SourceAddress,0xFA,sizeof(Txdata.SourceAddress));
    Txdata.Command=ReadParaCmd;
   // memcpy(Txdata.destinationAddress,ReadPara,12);
    QByteArray   temp;
    temp.setRawData((char*)&Txdata,28);
    serial->write(temp,28);
    RxData.clear();
    temp=serial->readAll();
    connect(serial, &QSerialPort::readyRead, this, &MainWindow::Read_Para);
    timer=new QTimer(this);
    timer->setSingleShot(true);
    timer->start(Comm_TimerOUT );
    connect(timer , &QTimer::timeout, this, &MainWindow::Comm_TimerOut);
    ui->ReadParaButton->setEnabled(false);
    ui->writeParaButton->setEnabled(false);
    ui->ReadDatapushButton->setEnabled(false);
    ui->CheckpushButton->setEnabled(false);
    ui->SendpushButton->setEnabled(false);

}

void MainWindow::on_CheckpushButton_clicked()
{
    memset(&Txdata.destinationAddress,0xFF,sizeof(Txdata.destinationAddress));
    memset(&Txdata.SourceAddress,0xFA,sizeof(Txdata.SourceAddress));
    Txdata.Command=CheckParaCmd;
   // memcpy(Txdata.destinationAddress,ReadPara,12);
    QByteArray   temp;
    temp.setRawData((char*)&Txdata,28);
    serial->write(temp,28);
    RxData.clear();
    temp=serial->readAll();
    connect(serial, &QSerialPort::readyRead, this, &MainWindow::Check_Para);
    timer=new QTimer(this);
    timer->setSingleShot(true);
    timer->start(Check_TimerOUT );
    connect(timer , &QTimer::timeout, this, &MainWindow::Comm_TimerOut);
    ui->ReadParaButton->setEnabled(false);
    ui->writeParaButton->setEnabled(false);
    ui->ReadDatapushButton->setEnabled(false);
    ui->CheckpushButton->setEnabled(false);
    ui->SendpushButton->setEnabled(false);
}

void MainWindow::Check_Para()
{
    timer->stop();
    QByteArray buf;
    buf=serial->readAll();
    if( buf.contains("OK"));
    ui->textBrowser->append( "OK");
    ui->ReadParaButton->setEnabled(true);
    ui->writeParaButton->setEnabled(true);
    ui->ReadDatapushButton->setEnabled(true);
    ui->CheckpushButton->setEnabled(true);
    ui->SendpushButton->setEnabled(true);
}

void MainWindow::on_writeParaButton_clicked()
{
    memcpy(Txdata.destinationAddress,Rxdata.SourceAddress,12);
    memset(&Txdata.SourceAddress,0xFA,sizeof(Txdata.SourceAddress));
    Txdata.Command=WriteParaCmd;
   // memcpy(Txdata.destinationAddress,ReadPara,12);
    QByteArray   temp;
    temp.setRawData((char*)&Txdata,sizeof(struct sSystemProtcolFrame));
    serial->write(temp,sizeof(struct sSystemProtcolFrame));
}

void MainWindow::on_ClearpushButton_clicked()
{
    ui->textBrowser->clear();

}


