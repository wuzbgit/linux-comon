#ifndef __THREAD_H__
#define __THREAD_H__

#include <pthread.h>

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef void* (* ptr_thread_func ) (void * pParam);

    int32_t thread_create(const char* name, pthread_t * ptr_handle, ptr_thread_func ptr_func, void * ptr_param, unsigned int uFlag, int detach_enable, unsigned int stack_size);
    int32_t thread_cancel(pthread_t tid, bool hard);

#ifdef __cplusplus
}
#endif

#endif /*__THREAD_H__*/
