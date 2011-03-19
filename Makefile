
.PHONY: all  FLAGS COMPILE clean

ifeq ($(MAKECMDGOALS),clean)
clean :
	./setPath " "
	rm -f *.a *.o FlagsForMake *.so so_locations CMessage
	cd lib; rm -f *.*
	cd bin; rm -f Int calc events2tab s_calchep show_distr sum_distr plot_view event_info event_mixer make_VandP lhe2tab nt_maker
	rm -fr test/tmp/* test/results/*    test/Processes test/Events test/html
	if(test -r FlagsForSh) then rm -i  FlagsForSh; fi
	- unlink test/bin
endif

all:FLAGS COMPILE

FLAGS:FlagsForSh FlagsForMake

FlagsForSh:
	./getFlags
	rm -f *.a *.o  *.so
	@if(test -r CMessage) then cat CMessage; fi
FlagsForMake:FlagsForSh
	./getFlags
	rm -f *.a *.o  *.so
	@if(test -r CMessage) then cat CMessage; fi
COMPILE:FlagsForMake
	./setPath $(CURDIR)
	$(MAKE) -C c_source/getmem 
	$(MAKE) -C c_source/chep_crt
	$(MAKE) -C c_source/service2
	$(MAKE) -C c_source/polynom  
	$(MAKE) -C c_source/symb
	$(MAKE) -C c_source/sqme_aux
	$(MAKE) -C c_source/plot 
	$(MAKE) -C c_source/num 
	$(MAKE) -C c_source/tab
	$(MAKE) -C c_source/SLHAplus
	$(MAKE) -C c_source/mix_events
	@if(test -r CMessage) then cat CMessage; fi
	@if( test ! -d test/bin) then ln -s  `pwd`/bin  `pwd`/test/bin; fi