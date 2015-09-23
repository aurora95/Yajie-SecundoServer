#ifndef XHANDLER_H
#define XHANDLER_H

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
#include "XBroadcaster.h"
#include "XUserInitializer.h"
#include "HandlerBase.h"
#include <mongo/bson/bson.h>
#include <mongo/client/dbclient.h>


class XHandler:public HandlerBase<XMessage>
{
    public:
        XHandler();
        virtual ~XHandler();
        int parse_and_push(char buf[], int fd);

    protected:
    private:
        void HandleMessage(XMessage &msg);

        void setWords(XMessage &msg);
        void setLocation(XMessage &msg);
        void setUser(XMessage &msg);
        Coordinate getLocation(int user_id);
        void Broadcast(XMessage &bmsg, Coordinate center);
        void SendInitInfo(int fd, Coordinate center);
        void CloseClient(int fd);

        XBroadcaster broadcaster;
        XUserInitializer initializer;
};

#endif // XHANDLER_H
