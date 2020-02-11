#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QMessageBox>
#include<QPushButton>
#include"../../libmodbus/src/modbus.h"
#include<iostream>
using namespace  std;
#include"open62541.h"
#include<signal.h>
#include<stdlib.h>

#include<QDebug>


UA_Boolean running =true;//全局布尔值变量，控制服务器的运行与停止，联合启动和停止按钮使用
uint16_t a=0;//全局变量a，将子线程2中读到的modbus值返回到变量a中，再利用open函数将a写岛opc变量节点上
bool set_status=true;//实时读取和写入数据的实时布尔值


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //创建按钮，点击返回读取寄存器的个数
    QPushButton *btn1=new QPushButton("tcp_read_numers",this);
    btn1->resize(200,50);
    connect(btn1,&QPushButton::clicked,this,&MainWindow::doTcpQuery);//基本不用看了，这个槽函数没什么用
    //创建按钮，点击返回一个寄存器的值
    QPushButton *btn2=new QPushButton("tcp_read_value",this);
    btn2->resize(200,50);
    btn2->move(0,50);
    connect(btn2,&QPushButton::clicked,this,&MainWindow::dealmodbus);

    //开始添加启动和停止按钮
    btn3=new QPushButton("启动",this);
    btn3->resize(200,50);
    btn3->move(0,100);

    btn4=new QPushButton("停止",this);
    btn4->resize(200,50);
    btn4->move(0,150);

    //点击启动按钮，创建槽函数开启子线程
    connect(btn3,&QPushButton::clicked,this,&MainWindow::dealopen);
    //点击停止按钮，退出连接
    connect(btn4,&QPushButton::clicked,this,&MainWindow::shutdown);

    //添加配置按钮，完成一个特定值的映射
    btn5=new QPushButton("配置",this);
    btn5->resize(200,50);
    btn5->move(0,200);

    //创建ua向modbus tcp写入数据的启动按钮，同时关闭modbus实时读取，采用异步通信
    ua_write_modbus_tcp=new QPushButton("tcp_write_value",this);
    ua_write_modbus_tcp->resize(200,50);
    ua_write_modbus_tcp->move(200,50);
    connect(ua_write_modbus_tcp,&QPushButton::clicked,this,&MainWindow::dealmodbuswrite);
}


MainWindow::~MainWindow()
{
    running =0;
    delete ui;

}
//添加a变量，并通过回调实现周期的采集数据
static void update_modbus_value(UA_Server *server)
{       set_status=true;
        UA_Variant value;
        UA_Variant_setScalar(&value,&a,&UA_TYPES[UA_TYPES_INT32]);
        UA_NodeId myIegerNodeId =UA_NODEID_STRING(1,"the.answer");       
        UA_Server_writeValue(server,myIegerNodeId,value);



        //每次将值写入节点得时候，数据更新一次
//通过上面这个函数得测试，得出想法：重新创建一个线程，添加变量，利用回调（回调可以直接利用已有的callback），通过readvalue实时读取节点上得值

}
//添加存取ua写入的变量，然后放到回调函数中，实时更新
static void update_modbus_read_value(UA_Server *server)
{
        UA_Variant read_value;
        UA_NodeId myIegerNodeId=UA_NODEID_STRING(1,"the.answer");
        int check=UA_Server_readValue(server,myIegerNodeId,&read_value);
        if(check==0)
        {
            cout<<"read_value success"<<endl;
        }
        else
        {
            cout<<"read_value failed"<<endl; //读取不成功！
        }
        uint16_t *k=NULL;
        k=(uint16_t*)read_value.data;
        cout<<(*k)<<endl;//这个用法是对的，但是该函数不应该在addVariable中更新，因为这样读到的值永远都为a的初始值，
        //该函数这样用只是在开始的时候被调用了一次，然后就定死了，不能够实时地读取opc上节点的值
//想法：点击tcp_modbus_write按钮的时候，跳转到槽函数，明确需求：该节点和Modbus异步通信，并且opc在写入模式下可以实时更改modbus的值
//so:槽函数首先要关闭modbus向opc传输值（通过布尔值实现），其次，涉及到实时写入，工作量大，需要创建子线程
//last:在子线程中，利用while循环，间隔一定的时间就执行读取操作，并将读到的值通过libmodbus函数写入到modbus slave中
//如果只能读取一次的话，while实现不了，再试试回调函数

}
//为server添加变量节点

static void addVariable(UA_Server *server)
{
    /* Define the attribute of the myInteger variable node */
    UA_VariableAttributes attr = UA_VariableAttributes_default;

    UA_Variant_setScalar(&attr.value, &a, &UA_TYPES[UA_TYPES_INT32]);
    attr.description = UA_LOCALIZEDTEXT("en-US","the answer");
    attr.displayName = UA_LOCALIZEDTEXT("en-US","the answer");
    attr.dataType = UA_TYPES[UA_TYPES_INT32].typeId;
    attr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;

    /* Add the variable node to the information model */
    UA_NodeId myIntegerNodeId = UA_NODEID_STRING(1, "the.answer");
    UA_QualifiedName myIntegerName = UA_QUALIFIEDNAME(1, "the answer");
    UA_NodeId parentNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
    UA_NodeId parentReferenceNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES);
    UA_Server_addVariableNode(server, myIntegerNodeId, parentNodeId,
                              parentReferenceNodeId, myIntegerName,
                              UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), attr, NULL, NULL);


    update_modbus_value(server);//手动更新一次变量a得值,没有得话，从modbus得值无法写入opc节点a上，opc节点为默认
    update_modbus_read_value(server);


}
//开始创建回调函数:作用就是可以实时更新从Modbus上读取到的变量a
static void beforereadmodbus(UA_Server *server,
                             const UA_NodeId *sessionId,
                             void *sessionContext,
                             const UA_NodeId *nodeId,
                             void *nodeContext,
                             const UA_NumericRange *range,
                             const UA_DataValue *data){   
    if(set_status==1)
     {
        update_modbus_value(server);
     }

}
static void afterreadmodbus(UA_Server *server,
                            const UA_NodeId *sessionId,
                            void *sessionContext,
                            const UA_NodeId *nodeId,
                            void *nodeContext,
                            const UA_NumericRange *range,
                            const UA_DataValue *data){
    UA_LOG_INFO(UA_Log_Stdout,UA_LOGCATEGORY_USERLAND,
                "tHE VARIABLE WAS UPDATED");
}
static void addmodbusvaluebacktoaddvariable(UA_Server *server)
{
    UA_NodeId myIntegerNodeId = UA_NODEID_STRING(1, "the.answer");
    UA_ValueCallback callback ;

    callback.onRead=beforereadmodbus;
    callback.onWrite=afterreadmodbus;
    UA_Server_setVariableNode_valueCallback(server,myIntegerNodeId,callback);
}
//回调函数创建结束


//开始创建子线程函数1：启动服务器
void *version_manage_thread(void *arg) //监听4840端口的服务器子线程
{
    running = true;

    UA_Server *server=UA_Server_new();
    UA_ServerConfig_setDefault(UA_Server_getConfig(server));

    addVariable(server); //添加节点 id名为（1，the.answer)   
    addmodbusvaluebacktoaddvariable(server);
    UA_StatusCode retval = UA_Server_run(server, &running);
    UA_Server_delete(server);
    return NULL;
}

//开始创建子线程函数2
void *threadrun(void *arg)
{
    set_status=true;//这个作用是当读取Modbus值时候，想写入完值，通过这个布尔值转换可以再次开始读取
    modbus_t *ctx;
    uint16_t tab[20]={0};   
    ctx=modbus_new_tcp("127.0.0.1",502);
    modbus_set_slave(ctx,2);
    int status=modbus_connect(ctx);
    if(status==0)
    {
        while(set_status)
        {
            int modbus_read_num=modbus_read_registers(ctx,0,1,tab);
            a=tab[0];
            cout<<a<<endl;
            Sleep(1000);
            if(modbus_read_num!=1)
            {
                set_status=false;
                cout<<"modbus slave has problem,please have a check!"<<endl;
            }
        }
    }
    else{
        set_status=false;
        cout<<"modbus_connect failed"<<endl;
    }
}
//创建子线程函数3，实现ua节点实时读取，并写入Modbus中
void *tcp_ua_write( void *arg)
{
//        UA_Server *server;
//        UA_Variant read_value;
//        UA_NodeId myIegerNodeId=UA_NODEID_STRING(1,"the.answer");

//        int check=UA_Server_readValue(server,myIegerNodeId,&read_value);
//        if(check==0)
//        {
//            cout<<"read_value success!"<<endl;
//        }
//        else
//        {
//            cout<<"read_value failed!"<<endl;
//        }
//        uint16_t *k;
//        k=(uint16_t*)read_value.data;
//        cout<<*k<<endl;
    cout<<"aaa"<<endl;

}


//创建dealopen函数，开启服务器，创建子线程1
void MainWindow::dealopen()
{


    pthread_t version_manage_thread_t;

    if(pthread_create(&version_manage_thread_t, NULL, version_manage_thread, NULL) == 0)
    {
        printf("version_manage thread create success\n");
    }
    else
    {
        printf("version_manage thread create error\n");
        exit(1);
    }

}

//创建modbus_thread函数，实时读取Modbus的值，运行在子线程2中
void MainWindow::dealmodbus()
{
    pthread_t thread2;
    if (pthread_create(&thread2,NULL,threadrun,NULL)==0)
    {
        printf("thread2 create success\n");
    }
    else
    {
        printf("thread 2 create failed\n");
        exit(1);
    }
}
//创建modbus tcp连接，并且创建modbus tcp写入的子线程3：ua写入值到modbus slave上
//先连接槽函数，点击这个按钮实现停止modbus向ua的实时读取
void MainWindow::dealmodbuswrite()
{
    set_status=false;
    pthread_t thread_write;
    if(pthread_create(&thread_write,NULL,tcp_ua_write,NULL)==0)
    {
        printf("thread_write create success\n");
    }
    else
    {
        printf("thread_write create failed\n");
        exit(1);
    }

}



//创建关闭按钮的槽函数
void MainWindow::shutdown()
{
    running =false;
    set_status=false;
}



//创建modbus tcp连接，读取寄存器的个数
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








