#

DEPTH= ..

include $(DEPTH)/config/config.mk

MODULE_NAME = xpcci
DIRS=src glue

# Always clean test when rebuild
ifeq ($(BUILD_TESTS),1)
libs:: clean_tests
clean:: clean_tests
endif

include $(topsrcdir)/config/rules.mk

ifeq ($(BUILD_TESTS),1)
libs:: build_tests
endif


.PHONY: build_tests

build_tests:
	$(MAKE) -C tests

clean_tests:
	$(MAKE) -C tests clean
