
DEPTH= ..
VPATH= ./source

include $(DEPTH)/config/config.mk

MODULE_NAME  = doc

ifndef DOXYGEN_PATH
$(error DOXYGEN_PATH not set !)
endif

DOC_INSTALL = $(BUILD_DIR)/doc

libs::
	@mkdir -p $(DOC_INSTALL)
	@cp Doxyfile $(DOC_INSTALL)/
	@cd $(DOC_INSTALL); $(DOXYGEN_PATH);

clean::
	@rm -fr $(DOC_INSTALL)

include $(topsrcdir)/config/rules.mk
