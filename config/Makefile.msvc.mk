#
# Makefile for visual C++ 8
# Use gnu make
#
.SUFFIXES: .res .rc .c .cpp .obj

# Resolve defs includes path and lib files

MODULE_INCL := $(patsubst %,-I"%",$(call normalizepath,$(MODULE_INCL)))
MODULE_DEFS := $(patsubst %,-D "%",$(MODULE_DEFS)) 
MODULE_LIBS := $(patsubst -l%,%.lib,$(MODULE_LIBS))

MODULE_LIBDIR := $(patsubst %,-LIBPATH:"%",$(call normalizepath,$(MODULE_LIBDIR)))

_OBJDIR := $(call normalizepath,$(OBJDIR))
_MODULE_EXPORT := $(call normalizepath,$(MODULE_EXPORT))

ifdef MODULE_IMPORT
_MODULE_IMPORT := $(call normalizepath,$(MODULE_IMPORT))
endif

ifndef NO_LOCAL_INCLUDE
LOCAL_INCLUDE = -I "./"
endif

#compile rules
CC  = cl.exe
LD  = link.exe
RSC = rc.exe
#=====================
# Debug options
#=====================


ifdef DM_DEBUG
RUNTIME_OPT  = -MDd
MODULE_DEFS += -D "_DEBUG"
OPTIMIZE_OPT = -ZI -Od -RTC1
LD_DEBUG_OPT = -incremental:yes -debug \
               -pdb:"$(_OBJDIR)/$(LIBRARY_NAME).pdb" 
RSCFLAGS     = -l 0x40c -i "windows" -d "_DEBUG"

else
MODULE_DEFS += -D "NDEBUG"
RUNTIME_OPT  = -MD
OPTIMIZE_OPT = -O2
LD_DEBUG_OPT = -incremental:no
RSCFLAGS     = -l 0x40c -i "windows" -d "NDEBUG"
endif

DEFS = -D "WIN32" -D "_MBCS" -D "NOMINMAX" -D "_CRT_SECURE_NO_DEPRECATE" -D "_SCL_SECURE_NO_WARNINGS" $(MODULE_DEFS)
OPT  = -nologo $(OPTIMIZE_OPT) -W3 -EHsc  -Fo"$(_OBJDIR)\\" -Fd"$(_OBJDIR)\\" -FD -c

# Using STLPORT, 
# it must be the first entry in include search path

ifdef STLPORT_PATH
MSVC_INCLUDES  = -I "$(STLPORT_PATH)"
DEFS += -D "USE_STLPORT"
endif

ifdef MODULE_EXE
#=====================
# Building executable
#=====================

INCL    = $(LOCAL_INCLUDE) $(MSVC_INCLUDES) $(MODULE_INCL)
LIBS    = $(MODULE_LIBS) $(MODULE_EXTRA_LIBS) kernel32.lib user32.lib advapi32.lib
LDFLAGS = -nologo \
          -machine:I386 \
          -out:"$(_MODULE_EXPORT)" \
          $(LD_DEBUG_OPT) \
          $(MODULE_LIBDIR) \
		  -WX

ifeq ($(EXEC_UI_MODE),CONSOLE)
  MODULE_DEFS += -D "_CONSOLE"
  LDFLAGS     += -subsystem:console
endif

OPT += $(RUNTIME_OPT)

# We can do that because we are not linking incrementally
VC8_EMBED_MANIFEST=@mt.exe -nologo -manifest "$(_MODULE_EXPORT).manifest" -outputresource:"$(_MODULE_EXPORT);1" ;  rm -f "$(_MODULE_EXPORT).manifest"

else # MODULE_EXE

DEFS += -D "_WINDOWS"


ifdef MODULE_SHARED

#=====================
# Building shared dll
#=====================

INCL     = $(LOCAL_INCLUDE) $(MSVC_INCLUDES) $(MODULE_INCL)
DEFS    += -D "_USRDLL"
OPT     += $(RUNTIME_OPT)
LIBS     = $(MODULE_LIBS) $(MODULE_EXTRA_LIBS) kernel32.lib user32.lib advapi32.lib 
LDFLAGS  = -nologo \
           -dll \
           -machine:I386 \
           -out:"$(_MODULE_EXPORT)" \
           $(LD_DEBUG_OPT) \
           $(MODULE_LIBDIR) \
		   -WX

ifdef MODULE_IMPORT
LDFLAGS += -implib:"$(_MODULE_IMPORT)" 
else
LDFLAGS += -implib:"$(_OBJDIR)/$(LIBRARY_NAME).lib"
endif

ifdef DEF_FILE
LDFLAGS += -def:"$(DEF_FILE)"
endif

# We can do that because we are not linking incrementally
VC8_EMBED_MANIFEST=@mt.exe -nologo -manifest "$(_MODULE_EXPORT).manifest" -outputresource:"$(_MODULE_EXPORT);2" ; rm -f "$(_MODULE_EXPORT).manifest"

else

#=====================
# Building static lib
#=====================

INCL    = $(LOCAL_INCLUDE) $(MSVC_INCLUDES) $(MODULE_INCL)
LD     += -lib
DEFS   += -D "_LIB"
LDFLAGS = -nologo -out:"$(_MODULE_EXPORT)"

ifeq ($(DM_INTERNAL),1)
OPT  += $(RUNTIME_OPT)
else
OPT     += -Zl
LDFLAGS += -NODEFAULTLIB
endif

endif # MODULE_SHARED
endif # MODULE_EXE


CCFLAGS = $(OPT) $(INCL) $(DEFS)

OBJECTS   = $(patsubst %.cpp,$(_OBJDIR)/%.obj,$(MODULE_FILES))
OBJECTS  += $(patsubst %.rc,$(_OBJDIR)/%.res,$(MODULE_RES))
OBJECTS  += $(patsubst %.c,$(_OBJDIR)/%.obj,$(CMODULE_FILES))

build: $(_MODULE_EXPORT)

$(_OBJDIR)/%.obj : %.cpp
	$(CC) $(CCFLAGS) $<

$(_OBJDIR)/%.obj : %.c
	$(CC) $(CCFLAGS) $<

$(_OBJDIR)/%.res : %.rc
	$(RSC) $(RSCFLAGS) \/fo "$@" $< 

# Need to enclose args between quotes on windows
$(_MODULE_EXPORT): $(OBJECTS)
	$(LD) $(LDFLAGS) $(patsubst %,"%",$(OBJECTS)) $(LIBS)
	$(VC8_EMBED_MANIFEST)

cleanup:
	@rm -fr "$(OBJDIR)"
	@rm -f  "$(MODULE_EXPORT)"
	@rm -f  "$(MODULE_IMPORT)"
	@rm -f  $(patsubst %.lib,"%.exp",$(MODULE_IMPORT))
	@rm -f  $(patsubst %.$(MODULE_EXT),"%.ilk",$(MODULE_EXPORT))

config:
	@echo Compiler configuration:
	@echo PATH=$(PATH)
	@echo LIB="$(LIB)"
	@echo INCLUDE="$(INCLUDE)"
	@$(CC)


