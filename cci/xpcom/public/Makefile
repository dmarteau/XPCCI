#
# Makefile for xpcom supports
#

DEPTH= ../../..

include $(DEPTH)/config/config.mk

MODULE_NAME = xpcom

XPCOM_DIST=$(BUILD_DIR)/xpcom

XPCOM_EXPORTS=cciIXPCOMSupports.idl \
              cciXPCOMTypes.idl     \
              $(NULL)

libs::
	@mkdir -p $(XPCOM_DIST)
	@rm -f $(XPCOM_DIST)/*.idl $(XPCOM_DIST)/*.h $(XPCOM_DIST)/filelist
	cp ccidaim.idl $(XPCOM_DIST)/
	for f in $(XPCOM_EXPORTS); do \
	  cp "$$f" $(XPCOM_DIST)/; \
	done
	cd $(XPCOM_DIST); ls *.idl > filelist


clean:: 
	@rm -f $(XPCOM_DIST)/*.idl $(XPCOM_DIST)/*.h $(XPCOM_DIST)/filelist
	
include $(topsrcdir)/config/rules.mk

