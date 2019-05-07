
DEPTH=../..

include $(DEPTH)/config/config.mk

MODULE_NAME=platform
EXPORTS = ac_dm_config.h \
          dmTypes.h \
          dmTarget.h \
          dmCrt.h \
          dmCrt.cpp \
          dmCompilers.h \
          dmDebugBreak.h \
          $(NULL)

ifeq ($(OS_ARCH),WINNT)
  EXPORTS += win/_dmWinVer.h
endif

EXPORTS += $(NULL)

include $(topsrcdir)/config/rules.mk
