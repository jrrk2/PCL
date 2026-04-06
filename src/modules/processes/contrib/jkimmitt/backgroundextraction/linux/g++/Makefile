######################################################################
# BackgroundExtraction Module
# Platform: Linux/g++
# Configuration: Release/all
######################################################################

#
# Targets
#

export PCLSRCDIR ?= /home/jonathan/PCL/src
export PCLINCDIR ?= /home/jonathan/PCL/include
export PCLLIBDIR64 ?= $(PCLSRCDIR)/pcl/linux/g++/x64/Release
export PCLBINDIR64 ?= $(PCLSRCDIR)/../bin

.PHONY: all
all:
	$(MAKE) -f ./makefile-x64 --no-print-directory

.PHONY: clean
clean:
	$(MAKE) -f ./makefile-x64 --no-print-directory clean
