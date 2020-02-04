#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QMessageBox>
#include<QPushButton>
#include"../../libmodbus/src/modbus.h"
#include<iostream>
using namespace  std;



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //创建按钮，点击返回读取寄存器的个数
    QPushButton *btn1=new QPushButton("tcp_read_numers",this);
    btn1->resize(200,50);
    connect(btn1,&QPushButton::clicked,this,&MainWindow::doTcpQuery);
    //创建按钮，点击返回一个寄存器的值
    QPushButton *btn2=new QPushButton("tcp_read_value",this);
    btn2->resize(200,50);
    btn2->move(0,50);
    connect(btn2,&QPushButton::clicked,[=](){
        modbus_t *ctx;
        uint16_t tab[20]={0};
        ctx=modbus_new_tcp("127.0.0.1",502);
        modbus_set_slave(ctx,2);
        int status=modbus_connect(ctx);
        modbus_read_registers(ctx,0,1,tab);
        uint16_t a=tab[0];
        if(status==0)
        {
            QMessageBox box;
            QMessageBox::about(NULL, "report", QString("value is %1").arg(a));
            cout<<tab[0]<<endl;
        }
        else{
            QMessageBox box;
            box.setText("read failed");
            box.exec();
        }
    });

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::doTcpQuery()
{
    modbus_t *mb;
    uint16_t tab_reg[32]={0};
    mb = modbus_new_tcp("127.0.0.1", 502);
    modbus_set_slave(mb, 2);
    int status=modbus_connect(mb);
    int regs=modbus_read_registers(mb, 0, 1, tab_reg);
    if(status==0)
    {
    QMessageBox box;
    box.resize(100,70);
    QMessageBox::about(NULL, "report", QString("Tcp读取寄存器的个数:%1").arg(regs));
    }
    else{
        QMessageBox box;
        box.setText("connectted failed");
        box.exec();
    }
    modbus_close(mb);
    modbus_free(mb);

}

