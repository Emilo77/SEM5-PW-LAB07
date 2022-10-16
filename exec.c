#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "err.h"

int main ()
{
  pid_t pid;
  
  switch (pid = fork()) {                     /* tworzenie procesu potomnego */
    case -1: 
      syserr("Error in fork\n");
   
    case 0:                                   /* proces potomny */
 
      printf("I am a child and my pid is %d\n", getpid());      

      execlp("ps", "ps", NULL);               /* wykonuje program ps */
      syserr("Error in execlp\n");
    
    default:                                  /* proces macierzysty */

      printf("I am a parent and my pid is %d\n", getpid());
      
      if (wait(0) == -1) syserr("Error in wait\n");
                                              /* czeka na zakończenie potomka */
      return 0;
  } 
}

