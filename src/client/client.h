#ifndef CLIENT_H__
#define CLIENT_H__

#define DEFAULT_PLAYERCMD "/usr/bin/mpg123 - > /dev/null"

struct client_conf_st
{
    char* rcvport;
    char* mgroup;      //多播地址
    char* player_cmd;  //命令行传输
};

extern struct client_conf_st client_conf; //声明,别的.c在包了当前.h之后也能够看到该全局变量，extren扩展了当前全局变量的作用域

//Makefile中哪句话的含义：不管原来的编译属性是什么样，-I表示指定当前头文件所在的路径，如果标准的include路径下没有，就会在你指定的路径下查找

#endif
