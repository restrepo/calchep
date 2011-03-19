.PHONY: all

include ../../FlagsForMake

lib = ../../lib
bin = ../../bin


ifeq (,$(CERN))
   nt_maker =
else
ifeq (,$(FC))
   nt_maker =
else    
   nt_maker = $(bin)/nt_maker
endif 
endif

all: $(bin)/event_mixer  $(nt_maker)

$(bin)/event_mixer:event_mixer.c event2pyth.c $(lib)/libSLHAplus.a
	$(CC) $(CFLAGS)   -o $@ $^ -lm 

$(bin)/nt_maker:nt_maker_02.F readLHE.c 
	- $(FC) $(FFLAGS) -m32 -o $@ $^ -L$(CERN)/pro/lib/ -lpacklib_noshift

