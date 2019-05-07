

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

ifdef GCC_VERSION
  GCC_OPT_VERSION = -V $(GCC_VERSION)
endif

CC       = g++ $(GCC_OPT_VERSION)
LD       = g++ $(GCC_OPT_VERSION)
AR       = ar -rcvs
RANLIB   = ranlib
INCL     = $(LOCAL_INCLUDE) $(MODULE_INCL)  
DEFS     = $(MODULE_DEFS)
OPT      = -Wall -Wno-non-virtual-dtor 
CCFLAGS  = $(OPT) -std=c++11 -fno-rtti $(INCL) $(DEFS) 
CFLAGS   = $(OPT) $(INCL) $(DEFS)
LIBS     = $(MODULE_EXTRA_LIBS) $(MODULE_LIBS) 
LDFLAGS  = -L. $(MODULE_LIBDIR)

# GCC4 visibility 
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

# Components should only export only one symbol "__CCI_Module"  
#ifeq ($(IS_COMPONENT),1)
#LDFLAGS += -Xlinker -exported_symbol -Xlinker __CCI_Module
#endif

DEFS += -DDM_THROW_BAD_ALLOC_SIG

ifdef MODULE_EXE
#=====================
# Options for executable
#=====================

LDFLAGS += -Xlinker -rpath-link -Xlinker $(BINDIR)

else # MODULE_EXE

ifdef DM_DEBUG
# Optimize : May be useful on some platforms
#OPT     += -ffunction-sections
#LDFLAGS += -Wl,--gc-sections
endif

ifdef MODULE_SHARED 
#=====================
# Options for shared dll
#=====================

 CCFLAGS += -fPIC
 
 # GCC4 visibility
 CCFLAGS += -Wno-strict-aliasing
  
 LDFLAGS += -shared

 MODULE_EXE:=1

else 
#========================
# Options for static libs
#========================

  # Check for PIC mandatory on AMD64 bits arch
ifeq ($(shell uname -m),x86_64)
  CCFLAGS += -fPIC
endif

endif

endif

OBJECTS  = $(patsubst %.cpp,$(OBJDIR)/%.o,$(MODULE_FILES))
OBJECTS += $(patsubst %.c,$(OBJDIR)/%.o,$(CMODULE_FILES))

build: $(MODULE_EXPORT)

$(OBJDIR)/%.o : %.cpp
	$(CC) $(CCFLAGS) -c -o $@ $<


$(OBJDIR)/%.o : %.c
	gcc $(CFLAGS) -c -o $@ $<

$(MODULE_EXPORT): $(OBJECTS)
ifdef MODULE_EXE
	$(LD) $(LDFLAGS) -o $(MODULE_EXPORT) $(OBJECTS) $(MODULE_STATIC_LIBS) $(LIBS)  
# ifdef DM_DEBUG
#	strip $(MODULE_EXPORT)
# endif
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
	@$(CC) --version

