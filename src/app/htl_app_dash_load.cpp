#include <htl_stdinc.hpp>

#include <inttypes.h>
#include <stdlib.h>

#include <string>
#include <sstream>
#include <vector>
using namespace std;

#include <htl_core_log.hpp>
#include <htl_core_error.hpp>
#include <htl_core_aggregate_ret.hpp>
#include <htl_app_http_client.hpp>

#include <htl_app_dash_load.hpp>
#include <htl_app_dash_parser.hpp>

#include <algorithm>

#define DEFAULT_SEG_DURATION 2

StDashTask::StDashTask(){
    target_duration = DEFAULT_SEG_DURATION
}

StDashTask::~StDashTask(){

}

int StDashTask::Initialize(std::string http_url, bool vod, double startup, double delay, double error, int count) {
    int ret = ERROR_SUCCESS;

    is_vod = vod;

    if ((ret = InitializeBase(http_url, startup, delay, error, count)) != ERROR_SUCCESS) {
        return ret;
    }

    return ret;
}

Uri* StDashTask::GetUri() {
    return &url;
}


int StDashTask::ProcessTask() {
    int ret = ERROR_SUCCESS;
    
    Trace("start to process HLS task #%d, schema=%s, host=%s, port=%d, path=%s, startup=%.2f, delay=%.2f, error=%.2f, count=%d", 
        GetId(), url.GetSchema(), url.GetHost(), url.GetPort(), url.GetPath(), startup_seconds, delay_seconds, error_seconds, count);
    
    // if count is zero, infinity loop.
    for(int i = 0; count == 0 || i < count; i++){
        statistic->OnTaskStart(GetId(), url.GetUrl());
        
        StHttpClient client;
        if((ret = ProcessMPD(client)) != ERROR_SUCCESS){
            statistic->OnTaskError(GetId(), 0);
            
            Error("http client process m3u8 failed. ret=%d", ret);
            st_usleep((st_utime_t)(error_seconds * 1000 * 1000));
            continue;
        }
        
        Info("[HLS] %s download completed.", url.GetUrl());
    }
    
    return ret;
}

int StDashTask::ProcessMPD(StHttpClient& client) {
    int ret = ERROR_SUCCESS;
    
    string mpd;
    if ((ret = client.DownloadString(&url, &mpd)) != ERROR_SUCCESS) {
        ERROR("http client get dash failed, ret=%d", ret);
        return ret;
    }
    TRACE("[DASH] get mpd %s get success, length=%"PRId64, url.GetUrl(), (int64_t)mpd.length());

    DashSEG start_seg;

    if((ret = DashMPDParser::ParseMPDData(&url, mpd, start_seg, target_duration)) != ERROR_SUCCESS){
        Error("http client parse mpd content failed. ret=%d", ret);
        return ret;
    }


    if((ret = ProcessSEG(client, start_seg)) != ERROR_SUCCESS) {
        Error("http client download dash seg file failed. ret=%d", ret);
        return ret;
    }

    return ret;
}

int StDashTask::ProcessSEG(StHttpClient& client, DashSEG& start) {
    int ret = ERROR_SUCCESS;

    DashSEG seg_object = *start;
    seg_object.get_seg_url();
    if(!is_vod){
        while(true){
            if(!is_vod){
                last_downloaded_seg = seg_object;
            }
            Info("start to process seg %s", seg_object.real_seg_url.c_str());
            if(DownloadSEG(client, seg_object) != ERROR_SUCCESS) {
                int sleep_ms = StUtility::BuildRandomMTime((target_duration > 0)? target_duration:DEFAULT_SEG_DURATION);
                Trace("[DASH] no fresh seg, wait for a while. sleep %dms", sleep_ms);
                st_usleep(sleep_ms * 1000);
            } else {
                seg_object.index += 1;
                seg_object.get_seg_url();
            }
        }
        
    } else {

    }
    
    return ret;
}


int StDashTask::DownloadSEG(StHttpClient& client, DashSEG& seg){
    int ret = ERROR_SUCCESS;
    
    HttpUrl url;
    
    if((ret = url.Initialize(seg.real_seg_url)) != ERROR_SUCCESS){
        Error("initialize seg url failed. ret=%d", ret);
        return ret;
    }
    
    Info("[SEG] url=%s, duration=%.2f, delay=%.2f", url.GetUrl(), seg.duration, delay_seconds);
    statistic->OnSubTaskStart(GetId(), seg.real_seg_url);
    
    if((ret = client.DownloadString(&url, NULL)) != ERROR_SUCCESS){
        statistic->OnSubTaskError(GetId(), (int)seg.duration);
            
        Error("http client download seg file %s failed. ret=%d", url.GetUrl(), ret);
        return ret;
    }
    
    int sleep_ms = StUtility::BuildRandomMTime((delay_seconds >= 0)? delay_seconds:seg.duration);
    Trace("[DASH] url=%s download, duration=%.2f, delay=%.2f, size=%"PRId64", sleep %dms", 
        url.GetUrl(), seg.duration, delay_seconds, client.GetResponseHeader()->content_length, sleep_ms);
    st_usleep(sleep_ms * 1000);
    
    statistic->OnSubTaskEnd(GetId(), (int)seg.duration);
    
    return ret;
}