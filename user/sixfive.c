#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

char buf[1024];

void match(char *p, int l)
{

    if (l == 0) return;

    while (*p == '0')
    {

        ++p, --l;

    }

    if (l == 0) return;

    if (*(p + l - 1) == '0' || *(p + l - 1) == '5')
    {

        write(1, p, l);
        printf("\n");

        return;

    }

    int tmp = 0;
    for (char *t = p; t < p + l; ++t)
    {

        tmp *= 10;
        tmp += *t - '0';
        tmp %= 6;

    }

    if (!tmp)
    {

        write(1, p, l);
        printf("\n");

        return;

    }

}

void process_file(char *name)
{

    int fd = open(name, O_RDONLY);

    if (fd < 0)
    {

        fprintf(2, "sixfive: cannot open %s\n", name);
        return;

    }

    char c;
    int flag = 0, t = 0;
    while (read(fd, &c, 1))
    {

        if (!flag)
        {

            if (strchr(" -\r\t\n./,", c))
            {

                match(buf, t);
                t = 0;

            }
            else if (c < '0' || c > '9')
            {

                flag = 1;
                t = 0;

            }
            else
            {

                buf[t++] = c;

            }

        }
        else if (strchr(" -\r\t\n./,", c))
        {

            flag = 0;

        }

    }

    if (!flag) match(buf, t);

    close(fd);

}

int main(int argc, char *argv[])
{

    if (argc < 2)
    {

        fprintf(2, "usage: sixfive [file ...]\n");
        exit(1);
        
    }

    for (int i = 1; i < argc; ++i)
        process_file(argv[i]);

    exit(0);

}
