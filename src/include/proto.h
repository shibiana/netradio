#ifndef PROTO_H__
#define PROTO_H__

#include <site_type.h>

#define DEFAULT_MGROUP  "224.2.2.2"             //多播组和地址可任意指定值
#define DEFAULT_RCVPORT "1989"

#define CHNNR            100                     //总体的频道号

#define LISTCHNID        0                       //0号频道往外发送节目单，即当前的节目单所在的频道

#define MINCHNID         1                       //最小的开始频道

#define MAXCHNID         (MINCHNID+chnnr-1)      //最大的频道

#define MSG_CHANNEL_MAX （65536-20-8）         //udp报文段的推荐长度65536 IP包的报头（20-60）UDP包的报头8
#define MAX_DATA         (MSG_CHNNEL_MAX-sizeof(chnid_t))

#define MSG_LIST_MAX     (65536-20-8)          //list包的上限值
#define MAX_ENTRY        (MSG_LIST_MAX-sizeof(chnid_t))

struct msg_channel_st                          //两种包：往外发送数据的包
{
    chnid_t chnid; //0-255 表示id号
    uint8_t data[1]; //变长数组 占位
    
}__attribute__((packed)); //这个结构不需要对齐,UDP包不能考虑对齐

struct msg_listentry_st                        
{
    chnid_t chnid; 
    uint16_t len;                              //自述该结构体的大小
    uint8_t desc[1];                           //节目描述
}__attribute((packed));                        //每条记录的包

struct msg_list_st                             //节目单这个包
{
    chnid_t chnid;                             /*must be LISTCHNID*/
    struct msg_listentry_st entry[1];          //变长来实现

}__attribute__((packed));





#endif
