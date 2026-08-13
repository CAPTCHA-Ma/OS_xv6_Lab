#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

void memdump(char *fmt, char *data);

char dtoh(int num)
{

  if (num >= 0 && num <= 9) return (char)('0' + num);
  else return (char)('A' + num - 10);

}

int
main(int argc, char *argv[])
{
  if(argc == 1){
    printf("Example 1:\n");
    int a[2] = { 61810, 2025 };
    memdump("ii", (char*) a);
    
    printf("Example 2:\n");
    memdump("S", "a string");
    
    printf("Example 3:\n");
    char *s = "another";
    memdump("s", (char *) &s);

    struct sss {
      char *ptr;
      int num1;
      short num2;
      char byte;
      char bytes[8];
    } example;
    
    example.ptr = "hello";
    example.num1 = 1819438967;
    example.num2 = 100;
    example.byte = 'z';
    strcpy(example.bytes, "xyzzy");
    
    printf("Example 4:\n");
    memdump("pihcS", (char*) &example);
    
    printf("Example 5:\n");
    memdump("sccccc", (char*) &example);
  } else if(argc == 2){
    // format in argv[1], up to 512 bytes of data from standard input.
    char data[512];
    int n = 0;
    memset(data, '\0', sizeof(data));
    while(n < sizeof(data)){
      int nn = read(0, data + n, sizeof(data) - n);
      if(nn <= 0)
        break;
      n += nn;
    }
    memdump(argv[1], data);
  } else {
    printf("Usage: memdump [format]\n");
    exit(1);
  }
  exit(0);
}

void
memdump(char *fmt, char *data)
{
  // Your code here.

  char *f = fmt;
  char *p = data;

  while (*f != '\0')
  {

    switch (*f)
    {

    case 'i':
    {

      int sum = 0, base = 1;
      for (int i = 0; i < 4; ++i)
      {
        
        int tmp = (int)*p;
        sum += tmp * base;
        ++p;
        base *= 256;

      }

      printf("%d\n", sum);

      break;

    }

    case 'p':
    {      
      
      char ans[16];
      for (int i = 0; i < 8; ++i)
      {

        int l = (int)*p / 16,
          r = (int)*p % 16;

        
        ans[2 * i + 1] = dtoh(l);
        ans[2 * i] = dtoh(r);

        ++p;

      }

      int i = 15;
      for (; i >= 0; i--)
      {

        if (ans[i] != '0') break;

      }

      if (i == 16) printf("0\n");
      else
      {

        for (; i >= 0; i--) printf("%c", ans[i]);
        printf("\n");

      }

      break;
    
    }

    case 'h':
    {

      int sum = 0, base = 1;
      for (int i = 0; i < 2; ++i)
      {

        sum += (int)*p * base;
        base *= 256;

        ++p;

      }

      printf("%d\n", sum);

      break;

    }

    case 'c':
    {

      printf("%c\n", *p);
      ++p;

      break;

    }

    case 's':
    {

      printf("%s\n", *(char **)p);

      p += 8;

      break;

    }

    case 'S':
    {

      printf("%s\n", p);

      break;

    }

    }

    ++f;

  }

}
