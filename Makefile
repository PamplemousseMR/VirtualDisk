OUTDIR = OFiles/
EXEC = disk

CC = gcc
CCFLAGS_STD = -Werror -Wall -O3 -pedantic
CCFLAGS = $(CC) $(CCFLAGS_STD) -ansi -o
CCFLAGS_MAIN = $(CC) $(CCFLAGS_STD) -o

all : 
	make disk

OBJETS = $(OUTDIR)main.o $(OUTDIR)testfichier.o $(OUTDIR)disquevirtuel.o $(OUTDIR)bloc.o $(OUTDIR)tools.o

disk : disk_directory disk_compile

disk_directory :
	mkdir -p $(OUTDIR)

disk_compile : $(OBJETS)
	$(CCFLAGS_MAIN) $(EXEC) $(OBJETS)

$(OUTDIR)main.o : Sources/main.c Sources/testfichier.h Sources/disquevirtuel.h
	$(CCFLAGS) $(OUTDIR)main.o -c Sources/main.c

$(OUTDIR)disquevirtuel.o : Sources/disquevirtuel.c Sources/disquevirtuel.h Sources/bloc.h Sources/tools.h
	$(CCFLAGS) $(OUTDIR)disquevirtuel.o -c Sources/disquevirtuel.c

$(OUTDIR)bloc.o : Sources/bloc.c Sources/bloc.h Sources/tools.h
	$(CCFLAGS) $(OUTDIR)bloc.o -c Sources/bloc.c

$(OUTDIR)tools.o : Sources/tools.c Sources/tools.h
	$(CCFLAGS) $(OUTDIR)tools.o -c Sources/tools.c

$(OUTDIR)testfichier.o : Sources/testfichier.c Sources/testfichier.h
	$(CCFLAGS) $(OUTDIR)testfichier.o -c Sources/testfichier.c

clean :
	rm -f $(OBJETS)
	rm -f $(EXEC) 
	rm -r -f $(OUTDIR)

