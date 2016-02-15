#include "User.h"
#include <string.h>
#include <mongo/client/dbclient.h>
#include <mongo/bson/bson.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <string>
#include <sstream>
#include "XMongoDBService.h"

using namespace std;
using namespace boost::property_tree;
using namespace mongo;

void User::initializeDatabase()
{
    try
    {
        XMongoDBService::GetConnection().connect("localhost");

        XMongoDBService::GetConnection().createIndex(USERS_COLLECTION_NAME, BSON("loc" << "2d"));

        mongo::IndexSpec specfd;
        specfd.addKey("fd");
        XMongoDBService::GetConnection().createIndex(WORDS_COLLECTION_NAME, specfd);
        specfd.unique();
        XMongoDBService::GetConnection().createIndex(USERS_COLLECTION_NAME, specfd);

        mongo::IndexSpec spectime;
        spectime.addKey("timestramp").expireAfterSeconds(EXPIRE_TIME);
        XMongoDBService::GetConnection().createIndex(WORDS_COLLECTION_NAME, spectime);

    }
    catch( const mongo::DBException &e ) {
        std::cout << "caught " << e.what() << std::endl;
    }
}

User::User()
{
    memset(words, 0, sizeof(words));
    //ctor
}

User::~User()
{
    //dtor
}

User::User(int fd):id(fd)
{
    memset(words, 0, sizeof(words));
    std::auto_ptr<mongo::DBClientCursor> user = XMongoDBService::GetConnection().query(USERS_COLLECTION_NAME, MONGO_QUERY("fd"<<fd));
    mongo::BSONObj obj;
    if(user->more())
        obj = user->next();
    else
    {
        fd = -1;
        return;
    }
    std::vector<BSONElement> loc_array= obj.getField("loc").Array();
    loc.longitude = loc_array[0].Double();
    loc.latitude = loc_array[1].Double();
    skin = obj.getIntField("skin");
    std::auto_ptr<mongo::DBClientCursor> word = XMongoDBService::GetConnection().query(WORDS_COLLECTION_NAME, MONGO_QUERY("fd"<<fd));
    if(word->more())
        obj = word->next();
    strcpy(words, obj.getStringField("content"));
}

User::User(mongo::BSONObj user_without_words)
{
    memset(words, 0, sizeof(words));
    id = user_without_words.getIntField("fd");
    std::vector<BSONElement> loc_array= user_without_words.getField("loc").Array();
    loc.longitude = loc_array[0].Double();
    loc.latitude = loc_array[1].Double();
    skin = user_without_words.getIntField("skin");

    mongo::BSONObj obj;
    std::auto_ptr<mongo::DBClientCursor> word = XMongoDBService::GetConnection().query(WORDS_COLLECTION_NAME, MONGO_QUERY("fd"<<id));
    if(word->more())
        obj = word->next();
    strcpy(words, obj.getStringField("content"));
}



void User::toJSON(char* dest)
{
    ptree pt;
    pt.put("user_id", id);
    pt.put("msg_type", XMSG_USER);
    string str_words(words);   //words
    pt.put("words", str_words);
    ptree location;                      //loc
    location.put<double>("longitude", loc.longitude);
    location.put<double>("latitude", loc.latitude);
    pt.add_child("loc", location);
    pt.put<int>("skin", skin); //skin

    stringstream output_string;
    json_parser::write_json(output_string, pt);
    string json_string = output_string.str();
    strcpy(dest, json_string.c_str());
}

void User::setWords(XMessage &msg)
{
    TESTINFO("set words\n");
    bool upsert = true;
    try
    {
        XMongoDBService::GetConnection().update(WORDS_COLLECTION_NAME,
                  MONGO_QUERY("fd" << msg.user_id),
                  BSON("fd" << msg.user_id << "content" << msg.msgdata.words << "timestramp" << mongo::DATENOW),
                  upsert);
    }
    catch( const mongo::DBException &e ) {
        std::cout << "caught " << e.what() << std::endl;
    }
}

void User::setLocation(XMessage &msg)
{
    TESTINFO("set location\n");
    try
    {
        XMongoDBService::GetConnection().update(USERS_COLLECTION_NAME,
                  BSON("fd" << msg.user_id),
                  BSON("$set" << BSON("loc" << BSON_ARRAY(msg.msgdata.loc.longitude << msg.msgdata.loc.latitude))));
    }
    catch( const mongo::DBException &e ) {
        std::cout << "caught " << e.what() << std::endl;
    }
}

void User::setUser(XMessage &msg)
{
    TESTINFO("set user: id %d, skin %d, loc: %lf %lf\n", msg.user_id, msg.msgdata.skin, msg.msgdata.loc.latitude, msg.msgdata.loc.longitude);
    mongo::BSONObj user = BSON("fd"<<msg.user_id <<
                        "loc"<<BSON_ARRAY(msg.msgdata.loc.longitude << msg.msgdata.loc.latitude) <<
                        "skin"<<msg.msgdata.skin);
    try
    {
        XMongoDBService::GetConnection().insert(USERS_COLLECTION_NAME, user );
    }
    catch( const mongo::DBException &e ) {
        std::cout << "caught " << e.what() << std::endl;
    }
}

Coordinate<double> User::getLocation(int user_id)
{
    Coordinate<double> loc;

    std::auto_ptr<mongo::DBClientCursor> user = XMongoDBService::GetConnection().query(USERS_COLLECTION_NAME, MONGO_QUERY("fd"<<user_id));
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

std::auto_ptr<mongo::DBClientCursor> User::queryByLocation(Coordinate<double> center)
{
    Query q = MONGO_QUERY("loc" << BSON("$geoWithin" << BSON("$centerSphere" << BSON_ARRAY(BSON_ARRAY(center.longitude << center.latitude) << RADIUS(VISIBLE_RANGE)))));
    return XMongoDBService::GetConnection().query(USERS_COLLECTION_NAME, q);
}

void User::removeUser(int fd)
{
    XMongoDBService::GetConnection().remove(USERS_COLLECTION_NAME, BSON("fd"<<fd));
    XMongoDBService::GetConnection().remove(WORDS_COLLECTION_NAME, BSON("fd"<<fd));
}
