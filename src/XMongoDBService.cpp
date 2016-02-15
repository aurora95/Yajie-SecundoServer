#include "XMongoDBService.h"

XMongoDBService* XMongoDBService::instance = NULL;

XMongoDBService* XMongoDBService::GetInstance()
{
    /*if(instance == NULL)
    {
        Lock();*/
        if(instance == NULL)
        {
            instance = new XMongoDBService();
        }
        /*UnLock();
    }*/
    return instance;
}

mongo::DBClientConnection& XMongoDBService::GetConnection()
{
    return GetInstance()->getConnection();
}

XMongoDBService::XMongoDBService()
{
    mongo::client::initialize();
    connect();
}

XMongoDBService::~XMongoDBService()
{
    //dtor
}

XMongoDBService& XMongoDBService::connect()
{
    try
    {
        dbConn.connect("localhost");
    }
    catch( const mongo::DBException &e ) {
        std::cout << "caught " << e.what() << std::endl;
    }
    return *this;
}

mongo::DBClientConnection& XMongoDBService::getConnection()
{
    return dbConn;
}
