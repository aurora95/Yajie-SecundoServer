#ifndef APPSERVER_H
#define APPSERVER_H
#include <iostream>
#include <ev++.h>
#include <stdio.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <mongo/bson/bson.h>
#include <mongo/client/dbclient.h>
#include "XHandler.h"


class AppServer
{
    public:
        AppServer(int port);
        virtual ~AppServer();
        void accept_cb(ev::io &watcher, int revents);
        void read_cb(ev::io &watcher, int revents);
        void run();
    protected:
    private:
        int listen_fd;
        ev::io accept_watcher;
        XHandler MessageHandler;
};

#endif // APPSERVER_H
