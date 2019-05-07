#=========================================================
# Makefile for libraries and modules
# The following should be defined in the module Makefile:
#
# MODULE_NAME    The name of the current module to compile
# MODULE_EXT     Output extension
# MODULE_INCL    Additional includes path
# MODULE_LIBS    Additional lib files
# MODULE_DEFS    Additional defines
# MODULE_FILES   List of source files to compile and link
#
# BUILD_DIR  is defined in config/config.mk
#=========================================================

# Defines for lib version


MODULE_DEFS += __DM_LIB_VERSION__=$(DM_RUNTIME_VER_MAJOR).$(DM_RUNTIME_VER_MINOR)
MODULE_DEFS += __DM_LIB_REVISION__=$(DM_RUNTIME_VER_REVISION)

MODULE_DEFS += DM_RUNTIME_VER_MAJOR=$(DM_RUNTIME_VER_MAJOR)
MODULE_DEFS += DM_RUNTIME_VER_MINOR=$(DM_RUNTIME_VER_MINOR)
MODULE_DEFS += DM_RUNTIME_VER_REVISION=$(DM_RUNTIME_VER_REVISION)


# Use a specific package
ifdef MODULE_PACKAGE
  MOD_PATH      += apps/$(MODULE_PACKAGE)/
  MODULE_LIBDIR += $(BUILD_DIR)/lib
endif

BINDIR=$(BUILD_DIR)/$(MOD_PATH)bin
CCIDIR=$(BUILD_DIR)/$(MOD_PATH)bin/cci
LIBDIR=$(BUILD_DIR)/$(MOD_PATH)lib
EXEDIR=$(BUILD_DIR)/$(MOD_PATH)bin
INCDIR=$(BUILD_DIR)/$(MOD_PATH)include
IDLDIR=$(BUILD_DIR)/$(MOD_PATH)idl

MODULE_INCL += $(INCDIR)

ifdef LIBRARY_NAME
OBJDIR=$(BUILD_DIR)/.parts/$(LIBRARY_NAME)
endif

MODULE_LIBDIR += $(LIBDIR)

# Add the 'DEBUG' macros
ifdef DM_DEBUG
 MODULE_DEFS += DEBUG
endif

# Set EXPORTS directory

ifndef MODULE_EXPORT_NAME
MODULE_EXPORT_NAME=$(MODULE_NAME)
endif

# If MODULE_EXPORT_NAME is defined but not set
# we export in the main include directory
ifeq ($(MODULE_EXPORT_NAME),)
  EXPORT_DIR=$(INCDIR)
  IDL_EXPORT_DIR=$(IDLDIR)
else
  EXPORT_DIR=$(INCDIR)/$(MODULE_EXPORT_NAME)
  IDL_EXPORT_DIR=$(IDLDIR)/$(MODULE_EXPORT_NAME)
endif


ifeq ($(MODULE_EXT),exe)
#=======================================
# Build an executable using xpcci library
#=======================================
MODULE_EXPORT = $(EXEDIR)/$(LIBRARY_NAME)$(EXESFX)
MODULE_EXE:=1

#=======================================
# Define path to the xpcci system relatives to bin directory
# This is because test programs must be executed with bin
# as working directory (otherwise dll names and modules names 
# won't be resolved).
#=======================================

ifdef MODULE_ID
MODULE_DEFS += "_DM_APPID=\"$(MODULE_ID)\""
endif

else
#=======================================
# Build a cci component using core 
# libraries
#=======================================

ifeq ($(IS_COMPONENT),1)
MODULE_LIBRARY_NAME = $(LIBRARY_NAME).cci
MODULE_EXPORT = $(CCIDIR)/$(MODULE_NAME)/$(MODULE_LIBRARY_NAME)
MODULE_FILES += cciComponentGlue.cpp
VPATH += :$(topsrcdir)/cci/glue
MODULE_SHARED:=1
REQUIRES += cci
MODULE_DEFS  += XPCCI_GLUE 
MODULE_LIBS  += -lcciglue
endif

#ifeq ($(IS_STATIC_COMPONENT),1)
#  MODULE_EXT:=lib
#  MODULE_DEFS += CCI_TRANSLATE_MODULE_ENTRY_POINT
#  REQUIRES += cci
#endif

#=======================================
# Build a dynamic library
#=======================================

ifeq ($(MODULE_EXT),dll)
MODULE_LIBRARY_NAME = $(DLLPREFIX)$(LIBRARY_NAME)$(DLLSFX)
MODULE_EXPORT =$(BINDIR)/$(MODULE_LIBRARY_NAME)
MODULE_IMPORT =$(LIBDIR)/$(LIBPREFIX)$(LIBRARY_NAME)$(LIBSFX)
MODULE_SHARED:=1
endif

#=======================================
# Build a static library
#=======================================

ifeq ($(MODULE_EXT),lib)
MODULE_LIBRARY_NAME = $(LIBPREFIX)$(LIBRARY_NAME)$(LIBSFX)
MODULE_EXPORT = $(LIBDIR)/$(MODULE_LIBRARY_NAME)
endif

endif # MODULE_EXE

#=============================================
# Predefined setups for some general 
# configurations
#=============================================

# We link against xpcci dll
ifdef XPCCI_LIB
MODULE_DEFS  += XPCCI_LIB 
MODULE_LIBS  += -lxpcci
endif

ifdef DM_UTILITIES
MODULE_DEFS += DM_UTILITIES
MODULE_LIBS += -lxpcci_utilities
MODULE_INCL += $(topsrcdir)/utilities/include
endif

#==============================================
# Build process and sub-processes
#==============================================

.PHONY: clean cleanup

# Always depends on platform
MODULE_INCL += $(INCDIR)/platform

# Add dependencies to include directories
ifdef REQUIRES
  MODULE_INCL += $(patsubst %,$(INCDIR)/%,$(REQUIRES))
#  $(error REQUIRES=$(REQUIRES) MODULE_INCL=$(MODULE_INCL))
endif

# Add module name to the include search
MODULE_INCL += $(INCDIR)/$(MODULE_NAME)

# Build/Clean only if we have something to build
ifdef MODULE_EXPORT
  BUILD=build
  CLEAN=cleanup
endif

# Add rules for making subdirs
ifdef DIRS
  CLEAN_SUBDIRS=clean_subdirs
endif


# Add rules for building idl headers
ifdef IDLSRC
BUILD_IDL=build_idl
CLEAN_IDL=clean_idl
include $(topsrcdir)/config/idl.mk
endif


.PHONY: $(DIRS) $(BUILD_IDL)

module: export build_idl0 libs

libs:: $(DIRS) $(BUILD)

build_idl0: $(BUILD_IDL)
ifdef DIRS
	@for d in $(DIRS); do \
		$(MAKE) -C $$d build_idl0; \
	done
endif	

include $(MAKE_TARGET)

clean:: $(CLEAN_SUBDIRS) clean_deps $(CLEAN) $(CLEAN_IDL)

export::
ifdef MODULE_PACKAGE
	@echo "Building module for package $(MODULE_PACKAGE)"
endif
ifdef OBJDIR
	mkdir -p $(OBJDIR)
endif
	@mkdir -p $(BINDIR)
	@mkdir -p $(LIBDIR)
	@mkdir -p $(CCIDIR)
	@mkdir -p $(INCDIR)
	@mkdir -p $(IDLDIR)
ifdef IS_COMPONENT
	@mkdir -p $(CCIDIR)/$(MODULE_NAME)
endif
ifdef EXPORTS
	@mkdir -p $(EXPORT_DIR)
	@for f in $(EXPORTS); do \
		cp $(COPY_FLAGS) "$$f" "$(EXPORT_DIR)"; \
	done
endif
ifdef IDLSRC
	@mkdir -p $(EXPORT_DIR)
	@mkdir -p $(IDL_EXPORT_DIR)
	@for f in $(IDLSRC); do \
		cp $(COPY_FLAGS) "$$f" "$(IDL_EXPORT_DIR)"; \
	done
endif
ifdef DIRS
	@for d in $(DIRS); do \
		$(MAKE) -C $$d export; \
	done
endif

clean_deps:
ifdef EXPORTS
	@for f in $(EXPORTS); do \
	  rm -f "$(EXPORT_DIR)/$$f"; \
	done
endif
ifdef IDLSRC
	@for f in $(IDLSRC); do \
	  rm -f "$(IDL_EXPORT_DIR)/$$f"; \
	done
endif

ifdef DIRS
$(DIRS):
	@$(MAKE) -C $@ libs
endif

clean_subdirs:
	@for d in $(DIRS); do \
		$(MAKE) -C $$d clean; \
	done

-include $(MY_RULES)

echo-variable-%:
	@echo "$($*)"

echo-dirs:
	@echo $(DIRS)

echo-module:
	@echo $(MODULE_EXPORT_NAME)

echo-requires:
	@echo $(REQUIRES)
