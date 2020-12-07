#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <unistd.h>

void printPermission(mode_t buf)
{
    printf((buf & S_IRUSR) ? "r" : "-");
    printf((buf & S_IWUSR) ? "w" : "-");
    printf((buf & S_IXUSR) ? "x" : "-");
    printf((buf & S_IRGRP) ? "r" : "-");
    printf((buf & S_IWGRP) ? "w" : "-");
    printf((buf & S_IXGRP) ? "x" : "-");
    printf((buf & S_IROTH) ? "r" : "-");
    printf((buf & S_IWOTH) ? "w" : "-");
    printf((buf & S_IXOTH) ? "x" : "-");
}

void ls(char *dir, int type)
{
    DIR *dirStream = opendir(dir);
    if (dirStream == NULL)
    {
        printf("Error opening file %s\n", dir);
        return;
    }

    struct dirent *dr;
    dr = readdir(dirStream);
    while (dr != NULL)
    {
        if (dr->d_name[0] != '.')
        {
            if (type == 1)
            {
                printf("%lu  ", dr->d_ino);
            }
            if (dr->d_type == DT_DIR)
            {
                printf("'%s'  ", dr->d_name);
            }
            else
            {
                printf("%s  ", dr->d_name);
            }
        }
        dr = readdir(dirStream);
    }
    printf("\n");
}

void ls_l(char *dir)
{
    DIR *dirStream = opendir(dir);
    if (dirStream == NULL)
    {
        printf("Error opening file %s\n", dir);
        return;
    }

    struct dirent *dr;
    dr = readdir(dirStream);
    struct stat buf;
    // struct passwd username;
    // struct group groupname;
    char time[20];
    struct tm *timeinfo;
    char *realPath;
    while (dr != NULL)
    {
        lstat(dr->d_name, &buf);
        timeinfo = localtime(&buf.st_mtime);
        if (dr->d_name[0] != '.')
        {
            if (S_ISLNK(buf.st_mode))
            {
                printf("l");
                realPath = malloc(buf.st_size + 1);
                ssize_t nbytes = readlink(dr->d_name, realPath, buf.st_size + 1);
                if (nbytes == -1)
                {
                    printf("Error Readlink\n");
                    exit(1);
                }
                realPath[buf.st_size] = '\0';
            }
            else
            {
                printf((S_ISDIR(buf.st_mode)) ? "d" : "-");
            }
            printPermission(buf.st_mode);
            printf("%14lu", buf.st_nlink);
            printf("%14s", getpwuid(buf.st_uid)->pw_name);
            printf("%14s", getgrgid(buf.st_uid)->gr_name);
            printf("%14lu", buf.st_size);
            strftime(time, sizeof(time), "%b %m %y %H:%M", timeinfo);
            printf("%14s", time);
            if (dr->d_type == DT_DIR)
            {
                printf("%14s%s'", "'", dr->d_name);
            }
            else
            {
                printf("%14s", dr->d_name);
                if (S_ISLNK(buf.st_mode))
                {
                    printf(" -> %s", realPath);
                }
            }
            printf("\n");
        }
        dr = readdir(dirStream);
    }
    free(realPath);
}

void ls_R(char *dir)
{
    ls(dir, 0);
    DIR *dirStream = opendir(dir);
    if (dirStream == NULL)
    {
        printf("Error opening file %s\n", dir);
        return;
    }

    struct dirent *dr;
    dr = readdir(dirStream);
    while (dr != NULL)
    {
        if (dr->d_name[0] != '.')
        {
            if (dr->d_type == DT_DIR)
            {
                ls_R(dr->d_name);
            }
        }
        dr = readdir(dirStream);
    }
    printf("\n");
}

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        ls(".", 0);
    }
    else if (argc == 2)
    {
        if (strcmp(argv[1], "-i") == 0)
        {
            ls(".", 1);
        }
        else if (strcmp(argv[1], "-l") == 0)
        {
            ls_l(".");
        }
        else if (strcmp(argv[1], "-R") == 0)
        {
            printf("-R\n");
        }
        else
        {
            ls(argv[1], 0);
        }
    }
    else
    {
        if (strcmp(argv[1], "-i") == 0)
        {
            for (int i = 2; i < argc; i++)
            {
                printf("%s: \n", argv[i]);
                ls(argv[i], 1);
            }
        }
        else if (strcmp(argv[1], "-l") == 0)
        {
            printf("-l\n");
        }
        else if (strcmp(argv[1], "-R") == 0)
        {
            printf("-R\n");
        }
        else
        {
            for (int i = 1; i < argc; i++)
            {
                printf("%s: \n", argv[i]);
                ls(argv[i], 0);
            }
        }
    }
}