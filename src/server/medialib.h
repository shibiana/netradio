#ifndef MEDIALIB_H__
#define MEDIALIB_H__

//获取频道信息的时候里面有哪些内容
struct mlib_listentry_st
{
  chnid_t  chnid;  //频道号
  //注意网络传输不会用指针作为参数传递  
  char* desc;
};

int mlib_getchnlist(struct mlib_listentry_st** , int *);
int mlib_freechnlist(struct mlib_listentry_st*);

ssize_t mlib_readchn(chnid_t, void*, size_t);
#endif
