#
# Makefile for cci glue
#

DEPTH= ../..
VPATH = ./

include $(DEPTH)/config/config.mk

MODULE_NAME = cci

XPIDL_MODULE = xpcci_base

IDLSRC = ccirootidl.idl \
         cciISupports.idl \
         cciIModule.idl \
         cciIModuleLoader.idl \
         cciIClassInfo.idl \
         cciIFactory.idl \
         cciIComponentManager.idl \
         cciIComponentRegistrar.idl \
         cciIServiceManager.idl \
         cciIInterfaceRequestor.idl \
         cciISimpleEnumerator.idl \
         cciIStringEnumerator.idl \
         cciICategoryManager.idl \
         cciIMemory.idl \
         cciIStringIterator.idl \
         cciIWeakReference.idl \
         cciIProgrammingLanguage.idl \
         $(NULL)

include $(topsrcdir)/config/rules.mk
