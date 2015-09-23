
#ifndef XMESSAGE_INCLUDED
#define XMESSAGE_INCLUDED

#define XMSG_SPEAK   0x01
#define XMSG_MOVE    0x02
#define XMSG_INIT    0x04
#define XMSG_DISCONN 0x08
#define XMSG_USER    0x10
#define XMSG_ID      0x20

#define LEGAL_XMSG   (XMSG_SPEAK|XMSG_MOVE|XMSG_INIT|XMSG_DISCONN|XMSG_USER|XMSG_ID)

#include "Settings.h"

using namespace std;

struct Coordinate
{
    double longitude;
    double latitude;
};


struct XMessage
{
    int user_id;
    int msg_type;
    struct MSGDATA
    {
        char words[WORDS_LEN];
        Coordinate loc;
        int skin;
    }data;
    void toJSON(char* dest);
    void fromJSON(char* src);
};


#endif // XMESSAGE_INCLUDED
