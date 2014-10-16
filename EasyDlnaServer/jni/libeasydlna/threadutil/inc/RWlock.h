#ifndef _RWLOCK_H_
#define _RWLOCK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <pthread.h>


struct easydlna_rwlock_xt
{
	pthread_mutex_t lock;
	pthread_mutex_t wlock;
	int				nreadcount;
};

typedef struct easydlna_rwlock_xt easydlna_rwlock_t;

int easydlna_rwlock_init(easydlna_rwlock_t* prwlock,const pthread_mutexattr_t* pinit);
int easydlna_rwlock_destroy(easydlna_rwlock_t * rwlock);
int easydlna_rwlock_rdlock(easydlna_rwlock_t *plock);
int easydlna_rwlock_wrlock(easydlna_rwlock_t *plock);
int easydlna_rwlock_unlock(easydlna_rwlock_t* plock, int nreadlock);
int easydlna_rwlock_destroy(easydlna_rwlock_t * prwlock);

#ifdef __cplusplus
}
#endif

#endif
