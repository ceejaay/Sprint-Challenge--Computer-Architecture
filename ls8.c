#include <stdio.h>
#include "cpu.h"

/**
 * Main
 */
int main(int argc, char **argv)
{
/* int n = sizeof(argv); */
/* printf(" length of array => %d\n", n/sizeof(char)); */

  struct cpu cpu;

  cpu_init(&cpu);
  cpu_load(&cpu, argv, argc);
  cpu_run(&cpu);

  return 0;
  
}

/* extern char *argv; */
