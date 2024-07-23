// $Id$
// Author: artur   2018/11/26


//_____________________________________________________________________________
//
// SpdGeoFactory
//_____________________________________________________________________________

#include "SpdGeoFactory.h"

#include "SpdItsGeoMapperX.h"
#include "SpdMvdGeoMapper.h"

#include "SpdTofBGeoMapper.h"
#include "SpdTofECGeoMapper.h"

#include "SpdEcalTBGeoMapper.h"
#include "SpdEcalBGeoMapper.h"
#include "SpdEcalTB2GeoMapper.h"
#include "SpdEcalTECGeoMapper.h"
#include "SpdEcalECGeoMapper.h"
#include "SpdEcalTEC2GeoMapper.h"
#include "SpdEcalSBGeoMapper.h"
#include "SpdEcalSECGeoMapper.h"

#include "SpdRsTBGeoMapper.h"
#include "SpdRsTB2GeoMapper.h"
#include "SpdRsTECGeoMapper.h"
#include "SpdRsTEC2GeoMapper.h"
#include "SpdRsSBGeoMapper.h"
#include "SpdRsSECGeoMapper.h"

#include "SpdTsTBGeoMapper.h"
#include "SpdTsTECGeoMapper.h"
#include "SpdTsSBGeoMapper.h"
#include "SpdTsSECGeoMapper.h"

#include "SpdItsGeoBuilder.h"
#include "SpdTsTBGeoBuilder.h"
#include "SpdTsTECGeoBuilder.h"
#include "SpdTsSBGeoBuilder.h"

#include "SpdZdcGeoMapper.h"
#include "SpdBbcGeoMapper.h"
#include "SpdAegGeoMapper.h"

#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdGeoFactory)

SpdGeoFactory* SpdGeoFactory::fInstance = 0;

SpdGeoFactory* SpdGeoFactory::Instance()
{
  return fInstance ? fInstance : new SpdGeoFactory();
}

//_____________________________________________________________________________
SpdGeoFactory::SpdGeoFactory() 
{
   if (fInstance) {
       cout << "-F- <SpdGeoFactory::SpdGeoFactory> Fatal error."
            << "Singleton object already exists" << endl;
       exit(1);
   }
     
   fInstance = this;
}

//_____________________________________________________________________________
SpdGeoFactory::~SpdGeoFactory() 
{
   fInstance = 0;
}

//_____________________________________________________________________________
SpdNodesIdTable* SpdGeoFactory::GetNodesIdTable(TString classname)
{
   if (classname == "SpdNodesIdTable") return new SpdNodesIdTable();
   return 0;
}

//_____________________________________________________________________________
SpdGeoMapper* SpdGeoFactory::SearchForMapper(DetectorId id, TString type)
{
  SpdGeoMapper* mapper = 0;
  
  switch (id) {
    
    case  kSpdIts : {
               if (type == "its") mapper = SpdItsGeoMapperX::GetMapper(); 
          else if (type == "mvd") mapper = SpdMvdGeoMapper::GetMapper(); 
          break; 
    }
    case  kSpdEcalTB  : { 
          mapper = SpdEcalBGeoMapper::GetMapper(); 
          break; 
    }
    case  kSpdEcalTEC : { 
          mapper = SpdEcalECGeoMapper::GetMapper(); 
          break; 
    }
    case  kSpdTofB  : { 
          mapper = SpdTofBGeoMapper::GetMapper(); 
          break; 
    }
    case  kSpdTofEC  : { 
          mapper = SpdTofECGeoMapper::GetMapper(); 
          break; 
    }
    case  kSpdTsTB  : { 
          mapper = SpdTsTBGeoMapper::GetMapper(); 
          break; 
    }
    case  kSpdTsTEC : { 
          mapper = SpdTsTECGeoMapper::GetMapper(); 
          break; 
    }
    case  kSpdRsTB  : { 
          mapper = SpdRsTBGeoMapper::GetMapper(); 
          break; 
    }
    case  kSpdRsTEC : { 
          mapper = SpdRsTECGeoMapper::GetMapper(); 
          break; 
    } 
    case  kSpdEcalSB  : { 
          mapper = SpdEcalSBGeoMapper::GetMapper(); 
          break; 
    }
    case  kSpdEcalSEC : { 
          mapper = SpdEcalSECGeoMapper::GetMapper(); 
          break; 
    }
    case  kSpdTsSB  : { 
          mapper = SpdTsSBGeoMapper::GetMapper(); 
          break; 
    }
    case  kSpdTsSEC : { 
          mapper = SpdTsSECGeoMapper::GetMapper(); 
          break; 
    }
    case  kSpdRsSB  : { 
          mapper = SpdRsSBGeoMapper::GetMapper(); 
          break; 
    }
    case  kSpdRsSEC : { 
          mapper = SpdRsSECGeoMapper::GetMapper(); 
          break; 
    }
    case  kSpdZdc : {
          mapper = SpdZdcGeoMapper::GetMapper(); 
          break; 
    }
    case  kSpdBbc : {
          mapper = SpdBbcGeoMapper::GetMapper(); 
          break; 
    }
    case  kSpdAeg : {
          mapper = SpdAegGeoMapper::GetMapper(); 
          break; 
    } 
    
    default: return 0;
  }
    
  return mapper;
}

//_____________________________________________________________________________
SpdGeoMapper* SpdGeoFactory::SearchForMapper(TString classname)
{
  SpdGeoMapper* mapper = 0;
  
  //cout << "-I <SpdGeoFactory::SearchForMapper> " << classname << endl;
  
       if (classname == "SpdItsGeoMapperX")     mapper = SpdItsGeoMapperX::GetMapper();
  else if (classname == "SpdMvdGeoMapper")      mapper = SpdMvdGeoMapper::GetMapper();
  
  else if (classname == "SpdEcalTBGeoMapper")   mapper = SpdEcalTBGeoMapper::GetMapper();
  else if (classname == "SpdEcalTB2GeoMapper")  mapper = SpdEcalTB2GeoMapper::GetMapper();
  else if (classname == "SpdEcalBGeoMapper")    mapper = SpdEcalBGeoMapper::GetMapper();
  else if (classname == "SpdEcalTECGeoMapper")  mapper = SpdEcalTECGeoMapper::GetMapper();
  else if (classname == "SpdEcalTEC2GeoMapper") mapper = SpdEcalTEC2GeoMapper::GetMapper();
  else if (classname == "SpdEcalECGeoMapper")   mapper = SpdEcalECGeoMapper::GetMapper();
  else if (classname == "SpdTsTBGeoMapper")     mapper = SpdTsTBGeoMapper::GetMapper();
  else if (classname == "SpdTsTECGeoMapper")    mapper = SpdTsTECGeoMapper::GetMapper();
  else if (classname == "SpdTofBGeoMapper")     mapper = SpdTofBGeoMapper::GetMapper();
  else if (classname == "SpdTofECGeoMapper")    mapper = SpdTofECGeoMapper::GetMapper();
  else if (classname == "SpdRsTBGeoMapper")     mapper = SpdRsTBGeoMapper::GetMapper();
  else if (classname == "SpdRsTB2GeoMapper")    mapper = SpdRsTB2GeoMapper::GetMapper();
  else if (classname == "SpdRsTECGeoMapper")    mapper = SpdRsTECGeoMapper::GetMapper();
  else if (classname == "SpdRsTEC2GeoMapper")   mapper = SpdRsTEC2GeoMapper::GetMapper();
  
  else if (classname == "SpdZdcGeoMapper")      mapper = SpdZdcGeoMapper::GetMapper();
  else if (classname == "SpdBbcGeoMapper")      mapper = SpdBbcGeoMapper::GetMapper();
  else if (classname == "SpdAegGeoMapper")      mapper = SpdAegGeoMapper::GetMapper();
  
  else if (classname == "SpdEcalSBGeoMapper")   mapper = SpdEcalSBGeoMapper::GetMapper();
  else if (classname == "SpdEcalSECGeoMapper")  mapper = SpdEcalSECGeoMapper::GetMapper();
  else if (classname == "SpdTsSBGeoMapper")     mapper = SpdTsSBGeoMapper::GetMapper();
  else if (classname == "SpdTsSECGeoMapper")    mapper = SpdTsSECGeoMapper::GetMapper();
  else if (classname == "SpdRsSBGeoMapper")     mapper = SpdRsSBGeoMapper::GetMapper();
  else if (classname == "SpdRsSECGeoMapper")    mapper = SpdRsSECGeoMapper::GetMapper();
    
  return mapper;
}

//_____________________________________________________________________________
SpdGeoMapper* SpdGeoFactory::Mapper(TString classname) 
{
  SpdGeoMapper* mapper = 0;
  
       if (classname == "SpdItsGeoMapperX")     mapper = SpdItsGeoMapperX::Instance();
  else if (classname == "SpdMvdGeoMapper")      mapper = SpdMvdGeoMapper::Instance();
 
  else if (classname == "SpdEcalTBGeoMapper")   mapper = SpdEcalTBGeoMapper::Instance();
  else if (classname == "SpdEcalTB2GeoMapper")  mapper = SpdEcalTB2GeoMapper::Instance();
  else if (classname == "SpdEcalBGeoMapper")    mapper = SpdEcalBGeoMapper::Instance();
  else if (classname == "SpdEcalTECGeoMapper")  mapper = SpdEcalTECGeoMapper::Instance();
  else if (classname == "SpdEcalTEC2GeoMapper") mapper = SpdEcalTEC2GeoMapper::Instance();
  else if (classname == "SpdEcalECGeoMapper")   mapper = SpdEcalECGeoMapper::Instance();
  else if (classname == "SpdTsTBGeoMapper")     mapper = SpdTsTBGeoMapper::Instance();
  else if (classname == "SpdTsTECGeoMapper")    mapper = SpdTsTECGeoMapper::Instance();
  else if (classname == "SpdTofBGeoMapper")     mapper = SpdTofBGeoMapper::Instance();
  else if (classname == "SpdTofECGeoMapper")    mapper = SpdTofECGeoMapper::Instance();
  else if (classname == "SpdRsTBGeoMapper")     mapper = SpdRsTBGeoMapper::Instance();
  else if (classname == "SpdRsTB2GeoMapper")    mapper = SpdRsTB2GeoMapper::Instance();
  else if (classname == "SpdRsTECGeoMapper")    mapper = SpdRsTECGeoMapper::Instance();
  else if (classname == "SpdRsTEC2GeoMapper")   mapper = SpdRsTEC2GeoMapper::Instance();
  
  else if (classname == "SpdZdcGeoMapper")      mapper = SpdZdcGeoMapper::Instance();
  else if (classname == "SpdBbcGeoMapper")      mapper = SpdBbcGeoMapper::Instance();
  else if (classname == "SpdAegGeoMapper")      mapper = SpdAegGeoMapper::Instance();
  
  else if (classname == "SpdEcalSBGeoMapper")   mapper = SpdEcalSBGeoMapper::Instance();
  else if (classname == "SpdEcalSECGeoMapper")  mapper = SpdEcalSECGeoMapper::Instance();
  else if (classname == "SpdTsSBGeoMapper")     mapper = SpdTsSBGeoMapper::Instance();
  else if (classname == "SpdTsSECGeoMapper")    mapper = SpdTsSECGeoMapper::Instance();
  else if (classname == "SpdRsSBGeoMapper")     mapper = SpdRsSBGeoMapper::Instance();
  else if (classname == "SpdRsSECGeoMapper")    mapper = SpdRsSECGeoMapper::Instance();
    
  return mapper;
}

//_____________________________________________________________________________
SpdGeoBuilder* SpdGeoFactory::SearchForBuilder(DetectorId id, TString type)
{
  SpdGeoBuilder* builder = 0;
  
  switch (id) {
    
    case  kSpdIts : {
          builder = SpdItsGeoBuilder::GetBuilder(); 
          break; 
    }
 
    case  kSpdTsTB : { 
          builder = SpdTsTBGeoBuilder::GetBuilder(); 
          break; 
    }
    
    case  kSpdTsTEC : { 
          builder = SpdTsTECGeoBuilder::GetBuilder(); 
          break; 
    }
   
    case  kSpdTsSB : { 
          builder = SpdTsSBGeoBuilder::GetBuilder(); 
          break; 
    }
    
    default: return 0;
  }
    
  return builder;
}

//_____________________________________________________________________________
SpdGeoBuilder* SpdGeoFactory::SearchForBuilder(TString classname)
{
  SpdGeoBuilder* builder = 0;
  
       if (classname == "SpdItsGeoBuilder")   builder = SpdItsGeoBuilder::GetBuilder();
  else if (classname == "SpdTsTBGeoBuilder")  builder = SpdTsTBGeoBuilder::GetBuilder();
  else if (classname == "SpdTsTECGeoBuilder") builder = SpdTsTECGeoBuilder::GetBuilder();
  
  else if (classname == "SpdTsSBGeoBuilder")  builder = SpdTsSBGeoBuilder::GetBuilder();
  
  return builder;

}

//_____________________________________________________________________________
SpdGeoBuilder* SpdGeoFactory::Builder(TString classname) 
{ 
  SpdGeoBuilder* builder = 0;
  
       if (classname == "SpdItsGeoBuilder")   builder = SpdItsGeoBuilder::Instance();
  else if (classname == "SpdTsTBGeoBuilder")  builder = SpdTsTBGeoBuilder::Instance();
  else if (classname == "SpdTsTECGeoBuilder") builder = SpdTsTECGeoBuilder::Instance();
  
  else if (classname == "SpdTsSBGeoBuilder")  builder = SpdTsSBGeoBuilder::Instance();
  
  return builder;
}


