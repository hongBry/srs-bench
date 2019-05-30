#ifndef _htl_app_dash_parser_hpp
#define _htl_app_dash_parser_hpp


#include <string>
#include <vector>

#include <htl_core_uri.hpp>

struct DashSEG
{
    int index;
    std::string seg_url;
    double duration;
    std::string real_seg_url;
    
    bool operator== (const DashSEG& b)const{
        return seg_url == b.seg_url;
    }

    virtual void get_seg_url();
};

class DashMPDParser
{
public:
    DashMPDParser();
    virtual ~DashMPDParser();
public:
    static int ParseMPDData(HttpUrl* url, std::string dash, DashSEG& start_seg, int& target_duration);
};

#endif
