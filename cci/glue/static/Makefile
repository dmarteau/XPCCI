#
# Makefile for cci glue
#

DEPTH= ../../..
VPATH = ./:../

include $(DEPTH)/config/config.mk

#
# CCI support library
#
# This library can be linked with
# modules that link directly with daim library 
#

MODULE_NAME  = cci
LIBRARY_NAME = cciglue_s
MODULE_EXT   = lib

MODULE_FILES=cciComponentManagerUtils.cpp \
             cciInterfaceRequestorUtils.cpp \
             cciGenericFactoryUtils.cpp \
             cciSupportsImpl.cpp \
             cciCOMPtr.cpp \
             cciEnumeratorUtils.cpp \
             cciWeakReference.cpp \
             $(NULL)

include $(topsrcdir)/config/rules.mk
