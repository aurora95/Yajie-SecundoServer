#ifndef SETTINGS_H_INCLUDED
#define SETTINGS_H_INCLUDED
#include "stdio.h"

#define PORT 6666
#define BUFFER_SIZE 1024
#define MAX_ALLOWED_CLIENT 10240
#define WORDS_LEN 256
#define EXPIRE_TIME 1200
#define VISIBLE_RANGE 0.5 //km
#define RADIUS(km) km/6378.1

#define USERS_COLLECTION_NAME "AppDATA.users"
#define WORDS_COLLECTION_NAME "AppDATA.words"

/*------------TCP Keepalive Settings------------*/
#define KEEPALIVE_TIME      600
#define KEEPALIVE_INTVL     20
#define KEPPALIVE_PROBES    3
/*----------------------------------------------*/


/*------------------Debug Mode------------------*/
#define DEBUGMODE

#ifdef DEBUGMODE
#define TESTINFO(fmt, args...) printf(fmt, ##args)
#else
#define TESTINFO(fmt, args...)
#endif

#endif // SETTINGS_H_INCLUDED
