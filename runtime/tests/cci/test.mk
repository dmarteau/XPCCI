
DEPTH=../../..

include $(DEPTH)/config/config.mk

MODULE_INCL  = $(DEPTH)/include/unit_tests
MODULE_NAME  = tests
LIBRARY_NAME = $(TEST_NAME)
MODULE_EXT   = exe

MODULE_FILES=$(TEST_NAME).cpp

REQUIRES=cci daim

MODULE_LIBS = -lcciglue_s

# Link against xpcci lib
XPCCI_LIB:=1

include $(topsrcdir)/config/rules.mk
