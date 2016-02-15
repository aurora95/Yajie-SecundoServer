#include "XMessage.h"
#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <string>
#include <sstream>

using namespace std;
using namespace boost::property_tree;


void XMessage::toJSON(char* dest)
{
    ptree pt;
    pt.put<int>("user_id", user_id);
    pt.put<int>("msg_type", msg_type);
    switch(msg_type)
    {
    case XMSG_SPEAK:
        {
            string str_words(msgdata.words);
            pt.put("words", str_words);
            break;
        }
    case XMSG_MOVE:
        {
            ptree loc;
            loc.put<double>("longitude", msgdata.loc.longitude);
            loc.put<double>("latitude", msgdata.loc.latitude);
            pt.add_child("loc", loc);
            break;
        }
    case XMSG_INIT:
        {
            ptree loc;
            loc.put<double>("longitude", msgdata.loc.longitude);
            loc.put<double>("latitude", msgdata.loc.latitude);
            pt.add_child("loc", loc);
            pt.put<int>("skin", msgdata.skin);
            break;
        }
    case XMSG_USER:
        {
            string str_words(msgdata.words);   //words
            pt.put("words", str_words);

            ptree loc;                      //loc
            loc.put<double>("longitude", msgdata.loc.longitude);
            loc.put<double>("latitude", msgdata.loc.latitude);
            pt.add_child("loc", loc);

            pt.put<int>("skin", msgdata.skin); //skin
            break;
        }
    /*case XMSG_QUERYMAP:
        {
            ptree loc;
            loc.put<double>("longitude", mapdata.loc.longitude);
            loc.put<double>("latitude", mapdata.loc.latitude);
            pt.add_child("loc", loc);

            break;
        }
    case XMSG_POSTMAP:
        {
            ptree loc;
            loc.put<double>("longitude", mapdata.loc.longitude);
            loc.put<double>("latitude", mapdata.loc.latitude);
            pt.add_child("loc", loc);

            pt.put<int>("length", mapdata.length);

            ptree bias;
            bias.put<int>("x", mapdata.bias.x);
            bias.put<int>("y", mapdata.bias.y);
            pt.add_child("bias", bias);

            string str_data(mapdata.data);
            pt.put("data", str_data);

            break;
        }
    case XMSG_GETMAP:
        {
            ptree loc;
            loc.put<double>("longitude", mapdata.loc.longitude);
            loc.put<double>("latitude", mapdata.loc.latitude);
            pt.add_child("loc", loc);

            break;
        }
    case XMSG_ANSMAP:
        {
            ptree loc;
            loc.put<double>("longitude", mapdata.loc.longitude);
            loc.put<double>("latitude", mapdata.loc.latitude);
            pt.add_child("loc", loc);

            pt.put("version", mapdata.version);

            break;
        }*/
    default:
        break;
    }
    stringstream output_string;
    json_parser::write_json(output_string, pt);
    string json_string = output_string.str();
    strcpy(dest, json_string.c_str());
}

void XMessage::fromJSON(char* src)
{
    stringstream jsonstream;
    jsonstream<<src;
    ptree pt;
    try{
        json_parser::read_json(jsonstream, pt);

        user_id = pt.get<int>("user_id");
        msg_type = pt.get<int>("msg_type");
        switch(msg_type)
        {
        case XMSG_SPEAK:
            {
                string str_data = pt.get<string>("words");
                strcpy(msgdata.words, str_data.c_str());
                break;
            }
        case XMSG_MOVE:
            {
                ptree loc = pt.get_child("loc");
                msgdata.loc.latitude = loc.get<double>("latitude");
                msgdata.loc.longitude = loc.get<double>("longitude");
                break;
            }
        case XMSG_INIT:
            {
                ptree loc = pt.get_child("loc");
                msgdata.loc.latitude = loc.get<double>("latitude");
                msgdata.loc.longitude = loc.get<double>("longitude");
                msgdata.skin = pt.get<int>("skin");
                break;
            }
        case XMSG_USER:
            {
                string str_data = pt.get<string>("words");
                strcpy(msgdata.words, str_data.c_str());
                ptree loc = pt.get_child("loc");
                msgdata.loc.latitude = loc.get<double>("latitude");
                msgdata.loc.longitude = loc.get<double>("longitude");
                msgdata.skin = pt.get<int>("skin");
                break;
            }
        /*case XMSG_GETMAP:
            {
                ptree loc = pt.get_child("loc");
                mapdata.loc.latitude = loc.get<double>("latitude");
                mapdata.loc.longitude = loc.get<double>("longitude");
                break;
            }
        case XMSG_POSTMAP:
            {
                ptree loc = pt.get_child("loc");
                mapdata.loc.latitude = loc.get<double>("latitude");
                mapdata.loc.longitude = loc.get<double>("longitude");
                ptree bias = pt.get_child("bias");
                mapdata.bias.x = bias.get<int>("x");
                mapdata.bias.y = bias.get<int>("y");
                mapdata.length = bias.get<int>("length");
                string str_data = pt.get<string>("data");
                strcpy(mapdata.data, str_data.c_str());
                break;
            }
        case XMSG_QUERYMAP:
            {
                ptree loc = pt.get_child("loc");
                mapdata.loc.latitude = loc.get<double>("latitude");
                mapdata.loc.longitude = loc.get<double>("longitude");
                break;
            }
        case XMSG_ANSMAP:
            {
                ptree loc = pt.get_child("loc");
                mapdata.loc.latitude = loc.get<double>("latitude");
                mapdata.loc.longitude = loc.get<double>("longitude");
                mapdata.version = loc.get<uint64_t>("version");
                break;
            }*/
        default:
            break;
        }
    }
    catch(ptree_error & e){
        cout<<e.what()<<endl;
    }
}
