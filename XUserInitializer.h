#ifndef XUSERINITIATER_H
#define XUSERINITIATER_H

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
#include "HandlerBase.h"
#include <mongo/bson/bson.h>
#include <mongo/client/dbclient.h>

struct InitStruct
{
    int fd;
    Coordinate loc;
};


class XUserInitializer:public HandlerBase<InitStruct>
{
    public:
        XUserInitializer();
        virtual ~XUserInitializer();

    protected:
    private:
        void HandleMessage(InitStruct &init);
};

#endif // XUSERINITIATER_H
