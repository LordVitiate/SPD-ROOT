#--------------------------------------------------------------------------
# File and Version Information:
#      $Id: common.mk,v 1.20 2004/04/14 14:49:56 asanchez Exp $
#
# Description:
# 	Common part for all standard makefiles
#
# Environment:
#      Software developed for the PANDA Detector at GSI, Darmstadt
#
# Author List:
#      Sergey Ganzhur                Original Author
#
# Copyright Information:
#      Copyright (C) 2002           Ruhr Universitaet Bochum
#
#------------------------------------------------------------------------
# Assumes the environment variable ROOTSYS to exist
#

# Define directory paths

OBJDIR        = ./tmp
TMPDIR        = ./tmp
LIBDIR        = ./lib
SLIBDIR       = ./lib
BINDIR        = ./

ROOTCFLAGS    = $(shell root-config --cflags)
ROOTLIBS      = $(shell root-config --libs)
ROOTGLIBS     = $(shell root-config --glibs)

# Linux with egcs

F77	      = gfortran
CXX           = g++
CXXFLAGS      = -g -O0 -Wall -DGNU_GCC -fexceptions -fPIC
LD            = g++
LDFLAGS       = -g -Wl,-rpath,$(LIBDIR):$(ROOTSYS)/lib:/lib
SOFLAGS       = -shared
F77FLAGS      = -fPIC


CPPFLAGS += -Iinclude
#CPPFLAGS += -I$(CLHEP_INCLUDE_DIR)
#CPPFLAGS += -I$(GEANT4_INCLUDE_DIR)
CPPFLAGS += -I$(Geant4_INCLUDE_DIRS)
#LDFLAGS  += -L$(CLHEP_LIB_DIR)
#LDFLAGS  += -L$(CLHEP_LIBRARY_DIR)
#LDLIBS   += -l$(CLHEP_LIB)

CPPFLAGS 	+= $(ROOTCFLAGS) -I$(ROOTSYS)/include -I..

LIBS          = $(ROOTLIBS) -lEG  -lTreePlayer -lMinuit
GLIBS         = $(ROOTGLIBS) -lEG -lTreePlayer -lMinuit

FTFLIB		= -L$(LIBDIR) -lFtfEvtGen

#LDLIBS		+= -L$(GEANT4_LIBRARY_DIR) -lG4clhep -lG4global -lG4geometry -lG4physicslists -lG4materials -lG4processes -lG4track -#lG4particles -lEG $(GLIBS)

LDLIBS		+= -L$(SIMPATH)/lib -lG4clhep -lG4global -lG4geometry -lG4physicslists -lG4materials -lG4processes -lG4track -lG4particles -lEG $(GLIBS)

# Static pattern rule for object file dependency on sources:
$(OBJDIR)/%.o: %.cc
	@echo Compiling  file $< ...
	@if [ ! -d $(OBJDIR) ] ; then mkdir -p $(OBJDIR)  ;fi
	@($(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@ )

$(OBJDIR)/%.o: %.c
	@echo Compiling  file $< ...
	@if [ ! -d $(OBJDIR) ] ; then mkdir -p $(OBJDIR)  ;fi
	@($(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@ )


$(OBJDIR)/%.o: %.cxx
	@echo Compiling  file $< ...
	@if [ ! -d $(OBJDIR) ] ; then mkdir -p $(OBJDIR)  ;fi
	@($(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@ )


$(OBJDIR)/%.o: %.f
	@echo Compiling file $< ...
	@if [ ! -d $(OBJDIR) ] ; then mkdir -p $(OBJDIR)  ;fi
	@$(F77) $(F77FLAGS) -c $< -o $@












