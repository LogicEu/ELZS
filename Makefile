# elzs makefile

STD=-std=c99
WFLAGS=-Wall -Wextra
OPT=-O2
IDIR=-I.
LIBS=imgtool
CC=gcc
NAME=elzs
SRC=*.c

LDIR=lib
IDIR += $(patsubst %,-I%/,$(LIBS))
LSTATIC=$(patsubst %,lib%.a,$(LIBS))
LPATHS=$(patsubst %,$(LDIR)/%,$(LSTATIC))
LFLAGS=$(patsubst %,-L%,$(LDIR))
LFLAGS += $(patsubst %,-l%,$(LIBS))
LFLAGS += -lz -lpng -ljpeg

SCRIPT=build.sh

CFLAGS=$(STD) $(WFLAGS) $(OPT) $(IDIR)

$(NAME): $(LPATHS) $(SRC)
	$(CC) -o $@ $(SRC) $(CFLAGS) $(LFLAGS)

$(LDIR)/$(LDIR)%.a: $(LDIR)%.a $(LDIR)
	mv $< $(LDIR)/

$(LDIR): 
	@[ -d $@ ] || mkdir $@ && echo "mkdir $@"

$(LDIR)%.a: %
	cd $^ && make && mv $@ ../

exe:
	$(CC) -o $(NAME) $(SRC) $(CFLAGS) $(LFLAGS)

clean: $(SCRIPT)
	./$^ $@
    
install: $(SCRIPT)
	./$^ $@
 
uninstall: $(SCRIPT)
	./$^ $@ 
