.PHONY: all

include ../../FlagsForMake

lib = ../../lib

override CFLAGS += -Iinclude
all: $(lib)/model_aux.a

OBJ = $(notdir $(subst .c,.o,$(wildcard *.c)))

$(lib)/model_aux.a: $(lib)/model_aux.a($(OBJ))
	$(RANLIB) $@
	@rm -f $(OBJ)
