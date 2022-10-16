#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "err.h"

int main ()
{
  printf("Hello from %d\n", getpid());

  if (fork() == -1) syserr("Error in fork\n");  /* powstaje nowy proces */

  printf("Goodbye from %d\n", getpid());
  
  return 0;  
}

