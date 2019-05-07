#==============================================
# Configuration Makefile
#==============================================

include $(DEPTH)/config/config.opt

#
# Personal makefile customizations go in these optional make include files.
#
MY_CONFIG := $(DEPTH)/myconfig.mk
MY_RULES  := $(DEPTH)/myrules.mk

#
# Include user/app overrides
#
-include $(MY_CONFIG)

#==========================================
# Define output object dir if not already defined
#==========================================
#ifndef DM_OBJDIR

#ifdef DM_DEBUG
#BUILD_DIR=$(shell cd $(topsrcdir)/../;pwd)/bin-debug-$(BUILD_CONFIG)
#else
#BUILD_DIR=$(shell cd $(topsrcdir)/../;pwd)/bin-release-$(BUILD_CONFIG)
#endif

#else
BUILD_DIR=$(DM_OBJDIR)
#endif

MAKE_TARGET=$(topsrcdir)/config/Makefile.$(BUILD_CONFIG).mk

ifeq ($(OS_ARCH),WINNT)
# assume MSYS
#  We use 'pwd -W' to get DOS form of the path.  However, since the given path
#  could be a file or a non-existent path, we cannot call 'pwd -W' directly
#  on the path.  Instead, we extract the root path (i.e. "c:/"), call 'pwd -W'
#  on it, then merge with the rest of the path.
root-path = $(shell echo $(1) | sed -e "s|\(/[^/]*\)/\?\(.*\)|\1|")
non-root-path = $(shell echo $(1) | sed -e "s|\(/[^/]*\)/\?\(.*\)|\2|")
dos-path      = $(foreach p,$(1),$(if $(filter /%,$(p)),$(shell cd $(call root-path,$(p)) && pwd -W)$(call non-root-path,$(p)),$(p)))
# Normalize to windows path (i.e c:\)
normalizepath = $(subst /,\,$(call dos-path,$(1)))
else
normalizepath = $(1)
endif


