#ifndef USER_H
#define USER_H
#include "XMessage.h"
#include <mongo/client/dbclient.h>
#include <mongo/bson/bson.h>
/**************A Helper Class to get User data****************/
class User
{
    public:
        User();
        User(int fd, mongo::DBClientConnection &dbconn);
        User(mongo::BSONObj user_without_words, mongo::DBClientConnection &dbconn);
        void toJSON(char* dest);
        virtual ~User();

        int id; //same as client_fd
        Coordinate loc;
        char words[WORDS_LEN];
        int skin;
};

#endif // USER_H
