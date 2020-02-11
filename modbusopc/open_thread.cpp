#include "open_thread.h"
#include"open62541.h"

open_thread::open_thread(QObject *parent) : QThread(parent)
{

}
static volatile UA_Boolean running =true;
static void stopHandler(int sig)
{
    UA_LOG_INFO(UA_Log_Stdout,UA_LOGCATEGORY_USERLAND,"received crtl-c");
    running =false;
}

//开始创建run函数的实现
void QThread::run()
{
    signal(SIGINT, stopHandler);
    signal(SIGTERM, stopHandler);
    UA_Server *server = UA_Server_new();
    UA_ServerConfig_setDefault(UA_Server_getConfig(server));
    UA_StatusCode retval = UA_Server_run(server, &running);
    UA_Server_delete(server);

}
