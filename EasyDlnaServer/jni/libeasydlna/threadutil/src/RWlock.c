#include "RWlock.h"

int easydlna_rwlock_init(easydlna_rwlock_t* prwlock,const pthread_mutexattr_t* pinit)
{
	prwlock->nreadcount = 0;
	pthread_mutex_init(&prwlock->lock,pinit);
	return pthread_mutex_init(&prwlock->wlock,pinit);
};
int easydlna_rwlock_rdlock(easydlna_rwlock_t *plock)
{
	pthread_mutex_lock(&plock->lock);
	if(++plock->nreadcount == 1)
		pthread_mutex_lock(&plock->wlock);

	return pthread_mutex_unlock(&plock->lock);
};
int easydlna_rwlock_wrlock(easydlna_rwlock_t *plock)
{
	return pthread_mutex_lock(&plock->wlock);
};
int easydlna_rwlock_unlock(easydlna_rwlock_t* plock, int nreadlock)
{
	if(nreadlock == 0)
	{
		return pthread_mutex_unlock(&plock->wlock);
	}
	else
	{
		pthread_mutex_lock(&plock->lock);
		if(--plock->nreadcount == 0)
			pthread_mutex_unlock(&plock->wlock);
		return pthread_mutex_unlock(&plock->lock);
	}
};
int easydlna_rwlock_destroy(easydlna_rwlock_t * prwlock)
{
	pthread_mutex_destroy(&prwlock->lock);
	prwlock->nreadcount = 0;
	return pthread_mutex_destroy(&prwlock->wlock);
};
