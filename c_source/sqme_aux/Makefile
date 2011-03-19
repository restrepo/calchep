
include ../../FlagsForMake

override CFLAGS +=  -I../../include

OBJ = $(subst .c,.o,$(wildcard *.c))

lib = ../../lib
$(lib)/sqme_aux.o:sqme_aux.c sqme_aux.inc
	$(CC) $(CFLAGS) -c -o $@  sqme_aux.c
