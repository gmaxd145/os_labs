// Лабораторная работа 1 (ls)
// Задание:
//     Необходимо реализовать программу, повторяющую функциональность стандартной утилиты ls.
// Требования:
//     1. Вывод, идентичный выводу ls;
//     2. Поддержка опций -l и -a, а также их комбинации (getopt());
//     3. Выделение цветом разных типов файлов:
//         a. Обычный файл – без выделения;
//         b. Директория – выделение синим;
//         c. Исполняемый файл – выделение зелёным;
//         d. Ссылка – выделение бирюзовым.
//     4. Примеры использования:
//         a. ./myls -la ../..
//         b. ./myls -l ../ -a
//         c. ./myls .. -la
//         d. ./myls -l -a
//         e. и др.
// Проверять работоспособность программы необходимо на сервере кафедры. Сдача практического задания преподавателю происходит во время семинарского занятия, либо удалённо.


// Общие требования к сдаче лабораторных работ по дисциплине «Защищенные операционные системы» в 5-м семестре.
//     1. Лабораторные работы сдаются через gitlab/github. Лабораторные работы должны реализовываться в ОДНОМ репозитории в соответствующих подпапках. При сдаче лабораторной преподавателю отправляется ссылка на репозиторий и номер лабораторной/ых, которые вы собираетесь сдавать.
// Примерная структура репозитория LabsOS:
// LabsOS
//     ├── lab1
//     ├── lab2
//     └── lab3
//         ├── main.c
//         └── Makefile
//     2. Для каждой лабораторной работы необходимо предоставить Makefile для её сборки. При невыполнении данного условия лабораторная работа проверяться НЕ будет.
//     3. В репозитории не должно быть ничего лишнего. Только исходные коды лабораторных и файлы для сборки.
//     4. Срок сдачи лабораторных работ «вовремя» - 2 недели с момента публикации задания в группе в телеграме.
//     5. Проверять работоспособность лабораторных работ необходимо, прежде всего, на сервере кафедры.

#include <dirent.h>
#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <unistd.h>
#include <time.h>

#define GREEN "\x1b[32m"
#define BLUE "\x1b[34m"
#define DEFAULT "\033[39m"
#define CYAN "\033[96m"

int main(int argc, char ** argv)
{
    char* path = ".";
    bool opL = false, opA = false;
    int opt = 0;
    while ((opt = getopt(argc, argv, "la")) != -1) {
        switch (opt)
        {
            case 'l':
                opL = true;
                break;
            case 'a':
                opA = true;
                break;
        }
    }
    if (argc > optind)
    {
        path = argv[optind];
    }
    DIR *dir = opendir(path);
    if (!dir) {
        perror(path);
    }
    chdir(path);
    if (errno == ENOENT)
    {
        perror(path);
    }
    struct dirent* dr;
    struct stat st;
    int total = 0;
    while ((dr = readdir(dir))) {
        char* name = dr->d_name;
        if (name[0] == '.' && !opA)
        {
            continue;
        }
        lstat(name, &st);
        mode_t mode = st.st_mode;
//        if (errno == ENOENT)
//        {
//            perror(name);
//        }
//
        if (opL)
        {
            if (S_ISDIR(mode))
            {
                printf("%c", 'd');
            }
            else if (S_ISLNK(mode))
            {
                printf("%c", 'l');
            }
            else
            {
                printf("%c", '-');
            }
            printf("%c", (mode & S_IRUSR) ? 'r' : '-');
            printf("%c", (mode & S_IWUSR) ? 'w' : '-');
            printf("%c", (mode & S_IXUSR) ? 'x' : '-');
            printf("%c", (mode & S_IRGRP) ? 'r' : '-');
            printf("%c", (mode & S_IWGRP) ? 'w' : '-');
            printf("%c", (mode & S_IXGRP) ? 'x' : '-');
            printf("%c", (mode & S_IROTH) ? 'r' : '-');
            printf("%c", (mode & S_IWOTH) ? 'w' : '-');
            printf("%c ", (mode & S_IXOTH) ? 'x' : '-');

            total += st.st_blocks;
            printf("%3ld ", st.st_nlink);
            struct passwd *pwd = getpwuid(st.st_uid);
            if (!pwd)
            {
                printf("%10d ", st.st_uid);
            }
            else
            {
                printf("%10s ", pwd->pw_name);
            }
            struct group* gr = getgrgid(st.st_gid);
            printf("%10s ", gr->gr_name);
            printf("%10ld ", st.st_size);
            char* time = ctime(&st.st_mtime);
            struct tm* mtime = localtime(&st.st_mtime);
            strftime(time, 20, "%b %d %H:%M ", mtime);
            printf("%s ", time);
        }

        if (S_ISDIR(mode))
        {
            printf(BLUE "%s " DEFAULT, name);
        }
        else if (S_ISLNK(mode))
        {
            if (opL)
            {
                char linkName[128];
                int linkSize;
                if ((linkSize = readlink(name, linkName, sizeof(linkName))) == -1)
                {
                    perror(name);
                }
                else
                {
                    linkName[linkSize] = '\0';
                    printf(CYAN "%s " DEFAULT, name);
                    printf("-> ");

                    struct stat tempSt;
                    stat(linkName, &tempSt);
                    mode_t tempMode = tempSt.st_mode;
                    if (S_ISDIR(tempMode))
                    {
                        printf(BLUE "%s " DEFAULT, linkName);
                    }
                    else if (tempMode & S_IXUSR)
                    {
                        printf(GREEN "%s " DEFAULT, linkName);
                    }
                    else
                    {
                        printf(DEFAULT "%s " DEFAULT, linkName);
                    }
                }
            }
        }
        else if (mode & S_IXUSR)
        {
            printf(GREEN "%s " DEFAULT, name);
        }
        else
        {
            printf(DEFAULT "%s " DEFAULT, dr->d_name);
        }
        if (opL)
        {
            printf("\n");
        }
    }
    if (opL)
    {
        printf("total %d\n", total / 2);
    }
    if (!opL)
    {
        printf("\n");
    }
    closedir(dir);

    return 0;
}
