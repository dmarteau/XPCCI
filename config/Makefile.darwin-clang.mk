

# Resolve defs includes path and lib files
MODULE_DEFS  :=$(patsubst %,-D%,$(MODULE_DEFS)) 
MODULE_INCL  :=$(patsubst %,-I%,$(MODULE_INCL))
MODULE_LIBDIR:=$(patsubst %,-L%,$(MODULE_LIBDIR))

ifndef NO_LOCAL_INCLUDE
LOCAL_INCLUDE = -I.
endif

MODULE_LIBDIR += -L$(BINDIR)

#========================
# Common compile options
#========================

CC       = clang++ 
LD       = clang++
AR       = ar -rcvs
RANLIB   = ranlib
INCL     = $(LOCAL_INCLUDE) $(MODULE_INCL)  
DEFS     = $(MODULE_DEFS)
OPT      = -Wall -Wno-non-virtual-dtor 
CCFLAGS  = $(OPT) -std=c++11 -fno-rtti $(INCL) $(DEFS)
CFLAGS   = $(OPT) $(INCL) $(DEFS)
LIBS     = $(MODULE_EXTRA_LIBS) $(MODULE_LIBS) 
LDFLAGS  = -L. $(MODULE_LIBDIR)

# Visibility
CCFLAGS += -fvisibility-inlines-hidden -fvisibility=hidden

#=====================
# Debug options
#=====================

ifdef DM_DEBUG
 OPT  += -g
 DEFS += -D_DEBUG
else
 OPT += -O2
endif

# We need to set operator new as not exported because 
# 1. Sometime only one of operator new/delete is exported and
# that cause a big mess in client code
# 2. Library may be used in client code where custom definition of operator new/delete
# has to be avoided

# linker tend to export too many symbols, components should only export
# one symbol "__CCI_Module"  

#LDFLAGS  += -Wl,-rpath,./

ifdef MODULE_EXE
#=====================
# Options for executable
#=====================

else # MODULE_EXE

DEFS += -DDM_THROW_BAD_ALLOC_SIG

ifdef MODULE_SHARED

#=====================
# Options for shared dll
#=====================

CCFLAGS += -fPIC 

# GCC4 visibility
CCFLAGS += -Wno-strict-aliasing

ifeq ($(IS_COMPONENT),1)
LDFLAGS += -Wl,-exported_symbol,__CCI_Module
LDFLAGS += -bundle

else
# Don't export new/delete custom symbols 
LDFLAGS += \
 -unexported_symbols_list $(topsrcdir)/config/unexported.exp \
 -dynamiclib \
 -compatibility_version $(DM_RUNTIME_VER_MAJOR).$(DM_RUNTIME_VER_MINOR) \
 -current_version $(DM_RUNTIME_VER_MAJOR).$(DM_RUNTIME_VER_MINOR).$(DM_RUNTIME_VER_REVISION) \
 $(NULL)
 
ifeq ($(OSX_FRAMEWORK),1)
LDFLAGS += -install_name $(OSX_VERSION_FRAMEWORK_PREFIX)/$(MODULE_LIBRARY_NAME)
else
LDFLAGS += -install_name @executable_path/$(MODULE_LIBRARY_NAME)
endif
endif

MODULE_EXE:=1

endif # MODULE_SHARED
endif # MODULE_EXE

OBJECTS  = $(patsubst %.cpp,$(OBJDIR)/%.o,$(MODULE_FILES))
OBJECTS += $(patsubst %.c,$(OBJDIR)/%.o,$(CMODULE_FILES))

build: $(MODULE_EXPORT)

$(OBJDIR)/%.o : %.cpp
	$(CC) $(CCFLAGS) -c -o $@ $<


$(OBJDIR)/%.o : %.c
	clang $(CFLAGS) -c -o $@ $<

$(MODULE_EXPORT): $(OBJECTS)
ifdef MODULE_EXE
	$(LD) $(LDFLAGS) -o $(MODULE_EXPORT) $(OBJECTS) $(MODULE_STATIC_LIBS) $(LIBS) 
ifeq ($(IS_COMPONENT),1)
endif
ifndef DM_DEBUG
	strip -x $(MODULE_EXPORT)
endif
else
	$(AR) $@ $(OBJECTS)
	$(RANLIB) $@
endif

cleanup:
	@rm -fr $(OBJDIR)
	@rm -f $(MODULE_EXPORT)


config:
	@echo Compiler configuration:
	@echo PATH=$(PATH)
	@echo LIB="$(LIB)"
	@echo INCLUDE="$(INCLUDE)"
	@echo LD=$(LD)
	@$(CC) --version

