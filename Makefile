SRC=./src
IDIR=./include
ODIR=./obj
UTILSDIR=./utilities

CC=gcc
CFLAGS=-I$(IDIR) -lcjson

_OBJ=main.o
_OBJ_UTILS=can_utilities.o json_utilities.o

UTILS_SRC=$(UTILSDIR)/*.c

OBJ=$(patsubst %, $(ODIR)/%, $(_OBJ))
OBJ_UTILS=$(patsubst %, $(ODIR)/%, $(_OBJ_UTILS))

all: main

$(ODIR)/%.o:	%.c $(IDIR)/*.h
				$(CC) -c -o $@ $< $(CFLAGS)

$(OBJ_UTILS):	$(UTILS_SRC)
	@if test ! -d "./obj" ; then \
		echo "Creating obj DIR"; \
    	mkdir ./obj; \
	fi
	cd $(UTILSDIR) && $(MAKE)

main:	$(OBJ_UTILS) $(OBJ)
		$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean clean_all

clean:
		rm -rf $(ODIR)

clean_all:
		rm -rf $(ODIR) main
