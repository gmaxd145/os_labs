// Лабораторная 4 (chmod)
//     1.  Реализовать утилиту mychmod, аналогичную по функциональности системной утилите chmod.
// Примеры использования:
//         a. ./mychmod +x file.txt
//         b. ./mychmod u-r file.txt
//         c. ./mychmod g+rw file.txt
//         d. ./mychmod 766 file.txt
// Всегда перед сдачей лабораторной проверяйте её работоспособность на сервере кафедры!

#include <stdio.h>

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        fprintf(stderr, "%s\n", "Incorrect command line arguments");
    }    

    return 0;
}