#include <stdio.h>
#include <proto.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "server_conf.h"

/*-M 指定多播组
 *-P 指定接受端口
  -F 前台运行
  -D 指定媒体库位置
  -I 指定网卡信息
  -H 显示帮助
 * */

struct server_conf_st server_conf = {.rcvport = DEFAULT_RCVPORT,\
                                    .mgroup = DEFAULT_MGROUP,\
                                    .media_dir = DEFAULT_MEDIADIR,\
                                    .runmode = RUN_DAEMON,\
                                    .ifname = DEFAULT_IF}


static void printfhelp(void)
{
    printf("-M 指定多播组\n");
    printf("-P 指定接受端口\n");
    printf("-F 前台运行\n");
    printf("-D 指定媒体库位置\n");
    printf("-I 指定网卡信息\n");
    printf("-H 显示帮助\n");
}


static void daemon_exit(int s)
{   
    /*放来不及释放的工作*/


    closelog();

    exit(0);
}

static daemonize(void)
{
    pid_t pid;
    int fd;

    pid = fork();
    if(pid < 0)
    {
       // perror("fork()");
        syslog(LOG_ERR,"fork():%s",strerror(errno));
        return -1;
    }
    
    if(pid > 0)
        exit(0);

    fd = open("/dev/null",O_RDWR);
    if(fd < 0)
    {
       // perror("open()");
        syslog(LOG_WARNING, "open():%s",strerror(errno));
        return -2;
    }
    else
    {
        dup2(fd,0);
        dup2(fd,1);
        dup2(fd,2);

        if(fd >2)
        {
            close(fd);
        }
    }

    setsid();                             //调用是有条件的man 2 :创建新的会话（如果调用的进程不是当前进程组的组长的话），并设置进程组的ID
    chdir("/");
    umask(0);
    return 0;
}

int main(int argc, char** argv)
{
    int c;
    struct sigaction sa;

    sa.sa_handler = daemon_exit;
    sigemptyset(&sa.sa_mask);
    sigaddset(&sa.sa_mask,SIGINT);
    sigaddset(&sa,sa_mask,SIGQUIT);
    sigaddset(&sa,sa_mask,SIGTERM);

    //当前守护进程必定是由信号打断
    //定义以下三个信号的行为
    sigaction(SIGTERM,&sa,NULL);
    sigaction(SIGINT,&sa,NULL);
    sigaction(SIGQUIT,&sa,NULL);

    //系统日志
    openlog("netradio",LOG_PID|LOG_PERROR,LOG_DAEMON);



    /*命令行分析*/
    while(1)
    {
        c = getopt(argc,argv,"M:P:FD:I:H");
        if(c < 0)
            break;
        switch(c)
        {
            case 'M':
                server_conf.mgroup = optarg;
                break;
            case 'P':
                server_conf.rcvport = optarg;
                break;
            case 'F':
                server_conf.runmode = RUN_FOREGROUND;
                break;
            case 'D':
                server_conf.media_dir = optarg;
                break;
            case 'I':
                server_conf.ifname = optarg;
                break;
            case 'H':
                printfhelp();
                exit(0);
                break;
            default:
                abort();
                break;
        }
    }


    /*守护进程的实现*/
    if(server_conf.runmode == RUN_DAEMON)
    {
         if(daemonize() != 0)           //将当前的进程实现为一个守护进程
             exit(1);
    }
    else if(server_conf.runmode == RUN_FOREGROUND)
    {
        /*do nothing*/
    }
    else{
        //fprintf(stderr,"EINVAL\n");  //表示参数非法
        syslog(LOG_ERR,"EINVAL server_conf.runmode.");
        exit(1);
    }

    /*SOCKET初始化*/

    /*获取频道信息*/

    /*创建节目单线程*/

    /*创建频道线程*/

    while(1)
        pause();


    exit(0);
}
