.SUFFIXES:.
SHELL=/bin/sh


prefix ?= /usr/local
exec_prefix ?= $(prefix)
bindir ?= $(exec_prefix)/bin
sbindir ?= $(exec_prefix)/sbin
datarootdir ?= $(prefix)/share
datadir ?= $(datarootdir)
sysconfdir ?= $(prefix)/etc

INSTALL=install -c

CFLAGS+=-std=c99
#CXXFLAGS=-std=c++11
#CXXFLAGS=-std=c++14
CXXFLAGS=-std=c++17
CPPFLAGS+=-Wpedantic -Wall -Wextra -Werror
CPPFLAGS+=-O2
CPPFLAGS+=-g
CPPFLAGS+=-I$(srcdir)
CPPFLAGS+=-fno-optimize-sibling-calls
#CPPFLAGS+=-fno-inline

LDLIBS+=-lpthread

# note: don't prefix ./ in dirnames
srcdir:=src/nel
testdir:=tests
builddir:=build/$(shell uname -m)-$(shell uname -s)-$(CXX)

###########################################

TARGETS:=
#TARGES+=$(builddir)/akish1
CHECK_TARGETS:= #Makefile
CHECK_TARGETS+=$(builddir)/test-optional
CHECK_TARGETS+=$(builddir)/test-result
CHECK_TARGETS+=$(builddir)/test-performance
CHECK_TARGETS+=$(builddir)/test_performance.s
#SRCS:=$(srcdir)/actorref.cc
#SRCS+=$(srcdir)/actorcontext.cc
#SRCS+=$(srcdir)/actor.cc
#SRCS+=$(srcdir)/queue.cc
#SRCS+=$(srcdir)/mailbox.cc
#SRCS+=$(srcdir)/scheduler.cc

CHECK_SRCS:= #Makefile
CHECK_SRCS+=$(testdir)/test_optional.cc
CHECK_SRCS+=$(testdir)/test_result.cc
CHECK_SRCS+=$(testdir)/test_performance.cc

OBJS:=$(SRCS:.cc=.o)
CHECK_OBJS:=$(CHECK_SRCS:.cc=.o)

OBJS:=$(patsubst $(srcdir)/%,$(builddir)/%,$(OBJS))
CHECK_OBJS:=$(patsubst $(testdir)/%,$(builddir)/%,$(CHECK_OBJS))

.PHONY: all distclean clean compile check lint

all: compile

distclean: clean
	$(RM) -r $(builddir)
	if [ -e $(builddir) ]; then rmdir -p $(builddir);fi

clean:
	$(RM) -r $(TARGETS)
	$(RM) -r $(OBJS)
	$(RM) -r $(CHECK_TARGETS)
	$(RM) -r $(CHECK_OBJS)

# gnu make manul says check target is to self-test the program, but the program must be already built (i.e. don't build prog as a dep).
# if this is the case then it cannot be a function-level test (i.e. tests functions)
# it must then be a functional test of the app/program.
# so where to put the function-level tests (aka unittests)?

check: $(CHECK_TARGETS)
compile: $(TARGETS)

$(builddir):
	mkdir -p $@

### component specific rules

# $(builddir)/akish1: $(builddir)/main.o

$(builddir)/test-optional: $(builddir)/test_optional.o
	$(LINK.cc) $^ $(LOADLIBES) $(LDLIBS) -o $@

$(builddir)/test-result: $(builddir)/test_result.o
	 $(LINK.cc) $^ $(LOADLIBES) $(LDLIBS) -o $@

$(builddir)/test-performance: $(builddir)/test_performance.o
	 $(LINK.cc) $^ $(LOADLIBES) $(LDLIBS) -o $@

#$(builddir)/test_performance.s: $(srcdir)/test_performance.cc

### handle auto deps generation for makefile ###

DEPDIR := $(builddir)/deps.d

#$(shell mkdir -p $(DEPDIR) >/dev/null)
$(DEPDIR):
	mkdir -p $@

DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td

COMPILE.c = $(CC) $(DEPFLAGS) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c
COMPILE.cc = $(CXX) $(DEPFLAGS) $(CXXFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c
POSTCOMPILE = mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d


$(builddir)/%.o : $(srcdir)/%.c
$(builddir)/%.o : $(srcdir)/%.c $(DEPDIR)/%.d | $(DEPDIR)  $(builddir)
	$(COMPILE.c) $(OUTPUT_OPTION) $<
	$(POSTCOMPILE)

$(builddir)/%.o : $(srcdir)/%.cc
$(builddir)/%.o : $(srcdir)/%.cc $(DEPDIR)/%.d | $(DEPDIR)  $(builddir)
	$(COMPILE.cc) $(OUTPUT_OPTION) $<
	$(POSTCOMPILE)

$(builddir)/%.o : $(testdir)/%.cc
$(builddir)/%.o : $(testdir)/%.cc $(DEPDIR)/%.d | $(DEPDIR)  $(builddir)
	$(COMPILE.cc) -O2 -g0 $(OUTPUT_OPTION) $<
	$(POSTCOMPILE)

$(builddir)/%.s : $(testdir)/%.cc
$(builddir)/%.s : $(testdir)/%.cc $(DEPDIR)/%.d | $(DEPDIR)  $(builddir)
	$(COMPILE.cc) -O2 -g0 -S $(OUTPUT_OPTION) $<

$(builddir)/%.o : $(srcdir)/%.cxx
$(builddir)/%.o : $(srcdir)/%.cxx $(DEPDIR)/%.d | $(DEPDIR) $(builddir)
	$(COMPILE.cc) $(OUTPUT_OPTION) $<
	$(POSTCOMPILE)

$(DEPDIR)/%.d: ;
.PRECIOUS: $(DEPDIR)/%.d

.PHONY: clean.depdir
clean.depdir:
	$(RM) $(patsubst %,$(DEPDIR)/%.Td,$(basename $(notdir $(OBJS))))
	$(RM) $(patsubst %,$(DEPDIR)/%.Td,$(basename $(notdir $(CHECK_OBJS))))
	$(RM) $(patsubst %,$(DEPDIR)/%.d,$(basename $(notdir $(OBJS))))
	$(RM) $(patsubst %,$(DEPDIR)/%.d,$(basename $(notdir $(CHECK_OBJS))))

clean: clean.depdir

.PHONY: distclean.depdir
distclean.depdir: clean.depdir
distclean.depdir:
	if [ -e $(DEPDIR) ]; then rmdir -p $(DEPDIR);fi

distclean: distclean.depdir

-include $(patsubst $(builddir)/%,$(DEPDIR)/%.d,$(basename $(OBJS)))
-include $(patsubst $(builddir)/%,$(DEPDIR)/%.d,$(basename $(CHECK_OBJS)))
## end auto deps handling for makefile##
