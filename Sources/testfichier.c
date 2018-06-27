#include "testfichier.h"

void lauchCopy()
{
  int fd1;
  int fd2;
  int res;
  char t;
  int i=0;
  char* file = (char*)malloc(sizeof(char) * 100);
  fprintf(stdout,"Quel fichier voulez-vous copié : ");
  if(scanf("%100s", file) == EOF) 
  {
   	perror("Erreur scanf");
   	exit(EXIT_FAILURE);
  }
 	if((fd1 = open(file, O_RDWR, S_IRUSR|S_IWUSR)) == -1) 
 	{
   	perror("Erreur lors de l'ouverture du fichier");
   	exit(EXIT_FAILURE);
  }

  free(file);
  file = (char*)malloc(sizeof(char) * 100);
  fprintf(stdout,"Dans quel fichier voulez-vous copié : ");
  if(scanf("%100s", file) == EOF) 
  {
   	perror("Erreur scanf");
   	exit(EXIT_FAILURE);
  }
 	if((fd2 = open(file, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR)) == -1) 
 	{
    	perror("Erreur lors de l'ouverture du fichier");
    	exit(EXIT_FAILURE);
  	}
	do 
	{
		res = read(fd1, &t, sizeof(char));
	    if(res == -1)
	    {
	    	fprintf(stderr,"Erreur lors de la lecture de la valeur %d\n", i);
	      	exit(EXIT_FAILURE);
	    }
	    if(write(fd2, &t, sizeof(char)) == -1) 
  		{
    		perror("Erreur write");
    		exit(EXIT_FAILURE);
  		}
  		i++;
	} while(res != 0);

  	if(close(fd1) == -1) 
  	{
    	perror("Erreur lors de la fermeture du ficher");
    	exit(EXIT_FAILURE);
  	}
  	if(close(fd2) == -1) 
  	{
    	perror("Erreur lors de la fermeture du ficher");
    	exit(EXIT_FAILURE);
  	}	
  	free(file);
}

void lauchAddToFile()
{
    int fd;
    char* toadd;
    int res;
  	char t;
    int i = 0;
    char* file = (char*)malloc(sizeof(char) * 100);
  	fprintf(stdout,"Quel fichier voulez-vous ouvrir/créer : ");
  	if(scanf("%100s", file) == EOF) 
  	{
    	perror("Erreur scanf");
    	exit(EXIT_FAILURE);
  	}

 	if((fd = open(file, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR)) == -1) 
 	{
    	perror("Erreur lors de l'ouverture du fichier");
    	exit(EXIT_FAILURE);
  	}

  	toadd= (char*)malloc(sizeof(char) * 999);
  	printf("Que voulez-vous ajouter : ");
  	if(scanf("%999s", toadd) == EOF) 
  	{
    	perror("Erreur scanf");
    	exit(EXIT_FAILURE);
  	}

  	do 
	{
		res = read(fd, &t, sizeof(char));
	    if(res == -1)
	    {
  			perror("Erreur lors du deplacement dans le fichier");
	      	exit(EXIT_FAILURE);
	    }
	} while(res != 0);	

 
  	if(write(fd, toadd, sizeof(char) * strlen(toadd)) == -1) 
  	{
    	perror("Erreur write");
    	exit(EXIT_FAILURE);
  	}

  	if(lseek(fd,0,SEEK_SET) == (off_t)-1)
  	{
  		perror("Erreur lors du deplacement dans le fichier");
  		exit(EXIT_FAILURE);
  	}

	do 
	{
		res = read(fd, &t, sizeof(char));
	    if(res == -1)
	    {
	    	fprintf(stderr, "Erreur lors de la lecture de la valeur %d", i);
	      	perror(" ");
	      	exit(EXIT_FAILURE);
	    }
	    if(res != 0)
      		fprintf(stdout, "%c", t);
	} while(res != 0);

	printf("\n");

  	if(close(fd) == -1) 
  	{
    	perror("Erreur lors de la fermeture du ficher");
    	exit(EXIT_FAILURE);
  	}	
  	free(file);
  	free(toadd);
}

void launchTaillefichier()
{
    int fd;
    int res;
  	char t;
  	int begin = 0;
  	int end = -1;
	char part = 'a';
  	int i = 0;
	char* file = (char*)malloc(sizeof(char) * 100);
  	fprintf(stdout,"Quel fichier voulez-vous ouvrir : ");
  	if(scanf("%100s", file) == EOF) 
  	{
    	perror("Erreur scanf");
    	exit(EXIT_FAILURE);
  	}

 	if((fd = open(file, O_RDWR, S_IRUSR|S_IWUSR)) == -1) 
 	{
    	perror("Erreur lors de l'ouverture du fichier");
    	exit(EXIT_FAILURE);
  	}

  	fprintf(stdout,"Voulez-vous faire un affichage ? y/n : ");
  	while(part != 'y' && part!='n')
  	{
	  	if(scanf("%c", &part) == EOF) 
	  	{
	    	perror("Erreur scanf");
	    	exit(EXIT_FAILURE);
	  	}
  	}

  	if(part == 'y')
  	{
	  	fprintf(stdout,"Saisissez la valeur de debut : ");
	  	if(scanf("%d", &begin) == EOF) 
	  	{
	    	perror("Erreur scanf");
	    	exit(EXIT_FAILURE);
	  	}
	  	if(begin < 0)
	  	{
	  		perror("Valeur inferieur a 0\n");
	  		begin = 0;
	  	}
	  	fprintf(stdout,"Saisissez la valeur de fin : ");
	  	if(scanf("%d", &end) == EOF) 
	  	{
	    	perror("Erreur scanf");
	    	exit(EXIT_FAILURE);
	  	}
	  	if(end < begin)
	  	{
	  		perror("Valeur inferieur a la valeur de debut");
	  		part = 'n';
	  	}
  	}


	do 
	{
		res = read(fd, &t, sizeof(char));
	    if(res == -1)
	    {
	    	fprintf(stderr,"Erreur lors de la lecture de la valeur %d\n", i);
	      	exit(EXIT_FAILURE);
	    }
	    if(res != 0)
	    {
	      	i++;
	      	if(i > begin && i < end && part == 'y')
	      		fprintf(stdout, "%c", t);
	    }
	} while(res != 0);	

        fprintf(stdout,"\ntaille du fichier \"%s\": %lu octets\n",file, i*sizeof(char));

  	if(close(fd) == -1) 
  	{
    	perror("Erreur lors de la fermeture du ficher");
    	exit(EXIT_FAILURE);
  	}
  	free(file);
}
