#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include "kernel/riscv.h"

#define SIZE (8 * 4096)

char cmp[] = "This may help.";

int
main(int argc, char *argv[])
{
  
  char *saddr, *naddr;
  saddr = sbrk(0);

  while ((naddr = sbrk(SIZE)) != (char *)-1)
  {

    for (; saddr <= naddr - 15; ++saddr)
    {

      if (!strcmp(saddr, cmp))
      {

        sbrk(SIZE);
        if (!strcmp(saddr + 16, "(null)")) continue;
        printf("%s\n", saddr + 16);
        exit(0);
        
      }

    }

  }

  printf("attack: can't found\n");
  exit(1);
}
