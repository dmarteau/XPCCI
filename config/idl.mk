#=========================================================
# Make rules for building idl files
#
# This file is intended to be included in the main makefile
#=========================================================

.SUFFIXES: .idl

PYIDLBIN=$(topsrcdir)/cci/idl-parser/header.py

CPP_IDL_FILES = $(patsubst %.idl,$(EXPORT_DIR)/%.h,$(IDLSRC))
IDL_REQUIRES  = cci $(MODULE_EXPORT_NAME) $(REQUIRES)
IDL_INCLUDES  = $(patsubst %,-I"$(IDLDIR)/%",$(IDL_REQUIRES))
IDL_PYCACHE   = "$(BUILD_DIR)/.parts/xpidl" 
IDL_FLAGS     = --cachedir $(IDL_PYCACHE)

build_idl:$(CPP_IDL_FILES)

# Rule for building C++ header
$(EXPORT_DIR)/%.h : %.idl
	@echo $<
	@mkdir -p $(IDL_PYCACHE)
	@python $(PYIDLBIN) $(IDL_FLAGS) $(IDL_INCLUDES) $< > "$@"

clean_idl:
	@for f in $(CPP_IDL_FILES); do \
	  rm -f "$$f"; \
	done
