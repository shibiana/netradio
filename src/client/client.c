#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <getopt.h>
#include <net/if.h>

#include <proto.h>
#include "client.h"
/*
 * -M --mgroup  指定多播组
 * -P --port    指定接收端口
 * -p --player  指定播放器
 * -H --help    显示帮助
* */

struct client_conf_st client_conf ={\
        .rcvport = DEFAULT_RCVPORT,\
        .mgroup  = DEFAULT_MGROUP,\
        .player_cmd = DEFAULT_PLAYERCMD};                            

static void printhelp(void)
{
    printf("-P --port  zd port\n");
    printf("-M --mgroup zd dbz\n");
    printf("-p -- player zd mlh\n");
    printf("-H --help show help\n");
}

static ssize_t writen(int fd, const char* buf, size_t len)
{
    int pos = 0;
    int ret;
    while(len > 0)
    {
        ret = write(fd,buf+pos,len);
        if(ret < 0)
        {
            if(errno == EINTR)
            {
                continue;
            }
            perror("write()");
            return -1;
        }
        len -= ret;
        pos += ret;
    }
    return pos;
}

int main(int argc, char* argv[])
{
    int pd[2];//管道的两端
    int val;
    struct sockaddr_in laddr,serveraddr,raddr;
    socklen_t serveraddr_len,raddr_len;
    int c;
    pid_t pid;
    int sd;
    int index = 0;
    int chosenid;
    int len,ret;
    struct ip_mreqn mreq;
    struct option argarr[] = {{"port",1,NULL,'P'},{"mgroup",1,NULL,'M'},\
                              {"player",1,NULL,'p'},{"help",0,NULL,'H'},\
                             {NULL,0,NULL,0}};                           //最后一个表示当前结构体类型的结束
    /*初始化
    * 级别： 默认值，配置文件，环境变量，命令行参数（越到后面级别越高，如果有后面的值，就看后面的而忽略前面的值）
    *
    * */
    while(1)
    {
        //命令行解析
        c = getopt_long(argc,argv,"P:M:p:H",argarr,&index); //可以man getopt_long:第三个参数表示需要在argv中找到什么样的内容（字符串）
        if(c < 0)
            break;
        switch(c)
        {
            case 'P':
                client_conf.rcvport = optarg;
                break;
            case 'M':
                client_conf.mgroup = optarg;
                break;
            case 'p':
                client_conf.player_cmd = optarg;
                break;
            case 'H':
                printhelp();
                exit(0);
                break;
            default:
                abort();//自己给自己发送一个结束信号
                break;
        }
    } 


    sd = socket(AF_INET,SOCK_DGRAM,0);
    if(sd < 0)
    {
        perror("socket()");
        exit(1);
    }

    inet_pton(AF_INET,client_conf.mgroup,&mreq.imr_multiaddr);                                         //addr要存到imr_multiaddr中去
    /*if error校验*/
    inet_pton(AF_INET,"0.0.0.0",&mreq.imr_address);                                                    //当前本地的地址，"0.0.0.0"表示any addr
    mreq.imr_ifindex = if_nametoindex("ens33");                                                        //当前设备的索引号，指定走哪个宏


    if(setsockopt(sd,IPPROTO_IP,IP_ADD_MEMBERSHIP,&mreq,sizeof(mreq))<0)                                //更改属性,加入多播组 man 7 ip属于ip层属性
    {
        perror("setsockopt()");
        exit(1);
    }


    val = 1;   
    if(setsockopt(sd,IPPROTO_IP,IP_MULTICAST_LOOP,&val,sizeof(val)) < 0)
    {
        perror("setsockopt()");
        exit(1);
    }

    laddr.sin_family = AF_INET;
    laddr.sin_port = htons(atoi(client_conf.rcvport));                                              //将来要通过网络上传自己的ip端口信息htons(l)
    inet_pton(AF_INET,"0.0.0.0",&laddr.sin_addr);

    if(bind(sd,(void*)&laddr,sizeof(laddr))<0)                                                          //绑定本地的ip地址，也在man 7 ip中
    {
        perror("bind()");
        exit(1);
    }

    if(pipe(pd) < 0)
    {
        perror("pipe()");
        exit(1);
    }

    pid = fork();
    if(pid < 0)
    {
        perror("fork()");
        exit(1);
    }

    if(pid == 0)
    {
        //子进程：调用解码器，而解码器只能调用标准输入来的内容
Unable to exec gcc.real: No such file or directory
        close(sd);                            //子进程不需要用到解码器，只需要从管道的读端读取数据就行
        close(pd[1]);                         //关闭自身暂时不需要使用的写的pd[1]
        dup2(pd[0],0);                        //将管道的读端重定向到标准输入的位置
        if(pd[0] > 0)
            close(pd[0]);
        execl("/bin/sh","sh","-c",client_conf.player_cmd,NULL);
        perror("execl()");
        exit(1);
    }


    //parent
    //父进程：从网络上收包，发送给子进程
    //先收节目单
    struct msg_list_st* msg_list;

    msg_list = malloc(MSG_CHANNEL_MAX);
    if(msg_list == NULL)
    {
        perror("malloc()");
        exit(1);
    }

    len = recvfrom(sd, msg_list, MSG_LIST_MAX,0,(void *)&serveraddr,&serveraddr_len);            //后两个参数是对端地址和对端地址的长度
    
    while(1)
    {
        if(len < sizeof(struct msg_list_st))
        {
            fprintf(stderr,"message is too small.\n");
            continue;
        }
        if(msg_list->chnid != LISTCHNID)
        {
            fprintf(stderr,"chnid is not match!\n");
            continue;
        }
        break;
    }
    //打印节目单并选择频道
    struct msg_listentry_st* pos;
    for(pos = msg_list->entry;(char*)pos < (((char*)msg_list)+len);pos =(void*)(((char*)pos)+ntohs(pos->len)))
    {
        printf("channel %d:%s\n",pos->chnid,pos->desc);
    }
    
    free(msg_list);
    while(1)
    {
        ret = scanf("%d",&chosenid);
        if(ret != 1)
            exit(1);
    }
    //收频道包，发送给子进程
    struct msg_channel_st* msg_channel;
    msg_channel = malloc(MSG_CHANNEL_MAX);
    if(msg_channel == NULL)
    {
        perror("malloc()");
        exit(1);
    }

    while(1)
    {
        len=recvfrom(sd,msg_channel,MSG_CHANNEL_MAX,0,(void*)&raddr,&raddr_len);
        if(raddr.sin_addr.s_addr != serveraddr.sin_addr.s_addr || raddr.sin_port != serveraddr.sin_port)
        {
            fprintf(stderr,"Ignore:address not match!\n");
            continue;
        }

        if(len < sizeof(struct msg_channel_st))
        {
            fprintf(stderr,"Ignore:message too small!\n");
            continue;
        }

        if(msg_channel->chnid == chosenid)
        {
            fprintf(stdout,"accpeted msg:%d recieved.\n",msg_channel->chnid);
            if(writen(pd[1],msg_channel->data,len-sizeof(chnid_t))<0)
            {
                exit(1);
            }
        }
    }

    free(msg_channel);
    close(sd);

    exit(0);
}
