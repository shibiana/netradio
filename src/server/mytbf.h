#ifndef MYTBF_H__
#define MYTBF_H__

#define MYTBF_MAX 1024
typedef void mytbf_t;

mytbf_t* mytbf_init(int cps,int burst);//速率和上限

int mytbf_fetchtoken(mytbf_t*, int);//int表示要取到多少令牌桶

int mytbf_returntoken(mytbf_t*, int);

int mytbf_destroy(mytbf_t*);

#endif
