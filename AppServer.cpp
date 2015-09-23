#include "AppServer.h"
#include <iostream>
#include <ev++.h>
#include <stdio.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/tcp.h>
#include "mongo/bson/bson.h"
#include "mongo/client/dbclient.h"
#include "TCPHelper.h"
#include "XHandler.h"
#include "Settings.h"
AppServer::AppServer(int port)
{
    listen_fd = open_listenfd(port);
    if(listen_fd < 0)
    {
        perror("error in open_listenfd\n");
    }
    accept_watcher.set <AppServer, &AppServer::accept_cb > (this);
    accept_watcher.set(listen_fd, ev::READ);
}

AppServer::~AppServer()
{
    //dtor
}

void AppServer::run()
{
    struct ev_loop *loop = EV_DEFAULT;
    accept_watcher.start();
    ev_run (loop, 0);
}

void AppServer::accept_cb(ev::io &watcher, int revents)
{
    if(EV_ERROR & revents)
    {
        perror("Error event in accept\n");
        return;
    }

    sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_fd;
    client_fd = accept(watcher.fd, (struct sockaddr*)&client_addr, &client_len);
    if(client_fd < 0)
    {
        perror("Accept error!\n");
        return;
    }
    int flags = fcntl(client_fd, F_GETFL, 0);       //set nonblocking
    fcntl(client_fd, F_SETFL, flags|O_NONBLOCK);    //

    int keepalive = 1;                              //set KeepAlive
    int keepidle = KEEPALIVE_TIME;
    int keepinterval = KEEPALIVE_INTVL;
    int keepcount = KEPPALIVE_PROBES;
    setsockopt(client_fd, SOL_SOCKET, SO_KEEPALIVE, (void*)&keepalive, sizeof(keepalive));
    setsockopt(client_fd, SOL_TCP, TCP_KEEPIDLE, (void*)&keepidle , sizeof(keepidle ));
    setsockopt(client_fd, SOL_TCP, TCP_KEEPINTVL, (void *)&keepinterval , sizeof(keepinterval ));
    setsockopt(client_fd, SOL_TCP, TCP_KEEPCNT, (void *)&keepcount , sizeof(keepcount ));

    ev::io *read_watcher = new ev::io;
    //accept_watcher.set <AppServer, &AppServer::accept_cb > (this);
    //accept_watcher.set(listen_fd, ev::READ);
    if(read_watcher == NULL)
    {
        perror("Failed to create new ev_io *read_watcher!\n");
        return;
    }
    read_watcher->set <AppServer, &AppServer::read_cb> (this);
    read_watcher->start(client_fd, ev::READ);

    TESTINFO("client connected.\n");
    return;
}

void AppServer::read_cb(ev::io &watcher, int revents)
{
    if(EV_ERROR & revents)
    {
        perror("Error event in accept\n");
        return;
    }

    char buffer[BUFFER_SIZE];
    int readlen;

    readlen = recv(watcher.fd, buffer, BUFFER_SIZE, 0);
    if(readlen < 0)
    {
        perror("read error!\n");
        return;
    }
    if(readlen == 0)
    {
        TESTINFO("client disconnected.\n");
        XMessage msg;
        msg.user_id = watcher.fd;
        msg.msg_type = XMSG_DISCONN;
        MessageHandler.push(msg);
        watcher.stop();
        return;
    }
    else//
    {
        buffer[readlen] = '\0';
        TESTINFO("Received: %s", buffer);
        MessageHandler.parse_and_push(buffer, watcher.fd);

    }

    return;
}
