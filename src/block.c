#include "block.h"

bloc_t* createBloc(int _size, char* _data)
{
    bloc_t* res = (bloc_t*)malloc(sizeof(bloc_t));
    res->m_size = _size;
    res->m_data = (char*)malloc(sizeof(char) * res->m_size);
    memset(res->m_data, '\0', res->m_size);
    res->m_currentPosition = 0;
    if (_data != NULL)
    {
        if (strlen(_data) <= res->m_size)
        {
            strcpy(res->m_data, _data);
            res->m_currentPosition = strlen(_data);
        }
        else
        {
            printf("[createBloc] longueur de la chaine supérieur à la taille du bloc\n");
        }
    }
    return res;
}

bloc_t* createFromFileBloc(int _fd)
{
    bloc_t* res = (bloc_t*)malloc(sizeof(bloc_t));

    int readRes = read(_fd, &(res->m_size), sizeof(int));
    if(readRes == -1)
    {
        fprintf(stderr, "[createFromFileBloc] Erreur lors de la lecture de la taille\n");
        exit(EXIT_FAILURE);
    }
    readRes = read(_fd, &(res->m_currentPosition), sizeof(int));
    if(readRes == -1)
    {
        fprintf(stderr, "[createFromFileBloc] Erreur lors de la lecture de la position courante\n");
        exit(EXIT_FAILURE);
    }

    res->m_data = (char*)malloc(sizeof(char) * res->m_size);

    readRes = read(_fd, res->m_data, sizeof(char) * res->m_size);
    if(readRes == -1)
    {
        fprintf(stderr, "[createFromFileBloc] Erreur lors de la lecture de la position courante\n");
        exit(EXIT_FAILURE);
    }

    return res;

}

int destroyBloc(bloc_t* _bloc)
{
    if (_bloc != NULL)
    {
        free(_bloc->m_data);
        free(_bloc);
        return 1;
    }
    else
    {
        fprintf(stderr, "[destroyBloc] paramètre NULL\n");
    }
    return 0;
}

int addDataBloc(bloc_t* _bloc, char* _data)
{
    if(_bloc != NULL)
    {
        if (_data != NULL)
        {
            if (strlen(_data) <= _bloc->m_size - _bloc->m_currentPosition)
            {
                strcpy(_bloc->m_data + _bloc->m_currentPosition, _data);
                _bloc->m_currentPosition = _bloc->m_currentPosition + strlen(_data);
                return 1;
            }
            else
            {
                printf("[addDataBloc] longueur de la chaine supérieur à la taille du bloc\n");
            }
        }
    }
    else
    {
        fprintf(stderr, "[addDataBloc] paramètre NULL\n");
    }
    return 0;
}

int addCharDataBloc(bloc_t* _bloc, char _data)
{
    if(_bloc != NULL)
    {
        if (1 <= _bloc->m_size - _bloc->m_currentPosition)
        {
            if(_data == 0x0)
            {
                _data = '0';
            }
            memcpy(_bloc->m_data + _bloc->m_currentPosition, &_data, sizeof(_data));
            _bloc->m_currentPosition++;
            return 1;
        }
        else
        {
            printf("[addCharDataBloc] longueur de la chaine supérieur à la taille du bloc\n");
        }
    }
    else
    {
        fprintf(stderr, "[addCharDataBloc] paramètre NULL\n");
    }
    return 0;
}

char* getDataBloc(bloc_t* _bloc, int _i)
{
    if (_bloc != NULL)
    {
        char c;
        char* res;
        int j;
        int taille = 0;
        if(_i>_bloc->m_currentPosition)
        {
            return NULL;
        }
        while((c=_bloc->m_data[_i + taille]) != END_OF_STRING)
        {
            ++taille;
        }
        res = (char*)malloc(sizeof(char) * taille + sizeof(char));
        for(j=0 ; j<taille ; j++)
        {
            res[j] = '0';
        }
        taille = 0;
        while((c = _bloc->m_data[_i + taille]) != END_OF_STRING)
        {
            res[taille] = c;
            taille++;
        }
        res[taille] = '\0';
        return res;

    }
    else
    {
        fprintf(stderr, "[getDataBloc] paramètre NULL\n");
    }

    return 0;
}

char getCharDataBloc(bloc_t* _bloc, int _i)
{
    if (_bloc != NULL)
    {
        if(_i>_bloc->m_currentPosition)
        {
            return '0';
        }
        return _bloc->m_data[_i];

    }
    else
    {
        fprintf(stderr, "[getCharDataBloc] paramètre NULL\n");
    }

    return 0;
}

int getIntDataBloc(bloc_t* _bloc, int _i)
{
    int value;
    if (_bloc != NULL)
    {
        char c;
        char* res;
        int taille = 0;
        if(_i>_bloc->m_currentPosition)
        {
            return 0;
        }
        res = (char*)malloc(sizeof(char) * sizeof(int) + sizeof(char));
        while(taille != 4)
        {
            c = _bloc->m_data[_i + taille];
            res[taille] = c;
            taille++;
        }
        res[sizeof(int)] = '\0';
        value = atoi(res);
        free(res);
        return value;

    }
    else
    {
        fprintf(stderr, "[getIntDataBloc] paramètre NULL\n");
    }

    return 0;
}

int addIntDataBloc(bloc_t* _bloc, int _num)
{
    if(_bloc != NULL)
    {
        char* to;
        int i;
        int j=0;
        char* n = itoChar(_num);
        if(strlen(n) > sizeof(int))
        {
            printf("[addIntDataBloc] valeur trop grande\n");
            return 0;
        }
        if(strlen(n) != sizeof(int))
        {
            to = (char*)malloc(sizeof(char) * sizeof(int) + sizeof(char));
            for(i=0 ; i<sizeof(int) ; i++)
            {
                if(i>=sizeof(int) - strlen(n))
                {
                    to[i] = n[j++];
                }
                else
                {
                    to[i] = '0';
                }
            }
            to[sizeof(int)] = '\0';
            if(addDataBloc(_bloc,to) == 0)
            {
                printf("[addIntDataBloc] pas assez de place\n");
                return 0;
            }
            free(to);
        }
        else if(addDataBloc(_bloc,n) == 0)
        {
            printf("[addIntDataBloc] pas assez de place\n");
            return 0;
        }
        free(n);
        return 1;
    }
    else
    {
        fprintf(stderr, "[addIntDataBloc] paramètre NULL");
    }
    return 0;
}

void displayBloc(bloc_t* _bloc)
{
    if(_bloc != NULL)
    {
        printf("[bloc]\n\tsize : %d\n\tcurrent position : %d\n\tdata : %s\n",_bloc->m_size,_bloc->m_currentPosition,_bloc->m_data);
    }
    else
    {
        fprintf(stderr, "[displayBloc] paramètre NULL\n");
    }
}

int saveBloc(bloc_t* _bloc, int _fd)
{
    if (_bloc != NULL)
    {
        if(write(_fd, &(_bloc->m_size), sizeof(int)) == -1)
        {
            fprintf(stderr, "[saveBloc] Erreur write\n");
            exit(EXIT_FAILURE);
        }

        if(write(_fd, &(_bloc->m_currentPosition), sizeof(int)) == -1)
        {
            fprintf(stderr, "[saveBloc] Erreur write\n");
            exit(EXIT_FAILURE);
        }

        if(write(_fd, _bloc->m_data, sizeof(char) * _bloc->m_size) == -1)
        {
            fprintf(stderr, "[saveBloc] Erreur write\n");
            exit(EXIT_FAILURE);
        }
        return 1;

    }
    else
    {
        fprintf(stderr, "[saveBloc] paramètre NULL\n");
    }
    return 0;

}

int isFullBloc(bloc_t* _bloc)
{
    if (_bloc != NULL)
    {
        return _bloc->m_currentPosition == _bloc->m_size;
    }
    else
    {
        fprintf(stderr, "[isFullBloc] paramètre NULL\n");
    }

    return 0;

}

void emptyBloc(bloc_t* _bloc)
{
    if (_bloc != NULL)
    {
        int i;
        for(i=0 ; i<_bloc->m_size ; i++)
        {
            _bloc->m_data[i] = '\0';
        }
        _bloc->m_currentPosition = 0;
    }
    else
    {
        fprintf(stderr, "[emptyBloc] paramètre NULL\n");
    }
}

void removeBloc(bloc_t* _bloc, int _beg, int _end)
{
    if (_bloc != NULL)
    {
        if(_beg >= 0 && _beg < _end && _beg <_bloc->m_size && _end<_bloc->m_size)
        {
            char* pcs;
            char* pcd;
            int diff = _bloc->m_size - _end;
            _bloc->m_currentPosition -= _end-_beg+1;
            for(pcd = _bloc->m_data + _beg, pcs = _bloc->m_data + _end + 1; pcs < _bloc->m_data + _bloc->m_size ; pcd++, pcs++)
            {
                *pcd = *pcs;
            }
            memset(_bloc->m_data + _beg + diff, 0, _bloc->m_size-_beg-diff);
        }
        else
        {
            printf("[remove] borne incorecte\n");
        }
    }
    else
    {
        fprintf(stderr, "[remove] paramètre NULL");
    }
}
