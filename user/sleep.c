#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{

    if (argc != 2) 
    {

        fprintf(2, "usage: sleep [num]\n");
        exit(1);

    }
    else
    {

        int num = atoi(argv[1]);

        pause(num);

    }

    exit(0);

}