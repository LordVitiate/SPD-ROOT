/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             * 
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *  
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
#include "SpdHmrsGeo.h"
#include "FairGeoNode.h"

ClassImp(SpdHmrsGeo)

// -----   Default constructor   -------------------------------------------
SpdHmrsGeo::SpdHmrsGeo()
  : FairGeoSet()
{
  // Constructor
  // fName has to be the name used in the geometry for all volumes.
  // If there is a mismatch the geometry cannot be build.
  fName="SpdHmrs";
  maxSectors=0;
  maxModules=10;
}

// -------------------------------------------------------------------------

const char* SpdHmrsGeo::getModuleName(Int_t m)
{
  /** Returns the module name of SpdHmrs number m
      Setting SpdHmrs here means that all modules names in the
      ASCII file should start with SpdHmrs otherwise they will
      not be constructed
  */
  sprintf(modName,"SpdHmrs%i",m+1);
  return modName;
}

const char* SpdHmrsGeo::getEleName(Int_t m)
{
  /** Returns the element name of Det number m */
  sprintf(eleName,"SpdHmrs%i",m+1);
  return eleName;
}
