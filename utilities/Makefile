include ../Makefile.variable

SRC=../src
IDIR=../include
ODIR=../obj

CC=gcc

all: $(OBJ_UTILS)

$(ODIR)/%.o:	%.c $(IDIR)/*.h
				$(CC) -c -o $@ $< $(CFLAGS)
