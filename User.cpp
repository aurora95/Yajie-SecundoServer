#include "User.h"
#include <string.h>
#include <mongo/client/dbclient.h>
#include <mongo/bson/bson.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <string>
#include <sstream>

using namespace std;
using namespace boost::property_tree;
using namespace mongo;


User::User()
{
    memset(words, 0, sizeof(words));
    //ctor
}

User::~User()
{
    //dtor
}

User::User(int fd, mongo::DBClientConnection &dbconn):id(fd)
{
    memset(words, 0, sizeof(words));
    std::auto_ptr<mongo::DBClientCursor> user = dbconn.query(USERS_COLLECTION_NAME, MONGO_QUERY("fd"<<fd));
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
    std::auto_ptr<mongo::DBClientCursor> word = dbconn.query(WORDS_COLLECTION_NAME, MONGO_QUERY("fd"<<fd));
    if(word->more())
        obj = word->next();
    strcpy(words, obj.getStringField("content"));
}

User::User(mongo::BSONObj user_without_words, mongo::DBClientConnection &dbconn)
{
    memset(words, 0, sizeof(words));
    id = user_without_words.getIntField("fd");
    std::vector<BSONElement> loc_array= user_without_words.getField("loc").Array();
    loc.longitude = loc_array[0].Double();
    loc.latitude = loc_array[1].Double();
    skin = user_without_words.getIntField("skin");

    mongo::BSONObj obj;
    std::auto_ptr<mongo::DBClientCursor> word = dbconn.query(WORDS_COLLECTION_NAME, MONGO_QUERY("fd"<<id));
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

