
#include <htl_stdinc.hpp>

#include <inttypes.h>
#include <stdlib.h>

#include <string>
#include <sstream>
#include <vector>
using namespace std;

#include <htl_core_error.hpp>
#include <htl_core_log.hpp>
#include <htl_app_http_client.hpp>

#include <htl_app_dash_parser.hpp>



class String
{
private:
    string value;
public:
    String(string str = ""){
        value = str;
    }
public:
    String& set_str(string str){
        value = str;
        return *this;
    }
    int length(){
        return (int)value.length();
    }
    bool startswith(string key, String* left = NULL){
        size_t pos = value.find(key);
        
        if(pos == 0 && left != NULL){
            left->set_str(value.substr(pos + key.length()));
            left->strip();
        }
        
        return pos == 0;
    }
    bool endswith(string key, String* left = NULL){
        size_t pos = value.rfind(key);
        
        if(pos == value.length() - key.length() && left != NULL){
            left->set_str(value.substr(pos));
            left->strip();
        }
        
        return pos == value.length() - key.length();
    }
    String& strip(){
        if (value.empty()) {
            return *this;
        }
        
        // macro to test whether char is a space.
        #define __IS_SPACE(ch) (ch == '\n' || ch == '\r' || ch == ' ')
        
        // find the start and end which is not space.
        char* bytes = (char*)value.data();
        
        // find the end not space
        char* end = NULL;
        for (end = bytes + value.length() - 1; end > bytes && __IS_SPACE(end[0]); end--) {
        }

        // find the start not space
        char* start = NULL;
        for (start = bytes; start < end && __IS_SPACE(start[0]); start++) {
        }
        
        // get the data to trim.
        value = value.substr(start - bytes, end - start + 1);
        
        return *this;
    }
    string getline(){
        size_t pos = string::npos;
        
        if((pos = value.find("\n")) != string::npos){
            return value.substr(0, pos);
        }
        
        return value;
    }
    String& remove(int size){
        if(size >= (int)value.length()){
            value = "";
            return *this;
        }
        
        value = value.substr(size);
        return *this;
    }
    const char* c_str(){
        return value.c_str();
    }
};

string replace(string str, string)

void DashSEG::get_seg_url() {
    string ret = seg_url;
    string old_str = "[NUMBER]";

    long unsigned int pos = 0;
    while((pos = ret.find(old_str, pos)) != std::string::npos){
        ret = ret.replace(pos, old_str.length(), to_string(index));
    }
    real_seg_url = ret;
}



DashMPDParser::DashMPDParser(){

}

DashMPDParser::~DashMPDParser(){

}

int DashMPDParser::ParseMPDData(HttpUrl* url, string dash, DashSEG& start_seg, int& target_duration) {
    int ret = ERROR_SUCCESS;

    target_duration = 2

    start_seg.index = 1;
    start_seg.seg_url = "";
    start_seg.duration = 1.9;
    return ret;
}