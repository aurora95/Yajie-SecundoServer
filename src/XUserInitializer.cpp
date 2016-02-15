#include "XUserInitializer.h"
#include "User.h"
#include "Settings.h"
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <mongo/bson/bson.h>
#include <mongo/client/dbclient.h>
#include <vector>


using namespace mongo;
using namespace mongo::geo;

XUserInitializer::XUserInitializer():HandlerBase()
{

}

XUserInitializer::~XUserInitializer()
{
    //dtor
}

void XUserInitializer::HandleMessage(InitStruct &init)
{
    Coordinate<double> center = init.loc;
    if(center.latitude < -90 || center.latitude > 90 || center.longitude < -180 || center.longitude > 180)
    {
        perror("wrong location!");
        return;
    }

    XMessage id_msg;
    id_msg.msg_type = XMSG_ID; id_msg.user_id = init.fd;
    char buf_id[BUFFER_SIZE];
    id_msg.toJSON(buf_id);
    if(send(init.fd, buf_id, strlen(buf_id), 0) < 0)
    {
        perror("error when sending id message!");
        return;
    }
    TESTINFO("Send User: %s\n", buf_id);

    std::auto_ptr<mongo::DBClientCursor> users = User::queryByLocation(center);
    std::vector<User> user_array;
    while(users->more())
    {
        user_array.push_back(User(users->next()));
    }
    int users_num = user_array.size();


    for(int i = 0; i< users_num; i++)
    {
        char buf_user[BUFFER_SIZE];
        user_array[i].toJSON(buf_user);
        int retval;
        int len = strlen(buf_user);
        while((retval = send(init.fd, buf_user, len, 0)) < len)
        {
            if(retval < 0)
            {
                perror("socket error!");
                return;
            }
        }
        TESTINFO("Send User: %s\n", buf_user);
    }
}
