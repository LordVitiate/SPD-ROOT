// $Id$
// Author: artur   2019/07/04

//_____________________________________________________________________________
//
// SpdTsTECGeoBuilder
//_____________________________________________________________________________

#include <TGeoManager.h>
#include <TGeoPgon.h>
#include <TGeoXtru.h>

#include "SpdTsTECGeoBuilder.h"
#include "SpdTsECVolPars.h"

#include <iostream>
using std::cout;
using std::endl;

ClassImp(SpdTsTECGeoBuilder)

SpdTsTECGeoBuilder* SpdTsTECGeoBuilder::fInstance = 0;

SpdTsTECGeoBuilder* SpdTsTECGeoBuilder::Instance() 
{ return (fInstance) ? fInstance : new SpdTsTECGeoBuilder(); }

SpdTsTECGeoBuilder* SpdTsTECGeoBuilder::GetBuilder() 
{ return fInstance; }


ClassImp(SpdTsTECGeoBuilder)

//_____________________________________________________________________________
SpdTsTECGeoBuilder::SpdTsTECGeoBuilder():SpdGeoBuilder(),fGeoMapper(0)
{
   if (fInstance) {
       Fatal("-F- <SpdTsTECGeoBuilder::SpdTsTECGeoBuilder>", 
             "Singleton instance already exists.");
       return;
   }
   
   fInstance = this;
}

//_____________________________________________________________________________
SpdTsTECGeoBuilder::~SpdTsTECGeoBuilder() 
{
   fInstance = 0;
}

//_____________________________________________________________________________
void SpdTsTECGeoBuilder::SetMapper(SpdGeoMapper* mapper) 
{ 
   fGeoMapper = mapper;
}

//_____________________________________________________________________________
SpdGeoTable* SpdTsTECGeoBuilder::GetTable() const 
{ 
   if (!fGeoMapper) return 0; 
   return fGeoMapper->GetGeoTable();
}

//_____________________________________________________________________________
Bool_t SpdTsTECGeoBuilder::ConstructGeometry()
{
   if (fLockGeometry) {
       cout << "-I- <SpdTsTECGeoBuilder::ConstructGeometry>" 
            << " Geometry is constructed " << endl;
       return kTRUE;
   }
   
   if (!fMasterVolume) {
       cout << "-W- <SpdTsTECGeoBuilder::ConstructGeometry> No master volume " << endl;
       return kFALSE;
   }
   
   if (!fGeoMapper) {
       cout << "-W- <SpdTsTECGeoBuilder::ConstructGeometry> No geomapper " << endl;
       return kFALSE;
   }
   
   if (!fGeoMapper->GetGeoTable()) {
       cout << "-W- <SpdTsTECGeoBuilder::ConstructGeometry> No geotable " << endl;
       return kFALSE;
   }
   
   SpdGeoTable* table = fGeoMapper->GetGeoTable();
   
   cout << "-I- <SpdTsTECGeoBuilder::ConstructGeometry> GEOMETRY LEVEL: " 
        << table->GetMaxGeoLevel() << endl;
    
   ConstructVolumes(table);
    
   const SpdGeoTableRec* rec;
   TGeoVolume *mother, *daughter;
   TGeoMatrix* matrix;
    
   Int_t n, nn, nf;
  
   Int_t maxgeolevel = table->GetMaxGeoLevel();
  
   for (Int_t level(maxgeolevel-1); level > -1; level--) {  
 
        n = table->GetNRec(level);  
       
        for (Int_t i(0); i<n; i++) {
            
             rec = table->GetRec(level,i);
          
             mother = GetVolume(rec->GetMotherVolName());               
             if (!mother) continue;
             
             daughter = GetVolume(rec->GetVolName());
             if (!daughter) continue;
             
             //cout << "<SpdTsTECGeoBuilder::ConstructGeometry>"
             //     << " add node: " << daughter->GetName() 
             //     << " to " << mother->GetName() << endl;
                   
             nn = rec->GetNNodes();
             nf = rec->GetFirstNodeNum();
            
             for (Int_t j(0); j<nn; j++) 
             {
                  matrix = fGeoMapper->GetNodeMatrix(rec,j);      
                  mother->AddNode(daughter, nf++, matrix);
                  
                  //cout << "<SpdTsTECGeoBuilder::ConstructGeometry>"
                  //     << " add node: " << nf-1 << " " << daughter->GetName() 
                  //     << " to " << mother->GetName() << endl;
             }
       }
   }
  
   return kTRUE;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*/////////////////////////////////////////////////////////////////////////////
////////////////////////////// BUILD VOLUMES ////////////////////////////////// 
/////////////////////////////////////////////////////////////////////////////*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//_____________________________________________________________________________
TGeoVolume* SpdTsTECGeoBuilder::BuildVolume(const SpdGeoVolPars* vpars)
{
   if (!vpars) return 0;
   if (!gGeoManager) return 0;
   
   TGeoVolume* vol = 0;
     
   TString oname = vpars->ClassName();
   
   TGeoMedium* med = GetMedia(vpars->GetMedia());  
   
   if (vpars->GetMedia() != med->GetName()) 
   {
       static Bool_t media_is_changed = false;
       if (!media_is_changed) {
           cout << "-W- <SpdTsTBGeoBuilder::BuildVolume> +++++ ALL MATERIALS ARE SET AS -> "
                << med->GetName() << " +++++ " << endl;
       }
       media_is_changed = true;
       
       //cout << "-W- <SpdTsTECGeoBuilder::BuildVolume> "
       //     << " Volume's \"" << vpars->GetName() << "\" "
       //     << " media has been changed : " << vpars->GetMedia() 
       //     <<" -> " << med->GetName() << endl;
            
       //const_cast<SpdGeoVolPars*>(vpars)->SetActivity(0); // WARNING      
   }
   
   const_cast<SpdGeoVolPars*>(vpars)->SetMedia(med->GetName()); // WARNING
   
   /* ++++++++++ BUILD MODULE ++++++++++ */
   
   if (oname == "SpdTsECStrawModulePgon") {
    
       const SpdTsECStrawModulePgon* pars = dynamic_cast<const SpdTsECStrawModulePgon*>(vpars);
       
       Int_t  NSectors = pars->GetNSectors();
       Double_t Length = pars->GetLength(); 
       Double_t Hmin   = pars->GetHmin();
       Double_t Hmax   = pars->GetHmax();
       
       vol = gGeoManager->MakePgon(vpars->GetName(),med,0,360,NSectors,2);
   
       TGeoPgon* pgon = (TGeoPgon*)vol->GetShape();
   
       pgon->DefineSection(0,-0.5*Length, Hmin, Hmax);
       pgon->DefineSection(1, 0.5*Length, Hmin, Hmax);
       
   }
   
   else if (oname == "SpdTsECStrawModuleArb") {
    
       const SpdTsECStrawModuleArb* pars = dynamic_cast<const SpdTsECStrawModuleArb*>(vpars);
       
       Double_t Length = pars->GetLength();
       Double_t H = pars->GetH(); 
       Double_t Lmin = pars->GetLmin();
       Double_t Lmax = pars->GetLmax();
       
       Double_t v[16];
  
       v[0]  =  0.5*Lmin; v[1]  = -0.5*H;
       v[2]  = -0.5*Lmin; v[3]  = -0.5*H;
       v[4]  = -0.5*Lmax; v[5]  =  0.5*H;
       v[6]  =  0.5*Lmax; v[7]  =  0.5*H;
  
       v[8]  =  0.5*Lmin; v[9]  = -0.5*H;
       v[10] = -0.5*Lmin; v[11] = -0.5*H;
       v[12] = -0.5*Lmax; v[13] =  0.5*H;
       v[14] =  0.5*Lmax; v[15] =  0.5*H;
       
       vol = gGeoManager->MakeArb8(vpars->GetName(),med,0.5*Length,v);
   }
   
   else if (oname == "SpdTsECStrawModule") {
    
       const SpdTsECStrawModule* pars = dynamic_cast<const SpdTsECStrawModule*>(vpars);
       
       Double_t Length = pars->GetLength();
       Double_t Rmin = pars->GetRmin(); 
       Double_t Rmax = pars->GetRmax();
       
       vol = gGeoManager->MakeTube(vpars->GetName(), med, Rmin, Rmax, 0.5*Length);
   }
   
   /* ++++++++++ BUILD LAYER ++++++++++ */
   
   else if (oname == "SpdTsECStrawLayer") {
    
       const SpdTsECStrawLayer* pars = dynamic_cast<const SpdTsECStrawLayer*>(vpars);
       
       Double_t Length = pars->GetLength();
       Int_t    NV = pars->GetNVertices();
       const Double_t* Xv = pars->GetXVertices(); 
       const Double_t* Yv = pars->GetYVertices();
  
       vol = gGeoManager->MakeXtru(vpars->GetName(),med,2);
  
       TGeoXtru* xtru = (TGeoXtru*)vol->GetShape();
       xtru->DefinePolygon(NV,Xv,Yv);
       xtru->DefineSection(0,-0.5*Length);
       xtru->DefineSection(1, 0.5*Length);
   }
   
   /* ++++++++++ BUILD STRAW ++++++++++ */
   
   else if (oname == "SpdTsECStraw") {
     
       const SpdTsECStraw* pars = dynamic_cast<const SpdTsECStraw*>(vpars);
       
       Double_t HLength = pars->GetHalfLength();
       Double_t ORadius = pars->GetRadius(); 
      
       vol = gGeoManager->MakeTube(vpars->GetName(), med, 0, ORadius, HLength); 
   }
   
   /* ++++++++++ BUILD STRAW INTWERNAL ++++++++++ */
   
   else if (oname == "SpdTsECStrawInternal") {
     
       const SpdTsECStrawInternal* pars = dynamic_cast<const SpdTsECStrawInternal*>(vpars);
       
       Double_t HLength = pars->GetHalfLength();
       Double_t IRadius = pars->GetInnerRadius();
       Double_t ORadius = pars->GetOuterRadius();
       
       vol = gGeoManager->MakeTube(vpars->GetName(), med, IRadius, ORadius, HLength); 
   }
   
   else {
     
        cout << "-I- <SpdTsTECGeoBuilder::BuildVolume> " 
             << "Unknown volume type: " << oname << endl;
     
   }
   
   if (vol) {
       vol->SetLineColor(vpars->GetLineColor());
       vol->SetFillColor(vpars->GetFillColor());
       vol->SetTransparency(vpars->GetTransparency());
       //vol->SetTransparency(0);
   }
   
   return vol; 
}


