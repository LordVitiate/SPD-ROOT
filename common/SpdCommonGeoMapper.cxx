// $Id$
// Author: artur   2018/01/30

//_____________________________________________________________________________
//
// SpdCommonGeoMapper
//_____________________________________________________________________________

#include "FairGeoLoader.h"
#include "FairGeoInterface.h" 
#include "FairGeoMedia.h"

#include "SpdCommonGeoMapper.h"
#include "SpdPassiveModule.h"
#include "SpdDetector.h"
#include "SpdGeoMapper.h"
#include "SpdModulesMap.h"

#include <TGeoManager.h>
#include <TSystem.h>

#include <iostream>

using std::cout;
using std::endl;

using std::map;
using std::pair;

SpdCommonGeoMapper* SpdCommonGeoMapper::fInstance = 0;

TGeoVolume*  SpdCommonGeoMapper::theMasterVolume  = 0;

/*==================================================================*/
/*==================================================================*/
/*==================================================================*/

Int_t    SpdCommonGeoMapper::theNGeoSectors             = 8;     // > 2 and < 13
Double_t SpdCommonGeoMapper::theSectorClearance         = 0.4;   // cm

/*============================= PIPE ===============================*/
 
Int_t    SpdCommonGeoMapper::thePipeDefGeoType          = 5;           // : 1,2,3,4
//TString  SpdCommonGeoMapper::thePipeMaterial1           = "beryllium"; // : 1,2,3,4
TString  SpdCommonGeoMapper::thePipeMaterial1           = "aluminium"; // : 1,2,3,4
TString  SpdCommonGeoMapper::thePipeMaterial2           = "aluminium"; // : 4

Double_t SpdCommonGeoMapper::thePipeOuterRadius         = 3.1;         // [cm]  : 1,2,3
Double_t SpdCommonGeoMapper::thePipeThickness           = 0.1;         // [cm]  : 1,2,3

Double_t SpdCommonGeoMapper::thePipeLength1             = 1400.;       // [cm]  : 1
Double_t SpdCommonGeoMapper::thePipeLength2             = 1000.;       // [cm]  : 2
Double_t SpdCommonGeoMapper::thePipeLength3             = 1000.;       // [cm]  : 3


/*===================== INNER TRACKING SYSTEM ======================*/

Int_t    SpdCommonGeoMapper::theItsDefGeoType           = 3;            // : 1
TString  SpdCommonGeoMapper::theItsBaseMaterial         = "air";       
TString  SpdCommonGeoMapper::theItsLadderMaterial       = "silicon";       
TString  SpdCommonGeoMapper::theItsSensorMaterial       = "silicon";
Int_t    SpdCommonGeoMapper::theItsNLayers              = 5;      
Double_t SpdCommonGeoMapper::theItsMinRadius            = 5.0;          // cm
Double_t SpdCommonGeoMapper::theItsMaxRadius            = 26.5;          // cm
Double_t SpdCommonGeoMapper::theItsMaxLength            = 200.;         // cm

/*===================== BEAM-BEAM COUNTER (BBC) ======================*/

Int_t    SpdCommonGeoMapper::theBbcDefGeoType           = 1;        
TString  SpdCommonGeoMapper::theBbcBaseMaterial         = "silicon";  // "air"
Double_t SpdCommonGeoMapper::theBbcThickness            = 0.5; //2.;    // cm
Double_t SpdCommonGeoMapper::theBbcSize                 = 84.7;   // cm
Double_t SpdCommonGeoMapper::theBbcWidth                = 78.7;   // cm 
Double_t SpdCommonGeoMapper::theBbcMinDist              = 186.6; // cm

/*===================== TIME-OF-FLIGHT SYSTEM (BARREL) ======================*/
    
Int_t    SpdCommonGeoMapper::theTofBDefGeoType          = 1;         
TString  SpdCommonGeoMapper::theTofBBaseMaterial        = "air";     
Double_t SpdCommonGeoMapper::theTofBLength              = 371.2; // cm
Double_t SpdCommonGeoMapper::theTofBSize                = 112.0;  // cm
Double_t SpdCommonGeoMapper::theTofBWidth               = 18.7;   // cm
    
/*===================== TIME-OF-FLIGHT SYSTEM (ENDCAPS) =====================*/
    
Int_t    SpdCommonGeoMapper::theTofECDefGeoType         = 1;        
TString  SpdCommonGeoMapper::theTofECBaseMaterial       = "air";  
Double_t SpdCommonGeoMapper::theTofECThickness          = 6.; //28.;   // cm
Double_t SpdCommonGeoMapper::theTofECSize               = 89.2;  // cm
Double_t SpdCommonGeoMapper::theTofECWidth              = 74.2;  // cm 
Double_t SpdCommonGeoMapper::theTofECMinDist            = 194.0;  // cm

/*===================== AEROGEL (AEG) ======================*/

Int_t    SpdCommonGeoMapper::theAegDefGeoType           = 1;        
TString  SpdCommonGeoMapper::theAegBaseMaterial         = "air";  
Double_t SpdCommonGeoMapper::theAegThickness            = 30.0;   // cm
Double_t SpdCommonGeoMapper::theAegSize                 = 84.7;   // cm
Double_t SpdCommonGeoMapper::theAegWidth                = 78.7;   // cm 
Double_t SpdCommonGeoMapper::theAegMinDist              = 153.6; //199.6; // cm
    
/*==================================================================*/
/*= ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ =*/
/*= ++++++++++++++++++ TOROIDAL GEOMETRY +++++++++++++++++++++++++ =*/
/*= ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ =*/
/*==================================================================*/

/*==================== BASKET (TOR.GEOM.) ==========================*/

Int_t    SpdCommonGeoMapper::theTorBasketDefGeoType     = 1;    // : 1
Double_t SpdCommonGeoMapper::theTorBasketTotalLen       = 510;  // cm

TString  SpdCommonGeoMapper::theTorBasketBaseMaterial   = "air"; 
TString  SpdCommonGeoMapper::theTorBasketLeafMaterial   = "bskt_steel_006"; 
TString  SpdCommonGeoMapper::theTorBasketCasingMaterial = "bskt_steel_003"; 

/*==================== TOROIDAL MAGNET =============================*/

Int_t    SpdCommonGeoMapper::theTorMagnetDefGeoType     = 1;    // : 1,2
TString  SpdCommonGeoMapper::theTorMagnetCoilMaterial   = "MCoilSubAir"; 

Double_t SpdCommonGeoMapper::theTorMagnetDistToAxis1    = 13.;  // cm
Double_t SpdCommonGeoMapper::theTorMagnetDistToAxis2    = 65.;  // cm

Double_t SpdCommonGeoMapper::theTorMagnetLen11          = 170.; // cm
Double_t SpdCommonGeoMapper::theTorMagnetSize11         = 120.; // cm

Double_t SpdCommonGeoMapper::theTorMagnetLen12          = 170.; // cm
Double_t SpdCommonGeoMapper::theTorMagnetSize12         = 167;  // cm

Double_t SpdCommonGeoMapper::theTorMagnetLen2           = 500.; // cm
Double_t SpdCommonGeoMapper::theTorMagnetSize2          = 120.; // cm

Double_t SpdCommonGeoMapper::theTorMagnetCoilLen11      = 160.; // cm
Double_t SpdCommonGeoMapper::theTorMagnetCoilLen12      = 160.; // cm

Double_t SpdCommonGeoMapper::theTorMagnetCoilLen2       = 500.; // cm

Double_t SpdCommonGeoMapper::theTorMagnetCoilThickness  = 4.;   // cm
Double_t SpdCommonGeoMapper::theTorMagnetCoilWidth      = 22.;  // cm
Double_t SpdCommonGeoMapper::theTorMagnetCoilRR         = 10.;  // cm

/*=========== TSTB (TOROIDAL TRACKING SYSTEM BARREL) ===============*/

Int_t    SpdCommonGeoMapper::theTsTBDefGeoType          = 1 ;    // : 1,2

TString  SpdCommonGeoMapper::theTsTBBaseMaterial        = "air";

Double_t SpdCommonGeoMapper::theTsTBLen11               = 170.; // cm 
Double_t SpdCommonGeoMapper::theTsTBSize11              = 185.; // cm 
Double_t SpdCommonGeoMapper::theTsTBWidth11             = 120.; // cm 

Double_t SpdCommonGeoMapper::theTsTBLen12               = 170.; // cm 
Double_t SpdCommonGeoMapper::theTsTBSize12              = 185.; // cm 
Double_t SpdCommonGeoMapper::theTsTBWidth12             = 120.; // cm 

Double_t SpdCommonGeoMapper::theTsTBLen2                = 510.; // cm 
Double_t SpdCommonGeoMapper::theTsTBSize2               = 185.; // cm
Double_t SpdCommonGeoMapper::theTsTBWidth2              = 120.; // cm

Double_t SpdCommonGeoMapper::theTsTBLen                 = 236.; // cm 
Double_t SpdCommonGeoMapper::theTsTBSize                =  85.; // cm
Double_t SpdCommonGeoMapper::theTsTBWidth               =  58.; // cm

Bool_t   SpdCommonGeoMapper::theTsTBMakeOffset11        = 1;    //  (true, false)
Bool_t   SpdCommonGeoMapper::theTsTBMakeOffset12        = 1;    //  (true, false)
Bool_t   SpdCommonGeoMapper::theTsTBMakeOffset2         = 1;    //  (true, false)
Bool_t   SpdCommonGeoMapper::theTsTBMakeOffset          = 1;    //  (true, false)

/*========== TSTEC (TOROIDAL TRACKING SYSTEM ENDCAPS) =============*/

Int_t    SpdCommonGeoMapper::theTsTECDefGeoType         = 1; // : 1,2

TString  SpdCommonGeoMapper::theTsTECBaseMaterial       = "air";
TString  SpdCommonGeoMapper::theTsTECLayerMaterial      = "air";

Double_t SpdCommonGeoMapper::theTsTECMinDist1           = 255.; // cm
Double_t SpdCommonGeoMapper::theTsTECMinDist2           = 255.; // cm
Double_t SpdCommonGeoMapper::theTsTECMinDist3           = 0.;   // cm

Double_t SpdCommonGeoMapper::theTsTECSize               = 340.; // cm
Double_t SpdCommonGeoMapper::theTsTECWidth              = 330.; // cm

Double_t SpdCommonGeoMapper::theTsTECLength1            = 32.;  // cm
Double_t SpdCommonGeoMapper::theTsTECLength2            = 32.;  // cm
Double_t SpdCommonGeoMapper::theTsTECLength3            = 0.;  // cm

/*===== ECALTB (TOROIDAL ELECTROMAGNETIC CALORIMETER BARREL) =======*/

Int_t    SpdCommonGeoMapper::theEcalTBDefGeoType        = 1;    // : 1,2

TString  SpdCommonGeoMapper::theEcalTBBaseMaterial      = "ScPb_6535V";

Double_t SpdCommonGeoMapper::theEcalTBLen11             = 170.; // cm 
Double_t SpdCommonGeoMapper::theEcalTBSize11            = 265.; // cm 
Double_t SpdCommonGeoMapper::theEcalTBWidth11           = 60.;  // cm 

Double_t SpdCommonGeoMapper::theEcalTBLen12             = 170.; // cm 
Double_t SpdCommonGeoMapper::theEcalTBSize12            = 265.; // cm 
Double_t SpdCommonGeoMapper::theEcalTBWidth12           = 60.;  // cm 

Double_t SpdCommonGeoMapper::theEcalTBLen2              = 510.; // cm 
Double_t SpdCommonGeoMapper::theEcalTBSize2             = 265.; // cm
Double_t SpdCommonGeoMapper::theEcalTBWidth2            = 60.;  // cm

/*===== ECALTEC (TOROIDAL ELECTROMAGNETIC CALORIMETER ENDCAPS) =====*/

Int_t    SpdCommonGeoMapper::theEcalTECDefGeoType       = 1; // : 1,2

TString  SpdCommonGeoMapper::theEcalTECBaseMaterial     = "ScPb_6535V";

Double_t SpdCommonGeoMapper::theEcalTECMinDist1         = 292.; // cm
Double_t SpdCommonGeoMapper::theEcalTECMinDist2         = 292.; // cm

Double_t SpdCommonGeoMapper::theEcalTECSize             = 340.; // cm
Double_t SpdCommonGeoMapper::theEcalTECWidth            = 330.; // cm
Double_t SpdCommonGeoMapper::theEcalTECThickness        = 58.;  // cm

/*============ RSTB (TOROIDAL RANGE SYSTEM BARREL) =================*/

Int_t    SpdCommonGeoMapper::theRsTBDefGeoType          = 1;    // : 1,2

TString  SpdCommonGeoMapper::theRsTBBaseMaterial        = "air";

Double_t SpdCommonGeoMapper::theRsTBLen11               = 170.; // cm 
Double_t SpdCommonGeoMapper::theRsTBSize11              = 340.; // cm 
Double_t SpdCommonGeoMapper::theRsTBWidth11             =  60.; // cm 

Double_t SpdCommonGeoMapper::theRsTBLen12               = 170.; // cm 
Double_t SpdCommonGeoMapper::theRsTBSize12              = 340.; // cm 
Double_t SpdCommonGeoMapper::theRsTBWidth12             =  60.; // cm 

Double_t SpdCommonGeoMapper::theRsTBLen2                = 510.; // cm 
Double_t SpdCommonGeoMapper::theRsTBSize2               = 340.; // cm
Double_t SpdCommonGeoMapper::theRsTBWidth2              =  60.; // cm

/*============ RSTEC (TOROIDAL RANGE SYSTEM ENDCAPS) ===============*/

Int_t    SpdCommonGeoMapper::theRsTECDefGeoType         = 1; // : 1,2

TString  SpdCommonGeoMapper::theRsTECBaseMaterial       = "air";

Double_t SpdCommonGeoMapper::theRsTECMinDist1           = 355.; // cm
Double_t SpdCommonGeoMapper::theRsTECMinDist2           = 355.; // cm

Double_t SpdCommonGeoMapper::theRsTECSize               = 340.; // cm
Double_t SpdCommonGeoMapper::theRsTECWidth              = 330.; // cm
Double_t SpdCommonGeoMapper::theRsTECThickness          = 60.;  // cm

/*==================================================================*/
/*= ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ =*/
/*= +++++++++++++++++++ SOLENOIDAL GEOMETRY ++++++++++++++++++++++ =*/
/*= ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ =*/
/*==================================================================*/
    
/*==================== SOLENOIDAL MAGNET ===========================*/

Int_t    SpdCommonGeoMapper::theSolMagnetDefGeoType     = 1;    // : 1,2
TString  SpdCommonGeoMapper::theSolMagnetBaseMaterial   = "MCoilSubAir";
TString  SpdCommonGeoMapper::theSolMagnetCryoMaterial   = "steel";

Int_t    SpdCommonGeoMapper::theSolMagnetNSections1     = 1;  
Int_t    SpdCommonGeoMapper::theSolMagnetNSections2     = 3;  

Double_t SpdCommonGeoMapper::theSolMagnetTotalLen1      = 510.; // cm
Double_t SpdCommonGeoMapper::theSolMagnetTotalSize1     = 252.; // cm
Double_t SpdCommonGeoMapper::theSolMagnetTotalWidth1    = 30.;  // cm

Double_t SpdCommonGeoMapper::theSolMagnetLen1           = 500.; // cm
Double_t SpdCommonGeoMapper::theSolMagnetSize1          = 248.; // cm
Double_t SpdCommonGeoMapper::theSolMagnetWidth1         = 22.;  // cm

Double_t SpdCommonGeoMapper::theSolMagnetInnerCryoW1    = 0.5;  // cm
Double_t SpdCommonGeoMapper::theSolMagnetOuterCryoW1    = 0.5;  // cm
Double_t SpdCommonGeoMapper::theSolMagnetEndfCryoW1     = 0.5;  // cm

Double_t SpdCommonGeoMapper::theSolMagnetLen2           = 170.; // cm
Double_t SpdCommonGeoMapper::theSolMagnetSize2          = 252.; // cm
Double_t SpdCommonGeoMapper::theSolMagnetWidth2         = 45.;  // cm

/*==================== SOLENOIDAL MAGNET2 ===========================*/

Int_t    SpdCommonGeoMapper::theSolMagnetTwoDefGeoType     = 1;    // : 1,2
TString  SpdCommonGeoMapper::theSolMagnetTwoBaseMaterial   = "MCoilSubAir2"; //density = 1.60
TString  SpdCommonGeoMapper::theSolMagnetTwoCryoMaterial   = "steel";

Int_t    SpdCommonGeoMapper::theSolMagnetTwoNSections     = 10;
 
Double_t SpdCommonGeoMapper::theSolMagnetTwoTotalLen      = 408.0; // cm //changed from 397.6
Double_t SpdCommonGeoMapper::theSolMagnetTwoTotalSize     = 200.4; // cm //changed from 188.4
Double_t SpdCommonGeoMapper::theSolMagnetTwoTotalWidth    = 27.;  // cm //changed from 23

Double_t SpdCommonGeoMapper::theSolMagnetTwoLen           = 40.8; // cm //changed from 39.76
Double_t SpdCommonGeoMapper::theSolMagnetTwoSize          = 200.4; // cm //changed from 188.4
Double_t SpdCommonGeoMapper::theSolMagnetTwoWidth         = 27.;  // cm //changed from 23

Double_t SpdCommonGeoMapper::theSolMagnetTwoInnerCryoW    = 0.5;  // cm
Double_t SpdCommonGeoMapper::theSolMagnetTwoOuterCryoW    = 0.5;  // cm
Double_t SpdCommonGeoMapper::theSolMagnetTwoEndfCryoW     = 0.5;  // cm
 

/*========== TSSB (SOLENOIDAL TRACKING SYSTEM BARREL) ==============*/

Int_t    SpdCommonGeoMapper::theTsSBDefGeoType          = 1;  

TString  SpdCommonGeoMapper::theTsSBBaseMaterial        = "air";

Int_t    SpdCommonGeoMapper::theTsSBNSections1          = 1;  

Double_t SpdCommonGeoMapper::theTsSBLen                 = 510.; // cm 
Double_t SpdCommonGeoMapper::theTsSBSize                = 112.; // cm 
Double_t SpdCommonGeoMapper::theTsSBWidth               =  47.; // cm 

/*========= TSSEC (SOLENOIDAL TRACKING SYSTEM ENDCAPS) =============*/

Int_t    SpdCommonGeoMapper::theTsSECDefGeoType         = 1; // : 1

TString  SpdCommonGeoMapper::theTsSECBaseMaterial       = "air";

Double_t SpdCommonGeoMapper::theTsSECMinDist            = 255.; // cm
Double_t SpdCommonGeoMapper::theTsSECSize               = 332.; // cm
Double_t SpdCommonGeoMapper::theTsSECWidth              = 322.; // cm
Double_t SpdCommonGeoMapper::theTsSECThickness          = 32.;  // cm

/*===== ECALSB (SOLENOIDAL ELECTROMAGNETIC CALORIMETER BARREL) =====*/

Int_t    SpdCommonGeoMapper::theEcalSBDefGeoType        = 1; // : 1,2

TString  SpdCommonGeoMapper::theEcalSBBaseMaterial      = "ScPb_6535V";

Int_t    SpdCommonGeoMapper::theEcalSBNSections1        = 1;  
Int_t    SpdCommonGeoMapper::theEcalSBNSections2        = 3;  

Double_t SpdCommonGeoMapper::theEcalSBLen               = 510.; // cm 
Double_t SpdCommonGeoMapper::theEcalSBSize              = 192.; // cm 
Double_t SpdCommonGeoMapper::theEcalSBWidth             = 60.;  // cm 

/*==== ECALSEC (SOLENOIDAL ELECTROMAGNETIC CALORIMETER ENDCAPS) ====*/

Int_t    SpdCommonGeoMapper::theEcalSECDefGeoType       = 1; // : 1

TString  SpdCommonGeoMapper::theEcalSECBaseMaterial     = "ScPb_6535V";

Double_t SpdCommonGeoMapper::theEcalSECMinDist          = 292.; // cm
Double_t SpdCommonGeoMapper::theEcalSECSize             = 332.; // cm
Double_t SpdCommonGeoMapper::theEcalSECWidth            = 322.; // cm
Double_t SpdCommonGeoMapper::theEcalSECThickness        = 58.;  // cm

/*============= RSSB (SOLENOIDAL RANGE SYSTEM BARREL) ==============*/

Int_t    SpdCommonGeoMapper::theRsSBDefGeoType          = 1; // : 1,2

TString  SpdCommonGeoMapper::theRsSBBaseMaterial        = "air";

Int_t    SpdCommonGeoMapper::theRsSBNSections1          = 1;  
Int_t    SpdCommonGeoMapper::theRsSBNSections2          = 3;  

Double_t SpdCommonGeoMapper::theRsSBLen                 = 510.; // cm 
Double_t SpdCommonGeoMapper::theRsSBSize                = 332.; // cm 
Double_t SpdCommonGeoMapper::theRsSBWidth               =  60.; // cm 

/*============ RSSEC (SOLENOIDAL RANGE SYSTEM ENDCAPS) =============*/

Int_t    SpdCommonGeoMapper::theRsSECDefGeoType         = 1; // : 1

TString  SpdCommonGeoMapper::theRsSECBaseMaterial       = "air";

Double_t SpdCommonGeoMapper::theRsSECMinDist            = 355.; // cm
Double_t SpdCommonGeoMapper::theRsSECSize               = 332.; // cm
Double_t SpdCommonGeoMapper::theRsSECWidth              = 322.; // cm
Double_t SpdCommonGeoMapper::theRsSECThickness          = 60.;  // cm

/*==================================================================*/
/*= ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ =*/
/*= ++++++++++++++++++++ HYBRID GEOMETRY +++++++++++++++++++++++++ =*/
/*= ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ =*/
/*==================================================================*/

/*========================= HYBRID MAGNET ==========================*/

Int_t    SpdCommonGeoMapper::theHybMagnetDefGeoType     = 1;    // : 1,2

TString  SpdCommonGeoMapper::theHybMagnetCoilMaterial   = "MCoilSubAir"; 

Double_t SpdCommonGeoMapper::theHybMagnetDistToAxis     = 62.;  // cm
Double_t SpdCommonGeoMapper::theHybMagnetDistToAxis1    = 160.; // cm
Double_t SpdCommonGeoMapper::theHybMagnetDistToAxis2    = 260.; // cm
Double_t SpdCommonGeoMapper::theHybMagnetDistToAxisC    = 44. ; // cm 

Double_t SpdCommonGeoMapper::theHybMagnetLen            = 236.; // cm
Double_t SpdCommonGeoMapper::theHybMagnetSize           = 114.; // cm
Double_t SpdCommonGeoMapper::theHybMagnetCoilLen        = 236.; // cm
Double_t SpdCommonGeoMapper::theHybMagnetRingSize       = 176.; // cm

Double_t SpdCommonGeoMapper::theHybMagnetCoilThickness  = 2.;   // cm
Double_t SpdCommonGeoMapper::theHybMagnetCoilWidth      = 20.;  // cm
Double_t SpdCommonGeoMapper::theHybMagnetCoilRR         = 5.;   // cm

ClassImp(SpdCommonGeoMapper)

//_____________________________________________________________________________
void SpdCommonGeoMapper::SetNGeoSectors(Int_t n)
{
  if (n < 3 || n > 12) {
      cout << "-W- <SpdCommonGeoMapper::SetNGeoSectors> "
           << "This value should be > 2 and < 13" << endl; 
      return;
  }
  theNGeoSectors = n; 
}

//_____________________________________________________________________________
SpdCommonGeoMapper::SpdCommonGeoMapper() 
{
   if (fInstance) {
       Fatal("SpdCommonGeoMapper", "Singleton instance already exists.");
       return;
   }
  
   fInstance = this;
}

//_____________________________________________________________________________
SpdCommonGeoMapper::~SpdCommonGeoMapper() 
{
   if (!thePassives.empty()) {
       map <SpdPassiveModule*, module_info*>::iterator it = thePassives.begin();
       for ( ; it !=  thePassives.end(); it++) delete it->first;
   }
   
   if (!theDetectors.empty()) {
       map <SpdDetector*, module_info*>::iterator it = theDetectors.begin();
       for ( ; it !=  theDetectors.end(); it++) delete it->first;
   }
}

//_____________________________________________________________________________
SpdCommonGeoMapper* SpdCommonGeoMapper::Instance() 
{ 
   if (fInstance) return fInstance; 
   fInstance = new SpdCommonGeoMapper();
   return fInstance;
}

//_____________________________________________________________________________
TGeoVolume* SpdCommonGeoMapper::GetMasterVolume()
{
   if (theMasterVolume) return theMasterVolume;
   if (gGeoManager) theMasterVolume = gGeoManager->GetTopVolume();
   return theMasterVolume;
}

//_____________________________________________________________________________
Int_t SpdCommonGeoMapper::GetMasterGeoLevel()
{
   return 1; 
}

//_____________________________________________________________________________
TString SpdCommonGeoMapper::GetMasterVolumeName()
{
   TGeoVolume* masterv = GetMasterVolume();
   if (!masterv) return "cave";
   return masterv->GetName();
}

//_____________________________________________________________________________
void SpdCommonGeoMapper::OpenGeometry(TString media)
{  
   if (media.IsWhitespace()) media = "media.geo";
   
   Bool_t found = false;
   
   TString medfile;
   while (true) 
   {
      // if absolute path
      if (media.BeginsWith("/") ) {
         if (!gSystem->AccessPathName(media.Data())) { 
             medfile = media;
             found = true; 
             break; 
         }
      }
      
      // if GEOMPATH is set
      TString mpath = getenv("GEOMPATH");
      mpath.ReplaceAll("//","/");
      if (!mpath.IsNull() ) {
          if (!mpath.EndsWith("/")) mpath += "/";
          medfile = mpath + media;
          if (!gSystem->AccessPathName(medfile.Data())) { found = true; break; }
      }
      
      // if file was not found look it in the standard path
      medfile = TString(getenv("VMCWORKDIR"))+"/geometry/";
      medfile += media;
      if (!gSystem->AccessPathName(medfile.Data())) { found = true; break; }
      
      // if file was not found set default 
      medfile = TString(getenv("VMCWORKDIR"))+"/geometry/media.geo";
      cout << "-I- <SpdCommonGeoMapper::OpenGeometry> File " << media << " not found."
           << " Set default media file. " << endl;
      break;
   }
   
   cout << "-I- <SpdCommonGeoMapper::OpenGeometry> Media file: " << medfile << endl;
   
   FairGeoLoader* loader = FairGeoLoader::Instance();
   if (!loader) loader = new FairGeoLoader("TGeo","Geo Loader");
   
   FairGeoInterface* GeoInterface = loader->getGeoInterface();
   GeoInterface->setMediaFile(medfile); 
   GeoInterface->readMedia(); 
}

//_____________________________________________________________________________
TString SpdCommonGeoMapper::GetActualMediaFileName() const 
{
   FairGeoLoader* loader = FairGeoLoader::Instance();
   if (!loader) return "";
   FairGeoInterface* GeoInterface = loader->getGeoInterface();
   return GeoInterface->getMedia()->getInputFile();
}

//_____________________________________________________________________________
void SpdCommonGeoMapper::DefineTorGeometrySet(Int_t type)
{
//    if (type < 0) {
//        cout << "-W- <SpdCommonGeoMapper::DefineTorGeometrySet> "
//             << "No actions will be taken " << endl;
//        return;
//    }
//    
//    if (type > 2) {
//        cout << "-W- <SpdCommonGeoMapper::DefineTorGeometrySet> "
//             << "Unknown geometry set: " << type << endl;
//        return;
//    }
   
   if (!thePassives.empty()) {
       map <SpdPassiveModule*, module_info*>::iterator it = thePassives.begin();
       for ( ; it !=  thePassives.end(); it++) {
            if (it->first->IsGeometryLocked()) {
                cout << "-W- <SpdCommonGeoMapper::DefineTorGeometrySet>"
                     << "Geometry is locked (passives)" << endl;
                return;
            }
       }
   }
   
   if (!theDetectors.empty()) {
       SpdGeoMapper* mapper;
       map <SpdDetector*, module_info*>::iterator it = theDetectors.begin();    
       for ( ; it !=  theDetectors.end(); it++) {
            mapper = it->first->GetMapper();
            if (mapper && mapper->IsGeometryLocked()) {
                cout << "-W- <SpdCommonGeoMapper::DefineTorGeometrySet> "
                     << "Geometry is locked (detectors)" << endl;
                return;
            }
       }
   }
   
   cout << "-I- <SpdCommonGeoMapper::DefineTorGeometrySet> " 
        << " Build geometry set of type: " << type << endl;
        
   theSolMagnetDefGeoType    = 0;
   theTsSBDefGeoType         = 0;
   theTsSECDefGeoType        = 0;
   theEcalSBDefGeoType       = 0; 
   theEcalSECDefGeoType      = 0;
   theRsSBDefGeoType         = 0;
   theRsSECDefGeoType        = 0;  
   
   if (type == 0) {
     
       thePipeDefGeoType         = 1;
       theTorBasketDefGeoType    = 0;
       theTorMagnetDefGeoType    = 0;
       theTsTBDefGeoType         = 0;
       theTsTECDefGeoType        = 0;
       theEcalTBDefGeoType       = 0; 
       theEcalTECDefGeoType      = 0;
       theRsTBDefGeoType         = 0;
       theRsTECDefGeoType        = 0;
   
       return;
   }
   
    if (type == -11) {
     
       thePipeDefGeoType         = 1;
       theTorBasketDefGeoType    = 1;
       theTorMagnetDefGeoType    = 1;
       theTsTBDefGeoType         = 0;
       theTsTECDefGeoType        = 0;
       theEcalTBDefGeoType       = 0; 
       theEcalTECDefGeoType      = 0;
       theRsTBDefGeoType         = 0;
       theRsTECDefGeoType        = 0;
   
       return;
   }
   
   if (type == -21) {
     
       thePipeDefGeoType         = 1;
       theTorBasketDefGeoType    = 0;
       theTorMagnetDefGeoType    = 2;
       theTsTBDefGeoType         = 0;
       theTsTECDefGeoType        = 0;
       theEcalTBDefGeoType       = 0; 
       theEcalTECDefGeoType      = 0;
       theRsTBDefGeoType         = 0;
       theRsTECDefGeoType        = 0;
   
       return;
   }
   
   if (type == 1) {
          
       thePipeDefGeoType         = 1;
       theTorBasketDefGeoType    = 1;
       theTorMagnetDefGeoType    = 1;
       theTsTBDefGeoType         = 1;
       theTsTECDefGeoType        = 1;
       theEcalTBDefGeoType       = 1; 
       theEcalTECDefGeoType      = 1;
       theRsTBDefGeoType         = 1;
       theRsTECDefGeoType        = 1;
       
       return;      
   }
   
   if (type == 2) {
          
       thePipeDefGeoType         = 1;
       theTorBasketDefGeoType    = 1;
       theTorMagnetDefGeoType    = 2;
       theTsTBDefGeoType         = 2;
       theTsTECDefGeoType        = 1;
       theEcalTBDefGeoType       = 2; 
       theEcalTECDefGeoType      = 2;
       theRsTBDefGeoType         = 2;
       theRsTECDefGeoType        = 2;
       
       return;      
   }
   
   if (type == 3) {
       
       SetTorHybridGeoPars();
       return;
   }
   
   cout << "-W- <SpdCommonGeoMapper::DefineTorGeometrySet> "
        << "Unknown geometry set: " << type << endl;
        
   thePipeDefGeoType         = 1;
   
   theTorBasketDefGeoType    = 0;
   theTorMagnetDefGeoType    = 0;
   theTsTBDefGeoType         = 0;
   theTsTECDefGeoType        = 0;
   theEcalTBDefGeoType       = 0; 
   theEcalTECDefGeoType      = 0;
   theRsTBDefGeoType         = 0;
   theRsTECDefGeoType        = 0;  
}
   
//_____________________________________________________________________________
void SpdCommonGeoMapper::DefineSolGeometrySet(Int_t type)
{
//    if (type < 0) {
//        cout << "-W- <SpdCommonGeoMapper::DefineSolGeometrySet> "
//             << "No actions will be taken " << endl;
//        return;
//    }
//    
//    if (type > 2) {
//        cout << "-W- <SpdCommonGeoMapper::DefineSolGeometrySet> "
//             << "Unknown geometry set: " << type << endl;
//        return;
//    }
   
   if (!thePassives.empty()) {
       map <SpdPassiveModule*, module_info*>::iterator it = thePassives.begin();
       for ( ; it !=  thePassives.end(); it++) {
            if (it->first->IsGeometryLocked()) {
                cout << "-W- <SpdCommonGeoMapper::DefineSolGeometrySet>"
                     << "Geometry is locked (passives)" << endl;
                return;
            }
       }
   }
   
   if (!theDetectors.empty()) {
       SpdGeoMapper* mapper;
       map <SpdDetector*, module_info*>::iterator it = theDetectors.begin();    
       for ( ; it !=  theDetectors.end(); it++) {
            mapper = it->first->GetMapper();
            if (mapper && mapper->IsGeometryLocked()) {
                cout << "-W- <SpdCommonGeoMapper::DefineSolGeometrySet> "
                     << "Geometry is locked (detectors)" << endl;
                return;
            }
       }
   }
   
   cout << "-I- <SpdCommonGeoMapper::DefineSolGeometrySet> " 
        << " Define geometry set of type: " << type << endl;
        
   theTorBasketDefGeoType    = 0;     
   theTorMagnetDefGeoType    = 0;
   theTsTBDefGeoType         = 0;
   theTsTECDefGeoType        = 0;
   theEcalTBDefGeoType       = 0; 
   theEcalTECDefGeoType      = 0;
   theRsTBDefGeoType         = 0;
   theRsTECDefGeoType        = 0;       
        
   if (type == 0) {
     
       thePipeDefGeoType         = 1;
       theSolMagnetDefGeoType    = 0;
       theTsSBDefGeoType         = 0;
       theTsSECDefGeoType        = 0;
       theEcalSBDefGeoType       = 0; 
       theEcalSECDefGeoType      = 0;
       theRsSBDefGeoType         = 0;
       theRsSECDefGeoType        = 0;
       
       return;
   }
   
   if (type == -11) {
     
       thePipeDefGeoType         = 1;
       theSolMagnetDefGeoType    = 1;      
       theTsSBDefGeoType         = 0;
       theTsSECDefGeoType        = 0;
       theEcalSBDefGeoType       = 0; 
       theEcalSECDefGeoType      = 0;
       theRsSBDefGeoType         = 0;
       theRsSECDefGeoType        = 0;
       
       return;
   }
   
    if (type == -21) {
     
       thePipeDefGeoType         = 1;
       theSolMagnetDefGeoType    = 2;
       theTsSBDefGeoType         = 0;
       theTsSECDefGeoType        = 0;
       theEcalSBDefGeoType       = 0; 
       theEcalSECDefGeoType      = 0;
       theRsSBDefGeoType         = 0;
       theRsSECDefGeoType        = 0;
       
       return;
   }
   
   if (type == 1) {
          
       thePipeDefGeoType         = 1;
       theSolMagnetDefGeoType    = 1;
       theTsSBDefGeoType         = 1;
       theTsSECDefGeoType        = 1;
       theEcalSBDefGeoType       = 1; 
       theEcalSECDefGeoType      = 1;
       theRsSBDefGeoType         = 1;
       theRsSECDefGeoType        = 1;
       
       return;      
   }
   
   if (type == 2) {
          
       thePipeDefGeoType         = 1;
       theSolMagnetDefGeoType    = 2;
       theTsSBDefGeoType         = 1;
       theTsSECDefGeoType        = 1;
       theEcalSBDefGeoType       = 2; 
       theEcalSECDefGeoType      = 1;
       theRsSBDefGeoType         = 2;
       theRsSECDefGeoType        = 1;
       
       return;      
   }    
   
   if (type == 3) {
       
       SetSolHybridGeoPars();
       return;
   }
   
   cout << "-W- <SpdCommonGeoMapper::DefineSolGeometrySet> "
        << "Unknown geometry set: " << type << endl;
        
   thePipeDefGeoType         = 1;
   
   theSolMagnetDefGeoType    = 0;
   theTsSBDefGeoType         = 0;
   theTsSECDefGeoType        = 0;
   theEcalSBDefGeoType       = 0; 
   theEcalSECDefGeoType      = 0;
   theRsSBDefGeoType         = 0;
   theRsSECDefGeoType        = 0; 
}

//_____________________________________________________________________________
void SpdCommonGeoMapper::DefineHybGeometrySet(Int_t type)
{
//    if (type < 0) {
//        cout << "-W- <SpdCommonGeoMapper::DefineHybGeometrySet> "
//             << "No actions will be taken " << endl;
//        return;
//    }
//    
//    if (type > 2) {
//        cout << "-W- <SpdCommonGeoMapper::DefineHybGeometrySet> "
//             << "Unknown geometry set: " << type << endl;
//        return;
//    }
   
   if (!thePassives.empty()) {
       map <SpdPassiveModule*, module_info*>::iterator it = thePassives.begin();
       for ( ; it !=  thePassives.end(); it++) {
            if (it->first->IsGeometryLocked()) {
                cout << "-W- <SpdCommonGeoMapper::DefineHybGeometrySet>"
                     << "Geometry is locked (passives)" << endl;
                return;
            }
       }
   }
   
   if (!theDetectors.empty()) {
       SpdGeoMapper* mapper;
       map <SpdDetector*, module_info*>::iterator it = theDetectors.begin();    
       for ( ; it !=  theDetectors.end(); it++) {
            mapper = it->first->GetMapper();
            if (mapper && mapper->IsGeometryLocked()) {
                cout << "-W- <SpdCommonGeoMapper::DefineHybGeometrySet> "
                     << "Geometry is locked (detectors)" << endl;
                return;
            }
       }
   }
   
   cout << "-I- <SpdCommonGeoMapper::DefineHybGeometrySet> Type: " << type << endl;
   
   theSolMagnetDefGeoType    = 0;
   theTsSBDefGeoType         = 0;
   theTsSECDefGeoType        = 0;
   theEcalSBDefGeoType       = 0; 
   theEcalSECDefGeoType      = 0;
   theRsSBDefGeoType         = 0;
   theRsSECDefGeoType        = 0;     

   /*ATTENTION*/ SetTorHybridGeoPars(); /*ATTENTION*/
 
   theTsTBMakeOffset11       = kTRUE;     
   theTsTBMakeOffset12       = kFALSE;
   
   /*                                                   */  
   /* !ATTENTION! REDEFINE TSTB  PARAMETERS !ATTENTION! */
   /* !ATTENTION! REDEFINE TSTEC PARAMETERS !ATTENTION! */
   /*                                                   */  
   /* ==== TSTB (TOROIDAL TRACKING SYSTEM BARREL) ====  */
   /* ==== TSTEC (TOROIDAL TRACKING SYSTEM ENDCAPS) ==  */
   /*                                                   */  
    
   /* +++++ TYPE = 1 +++++ */
   
   if (type == 1) 
   {
       theTsTBDefGeoType         = 1;    // TsTB

       theTsTBLen11              = 236.; // (170) cm 
       theTsTBSize11             = 160.; // (185) cm 
       theTsTBWidth11            =  95.; // (120) cm 
            
       theTsTBLen12              = 200.; // (170) cm 
       theTsTBSize12             = 160.; // (185) cm 
       theTsTBWidth12            = 145.; // (120) cm 
       
       //-------------------------------------------
       
       theTsTECDefGeoType        = 0;    // TsTEC
       
       theTsTECLength1           = 0.;   // (255) cm
       theTsTECLength2           = 0.;   // (255) cm
       theTsTECLength3           = 0.;   //   (0) cm
          
       return;
   }
       
   /* +++++ TYPE = 2 +++++ */
   
   if (type == 2) 
   {
       theHybMagnetDefGeoType    = 2;    
   
       theTsTBDefGeoType         = 2;    // TsTB
       
       theTsTBLen2               = 236.; // (510) cm 
       theTsTBSize2              = 160.; // (185) cm 
       theTsTBWidth2             =  95.; // (120) cm 
       
       //-------------------------------------------
           
       theTsTECDefGeoType        = 3;    // TsTEC
       
       theTsTECMinDist1          = 0.;   // (255) cm
       theTsTECMinDist2          = 0.;   // (255) cm
       theTsTECMinDist3          = 118.; //   (0) cm
   
       theTsTECSize              = 170.; // (340) cm
       theTsTECWidth             = 160.; // (330) cm
       
       theTsTECLength1           = 0.;   //  (32) cm
       theTsTECLength2           = 0.;   //  (32) cm
       theTsTECLength3           = 200.; //   (0) cm
                 
       return;
   }
   
   cout << "-W- <SpdCommonGeoMapper::DefineHybGeometrySet> "
        << "Unknown geometry set: " << type << endl;
  
   theTsTBDefGeoType         = 0;
   theTsTECDefGeoType        = 0;
   theEcalTBDefGeoType       = 0; 
   theEcalTECDefGeoType      = 0;
   theRsTBDefGeoType         = 0;
   theRsTECDefGeoType        = 0;      
     
}

//_____________________________________________________________________________
void SpdCommonGeoMapper::DefineQslGeometrySet()
{
   if (!thePassives.empty()) {
       map <SpdPassiveModule*, module_info*>::iterator it = thePassives.begin();
       for ( ; it !=  thePassives.end(); it++) {
            if (it->first->IsGeometryLocked()) {
                cout << "-W- <SpdCommonGeoMapper::DefineQslGeometrySet>"
                     << "Geometry is locked (passives)" << endl;
                return;
            }
       }
   }
   
   if (!theDetectors.empty()) {
       SpdGeoMapper* mapper;
       map <SpdDetector*, module_info*>::iterator it = theDetectors.begin();    
       for ( ; it !=  theDetectors.end(); it++) {
            mapper = it->first->GetMapper();
            if (mapper && mapper->IsGeometryLocked()) {
                cout << "-W- <SpdCommonGeoMapper::DefineQslGeometrySet> "
                     << "Geometry is locked (detectors)" << endl;
                return;
            }
       }
   }
   
   cout << "-I- <SpdCommonGeoMapper::DefineQslGeometrySet> " << endl;
   
   theSolMagnetDefGeoType    = 0;
   theTsSBDefGeoType         = 0;
   theTsSECDefGeoType        = 0;
   theEcalSBDefGeoType       = 0; 
   theEcalSECDefGeoType      = 0;
   theRsSBDefGeoType         = 0;
   theRsSECDefGeoType        = 0;     
  
    /*=================================================================*/
   
   thePipeDefGeoType         = 5;
   
   theTorBasketDefGeoType    = 0;
   theTorMagnetDefGeoType    = 0;
   
   /*========================= MAGNET =================================*/

   theHybMagnetDefGeoType     = 3;    
      
   theHybMagnetDistToAxis     = 40.;  // cm
   theHybMagnetDistToAxis1    = 125.; // cm
   theHybMagnetDistToAxis2    = 185.; // cm
   
   theHybMagnetRingSize       = 129.; // cm
   
   theHybMagnetCoilThickness  = 8.;   // cm
   theHybMagnetCoilWidth      = 10.;  // cm

   /*===================== INNER TRACKING SYSTEM ======================*/

   theItsDefGeoType           = 3;      // : 1
   
   theItsNLayers              = 5;  
   
   theItsMinRadius            = 5.0;    // cm
   theItsMaxRadius            = 26.5;    // (60,TOR) cm
   theItsMaxLength            = 200.;   // cm
   
   /*================== TSTB (TRACKING SYSTEM BARREL) =================*/

   theTsTBDefGeoType         = 3;
   
   theTsTBLen                = 236.; // cm
   theTsTBSize               = 85.0; // cm; size == 86.7*cos(pi/16)
   theTsTBWidth              = 85.0 - 27.0; // cm
   
   theTsTBMakeOffset         = kTRUE;
   
   /*================= TSTEC (TRACKING SYSTEM ENDCAPS) ================*/
  
   theTsTECDefGeoType        = 3;    // TsTEC

   theTsTECMinDist3          = 126.6; // cm
   theTsTECLength3           = 24.0; // cm
   
   theTsTECSize              = 80.; // cm
   theTsTECWidth             = 80.-15.; // cm
      
   /*================== TIME-OF-FLIGHT SYSTEM (BARREL) ================*/
    
   theTofBDefGeoType         = 2;         
    
   /*================== TIME-OF-FLIGHT SYSTEM (ENDCAPS) ===============*/
    
   theTofECDefGeoType        = 2;        
   
   /*===== ECALTB (TOROIDAL ELECTROMAGNETIC CALORIMETER BARREL) =======*/

   theEcalTBDefGeoType       = 2;    
   
   theEcalTBBaseMaterial     = "air";
   
   theEcalTBLen2             = 372.0;  // (510,TOR) cm 
   theEcalTBSize2            = 168.4;  // (265,TOR) cm
   theEcalTBWidth2           = 57.0;   // (60,TOR)  cm

   /*===== ECALTEC (TOROIDAL ELECTROMAGNETIC CALORIMETER ENDCAPS) =====*/

   theEcalTECDefGeoType      = 1; 
   
   theEcalTECBaseMaterial    = "air";

   theEcalTECMinDist1        = 204.0; // (320,HYB) cm

   theEcalTECSize            = 155.1; // 163.4; // (265,HYB) cm
   theEcalTECWidth           = 155.4 - 32.14/2; // (255,HYB) cm
   theEcalTECThickness       = 55.0; //45.;   // (60,HYB)  cm
   
   /*============ RSTB (TOROIDAL RANGE SYSTEM BARREL) =================*/

   theRsTBDefGeoType         = 2;    
   
   theRsTBLen2               = 523.2;  // 493.2 cm 
   theRsTBSize2              = 341.4;  // 331.4 cm
   theRsTBWidth2             = 139.0;  // (60,TOR)  cm

   /*============ RSTEC (TOROIDAL RANGE SYSTEM ENDCAPS) ===============*/

   theRsTECDefGeoType        = 1; 

   theRsTECMinDist1          = 261.6; // 246.6 cm

   theRsTECSize              = 341.4;  // 331.4 cm
   theRsTECWidth             = 331.4;  // 304.4 cm
   theRsTECThickness         = 139.0;  // (110.5,HYB) cm
}

//_____________________________________________________________________________
void SpdCommonGeoMapper::SetTorHybridGeoPars()
{
   cout << "-I- <SpdCommonGeoMapper::DefineTorHybridGeoPars> " << endl;
   
   thePipeDefGeoType         = 4;
   
   theTorBasketDefGeoType    = 0;
   theTorMagnetDefGeoType    = 1;

   /*=========== TSTB (TOROIDAL TRACKING SYSTEM BARREL) ===============*/
   
   theTsTBDefGeoType         = 1;

   theTsTBLen11              = 236.; // (170) cm 
   theTsTBSize11             = 160.; // (185) cm 
   theTsTBWidth11            =  95.; // (120) cm 
       
   theTsTBLen12              = 164.; // (170) cm 
   theTsTBSize12             = 160.; // (185) cm 
   theTsTBWidth12            =  95.; // (120) cm 
   
   /*========== TSTEC (TOROIDAL TRACKING SYSTEM ENDCAPS) =============*/

   theTsTECDefGeoType        = 1;   

   theTsTECMinDist1          = 282.; // (255) cm
   theTsTECMinDist2          = 282.; // (255) cm
   
   theTsTECSize              = 160.; // (340) cm
   theTsTECWidth             = 150.; // (330) cm
   theTsTECLength1           = 36.;  // (32)  cm
   theTsTECLength2           = 36.;  // (32)  cm
   
   /*===== ECALTB (TOROIDAL ELECTROMAGNETIC CALORIMETER BARREL) =======*/

   theEcalTBDefGeoType       = 1;    

   theEcalTBLen11            = 236.; // (170) cm 
   theEcalTBSize11           = 265.; // cm 
   theEcalTBWidth11          = 60.;  // cm 

   theEcalTBLen12            = 202.; // (170) cm 
   theEcalTBSize12           = 265.; // cm 
   theEcalTBWidth12          = 60.;  // cm 

   /*===== ECALTEC (TOROIDAL ELECTROMAGNETIC CALORIMETER ENDCAPS) =====*/

   theEcalTECDefGeoType      = 1; 

   theEcalTECMinDist1        = 320.; // (292) cm

   theEcalTECSize            = 265.; // (340) cm
   theEcalTECWidth           = 255.; // (330) cm
   theEcalTECThickness       = 60.;  // (58)  cm
   
   /*============ RSTB (TOROIDAL RANGE SYSTEM BARREL) =================*/

   theRsTBDefGeoType         = 1;    

   theRsTBLen11              = 236.; // (170) cm 
   theRsTBSize11             = 419.; // (340) cm 
   theRsTBWidth11            = 139.; // (60)  cm 

   theRsTBLen12              = 282.; // (170) cm 
   theRsTBSize12             = 419.; // (340) cm 
   theRsTBWidth12            = 139.; // (60)  cm 

   /*============ RSTEC (TOROIDAL RANGE SYSTEM ENDCAPS) ===============*/

   theRsTECDefGeoType        = 1; 

   theRsTECMinDist1          = 400;  // (355) cm

   theRsTECSize              = 419.; // (340) cm
   theRsTECWidth             = 409.; // (330) cm
   theRsTECThickness         = 110.5; // (60) cm
}

//_____________________________________________________________________________
void SpdCommonGeoMapper::SetSolHybridGeoPars()
{
   cout << "-I- <SpdCommonGeoMapper::DefineSolHybridGeoPars> " << endl;
   
   thePipeDefGeoType         = 4;
   
   /*==================== SOLENOIDAL MAGNET ===========================*/

   theSolMagnetDefGeoType    = 1; 
   
   theSolMagnetNSections1    = 1;
   
   theSolMagnetTotalLen1     = 640.; // (510) cm
   theSolMagnetTotalSize1    = 252.; // cm
   theSolMagnetTotalWidth1   = 30.;  // cm

   theSolMagnetLen1          = 630.; // (500) cm
   theSolMagnetSize1         = 248.; // cm
   theSolMagnetWidth1        = 22.;  // cm

   /*========== TSSB (SOLENOIDAL TRACKING SYSTEM BARREL) ==============*/

   theTsSBDefGeoType         = 1;  

   theTsSBNSections1         = 1;  

   theTsSBLen                = 564.; // (510) cm 
   theTsSBSize               = 112.; // cm 
   theTsSBWidth              =  47.; // cm 

   /*========= TSSEC (SOLENOIDAL TRACKING SYSTEM ENDCAPS) =============*/

   theTsSECDefGeoType        = 1; 

   theTsSECMinDist           = 282.; // (255) cm
   theTsSECSize              = 112.; // (332) cm
   theTsSECWidth             = 102.; // (322) cm
   theTsSECThickness         = 36.;  // (32)  cm
   
   /*===== ECALSB (SOLENOIDAL ELECTROMAGNETIC CALORIMETER BARREL) =====*/

   theEcalSBDefGeoType       = 1; 

   theEcalSBNSections1       = 1;  

   theEcalSBLen              = 640.; // (510) cm 
   theEcalSBSize             = 192.; // cm 
   theEcalSBWidth            = 60.;  // cm 

   /*==== ECALSEC (SOLENOIDAL ELECTROMAGNETIC CALORIMETER ENDCAPS) ====*/

   theEcalSECDefGeoType      = 1; 

   theEcalSECMinDist         = 320.; // (292) cm
   theEcalSECSize            = 192.; // (332) cm
   theEcalSECWidth           = 182.; // (322) cm
   theEcalSECThickness       = 60.;  // (58)  cm
   
   /*============= RSSB (SOLENOIDAL RANGE SYSTEM BARREL) ==============*/

   theRsSBDefGeoType         = 1; 

   theRsSBNSections1         = 1;  

   theRsSBLen                = 800.; // (510) cm 
   theRsSBSize               = 411.; // (332) cm 
   theRsSBWidth              = 139.; // (60)  cm 

   /*============ RSSEC (SOLENOIDAL RANGE SYSTEM ENDCAPS) =============*/

   theRsSECDefGeoType        = 1; 

   theRsSECMinDist           = 400.; // (355) cm
   theRsSECSize              = 411.; // (332) cm
   theRsSECWidth             = 401.; // (322) cm
   theRsSECThickness         = 110.5; // cm
}

//_____________________________________________________________________________
void SpdCommonGeoMapper::AddPassive(SpdPassiveModule* module)
{
   if (!module) return;
 
   cout << "-I- <SpdCommonGeoMapper::AddPassive> " << module->GetName() << endl;
   
   module_info* einfo = new module_info();
   einfo->name_ = module->GetName();
   
   thePassives.insert(pair<SpdPassiveModule*,module_info*>(module,einfo));   
}

//_____________________________________________________________________________
void SpdCommonGeoMapper::AddDetector(SpdDetector* detector)
{ 
   if (!detector) return;
   
   cout << "-I- <SpdCommonGeoMapper::AddDetector> " << detector->GetName() << endl;
   
   module_info* einfo = new module_info();
   einfo->name_ = detector->GetName();
   
   theDetectors.insert(pair<SpdDetector*,module_info*>(detector,einfo));   
}

//_____________________________________________________________________________
FairModule* SpdCommonGeoMapper::SearchForModule(Int_t id)  const 
{ 
  FairModule* module = 0;
  
  if (id < 0) module = SearchForPassive(id);
  else module = SearchForActive(id);
  
  return module; 
}

//_____________________________________________________________________________
SpdPassiveModule* SpdCommonGeoMapper::SearchForPassive(Int_t id) const 
{ 
   if (!(id < 0)) return 0;
     
   if (!thePassives.empty()) {
       map <SpdPassiveModule*, module_info*>::const_iterator it = thePassives.begin();
       for ( ; it !=  thePassives.end(); it++) {
            if (it->first->GetId() == id) return it->first;
       }
   }
 
   return 0;
}

//_____________________________________________________________________________
SpdDetector* SpdCommonGeoMapper::SearchForActive(Int_t id) const 
{ 
   if (id < 0) return 0;
     
   if (!theDetectors.empty()) {
       map <SpdDetector*, module_info*>::const_iterator it = theDetectors.begin();
       for ( ; it !=  theDetectors.end(); it++) {
            if (it->first->GetDetId() == id) return it->first;
       }
   }
   
   return 0; 
}

//_____________________________________________________________________________
void SpdCommonGeoMapper::UnsetMaterials(Bool_t precise, TString option, 
                                        Bool_t active, Bool_t passive)
{
   if (active && !theDetectors.empty()) {
       SpdGeoMapper* mapper;
       map <SpdDetector*, module_info*>::iterator it = theDetectors.begin();    
       for ( ; it !=  theDetectors.end(); it++) {
            mapper = it->first->GetMapper();
            mapper->UnsetMaterials(precise,option);
       }
   }
   
   if (passive && !thePassives.empty()) {
       map <SpdPassiveModule*, module_info*>::iterator it = thePassives.begin();
       for ( ; it !=  thePassives.end(); it++) {
            if (option == "air") it->first->UnsetMaterials("air");
            else {
                if (precise) it->first->UnsetMaterials("vacuum2");
                else it->first->UnsetMaterials("vacuum");
            }
       }
   }
}

//_____________________________________________________________________________
void SpdCommonGeoMapper::SaveEmptyHits(Bool_t savehits)
{  
   if (theDetectors.empty()) return;
   
   map <SpdDetector*, module_info*>::iterator it = theDetectors.begin();    
   for ( ; it !=  theDetectors.end(); it++) it->first->SaveEmptyHits(savehits);
}

//_____________________________________________________________________________
void SpdCommonGeoMapper::ConstructGeometry()
{ 
   if (!thePassives.empty()) {
       map <SpdPassiveModule*, module_info*>::iterator it = thePassives.begin();
       for ( ; it !=  thePassives.end(); it++) {     
            if (!it->first->IsGeometryLocked()) it->first->ConstructGeometry();
            it->second->prefix_ = it->first->GetPrefix();
            it->second->id_ = it->first->GetModId();                           
       }
   }
   
   if (!theDetectors.empty()) {
       SpdGeoMapper* mapper;
       map <SpdDetector*, module_info*>::iterator it = theDetectors.begin();    
       for ( ; it !=  theDetectors.end(); it++) {
            mapper = it->first->GetMapper();
            if (mapper) {
                if (!mapper->IsGeometryLocked()) it->first->ConstructGeometry();           
                it->second->prefix_ = mapper->GetGeoPrefix();;
                it->second->id_ = it->first->GetDetId();             
            }   
       }
   }
}

//_____________________________________________________________________________
void SpdCommonGeoMapper::FillModulesMap()
{
   SpdModulesMap* mmap = SpdModulesMap::Instance();
   mmap->Clear();
   
   mmap->Init(GetMasterGeoLevel(),GetMasterVolumeName());
  
   TString prefix;
   Int_t id;
   
   if (!thePassives.empty()) {
       map <SpdPassiveModule*, module_info*>::iterator it = thePassives.begin();
       for ( ; it != thePassives.end(); it++) {   
                  
            prefix = it->first->GetPrefix();
            id = it->first->GetId();
            
            if (!prefix.IsWhitespace()) mmap->Add(prefix,id);
            
            it->second->prefix_ = prefix;
            it->second->id_ = id;        
       }
   }
     
   if (!theDetectors.empty()) {
       SpdGeoMapper* mapper;
       map <SpdDetector*, module_info*>::iterator it = theDetectors.begin();    
       for ( ; it !=  theDetectors.end(); it++) {
            mapper = it->first->GetMapper();
            
            if (mapper) {        
              
                prefix = mapper->GetGeoPrefix();   
                id = it->first->GetDetId();
                
                if (!prefix.IsWhitespace()) mmap->Add(prefix,id);
             
                it->second->prefix_ = prefix;
                it->second->id_ = id;  
            }   
       }
   }
}

//_____________________________________________________________________________
SpdDetector** SpdCommonGeoMapper::GetListOfDetectors(Int_t& nd)
{
   if (theDetectors.empty()) {
       nd = 0;
       return 0;
   }
  
   nd = Int_t(theDetectors.size());
   SpdDetector** dets = new SpdDetector*[nd];
  
   map <SpdDetector*, module_info*>::iterator itd = theDetectors.begin();
   for (Int_t i(0); itd !=  theDetectors.end(); itd++, i++) dets[i] = itd->first;
   
   return dets;
}

//_____________________________________________________________________________
SpdPassiveModule** SpdCommonGeoMapper::GetListOfPassives(Int_t& np)
{
   if (thePassives.empty()) {
       np = 0;
       return 0;
   }
  
   np = Int_t(thePassives.size());
   SpdPassiveModule** mods = new SpdPassiveModule*[np];
  
   map <SpdPassiveModule*, module_info*>::iterator itp = thePassives.begin();
   for (Int_t i(0); itp !=  thePassives.end(); itp++, i++) mods[i] = itp->first;
   
   return mods;
}

//_____________________________________________________________________________
void SpdCommonGeoMapper::PrintGeometryList() 
{
   TString divider('+',150);
   TString divider2('=',150);
   
   cout << "\n" << divider << endl;
   cout << "<SpdCommonGeoMapper::PrintGeometryList> " << endl;
   cout << divider << endl;
   
   cout << "PASSIVES : " << thePassives.size() << "\n\n";
   
   map <SpdPassiveModule*, module_info*>::iterator itp = thePassives.begin();
   for ( ; itp !=  thePassives.end(); itp++) {
        itp->first->Print("");
   }
   
   cout << divider2 << endl;
   
   cout << "DETECTORS : " << theDetectors.size() << "\n\n";
   
   map <SpdDetector*, module_info*>::iterator itd = theDetectors.begin();
   for ( ; itd !=  theDetectors.end(); itd++) {
       itd->first->Print("");
   }
   
   cout << divider << endl;
}

//_____________________________________________________________________________
void SpdCommonGeoMapper::PrintGeometry()
{
   TString divider('=',150);
   
   cout << "\n" << divider << endl;
   cout << "<SpdCommonGeoMapper::PrintGeometry> " << endl;
   cout << divider << endl;
   
   printf("%5s %5s %5s     %-20s %20s %7s %12s %16s %12s  %s\n\n",
          "N","I","ID","Name","Class","GeoType","V[m^3]","Mass[kg]","D[g/cm^3]","Prefix");
   
   Int_t n(0), m;
   Double_t tcapacity(0), tmass(0);
   Double_t capacity(0), mass(0);
   
   capacity = 0;
   mass = 0;
   m = 0;
   
   SpdPassiveModule* pm;
   
   map <SpdPassiveModule*, module_info*>::iterator itp = thePassives.begin();
   
   cout << "Passives " << " (" << thePassives.size() << "): "<< endl;
   
   for ( ; itp !=  thePassives.end(); itp++) {
        n++; 
        m++;
        pm = itp->first;
        printf("%5d %5d %5d     %-20s %20s %7d %12.6f %16.6f %12.6f  %-s\n", 
               n, m, itp->first->GetModId(),
               itp->second->name_.Data(), pm->ClassName(), pm->GetGeoType(), 
               pm->GetCapacity()*1e-6, pm->GetMass()*1e-3, pm->GetDensity(),
               pm->GetPrefix()
              );
        capacity += pm->GetCapacity();
        mass += pm->GetMass();
   }
   printf("%63s %7s %12.6f %16.6f \n","total:","", capacity*1e-6, mass*1e-3);
   
   tcapacity += capacity;
   tmass += mass;
   
   capacity = 0;
   mass = 0;
   m = 0;
   
   SpdDetector* dm;
   SpdGeoMapper* mapper;
   Int_t geotype;
   TString prefix;
   
   map <SpdDetector*, module_info*>::iterator itd = theDetectors.begin();
   
   cout << "Detectors " << " (" << theDetectors.size() << "): "<< endl;
   
   for ( ; itd !=  theDetectors.end(); itd++) {
        n++; 
        m++;
        dm = itd->first;
        geotype = -1;
        prefix  = "";
        mapper = dm->GetMapper();
        if (mapper) {
            geotype = mapper->GetGeoType();
            prefix = mapper->GetGeoPrefix();
        }
        printf("%5d %5d %5d     %-20s %20s %7d %12.6f %16.6f %12.6f  %-s\n", 
               n, m, itd->first->GetDetId(),
               itd->second->name_.Data(), dm->ClassName(), geotype, 
               dm->GetCapacity()*1e-6, dm->GetMass()*1e-3, dm->GetDensity(),
               prefix.Data()
              );
        capacity += dm->GetCapacity();
        mass += dm->GetMass();
   }
   printf("%63s %7s %12.6f %16.6f \n","total:","", capacity*1e-6, mass*1e-3);
   
   tcapacity += capacity;
   tmass += mass;
   
   cout << "\n";
   printf("%63s %7s %12.6f %16.6f \n","TOTAL:","", tcapacity*1e-6, tmass*1e-3);
   
   cout << divider << endl;
 
}


