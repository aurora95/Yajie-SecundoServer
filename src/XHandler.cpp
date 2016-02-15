#include "XHandler.h"
#include "XBroadcaster.h"
#include "HandlerBase.h"
#include "XUserInitializer.h"
#include "Settings.h"
#include "User.h"
#include <limits.h>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <mongo/bson/bson.h>
#include <mongo/client/dbclient.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <string>
#include <sstream>
using namespace std;
using namespace boost::property_tree;

XHandler::XHandler():HandlerBase()
{
}

XHandler::~XHandler()
{
    //dtor
}

int XHandler::parse_and_push(char buf[], int fd)
{
    XMessage msg;
    msg.fromJSON(buf);

    if(msg.msg_type == XMSG_INIT)
        msg.user_id = fd;
    else if(msg.user_id != fd)
    {
        msg.user_id = fd;
        TESTINFO("wrong user_id!\n");
    }
    if(msg.msg_type & LEGAL_XMSG)
    {
        MessageQueue.push(msg);
        TESTINFO("pushed message\n");
        return 0;
    }
    else
    {
        perror("message type error!");
        return 1;
    }
}

void XHandler::HandleMessage(XMessage &msg)
{
    switch(msg.msg_type)
    {
    case XMSG_SPEAK:
        TESTINFO("msg speak\n");
        User::setWords(msg);
        Broadcast(msg, User::getLocation(msg.user_id));
        break;

    case XMSG_MOVE:
        TESTINFO("msg move\n");
        User::setLocation(msg);
        Broadcast(msg, msg.msgdata.loc);
        break;

    case XMSG_INIT:
        TESTINFO("msg init\n");
        User::setUser(msg);
        Broadcast(msg, msg.msgdata.loc);
        SendInitInfo(msg.user_id, msg.msgdata.loc);
        break;

    case XMSG_DISCONN:
        Broadcast(msg, User::getLocation(msg.user_id));
        CloseClient(msg.user_id);
        break;

    default:
        TESTINFO("message type error!");
    }
}


void XHandler::Broadcast(XMessage &msg, Coordinate<double> center)
{
    bcMessage bcmsg;
    bcmsg.msg = msg;
    bcmsg.center = center;
    broadcaster.push(bcmsg);
}
void XHandler::SendInitInfo(int fd, Coordinate<double> center)
{
    InitStruct init;
    init.fd = fd;
    init.loc = center;
    initializer.push(init);
}
void XHandler::CloseClient(int fd)
{
    User::removeUser(fd);
    close(fd);
}
