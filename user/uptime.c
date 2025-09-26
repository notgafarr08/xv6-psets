#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(void)
{
  int ticks = uptime();   // call the system call
  printf("Uptime in ticks: %d\n", ticks);
  exit(0);
}

