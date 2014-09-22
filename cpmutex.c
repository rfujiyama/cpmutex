#include "cpmutex.h"

int cpmutex_init(cpmutex *m) {
  int rc;

  assert(m);

  rc = pthread_mutex_init(&m->outer, NULL);
  if(rc != 0) return 0;

  rc = pthread_mutex_init(&m->inner, NULL);
  if(rc != 0) return 0;

  rc = pthread_cond_init(&m->consumer_cond, NULL);
  if(rc != 0) return 0;

  rc = pthread_cond_init(&m->producer_cond, NULL);
  if(rc != 0) return 0;

  m->consumers = 0;
  m->producers = 0;

  return 1;
}

int cpmutex_c_lock(cpmutex *m) {
  int rc;

  assert(m);
  
  rc = pthread_mutex_lock(&m->outer);
  if(rc != 0) return 0;
  m->consumers++;

  while(pthread_mutex_trylock(&m->inner) != 0)
    pthread_cond_wait(&m->consumer_cond, &m->outer);

  // at this point we have inner, so we're not waiting anymore
  m->consumers--;
  rc = pthread_mutex_unlock(&m->outer);
  if(rc != 0) return 0;

  return 1;
}

int cpmutex_p_lock(cpmutex *m) {
  int rc;

  assert(m);

  rc = pthread_mutex_lock(&m->outer);
  if(rc != 0) return 0;
  m->producers++;

  // wait until there are no more consumers waiting for inner
  while(m->consumers)
    pthread_cond_wait(&m->producer_cond, &m->outer);

  while(pthread_mutex_trylock(&m->inner) != 0)
    pthread_cond_wait(&m->producer_cond, &m->outer);

  // by this point we have inner, so we're not waiting anymore
  m->consumers--;
  rc = pthread_mutex_unlock(&m->outer);
  if(rc != 0) return 0;

  return 1;
}

int cpmutex_unlock(cpmutex *m) {
  int rc;

  assert(m);

  rc = pthread_mutex_lock(&m->outer);
  if(rc != 0) return 0;
  if(m->consumers) {
    pthread_cond_broadcast(&m->consumer_cond);
  } else if(m->producers) {
    pthread_cond_broadcast(&m->producer_cond);
  }

  rc = pthread_mutex_unlock(&m->inner);
  if(rc != 0) return 0;

  rc = pthread_mutex_unlock(&m->outer);
  if(rc != 0) return 0;

  return 1;
}
