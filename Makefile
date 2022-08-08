SRC=./src
IDIR=./include
ODIR=./obj

CC=gcc
CFLAGS=-I$(IDIR)

_OBJ=main.o can_utilities.o
OBJ=$(patsubst %, $(ODIR)/%, $(_OBJ))

$(ODIR)/%.o:	%.c $(IDIR)/*.h
				$(CC) -c -o $@ $< $(CFLAGS)

main:	$(OBJ)
		$(CC) -o $@ $^ $(CFLAGS)

clean:
		rm -f $(ODIR)/*.o main
