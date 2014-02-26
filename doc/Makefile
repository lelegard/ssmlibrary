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
#  Makefile for the doc subdirectory of the SSM library.
#
#-----------------------------------------------------------------------------
#
#  Modification history:
#    2014-01-06 Thierry Lelegard
#               Original creation.
#
#-----------------------------------------------------------------------------

include ../Makefile.ssm

.PHONY: doc
NODEPS += doc

INFILES := $(wildcard ssm*.texi)
OUTFILES = ssm.info ssm.html ssm.pdf
EXTRA_DISTCLEAN = $(OUTFILES) version.texi
TEXI2HTML_FLAGS += --css-include=texinfo.css

SSM_MAJOR_VERSION = $(shell grep "\#define *SSM_MAJOR_VERSION" ../include/ssm.h | head -1 | awk '{print $$3}')
SSM_MINOR_VERSION = $(shell grep "\#define *SSM_MINOR_VERSION" ../include/ssm.h | head -1 | awk '{print $$3}')

# No default target, use "make doc" explicitely.
default:
	@true

# Generate the documentation in various formats.
doc: $(OUTFILES)
$(OUTFILES): $(INFILES) version.texi texinfo.tex
ssm.html: texinfo.css
version.texi: ../include/ssm.h Makefile
	@echo Rebuilding $@
	@LANG=en_US echo >$@ "@set UPDATED $$(date +'%d %B %Y')"
	@LANG=en_US echo >>$@ "@set UPDATED-MONTH $$(date +'%B %Y')"
	@echo >>$@ "@set EDITION $(SSM_MAJOR_VERSION).$(SSM_MINOR_VERSION)"
	@echo >>$@ "@set VERSION $(SSM_MAJOR_VERSION).$(SSM_MINOR_VERSION)"
