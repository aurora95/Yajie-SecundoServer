#ifndef HANDLERBASE_H
#define HANDLERBASE_H
#include <ev++.h>
#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <queue>
#include "Settings.h"
#include "XMessage.h"
#include <mongo/bson/bson.h>
#include <mongo/client/dbclient.h>
template <typename MsgType>
class HandlerBase
{
    public:
        HandlerBase();
        virtual ~HandlerBase();
        void push(MsgType &msg);
        void MainWork();

        boost::thread *handlethread;
    protected:
        std::queue<MsgType> MessageQueue;
        virtual void HandleMessage(MsgType &msg);
};

template <typename MsgType>
HandlerBase<MsgType>::HandlerBase()
{

    handlethread = new boost::thread(boost::bind(&HandlerBase::MainWork, this));
}

template <typename MsgType>
HandlerBase<MsgType>::~HandlerBase()
{
    //dtor
}


template <typename MsgType>
void HandlerBase<MsgType>::push(MsgType &msg)
{
    MessageQueue.push(msg);
}

template <typename MsgType>
void HandlerBase<MsgType>::MainWork()
{
    while(1)
    {
        MsgType msg;
        if(!MessageQueue.empty())
        {
            msg = MessageQueue.front();
            MessageQueue.pop();
            HandleMessage(msg);
        }
    }
}

template <typename MsgType>
void HandlerBase<MsgType>::HandleMessage(MsgType &msg)
{

}


#endif // HANDLERBASE_H
