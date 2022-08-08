SRC=./src
IDIR=./include
ODIR=./obj

CC=gcc
CFLAGS=-I$(IDIR)

_OBJ=main.o
OBJ=$(patsubst %, $(ODIR)/%, $(_OBJ))

DEPS=

$(ODIR)/%.o:	%.c $(DEPS)
				$(CC) -c -o $@ $< $(CFLAGS)

main:	$(OBJ)
		$(CC) -o $@ $^ $(CFLAGS)

clean:
		rm -f $(ODIR)/*.o
