#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QPushButton>
#include"open62541.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QPushButton *btn3;//创建一个启动按钮，开启服务器进行通讯；
    QPushButton *btn4;//创建一个停止按钮，断开与服务器的通讯
    QPushButton *btn5;//创建一个配置按钮，按下之后可以modbus slva上读取得值映射到变量节点上
    QPushButton *ua_write_modbus_tcp;// //创建一个ua写入modbus(tcp）的启动按钮
    Ui::MainWindow *ui;//ui特有的，通过指针访问我所创建的类mainwindow
//  void doRtuQuery();
    void doTcpQuery();//创建tcp通讯的槽函数

private slots:
    void dealopen();//开始按钮的槽函数，里面包含子线程1：启动服务器
    void shutdown();//停止按钮的槽函数，将布尔变量置为false，停止服务器
    void dealmodbus();//创建子线程2的槽函数，实时读取modbus设备的值
    void dealmodbuswrite();
};
#endif // MAINWINDOW_H
