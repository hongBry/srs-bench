


#include <htl_stdinc.hpp>

#include <getopt.h>
#include <stdlib.h>

#include <string>
using namespace std;

#include <htl_core_log.hpp>
#include <htl_core_error.hpp>
#include <htl_app_dash_load.hpp>

#include <htl_main_utility.hpp>

#define DefaultDelaySeconds -1
#define DefaultHttpUrl ""
#define DefaultVod false

int discovery_options(int argc, char** argv, 
    bool& show_help, bool& show_version, string& url, bool& vod, int& threads, 
    double& startup, double& delay, double& error, double& report, int& count)
{
    int ret = ERROR_SUCCESS;
    
    static option long_options[] = {
        SharedOptions()
        {"vod", no_argument, 0, 'o'},
        {0, 0, 0, 0}
    };
    
    int opt = 0;
    int option_index = 0;
    while((opt = getopt_long(argc, argv, "hvc:r:t:os:d:e:m:", long_options, &option_index)) != -1){
        switch(opt){
            case 'o':
                vod = true;
                break;
            ProcessSharedOptions()
            default:
                show_help = true;
                break;
        }
    }
    
    // check values
    if(url == ""){
        show_help = true;
        return ret;
    }

    return ret;
}

void help(char** argv){
    printf("%s, Copyright (c) 2013-2015 winlin\n", ProductHTTPName);
    
    printf(""
        "Usage: %s <Options> <-u URL>\n"
        "%s base on st(state-threads), support huge concurrency.\n"
        "Options:\n"
        ShowHelpPart1()
        "  -o, --vod                        Whether url is vod, loop the m3u8 file list. default is %s\n"
        ShowHelpPart2()
        "\n"
        "\n"
        "Examples:\n"
        "1. start a client\n"
        "   %s -c 1 -r %s\n"
        "2. start 1000 clients\n"
        "   %s -c 1000 -r %s\n"
        "3. start 10000 clients\n"
        "   %s -c 10000 -r %s\n"
        "4. start 100000 clients\n"
        "   %s -c 100000 -r %s\n"
        "5. start 10000 vod clients\n"
        "   %s -c 10000 -o -r %s\n"
        "\n"
        "This program built for %s.\n"
        "Report bugs to <%s>\n",
        argv[0], argv[0], 
        DefaultThread, DefaultHttpUrl, DefaultCount, // part1
        (DefaultVod? "true":"false"), // vod
        (double)DefaultStartupSeconds, (double)DefaultDelaySeconds, // part2
        DefaultErrorSeconds, DefaultReportSeconds, // part2
        argv[0], DefaultHttpUrl, argv[0], DefaultHttpUrl, argv[0], DefaultHttpUrl, argv[0], DefaultHttpUrl,
        argv[0], DefaultHttpUrl,
        BuildPlatform, BugReportEmail);
        
    exit(0);
}


int main(int argc, char** argv){
    int ret = ERROR_SUCCESS;


     bool show_help = false, show_version = false; 
    string url; bool vod = DefaultVod; int threads = DefaultThread; 
    double start = DefaultStartupSeconds, delay = DefaultDelaySeconds, error = DefaultErrorSeconds; 
    double report = DefaultReportSeconds; int count = DefaultCount;
    
    if((ret = discovery_options(argc, argv, show_help, show_version, url, vod, threads, start, delay, error, report, count)) != ERROR_SUCCESS){
        Error("discovery options failed. ret=%d", ret);
        return ret;
    }
    Trace("params url=%s, vod=%d, threads=%d, start=%.2f, delay=%.2f, error=%.2f, report=%.2f, count=%d", 
        url.c_str(), vod, threads, start, delay, error, report, count);
    
    if(show_help){
        help(argv);
    }
    if(show_version){
        version();
    }
    
    StFarm farm;
    
    if((ret = farm.Initialize(report)) != ERROR_SUCCESS){
        Error("initialize the farm failed. ret=%d", ret);
        return ret;
    }

    for(int i = 0; i < threads; i++){
        StDashTask* task = new StDashTask();

        if((ret = task->Initialize(url, vod, start, delay, error, count)) != ERROR_SUCCESS){
            Error("initialize task failed, url=%s, ret=%d", url.c_str(), ret);
            return ret;
        }
        
        if((ret = farm.Spawn(task)) != ERROR_SUCCESS){
            Error("st farm spwan task failed, ret=%d", ret);
            return ret;
        }
    }
    
    farm.WaitAll();
    
    return 0;
    
}