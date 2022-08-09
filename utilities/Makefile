SRC=../src
IDIR=../include
ODIR=../obj

CC=gcc
CFLAGS=-I$(IDIR)

_OBJ=can_utilities.o json_utilities.o
OBJ=$(patsubst %, $(ODIR)/%, $(_OBJ))

all: $(OBJ)

$(ODIR)/%.o:	%.c $(IDIR)/*.h
				$(CC) -c -o $@ $< $(CFLAGS)
