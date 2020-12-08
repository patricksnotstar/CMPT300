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

int selectedOptions[3];
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

void ls(char *dir)
{
    DIR *dirStream = opendir(dir);
    if (dirStream == NULL)
    {
        printf("Error opening file %s\n", dir);
        exit(1);
    }

    struct dirent *dr;
    dr = readdir(dirStream);
    while (dr != NULL)
    {
        if (dr->d_name[0] != '.')
        {
            if (dr->d_type == DT_DIR)
            {
                printf("'%s'\n", dr->d_name);
            }
            else
            {
                printf("%s\n", dr->d_name);
            }
        }
        dr = readdir(dirStream);
    }
    closedir(dirStream);
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
    struct stat buf;
    char time[20];
    struct tm *timeinfo;
    char *realPath;
    int freeFlag = 0;
    while ((dr = readdir(dirStream)) != NULL)
    {
        if (dr->d_name[0] != '.')
        {
            char *path = malloc(sizeof(dir) + sizeof(dr->d_name) + 1);
            sprintf(path, "%s/%s", dir, dr->d_name);
            if (lstat(path, &buf) == -1)
            {
                printf("Cannot read file %s\n", dr->d_name);
                exit(1);
            }
            timeinfo = localtime(&buf.st_mtime);
            if (selectedOptions[0] == 1)
            {
                printf("%lu ", dr->d_ino);
            }
            if (selectedOptions[1] == 1)
            {
                if (S_ISLNK(buf.st_mode))
                {
                    freeFlag = 1;
                    printf("l");
                    realPath = malloc(buf.st_size + 1);
                    ssize_t nbytes = readlink(path, realPath, buf.st_size + 1);
                    if (nbytes == -1)
                    {
                        printf("Cannot read link\n");
                        exit(1);
                    }
                    realPath[buf.st_size] = '\0';
                }
                else
                {
                    printf((S_ISDIR(buf.st_mode)) ? "d" : "-");
                }
                printPermission(buf.st_mode);
                printf("%2lu", buf.st_nlink);
                printf("%14s", getpwuid(buf.st_uid)->pw_name);
                printf("%14s", getgrgid(buf.st_gid)->gr_name);
                printf("%9lu", buf.st_size);
                strftime(time, sizeof(time), "%b %m %Y %H:%M", timeinfo);
                printf(" %s ", time);
            }
            if (strchr(dr->d_name, ' ') != NULL)
            {
                printf("'%s'", dr->d_name);
            }
            else
            {
                printf("%s", dr->d_name);
            }

            if (selectedOptions[1] == 1 && S_ISLNK(buf.st_mode))
            {
                if (strchr(realPath, ' ') != NULL)
                {
                    printf(" -> '%s'", realPath);
                }
                else
                {
                    printf(" -> %s", realPath);
                }
            }
            printf("\n");
            free(path);
        }
        if (freeFlag == 1)
        {
            free(realPath);
            freeFlag = 0;
        }
    }

    closedir(dirStream);
}

void ls_R(char *dir)
{
    if (strchr(dir, ' ') != NULL)
    {
        printf("'%s': \n", dir);
    }
    else
    {
        printf("%s: \n", dir);
    }
    ls_l(dir);
    printf("\n");
    DIR *dirStream = opendir(dir);

    struct dirent *dr;
    dr = readdir(dirStream);
    while (dr != NULL)
    {
        if (dr->d_name[0] != '.')
        {
            if (dr->d_type == DT_DIR)
            {
                char *realPath = malloc(sizeof(dir) + sizeof(dr->d_name) + 1);
                if (dir[strlen(dir) - 1] == '/')
                {
                    sprintf(realPath, "%s%s", dir, dr->d_name);
                }
                else
                {
                    sprintf(realPath, "%s/%s", dir, dr->d_name);
                }
                ls_R(realPath);
                free(realPath);
            }
        }
        dr = readdir(dirStream);
    }
    closedir(dirStream);
}

int main(int argc, char *argv[])
{
    selectedOptions[0] = 0;
    selectedOptions[1] = 0;
    selectedOptions[2] = 0;
    if (argc == 1)
    {
        ls(".");
    }
    else
    {
        int lastOptionIdx = 0;
        for (int i = 1; i < argc; i++)
        {
            if (argv[i][0] == '-')
            {
                lastOptionIdx++;
                for (int j = 1; j < strlen(argv[i]); j++)
                {
                    switch (argv[i][j])
                    {
                    case 'i':
                        selectedOptions[0] = 1;
                        break;
                    case 'l':
                        selectedOptions[1] = 1;
                        break;
                    case 'R':
                        selectedOptions[2] = 1;
                        break;
                    default:
                        printf("Invalid option -- %c\n", argv[i][j]);
                        exit(1);
                    }
                }
            }
        }

        if (selectedOptions[2] == 1)
        {
            if (lastOptionIdx + 1 == argc)
            {
                ls_R(".");
            }
            else
            {
                for (int i = lastOptionIdx + 1; i < argc; i++)
                {
                    ls_R(argv[i]);
                }
            }
        }
        else
        {
            if (lastOptionIdx + 1 == argc)
            {
                ls_l(".");
            }
            else
            {
                for (int i = lastOptionIdx + 1; i < argc; i++)
                {
                    ls_l(argv[i]);
                }
            }
        }
    }
}