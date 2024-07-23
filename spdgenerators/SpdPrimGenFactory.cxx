// $Id$
// Author: artur   2018/12/05


//_____________________________________________________________________________
//
// SpdPrimGenFactory
//_____________________________________________________________________________

#include "SpdPrimGenFactory.h"

#include "SpdParticleGenerator.h"
#include "SpdMultiParticleGenerator.h"
#include "SpdEvtBaseGenerator.h"
#include "SpdIsotropicGenerator.h"
#include "SpdPythia6Generator.h"
#include "SpdPythia8Generator.h"
#include "SpdFtfGenerator.h"


#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdPrimGenFactory)

SpdPrimGenFactory* SpdPrimGenFactory::fInstance = 0;

SpdPrimGenFactory* SpdPrimGenFactory::Instance()
{
  return fInstance ? fInstance : new SpdPrimGenFactory();
}

//_____________________________________________________________________________
SpdPrimGenFactory::SpdPrimGenFactory() 
{
  if (fInstance) {
      cout << "-F- <SpdGeoFactory::SpdGeoFactory> Fatal error."
           << "Singleton object already exists" << endl;
      exit(1);
  }
     
  fInstance = this;
}

//_____________________________________________________________________________
SpdPrimGenFactory::~SpdPrimGenFactory() 
{
  fInstance = 0;
}

//_____________________________________________________________________________
SpdGenerator* SpdPrimGenFactory::CreateGenerator(TString gentype)
{
  SpdGenerator* gen = 0;
  
       if (gentype == "SpdParticleGenerator") gen = new SpdParticleGenerator();
  else if (gentype == "SpdMultiParticleGenerator") gen = new SpdMultiParticleGenerator();      
  else if (gentype == "SpdIsotropicGenerator") gen = new SpdIsotropicGenerator();
  else if (gentype == "SpdPythia6Generator") gen = new SpdPythia6Generator();
  else if (gentype == "SpdPythia8Generator") gen = new SpdPythia8Generator();
  else if (gentype == "SpdFtfGenerator") gen = new SpdFtfGenerator();
  else if (gentype == "SpdEvtBaseGenerator") gen = new SpdEvtBaseGenerator();
  else if (gentype == "") gen = new SpdPythia6Generator();
  
  return gen;
}

//_____________________________________________________________________________
SpdGenerator* SpdPrimGenFactory::CreateGenerator(TString gentype, TString genname)
{
  SpdGenerator* gen = 0;
  
       if (gentype == "SpdParticleGenerator") gen = new SpdParticleGenerator(genname);
  else if (gentype == "SpdMultiParticleGenerator") gen = new SpdMultiParticleGenerator(genname);     
  else if (gentype == "SpdIsotropicGenerator")  gen = new SpdIsotropicGenerator(genname);
  else if (gentype == "SpdPythia6Generator") gen = new SpdPythia6Generator(genname);
  else if (gentype == "SpdPythia8Generator") gen = new SpdPythia8Generator(genname);
  else if (gentype == "SpdFtfGenerator") gen = new SpdFtfGenerator(genname);
  else if (gentype == "SpdEvtBaseGenerator") gen = new SpdEvtBaseGenerator(genname);
  else if (gentype == "") gen = new SpdPythia6Generator(genname);
  
  return gen;  
}

