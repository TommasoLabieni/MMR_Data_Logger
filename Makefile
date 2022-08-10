include ./Makefile.variable

SRC=./src
IDIR=./include
ODIR=./obj
UTILSDIR=./utilities


UTILS_SRC=$(UTILSDIR)/*.c

_OBJ=main.o
OBJ=$(patsubst %, $(ODIR)/%, $(_OBJ))

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
