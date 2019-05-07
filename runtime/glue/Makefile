#

DEPTH= ../..
VPATH= ../src/

include $(DEPTH)/config/config.mk

LIBRARY_NAME=xpcciglue
MODULE_EXT=lib
MODULE_DEFS=XPCCI_GLUE
MODULE_INCL=$(DEPTH)/utilities/include

REQUIRES=cci

MODULE_FILES=xpcciglue.cpp \
             $(NULL)

EXPORTS = xpcciglue.h

include $(topsrcdir)/config/rules.mk
