


#ifndef _htl_app_dash_load_hpp
#define _htl_app_dash_load_hpp

#include <htl_app_http_client.hpp>
#include <htl_app_task_base.hpp>


class StDashTask : public StBaseTask
{
private:
    HttpUrl url;

    std::string last_downloaded_seg;
    int target_duration;
    bool is_vod;

public:
    StDashTask();
    virtual ~StDashTask();

public:
    virtual int Initialize(std::string http_url, bool vod, double startup, double delay, double error, int count);

protected:
    virtual Uri* GetUri();
    virtual int ProcessTask();

private:
    virtual int ProcessMPD(StHttpClient& client);
    virtual int ProcessSEG(StHttpClient& client, DashSEG& start);
    virtual int DownloadSEG(StHttpClient& client, DashSEG& seg);

}


#endif