#ifndef USER_H
#define USER_H
#include "XMessage.h"
#include <mongo/client/dbclient.h>
#include <mongo/bson/bson.h>
/**************A Helper Class to get User data****************/
class User
{
public:
    int id; //same as client_fd
    Coordinate<double> loc;
    char words[WORDS_LEN];
    int skin;

    static void initializeDatabase();

    User();
    User(int fd);
    User(mongo::BSONObj user_without_words);
    void toJSON(char* dest);
    ~User();
    static void setWords(XMessage &msg);
    static void setLocation(XMessage &msg);
    static void setUser(XMessage &msg);
    static Coordinate<double> getLocation(int user_id);
    static std::auto_ptr<mongo::DBClientCursor> queryByLocation(Coordinate<double> center);
    static void removeUser(int fd);


};

#endif // USER_H
