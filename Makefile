SRC=./src
IDIR=./include
ODIR=./obj
UTILSDIR=./utilities

CC=gcc
CFLAGS=-I$(IDIR)

_OBJ=main.o
_OBJ_UTILS=can_utilities.o json_utilities.o
OBJ=$(patsubst %, $(ODIR)/%, $(_OBJ))
OBJ_UTILS=$(patsubst %, $(ODIR)/%, $(_OBJ_UTILS))

all: main

$(ODIR)/%.o:	%.c $(IDIR)/*.h
				$(CC) -c -o $@ $< $(CFLAGS)

$(OBJ_UTILS):
	@if test ! -d "./obj" ; then \
		echo "Creating obj DIR"; \
    	mkdir ./obj; \
	fi
	cd $(UTILSDIR) && $(MAKE)

main:	$(OBJ_UTILS) $(OBJ)
		$(CC) -o $@ $^ $(CFLAGS)

clean:
		rm -rf $(ODIR) main
