#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/types.h>

#define FILE_NAME_MAX_SIZE 128

struct FileData
{
    char fileName[FILE_NAME_MAX_SIZE];
    mode_t fileMode;
    off_t fileSize;
};


void printHelp()
{
    printf("./archiver arch_name -i \"fileName\" -- не больше файла за одну команду\n");
    printf("./archiver arch_name –e \"fileName\" -- файл извлекается из архива, и в архиве больше не находится\n");
    printf("./archiver arch_name –s\n");
    printf("./archiver –h\n");
}

void insertFile(char* archName, char* fileName)
{
    bool isArchExists = access(archName, F_OK);
    int archFile = open(archName, O_RDWR | O_CREAT, 00777);    
    if (archFile == -1)
    {
        perror("Open arch error");
        exit(EXIT_FAILURE);
    }
    int filesCount = 0;
    if (!isArchExists)
    {
        write(archFile, &filesCount, sizeof(filesCount));  
        lseek(archFile, 0, SEEK_SET);
    }
    read(archFile, &filesCount, sizeof(filesCount));
    printf("!!!!!!!!!!!!   %d\n", filesCount);
    struct FileData fileData;
    for (int i = 0; i < filesCount; ++i)
    {
        read(archFile, &fileData, sizeof(struct FileData));
        if (fileData.fileName == fileName)
        {
            printf("%s\n", "Try to insert file which already in archive");
            exit(EXIT_SUCCESS);
        }
        lseek(archFile, fileData.fileSize, SEEK_CUR);
    }
    struct stat st;
    if (stat(fileName, &st) == -1)
    {
        perror("Stat error");
        exit(EXIT_FAILURE);
    }
    strcpy(fileData.fileName, fileName);
    fileData.fileMode = st.st_mode;
    fileData.fileSize = st.st_size;
    write(archFile, &fileData, sizeof(struct FileData));
    int file = open(fileName, O_RDONLY);
    if (file == -1)
    {
        perror("Open file error");
        exit(EXIT_FAILURE);
    }
    char* fileContentBuf = malloc(fileData.fileSize);
    int rBytes = read(file, fileContentBuf, fileData.fileSize);
    if (rBytes == -1)
    {
        perror("Read error");
        exit(EXIT_FAILURE);
    }
    write(archFile, fileContentBuf, rBytes);
    lseek(archFile, 0, SEEK_SET);
    ++filesCount;
    printf("??????????????   %d\n", filesCount);
    if(write(archFile, &filesCount, sizeof(filesCount)) == -1)
    {   
        perror("AAAAAAAAAAA");
    }
    close(file);
    close(archFile);
    free(fileContentBuf);
}

int main(int argc, char** argv)
{   
    int opt;
    bool iFlag = false;
    bool eFlag = false;
    bool sFlag = false;
    bool hFlag = false;
    while ((opt = getopt(argc, argv, "iesh")) != -1)
    {
        switch (opt)
        {
            case 'i':
            {
                iFlag = true;
                break;
            }
            case 'e':
            {
                eFlag = true;
                break;
            }
            case 's':
            {
                sFlag = true;
                break;
            }
            case 'h':
            {
                hFlag = true;
                break;
            }
            case '?':
            {
                fprintf(stderr, "%s\n", "Incorrect line arguments options");
                exit(EXIT_FAILURE);
            }
        }
    }
    if ((iFlag || eFlag) && argc != 4)
    {
        fprintf(stderr, "%s\n", "Incorrect command line arguments");
        exit(EXIT_FAILURE);
    }
    if (hFlag)
    {
        printHelp();
    }
    char* archName = argv[optind];
    char* fileName = argv[++optind];
    if (iFlag)
    {       
        insertFile(archName, fileName);            
    }
    // if (eFlag)
    // {
    //     extractFile(archName, filePath);
    // }
    // if (sFlag)
    // {
    //     filesst(archName);
    // }

    return 0;
}