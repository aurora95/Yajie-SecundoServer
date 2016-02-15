#include "XBroadcaster.h"
#include "XMessage.h"
#include "Settings.h"
#include "User.h"
#include <limits.h>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <mongo/bson/bson.h>
#include <mongo/client/dbclient.h>



using namespace mongo;
using namespace mongo::geo;

XBroadcaster::XBroadcaster():HandlerBase()
{

}

XBroadcaster::~XBroadcaster()
{
    //dtor
}

void XBroadcaster::HandleMessage(bcMessage &bcmsg)
{
    Coordinate<double> center = bcmsg.center;
    if(center.latitude == INT_MIN && center.longitude == INT_MIN)
    {
        return;
    }
    if(center.latitude < -90 || center.latitude > 90 || center.longitude < -180 || center.longitude > 180)
    {
        perror("wrong location!\n");
        return;
    }

    char buf[BUFFER_SIZE];
    bcmsg.msg.toJSON(buf);

    std::auto_ptr<mongo::DBClientCursor> users = User::queryByLocation(center);
    while(users->more())
    {
        BSONObj obj = users->next();
        int fd = obj.getField("fd").Int();
        if(send(fd, buf, strlen(buf), 0) < 0)
        {
            perror("socket send error: buffer filled up!");
        }
        TESTINFO("Broadcasted: %s\n", buf);
    }
}
