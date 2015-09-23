#include "XHandler.h"
#include "XBroadcaster.h"
#include "HandlerBase.h"
#include "XUserInitializer.h"
#include "Settings.h"
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

    dbconn.createIndex(USERS_COLLECTION_NAME, BSON("loc" << "2d"));

    mongo::IndexSpec specfd;
    specfd.addKey("fd");
    dbconn.createIndex(WORDS_COLLECTION_NAME, specfd);
    specfd.unique();
    dbconn.createIndex(USERS_COLLECTION_NAME, specfd);


    mongo::IndexSpec spectime;
    spectime.addKey("timestramp").expireAfterSeconds(EXPIRE_TIME);
    dbconn.createIndex(WORDS_COLLECTION_NAME, spectime);
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
        setWords(msg);
        Broadcast(msg, getLocation(msg.user_id));
        break;

    case XMSG_MOVE:
        TESTINFO("msg move\n");
        setLocation(msg);
        Broadcast(msg, msg.data.loc);
        break;

    case XMSG_INIT:
        TESTINFO("msg init\n");
        setUser(msg);
        Broadcast(msg, msg.data.loc);
        SendInitInfo(msg.user_id, msg.data.loc);
        break;

    case XMSG_DISCONN:
        Broadcast(msg, getLocation(msg.user_id));
        CloseClient(msg.user_id);
        break;

    default:
        TESTINFO("message type error!");
    }
}

inline void XHandler::setWords(XMessage &msg)
{
    TESTINFO("set words\n");
    bool upsert = true;
    try
    {
        dbconn.update(WORDS_COLLECTION_NAME,
                  MONGO_QUERY("fd" << msg.user_id),
                  BSON("fd" << msg.user_id << "content" << msg.data.words << "timestramp" << mongo::DATENOW),
                  upsert);
    }
    catch( const mongo::DBException &e ) {
        std::cout << "caught " << e.what() << std::endl;
    }
}

inline void XHandler::setLocation(XMessage &msg)
{
    TESTINFO("set location\n");
    try
    {
        dbconn.update(USERS_COLLECTION_NAME,
                  BSON("fd" << msg.user_id),
                  BSON("loc" << BSON_ARRAY(msg.data.loc.longitude << msg.data.loc.latitude)));
    }
    catch( const mongo::DBException &e ) {
        std::cout << "caught " << e.what() << std::endl;
    }
}

inline void XHandler::setUser(XMessage &msg)
{
    TESTINFO("set user\n");
    mongo::BSONObj user = BSON("fd"<<msg.user_id <<
                        "loc"<<BSON_ARRAY(msg.data.loc.longitude << msg.data.loc.latitude) <<
                        "skin"<<msg.data.skin);
    try
    {
        dbconn.insert(USERS_COLLECTION_NAME, user );
    }
    catch( const mongo::DBException &e ) {
        std::cout << "caught " << e.what() << std::endl;
    }
}

Coordinate XHandler::getLocation(int user_id)
{
    Coordinate loc;

    std::auto_ptr<mongo::DBClientCursor> user = dbconn.query(USERS_COLLECTION_NAME, MONGO_QUERY("fd"<<user_id));
    mongo::BSONObj obj;
    if(user->more())
        obj = user->next();
    else
    {
        TESTINFO("In getLocation: Cannot find user!\n");
        loc.longitude = loc.latitude = INT_MIN;
        return loc;
    }
    std::vector<mongo::BSONElement> arr = obj.getField("loc").Array();
    loc.longitude = arr[0].Double();
    loc.latitude = arr[1].Double();
    return loc;
}
void XHandler::Broadcast(XMessage &msg, Coordinate center)
{
    bcMessage bcmsg;
    bcmsg.msg = msg;
    bcmsg.center = center;
    broadcaster.push(bcmsg);
}
void XHandler::SendInitInfo(int fd, Coordinate center)
{
    InitStruct init;
    init.fd = fd;
    init.loc = center;
    initializer.push(init);
}
void XHandler::CloseClient(int fd)
{
    dbconn.remove(USERS_COLLECTION_NAME, BSON("fd"<<fd));
    dbconn.remove(WORDS_COLLECTION_NAME, BSON("fd"<<fd));
    close(fd);
}
