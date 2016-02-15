#ifndef XMONGODBSERVICE_H
#define XMONGODBSERVICE_H

#include <mongo/bson/bson.h>
#include <mongo/client/dbclient.h>
#include "Settings.h"

class XMongoDBService
{
    public:
        static XMongoDBService* GetInstance();
        static mongo::DBClientConnection& GetConnection();
        virtual ~XMongoDBService();

        mongo::DBClientConnection& getConnection();

    private:
        XMongoDBService& connect();
        XMongoDBService();

        mongo::DBClientConnection dbConn;

        static XMongoDBService *instance;
};

#endif // XMONGODBSERVICE_H
