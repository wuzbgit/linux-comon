#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <sched.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "thread.h"
#include "debug/debug.h"

enum{
    TRIES_TIMES       = 3,
};

#define PTHREAD_RET_CHK(ret, func)  { \
    if (ret != 0) {    \
        LOG_ERROR("ERROR: %s  failed with code %d (%s) \n",#func,ret,strerror(errno)); \
    } \
}

/*
 *1. scheduled policy: FIFO, RR, other?
 *2. scope: kernel or pure thread?
 */
/*
 *Return:
 *  0 if okay
 * -1 if failed
 */
int thread_create(const char* name, pthread_t *thread_id, ptr_thread_func ptr_func, void * ptr_param, unsigned int uFlag, int detach_enable, unsigned int stack_size)
{
    enum{
        MIN_STACKSIZE = 16*1024,
        
        PAGE_SHIFT    = 12,
        PAGE_SIZE     = (1UL << PAGE_SHIFT),
        PAGE_MASK     = (~(PAGE_SIZE-1))        
    };

#define PAGE_ALIGN(addr)    (((addr)+PAGE_SIZE-1)&PAGE_MASK)

	pthread_t 		tt;
	pthread_attr_t	attr;
    struct sched_param param;
    int             ret = 0;

    if(NULL == ptr_func){
        return -1;
    }

    /*init*/
	ret = pthread_attr_init(&attr);
    PTHREAD_RET_CHK(ret, pthread_attr_init);

	ret = pthread_attr_setstacksize(&attr, stack_size<MIN_STACKSIZE?MIN_STACKSIZE:PAGE_ALIGN(stack_size));
    PTHREAD_RET_CHK(ret, pthread_attr_setstacksize);

	ret = pthread_attr_setguardsize(&attr, PAGE_SIZE);
    PTHREAD_RET_CHK(ret, pthread_attr_setguardsize);

    ret = pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
    PTHREAD_RET_CHK(ret, pthread_attr_setscope);

    if(detach_enable){
        ret = pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED); 
        PTHREAD_RET_CHK(ret, pthread_attr_setdetachstate);
    }

	if (uFlag == 0) {
		ret = pthread_attr_setschedpolicy(&attr, SCHED_OTHER);
        PTHREAD_RET_CHK(ret, pthread_attr_setschedpolicy);
        
        if(0 == ret){
		    param.sched_priority = 0;

            ret = pthread_attr_setschedparam(&attr, &param);
            PTHREAD_RET_CHK(ret, pthread_attr_setschedparam);
        }
	}
    else {
		ret = pthread_attr_setschedpolicy(&attr, SCHED_RR);
        PTHREAD_RET_CHK(ret, pthread_attr_setschedpolicy);

        if(0 == ret){
            param.sched_priority = uFlag;

            ret = pthread_attr_setschedparam(&attr, &param);
            PTHREAD_RET_CHK(ret, pthread_attr_setschedparam);
            
            ret = pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
            PTHREAD_RET_CHK(ret, pthread_attr_setinheritsched);
        }
	}

    if(NULL != thread_id){
        ret = pthread_create(thread_id, &attr, (void*(*)(void*))ptr_func ,ptr_param);
        tt  = *thread_id;
    }
    else{
	    ret = pthread_create(&tt, &attr, (void*(*)(void*))ptr_func ,ptr_param);
    }

	if (ret != 0){
        int32_t lerrno = errno;
        LOG_ERROR("pthread_create: failed, ret=%d (%s)\n", ret,strerror(lerrno));
        errno = lerrno;
        return -1;
	}

    pthread_attr_destroy(&attr);
    PTHREAD_RET_CHK(ret, pthread_attr_destroy);

    if(NULL != name)
        pthread_setname_np(tt, name);

    if(NULL != thread_id)
	    *thread_id = tt;

    return 0;
}

/*
 *Return:
 *  0 if okay
 * -1 if failed
 */
int32_t qxwz_thread_cancel(pthread_t tid, bool hard)
{
    int32_t itimes = 0;
    
    if(0 != tid){
        if(hard){
            pthread_cancel(tid);
        }

        usleep(5000);

        for(itimes=0; itimes<TRIES_TIMES; itimes++){
            if(0 == pthread_kill(tid, 0)){
                usleep(1000*50);
            }
            else{
                break;
            }
        }
    }

    return (TRIES_TIMES == itimes) ? -1 : 0;
}

