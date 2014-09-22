#ifndef _CPMUTEX_
#define _CPMUTEX_

#include <pthread.h>
#include <assert.h>

typedef struct cpmutex cpmutex;

struct cpmutex {
  pthread_mutex_t outer;
  pthread_mutex_t inner;
  size_t consumers;
  size_t producers;
  pthread_cond_t consumer_cond;
  pthread_cond_t producer_cond;
};

int cpmutex_init(cpmutex *m);
int cpmutex_c_lock(cpmutex *m);
int cpmutex_p_lock(cpmutex *m);
int cpmutex_unlock(cpmutex *m);

#endif
