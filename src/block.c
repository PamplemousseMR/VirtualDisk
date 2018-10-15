#include "block.h"

bloc_t* createBloc(int size, char* data) {

    bloc_t* res = (bloc_t*)malloc(sizeof(bloc_t));
    res->size = size;
    res->data = (char*)malloc(sizeof(char) * res->size);
    memset(res->data, '\0', res->size);
    res->currentPosition = 0;
    if (data != NULL) {

        if (strlen(data) <= res->size) {

            strcpy(res->data, data);
            res->currentPosition = strlen(data);

        } else fprintf(stderr, "[createBloc] longueur de la chaine supérieur à la taille du bloc");

    }

    return res;

}

bloc_t* createFromFileBloc(int fd) {

    bloc_t* res = (bloc_t*)malloc(sizeof(bloc_t));

    int readRes = read(fd, &(res->size), sizeof(int));
    if(readRes == -1)
    {
        fprintf(stderr, "[createFromFileBloc] Erreur lors de la lecture de la taille");
        exit(EXIT_FAILURE);
    }
    readRes = read(fd, &(res->currentPosition), sizeof(int));
    if(readRes == -1)
    {
        fprintf(stderr, "[createFromFileBloc] Erreur lors de la lecture de la position courante");
        exit(EXIT_FAILURE);
    }

    res->data = (char*)malloc(sizeof(char) * res->size);

    readRes = read(fd, res->data, sizeof(char) * res->size);
    if(readRes == -1)
    {
        fprintf(stderr, "[createFromFileBloc] Erreur lors de la lecture de la position courante");
        exit(EXIT_FAILURE);
    }

    return res;

}

int destroyBloc(bloc_t* b)
{
    if (b != NULL)
    {
        free(b->data);
        free(b);
        return 1;
    }
    else
    {
        fprintf(stderr, "[destroyBloc] paramètre NULL");
    }
    return 0;
}

int addDataBloc(bloc_t* b, char* data) 
{
    if(b != NULL)
    {
        if (data != NULL)
        {
            if (strlen(data) <= b->size - b->currentPosition)
            {
                strcpy(b->data + b->currentPosition, data);
                b->currentPosition = b->currentPosition + strlen(data);
                return 1;
            }
            else fprintf(stderr, "[addDataBloc] longueur de la chaine supérieur à la taille du bloc");
        }
    }else printf("[addDataBloc] paramètre NULL");
    return 0;
}

int addCharDataBloc(bloc_t* b,char data)
{
    if(b != NULL)
    {
        if (1 <= b->size - b->currentPosition)
        {
            if(data == 0x0)
                data = '0';
            strcpy(b->data + b->currentPosition, &data);
            b->currentPosition++;
            return 1;
        }
        else fprintf(stderr, "[addCharDataBloc] longueur de la chaine supérieur à la taille du bloc");
    }else printf("[addCharDataBloc] paramètre NULL");
    return 0;
}

char* getDataBloc(bloc_t* b,int i)
{
    if (b != NULL) {
        char c;
        char* res;
        int j;
        int taille = 0;
        if(i>b->currentPosition)
            return NULL;
        while((c=b->data[i + taille]) != _END_OF_STRING_)
            taille++;
        res = (char*)malloc(sizeof(char) * taille + sizeof(char));
        for(j=0 ; j<taille ; j++)
            res[j] = '0';
        taille = 0;
        while((c = b->data[i + taille]) != _END_OF_STRING_)
        {
            res[taille] = c;
            taille++;
        }
        res[taille] = '\0';
        return res;

    } else fprintf(stderr, "[getDataBloc] paramètre NULL");

    return 0;
}

char getCharDataBloc(bloc_t* b,int i)
{
    if (b != NULL) {
        if(i>b->currentPosition)
            return '0';
        return b->data[i];

    } else fprintf(stderr, "[getCharDataBloc] paramètre NULL");

    return 0;
}

int getIntDataBloc(bloc_t* b,int i)
{
    int value;
    if (b != NULL)
    {
        char c;
        char* res;
        int taille = 0;
        if(i>b->currentPosition)
            return 0;
        res = (char*)malloc(sizeof(char) * sizeof(int) + sizeof(char));
        while(taille != 4)
        {
            c = b->data[i + taille];
            res[taille] = c;
            taille++;
        }
        res[sizeof(int)] = '\0';
        value = atoi(res);
        free(res);
        return value;

    } else fprintf(stderr, "[getIntDataBloc] paramètre NULL");

    return 0;
}

int addIntDataBloc(bloc_t* b,int num)
{
    if(b != NULL)
    {
        char* to;
        int i;
        int j=0;
        char* n = itoChar(num);
        if(strlen(n) > sizeof(int))
        {
            fprintf(stderr, "[addIntDataBloc] valeur trop grande");
            return 0;
        }
        if(strlen(n) != sizeof(int))
        {
            to = (char*)malloc(sizeof(char) * sizeof(int) + sizeof(char));
            for(i=0 ; i<sizeof(int) ; i++)
            {
                if(i>=sizeof(int) - strlen(n))
                    to[i] = n[j++];
                else
                    to[i] = '0';
            }
            to[sizeof(int)] = '\0';
            if(addDataBloc(b,to) == 0)
            {
                fprintf(stderr, "[addIntDataBloc] pas assez de place");
                return 0;
            }
            free(to);
        }
        else if(addDataBloc(b,n) == 0)
        {
            fprintf(stderr, "[addIntDataBloc] pas assez de place");
            return 0;
        }
        free(n);
        return 1;
    }else fprintf(stderr, "[addIntDataBloc] paramètre NULL");
    return 0;
}

void displayBloc(bloc_t* b) 
{
    if(b != NULL)
        printf("[bloc]\n\tsize : %d\n\tcurrent position : %d\n\tdata : %s\n",b->size,b->currentPosition,b->data);
    else fprintf(stderr, "[displayBloc] paramètre NULL");
}

int saveBloc(bloc_t* b, int fd) {

    if (b != NULL) {

        if(write(fd, &(b->size), sizeof(int)) == -1)
        {
            fprintf(stderr, "[saveBloc] Erreur write");
            exit(EXIT_FAILURE);
        }

        if(write(fd, &(b->currentPosition), sizeof(int)) == -1)
        {
            fprintf(stderr, "[saveBloc] Erreur write");
            exit(EXIT_FAILURE);
        }

        if(write(fd, b->data, sizeof(char) * b->size) == -1)
        {
            fprintf(stderr, "[saveBloc] Erreur write");
            exit(EXIT_FAILURE);
        }
        return 1;

    } else fprintf(stderr, "[saveBloc] paramètre NULL");
    return 0;

}

int isFullBloc(bloc_t* b) {

    if (b != NULL) {

        return b->currentPosition == b->size;

    } else fprintf(stderr, "[isFullBloc] paramètre NULL");

    return 0;

}

void emptyBloc(bloc_t* b)
{
    if (b != NULL)
    {
        int i;
        for(i=0 ; i<b->size ; i++)
            b->data[i] = '\0';
        b->currentPosition = 0;
    } else fprintf(stderr, "[emptyBloc] paramètre NULL");
}

void removeBloc(bloc_t* b,int beg ,int end)
{
    if (b != NULL) {

        if(beg >= 0 && beg < end && beg <b->size && end<b->size)
        {
            char* pcs;
            char* pcd;
            int diff = b->size - end;
            b->currentPosition -= end-beg+1;
            for(pcd = b->data + beg, pcs = b->data + end + 1; pcs < b->data + b->size ; pcd++, pcs++)
                *pcd = *pcs;
            memset(b->data + beg + diff, 0, b->size-beg-diff);
        }
        else fprintf(stderr, "[remove] borne incorecte");
    } else fprintf(stderr, "[remove] paramètre NULL");
}
