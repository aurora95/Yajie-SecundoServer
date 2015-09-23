#ifndef BROADCASTER_H
#define BROADCASTER_H

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

struct bcMessage
{
    XMessage msg;
    Coordinate center;
};

class XBroadcaster:public HandlerBase<bcMessage>
{
    public:
        XBroadcaster();
        virtual ~XBroadcaster();

    protected:
    private:
        void HandleMessage(bcMessage &bcmsg);
};

#endif // BROADCASTER_H
