# build main code/test/examples for modules
# modules are subfolders from root, containing a main for exes, liba for statics and libso for shared.
# all other .c,.cc files are treated as part of that module.
# src/<modl>main.{c,cc}
# src/<modl>liba.{c,cc}
# src/<modl>libso.{c,cc}
# builds an exe based on module name. <modl> for exes, lib<modl>.a for statics, lib<modl>.so for shared.
# for defined configs (debug,release)
# output is in target/<config>/
# builds tests and runs them for each module.
# tests are .c,.cc files in <modl>/src/ matching pattern test_*.{c,cc}
# tests are built into target/<config>/tests/test_<modl> as an exe
# builds examples as found in the examples/ folder.
# each .c.cc file is built into an exe into the target/<config/examples folder.
#
# you will need pkg-config and catch2 installed (for c++ tests)
# TODO: pick details up from .yaml file (will mean dropping make)
# caveats:
# will only work if only one of main.c main.cc liba.c liba.cc libso.c libso.cc exist for a module
# it will die horribly if theres not only one such.
# TODO: allow specifying .c/.cc files for each module and not auto-detect.
# TODO: rewrite in C++ and not makefile.. (or rust for portability/self-containment)

.SUFFIXES:
.SUFFIXES: .c .cc .a .o

# toolchains
# GCC
# CLANG
# ARM
TOOLCHAIN ?= native

ifeq ($(TOOLCHAIN),native)
OBJDUMP=objdump
CC=cc
CXX=c++
AR=ar
AS=as
LINK=$(CC)

else ifeq ($(TOOLCHAIN),gnu)

OBJDUMP=objdump
CC=gcc
CXX=g++
AR=ar
AS=as
LINK=$(CC)

else ifeq ($(TOOLCHAIN),clang)

OBJDUMP=objdump
CC=clang
CXX=clang++
AR=ar
AS=as
LINK=$(CC)

else ifeq ($(TOOLCHAIN),arm)

CC=arm-none-eabi-gcc
CXX=arm-none-eabi-g++
OBJDUMP=arm-none-eabi-objdump
CFLAGS+=-nostdlib
CXXFLAGS+=-nostdlib
LDLIBS+=-lnosys
LDFLAGS+=--specs=nosys.specs
#LDFLAGS+=--specs=nano.specs
AR=arm-none-eabi-ar
AS=arm-none-eabi-as
LINK=$(CC)

else

$(error TOOLCHAIN='$(TOOLCHAIN)' not supported)

endif


TARGET_TRIP:=$(CC) -dumpmachine

# Redefine link operation as not able to assign CC to $(CXX) in build rules.
# every attempt does not reassign to CC, just uses CC which messes up C++ linking
# but reassigning into another var works..
# ie. CC= $(CXX) doesn't, LINK=$(CXX) does..
LINK.o = $(LINK) $(LDFLAGS) $(TARGET_ARCH)

allowed_configs:=
allowed_configs+=debug
allowed_configs+=release
allowed_configs+=minsize
allowed_configs+=fast
ifeq ($(origin configs), undefined)
configs:=$(allowed_configs)
endif
ifneq ($(filter-out $(allowed_configs),$(configs)),)
$(error allowed configs: $(allowed_configs))
endif

#CFLAGS already defined
CFLAGS += -Werror -Wall -Wextra -Wpedantic -Wswitch-default -Wswitch-enum -Wuninitialized
CXXFLAGS += -Werror -Wall -Wextra -Wpedantic -Wswitch-default -Wswitch-enum -Wuninitialized -std=c++20 -fno-exceptions

# CFLAGS additionals for debug
# -Og is really bad.. lots of 'vars optimised out' occurrences.
debug_CFLAGS := -O0 -g
debug_CXXFLAGS := -O0 -g
debug_CPPFLAGS := -DDEBUG
#debug_LDFLAGS:=

release_CFLAGS := -O3 -g
release_CXXFLAGS := -O3 -g
release_CPPFLAGS := -DRELEASE
# nixos borken for lto: keep getting ar: lto needs a plugin..
# problem appears to be in binutils package not picking up compiler plugins..
#release_LDFLAGS:=
#release_LDFLAGS += --lto
#release_CFLAGS += --lto
#release_CXXFLAGS += --lto

minsize_CFLAGS := -Os -g
minsize_CXXFLAGS := -Os -g
minsize_CPPFLAGS := -DRELEASE
#minsize_LDFLAGS:=
#minsize_LDFLAGS+= --lto
#minsize_CFLAGS += --lto
#minsize_CXXFLAGS += --lto

fast_CFLAGS := -Ofast -g
fast_CXXFLAGS := -Ofast -g
fast_CPPFLAGS := -DRELEASE
#minsize_LDFLAGS:=
#minsize_LDFLAGS+= --lto
#minsize_CFLAGS += --lto
#minsize_CXXFLAGS += --lto

nel_DEPS:=

modls:=
modls+=nel

# 'global' additionals for compoent


nel_CXXFLAGS :=

# additionals for component per config
nel_debug_CFLAGS :=
nel_debug_CPPFLAGS :=
nel_debug_CXXFLAGS :=

nel_release_CFLAGS :=
nel_release_CPPFLAGS :=
nel_release_CXXFLAGS :=

nel_minsize_CFLAGS :=
nel_minsize_CPPFLAGS :=
nel_minsize_CXXFLAGS :=

nel_fast_CFLAGS :=
nel_fast_CPPFLAGS :=
nel_fast_CXXFLAGS :=


# build makefile to target/clean buildtree from src
# in makefile:
# $<modl>_DEPS:= lists deps of modl, relative to root of project.
# must be in same project..TODO: cross project building so can pull in projs from other repos.
# $modls:= <list of modules to build.

# folder tree laid out thus:
# examples/<nmame>.c
# examples/<nmame>.cc
# src/<modl>/
# src/<modl>/main.{c,cc} # for exe builds
# src/<modl>/liba.{c,cc} # for static lib builds
# src/<modl>/libso.{c,cc} # for shared lib builds
# src/<modl>/<other>.{c,cc}

# will compile examples/<file>.c into target/<config>/obj/examples/<file>.o
# will link target/<config>/obj/examples/<file>/<file>.o into target/<config>/examples/<file>
# i.e. every single .c and .cc file compiles to a single product.
# dependencies are from main project (is no deps between examples)

# will compile src/<modl>/<file>.{c,cc} into target/<config>/obj/<modl>/<file>.o
# will preprocess src/<modl>/<file>.{c,cc} into target/<config>/dep/<modl>/<file>.d for deps tracking of included files.

# for c,cc exes:
# will link target/<config>/obj/<modl>/<file>.o into target/<config>/obj/<modl>
# for c,cc statics:
# will link target/<config>/obj/<modl>/<file>.o into target/<config>/obj/lib<modl>.a
# for c,cc shared libs
# will link target/<config>/obj/<modl>/<file>.o into target/<config>/obj/lib<modl>.so

# TODO: error out if main.c/main.cc exist together
# TODO: error out if liba.c/liba.cc exist together
# TODO: error out if libso.c/libso.cc exist together
# TODO: error out if main.{c,cc} or liba.{c,cc} or libso.{c,cc} exist together

all: build

allsrc:=
allsrc+=$(foreach f,$(modls),$(shell find src/$(f) -name '*.c' -o -name '*.cc'))
allsrc+=$(shell find examples/$(f) -name '*.c' -o -name '*.cc')
allhdr:=
allhdr+=$(foreach f,$(modls),$(shell find src/$(f) -name '*.h' -o -name '*.hh'))
allhdr+=$(shell find examples/$(f) -name '*.h' -o -name '*.hh')

obj:=
clean:=
dep:=
rtests:=

COMPILE.sdep=$(AS) $(AFLAGS) $(TARGET_ARCH) -c -MM -MT $@
COMPILE.cdep=$(CC) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c -MM -MT $@
COMPILE.ccdep=$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c -MM -MT $@

# $1 = modl, $2 = config
define mk_modl
# TODO: this is eval'd every config, when want it evaled every module.
# but deps is being overhauled anyway, (very hard to impl in makefile)
$(1)_src_c:=$(patsubst src/$(1)/%,%,$(shell find src/$(1) ! -name 'test_*.c' -name '*.c'))
$(1)_src_cc:=$(patsubst src/$(1)/%,%,$(shell find src/$(1) ! -name 'test_*.cc' -name '*.cc'))
$(1)_src_s:=$(patsubst src/$(1)/%,%,$(shell find src/$(1) ! -name 'test_*.s' -name '*.s'))

# note: can end up with same .o for .c or .cc or .s
# possible fix is to use .c.o, .s.o and .cc.o for obj file extension.
$(1)_$(2)_o_c:=$$(patsubst %.c,target/$(2)/obj/$(1)/%.c.o,$$($(1)_src_c))
$(1)_$(2)_o_cc:=$$(patsubst %.cc,target/$(2)/obj/$(1)/%.cc.o,$$($(1)_src_cc))
$(1)_$(2)_o_s:=$$(patsubst %.s,target/$(2)/obj/$(1)/%.s.o,$$($(1)_src_s))

# note: can end up with same .d for .c or .cc or .s
# possible fix is to use .c.d, .s.d and .cc.d for dep file extension.
$(1)_$(2)_d_c:=$$(patsubst %.c,target/$(2)/dep/$(1)/%.c.d,$$($(1)_src_c))
$(1)_$(2)_d_cc:=$$(patsubst %.cc,target/$(2)/dep/$(1)/%.cc.d,$$($(1)_src_cc))
$(1)_$(2)_d_s:=$$(patsubst %.s,target/$(2)/dep/$(1)/%.s.d,$$($(1)_src_s))

target/$(2)/obj/$(1): | target/$(2)/obj ; $(RM) -r $$@ && mkdir $$@
target/$(2)/dep/$(1): | target/$(2)/dep ; $(RM) -r $$@ && mkdir $$@

$$(filter-out target/$(2)/dep/$(1),$$(patsubst %/,%,$$(sort $$(dir $$($(1)_$(2)_d_c))))): | target/$(2)/dep/$(1)
	$$(RM) -r $$@ && mkdir -p $$@

$$(foreach f,$$($(1)_$(2)_d_c),$$(eval $$(f): | $$(patsubst %/,%,$$(dir $$(f)))))

$$($(1)_$(2)_d_c): CPPFLAGS += $$($(1)_CPPFLAGS) $$($(2)_CPPFLAGS) $$($(1)_$(2)_CPPFLAGS) -Isrc
$$($(1)_$(2)_d_c): target/$(2)/dep/$(1)/%.c.d: src/$(1)/%.c
	$$(COMPILE.cdep) -MT $$(patsubst src/$(1)/%.c,target/$(2)/obj/$(1)/%.c.o,$$<) -MF $$@ $$<
dep+=$$($(1)_$(2)_d_c)
clean+=$$($(1)_$(2)_d_c)

$$(filter-out target/$(2)/obj/$(1),$$(patsubst %/,%,$$(sort $$(dir $$($(1)_$(2)_o_c))))): | target/$(2)/obj/$(1)
	$$(RM) -r $$@ && mkdir -p $$@

$$(foreach f,$$($(1)_$(2)_o_c),$$(eval $$(f): | $$(patsubst %/,%,$$(dir $$(f)))))

$$($(1)_$(2)_o_c): CPPFLAGS += $$($(1)_CPPFLAGS) $$($(2)_CPPFLAGS) $$($(1)_$(2)_CPPFLAGS) -Isrc
$$($(1)_$(2)_o_c): CFLAGS+=$$($(1)_CFLAGS) $$($(2)_CFLAGS) $$($(1)_$(2)_CFLAGS)
$$($(1)_$(2)_o_c): target/$(2)/obj/$(1)/%.c.o: src/$(1)/%.c
	$$(COMPILE.c) -o $$@  $$<
clean+=$$($(1)_$(2)_o_c)


$$(filter-out target/$(2)/dep/$(1),$$(patsubst %/,%,$$(sort $$(dir $$($(1)_$(2)_d_cc))))): | target/$(2)/dep/$(1)
	$(RM) -r $$@ && mkdir -p $$@

$$(foreach f,$$($(1)_$(2)_d_cc),$$(eval $$(f): | $$(patsubst %/,%,$$(dir $$(f)))))

$$($(1)_$(2)_d_cc): CPPFLAGS += $$($(1)_CPPFLAGS) $$($(2)_CPPFLAGS) $$($(1)_$(2)_CPPFLAGS) -Isrc
$$($(1)_$(2)_d_cc): target/$(2)/dep/$(1)/%.cc.d: src/$(1)/%.cc
	$$(COMPILE.ccdep) -MT $$(patsubst src/$(1)/%.cc,target/$(2)/obj/$(1)/%.cc.o,$$<) -MF $$@  $$<
dep+=$$($(1)_$(2)_d_cc)
clean+=$$($(1)_$(2)_d_cc)

$$(filter-out target/$(2)/obj/$(1),$$(patsubst %/,%,$$(sort $$(dir $$($(1)_$(2)_o_cc))))): | target/$(2)/obj/$(1)
	$(RM) -r $$@ && mkdir -p $$@

$$(foreach f,$$($(1)_$(2)_o_cc),$$(eval $$(f): | $$(patsubst %/,%,$$(dir $$(f)))))

$$($(1)_$(2)_o_cc): CPPFLAGS += $$($(1)_CPPFLAGS) $$($(2)_CPPFLAGS) $$($(1)_$(2)_CPPFLAGS) -Isrc
$$($(1)_$(2)_o_cc): CXXFLAGS+=$$($(1)_CXXFLAGS) $$($(2)_CXXFLAGS) $$($(1)_$(2)_CXXFLAGS)
$$($(1)_$(2)_o_cc): target/$(2)/obj/$(1)/%.cc.o: src/$(1)/%.cc
	$$(COMPILE.cc) -o $$@  $$<
clean+=$$($(1)_$(2)_o_cc)


$$(filter-out target/$(2)/dep/$(1),$$(patsubst %/,%,$$(sort $$(dir $$($(1)_$(2)_d_s))))): | target/$(2)/dep/$(1)
	$(RM) -r $$@ && mkdir -p $$@

$$(foreach f,$$($(1)_$(2)_d_s),$$(eval $$(f): | $$(patsubst %/,%,$$(dir $$(f)))))

dep+=$$($(1)_$(2)_d_s)
clean+=$$($(1)_$(2)_d_s)


$$(filter-out target/$(2)/obj/$(1),$$(patsubst %/,%,$$(sort $$(dir $$($(1)_$(2)_o_s))))): | target/$(2)/obj/$(1)
	$(RM) -r && mkdir -p $$@

$$(foreach f,$$($(1)_$(2)_o_s),$$(eval $$(f): | $$(patsubst %/,%,$$(dir $$(f)))))

$$($(1)_$(2)_o_s): target/$(2)/obj/$(1)/%.s.o: src/$(1)/%.s
	$$(COMPILE.s) -o $$@  $$<
clean+=$$($(1)_$(2)_o_s)

endef

# $1=modl
define mk_modtype
$(1)_typ1:=$(patsubst src/$(1)/%,%,$(wildcard src/$(1)/main.c src/$(1)/main.cc src/$(1)/main.s src/$(1)/liba.c src/$(1)/liba.cc src/$(1)/liba.s))
endef

# $1 = modl, $2=config
define mk_main.c
$(1)_$(2)_targ:=target/$(2)/$(1)
$$($(1)_$(2)_targ): $$($(1)_$(2)_o_c); $$(LINK.o) $$^ $$(LOADLIBES) $$(LDLIBS) -o $$@
$$($(1)_$(2)_targ): $(foreach m,$(modls),$(filter %.a %.so,$($(m)_$(2)_targ)))
$$($(1)_$(2)_targ): | target/$(2)
build: $$($(1)_$(2)_targ)
clean+=$$($(1)_$(2)_targ)
endef

define mk_main.cc
$(1)_$(2)_targ:=target/$(2)/$(1)
$$($(1)_$(2)_targ): LINK=$$(CXX)
$$($(1)_$(2)_targ): $$($(1)_$(2)_o_cc); $$(LINK.o) $$^ $$(LOADLIBES) $$(LDLIBS) -o $$@
$$($(1)_$(2)_targ): $(foreach m,$(modls),$(filter %.a %.so,$($(m)_$(2)_targ)))
$$($(1)_$(2)_targ): | target/$(2)
build: $$($(1)_$(2)_targ)
clean+=$$($(1)_$(2)_targ)
endef

define mk_liba.c
$(1)_$(2)_targ:=target/$(2)/lib$(1).a
$$($(1)_$(2)_targ): $$($(1)_$(2)_o_c); $$(AR) rcs $$@ $$^
$$($(1)_$(2)_targ): $(foreach m,$(modls),$(filter %.a %.so,$($(m)_$(2)_targ)))
$$($(1)_$(2)_targ): | target/$(2)
build: $$($(1)_$(2)_targ)
clean+=$$($(1)_$(2)_targ)
endef

define mk_liba.cc
$(1)_$(2)_targ:=target/$(2)/lib$(1).a
$$($(1)_$(2)_targ): $$($(1)_$(2)_o_cc); $$(AR) rcs $$@ $$^
$$($(1)_$(2)_targ): $(foreach m,$(modls),$(filter %.a %.so,$($(m)_$(2)_targ)))
$$($(1)_$(2)_targ): | target/$(2)
build: $$($(1)_$(2)_targ)
clean+=$$($(1)_$(2)_targ)
endef

target: ; $(RM) -r $@ && mkdir $@

#target/$(2):| target ; mkdir $$@

$(foreach c,$(configs),target/$(c)): | target ; $(RM) -r $@ && mkdir $@

$(foreach c,$(configs),$(eval target/$(c)/obj: | target/$(c); $$(RM) -r $$@ && mkdir $$@))
$(foreach c,$(configs),$(eval target/$(c)/dep: | target/$(c); $$(RM) -r $$@ && mkdir $$@))


# $(foreach m,$(modls),$(foreach c,$(configs),$(eval $(call mk_modl,$(m),$(c)))))
# $(eval $(call mk_modtype,nel,debug))
# $(eval $(call mk_modl,nel,debug))
# $(eval $(call mk_$(nel_typ1),nel,debug))
$(foreach m,$(modls),$(eval $(call mk_modtype,$(m))))
$(foreach m,$(modls),$(foreach c,$(configs),$(eval $(call mk_modl,$(m),$(c)))))
$(foreach m,$(modls),$(foreach c,$(configs),$(eval $(call mk_$($(m)_typ1),$(m),$(c)))))


# $1 = example name $2 = config
define mk_example
# TODO: this is eval'd every config, when want it evaled every module.
# but deps is being overhauled anyway, (very hard to impl in makefile)
$(1)_exsrc_c:=$(shell find examples/ -name '$(1).c')
$(1)_exsrc_cc:=$(shell find examples/ -name '$(1).cc')
$(1)_exsrc_s:=$(shell find examples/ -name '$(1).s')

$(1)_$(2)_exo_c:=$$(patsubst %.c,target/$(2)/obj/%.c.o,$$($(1)_exsrc_c))
$(1)_$(2)_exo_cc:=$$(patsubst %.cc,target/$(2)/obj/%.cc.o,$$($(1)_exsrc_cc))
$(1)_$(2)_exo_s:=$$(patsubst %.s,target/$(2)/obj/%.s.o,$$($(1)_exsrc_s))

$(1)_$(2)_exd_c:=$$(patsubst %.c,target/$(2)/dep/%.c.d,$$($(1)_exsrc_c))
$(1)_$(2)_exd_cc:=$$(patsubst %.cc,target/$(2)/dep/%.cc.d,$$($(1)_exsrc_cc))
$(1)_$(2)_exd_s:=$$(patsubst %.s,target/$(2)/dep/%.s.d,$$($(1)_exsrc_s))


$$($(1)_$(2)_exd_c): | target/$(2)/dep/examples
$$($(1)_$(2)_exd_c): CPPFLAGS += $$($(1)_CPPFLAGS) $$($(2)_CPPFLAGS) $$($(1)_$(2)_CPPFLAGS) -Isrc
$$($(1)_$(2)_exd_c): target/$(2)/dep/examples/%.c.d: examples/%.c
	$$(COMPILE.cdep) -MT $$(patsubst %.c,target/$(2)/obj/%.c.o,$$<) -MF $$@ $$<
clean += $$($(1)_$(2)_exd_c)
dep += $$($(1)_$(2)_exd_c)

$$($(1)_$(2)_exo_c): | target/$(2)/obj/examples
$$($(1)_$(2)_exo_c): CPPFLAGS += $$($(1)_CPPFLAGS) $$($(2)_CPPFLAGS) $$($(1)_$(2)_CPPFLAGS) -Isrc
$$($(1)_$(2)_exo_c): CFLAGS += $$($(1)_CFLAGS) $$($(2)_CFLAGS) $$($(1)_$(2)_CFLAGS)
$$($(1)_$(2)_exo_c): target/$(2)/obj/examples/%.c.o: examples/%.c
	$$(COMPILE.c)  -o $$@ $$<
clean += $$($(1)_$(2)_exo_c)


$$($(1)_$(2)_exd_cc): | target/$(2)/dep/examples
$$($(1)_$(2)_exd_cc): CPPFLAGS += $$($(1)_CPPFLAGS) $$($(2)_CPPFLAGS) $$($(1)_$(2)_CPPFLAGS) -Isrc
$$($(1)_$(2)_exd_cc): target/$(2)/dep/examples/%.cc.d: examples/%.cc
	$$(COMPILE.ccdep) -MT $$(patsubst %.cc,target/$(2)/obj/%.cc.o,$$<) -MF $$@ $$<

clean += $$($(1)_$(2)_exd_cc)
dep += $$($(1)_$(2)_exd_cc)

$$($(1)_$(2)_exo_cc): | target/$(2)/obj/examples
$$($(1)_$(2)_exo_cc): CPPFLAGS += $$($(1)_CPPFLAGS) $$($(2)_CPPFLAGS) $$($(1)_$(2)_CPPFLAGS) -Isrc
$$($(1)_$(2)_exo_cc): CXXFLAGS += $$($(1)_CXXFLAGS) $$($(2)_CXXFLAGS) $$($(1)_$(2)_CXXFLAGS)
$$($(1)_$(2)_exo_cc): target/$(2)/obj/examples/%.cc.o: examples/%.cc
	$$(COMPILE.cc)  -o $$@ $$<
clean += $$($(1)_$(2)_exo_cc)#$$($(1)_$(2)_testo_cc): | $$(dir $$($(1)_$(2)_testo_cc))

target/$(2)/examples/$(1): | target/$(2)/examples
target/$(2)/examples/$(1): $(foreach m,$(modls),$(filter %.a %.so,$($(m)_$(2)_targ)))
target/$(2)/examples/$(1): LDFLAGS += $$($(1)_LDFLAGS) $$($(2)_LDFLAGS) $$($(1)_$(2)_LDFLAGS)
target/$(2)/examples/$(1): LDLIBS += $$($(1)_LDLIBS) $$($(2)_LDLIBS) $$($(1)_$(2)_LDLIBS)
#target/$(2)/examples/$(1): $$($(1)_$(2)_exo_c)
#	$$(LINK.o) $$^ $$(LOADLIBES) $$(LDLIBS) -o $$@
target/$(2)/examples/$(1): LINK = $(CXX)
target/$(2)/examples/$(1): $$($(1)_$(2)_exo_cc)
	$$(LINK.o) $$^ $$(LOADLIBES) $$(LDLIBS) -o $$@
examples: target/$(2)/examples/$(1)
clean += target/$(2)/examples/$(1)

target/$(2)/examples/$(1).s: | target/$(2)/examples
target/$(2)/examples/$(1).s: target/$(2)/examples/$(1)
	$(OBJDUMP) -Sr --demangle $$< > $$@
examples: target/$(2)/examples/$(1).s
clean += target/$(2)/examples/$(1).s


clean += $$($(1)_$(2)_exo_s)
clean += $$($(1)_$(2)_exd_s)
dep += $$($(1)_$(2)_exd_s)

endef

$(foreach c,$(configs),$(eval target/$(c)/obj/examples: | target/$(c)/obj; $$(RM) -r $$@ && mkdir $$@))
$(foreach c,$(configs),$(eval target/$(c)/dep/examples: | target/$(c)/dep; $$(RM) -r $$@ && mkdir $$@))
$(foreach c,$(configs),$(eval target/$(c)/examples: | target/$(c); $$(RM) -r $$@ && mkdir $$@))

exs:=
exs+=$(patsubst examples/%.c,%,$(wildcard examples/*.c))
exs+=$(patsubst examples/%.cc,%,$(wildcard examples/*.cc))
# examples:=largestruct1_array
$(foreach e,$(exs),$(foreach c,$(configs),$(eval $(call mk_example,$(e),$(c)))))


define mk_modl_tests
# TODO: this is eval'd every config, when want it evaled every module.
# but deps is being overhauled anyway, (very hard to impl in makefile)
$(1)_testsrc_c:=$(patsubst src/$(1)/%,%,$(shell find src/$(1) -name '*.c'))
$(1)_testsrc_cc:=$(patsubst src/$(1)/%,%,$(shell find src/$(1) -name '*.cc'))
$(1)_testsrc_s:=$(patsubst src/$(1)/%,%,$(shell find src/$(1) -name '*.s'))

# note: can end up with same .o for .c or .cc or .s
# possible fix is to use .c.o, .s.o and .cc.o for obj file extension.
$(1)_$(2)_testo_c:=$$(patsubst %.c,target/$(2)/obj/tests/$(1)/%.c.o,$$($(1)_testsrc_c))
$(1)_$(2)_testo_cc:=$$(patsubst %.cc,target/$(2)/obj/tests/$(1)/%.cc.o,$$($(1)_testsrc_cc))
$(1)_$(2)_testo_s:=$$(patsubst %.s,target/$(2)/obj/tests/$(1)/%.s.o,$$($(1)_testsrc_s))

# note: can end up with same .d for .c or .cc or .s
# possible fix is to use .c.d, .s.d and .cc.d for dep file extension.
$(1)_$(2)_testd_c:=$$(patsubst %.c,target/$(2)/dep/tests/$(1)/%.c.d,$$($(1)_testsrc_c))
$(1)_$(2)_testd_cc:=$$(patsubst %.cc,target/$(2)/dep/tests/$(1)/%.cc.d,$$($(1)_testsrc_cc))
$(1)_$(2)_testd_s:=$$(patsubst %.s,target/$(2)/dep/tests/$(1)/%.s.d,$$($(1)_testsrc_s))

target/$(2)/obj/tests/$(1): | target/$(2)/obj/tests ; $$(RM) -r $$@ && mkdir $$@

target/$(2)/dep/tests/$(1): | target/$(2)/dep/tests ; $$(RM) -r $$@ && mkdir $$@


$$(filter-out target/$(2)/dep/tests/$(1),$$(patsubst %/,%,$$(sort $$(dir $$($(1)_$(2)_testd_c))))): | target/$(2)/dep/tests/$(1)
	$$(RM) -r $$@ && mkdir -p $$@

$$(foreach f,$$($(1)_$(2)_testd_c),$$(eval $$(f): | $$(patsubst %/,%,$$(dir $$(f)))))

$$($(1)_$(2)_testd_c): CPPFLAGS += $$($(1)_CPPFLAGS) $$($(2)_CPPFLAGS) $$($(1)_$(2)_CPPFLAGS)
$$($(1)_$(2)_testd_c): CPPFLAGS += $(shell pkg-config --cflags catch2) -Isrc -DTEST
$$($(1)_$(2)_testd_c): target/$(2)/dep/tests/$(1)/%.c.d: src/$(1)/%.c
	$$(COMPILE.cdep) -MT $$(patsubst src/$(1)/%.c,target/$(2)/obj/tests/$(1)/%.c.o,$$<) -MF $$@ $$<
dep+=$$($(1)_$(2)_testd_c)
clean+=$$($(1)_$(2)_testd_c)


$$(filter-out target/$(2)/obj/tests/$(1),$$(patsubst %/,%,$$(sort $$(dir $$($(1)_$(2)_testo_c))))): | target/$(2)/obj/tests/$(1)
	$$(RM) -r $$@ && mkdir -p $$@

$$(foreach f,$$($(1)_$(2)_testo_c),$$(eval $$(f): | $$(patsubst %/,%,$$(dir $$(f)))))

$$($(1)_$(2)_testo_c): CPPFLAGS += $$($(1)_CPPFLAGS) $$($(2)_CPPFLAGS) $$($(1)_$(2)_CPPFLAGS)
$$($(1)_$(2)_testo_c): CPPFLAGS += $(shell pkg-config --cflags check) -Isrc -DTEST
$$($(1)_$(2)_testo_c): CFLAGS+=$$($(1)_CFLAGS) $$($(2)_CFLAGS) $$($(1)_$(2)_CFLAGS)
$$($(1)_$(2)_testo_c): CPPFLAGS += -O0 -g
$$($(1)_$(2)_testo_c): target/$(2)/obj/tests/$(1)/%.c.o: src/$(1)/%.c
	$$(COMPILE.c) -o $$@  $$<
clean+=$$($(1)_$(2)_testo_c)


$$(filter-out target/$(2)/dep/tests/$(1),$$(patsubst %/,%,$$(sort $$(dir $$($(1)_$(2)_testd_cc))))): | target/$(2)/dep/tests/$(1)
	$$(RM) -r $$@ && mkdir -p $$@

$$(foreach f,$$($(1)_$(2)_testd_cc),$$(eval $$(f): | $$(patsubst %/,%,$$(dir $$(f)))))

$$($(1)_$(2)_testd_cc): CPPFLAGS += $$($(1)_CPPFLAGS) $$($(2)_CPPFLAGS) $$($(1)_$(2)_CPPFLAGS)
$$($(1)_$(2)_testd_cc): CPPFLAGS += $(shell pkg-config --cflags catch2) -Isrc -DTEST
$$($(1)_$(2)_testd_cc): target/$(2)/dep/tests/$(1)/%.cc.d: src/$(1)/%.cc
	$$(COMPILE.ccdep) -MT $$(patsubst src/$(1)/%.cc,target/$(2)/obj/tests/$(1)/%.cc.o,$$<) -MF $$@ $$<
dep+=$$($(1)_$(2)_testd_cc)
clean+=$$($(1)_$(2)_testd_cc)


$$(filter-out target/$(2)/obj/tests/$(1),$$(patsubst %/,%,$$(sort $$(dir $$($(1)_$(2)_testo_cc))))): | target/$(2)/obj/tests/$(1)
	$$(RM) -r $$@ && mkdir -p $$@

$$(foreach f,$$($(1)_$(2)_testo_cc),$$(eval $$(f): | $$(patsubst %/,%,$$(dir $$(f)))))

$$($(1)_$(2)_testo_cc): CPPFLAGS += $$($(1)_CPPFLAGS) $$($(2)_CPPFLAGS) $$($(1)_$(2)_CPPFLAGS)
$$($(1)_$(2)_testo_cc): CPPFLAGS += $(shell pkg-config --cflags catch2) -Isrc -DTEST
$$($(1)_$(2)_testo_cc): CXXFLAGS += $$($(1)_CXXFLAGS) $$($(2)_CXXFLAGS) $$($(1)_$(2)_CXXFLAGS)
$$($(1)_$(2)_testo_cc): CXXFLAGS += -O0 -g
$$($(1)_$(2)_testo_cc): CXXFLAGS += -fexceptions
$$($(1)_$(2)_testo_cc): target/$(2)/obj/tests/$(1)/%.cc.o: src/$(1)/%.cc
	$$(COMPILE.cc) -o $$@  $$<
clean+=$$($(1)_$(2)_testo_cc)


$$(filter-out target/$(2)/dep/tests/$(1),$$(patsubst %/,%,$$(sort $$(dir $$($(1)_$(2)_testd_s))))): | target/$(2)/dep/tests/$(1)
	$$(RM) -r $$@ && mkdir -p $$@
$$(foreach f,$$($(1)_$(2)_testd_s),$$(eval $$(f): | $$(patsubst %/,%,$$(dir $$(f)))))
dep+=$$($(1)_$(2)_testd_s)
clean+=$$($(1)_$(2)_testd_s)


$$(filter-out target/$(2)/obj/tests/$(1),$$(patsubst %/,%,$$(sort $$(dir $$($(1)_$(2)_testo_s))))): | target/$(2)/obj/tests/$(1)
	$$(RM) -r $$@ && mkdir -p $$@

$$(foreach f,$$($(1)_$(2)_testo_s),$$(eval $$(f): | $$(patsubst %/,%,$$(dir $$(f)))))

$$($(1)_$(2)_testo_s): target/$(2)/obj/tests/$(1)/%.s.o: src/$(1)/%.s
	$$(COMPILE.s) -o $$@  $$<
clean+=$$($(1)_$(2)_testo_s)


target/$(2)/tests/test_$(1): | target/$(2)/tests
#target/$(2)/tests/test_$(1): $(foreach m,$(modls),$(filter %.a %.so,$($(m)_$(2)_targ)))
target/$(2)/tests/test_$(1): LDFLAGS += $($(2)_LDFLAGS)
target/$(2)/tests/test_$(1): LDLIBS += $($(2)_LDLIBS)
#target/$(2)/tests/test_$(1): LDLIBS += $(shell pkg-config --libs check)
#target/$(2)/tests/test_$(1): $$($(1)_$(2)_testo_c); $$(LINK.o) $$^ $$(LOADLIBES) $$(LDLIBS) -o $$@
target/$(2)/tests/test_$(1): LDLIBS += $(shell pkg-config --libs catch2)
target/$(2)/tests/test_$(1): LDFLAGS += -fexceptions
target/$(2)/tests/test_$(1): LINK = $$(CXX)
target/$(2)/tests/test_$(1): $$($(1)_$(2)_testo_cc); $$(LINK.o) $$^ $$(LOADLIBES) $$(LDLIBS) -o $$@
#target/$(2)/tests/test_$(1): $$($(1)_$(2)_testo_s); $$(LINK.o) $$^ $$(LOADLIBES) $$(LDLIBS) -o $$@

.PHONY: run_target/$(2)/tests/test_$(1)
run_target/$(2)/tests/test_$(1): target/$(2)/tests/test_$(1)
	target/$(2)/tests/test_$(1) -b

rtests += target/$(2)/tests/test_$(1)
clean += target/$(2)/tests/test_$(1)

endef

$(foreach c,$(configs),$(eval target/$(c)/tests:| target/$(c); $$(RM) -r $$@ && mkdir $$@))
$(foreach c,$(configs),$(eval target/$(c)/obj/tests: |target/$(c)/obj; $$(RM) -r $$@ && mkdir $$@))
$(foreach c,$(configs),$(eval target/$(c)/dep/tests: |target/$(c)/dep; $$(RM) -r $$@ && mkdir $$@))

$(foreach m,$(modls),$(foreach c,$(configs),$(eval $(call mk_modl_tests,$(m),$(c)))))

#==================================================================================================

.PHONY: build
build:

.PHONY: examples
examples:

.PHONY: clean
clean:
	$(RM) $(clean)
	find . -name '*.orig' -delete

.PHONY: test tests
tests: $(addprefix run_,$(rtests))
test: tests

distclean:
	$(RM) -r target
	find . -name '*.orig' -delete

# clang-format does not format class/struct defs how I like them
# with access spec 1 indent, and members 1 more indent after that
# currently puts access spec on same level as members..
# but then I want a full reformatter (space between functions, members etc.. managed as well).
.PHONY: format
format:
	astyle --project -I -n  $(allsrc) $(allhdr)
	env -u NIX_CFLAGS_COMPILE clang-format -style=file -i $(allsrc) $(allhdr)

ifeq ($(MAKECMDGOALS),distclean)
else ifeq ($(MAKECMDGOALS),clean)
else ifeq ($(MAKECMDGOALS),format)
else
-include $(dep)
endif
