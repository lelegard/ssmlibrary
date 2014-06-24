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
#  Makefile for the top-level directory of the SSM library source tree.
#
#-----------------------------------------------------------------------------
#
#  Modification history:
#    2014-01-06 Thierry Lelegard
#               Original creation.
#
#-----------------------------------------------------------------------------

include Makefile.ssm

# By default, recurse make target in all subdirectories
RECURSE_ORDER = src utests demos doc $(wildcard msvc*)
default:
	+@$(RECURSE)

# Run the unitary tests.
.PHONY: test test-debug valgrind valgrind-debug
test valgrind:
	+@$(MAKE) -C src
	+@$(MAKE) -C utests $@
test-debug valgrind-debug:
	+@$(MAKE) DEBUG=TRUE -C src
	+@$(MAKE) DEBUG=TRUE -C utests $(subst -debug,,$@)

# Perform coverage tests.
.PHONY: gcov
gcov:
	+$(MAKE) DEBUG=true GCOV=true -C src clean
	+$(MAKE) DEBUG=true GCOV=true -C src
	+$(MAKE) DEBUG=true GCOV=true -C utests clean
	+$(MAKE) DEBUG=true GCOV=true -C utests test-static test-crash-static
	+$(MAKE) DEBUG=true GCOV=true -C src gcov

# Show code footprint.
.PHONY: showcodesize
showcodesize:
	+$(MAKE) -C src clean
	+$(MAKE) -C src optsize showcodesize

# Generate the documentation.
.PHONY: doc
doc:
	+@$(MAKE) -C doc doc

# Perform all tests that should succeed before integrating the current code state.
.PHONY: preintegration
preintegration: \
	distclean \
	default \
	debug \
	$(if $(shell which cppcheck 2>/dev/null),cppcheck,) \
	$(if $(shell which flawfinder 2>/dev/null),flawfinder-ssm,) \
	test \
	test-debug \
	$(if $(shell which valgrind 2>/dev/null),valgrind-debug,) \
	$(if $(shell which gcov 2>/dev/null),gcov,)

# Execute flawfinder on SSM library code only.
.PHONY: flawfinder-ssm
flawfinder-ssm:
	$(FLAWFINDER) $(FLAWFINDER_FLAGS) include src

# Make a tarball of the current source tree
.PHONY: tarball
tarball:
	tar czf ../ssm-$(SSM_VERSION).tgz \
	    -C .. $(notdir $(shell pwd)) \
	    --transform='s/^$(notdir $(shell pwd))/ssm-$(SSM_VERSION)/' \
	    $(patsubst %,--exclude '%',$(NOSOURCE) kernelmodule *.html *.info version.texi)
