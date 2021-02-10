#ifndef SERVER_CONF_H__
#define SERVER_CONF_H__

#define DEFAULT_MEDIADIR  "/var/media/"
#define DEFAULT IF        "ens33"

enum{
    RUN_DAEMON = 1,
    RUN_FOREGROUND;

struct server_conf_st
{
    char* rcvport;  //默认的接受端口
    char* mgroup;   //默认的多播组的号码
    char* media_dir;//媒体库的相关位置
    char runmode;   //前台或者后台运行的模式
    char* ifname;   //指定网卡信息
};

extern struct server_conf_st server_conf;


#endif
