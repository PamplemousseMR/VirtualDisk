#include "virtualdisk.h"

static int checkDirectory(disk_t*,char*);
static int findEmptyBloc(disk_t*);
static void removeDirectoriesRecDisk(disk_t*, bloc_t*);

disk_t* createDisk(int _size, int _nbBlock)
{
    int* pi;
    int i;
    char* name = ".";
    header h = _HEADER_DIRECTORY_;
    if (_size >= _MIN_BLOC_SIZE_ && _nbBlock >= _MIN_BLOC_NUMBER_)
    {
        disk_t* res;
        if(_size % _RECORDING_ != 0)
        {
            printf("[createDisk] la taille des blocks doit etre modulo %d\n",_RECORDING_);
            return NULL;
        }
        res = (disk_t*)malloc(sizeof(disk_t));
        res->m_nbBlocs = _nbBlock;
        res->m_blocSize = _size;
        res->m_currentDirectory = 0;
        res->m_bitmap = (int*)malloc(sizeof(int) * _nbBlock);
        for (pi = res->m_bitmap; pi < res->m_bitmap + _nbBlock; pi++)
        {
            *pi = 0;
        }
        res->m_bitmap[0] = 1;

        res->m_blocs = (bloc_t**)malloc(sizeof(bloc_t*) * _nbBlock);

        for(i=0 ; i<_nbBlock ; ++i)
        {
            res->m_blocs[i] = createBloc(_size, "");
        }
        addCharDataBloc(res->m_blocs[res->m_currentDirectory], h);
        for (i = 0; i < _NAME_SIZE_; i++)
        {
            if (i < strlen(name))
            {
                addCharDataBloc(res->m_blocs[res->m_currentDirectory], name[i]);
            }
            else if (i == strlen(name))
            {
                addCharDataBloc(res->m_blocs[res->m_currentDirectory],END_OF_STRING);
            }
            else
            {
                addCharDataBloc(res->m_blocs[res->m_currentDirectory],'0');
            }
        }
        addIntDataBloc(res->m_blocs[res->m_currentDirectory],res->m_currentDirectory);
        return res;

    }
    else
    {
        printf("[createDisk] taille ou nombre de blocs inférieur au minimum requis\n");
    }

    return NULL;
}

disk_t* createFromFileDisk(char* _path)
{
    if (_path != NULL)
    {
        int fd;
        int readRes;
        int* pi;
        bloc_t** ppb;
        disk_t* res = (disk_t*)malloc(sizeof(disk_t));
        if((fd = open(_path, O_RDWR, S_IRUSR|S_IWUSR)) == -1)
        {
            fprintf(stderr, "[createFromFileDisk] Erreur lors de l'ouverture du fichier\n");
            return 0;
        }

        readRes = read(fd, &(res->m_nbBlocs), sizeof(int));
        if(readRes == -1)
        {
            fprintf(stderr, "[createFromFileDisk] Erreur lors de la lecture du nombre de blocs\n");
            exit(EXIT_FAILURE);
        }

        readRes = read(fd, &(res->m_blocSize), sizeof(int));
        if(readRes == -1)
        {
            fprintf(stderr, "[createFromFileDisk] Erreur lors de la lecture de la taille des blocs\n");
            exit(EXIT_FAILURE);
        }

        readRes = read(fd, &(res->m_currentDirectory), sizeof(int));
        if(readRes == -1)
        {
            fprintf(stderr, "[createFromFileDisk] Erreur lors de la lecture du pointeur courant\n");
            exit(EXIT_FAILURE);
        }

        res->m_bitmap = (int*)malloc(sizeof(int) * res->m_nbBlocs);

        for (pi = res->m_bitmap; pi < res->m_bitmap + res->m_nbBlocs; pi++)
        {
            readRes = read(fd, pi, sizeof(int));
            if(readRes == -1)
            {
                fprintf(stderr, "[createFromFileDisk] Erreur lors de la lecture de la bitmap\n");
                exit(EXIT_FAILURE);
            }
        }

        res->m_blocs = (bloc_t**)malloc(sizeof(bloc_t*) * res->m_nbBlocs);

        for (ppb = res->m_blocs; ppb < res->m_blocs + res->m_nbBlocs; ppb++)
        {
            (*ppb) = createFromFileBloc(fd);
        }

        return res;
    }
    else
    {
        fprintf(stderr, "[createFromFileDisk] paramètre NULL\n");
    }

    return NULL;
}

int destroyDisk(disk_t* _disk)
{
    bloc_t** ppb;
    if (_disk != NULL)
    {
        for (ppb = _disk->m_blocs; ppb < _disk->m_blocs + _disk->m_nbBlocs; ppb++)
        {
            destroyBloc(*ppb);
        }
        free(_disk->m_blocs);
        free(_disk->m_bitmap);
        free(_disk);
        return 1;

    }
    else
    {
        fprintf(stderr, "[detroyDisk] paramètre NULL\n");
    }
    return 0;
}

int displayDisk(disk_t* _disk)
{
    int* pi;
    bloc_t** ppb;
    if (_disk != NULL)
    {
        printf("[disk_t]\n\tnbBlocks : %d,\n\tblocSize : %d,\n\tcurrentDirectory : %d,\n\tbitmap : [", _disk->m_nbBlocs, _disk->m_blocSize, _disk->m_currentDirectory);

        for (pi = _disk->m_bitmap; pi < _disk->m_bitmap + _disk->m_nbBlocs; pi++)
        {
            printf("%d,", *pi);
        }

        printf("],\n\tblocs : \n");

        for (ppb = _disk->m_blocs; ppb < _disk->m_blocs + _disk->m_nbBlocs; ppb++)
        {
            displayBloc(*ppb);
        }
        return 1;
    }
    else
    {
        fprintf(stderr, "[displayDisk] paramètre NULL\n");
    }
    return 0;
}

void pwdDisk(disk_t* _disk)
{
    if (_disk != NULL)
    {
        /* Le répertoire .. est toujours celui du haut */
        char* nom;
        char* temp;
        bloc_t* courant;
        bloc_t* parent;
        int iParent;
        int iCourant;
        int i; /* indice pour les parcours */
        nom = "";
        courant = _disk->m_blocs[_disk->m_currentDirectory];
        temp = getDataBloc(courant, _HEADER_SIZE_);
        while(strcmp(temp, ".") != 0)
        {
            iParent = getIntDataBloc(courant, _HEADER_SIZE_ + _NAME_SIZE_);
            iCourant = getIntDataBloc(courant, _RECORDING_ + _HEADER_SIZE_ + _NAME_SIZE_);
            parent = _disk->m_blocs[iParent];
            
            /* recherche du dossier qui correspond à l'ancient iCourant pour récupérer le nom */
            
            i = 0;
            while (getIntDataBloc(parent, i * _RECORDING_ + _HEADER_SIZE_ + _NAME_SIZE_) != iCourant && i < parent->m_size / _RECORDING_)
            {
                i++;
            }

            if (i != parent->m_size / _RECORDING_)
            {
                nom = strcat(strcat(getDataBloc(parent, i * _RECORDING_ + _HEADER_SIZE_), "/"), nom);
                courant = parent;
            }
            else
            {
                printf("[pwdDisk] ancien répertoire courant invisible dans son répertoire parent\n");
            }
            free(temp);
            temp = getDataBloc(courant, _HEADER_SIZE_);
        }
        free(temp);

        printf("%s\n",nom);
        
    }
    else
    {
        fprintf(stderr, "[pwdDisk] paramètre NULL\n");
    }
    
}

void printFileContentDisk(disk_t* _disk, char* _name)
{
    if (_disk != NULL && _name != NULL)
    {
        int i = 0;
        int ok = 1;
        int dataBlocAdd;
        char* temp;
        while(ok)
        {
            temp = getDataBloc(_disk->m_blocs[_disk->m_currentDirectory] , _HEADER_SIZE_ + i*_RECORDING_);
            if(temp == NULL)
            {
                break;
            }
            if(strcmp(temp,_name)==0 && getCharDataBloc(_disk->m_blocs[_disk->m_currentDirectory],i*_RECORDING_) == _HEADER_FILE_)
            {
                break;
            }
            i++;
            free(temp);
        }
        if(temp != NULL && strcmp(temp,_name) == 0 && getCharDataBloc(_disk->m_blocs[_disk->m_currentDirectory],i*_RECORDING_) == _HEADER_FILE_)
        {
            dataBlocAdd = getIntDataBloc(_disk->m_blocs[_disk->m_currentDirectory], i * _RECORDING_ + _HEADER_SIZE_ + _NAME_SIZE_);
            printf("%s", _disk->m_blocs[dataBlocAdd]->m_data);
        }
        else
        {
            printf("[printFileContentDisk] Le fichier n'existe pas\n");
        }
        free(temp);
    }
    else
    {
        fprintf(stderr, "[printFileContentDisk] paramètre NULL\n");
    }
}

int saveDisk(disk_t* _disk, char* _path)
{
    if (_disk != NULL && _path != NULL)
    {
        int fd;
        int* pi;
        bloc_t** ppb;
        if((fd = open(_path, O_RDWR|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR)) == -1)
        {
            fprintf(stderr, "[saveDisk] Erreur lors de l'ouverture du fichier\n");
            return 0;
        }

        if(write(fd, &(_disk->m_nbBlocs), sizeof(int)) == -1)
        {
            fprintf(stderr, "[saveDisk] Erreur lors de l'écriture du nombre de blocs\n");
            exit(EXIT_FAILURE);
        }

        if(write(fd, &(_disk->m_blocSize), sizeof(int)) == -1)
        {
            fprintf(stderr, "[saveDisk] Erreur lors de l'écriture de la taille d'un bloc\n");
            exit(EXIT_FAILURE);
        }

        if(write(fd, &(_disk->m_currentDirectory), sizeof(int)) == -1)
        {
            fprintf(stderr, "[saveDisk] Erreur lors de l'écriture du pointeur courant\n");
            exit(EXIT_FAILURE);
        }

        for (pi = _disk->m_bitmap; pi < _disk->m_bitmap + _disk->m_nbBlocs; pi++)
        {
            if(write(fd, pi, sizeof(int)) == -1)
            {
                fprintf(stderr, "[saveDisk] Erreur lors de l'écriture de la bitmap\n");
                exit(EXIT_FAILURE);
            }
        }
        for (ppb = _disk->m_blocs; ppb < _disk->m_blocs + _disk->m_nbBlocs; ppb++)
        {
            saveBloc(*ppb, fd);
        }
        return 1;
    }
    else
    {
        fprintf(stderr, "[saveDisk] paramètre NULL\n");
    }
    return 0;
}

int addDirectoryDisk(disk_t* _disk, char* _name)
{

    if (_disk != NULL && _name != NULL && strlen(_name) != 0)
    {
        if(strlen(_name)<=sizeof(filename)-sizeof(char))
        {
            int i;
            header h = _HEADER_DIRECTORY_;
            address a = findEmptyBloc(_disk);
            if(checkDirectory(_disk,_name))
            {
                printf("[addDirectoryDisk] le repertoire existe deja\n");
                return 0;
            }
            if(isFullBloc(_disk->m_blocs[_disk->m_currentDirectory]))
            {
                printf("[addDirectoryDisk] le repertoire est plein\n");
                return 0;
            }
            if(a == -1)
            {
                printf("[addDirectoryDisk] tout les blocks sont utilise\n");
                return 0;
            }
            addCharDataBloc(_disk->m_blocs[_disk->m_currentDirectory],h);
            for (i = 0; i < _NAME_SIZE_; i++)
            {
                if (i < strlen(_name))
                {
                    addCharDataBloc(_disk->m_blocs[_disk->m_currentDirectory], _name[i]);
                }
                else if (i == strlen(_name))
                {
                    addCharDataBloc(_disk->m_blocs[_disk->m_currentDirectory],END_OF_STRING);
                }
                else
                {
                    addCharDataBloc(_disk->m_blocs[_disk->m_currentDirectory],'0');
                }
            }
            addIntDataBloc(_disk->m_blocs[_disk->m_currentDirectory],a);
            addCharDataBloc(_disk->m_blocs[a],h);
            _name = "..";
            for (i = 0; i < _NAME_SIZE_; i++)
            {
                if (i < strlen(_name))
                {
                    addCharDataBloc(_disk->m_blocs[a], _name[i]);
                }
                else if (i == strlen(_name))
                {
                    addCharDataBloc(_disk->m_blocs[a],END_OF_STRING);
                }
                else
                {
                    addCharDataBloc(_disk->m_blocs[a],'0');
                }
            }
            addIntDataBloc(_disk->m_blocs[a],_disk->m_currentDirectory);
            addCharDataBloc(_disk->m_blocs[a],h);
            _name = ".";
            for (i = 0; i < _NAME_SIZE_; i++)
            {
                if (i < strlen(_name))
                {
                    addCharDataBloc(_disk->m_blocs[a], _name[i]);
                }
                else if (i == strlen(_name))
                {
                    addCharDataBloc(_disk->m_blocs[a],END_OF_STRING);
                }
                else
                {
                    addCharDataBloc(_disk->m_blocs[a],'0');
                }
            }
            addIntDataBloc(_disk->m_blocs[a],a);
            _disk->m_bitmap[a] = 1;
            return 1;
        }
        else
        {
            printf("[addDirectoryDisk] nom trop long\n");
        }
    }
    else
    {
        fprintf(stderr, "[addDirectoryDisk] paramètre NULL\n");
    }

    return 0;
}

int addFileDisk(disk_t* _disk, char* _name)
{
    if (_disk != NULL && _name != NULL && strlen(_name) != 0)
    {
        if(strlen(_name)<=sizeof(filename)-sizeof(char))
        {
            int i;
            header h = _HEADER_FILE_;
            address a = findEmptyBloc(_disk);
            if(checkFile(_disk,_name))
            {
                printf("[addFileDisk] le fichier existe deja\n");
                return 0;
            }
            if(isFullBloc(_disk->m_blocs[_disk->m_currentDirectory]))
            {
                printf("[addFileDisk] le repertoire est plein\n");
                return 0;
            }
            if(a == -1)
            {
                printf("[addFileDisk] tout les blocks sont utilise\n");
                return 0;
            }
            addCharDataBloc(_disk->m_blocs[_disk->m_currentDirectory],h);
            for (i = 0; i < _NAME_SIZE_; i++)
            {
                if (i < strlen(_name))
                {
                    addCharDataBloc(_disk->m_blocs[_disk->m_currentDirectory], _name[i]);
                }
                else if (i == strlen(_name))
                {
                    addCharDataBloc(_disk->m_blocs[_disk->m_currentDirectory],END_OF_STRING);
                }
                else
                {
                    addCharDataBloc(_disk->m_blocs[_disk->m_currentDirectory],'0');
                }
            }
            addIntDataBloc(_disk->m_blocs[_disk->m_currentDirectory],a);
            _disk->m_bitmap[a] = 1;
            return 1;
        }
        else
        {
            printf("[addFileDisk] nom trop long\n");
        }
    }
    else
    {
        fprintf(stderr, "[addFileDisk] paramètre NULL\n");
    }

    return 0;
}

int moveToDirectory(disk_t* _disk, char* _rep)
{
    if (_disk != NULL || _rep == NULL)
    {
        int i = 0;
        int ok = 1;
        char* temp;
        while(ok)
        {
            temp = getDataBloc(_disk->m_blocs[_disk->m_currentDirectory] , _HEADER_SIZE_ + i*_RECORDING_);
            if(temp == NULL)
            {
                break;
            }
            if(strcmp(temp,_rep)==0 && getCharDataBloc(_disk->m_blocs[_disk->m_currentDirectory],i*_RECORDING_) == _HEADER_DIRECTORY_)
            {
                break;
            }
            i++;
            free(temp);
        }
        if(temp != NULL && strcmp(temp,_rep) == 0 && getCharDataBloc(_disk->m_blocs[_disk->m_currentDirectory],i*_RECORDING_) == _HEADER_DIRECTORY_)
        {
            _disk->m_currentDirectory = getIntDataBloc(_disk->m_blocs[_disk->m_currentDirectory],i*_RECORDING_ + (_HEADER_SIZE_+_NAME_SIZE_));
        }
        else
        {
            printf("[moveToDirectory] le repertoire n'existe pas\n");
        }
        free(temp);
    }
    else
    {
        fprintf(stderr, "[moveToDirectory] paramètre NULL\n");
    }
    return 0;
}

int checkDirectory(disk_t* _disk, char* _rep)
{
    if (_disk != NULL || _rep == NULL)
    {
        int i = 0;
        int ok = 1;
        char* temp;
        while(ok)
        {
            temp = getDataBloc(_disk->m_blocs[_disk->m_currentDirectory] , _HEADER_SIZE_ + i*_RECORDING_);
            if(temp == NULL)
            {
                break;
            }
            if(strcmp(temp,_rep)==0 && getCharDataBloc(_disk->m_blocs[_disk->m_currentDirectory],i*_RECORDING_) == _HEADER_DIRECTORY_)
            {
                break;
            }
            i++;
            free(temp);
        }
        if(temp != NULL && strcmp(temp,_rep) == 0 && getCharDataBloc(_disk->m_blocs[_disk->m_currentDirectory],i*_RECORDING_) == _HEADER_DIRECTORY_)
        {
            free(temp);
            return 1;
        }
    }
    else
    {
        fprintf(stderr, "[checkDirectory] paramètre NULL\n");
    }
    return 0;
}

int checkFile(disk_t* _disk, char* _rep)
{
    if (_disk != NULL || _rep == NULL)
    {
        int i = 0;
        int ok = 1;
        char* temp;
        while(ok)
        {
            temp = getDataBloc(_disk->m_blocs[_disk->m_currentDirectory] , _HEADER_SIZE_ + i*_RECORDING_);
            if(temp == NULL)
            {
                break;
            }
            if(strcmp(temp,_rep)==0 && getCharDataBloc(_disk->m_blocs[_disk->m_currentDirectory],i*_RECORDING_) == _HEADER_FILE_)
            {
                break;
            }
            i++;
            free(temp);
        }
        if(temp != NULL && strcmp(temp,_rep) == 0 && getCharDataBloc(_disk->m_blocs[_disk->m_currentDirectory],i*_RECORDING_) == _HEADER_FILE_)
        {
            free(temp);
            return 1;
        }
        free(temp);
    }
    else
    {
        fprintf(stderr, "[checkDirectory] paramètre NULL\n");
    }
    return 0;
}

int findEmptyBloc(disk_t* _disk)
{
    if (_disk != NULL)
    {
        int i;
        for (i = 0; i < _disk->m_nbBlocs; i++)
        {
            if (_disk->m_bitmap[i] == 0)
            {
                return i;
            }
        }
        return -1;
    }
    else
    {
        fprintf(stderr, "[findEmptyBloc] paramètre NULL\n");
    }
    return -1;
}

int displayDirectoryDisk(disk_t* _disk)
{
    if (_disk != NULL)
    {
        char c;
        char* temp;
        int i=0;
        while(i<_disk->m_blocSize/_RECORDING_)
        {
            c = getCharDataBloc(_disk->m_blocs[_disk->m_currentDirectory] , i*_RECORDING_);
            temp = getDataBloc(_disk->m_blocs[_disk->m_currentDirectory] , _HEADER_SIZE_ + i*_RECORDING_);
            i++;
            if(temp != NULL)
            {
                printf("\t%c %s\n",c,temp);
                free(temp);
            }
            else
            {
                break;
            }
        }
        return 1;
    }
    else
    {
        fprintf(stderr, "[displayDirectoryDisk] paramètre NULL\n");
    }
    return -1;
}

int removeFile(disk_t* _disk, char* _name)
{
    if (_disk != NULL)
    {
        int i = 0;
        int ok = 1;
        char* temp;
        int dir;
        while(ok)
        {
            temp = getDataBloc(_disk->m_blocs[_disk->m_currentDirectory] , _HEADER_SIZE_ + i*_RECORDING_);
            if(temp == NULL)
            {
                break;
            }
            if(strcmp(temp,_name)==0 && getCharDataBloc(_disk->m_blocs[_disk->m_currentDirectory],i*_RECORDING_) == _HEADER_FILE_)
            {
                break;
            }
            i++;
            free(temp);
        }
        if(temp != NULL && strcmp(temp,_name) == 0 && getCharDataBloc(_disk->m_blocs[_disk->m_currentDirectory],i*_RECORDING_) == _HEADER_FILE_)
        {
            dir = getIntDataBloc(_disk->m_blocs[_disk->m_currentDirectory],i*_RECORDING_ + _HEADER_SIZE_ + _NAME_SIZE_);
            emptyBloc(_disk->m_blocs[dir]);
            removeBloc(_disk->m_blocs[_disk->m_currentDirectory],i*_RECORDING_ ,i*_RECORDING_ + _HEADER_SIZE_ + _NAME_SIZE_ + _ADDRESS_SIZE_-1);
            _disk->m_bitmap[dir] = 0;
        }
        else
        {
            printf("[removeFile] le fichier n'existe pas\n");
        }
        free(temp);
    }
    else
    {
        fprintf(stderr, "[removeFile] paramètre NULL\n");
    }
    return 0;
}

void rmdirDisk(disk_t* _disk, char* _name)
{
    if (_disk != NULL && _name != NULL)
    {
        int i = 0;
        int j;
        int ok = 1;
        char* temp;
        bloc_t* b = _disk->m_blocs[_disk->m_currentDirectory];

        while (ok)
        {
            temp = getDataBloc(b, _HEADER_SIZE_ + i * _RECORDING_);

            if(temp == NULL)
            {
                break;
            }
            if(strcmp(temp, _name) == 0 && getCharDataBloc(b, i * _RECORDING_) == _HEADER_DIRECTORY_)
            {
                break;
            }
            i++;
            free(temp);
        }

        if(temp != NULL && strcmp(temp, _name) == 0 && getCharDataBloc(b, i * _RECORDING_) == _HEADER_DIRECTORY_)
        {
            int idBlock = getIntDataBloc(b, i * _RECORDING_ + _HEADER_SIZE_ + _NAME_SIZE_);
            bloc_t* b = _disk->m_blocs[idBlock];
            j = _disk->m_currentDirectory;
            _disk->m_currentDirectory = idBlock;
            removeDirectoriesRecDisk(_disk, b);
            _disk->m_currentDirectory = j;
            removeBloc(_disk->m_blocs[_disk->m_currentDirectory], i * _RECORDING_,  i*_RECORDING_ + _HEADER_SIZE_ + _NAME_SIZE_ + _ADDRESS_SIZE_-1);
        }
        else
        {
            printf("[rmdirDisk] le fichier n'existe pas\n");
        }
    }
    else
    {
        fprintf(stderr, "[rmdirDisk] paramètre NULL\n");
    }
} 

void removeDirectoriesRecDisk(disk_t* _disk, bloc_t* _bloc)
{
    if (_disk != NULL && _bloc != NULL)
    {
        int newId;
        int i;
        int j;
        int dir;
        char* name;
        for (i = 0; i < _bloc->m_size && i < _bloc->m_currentPosition; i += _RECORDING_)
        {
            name = getDataBloc(_bloc,i+_HEADER_SIZE_);
            if (getCharDataBloc(_bloc, i) == _HEADER_DIRECTORY_ && strcmp(name,"..") != 0 && strcmp(name,".") != 0)
            {
                newId = getIntDataBloc(_bloc, i + _HEADER_SIZE_ + _NAME_SIZE_);
                j = _disk->m_currentDirectory;
                _disk->m_currentDirectory = newId;
                removeDirectoriesRecDisk(_disk , _disk->m_blocs[newId]);
                _disk->m_currentDirectory = j;
            }
            else if (getCharDataBloc(_bloc, i) == _HEADER_FILE_)
            {
                dir = getIntDataBloc(_bloc,i + _HEADER_SIZE_ + _NAME_SIZE_);
                emptyBloc(_disk->m_blocs[dir]);
                removeBloc(_disk->m_blocs[_disk->m_currentDirectory],i ,i + _HEADER_SIZE_ + _NAME_SIZE_ + _ADDRESS_SIZE_-1);
                i -= _RECORDING_;
                _disk->m_bitmap[dir] = 0;
            }
        }
        emptyBloc(_disk->m_blocs[_disk->m_currentDirectory]);
        _disk->m_bitmap[_disk->m_currentDirectory] = 0;
    }
    else
    {
        fprintf(stderr, "[removeDirectoriesRec] paramètre NULL\n");
    }
}
void displayTreeDisk(disk_t* _disk, bloc_t* _bloc, char* _tab, int* _map, int _si)
{
    if (_disk != NULL)
    {
        int newId;
        int i;
        int j;
        int k;
        char* name;
        if(_map == NULL)
        {
            _map = (int*)malloc(sizeof(int) * _disk->m_nbBlocs);
            for(i=0 ; i<_disk->m_nbBlocs ; i++)
                _map[i] = 1;
        }
        if(_bloc == NULL)
        {
            _bloc = _disk->m_blocs[_disk->m_currentDirectory];
        }
        if(_tab == NULL)
        {
            _tab = "│  ";
        }
        for (i = 0; i < _bloc->m_size && i < _bloc->m_currentPosition; i += _RECORDING_)
        {
            name = getDataBloc(_bloc,i+_HEADER_SIZE_);
            if (getCharDataBloc(_bloc, i) == _HEADER_DIRECTORY_ && strcmp(name,"..") != 0 && strcmp(name,".") != 0)
            {
                newId = getIntDataBloc(_bloc, i + _HEADER_SIZE_ + _NAME_SIZE_);
                j = _disk->m_currentDirectory;
                _disk->m_currentDirectory = newId;
                for(k=0 ; k<=_si-1; k++)
                {
                    if(_map[k] == 1)
                    {
                        printf("│  ");
                    }
                    else
                    {
                        printf("   ");
                    }
                }
                if(i >= _bloc->m_size - _RECORDING_ || i >= _bloc->m_currentPosition - _RECORDING_)
                {
                    printf("└──");
                    _tab = "   ";
                    _map[_si] = 0;
                }
                else
                {
                    printf("├──");
                }
                printf("d %s\n",name);
                displayTreeDisk(_disk , _disk->m_blocs[newId], _tab,_map,_si+1);
                _disk->m_currentDirectory = j;
            }
            else if (getCharDataBloc(_bloc, i) == _HEADER_FILE_)
            {
                for(k=0 ; k<=_si-1; k++)
                {
                    if(_map[k] == 1)
                    {
                        printf("│  ");
                    }
                    else
                    {
                        printf("   ");
                    }
                }
                if(i >= _bloc->m_size - _RECORDING_ || i >= _bloc->m_currentPosition - _RECORDING_)
                {
                    printf("└──");
                    _tab = "   ";
                    _map[_si] = 0;
                }
                else
                {
                    printf("├──");
                }
                printf("f %s\n",name);
            }
            else
            {
                for(k=0 ; k<=_si-1; k++)
                {
                    if(_map[k] == 1)
                    {
                        printf("│  ");
                    }
                    else
                    {
                        printf("   ");
                    }
                }
                if(i >= _bloc->m_size - _RECORDING_ || i >= _bloc->m_currentPosition - _RECORDING_)
                {
                    printf("└──");
                    _tab = "   ";
                    _map[_si] = 0;
                }
                else
                {
                    printf("├──");
                }
                printf("d %s\n",name);
            }
            free(name);
        }
        if(_si == 0)
        {
            free(_map);
        }
    }
    else
    {
        fprintf(stderr, "[displayTreeDisk] paramètre NULL\n");
    }
}

void addToFileDisk(disk_t* _disk, char* _name, char* _data)
{
    if (_disk != NULL)
    {
        int i = 0;
        int ok = 1;
        char* temp;
        int dir;
        while(ok)
        {
            temp = getDataBloc(_disk->m_blocs[_disk->m_currentDirectory] , _HEADER_SIZE_ + i*_RECORDING_);
            if(temp == NULL)
            {
                break;
            }
            if(strcmp(temp,_name)==0 && getCharDataBloc(_disk->m_blocs[_disk->m_currentDirectory],i*_RECORDING_) == _HEADER_FILE_)
            {
                break;
            }
            i++;
            free(temp);
        }
        if(temp != NULL && strcmp(temp,_name) == 0 && getCharDataBloc(_disk->m_blocs[_disk->m_currentDirectory],i*_RECORDING_) == _HEADER_FILE_)
        {
            dir = getIntDataBloc(_disk->m_blocs[_disk->m_currentDirectory],i*_RECORDING_ + _HEADER_SIZE_ + _NAME_SIZE_);
            addDataBloc(_disk->m_blocs[dir],_data);
        }
        else
        {
            printf("[addToFileDisk] le fichier n'existe pas\n");
        }
        free(temp);
    }
    else
    {
        fprintf(stderr, "[addToFileDisk] paramètre NULL\n");
    }
}
