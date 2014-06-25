#-----------------------------------------------------------------------------
#
#  Copyright (c) 2014, Thierry Lelegard
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 3.0 of the License, or (at your option) any later version.
#  See the file COPYING.txt included with this distribution for more
#  information.
#
#-----------------------------------------------------------------------------
#
#  Makefile for the src subdirectory of the SSM library (library code).
#
#-----------------------------------------------------------------------------
#
#  Modification history:
#    2014-01-06 Thierry Lelegard
#               Original creation.
#    2014-06-25 Thierry Lelegard
#               Added C11K code footprint.
#
#-----------------------------------------------------------------------------

include ../Makefile.ssm

.PHONY: gcov showcodesize

# Rebuild the static and dynamic libraries
default: $(SSMLIB) $(SSMSHLIB)

# All files in this directory are part of the library
$(SSMLIB): $(SSMLIB)($(OBJS))
$(SSMSHLIB): $(OBJS)
	@echo '  [CC] $@'; \
	$(CC) $(CFLAGS) $^ $(LDLIBS) -shared -o $@

# Report code coverage test results.
gcov:
	@rm -f *.gcov $(OBJDIR)/*.gcov
	gcov -o $(OBJDIR) *.c >$(OBJDIR)/gcov.log
	@mv -f *.gcov $(OBJDIR)
	@grep '#####' $(OBJDIR)/*.gcov || echo "100% CODE COVERAGE"

# Display code size.
PRODUCTION_FILES = $(basename $(shell ls *.c | grep -v _canary))
COMMON_FILES  = ssm_copy ssm_compare ssm_set
SBUFFER_FILES = $(filter ssm_sbuffer_%,$(PRODUCTION_FILES)) $(COMMON_FILES)
DBUFFER_FILES = $(filter ssm_dbuffer_%,$(PRODUCTION_FILES)) $(COMMON_FILES) ssm_memory_management ssm_alloc_size
SSTRING_FILES = $(filter ssm_sstring_%,$(PRODUCTION_FILES)) $(SBUFFER_FILES) ssm_cstring_length
DSTRING_FILES = $(filter ssm_dstring_%,$(PRODUCTION_FILES)) $(DBUFFER_FILES) ssm_cstring_length
C11K_FILES    = $(filter-out ssm_strerror_s,$(basename $(wildcard ssm_*_s.c))) ssm_copy ssm_set ssm_cstring_length
SIZE = printf "%-30s %5d bytes\n" $(1) $$(size -t $(addprefix $(OBJDIR)/,$(addsuffix .o,$(2))) | tail -1 | awk '{print $$4;}')
showcodesize: $(SSMLIB)
	@$(call SIZE,"Total code size",$(MODULES))
	@$(call SIZE,"Production only (non-canary)",$(PRODUCTION_FILES))
	@$(call SIZE,"Static strings only",$(SSTRING_FILES))
	@$(call SIZE,"Dynamic strings only",$(DSTRING_FILES))
	@$(call SIZE,"Static buffers only",$(SBUFFER_FILES))
	@$(call SIZE,"Dynamic buffers only",$(DBUFFER_FILES))
	@$(call SIZE,"C11 Annex K only",$(C11K_FILES))
