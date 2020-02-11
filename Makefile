.SUFFIXES:
.SUFFIXES: .c .cc .a .o


builddir:=build/debug
buildobjdir:=$(builddir)/obj

all: build

configs:=
configs+=debug
configs+=release


$(foreach c,$(configs),build/$(c) build/$(c)/obj):
	mkdir -p $@


#CFLAGS already defined
CFLAGS += -Werror -Wall -Wextra
CXXFLAGS += -fno-exceptions

# CFLAGS additionals for debug
debug_CFLAGS := -O0 -g
debug_CXXFLAGS := -O0 -g
debug_CPPFLAGS := -DDEBUG
debug_LDLIBS:=

release_CFLAGS := -O3
release_CXXFLAGS := -O3
release_CPPFLAGS := -DRELEASE
release_LDLIBS:= --lto

# $(config)_CFLAGS += -O3
# $(config)_CXXFLAGS += -O3
# $(config)_CPPFLAGS += -Ifoo
# 'global' additionals for compoent
#utilsc_CFLAGS := -O2

nel_CXXFLAGS :=

# additionals for component per config
#utilsc_debug_CFLAGS := -O2
#utilsc_debug_CPPFLAGS := -Ibar
#utilsc_release_CFLAGS := -O3
#utilsc_release_CPPFLAGS := -Ibar

nel_debug_CFLAGS :=
nel_debug_CPPFLAGS :=
nel_debug_CXXFLAGS :=

nel_release_CFLAGS :=
nel_release_CPPFLAGS :=
nel_release_CXXFLAGS :=


nel_DEPS:=


# - include build/$(config)/deps/$(modl)/*.d
modls:=
#modls+=utilsc
modls+=nel

src:=$(foreach f,$(modls),$(wildcard $(f)/src/*.c $(f)/src/*.cc))
obj:=
clean:=
dep:=

# TODO: depends of each module when linking. build/$(2)/lib$(1).a: $(foreach d,$($(1)_dep),$($(d)_targ))
# TODO: depends of each module when compiling = CPPFLAGS += $(foreach d,$($(1)_dep),-I $(d)/src)
# $1 = modl, $2 = config
define mk_modl
clean+=$(patsubst $(1)/src/%.c,build/$(2)/obj/$(1)/%.o,$(filter $(1)/src/%.c,$(src)))
clean+=$(patsubst $(1)/src/%.cc,build/$(2)/obj/$(1)/%.o,$(filter $(1)/src/%.cc,$(src)))
clean+=$(patsubst $(1)/src/%.s,build/$(2)/obj/$(1)/%.o,$(filter $(1)/src/%.s,$(src)))

build/$(2)/obj/$(1): ; mkdir -p $$@
build/$(2)/dep/$(1): ; mkdir -p $$@

$(if $(filter $(1)/src/%.c,$(src)),$(patsubst $(1)/src/%.c,build/$(2)/obj/$(1)/%.o,$(filter $(1)/src/%.c,$(src))): | build/$(2)/obj/$(1))
$(if $(filter $(1)/src/%.cc,$(src)),$(patsubst $(1)/src/%.cc,build/$(2)/obj/$(1)/%.o,$(filter $(1)/src/%.cc,$(src))): | build/$(2)/obj/$(1))
$(if $(filter $(1)/src/%.s,$(src)),$(patsubst $(1)/src/%.s,build/$(2)/obj/$(1)/%.o,$(filter $(1)/src/%.s,$(src))): | build/$(2)/obj/$(1))

$(if $(filter $(1)/src/%.c,$(src)),$(patsubst $(1)/src/%.c,build/$(2)/dep/$(1)/%.d,$(filter $(1)/src/%.c,$(src))): | build/$(2)/dep/$(1))
$(if $(filter $(1)/src/%.cc,$(src)),$(patsubst $(1)/src/%.cc,build/$(2)/dep/$(1)/%.d,$(filter $(1)/src/%.cc,$(src))): | build/$(2)/dep/$(1))
$(if $(filter $(1)/src/%.s,$(src)),$(patsubst $(1)/src/%.s,build/$(2)/dep/$(1)/%.d,$(filter $(1)/src/%.s,$(src))): | build/$(2)/dep/$(1))

$(if $(filter $(1)/src/%.c,$(src)),$(patsubst $(1)/src/%.c,build/$(2)/obj/$(1)/%.o,$(filter $(1)/src/%.c,$(src))): CPPFLAGS:=$$(CPPFLAGS) $$($(1)_CPPFLAGS) $$($(2)_CPPFLAGS) $$($(1)_$(2)_CPPFLAGS))
$(if $(filter $(1)/src/%.c,$(src)),$(patsubst $(1)/src/%.c,build/$(2)/obj/$(1)/%.o,$(filter $(1)/src/%.c,$(src))): CFLAGS:=$$(CFLAGS) $$($(1)_CFLAGS) $$($(2)_CFLAGS) $$($(1)_$(2)_CFLAGS))

$(if $(filter $(1)/src/%.cc,$(src)),$(patsubst $(1)/src/%.cc,build/$(2)/obj/$(1)/%.o,$(filter $(1)/src/%.cc,$(src))): CPPFLAGS:=$$(CPPFLAGS) $$($(1)_CPPFLAGS) $$($(2)_CPPFLAGS) $$($(1)_$(2)_CPPFLAGS))
$(if $(filter $(1)/src/%.cc,$(src)),$(patsubst $(1)/src/%.cc,build/$(2)/obj/$(1)/%.o,$(filter $(1)/src/%.cc,$(src))): CXXFLAGS:=$$(CXXFLAGS) $$($(1)_CXXFLAGS) $$($(2)_CXXFLAGS) $$($(1)_$(2)_CXXFLAGS))

$(if $(filter $(1)/src/%.c,$(src)),$(patsubst $(1)/src/%.c,build/$(2)/obj/$(1)/%.o,$(filter $(1)/src/%.c,$(src))): build/$(2)/obj/$(1)/%.o: $(1)/src/%.c ;$$(COMPILE.c) -o $$@  $$<)
$(if $(filter $(1)/src/%.cc,$(src)),$(patsubst $(1)/src/%.cc,build/$(2)/obj/$(1)/%.o,$(filter $(1)/src/%.cc,$(src))): build/$(2)/obj/$(1)/%.o: $(1)/src/%.cc ;$$(COMPILE.cc) -o $$@  $$<)
$(if $(filter $(1)/src/%.s,$(src)),$(patsubst $(1)/src/%.s,build/$(2)/obj/$(1)/%.o,$(filter $(1)/src/%.s,$(src))): build/$(2)/obj/$(1)/%.o: $(1)/src/%.s ;$$(COMPILE.s) -o $$@  $$<)

$(if $(filter $(1)/src/%.c,$(src)),$(patsubst $(1)/src/%.c,build/$(2)/dep/$(1)/%.d,$(filter $(1)/src/%.c,$(src))): build/$(2)/dep/$(1)/%.d: $(1)/src/%.c ;$$(COMPILE.c) -MM -MT $$(patsubst $(1)/src/%.c,build/$(2)/obj/$(1)/%.o,$$<) -o $$@ $$<)
$(if $(filter $(1)/src/%.cc,$(src)),$(patsubst $(1)/src/%.cc,build/$(2)/dep/$(1)/%.d,$(filter $(1)/src/%.cc,$(src))): build/$(2)/dep/$(1)/%.d: $(1)/src/%.cc ;$$(COMPILE.cc) -MM -MT $$(patsubst $(1)/src/%.cc,build/$(2)/obj/$(1)/%.o,$$<) -o $$@  $$<)
$(if $(filter $(1)/src/%.s,$(src)),$(patsubst $(1)/src/%.s,build/$(2)/dep/$(1)/%.d,$(filter $(1)/src/%.s,$(src))): build/$(2)/dep/$(1)/%.d: $(1)/src/%.s ;$$(COMPILE.s) -MM -MT $$(patsubst $(1)/src/%.s,build/$(2)/obj/$(1)/%.o,$$<) -o $$@  $$<)

$(if $(filter $(1)/src/%.c,$(src)),dep += $(patsubst $(1)/src/%.c,build/$(2)/dep/$(1)/%.d,$(filter $(1)/src/%.c,$(src))))
$(if $(filter $(1)/src/%.cc,$(src)),dep += $(patsubst $(1)/src/%.cc,build/$(2)/dep/$(1)/%.d,$(filter $(1)/src/%.cc,$(src))))
$(if $(filter $(1)/src/%.s,$(src)),dep +=$(patsubst $(1)/src/%.s,build/$(2)/dep/$(1)/%.d,$(filter $(1)/src/%.s,$(src))))

$(if $(filter $(1)/src/liba.c,$(src)),build/$(2)/lib$(1).a: | build/$(2))
$(if $(filter $(1)/src/liba.c,$(src)),build/$(2)/lib$(1).a: $(patsubst $(1)/src/%.c,build/$(2)/obj/$(1)/%.o,$(filter $(1)/src/%.c,$(src))); $$(AR) rcs $$@ $$^)
$(if $(filter $(1)/src/liba.c,$(src)),build: build/$(2)/lib$(1).a)
$(if $(filter $(1)/src/liba.c,$(src)),clean += build/$(2)/lib$(1).a)
$(if $(filter $(1)/src/liba.c,$(src)),$(1)_targ := build/$(2)/lib$(1).a)

$(if $(filter $(1)/src/liba.cc,$(src)),build/$(2)/lib$(1).a: | build/$(2))
$(if $(filter $(1)/src/liba.cc,$(src)),build/$(2)/lib$(1).a: $(patsubst $(1)/src/%.cc,build/$(2)/obj/$(1)/%.o,$(filter $(1)/src/%.cc,$(src))); $$(AR) rcs $$@ $$^)
$(if $(filter $(1)/src/liba.cc,$(src)),build: build/$(2)/lib$(1).a)
$(if $(filter $(1)/src/liba.cc,$(src)),clean += build/$(2)/lib$(1).a)
$(if $(filter $(1)/src/liba.cc,$(src)),$(1)_targ := build/$(2)/lib$(1).a)

$(if $(filter $(1)/src/main.c,$(src)),build/$(2)/$(1): | build/$(2))
$(if $(filter $(1)/src/main.c,$(src)),build/$(2)/$(1): $(patsubst $(1)/src/%.c,build/$(2)/obj/$(1)/%.o,$(filter $(1)/src/%.c,$(src))); $$(LINK.o) -o $$@ $$^)
$(if $(filter $(1)/src/main.c,$(src)),build: build/$(2)/$(1))
$(if $(filter $(1)/src/main.c,$(src)),clean += build/$(2)/$(1))
$(if $(filter $(1)/src/main.c,$(src)),$(1)_targ := build/$(2)/$(1))

$(if $(filter $(1)/src/main.cc,$(src)),build/$(2)/$(1): | build/$(2))
$(if $(filter $(1)/src/main.cc,$(src)),build/$(2)/$(1): $(patsubst $(1)/src/%.cc,build/$(2)/obj/$(1)/%.o,$(filter $(1)/src/%.cc,$(src))); $$(LINK.o) -o $$@ $$^)
$(if $(filter $(1)/src/main.cc,$(src)),build: build/$(2)/$(1))
$(if $(filter $(1)/src/main.cc,$(src)),clean += build/$(2)/$(1))
$(if $(filter $(1)/src/main.cc,$(src)),$(1)_targ := build/$(2)/$(1))

endef


$(foreach f,$(modls),$(eval $(call mk_modl,$(f),debug)))
$(foreach f,$(modls),$(eval $(call mk_modl,$(f),release)))
#$(eval $(call mk_modl,utilsc))
#$(eval $(call mk_modl,nel))
#fooc:= $(call mk_modl,utilsc)
#foocc:= $(call mk_modl,nel)


examples:=
examples+=$(patsubst examples/%.c,%,$(wildcard examples/*.c))
examples+=$(patsubst examples/%.cc,%,$(wildcard examples/*.cc))

examples_src:=$(wildcard examples/*.c examples/*.cc)

# $1 = example name $2 = config
define mk_example
clean += $(patsubst examples/$(1).c,build/$(2)/obj/examples/$(1).o,$(filter examples/$(1).c,$(examples_src)))
clean += $(patsubst examples/$(1).cc,build/$(2)/obj/examples/$(1).o,$(filter examples/$(1).cc,$(examples_src)))

$(if $(filter examples/$(1).c,$(examples_src)),$(patsubst examples/$(1).c,build/$(2)/obj/examples/$(1).o,$(filter examples/$(1).c,$(examples_src))): CPPFLAGS += $($(2)_CPPFLAGS) $(foreach m,$(modls),-I$(m)/src))
$(if $(filter examples/$(1).cc,$(examples_src)),$(patsubst examples/$(1).cc,build/$(2)/obj/examples/$(1).o,$(filter examples/$(1).cc,$(examples_src))): CPPFLAGS +=  $($(2)_CPPFLAGS)  $(foreach m,$(modls),-I$(m)/src))
$(if $(filter examples/$(1).c,$(examples_src)),$(patsubst examples/$(1).c,build/$(2)/obj/examples/$(1).o,$(filter examples/$(1).c,$(examples_src))): CFLAGS += $($(2)_CFLAGS))
$(if $(filter examples/$(1).cc,$(examples_src)),$(patsubst examples/$(1).cc,build/$(2)/obj/examples/$(1).o,$(filter examples/$(1).cc,$(examples_src))): CFLAGS +=  $($(2)_CFLAGS))
$(if $(filter examples/$(1).c,$(examples_src)),$(patsubst examples/$(1).c,build/$(2)/obj/examples/$(1).o,$(filter examples/$(1).c,$(examples_src))): CXXFLAGS += $($(2)_CXXFLAGS))
$(if $(filter examples/$(1).cc,$(examples_src)),$(patsubst examples/$(1).cc,build/$(2)/obj/examples/$(1).o,$(filter examples/$(1).cc,$(examples_src))): CXXFLAGS +=  $($(2)_CXXFLAGS))

$(if $(filter examples/$(1).c,$(examples_src)),$(patsubst examples/$(1).c,build/$(2)/dep/examples/$(1).d,$(filter examples/$(1).c,$(examples_src))): CPPFLAGS +=  $($(2)_CPPFLAGS) $(foreach m,$(modls),-I$(m)/src))
$(if $(filter examples/$(1).cc,$(examples_src)),$(patsubst examples/$(1).cc,build/$(2)/dep/examples/$(1).d,$(filter examples/$(1).cc,$(examples_src))): CPPFLAGS +=  $($(2)_CPPFLAGS) $(foreach m,$(modls),-I$(m)/src))

$(if $(filter examples/$(1).c,$(examples_src)),$(patsubst examples/$(1).c,build/$(2)/dep/examples/$(1).d,$(filter examples/$(1).c,$(examples_src))): build/$(2)/dep/examples/%.d: examples/%.c | build/$(2)/dep/examples; $$(COMPILE.c)  -MM -MT $$(patsubst examples/$(1).c,build/$(2)/obj/examples/$(1).o,$$<) -o $$@ $$<)
$(if $(filter examples/$(1).cc,$(examples_src)),$(patsubst examples/$(1).cc,build/$(2)/dep/examples/$(1).d,$(filter examples/$(1).cc,$(examples_src))): build/$(2)/dep/examples/%.d: examples/%.cc | build/$(2)/dep/examples; $$(COMPILE.cc) -MM -MT $$(patsubst examples/$(1).cc,build/$(2)/obj/examples/$(1).o,$$<) -o $$@ $$<)

$(if $(filter $(1)/src/%.c,$(src)),dep += $(patsubst $(1)/src/%.c,build/$(2)/dep/$(1)/%.d,$(filter $(1)/src/%.c,$(src))))
$(if $(filter $(1)/src/%.cc,$(src)),dep += $(patsubst $(1)/src/%.cc,build/$(2)/dep/$(1)/%.d,$(filter $(1)/src/%.cc,$(src))))
$(if $(filter $(1)/src/%.s,$(src)),dep +=$(patsubst $(1)/src/%.s,build/$(2)/dep/$(1)/%.d,$(filter $(1)/src/%.s,$(src))))

$(if $(filter examples/$(1).c,$(examples_src)),dep += build/$(2)/dep/examples/$(1).d)
$(if $(filter examples/$(1).cc,$(examples_src)),dep += build/$(2)/dep/examples/$(1).d)

$(if $(filter examples/$(1).c,$(examples_src)),$(patsubst examples/$(1).c,build/$(2)/obj/examples/$(1).o,$(filter examples/$(1).c,$(examples_src))): build/$(2)/obj/examples/%.o: examples/%.c | build/$(2)/obj/examples; $$(COMPILE.c) -o $$@ $$<)
$(if $(filter examples/$(1).cc,$(examples_src)),$(patsubst examples/$(1).cc,build/$(2)/obj/examples/$(1).o,$(filter examples/$(1).cc,$(examples_src))): build/$(2)/obj/examples/%.o: examples/%.cc | build/$(2)/obj/examples; $$(COMPILE.cc) -o $$@ $$<)

$(if $(filter examples/$(1).c,$(examples_src)), build/$(2)/examples/$(1): LDLIBS += $($(2)_LDLIBS))
$(if $(filter examples/$(1).cc,$(examples_src)), build/$(2)/examples/$(1): LDLIBS += $($(2)_LDLIBS))

$(if $(filter examples/$(1).c,$(examples_src)), build/$(2)/examples/$(1): $(patsubst examples/$(1).c,build/$(2)/obj/examples/$(1).o,$(filter examples/$(1).c,$(examples_src))) | build/$(2)/examples; $$(LINK.o) $$^ $$(LOADLIBES) $$(LDLIBS) -o $$@)
$(if $(filter examples/$(1).cc,$(examples_src)), build/$(2)/examples/$(1): $(patsubst examples/$(1).cc,build/$(2)/obj/examples/$(1).o,$(filter examples/$(1).cc,$(examples_src))) | build/$(2)/examples; $$(LINK.o) $$^ -lstdc++ $$(LOADLIBES) $$(LDLIBS) -o $$@)

$(if $(filter examples/$(1).c,$(examples_src)),build/$(2)/examples/$(1): $(foreach m,$(modls),$($(m)_targ)))
$(if $(filter examples/$(1).c,$(examples_src)),build: build/$(2)/examples/$(1))
$(if $(filter examples/$(1).c,$(examples_src)),clean += build/$(2)/examples/$(1))

$(if $(filter examples/$(1).cc,$(examples_src)),build/$(2)/examples/$(1):  $(foreach m,$(modls),$($(m)_targ)))
$(if $(filter examples/$(1).cc,$(examples_src)),build: build/$(2)/examples/$(1))
$(if $(filter examples/$(1).cc,$(examples_src)),clean += build/$(2)/examples/$(1))

$(if $(filter examples/$(1).c,$(examples_src)),build/$(2)/examples/$(1).s: build/$(2)/examples/$(1); objdump -S $$< > $$@)
$(if $(filter examples/$(1).c,$(examples_src)),build: build/$(2)/examples/$(1).s)
$(if $(filter examples/$(1).c,$(examples_src)),clean += build/$(2)/examples/$(1).s)

$(if $(filter examples/$(1).cc,$(examples_src)),build/$(2)/examples/$(1).s: build/$(2)/examples/$(1); objdump -S $$< > $$@)
$(if $(filter examples/$(1).cc,$(examples_src)),build: build/$(2)/examples/$(1).s)
$(if $(filter examples/$(1).cc,$(examples_src)),clean += build/$(2)/examples/$(1).s)

endef


$(foreach c,$(configs),build/$(c)/obj/examples build/$(c)/dep/examples build/$(c)/examples):
	mkdir -p $@


$(foreach f,$(examples),$(eval $(call mk_example,$(f),debug)))
$(foreach f,$(examples),$(eval $(call mk_example,$(f),release)))
#$(eval $(call mk_example,nocopy1,debug))
#foo:= $(call mk_example,nocopy1,debug)


.PHONY: build
build:

.PHONY: clean
clean:
	$(RM) $(clean)

distclean: clean
distclean:
	$(RM) -r build

-include $(dep)
