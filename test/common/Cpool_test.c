#include <Cpool_api.h>
#include <stdio.h>
#include <errno.h>

#define NPOOL 2
#define PROCS_PER_POOL 2
#define TIMEOUT 2
void *testit(void *);

int main() {
  int pid;
  int i, j;
  int ipool[NPOOL];
  int npool[NPOOL];
  int *arg;

  pid = getpid();

  printf("... Defining %d pools with %d elements each\n",
         NPOOL,PROCS_PER_POOL);

  for (i=0; i < NPOOL; i++) {
    if ((ipool[i] = Cpool_create(PROCS_PER_POOL,&(npool[i]))) < 0) {
      printf("### Error No %d creating pool (%s)\n",
             errno,strerror(errno));
    } else {
      printf("... Pool No %d created with %d processes\n",
             ipool[i],npool[i]);
    }
  }

  for (i=0; i < NPOOL; i++) {
    /* Loop on the number of processes + 1 ... */
    for (j=0; j <= npool[i]; j++) {
      if ((arg = malloc(sizeof(int))) == NULL) {
        printf("### Malloc error, errno = %d (%s)\n",
               errno,strerror(errno));
        continue;
      }
      *arg = i*10+j;
      printf("... Assign to pool %d (timeout=%d) the %d-th routine 0x%x(%d)\n",
             ipool[i],TIMEOUT,j+1,(unsigned int) testit,*arg);
      if (Cpool_assign(ipool[i], testit, arg, TIMEOUT)) {
        printf("### Can't assign to pool No %d (errno=%d [%s]) the %d-th routine\n",
               ipool[i],errno,strerror(errno),j);
        free(arg);
      } else {
        printf("... Okay for assign to pool No %d of the %d-th routine\n",
               ipool[i],j);
#ifndef _CTHREAD
        /* Non-thread environment: the child is in principle not allowed */
        /* to do free himself                                            */
        free(arg);
#endif
      }
    }
  }
  
  /* We wait enough time for our threads to terminate... */
  sleep(TIMEOUT*NPOOL*PROCS_PER_POOL);

  exit(EXIT_SUCCESS);
}

void *testit(void *arg) {
  int caller_pid, my_pid;

  my_pid = getpid();

  caller_pid = (int) * (int *) arg;

#ifdef _CTHREAD
  /* Thread environment : we free the memory */
  free(arg);
#endif

  printf("... I am PID=%d called by pool %d, try No %d\n",
         my_pid,caller_pid/10,caller_pid - 10*(caller_pid/10));

  /*
   * Wait up to the timeout + 1
   */
  sleep(TIMEOUT*2);

  return(NULL);
}




