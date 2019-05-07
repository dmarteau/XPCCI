#========================================
# Makefile for DAIM core components installation
#========================================

DEPTH=..

include $(DEPTH)/config/config.mk

ifndef INSTALL_DIR
$(error INSTALL_DIR not defined !)
endif

DIST_DIRS = bin modules

ifeq($(EXPORT_SDK),1)
DIST_DIRS += idl include lib
endif

export::
	@for d in $(DIST_DIRS); do \
		cp -R $(BUILD_DIR)/$$d $(INSTALL_DIR)/; \
	done

