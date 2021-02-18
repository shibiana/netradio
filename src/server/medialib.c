#include <stdio.h>
#include <stdlib.h>

#include "medialib.h"

struct channel_context_st
{
    chnid_t chnid;
    char* desc;
    //解析目录
    glob_t mp3glob;
    int pos;
    int pd;
    off_t offset; //播放的文件数据是一段段发出的
    mytbf_t* tbf;//流量控制
};
