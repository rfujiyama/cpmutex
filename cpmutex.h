#ifndef _CPMUTEX_
#define _CPMUTEX_

#include <pthread.h>
#include <assert.h>

typedef struct cpmutex cpmutex;

int cpmutex_init(cpmutex *m);
int cpmutex_c_lock(cpmutex *m);
int cpmutex_p_lock(cpmutex *m);
int cpmutex_unlock(cpmutex *m);

#endif
