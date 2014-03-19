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
OUTFILES_VERSION = $(patsubst ssm.%,ssm-$(SSM_VERSION).%,$(OUTFILES))
EXTRA_DISTCLEAN = $(patsubst ssm.%,ssm*.%,$(OUTFILES)) version.texi
TEXI2HTML_FLAGS += --css-include=texinfo.css

# No default target, use "make doc" explicitely.
default:
	@true

# Generate the documentation in various formats.
doc: $(OUTFILES_VERSION)
$(OUTFILES): $(INFILES) version.texi texinfo.tex
ssm.html: texinfo.css
version.texi: ../include/ssm.h Makefile
	@echo Rebuilding $@
	@echo >$@ "@set UPDATED $$(date +'%d %B %Y')"
	@echo >>$@ "@set UPDATED-MONTH $$(date +'%B %Y')"
	@echo >>$@ "@set EDITION $(SSM_VERSION)"
	@echo >>$@ "@set VERSION $(SSM_VERSION)"
ssm-$(SSM_VERSION).%: ssm.%
	cp $< $@
