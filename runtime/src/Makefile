#
# Makefile for daim dll
#

DEPTH= ../..
VPATH = ./

include $(DEPTH)/config/config.mk

LIBRARY_NAME = xpcci
MODULE_EXT   = dll
MODULE_DEFS  = DM_BUILD CCI_TRANSLATE_MODULE_ENTRY_POINT 

MODULE_FILES=xpcci.cpp \
             $(NULL)

MODULE_LIBS = -lcci_s

EXPORTS=xpcci.h

REQUIRES=cci

DM_UTILITIES:=1

include $(topsrcdir)/config/rules.mk

