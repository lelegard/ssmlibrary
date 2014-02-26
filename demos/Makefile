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
#  Makefile for the demos subdirectory of the SSM library.
#
#  Demo applications are built on all systems. On Linux, to build the
#  kernel module demo, use "make kernelmodule".
#
#-----------------------------------------------------------------------------
#
#  Modification history:
#    2014-01-06 Thierry Lelegard
#               Original creation.
#
#-----------------------------------------------------------------------------

include ../Makefile.ssm

.PHONY: kernelmodule

# Each source file in this directory is an executable.
EXECS := $(addprefix $(OBJDIR)/,$(filter-out kernelmodule,$(MODULES)))

# Rebuild all executables.
default: $(EXECS)
$(EXECS): $(SSMLIB)

# Temporary directory used for kernel module source.
LKMDIR := kernelmodule
NORECURSE_SUBDIRS += $(LKMDIR)
EXTRA_DISTCLEAN += $(LKMDIR)

# Build the temporary directory used for kernel module source and build the module.
kernelmodule:
	mkdir -p $(LKMDIR)
	cd $(LKMDIR); ln -sf ../Makefile.kernelmodule Makefile
	cd $(LKMDIR); ln -sf ../kernelmodule.c ../../include/ssm.h ../../src/ssm_*.[ch] .
	rm $(LKMDIR)/*_canary*
	$(MAKE) -C $(LKMDIR)
