# Paths to TMS9900 compilation tools 
# ( Set in environment to override paths )
#TMS9900_DIR?=/home/tursilion/gcc9900/bin
TMS9900_DIR?=/home/tursilion/newtms9900-gcc/newgcc9900/bin
ELF2EA5_DIR?=/home/tursilion/gcc9900/bin
EA5_SPLIT_DIR?=/home/tursilion/gcc9900/bin
CLASSIC99_DSK1?=/mnt/d/classic99/DSK1/
LIBTI99?=/mnt/d/work/ti/libti99/

# Full paths to the executables used
GAS=$(TMS9900_DIR)/tms9900-as
LD=$(TMS9900_DIR)/tms9900-ld
CC=$(TMS9900_DIR)/tms9900-gcc

CP=/usr/bin/cp
NAME=library

# Flags used during linking
LDFLAGS_EA5=--section-start .text=a000 --section-start .data=2080 -M

INCPATH=-I/mnt/d/work/libti99ALL
LIBPATH=-L/mnt/d/work/libti99ALL/buildti
LIBS=-lti99

# Note: no-function-cse prevents gcc from preferring registers for bl calls
CFLAGS=-Os -std=c99 -c -s --save-temp -DTI99 -fno-builtin -fno-function-cse
#-minline_rtl

.PHONY: all clean

# crt0 must be first!
OBJECT_LIST = crt0.o main.o

# List of all files needed in executable
PREREQUISITES=$(OBJECT_LIST)

all: $(PREREQUISITES)
	$(LD) $(OBJECT_LIST) $(LIBS) $(LIBPATH) $(LDFLAGS_EA5) -o $(NAME).ea5.elf > ea5.map
	$(ELF2EA5_DIR)/elf2ea5 $(NAME).ea5.elf $(NAME).ea5
	$(EA5_SPLIT_DIR)/ea5split $(NAME).ea5

clean:
	-rm *.o *.map *.lst *.sym *.ihx *.rom

# Since we don't need custom segment tags anymore, we could in theory just use a generic recipe...

main.o: main.c
	$(CC) $(CFLAGS) $(INCPATH) -o main.o main.c

# libti99 crt0 copied into folder
crt0.o: crt0.asm
	$(GAS) $(AFLAGS) crt0.asm -o crt0.o
