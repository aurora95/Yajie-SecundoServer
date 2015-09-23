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
            string str_words(data.words);
            pt.put("words", str_words);
            break;
        }
    case XMSG_MOVE:
        {
            ptree loc;
            loc.put<double>("longitude", data.loc.longitude);
            loc.put<double>("latitude", data.loc.latitude);
            pt.add_child("loc", loc);
            break;
        }
    case XMSG_INIT:
        {
            ptree loc;
            loc.put<double>("longitude", data.loc.longitude);
            loc.put<double>("latitude", data.loc.latitude);
            pt.add_child("loc", loc);
            pt.put<int>("skin", data.skin);
            break;
        }
    case XMSG_USER:
        {
            string str_words(data.words);   //words
            pt.put("words", str_words);

            ptree loc;                      //loc
            loc.put<double>("longitude", data.loc.longitude);
            loc.put<double>("latitude", data.loc.latitude);
            pt.add_child("loc", loc);

            pt.put<int>("skin", data.skin); //skin
            break;
        }
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
                strcpy(data.words, str_data.c_str());
                break;
            }
        case XMSG_MOVE:
            {
                ptree loc = pt.get_child("loc");
                data.loc.latitude = loc.get<double>("latitude");
                data.loc.longitude = loc.get<double>("longitude");
                break;
            }
        case XMSG_INIT:
            {
                ptree loc = pt.get_child("loc");
                data.loc.latitude = loc.get<double>("latitude");
                data.loc.longitude = loc.get<double>("longitude");
                data.skin = pt.get<int>("skin");
                break;
            }
        case XMSG_USER:
            {
                string str_data = pt.get<string>("words");
                strcpy(data.words, str_data.c_str());
                ptree loc = pt.get_child("loc");
                data.loc.latitude = loc.get<double>("latitude");
                data.loc.longitude = loc.get<double>("longitude");
                data.skin = pt.get<int>("skin");
            }
        default:
            break;
        }
    }
    catch(ptree_error & e){
        cout<<e.what()<<endl;
    }
}
void fromJSON(char* src);
