// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <sys/types.h>
// #include <dirent.h>
// #include <sys/stat.h>
// #include <pwd.h>
// #include <grp.h>
// #include <time.h>
// #include <unistd.h>

// int selectedOptions[3];

// void printPermission(mode_t buf)
// {
//     printf((buf & S_IRUSR) ? "r" : "-");
//     printf((buf & S_IWUSR) ? "w" : "-");
//     printf((buf & S_IXUSR) ? "x" : "-");
//     printf((buf & S_IRGRP) ? "r" : "-");
//     printf((buf & S_IWGRP) ? "w" : "-");
//     printf((buf & S_IXGRP) ? "x" : "-");
//     printf((buf & S_IROTH) ? "r" : "-");
//     printf((buf & S_IWOTH) ? "w" : "-");
//     printf((buf & S_IXOTH) ? "x" : "-");
// }

// void ls(char *dir)
// {
//     DIR *dirStream = opendir(dir);
//     if (dirStream == NULL)
//     {
//         printf("Error opening file %s\n", dir);
//         exit(1);
//     }

//     struct dirent *dr;
//     dr = readdir(dirStream);
//     while (dr != NULL)
//     {
//         if (dr->d_name[0] != '.')
//         {
//             if (dr->d_type == DT_DIR)
//             {
//                 printf("'%s'  ", dr->d_name);
//             }
//             else
//             {
//                 printf("%s  ", dr->d_name);
//             }
//         }
//         dr = readdir(dirStream);
//     }
//     printf("\n");
//     closedir(dirStream);
// }

// void ls_l(char *dir)
// {
//     DIR *dirStream = opendir(dir);
//     if (dirStream == NULL)
//     {
//         printf("Error opening file %s\n", dir);
//         return;
//     }

//     struct dirent *dr;
//     // dr = readdir(dirStream);
//     struct stat buf;
//     char time[20];
//     struct tm *timeinfo;
//     char *realPath;
//     int freeFlag = 0;
//     // while (dr != NULL)
//     while ((dr = readdir(dirStream)) != NULL)
//     {
//         if (dr->d_name[0] != '.')
//         {
//             char *path = malloc(sizeof(dir) + sizeof(dr->d_name) + 1);
//             sprintf(path, "%s/%s", dir, dr->d_name);
//             if (lstat(path, &buf) == -1)
//             {
//                 printf("Cannot read file %s\n", dr->d_name);
//                 exit(1);
//             }
//             timeinfo = localtime(&buf.st_mtime);
//             if (selectedOptions[0] == 1)
//             {
//                 printf("%lu  ", dr->d_ino);
//             }
//             if (selectedOptions[1] == 1)
//             {
//                 if (S_ISLNK(buf.st_mode))
//                 {
//                     freeFlag = 1;
//                     printf("l");
//                     realPath = malloc(buf.st_size + 1);
//                     ssize_t nbytes = readlink(path, realPath, buf.st_size + 1);
//                     if (nbytes == -1)
//                     {
//                         printf("Error Readlink\n");
//                         exit(1);
//                     }
//                     realPath[buf.st_size] = '\0';
//                 }
//                 else
//                 {
//                     printf((S_ISDIR(buf.st_mode)) ? "d" : "-");
//                 }
//                 printPermission(buf.st_mode);
//                 printf("%14lu", buf.st_nlink);
//                 printf("%14s", getpwuid(buf.st_uid)->pw_name);
//                 printf("%14s", getgrgid(buf.st_gid)->gr_name);
//                 printf("%14lu", buf.st_size);
//                 strftime(time, sizeof(time), "%b %m %y %H:%M", timeinfo);
//                 printf("%14s", time);
//             }
//             if (dr->d_type == DT_DIR)
//             {
//                 printf("%14s%s'", "'", dr->d_name);
//             }
//             else
//             {
//                 printf("%14s", dr->d_name);
//                 if (selectedOptions[1] == 1 && S_ISLNK(buf.st_mode))
//                 {
//                     printf(" -> %s", realPath);
//                 }
//             }
//             printf("\n");
//             free(path);
//         }
//         // dr = readdir(dirStream);
//         if (freeFlag == 1)
//         {
//             free(realPath);
//             freeFlag = 0;
//         }
//         // char *path = malloc(sizeof(char) * 1028);
//         // sprintf(path, "%s/%s", dir, dr->d_name);
//         // if (lstat(path, &buf) == -1)
//         // {
//         //     printf("Cannot access '%s': File or directory doesn't exist!\n", dir);
//         //     exit(1);
//         // }
//         // if (dr->d_name[0] != '.')
//         // {
//         //     char _time[128];
//         //     int isSoftLink = 0;
//         //     struct passwd *pwd = getpwuid(buf.st_uid);
//         //     struct group *grp = getgrgid(buf.st_gid);
//         //     if (S_ISLNK(buf.st_mode))
//         //     {
//         //         isSoftLink = 1;
//         //     }
//         //     struct tm *tm = localtime(&buf.st_mtime);
//         //     strftime(_time, sizeof(_time), "%b %d %Y %H:%M", tm);
//         //     if (isSoftLink)
//         //     {
//         //         printf("l");
//         //     }
//         //     else
//         //     {
//         //         printf("%c", (S_ISDIR(buf.st_mode)) ? 'd' : '-');
//         //     }
//         //     printPermission(buf.st_mode);
//         //     if (!isSoftLink)
//         //     {
//         //         printf(" %ld %s %s %lld %s %s\n", (long)buf.st_nlink, pwd->pw_name, grp->gr_name, (long long)buf.st_size, _time, dr->d_name);
//         //     }
//         //     else
//         //     {
//         //         char *link = malloc(buf.st_size + 1);
//         //         ssize_t nbytes = readlink(path, link, buf.st_size + 1);
//         //         if (nbytes == -1)
//         //         {
//         //             printf("Error - Readlink failed\n");
//         //             exit(1);
//         //         }
//         //         link[buf.st_size] = '\0';
//         //         printf(" %ld %s %s %lld %s %s -> %s\n", (long)buf.st_nlink, pwd->pw_name, grp->gr_name, (long long)buf.st_size, _time, dr->d_name, link);
//         //         free(link);
//         //     }
//         // }
//         // free(path);
//     }

//     closedir(dirStream);
// }

// void ls_R(char *dir)
// {
//     printf("%s: \n", dir);
//     ls_l(dir);
//     printf("\n");
//     DIR *dirStream = opendir(dir);

//     struct dirent *dr;
//     dr = readdir(dirStream);
//     while (dr != NULL)
//     {
//         if (dr->d_name[0] != '.')
//         {
//             if (dr->d_type == DT_DIR)
//             {
//                 char *realPath = malloc(sizeof(dir) + sizeof(dr->d_name) + 1);
//                 sprintf(realPath, "%s/%s", dir, dr->d_name);
//                 ls_R(realPath);
//                 free(realPath);
//             }
//         }
//         dr = readdir(dirStream);
//     }
//     closedir(dirStream);
// }

// int main(int argc, char *argv[])
// {
//     selectedOptions[0] = 0;
//     selectedOptions[1] = 0;
//     selectedOptions[2] = 0;
//     if (argc == 1)
//     {
//         ls(".");
//     }
//     else
//     {
//         int lastOptionIdx = 0;
//         for (int i = 1; i < argc; i++)
//         {
//             if (argv[i][0] == '-')
//             {
//                 lastOptionIdx++;
//                 for (int j = 1; j < strlen(argv[i]); j++)
//                 {
//                     switch (argv[i][j])
//                     {
//                     case 'i':
//                         selectedOptions[0] = 1;
//                         break;
//                     case 'l':
//                         selectedOptions[1] = 1;
//                         break;
//                     case 'R':
//                         selectedOptions[2] = 1;
//                         break;
//                     }
//                 }
//             }
//         }

//         if (selectedOptions[2] == 1)
//         {
//             if (lastOptionIdx + 1 == argc)
//             {
//                 ls_R(".");
//             }
//             else
//             {
//                 for (int i = lastOptionIdx + 1; i < argc; i++)
//                 {
//                     ls_R(argv[i]);
//                 }
//             }
//         }
//         else
//         {
//             if (lastOptionIdx + 1 == argc)
//             {
//                 ls_l(".");
//             }
//             else
//             {
//                 for (int i = lastOptionIdx + 1; i < argc; i++)
//                 {
//                     ls_l(argv[i]);
//                 }
//             }
//         }
//     }
// }

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdbool.h>

#define LI 258
#define IR 232
#define LR 235
#define ILR 340

// Declare options
enum options
{
    ls,
    ls_i,
    ls_l,
    ls_R
};
enum options option;
enum _file_types
{
    dir,
    reg,
    soft_link
};
enum _file_types file_type;

void call_Ls(char *dir);
void call_Ls_I(char *dir);
void call_Ls_L(char *dir);
void call_Ls_R(char *dir);
char *_find_command(int argc, char *argv[]);
int _command_value(char *command);
void _print_protection_bits(struct stat _file_info);
void _run_command(char *command, char *argv);

char *curr_dir = ".";
int command_value = 0;

int main(int argc, char *argv[])
{

    if (argc == 1)
    {
        call_Ls(curr_dir);
    }
    else if (argc == 2)
    {
        char *command = _find_command(argc, argv);
        command_value = _command_value(command);
        _run_command(command, curr_dir);
    }
    else if (argc > 2)
    {
        char *command = _find_command(argc, argv);
        command_value = _command_value(command);
        bool _done_flag = false;

        for (int i = 1; i < argc; i++)
        {
            if (argv[i][0] != '-')
            {
                _run_command(command, argv[i]);
                _done_flag = true;
            }
        }
        if (!_done_flag)
        {
            _run_command(command, curr_dir);
        }
    }

    puts("");
    return 0;
}

void _run_command(char *command, char *argv)
{
    if (strcmp(command, "-i") == 0)
    {
        call_Ls_I(argv);
    }
    else if (strcmp(command, "-l") == 0)
    {
        call_Ls_L(argv);
    }
    else if (strcmp(command, "-R") == 0)
    {
        call_Ls_R(argv);
    }
    else if (command_value == LI)
    {
        call_Ls_L(argv);
    }
    else if (command_value == IR || command_value == LR || command_value == ILR)
    {
        call_Ls_R(argv);
    }
    else
    {
        call_Ls(argv);
    }
}

int _command_value(char *command)
{
    int value = 0;
    for (int i = 0; i < strlen(command); i++)
    {
        value += command[i];
    }
    return value;
}

char *_find_command(int argc, char *argv[])
{
    static char command[4];
    command[0] = '-';
    int index = 1;
    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-')
        {
            for (int j = 1; j < strlen(argv[i]); j++)
            {
                if (strchr(command, argv[i][j]) == NULL)
                {
                    command[index++] = argv[i][j];
                }
            }
        }
    }
    return command;
}

void call_Ls_I(char *dir)
{
    DIR *directory = opendir(dir);
    if (directory == NULL)
    {
        printf("Cannot open %s\n", dir);
        exit(1);
    }
    struct stat _file_info;

    struct dirent *dp;
    int _file_counter = 0;

    if (dir != curr_dir)
    {
        printf("%s:\n", dir);
    }

    while ((dp = readdir(directory)) != NULL)
    {
        char *path = malloc(sizeof(dir) + sizeof(dp->d_name) + 1);
        sprintf(path, "%s/%s", dir, dp->d_name);
        if (lstat(path, &_file_info) == -1)
        {
            printf("Cannot access '%s': File or directory doesn't exist!\n", dir);
            exit(1);
        }
        if (dp->d_name[0] != '.')
        {

            printf("%ld  %s  ", (long)_file_info.st_ino, dp->d_name);
            _file_counter++;
        }
        // if (_file_counter % 5 == 0)
        // {
        //     puts("");
        // }
    }
    closedir(directory);
    puts("");
}

void call_Ls_L(char *dir)
{
    DIR *directory = opendir(dir);
    if (directory == NULL)
    {
        printf("Cannot open %s\n", dir);
        exit(1);
    }

    struct dirent *dp;
    struct stat _file_info;

    if (dir != curr_dir)
    {
        printf("%s:\n", dir);
    }

    while ((dp = readdir(directory)) != NULL)
    {
        char *path = malloc(sizeof(dir) + sizeof(dp->d_name) + 1);
        sprintf(path, "%s/%s", dir, dp->d_name);
        if (lstat(path, &_file_info) == -1)
        {
            printf("Cannot access '%s': File or directory doesn't exist!\n", dir);
            exit(1);
        }
        if (dp->d_name[0] != '.')
        {
            char _time[128];
            bool isSoftLink = false;
            struct passwd *pwd = getpwuid(_file_info.st_uid);
            struct group *grp = getgrgid(_file_info.st_gid);
            if (S_ISLNK(_file_info.st_mode))
            {
                isSoftLink = true;
            }
            struct tm *tm = localtime(&_file_info.st_mtime);
            strftime(_time, sizeof(_time), "%b %d %Y %H:%M", tm);
            if (command_value == LI || command_value == ILR)
            {
                printf("%ld ", (long)_file_info.st_ino);
            }
            if (isSoftLink)
            {
                printf("l");
            }
            else
            {
                printf("%c", (S_ISDIR(_file_info.st_mode)) ? 'd' : '-');
            }
            _print_protection_bits(_file_info);
            if (!isSoftLink)
            {
                printf(" %ld %s %s %lld %s %s\n", (long)_file_info.st_nlink, pwd->pw_name, grp->gr_name, (long long)_file_info.st_size, _time, dp->d_name);
            }
            else
            {
                char *link = malloc(_file_info.st_size + 1);
                ssize_t nbytes = readlink(path, link, _file_info.st_size + 1);
                if (nbytes == -1)
                {
                    printf("Error - Readlink failed\n");
                    exit(1);
                }
                link[_file_info.st_size] = '\0';
                printf(" %ld %s %s %lld %s %s -> %s\n", (long)_file_info.st_nlink, pwd->pw_name, grp->gr_name, (long long)_file_info.st_size, _time, dp->d_name, link);
                free(link);
            }
        }
        free(path);
    }
}

void call_Ls_R(char *dir)
{
    if (dir == curr_dir)
    {
        printf("%s:\n", dir);
    }
    if (command_value == IR)
    {
        call_Ls_I(dir);
    }
    else if (command_value == LR)
    {
        call_Ls_L(dir);
    }
    else if (command_value == ILR)
    {
        call_Ls_L(dir);
    }
    else
    {
        call_Ls(dir);
    }

    DIR *directory = opendir(dir);
    struct dirent *dp;

    while ((dp = readdir(directory)) != NULL)
    {
        if (dp->d_name[0] != '.')
        {
            if (dp->d_type == DT_DIR)
            {
                char *path = malloc(sizeof(dir) + sizeof(dp->d_name) + 1);
                sprintf(path, "%s/%s", dir, dp->d_name);
                call_Ls_R(path);
                free(path);
            }
        }
    }
    closedir(directory);
}

void call_Ls(char *dir)
{
    DIR *directory = opendir(dir);
    if (directory == NULL)
    {
        printf("Cannot open %s\n", dir);
        exit(1);
    }
    struct stat _file_info;
    if (lstat(dir, &_file_info) == -1)
    {
        printf("Cannot access '%s': File or directory doesn't exist!\n", dir);
        exit(1);
    }

    struct dirent *dp;
    int _file_counter = 0;

    if (dir != curr_dir)
    {
        printf("%s:\n", dir);
    }

    while ((dp = readdir(directory)) != NULL)
    {
        if (dp->d_name[0] != '.')
        {
            printf("%s   ", dp->d_name);
            _file_counter++;
        }
        // if (_file_counter % 5 == 0)
        // {
        //     puts("");
        // }
    }
    closedir(directory);
    puts("");
}

void _print_protection_bits(struct stat _file_info)
{
    printf("%c", (_file_info.st_mode & S_IRUSR) ? 'r' : '-');
    printf("%c", (_file_info.st_mode & S_IWUSR) ? 'w' : '-');
    printf("%c", (_file_info.st_mode & S_IXUSR) ? 'x' : '-');
    printf("%c", (_file_info.st_mode & S_IRGRP) ? 'r' : '-');
    printf("%c", (_file_info.st_mode & S_IWGRP) ? 'w' : '-');
    printf("%c", (_file_info.st_mode & S_IXGRP) ? 'x' : '-');
    printf("%c", (_file_info.st_mode & S_IROTH) ? 'r' : '-');
    printf("%c", (_file_info.st_mode & S_IWOTH) ? 'w' : '-');
    printf("%c", (_file_info.st_mode & S_IXOTH) ? 'x' : '-');
}