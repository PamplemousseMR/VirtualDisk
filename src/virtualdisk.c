#include "virtualdisk.h"

static int checkDirectory(disk_t*,char*);
static int findEmptyBloc(disk_t*);
static void removeDirectoriesRecDisk(disk_t*, bloc_t*);

disk_t* createDisk(int size, int nbBlock)
{
    int* pi;
    int i;
    bloc_t** ppb;
    char* name = ".";
    header h = _HEADER_DIRECTORY_;
    if (size >= _MIN_BLOC_SIZE_ && nbBlock >= _MIN_BLOC_NUMBER_)
    {
        disk_t* res;
        if(size % _RECORDING_ != 0)
        {
            printf("[createDisk] la taille des blocks doit etre modulo %d",_RECORDING_);
            return NULL;
        }
        res = (disk_t*)malloc(sizeof(disk_t));
        res->nbBlocs = nbBlock;
        res->blocSize = size;
        res->currentDirectory = 0;
        res->bitmap = (int*)malloc(sizeof(int) * nbBlock);
        for (pi = res->bitmap; pi < res->bitmap + nbBlock; pi++)
            *pi = 0;
        res->bitmap[0] = 1;

        res->blocs = (bloc_t**)malloc(sizeof(bloc_t*) * nbBlock);

        for (ppb = res->blocs; ppb < res->blocs + nbBlock; ppb++)
        {
            (*ppb) = createBloc(size, "");
        }
        addCharDataBloc(res->blocs[res->currentDirectory],h);
        for (i = 0; i < _NAME_SIZE_; i++)
        {
            if (i < strlen(name))
            {
                addCharDataBloc(res->blocs[res->currentDirectory], name[i]);
            }
            else if (i == strlen(name))
            {
                addCharDataBloc(res->blocs[res->currentDirectory],_END_OF_STRING_);
            }
            else
            {
                addCharDataBloc(res->blocs[res->currentDirectory],'0');
            }
        }
        addIntDataBloc(res->blocs[res->currentDirectory],res->currentDirectory);
        return res;

    }
    else fprintf(stderr, "[createDisk] taille ou nombre de blocs inférieur au minimum requis");

    return NULL;
}

disk_t* createFromFileDisk(char* path)
{
    if (path != NULL)
    {
        int fd;
        int readRes;
        int* pi;
        bloc_t** ppb;
        disk_t* res = (disk_t*)malloc(sizeof(disk_t));
        if((fd = open(path, O_RDWR, S_IRUSR|S_IWUSR)) == -1)
        {
            fprintf(stderr, "[createFromFileDisk] Erreur lors de l'ouverture du fichier");
            return 0;
        }

        readRes = read(fd, &(res->nbBlocs), sizeof(int));
        if(readRes == -1)
        {
            fprintf(stderr, "[createFromFileDisk] Erreur lors de la lecture du nombre de blocs");
            exit(EXIT_FAILURE);
        }

        readRes = read(fd, &(res->blocSize), sizeof(int));
        if(readRes == -1)
        {
            fprintf(stderr, "[createFromFileDisk] Erreur lors de la lecture de la taille des blocs");
            exit(EXIT_FAILURE);
        }

        readRes = read(fd, &(res->currentDirectory), sizeof(int));
        if(readRes == -1)
        {
            fprintf(stderr, "[createFromFileDisk] Erreur lors de la lecture du pointeur courant");
            exit(EXIT_FAILURE);
        }

        res->bitmap = (int*)malloc(sizeof(int) * res->nbBlocs);

        for (pi = res->bitmap; pi < res->bitmap + res->nbBlocs; pi++)
        {
            readRes = read(fd, pi, sizeof(int));
            if(readRes == -1)
            {
                fprintf(stderr, "[createFromFileDisk] Erreur lors de la lecture de la bitmap");
                exit(EXIT_FAILURE);
            }
        }

        res->blocs = (bloc_t**)malloc(sizeof(bloc_t*) * res->nbBlocs);

        for (ppb = res->blocs; ppb < res->blocs + res->nbBlocs; ppb++)
        {
            (*ppb) = createFromFileBloc(fd);
        }

        return res;
    }
    else
    {
        fprintf(stderr, "[createFromFileDisk] paramètre NULL");
    }

    return NULL;
}

int destroyDisk(disk_t* d)
{
    bloc_t** ppb;
    if (d != NULL)
    {
        for (ppb = d->blocs; ppb < d->blocs + d->nbBlocs; ppb++)
        {
            destroyBloc(*ppb);
        }
        free(d->blocs);
        free(d->bitmap);
        free(d);
        return 1;

    }
    else
    {
        fprintf(stderr, "[detroyDisk] paramètre NULL");
    }
    return 0;
}

int displayDisk(disk_t* d)
{
    int* pi;
    bloc_t** ppb;
    if (d != NULL)
    {
        printf("[disk_t]\n\tnbBlocks : %d,\n\tblocSize : %d,\n\tcurrentDirectory : %d,\n\tbitmap : [", d->nbBlocs, d->blocSize, d->currentDirectory);

        for (pi = d->bitmap; pi < d->bitmap + d->nbBlocs; pi++)
        {
            printf("%d,", *pi);
        }

        printf("],\n\tblocs : \n");

        for (ppb = d->blocs; ppb < d->blocs + d->nbBlocs; ppb++)
        {
            displayBloc(*ppb);
        }
        return 1;
    }
    else
    {
        fprintf(stderr, "[displayDisk] paramètre NULL");
    }
    return 0;
}

void pwdDisk(disk_t* d) {

    if (d != NULL)
    {
        /* Le répertoire .. est toujours celui du haut */
        char* nom;
        bloc_t* courant;
        bloc_t* parent;
        int iParent;
        int iCourant;
        int i; /* indice pour les parcours */
        nom = "";
        courant = d->blocs[d->currentDirectory];
        
        while (strcmp(getDataBloc(courant, _HEADER_SIZE_), ".") != 0)
        {
            iParent = getIntDataBloc(courant, _HEADER_SIZE_ + _NAME_SIZE_);
            iCourant = getIntDataBloc(courant, _RECORDING_ + _HEADER_SIZE_ + _NAME_SIZE_);
            parent = d->blocs[iParent];
            
            /* recherche du dossier qui correspond à l'ancient iCourant pour récupérer le nom */
            
            i = 0;
            while (getIntDataBloc(parent, i * _RECORDING_ + _HEADER_SIZE_ + _NAME_SIZE_) != iCourant && i < parent->size / _RECORDING_)
            {
                i++;
            }

            if (i != parent->size / _RECORDING_)
            {
                nom = strcat(strcat(getDataBloc(parent, i * _RECORDING_ + _HEADER_SIZE_), "/"), nom);
                courant = parent;
            }
            else
            {
                fprintf(stderr, "[pwdDisk] ancien répertoire courant invisible dans son répertoire parent");
            }
        }
        
        printf("%s\n",nom);
        
    } else fprintf(stderr, "[pwdDisk] paramètre NULL");
    
}

void printFileContentDisk(disk_t* d, char* name) {
    
    if (d != NULL && name != NULL)
    {
        int i = 0;
        int ok = 1;
        int dataBlocAdd;
        char* temp;
        while(ok)
        {
            temp = getDataBloc(d->blocs[d->currentDirectory] , _HEADER_SIZE_ + i*_RECORDING_);
            if(temp == NULL)
            {
                break;
            }
            if(strcmp(temp,name)==0 && getCharDataBloc(d->blocs[d->currentDirectory],i*_RECORDING_) == _HEADER_FILE_)
            {
                break;
            }
            i++;
            free(temp);
        }
        if(temp != NULL && strcmp(temp,name) == 0 && getCharDataBloc(d->blocs[d->currentDirectory],i*_RECORDING_) == _HEADER_FILE_)
        {
            dataBlocAdd = getIntDataBloc(d->blocs[d->currentDirectory], i * _RECORDING_ + _HEADER_SIZE_ + _NAME_SIZE_);
            printf("%s", d->blocs[dataBlocAdd]->data);
        }
        else
        {
            fprintf(stderr, "[printFileContentDisk] Le fichier n'existe pas");
        }
        free(temp);
    }
    else
    {
        fprintf(stderr, "[printFileContentDisk] paramètre NULL");
    }
}

int saveDisk(disk_t* d,char* path)
{
    if (d != NULL && path != NULL)
    {
        int fd;
        int* pi;
        bloc_t** ppb;
        if((fd = open(path, O_RDWR|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR)) == -1)
        {
            fprintf(stderr, "[saveDisk] Erreur lors de l'ouverture du fichier");
            return 0;
        }

        if(write(fd, &(d->nbBlocs), sizeof(int)) == -1)
        {
            fprintf(stderr, "[saveDisk] Erreur lors de l'écriture du nombre de blocs");
            exit(EXIT_FAILURE);
        }

        if(write(fd, &(d->blocSize), sizeof(int)) == -1)
        {
            fprintf(stderr, "[saveDisk] Erreur lors de l'écriture de la taille d'un bloc");
            exit(EXIT_FAILURE);
        }

        if(write(fd, &(d->currentDirectory), sizeof(int)) == -1)
        {
            fprintf(stderr, "[saveDisk] Erreur lors de l'écriture du pointeur courant");
            exit(EXIT_FAILURE);
        }

        for (pi = d->bitmap; pi < d->bitmap + d->nbBlocs; pi++) {

            if(write(fd, pi, sizeof(int)) == -1)
            {
                fprintf(stderr, "[saveDisk] Erreur lors de l'écriture de la bitmap");
                exit(EXIT_FAILURE);
            }

        }
        for (ppb = d->blocs; ppb < d->blocs + d->nbBlocs; ppb++) {

            saveBloc(*ppb, fd);

        }

        return 1;

    } else fprintf(stderr, "[saveDisk] paramètre NULL");
    return 0;
}

int addDirectoryDisk(disk_t* d,char* name)
{

    if (d != NULL && name != NULL && strlen(name) != 0)
    {
        if(strlen(name)<=sizeof(filename)-sizeof(char))
        {
            int i;
            header h = _HEADER_DIRECTORY_;
            address a = findEmptyBloc(d);
            if(checkDirectory(d,name))
            {
                fprintf(stderr, "[addDirectoryDisk] le repertoire existe deja");
                return 0;
            }
            if(isFullBloc(d->blocs[d->currentDirectory]))
            {
                fprintf(stderr, "[addDirectoryDisk] le repertoire est plein");
                return 0;
            }
            if(a == -1)
            {
                fprintf(stderr, "[addDirectoryDisk] tout les blocks sont utilise");
                return 0;
            }
            addCharDataBloc(d->blocs[d->currentDirectory],h);
            for (i = 0; i < _NAME_SIZE_; i++) {

                if (i < strlen(name)) {

                    addCharDataBloc(d->blocs[d->currentDirectory], name[i]);

                } else if (i == strlen(name)) {

                    addCharDataBloc(d->blocs[d->currentDirectory],_END_OF_STRING_);

                } else {

                    addCharDataBloc(d->blocs[d->currentDirectory],'0');

                }

            }
            addIntDataBloc(d->blocs[d->currentDirectory],a);
            addCharDataBloc(d->blocs[a],h);
            name = "..";
            for (i = 0; i < _NAME_SIZE_; i++) {

                if (i < strlen(name)) {

                    addCharDataBloc(d->blocs[a], name[i]);

                } else if (i == strlen(name)) {

                    addCharDataBloc(d->blocs[a],_END_OF_STRING_);

                } else {

                    addCharDataBloc(d->blocs[a],'0');

                }

            }
            addIntDataBloc(d->blocs[a],d->currentDirectory);
            addCharDataBloc(d->blocs[a],h);
            name = ".";
            for (i = 0; i < _NAME_SIZE_; i++) {

                if (i < strlen(name)) {

                    addCharDataBloc(d->blocs[a], name[i]);

                } else if (i == strlen(name)) {

                    addCharDataBloc(d->blocs[a],_END_OF_STRING_);

                } else {

                    addCharDataBloc(d->blocs[a],'0');

                }

            }
            addIntDataBloc(d->blocs[a],a);
            d->bitmap[a] = 1;
            return 1;
        }
        else fprintf(stderr, "[addDirectoryDisk] nom trop long");
    } else fprintf(stderr, "[addDirectoryDisk] paramètre NULL");

    return 0;

}

int addFileDisk(disk_t* d,char* name)
{
    if (d != NULL && name != NULL && strlen(name) != 0)
    {
        if(strlen(name)<=sizeof(filename)-sizeof(char))
        {
            int i;
            header h = _HEADER_FILE_;
            address a = findEmptyBloc(d);
            if(checkFile(d,name))
            {
                fprintf(stderr, "[addFileDisk] le fichier existe deja");
                return 0;
            }
            if(isFullBloc(d->blocs[d->currentDirectory]))
            {
                fprintf(stderr, "[addFileDisk] le repertoire est plein");
                return 0;
            }
            if(a == -1)
            {
                fprintf(stderr, "[addFileDisk] tout les blocks sont utilise");
                return 0;
            }
            addCharDataBloc(d->blocs[d->currentDirectory],h);
            for (i = 0; i < _NAME_SIZE_; i++) {

                if (i < strlen(name)) {

                    addCharDataBloc(d->blocs[d->currentDirectory], name[i]);

                } else if (i == strlen(name)) {

                    addCharDataBloc(d->blocs[d->currentDirectory],_END_OF_STRING_);

                } else {

                    addCharDataBloc(d->blocs[d->currentDirectory],'0');

                }

            }
            addIntDataBloc(d->blocs[d->currentDirectory],a);
            d->bitmap[a] = 1;
            return 1;
        }
        else fprintf(stderr, "[addFileDisk] nom trop long");
    } else fprintf(stderr, "[addFileDisk] paramètre NULL");

    return 0;
}

int moveToDirectory(disk_t* d,char* rep)
{
    if (d != NULL || rep == NULL)
    {
        int i = 0;
        int ok = 1;
        char* temp;
        while(ok)
        {
            temp = getDataBloc(d->blocs[d->currentDirectory] , _HEADER_SIZE_ + i*_RECORDING_);
            if(temp == NULL)
                break;
            if(strcmp(temp,rep)==0 && getCharDataBloc(d->blocs[d->currentDirectory],i*_RECORDING_) == _HEADER_DIRECTORY_)
                break;
            i++;
            free(temp);
        }
        if(temp != NULL && strcmp(temp,rep) == 0 && getCharDataBloc(d->blocs[d->currentDirectory],i*_RECORDING_) == _HEADER_DIRECTORY_)
        {
            d->currentDirectory = getIntDataBloc(d->blocs[d->currentDirectory],i*_RECORDING_ + (_HEADER_SIZE_+_NAME_SIZE_));
        }
        else
        {
            fprintf(stderr, "[moveToDirectory] le repertoire n'existe pas");
        }
        free(temp);
    } else fprintf(stderr, "[moveToDirectory] paramètre NULL");
    return 0;
}

int checkDirectory(disk_t* d,char* rep)
{
    if (d != NULL || rep == NULL)
    {
        int i = 0;
        int ok = 1;
        char* temp;
        while(ok)
        {
            temp = getDataBloc(d->blocs[d->currentDirectory] , _HEADER_SIZE_ + i*_RECORDING_);
            if(temp == NULL)
                break;
            if(strcmp(temp,rep)==0 && getCharDataBloc(d->blocs[d->currentDirectory],i*_RECORDING_) == _HEADER_DIRECTORY_)
                break;
            i++;
            free(temp);
        }
        if(temp != NULL && strcmp(temp,rep) == 0 && getCharDataBloc(d->blocs[d->currentDirectory],i*_RECORDING_) == _HEADER_DIRECTORY_)
            return 1;
    } else fprintf(stderr, "[checkDirectory] paramètre NULL");
    return 0;
}

int checkFile(disk_t* d,char* rep)
{
    if (d != NULL || rep == NULL)
    {
        int i = 0;
        int ok = 1;
        char* temp;
        while(ok)
        {
            temp = getDataBloc(d->blocs[d->currentDirectory] , _HEADER_SIZE_ + i*_RECORDING_);
            if(temp == NULL)
                break;
            if(strcmp(temp,rep)==0 && getCharDataBloc(d->blocs[d->currentDirectory],i*_RECORDING_) == _HEADER_FILE_)
            {
                break;
            }
            i++;
            free(temp);
        }
        if(temp != NULL && strcmp(temp,rep) == 0 && getCharDataBloc(d->blocs[d->currentDirectory],i*_RECORDING_) == _HEADER_FILE_)
        {
            free(temp);
            return 1;
        }
        free(temp);
    } else fprintf(stderr, "[checkDirectory] paramètre NULL");
    return 0;
}

int findEmptyBloc(disk_t* d) {

    if (d != NULL) {
        int i;
        for (i = 0; i < d->nbBlocs; i++) {

            if (d->bitmap[i] == 0) {

                return i;

            }

        }

        return -1;

    } else fprintf(stderr, "[findEmptyBloc] paramètre NULL");
    return -1;
}

int displayDirectoryDisk(disk_t* d)
{
    if (d != NULL)
    {
        char c;
        char* temp;
        int i=0;
        while(i<d->blocSize/_RECORDING_)
        {
            c = getCharDataBloc(d->blocs[d->currentDirectory] , i*_RECORDING_);
            temp = getDataBloc(d->blocs[d->currentDirectory] , _HEADER_SIZE_ + i*_RECORDING_);
            i++;
            if(temp != NULL)
            {
                printf("\t%c %s\n",c,temp);
                free(temp);
            }
            else
                break;
        }
        return 1;
    } else fprintf(stderr, "[displayDirectoryDisk] paramètre NULL");
    return -1;
}

int removeFile(disk_t* d, char* name)
{
    if (d != NULL)
    {
        int i = 0;
        int ok = 1;
        char* temp;
        int dir;
        while(ok)
        {
            temp = getDataBloc(d->blocs[d->currentDirectory] , _HEADER_SIZE_ + i*_RECORDING_);
            if(temp == NULL)
                break;
            if(strcmp(temp,name)==0 && getCharDataBloc(d->blocs[d->currentDirectory],i*_RECORDING_) == _HEADER_FILE_)
            {
                break;
            }
            i++;
            free(temp);
        }
        if(temp != NULL && strcmp(temp,name) == 0 && getCharDataBloc(d->blocs[d->currentDirectory],i*_RECORDING_) == _HEADER_FILE_)
        {
            dir = getIntDataBloc(d->blocs[d->currentDirectory],i*_RECORDING_ + _HEADER_SIZE_ + _NAME_SIZE_);
            emptyBloc(d->blocs[dir]);
            removeBloc(d->blocs[d->currentDirectory],i*_RECORDING_ ,i*_RECORDING_ + _HEADER_SIZE_ + _NAME_SIZE_ + _ADDRESS_SIZE_-1);
            d->bitmap[dir] = 0;
        }
        else
        {
            fprintf(stderr, "[removeFile] le fichier n'existe pas");
        }
        free(temp);
    } else fprintf(stderr, "[removeFile] paramètre NULL");
    return 0;
}

void rmdirDisk(disk_t* d, char* name) {

    if (d != NULL && name != NULL) {

        int i = 0;
        int j;
        int ok = 1;
        char* temp;
        bloc_t* b = d->blocs[d->currentDirectory];

        while (ok) {

            temp = getDataBloc(b, _HEADER_SIZE_ + i * _RECORDING_);

            if(temp == NULL)
                break;

            if(strcmp(temp, name) == 0 && getCharDataBloc(b, i * _RECORDING_) == _HEADER_DIRECTORY_)
                break;

            i++;
            free(temp);

        }

        if(temp != NULL && strcmp(temp, name) == 0 && getCharDataBloc(b, i * _RECORDING_) == _HEADER_DIRECTORY_) {

            int idBlock = getIntDataBloc(b, i * _RECORDING_ + _HEADER_SIZE_ + _NAME_SIZE_);
            bloc_t* b = d->blocs[idBlock];
            j = d->currentDirectory;
            d->currentDirectory = idBlock;
            removeDirectoriesRecDisk(d, b);
            d->currentDirectory = j;
            removeBloc(d->blocs[d->currentDirectory], i * _RECORDING_,  i*_RECORDING_ + _HEADER_SIZE_ + _NAME_SIZE_ + _ADDRESS_SIZE_-1);
        }
        else fprintf(stderr, "[rmdirDisk] le fichier n'existe pas");

    } else fprintf(stderr, "[rmdirDisk] paramètre NULL");

} 

void removeDirectoriesRecDisk(disk_t* d, bloc_t* b) {

    if (d != NULL && b != NULL) {

        int newId;
        int i;
        int j;
        int dir;
        char* name;
        for (i = 0; i < b->size && i < b->currentPosition; i += _RECORDING_)
        {
            name = getDataBloc(b,i+_HEADER_SIZE_);
            if (getCharDataBloc(b, i) == _HEADER_DIRECTORY_ && strcmp(name,"..") != 0 && strcmp(name,".") != 0)
            {
                newId = getIntDataBloc(b, i + _HEADER_SIZE_ + _NAME_SIZE_);
                j = d->currentDirectory;
                d->currentDirectory = newId;
                removeDirectoriesRecDisk(d , d->blocs[newId]);
                d->currentDirectory = j;
            }
            else if (getCharDataBloc(b, i) == _HEADER_FILE_)
            {
                dir = getIntDataBloc(b,i + _HEADER_SIZE_ + _NAME_SIZE_);
                emptyBloc(d->blocs[dir]);
                removeBloc(d->blocs[d->currentDirectory],i ,i + _HEADER_SIZE_ + _NAME_SIZE_ + _ADDRESS_SIZE_-1);
                i -= _RECORDING_;
                d->bitmap[dir] = 0;
            }
        }
        emptyBloc(d->blocs[d->currentDirectory]);
        d->bitmap[d->currentDirectory] = 0;
    } else fprintf(stderr, "[removeDirectoriesRec] paramètre NULL");

}
void displayTreeDisk(disk_t* d,bloc_t* b,char* tab,int* map,int si)
{
    if (d != NULL)
    {
        int newId;
        int i;
        int j;
        int k;
        char* name;
        if(map == NULL)
        {
            map = (int*)malloc(sizeof(int) * d->nbBlocs);
            for(i=0 ; i<d->nbBlocs ; i++)
                map[i] = 1;
        }
        if(b == NULL)
            b = d->blocs[d->currentDirectory];
        if(tab == NULL)
            tab = "│  ";
        for (i = 0; i < b->size && i < b->currentPosition; i += _RECORDING_)
        {
            name = getDataBloc(b,i+_HEADER_SIZE_);
            if (getCharDataBloc(b, i) == _HEADER_DIRECTORY_ && strcmp(name,"..") != 0 && strcmp(name,".") != 0)
            {
                newId = getIntDataBloc(b, i + _HEADER_SIZE_ + _NAME_SIZE_);
                j = d->currentDirectory;
                d->currentDirectory = newId;
                for(k=0 ; k<=si-1; k++)
                {
                    if(map[k] == 1)
                        printf("│  ");
                    else
                        printf("   ");
                }
                if(i >= b->size - _RECORDING_ || i >= b->currentPosition - _RECORDING_)
                {
                    printf("└──");
                    tab = "   ";
                    map[si] = 0;
                }
                else
                    printf("├──");
                printf("d %s\n",name);
                displayTreeDisk(d , d->blocs[newId], tab,map,si+1);
                d->currentDirectory = j;
            }
            else if (getCharDataBloc(b, i) == _HEADER_FILE_)
            {
                for(k=0 ; k<=si-1; k++)
                {
                    if(map[k] == 1)
                        printf("│  ");
                    else
                        printf("   ");
                }
                if(i >= b->size - _RECORDING_ || i >= b->currentPosition - _RECORDING_)
                {
                    printf("└──");
                    tab = "   ";
                    map[si] = 0;
                }
                else
                    printf("├──");
                printf("f %s\n",name);
            }
            else
            {
                for(k=0 ; k<=si-1; k++)
                {
                    if(map[k] == 1)
                        printf("│  ");
                    else
                        printf("   ");
                }
                if(i >= b->size - _RECORDING_ || i >= b->currentPosition - _RECORDING_)
                {
                    printf("└──");
                    tab = "   ";
                    map[si] = 0;
                }
                else
                    printf("├──");
                printf("d %s\n",name);
            }
        }
        si--;
        if(si == 0)
            free(map);
    } else fprintf(stderr, "[displayTreeDisk] paramètre NULL");
}

void addToFileDisk(disk_t* d, char* name, char* data)
{
    if (d != NULL)
    {
        int i = 0;
        int ok = 1;
        char* temp;
        int dir;
        while(ok)
        {
            temp = getDataBloc(d->blocs[d->currentDirectory] , _HEADER_SIZE_ + i*_RECORDING_);
            if(temp == NULL)
                break;
            if(strcmp(temp,name)==0 && getCharDataBloc(d->blocs[d->currentDirectory],i*_RECORDING_) == _HEADER_FILE_)
            {
                break;
            }
            i++;
            free(temp);
        }
        if(temp != NULL && strcmp(temp,name) == 0 && getCharDataBloc(d->blocs[d->currentDirectory],i*_RECORDING_) == _HEADER_FILE_)
        {
            dir = getIntDataBloc(d->blocs[d->currentDirectory],i*_RECORDING_ + _HEADER_SIZE_ + _NAME_SIZE_);
            addDataBloc(d->blocs[dir],data);
        }
        else
        {
            fprintf(stderr, "[addToFileDisk] le fichier n'existe pas");
        }
        free(temp);
    } else fprintf(stderr, "[addToFileDisk] paramètre NULL");
}
