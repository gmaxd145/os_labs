// Лабораторная 4 (chmod)
//     1.  Реализовать утилиту mychmod, аналогичную по функциональности системной утилите chmod.
// Примеры использования:
//         a. ./mychmod +x file.txt
//         b. ./mychmod u-r file.txt
//         c. ./mychmod g+rw file.txt
//         d. ./mychmod 766 file.txt
// Всегда перед сдачей лабораторной проверяйте её работоспособность на сервере кафедры!

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>

// enum target
// {
//     user,
//     group,
//     other
// };

// enum action
// {
//     plus,
//     minus,
//     equal
// }

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        fprintf(stderr, "%s\n", "Incorrect command line arguments");
        return 1;
    }    
    mode_t newMode = 0;
    char* options = argv[1];
    char* path = argv[2];
    if (options[0] >= '0' && options[0] <= '7') // проверить, что 2 аргумент это только цифры от 0 до 7 
    {
        newMode = strtol(options, NULL, 8); // handle strtol return value
    }
    else 
    {
        int i = 0;
        bool forUser, forGroup, forOther;
        forUser = forGroup = forOther = false;
        for (; options[i] != '+' && options[i] != '-' && options[i] != '='; i +=1)
        {
            if (options[i] == '\0')
            {
                fprintf(stderr, "%s\n", "Incorrect command line arguments: no action");
                return 1;
            }
            // struct target trgt;
            switch (options[i])
            {
            case 'u':
                forUser = true;
                break;
            case 'g':
                forGroup = true;
                break;
            case 'o':
                forOther = true;
                break;            
            default:
                fprintf(stderr, "%s\n", "Incorrect command line arguments: incorrect target");
                return 1;
            }
        }
        if (forUser == false && forGroup == false && forOther == false)
        {
            forUser = forGroup = forOther = true;
        }
        char operator = options[i];
        i+=1;
        struct stat st;
        if (stat(path, &st) != 0) 
        {
            perror(path);
            return 1;
        }
        newMode = st.st_mode;
        bool read = false;
        bool write = false;
        bool execute = false;
        while(options[i] != '\0')
        {
            switch (options[i])
            {
            case 'r':
                read = true;
                break;
            case 'w':
                write = true;
                break;
            case 'x':
                execute = true;
                break;            
            default:
                fprintf(stderr, "%s\n", "Incorrect command line arguments: incorrect permission");
                return 1;
            }
            i += 1;
        } 
        switch (operator)
            {
            case '+':
                if (forUser)
                {
                    if (read)    newMode |= S_IRUSR;  
                    if (write)   newMode |= S_IWUSR;
                    if (execute) newMode |= S_IXUSR;
                }
                if (forGroup)
                {
                    if (read)    newMode |= S_IRGRP;  
                    if (write)   newMode |= S_IWGRP;
                    if (execute) newMode |= S_IWGRP;
                }
                if (forOther)
                {
                    if (read)    newMode |= S_IROTH;  
                    if (write)   newMode |= S_IWOTH;
                    if (execute) newMode |= S_IXOTH;
                } 
                break;
            case '-':
                if (forUser)
                {
                    if (read)    newMode -= newMode & S_IRUSR;  
                    if (write)   newMode -= newMode & S_IWUSR;
                    if (execute) newMode -= newMode & S_IXUSR;
                }
                if (forGroup)
                {
                    if (read)    newMode -= newMode & S_IRGRP;  
                    if (write)   newMode -= newMode & S_IWGRP;
                    if (execute) newMode -= newMode & S_IXGRP;
                }
                if (forOther)
                {
                    if (read)    newMode -= newMode & S_IROTH;  
                    if (write)   newMode -= newMode & S_IWOTH;
                    if (execute) newMode -= newMode & S_IXOTH;
                }
                break;         
            }
    }
    if (chmod(path, newMode) != 0)
    {
        perror("");
        return 1;
    }
    return 0;
}