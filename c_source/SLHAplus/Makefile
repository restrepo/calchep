.PHONY: all

include ../../FlagsForMake
CALCHEP=../..

all: $(CALCHEP)/lib/libSLHAplus.a
 special = test_rdiag.c test_slha.c
OBJ = $(notdir $(subst .c,.o,$(filter-out $(special),$(wildcard *.c ))))
override CFLAGS += -I./include

$(CALCHEP)/lib/libSLHAplus.a:$(CALCHEP)/lib/libSLHAplus.a($(OBJ))
	$(RANLIB) $(CALCHEP)/lib/libSLHAplus.a
	rm -f *.o
	cp $(CALCHEP)/lib/libSLHAplus.a $(CALCHEP)/lib/model_aux.a
 
