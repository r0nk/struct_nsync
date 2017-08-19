#Copyright 2017 Rexy712 of Rexy & Co
SOURCE_DIRS:=src
OBJDIR:=obj
DEPDIR:=$(OBJDIR)/dep
INCLUDE_DIRS:=include
EXT:=cpp
MAIN_EXECUTABLE:=nsync

CC:=g++
CFLAGS:=-g -std=c++14 -Wall -pedantic -Wextra
release: CFLAGS+=-O2
LDFLAGS=
LDLIBS:=
STRIP:=strip

ifeq ($(OS),Windows_NT)
	mkdir=mkdir $(subst /,\,$(1)) > NUL 2>&1
	rm=del /F $(1) > NUL 2>&1
	rmdir=rd /s /q $(1) > NUL 2>&1
	move=move /y $(subst /,\,$(1)) $(subst /,\,$(2)) > NUL 2>&1
	MAIN_EXECUTABLE:=$(MAIN_EXECUTABLE).exe
	LDLIBS:=
else
	mkdir=mkdir -p $(1)
	rm=rm -f $(1)
	rmdir=rm -rf $(1)
	move=mv $(1) $(2)
endif

INTERNAL_CFLAGS=-c $(foreach dir,$(INCLUDE_DIRS),-I"$(dir)") -MMD -MP -MF"$(DEPDIR)/$*.d"
SOURCES:=$(foreach source,$(SOURCE_DIRS),$(foreach ext,$(EXT),$(wildcard $(source)/*.$(ext))))
OBJECTS:=$(addprefix $(OBJDIR)/,$(subst \,.,$(subst /,.,$(addsuffix .o,$(SOURCES)))))

all: $(MAIN_EXECUTABLE)

$(MAIN_EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $^ -o "$(basename $@)" $(LDLIBS)

.PHONY: release
release: $(OBJECTS)
	$(CC) $(LDFLAGS) $^ -o "$(basename $(MAIN_EXECUTABLE))" $(LDLIBS)
	$(STRIP) --strip-all "$(MAIN_EXECUTABLE)"

define GENERATE_OBJECTS

$$(OBJDIR)/$(subst \,.,$(subst /,.,$(1))).%.o: $(1)/%
	$$(CC) $$(CFLAGS) $$(INTERNAL_CFLAGS) "$$<" -o "$$@"

endef

$(foreach dir,$(SOURCE_DIRS),$(eval $(call GENERATE_OBJECTS,$(dir))))
$(OBJECTS): | $(OBJDIR) $(DEPDIR)

$(OBJDIR):
	$(call mkdir,"$@")
$(DEPDIR):
	$(call mkdir,"$@")

.PHONY: clean
clean:
	$(call rmdir,"$(DEPDIR)")
	$(call rmdir,"$(OBJDIR)")
	$(call rm,"$(MAIN_EXECUTABLE)")

-include $(wildcard $(DEPDIR)/*.d)

