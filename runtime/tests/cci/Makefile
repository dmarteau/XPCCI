#
# Makefile for daim unit tests
#
DEPTH=../../..

include $(DEPTH)/config/config.mk

MODULE_NAME = tests
DM_TESTS  = test_cciinit \
	        test_comptr \
	        test_crossref \
	        test_factory  \
	        test_catmngr \
            $(NULL)

libs::
	@for d in $(DM_TESTS); do \
		$(MAKE) -f test.mk TEST_NAME=$$d; \
	done

clean::
	@for d in $(DM_TESTS); do \
		$(MAKE) -f test.mk TEST_NAME=$$d clean; \
	done

include $(topsrcdir)/config/rules.mk
