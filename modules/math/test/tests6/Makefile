exename := tests6.elf

cc := c++
objdir := objs
includedirs := -I../../../test/include/ -I../../include/
compiler_flags := -std=c++11 -pthread -DDEBUG_MODE

libdir := ../../../lib
libs := -ltestutils -llimf-d
link_flags := #-s

files := $(shell find . -name "*.cpp")
objects := $(addprefix $(objdir)/, $(patsubst %.cpp,%.o, $(notdir $(files))))

.PHONY: clean

$(objdir)/%.o: %.cpp | $(objdir)
	$(cc) $(compiler_flags) -c $(includedirs) -o $@ $<

$(exename): $(objects)
	$(cc) $(link_flags) -L$(libdir) -o $@ $(objects) -pthread $(libs)

clean:
	-rm $(objdir)/*
	-rm $(exename)

$(objdir):
	mkdir -p $(objdir)
