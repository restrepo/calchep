.PHONY: all

include ../../FlagsForMake

Idir = .. chep_crt plot  service2 getmem

override CFLAGS += -Iinclude $(patsubst %,-I../%/include ,$(Idir))

special = n_calchep.c usrfun.c pdf_dummy.c   userInt.c

OBJ= $(notdir $(subst .c,.o,$(filter-out $(special),$(wildcard *.c strfun/*.c))))
VPATH = strfun
lib = ../../lib
bin = ../../bin

all: $(lib)/num_c.a $(lib)/dummy.a $(lib)/n_calchep.o  $(bin)/Int

$(lib)/num_c.a(decay.o): decay.inc

$(lib)num_c.a(decayLD.o): decay.inc 

$(lib)/num_c.a(q_kin.o): q_kin.inc

$(lib)/num_c.a(q_kinLD.o): q_kin.inc

$(lib)/num_c.a(4_vector.o): 4_vector.inc
                                                                                
$(lib)/num_c.a(4_vectorLD.o): 4_vector.inc



$(lib)/num_c.a:$(lib)/num_c.a($(OBJ))
	$(RANLIB) $@
	@rm -f $(OBJ)

$(lib)/dummy.a:$(lib)/dummy.a(usrfun.o pdf_dummy.o)
	$(RANLIB) $@
	@rm -f usrfun.o pdf_dummy.o

 $(lib)/n_calchep.o: n_calchep.c
	$(CC) $(CFLAGS) -c -o $@  $<

$(bin)/Int:  userInt.c $(lib)/serv.a $(lib)/num_c.a
	$(CC) -o $(bin)/Int  $(CFLAGS)  $(LDFALGS) userInt.c $(lib)/serv.a $(lib)/num_c.a  -lm
