
DEPTH= ..
VPATH= ./source

include $(DEPTH)/config/config.mk

MODULE_NAME  = xpcci_utilities
LIBRARY_NAME = xpcci_utilities
MODULE_EXT   = dll
MODULE_INCL = ./include
MODULE_DEFS = DM_UTILITIES_BUILD
MODULE_FILES=dmString.cpp \
             dmDate.cpp \
             dmEnvironment.cpp \
             dmExceptions.cpp \
             dmFastMemory.cpp \
             dmList.cpp \
             dmMemory.cpp \
             dmMutex.cpp \
             dmPortability.cpp \
             dmRWLocks.cpp \
             dmTime.cpp \
             dmUtillitiesBase.cpp \
             dmDebug.cpp \
             dmTraceMem.cpp \
             $(NULL)

ifeq ($(BUILD_CONFIG),-linux-gcc)
MODULE_LIBS = -ldl -lpthread
endif

# Always clean test when rebuild
ifeq ($(BUILD_TESTS),1)
libs:: clean_tests
clean:: clean_tests
endif

include $(topsrcdir)/config/rules.mk

ifeq ($(BUILD_TESTS),1)
libs:: build_tests
endif

build_tests:
	$(MAKE) -C tests

clean_tests:
	$(MAKE) -C tests clean
