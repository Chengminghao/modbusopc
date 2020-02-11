#ifndef OPEN_THREAD_H
#define OPEN_THREAD_H

#include <QObject>
#include<QThread>

class open_thread : public QThread
{
    Q_OBJECT
public:

    explicit open_thread(QObject *parent = nullptr);

protected:
    //QThread的虚函数
    //子线程的入口
    //不能直接调用，需要通过start来间接调用
    void run();

signals:

};

#endif // OPEN_THREAD_H
