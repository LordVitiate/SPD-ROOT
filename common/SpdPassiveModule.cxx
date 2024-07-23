// $Id$
// Author: artur   2018/01/30


//_____________________________________________________________________________
//
// SpdPassiveModule
//_____________________________________________________________________________

#include "SpdPassiveModule.h"
#include "SpdCommonGeoMapper.h"
#include "SpdParSetContFact.h"
#include "SpdPassiveGeoParSet.h"

#include "FairGeoBuilder.h"
#include "FairGeoMedia.h"

#include "TGeoManager.h"

#include <iostream>
using std::cout;
using std::endl;

ClassImp(SpdPassiveModule)

//_____________________________________________________________________________
SpdPassiveModule::SpdPassiveModule():FairModule(),fGeoType(-1),fMasterVolume(0),
fModuleId(-1),fUnsetMedia("")
{
   fName  = ClassName();
   fTitle = "SPD Passive Module";
   
   SpdCommonGeoMapper::Instance()->AddPassive(this);
}

//_____________________________________________________________________________
SpdPassiveModule::SpdPassiveModule(const char* Name, const char* Prefix, Int_t ModId):
FairModule(Name,Prefix),fGeoType(-1),fMasterVolume(0),fModuleId(ModId),fUnsetMedia("")
{  
   SpdCommonGeoMapper::Instance()->AddPassive(this);
}

//_____________________________________________________________________________
SpdPassiveModule::~SpdPassiveModule() 
{
 
}

//_____________________________________________________________________________
void SpdPassiveModule::Print(Option_t*) const
{
   cout << "<SpdPassiveModule::Print>" << endl;
   cout << " Geometry type:   " << fGeoType << endl;
   cout << " Name:            " << fName << endl;
   cout << " Id:              " << fModuleId << endl;
   cout << " Unset material:  " << fUnsetMedia << endl;
}

//_____________________________________________________________________________
Double_t SpdPassiveModule::GetDensity() const 
{ 
   Double_t capacity = GetCapacity();
   return (capacity > 1e-9) ? GetMass()/capacity : 0;
}

//_____________________________________________________________________________
TString SpdPassiveModule::FullName(TString name, Bool_t add_uscore)
{
   return (add_uscore) ? Form("%s_%s",fTitle.Data(),name.Data()) :  fTitle+name;
}

//_____________________________________________________________________________
TGeoMedium* SpdPassiveModule::FindMedium(TString& medname, TString default_med) 
{ 
   TGeoMedium* med = 0;
   
   if (medname.IsWhitespace()) return 0;
     
   if (gGeoManager) {
       med = gGeoManager->GetMedium(medname);
       if (med) return med;
   }
     
   FairGeoLoader* geoLoad = FairGeoLoader::Instance();  
   if (!geoLoad) return 0;     
          
   FairGeoInterface* geoFace = geoLoad->getGeoInterface();
   if (!geoFace) return 0;     
  
   FairGeoMedia* Media = geoFace->getMedia();
   if (!Media) return 0;    
  
   FairGeoBuilder* geoBuild = geoLoad->getGeoBuilder(); 
   if (!geoBuild) return 0;
   
   if (!Media->getMedium(medname)) return 0;
          
   Int_t nmed = geoBuild->createMedium(Media->getMedium(medname));
   med = gGeoManager->GetMedium(medname);
   
   if (med) return med;
   
   medname = default_med;
   
   if (default_med.IsWhitespace()) return 0;
   
   return FindMedium(medname,"");
}

//_____________________________________________________________________________
Bool_t SpdPassiveModule::FillParsIn(SpdPassiveGeoParSet* params)
{
   if (!params) return kFALSE;

   params->SetParameter(Form("%s/UnsetMedia",this->ClassName()),fUnsetMedia);
   
   return kTRUE;
}

//_____________________________________________________________________________    
Bool_t SpdPassiveModule::LoadParsFrom(SpdPassiveGeoParSet* params)
{
   if (!params) return kFALSE; 
   return kTRUE;
}

//_____________________________________________________________________________________
SpdPassiveGeoParSet* SpdPassiveModule::GetParameters()
{
   FairRun* run = FairRun::Instance();
   if (!run) return 0;
   FairRuntimeDb* rtdb = run->GetRuntimeDb(); 
   if (!rtdb) return 0;
   SpdPassiveGeoParSet* pars = (SpdPassiveGeoParSet*)(rtdb->getContainer("PassiveGeoParSet"));
   return pars;
}
