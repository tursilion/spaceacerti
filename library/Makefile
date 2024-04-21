#note: no cart in this one

# Paths to TMS9900 compilation tools
GAS=/home/tursilion/newtms9900-gcc/newgcc9900/bin/tms9900-as
LD=/home/tursilion/newtms9900-gcc/newgcc9900/bin/tms9900-ld
CC=/home/tursilion/newtms9900-gcc/newgcc9900/bin/tms9900-gcc
CP=/usr/bin/cp

# Path to elf2cart conversion utility
ELF2CART=/home/tursilion/elf2cart
ELF2EA5=/home/tursilion/elf2ea5
EA5PLIT=/home/tursilion/ea5split

# Flags used during linking
#
# Locate the code (.text section) and the data (.data section)
LDFLAGS_EA5=\
  --section-start .text=a000 --section-start .data=2080 -M

INCPATH=-I/mnt/d/work/libti99all
LIBPATH=-L/mnt/d/work/libti99all/buildti
LIBS=-lti99

C_FLAGS=\
  -DTI99 -Os -std=c99 -s --save-temp -fno-peephole2 -fno-builtin -fno-function-cse

# List of compiled objects used in executable
OBJECT_LIST_EA5=\
  crt0_ea5.o\
  
all: story story2

# Recipe to compile the executable
story: $(OBJECT_LIST_EA5) story1reader.o
	$(LD) $(OBJECT_LIST_EA5) story1reader.o $(LIBS) $(LIBPATH) $(LDFLAGS_EA5) -o storyread.ea5.elf > ea5.map
	$(ELF2EA5) storyread.ea5.elf storyread.ea5.bin
	$(EA5PLIT) storyread.ea5.bin
	$(CP) STORYREAD* /mnt/d/classic99/dsk1/

# Recipe to compile the executable
story2: $(OBJECT_LIST_EA5) story2reader.o
	$(LD) $(OBJECT_LIST_EA5) story2reader.o $(LIBS) $(LIBPATH) $(LDFLAGS_EA5) -o story2read.ea5.elf > ea5.map
	$(ELF2EA5) story2read.ea5.elf story2read.ea5.bin
	$(EA5PLIT) story2read.ea5.bin
	$(CP) STORY2READ* /mnt/d/classic99/dsk1/

# Recipe to clean all compiled objects
.phony clean:
	rm *.o
	rm *.elf
	rm *.map

# Recipe to compile all assembly files

%.o: %.asm
	$(GAS) $< -o $@

# Recipe to compile all C files
%.o: %.c
	$(CC) -c $< $(C_FLAGS) $(INCPATH) -o $@
