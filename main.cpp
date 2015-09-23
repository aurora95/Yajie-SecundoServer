#include <iostream>
#include <ev++.h>
#include <stdio.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <mongo/bson/bson.h>
#include <mongo/client/dbclient.h>
#include "AppServer.h"
#include "Settings.h"

int main()
{
    signal(SIGPIPE, SIG_IGN);
    mongo::client::initialize();
    AppServer server(PORT);
    server.run();
}
