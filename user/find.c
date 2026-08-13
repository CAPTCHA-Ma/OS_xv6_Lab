#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include "kernel/fs.h"

void recurse(char *path, char *fm, char** argv)
{

    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)) return;

    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';

    if ((fd = open(path, O_RDONLY)) < 0) return;

    while (read(fd, &de, sizeof(de)) == sizeof(de))
    {

        if (de.inum == 0) continue;

        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;

        if (stat(buf, &st) < 0) continue;

        if (strcmp(p, ".") == 0 || strcmp(p, "..") == 0) continue;

        switch (st.type)
        {

        case T_DEVICE:
            break;

        case T_DIR:
        
            recurse(buf, fm, argv);
            break;

        case T_FILE:

            if (!strcmp(p, fm)) 
            {

                if (argv != 0)
                {

                    char *nargv[32];
                    int l = 0;

                    while (argv[l] != 0 && l < 31)
                    {

                        nargv[l] = argv[l];
                        ++l;

                    }

                    nargv[l++] = p;
                    nargv[l] = 0;
                    
                    if (fork() == 0) exec(nargv[0], nargv);

                    wait(0);

                }
                else printf("%s\n", buf);

            }
            break;

        }

    }

    close(fd);

}

int main(int argc, char *argv[])
{

    if (argc < 3 || argc == 4 || (argc > 4 && strcmp(argv[3], "-exec")))
    {

        fprintf(2, "usage: find [path] [name] (-exec cmd)\n");
        exit(1);

    }

    int fd;
    struct stat st;

    char *path = argv[1], *name = argv[2];
    if ((fd = open(path, O_RDONLY)) < 0)
    {

        fprintf(2, "find: cannot open %s\n", path);
        exit(1);

    }

    if (fstat(fd, &st) < 0)
    {

        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        exit(1);

    }

    if (st.type != T_DIR)
    {

        fprintf(2, "usage: find [path] [name]\n");
        close(fd);
        exit(1);

    }

    close(fd);

    if (argc == 3) recurse(path, name, 0);
    else recurse(path, name, &argv[4]);

}