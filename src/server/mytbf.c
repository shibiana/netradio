#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <pthread.h>
#include "mytbf.h"

struct mytbf_st
{
    int cps;
    int burst;
    int token;
    int pos;
    pthread_mutex_t mut;
    pthread_cond_t cond;
};

static struct mytbf_st* job[MYTBF_MAX];
static pthread_mutex_t mut_job = PTHREAD_MUTEX_INITIALIZER;

static int get_free_pos_unlocked(void)
{
    int i;
    for (i = 0; i< MYTBF_MAX; i++)
    {
        if(job[i] == NULL)
        {
            return i;
        }
    }
    return -1;
}


mytbf_t* mytbf_init(int cps, int burst)
{
    struct mytbf_st* me;
    me = malloc(sizeof(*me));
    if(me == NULL)
    {
        return NULL;
    }
    me->caps = caps;
    me->burst = burst;
    me->token = 0;
    pthread_mutex_init(&me->mut,NULL);
    pthread_cond_init(&me->cond,NULL);

    pthread_mutex_lock(&mut_job);
    pos = get_free_pos_unlocked();
 
    if(pos < 0)
    {
        pthread_mutex_unlock(&mut_job);
        free(me);
        return NULL;
    }
    me->pos = pos;
    job[me->pos] = me;
    pthread_mutex_unlock(&mut_job);

    return me;
}

int mytbf_fetchtoken(mytbf_t*, int)
{
    
}

int mytbf_returntoken(mytbf_t*,int)
{
    
}

int mytbf_destroy(mytbf_t*)
{
    struct mytbf_st* me = ptr;

    pthread_mutex_lock(&mut_job);
    job[me->pos] = NULL;
    pthread_mutex_unlock(&mut_job);

    pthread_mutex_destroy(&me->nut);
    pthread_cond_destroy(&me->cond);
    free(ptr);
    return 0;
}
