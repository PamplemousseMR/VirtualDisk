#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "virtualdisk.h"

void printHelp()
{
    printf("╔═══════════════════════════════════════════════════════════╗\n");
    printf("║                       VIRTUAL DISK                        ║\n");
    printf("║        Created by Manciaux Romain and Peccard Remi        ║\n");
    printf("║        ls            : display current directory          ║\n");
    printf("║        tree          : display all files and directories  ║\n");
    printf("║        show          : display disk data                  ║\n");
    printf("║        cd     [dir]  : change current directory           ║\n");
    printf("║        mkdir  [name] : create new directory               ║\n");
    printf("║        rmdir  [name] : delete directory and all sub dirs  ║\n");
    printf("║        touch  [name] : create new file                    ║\n");
    printf("║        rm     [name] : delete file                        ║\n");
    printf("║        load   [path] : load virtual disk                  ║\n");
    printf("║        save   [path] : save virtual disk                  ║\n");
    printf("║        vim    [path] : add data to file                   ║\n");
    printf("║        create        : create new virtual disk            ║\n");
    printf("║        kate   [name] : display file content               ║\n");
    printf("║        pwd           : display path to current directory  ║\n");
    printf("║        exit          : end the program                    ║\n");
    printf("║        help          : print help                         ║\n");
    printf("╚═══════════════════════════════════════════════════════════╝\n");
}

void prog()
{
    disk_t* temp;
    char blockSize[128];
    char nbBloc[128];
    char cmd[128] = {'0'};
    char data[4096];
    char* tempChar;
    char* tempChar2;
    char* name = "C";
    char ext = ':';
    disk_t* d = createDisk(512, 32);
    while(cmd[0] != 'e' || cmd[1]!='x' || cmd[2]!='i' || cmd[3]!='t')
    {
        memset(cmd, '\0', 128);

        printf("%s%c",name,ext);
        if(fgets(cmd, 128, stdin) == NULL)
        {
            fprintf(stderr, "Erreur fgets");
            exit(EXIT_FAILURE);
        }
        if(cmd[0] == 'l' && cmd[1] == 's' && cmd[2] == '\n')
            displayDirectoryDisk(d);
        else if(cmd[0] == 'm' && cmd[1] == 'k' && cmd[2] == 'd' && cmd[3] == 'i' && cmd[4] == 'r' && cmd[5] == ' ')
        {
            tempChar = atoChar(&cmd[6]);
            addDirectoryDisk(d, tempChar);
            free(tempChar);
        }
        else if(cmd[0] == 't' && cmd[1] == 'o' && cmd[2] == 'u' && cmd[3] == 'c' && cmd[4] == 'h' && cmd[5] == ' ')
        {
            tempChar = atoChar(&cmd[6]);
            addFileDisk(d, tempChar);
            free(tempChar);
        }
        else if(cmd[0] == 'c' && cmd[1] == 'd'  && cmd[2] == ' ')
        {
            tempChar = atoChar(&cmd[3]);
            moveToDirectory(d, tempChar);
            free(tempChar);
        }
        else if(cmd[0] == 'r' && cmd[1] == 'm'  && cmd[2] == ' ')
        {
            tempChar = atoChar(&cmd[3]);
            removeFile(d, tempChar);
            free(tempChar);
        }
        else if(cmd[0] == 's' && cmd[1] == 'a' && cmd[2] == 'v' && cmd[3] == 'e'  && cmd[4] == ' ')
        {
            tempChar = atoChar(&cmd[5]);
            saveDisk(d, tempChar);
            free(tempChar);
        }
        else if(cmd[0] == 'r' && cmd[1] == 'm' && cmd[2] == 'd' && cmd[3] == 'i'  && cmd[4] == 'r' && cmd[5] == ' ')
        {
            tempChar = atoChar(&cmd[6]);
            rmdirDisk(d, tempChar);
            free(tempChar);
        }
        else if(cmd[0] == 'l' && cmd[1] == 'o' && cmd[2] == 'a' && cmd[3] == 'd' && cmd[4] == ' ')
        {
            tempChar = atoChar(&cmd[5]);
            temp = createFromFileDisk(tempChar);
            free(tempChar);
            if(temp != NULL)
            {
                destroyDisk(d);
                d = temp;
                temp = NULL;
            }
        }
        else if(cmd[0] == 'c' && cmd[1] == 'r' && cmd[2] == 'e' && cmd[3] == 'a' && cmd[4] == 't' && cmd[5] == 'e' && cmd[6] == '\n')
        {
            printf("\tTaille des blocks ? : ");
            if(fgets(blockSize, 128, stdin) == NULL)
            {
                fprintf(stderr, "Erreur fgets");
                exit(EXIT_FAILURE);
            }
            printf("\tNombre de blocks ? : ");
            if(fgets(nbBloc, 128, stdin) == NULL)
            {
                fprintf(stderr, "Erreur fgets");
                exit(EXIT_FAILURE);
            }
            temp = createDisk(atoi(blockSize) , atoi(nbBloc));
            if(temp != NULL)
            {
                destroyDisk(d);
                d = temp;
                temp = NULL;
            }
        }
        else if(cmd[0] == 't' && cmd[1] == 'r' && cmd[2] == 'e' && cmd[3] == 'e' && cmd[4] == '\n')
        {
            displayTreeDisk(d,NULL,NULL,NULL,0);
        }
        else if(cmd[0] == 'h' && cmd[1] == 'e' && cmd[2] == 'l' && cmd[3] == 'p' && cmd[4] == '\n')
        {
            printHelp();
        }
        else if(cmd[0] == 's' && cmd[1] == 'h' && cmd[2] == 'o' && cmd[3] == 'w' && cmd[4] == '\n')
        {
            displayDisk(d);
        }
        else if(cmd[0] == 'v' && cmd[1] == 'i' && cmd[2] == 'm' && cmd[3] == ' ')
        {
            tempChar = atoChar(&cmd[4]);
            if(checkFile(d, tempChar) == 1)
            {
                tempChar2 = atoChar(&cmd[4]);
                printFileContentDisk(d, tempChar2);
                free(tempChar2);
                if(fgets(data, 4096, stdin) == NULL)
                {
                    fprintf(stderr, "Erreur fgets");
                    exit(EXIT_FAILURE);
                }
                tempChar2 = atoChar(&cmd[4]);
                addToFileDisk(d, tempChar2, data);
                free(tempChar2);
            }
            else
            {
                printf("le fichier n'existe pas\n");
            }
            free(tempChar);
        }
        else if(cmd[0] == 'k' && cmd[1] == 'a' && cmd[2] == 't' && cmd[3] == 'e' && cmd[4] == ' ')
        {
            tempChar = atoChar(&cmd[5]);
            printFileContentDisk(d, tempChar);
            free(tempChar);
        }
        else if (cmd[0] == 'p' && cmd[1]=='w' && cmd[2]=='d' && cmd[3] == '\n')
        {
            pwdDisk(d);
        }
        else if( ( cmd[0] != 'e' || cmd[1]!='x' || cmd[2]!='i' || cmd[3]!='t' ) && cmd[0] != '\n')
        {
            printf("%s%c unknown command\n",name,ext);
        }
    }
    destroyDisk(d);
}

int main()
{
    printHelp();
    prog();
    return EXIT_SUCCESS;
}
