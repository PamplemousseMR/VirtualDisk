#ifndef __BLOCK_H__
#define __BLOCK_H__

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "tools.h"

#define _END_OF_STRING_ '!'

typedef struct {

	int size;
	char* data;
	int currentPosition;
} bloc_t;

bloc_t* createBloc(int, char*);
bloc_t* createFromFileBloc(int);
int addDataBloc(bloc_t*, char*);
int addCharDataBloc(bloc_t*,char);
int addIntDataBloc(bloc_t*,int);
void displayBloc(bloc_t*);
int saveBloc(bloc_t*, int);
char* getDataBloc(bloc_t*,int);
char getCharDataBloc(bloc_t*,int);
int getIntDataBloc(bloc_t*,int);
int isFullBloc(bloc_t*);
void emptyBloc(bloc_t*);
void removeBloc(bloc_t*,int,int);

#endif
