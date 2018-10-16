OUTDIR = virtualDiskOFiles/
EXEC = virtualDisk

CC = gcc
CCFLAGS_STD = -Werror -Wall -O3 -pedantic
CCFLAGS = $(CC) $(CCFLAGS_STD) -ansi -o
CCFLAGS_MAIN = $(CC) $(CCFLAGS_STD) -o

all : 
	make virtualDisk

OBJETS = $(OUTDIR)main.o $(OUTDIR)virtualdisk.o $(OUTDIR)block.o $(OUTDIR)tools.o

virtualDisk : virtualDisk_directory virtualDisk_compile

virtualDisk_directory :
	mkdir -p $(OUTDIR)

virtualDisk_compile : $(OBJETS)
	$(CCFLAGS_MAIN) $(EXEC) $(OBJETS)

$(OUTDIR)main.o : src/main.c src/virtualdisk.h
	$(CCFLAGS) $(OUTDIR)main.o -c src/main.c

$(OUTDIR)virtualdisk.o : src/virtualdisk.c src/virtualdisk.h src/block.h src/tools.h
	$(CCFLAGS) $(OUTDIR)virtualdisk.o -c src/virtualdisk.c

$(OUTDIR)block.o : src/block.c src/block.h src/tools.h
	$(CCFLAGS) $(OUTDIR)block.o -c src/block.c

$(OUTDIR)tools.o : src/tools.c src/tools.h
	$(CCFLAGS) $(OUTDIR)tools.o -c src/tools.c

clean :
	rm -f $(OBJETS)
	rm -f $(EXEC) 
	rm -r -f $(OUTDIR)

