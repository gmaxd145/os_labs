#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
//#include <errno.h>
#include <sys/stat.h>
#include <string.h>
//#include <sys/types.h>

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
    int archExistedBeforeOpen = access(archName, F_OK);
    int archFile = open(archName, O_RDWR | O_CREAT, 00666);
    if (archFile == -1)
    {
        perror("Open arch error");
        exit(EXIT_FAILURE);
    }
    int filesCount = 0;
    if (archExistedBeforeOpen != 0)
    {
        write(archFile, &filesCount, sizeof(filesCount));  
        lseek(archFile, 0, SEEK_SET);
    }
    read(archFile, &filesCount, sizeof(filesCount));
    struct FileData fileData;
    memset(fileData.fileName, '\0', FILE_NAME_MAX_SIZE);
    for (int i = 0; i < filesCount; ++i)
    {
        read(archFile, &fileData, sizeof(struct FileData));
        if (memcmp(fileData.fileName, fileName, strlen(fileName)) == 0)
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
    memset(fileData.fileName, '\0', FILE_NAME_MAX_SIZE);
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
    char fileContentBuf[fileData.fileSize];
    off_t rBytes = read(file, fileContentBuf, fileData.fileSize);
    if (rBytes == -1)
    {
        perror("Read error");
        exit(EXIT_FAILURE);
    }
    write(archFile, fileContentBuf, rBytes);
    lseek(archFile, 0, SEEK_SET);
    ++filesCount;
    write(archFile, &filesCount, sizeof(filesCount));
    close(file);
    close(archFile);
}

void extractFile(char* archName, char* fileName)
{
    int archFile = open(archName, O_RDWR, 00666);
    if (archFile == -1)
    {
        perror("Open arch error");
        exit(EXIT_FAILURE);
    }
    int filesCount = 0;
    read(archFile, &filesCount, sizeof(filesCount));
    struct FileData fileData;
    memset(fileData.fileName, '\0', FILE_NAME_MAX_SIZE);
    off_t delFileSize = 0;
    off_t delFileStartPos = 0;
    int i = 0;
    off_t for_debug;
    for (; i < filesCount; ++i)
    {
        read(archFile, &fileData, sizeof(struct FileData));
        if (memcmp(fileData.fileName, fileName, strlen(fileName)) == 0)
        {
            ++i;
            break;
        }
        lseek(archFile, fileData.fileSize, SEEK_CUR);
        for_debug = lseek(archFile, 0, SEEK_CUR);
    }
    //1
    for_debug = lseek(archFile, 0, SEEK_CUR);
    delFileSize = fileData.fileSize;
    if (delFileStartPos == -1) {
        perror("Error getting file position");
        exit(EXIT_FAILURE);
    }
    delFileStartPos = lseek(archFile, 0, SEEK_CUR);
    //2
    lseek(archFile, fileData.fileSize, SEEK_CUR);
    for_debug = lseek(archFile, 0, SEEK_CUR);
    off_t movePartStart = lseek(archFile, 0, SEEK_CUR);
    off_t movePartSize = 0;
    for (; i < filesCount; ++i)
    {
        read(archFile, &fileData, sizeof(struct FileData));
        lseek(archFile, fileData.fileSize, SEEK_CUR);
        movePartSize += sizeof(struct FileData);
        movePartSize += fileData.fileSize;
    }
    //3
    lseek(archFile, movePartStart, SEEK_SET);
    char movePartCopy[movePartSize];
    if (read(archFile, movePartCopy, movePartSize) == -1)
    {
        perror("Read error");
        exit(EXIT_FAILURE);
    }
    //4
    lseek(archFile, delFileStartPos, SEEK_SET);
    //5
    char delFileCopy[delFileSize];
    if (read(archFile, delFileCopy, delFileSize) == -1)
    {
        perror("Read delFileCopy error");
        exit(EXIT_FAILURE);
    }
    lseek(archFile, delFileStartPos - sizeof(struct FileData), SEEK_SET);
    // вывод ошибки если файл который мы экстрактим уже есть в директории
    int fileWithSameNameAsExctFile = access(fileName, F_OK);
    if (fileWithSameNameAsExctFile == 0)
    {
        fprintf(stderr, "%s\n", "try to extract file which already in directory");
        exit(EXIT_SUCCESS);
    }
    //
    int extractedFile = open(fileName, O_WRONLY | O_CREAT, 00666);
    write(extractedFile, delFileCopy, delFileSize);
    close(extractedFile);
    //6
    write(archFile, movePartCopy, movePartSize);
    //7
    ftruncate(archFile, delFileStartPos - sizeof(struct FileData) + movePartSize);
    lseek(archFile, 0, SEEK_SET);
    --filesCount;
    write(archFile, &filesCount, sizeof(filesCount));
    close(archFile);
}

void filestat(char* archName)
{
    int archFile = open(archName, O_RDONLY, 00666);
    if (archFile == -1)
    {
        perror("Open arch error");
        exit(EXIT_FAILURE);
    }
    int filesCount = 0;
    read(archFile, &filesCount, sizeof(filesCount));
    struct FileData fileData;
//    memset(fileData.fileName, '\0', FILE_NAME_MAX_SIZE);
    for (int i = 0; i < filesCount; ++i)
    {
        read(archFile, &fileData, sizeof(struct FileData));
        printf("%s %d\n", fileData.fileName, fileData.fileSize);
        lseek(archFile, fileData.fileSize, SEEK_CUR);
    }
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
     if (eFlag)
     {
         extractFile(archName, fileName);
     }
     if (sFlag)
     {
         filestat(archName);
     }

    return 0;
}