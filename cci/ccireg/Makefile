#
# Makefile for cci glue
#

DEPTH= ../..
VPATH = ./

.PHONY: cciregister

include $(DEPTH)/config/config.mk

MODULE_NAME  = cci
LIBRARY_NAME = ccireg
MODULE_EXT   = exe
MODULE_DEFS  = DM_BUILD CCI_TRANSLATE_MODULE_ENTRY_POINT 


MODULE_FILES = ccireg.cpp \
               $(NULL)

MODULE_LIBS = -lcci_s

DM_UTILITIES:=1

include $(topsrcdir)/config/rules.mk

libs::
	cp cciregister $(BINDIR)/
	chmod ugo+x  $(BINDIR)/cciregister

clean::
	@rm -f $(BINDIR)/cciregister
