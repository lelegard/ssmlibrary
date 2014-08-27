#-----------------------------------------------------------------------------
#
#  Copyright (c) 2014, Thierry Lelegard
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License, or (at your option) any later version.
#  See the file COPYING.txt included with this distribution for more
#  information.
#
#-----------------------------------------------------------------------------
#
#  Makefile for the utests subdirectory of the SSM library (unitary tests).
#
#-----------------------------------------------------------------------------
#
#  Modification history:
#    2014-01-06 Thierry Lelegard
#               Original creation.
#
#-----------------------------------------------------------------------------

include ../Makefile.ssm

# Use CUnit library.
LDLIBS += -lcunit

# There is only one executable, linked from all modules here.
# We build a version using libssm.a and another one using libssm.so.
default: $(OBJDIR)/utests $(OBJDIR)/utests-shared
$(OBJDIR)/utests: $(OBJS) $(SSMLIB)
$(OBJDIR)/utests-shared: $(OBJS) $(SSMSHLIB)
	@echo '  [LD] $@'; \
	$(CC) $(LDFLAGS) $(OBJS) -L$(dir $(SSMSHLIB)) -lssm $(LDLIBS) -o $@

# Run the unitary tests.
.PHONY: test test-static test-shared 
test: test-static test-shared
test-static: $(OBJDIR)/utests
	$(OBJDIR)/utests
test-shared: $(OBJDIR)/utests-shared
	$(LD_LIBRARY_PATH_NAME)=../src/$(OBJDIR):$$$(LD_LIBRARY_PATH_NAME) $(OBJDIR)/utests-shared

# Run the unitary test for the default memory corruption handler.
# This tests crashes the application.
.PHONY: test-crash test-crash-static test-crash-shared
UTEST_CRASH_FLAGS := -t sbuffer_canary.test_default_canary_corrupted_handler -p TEST_DEFAULT_CANARY_CORRUPTED_HANDLER
test-crash: test-crash-static test-crash-shared
test-crash-static: $(OBJDIR)/utests
	@echo "The following test is expected to crash, do not worry"
	$(OBJDIR)/utests $(UTEST_CRASH_FLAGS) || :
test-crash-shared: $(OBJDIR)/utests-shared
	@echo "The following test is expected to crash, do not worry"
	$(LD_LIBRARY_PATH_NAME)=../src/$(OBJDIR):$$$(LD_LIBRARY_PATH_NAME) $(OBJDIR)/utests-shared $(UTEST_CRASH_FLAGS) || :

# Run the unitary tests under valgrind control.
.PHONY: valgrind valgrind-static valgrind-shared
VALGRIND_FLAGS = -q --leak-check=full --track-origins=yes
valgrind: valgrind-static valgrind-shared
valgrind-static: $(OBJDIR)/utests
	valgrind $(VALGRIND_FLAGS) $(OBJDIR)/utests
valgrind-shared: $(OBJDIR)/utests-shared
	$(LD_LIBRARY_PATH_NAME)=../src/$(OBJDIR):$$$(LD_LIBRARY_PATH_NAME) valgrind $(VALGRIND_FLAGS) $(OBJDIR)/utests-shared
