#=========================================================
# Make rules for building idl files
#
# This file is intended to be included in the main makefile
#=========================================================

.SUFFIXES: .idl

PYIDLBIN=$(topsrcdir)/cci/idl-parser/header.py

XPTBIN=$(topsrcdir)/cci/idl-parser/typelib.py

CPP_IDL_FILES = $(patsubst %.idl,$(EXPORT_DIR)/%.h,$(IDLSRC))
XPT_IDL_FILES = $(patsubst %.idl,$(STAGE_DIR)/%.xpt,$(IDLSRC))
IDL_REQUIRES  = cci $(MODULE_EXPORT_NAME) $(REQUIRES)
IDL_INCLUDES  = $(patsubst %,-I"$(IDLDIR)/%",$(IDL_REQUIRES))
IDL_PYCACHE   = "$(BUILD_DIR)/.parts/xpidl" 
IDL_FLAGS     = --cachedir $(IDL_PYCACHE)

xpidl:: $(CPP_IDL_FILES) $(XPT_IDL_FILES)

# Rule for building C++ header
$(EXPORT_DIR)/%.h : %.idl
	@echo $<
	@mkdir -p $(IDL_PYCACHE)
	@python $(PYIDLBIN) $(IDL_FLAGS) $(IDL_INCLUDES) $< > "$@"

# Rule for building xpt files
$(STAGE_DIR)/%.xpt : %.idl
	@echo $<
	@mkdir -p $(IDL_PYCACHE)
	@python $(XPTBIN) $(IDL_FLAGS) $(IDL_INCLUDES) $< -o "$@"

clean::
	@for f in $(CPP_IDL_FILES); do \
	  rm -f "$$f"; \
	done
	@for f in $(XPT_IDL_FILES); do \
	  rm -f "$$f"; \
	done

