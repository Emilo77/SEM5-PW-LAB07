#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "err.h"

#define NR_PROC 5

int main ()
{
  pid_t pid;
  int i; 

  /* tworzenie procesów potomnych */
  for (i = 1; i <= NR_PROC; i++)
    switch (pid = fork()) {
      case -1: 
        syserr("Error in fork\n");

      case 0: /* proces potomny */
 
        printf("I am a child and my pid is %d\n", getpid());
        return 0;
    
    default: /* proces macierzysty */

      printf("I am a parent and my pid is %d\n", getpid());
    } 
         
  /* czekanie na zakończenie procesow potomnych */
  for (i = 1; i <= NR_PROC; i++)
    if (wait(0) == -1) 
      syserr("Error in wait\n");
  
  return 0;
  
}
