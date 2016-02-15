
#ifndef XMESSAGE_INCLUDED
#define XMESSAGE_INCLUDED

#define XMSG_SPEAK   0x001
#define XMSG_MOVE    0x002
#define XMSG_INIT    0x004
#define XMSG_DISCONN 0x008
#define XMSG_USER    0x010
#define XMSG_ID      0x020
//map stuffs
#define XMSG_GETMAP   0x040
#define XMSG_POSTMAP  0x080
#define XMSG_QUERYMAP 0x100
#define XMSG_ANSMAP   0x200

#define LEGAL_XMSG   (XMSG_SPEAK|XMSG_MOVE|XMSG_INIT|XMSG_DISCONN|XMSG_USER|XMSG_ID)

#include "Settings.h"
#include <stdint.h>

using namespace std;

template <typename T>
struct Coordinate
{
    T longitude;
    T latitude;
};

struct Msgdata
{
    char words[WORDS_LEN];
    Coordinate<double> loc;
    int skin;
};
/*****************this part should be in a seperated program******************
struct Mapdata
{
    Coordinate<double> loc;
    struct{
        int x;
        int y;
    }bias;
    int length;
    char data[MAPDATA_LENGTH * MAPDATA_LENGTH];
    uint64_t version;
};
*/
struct XMessage
{
    int user_id;
    int msg_type;
    Msgdata msgdata;
    //Mapdata mapdata;
    void toJSON(char* dest);
    void fromJSON(char* src);
};


#endif // XMESSAGE_INCLUDED
