// $Id$
// Author: artur   2018/05/29


//_____________________________________________________________________________
//
// SpdTsSBGeoBuilder
//_____________________________________________________________________________

#include <TGeoManager.h>
#include "SpdTsSBGeoBuilder.h"
#include "SpdTsBVolPars.h"

#include <iostream>
using std::cout;
using std::endl;

ClassImp(SpdTsSBGeoBuilder)

SpdTsSBGeoBuilder* SpdTsSBGeoBuilder::fInstance = 0;

SpdTsSBGeoBuilder* SpdTsSBGeoBuilder::Instance() 
{ return (fInstance) ? fInstance : new SpdTsSBGeoBuilder(); }

SpdTsSBGeoBuilder* SpdTsSBGeoBuilder::GetBuilder() 
{ return fInstance; }

//_____________________________________________________________________________
SpdTsSBGeoBuilder::SpdTsSBGeoBuilder():SpdGeoBuilder(),fGeoMapper(0)
{
   if (fInstance) {
       Fatal("-F- <SpdTsSBGeoBuilder::SpdTsSBGeoBuilder>", 
             "Singleton instance already exists.");
       return;
   }
   
   fInstance = this;
}

//_____________________________________________________________________________
SpdTsSBGeoBuilder::~SpdTsSBGeoBuilder() 
{
   fInstance = 0;
}

//_____________________________________________________________________________
void SpdTsSBGeoBuilder::SetMapper(SpdGeoMapper* mapper) 
{ 
   fGeoMapper = mapper;
}

//_____________________________________________________________________________
SpdGeoTable* SpdTsSBGeoBuilder::GetTable() const 
{ 
   if (!fGeoMapper) return 0; 
   return fGeoMapper->GetGeoTable();
}

//_____________________________________________________________________________
Bool_t SpdTsSBGeoBuilder::ConstructGeometry()
{
   if (fLockGeometry) {
       cout << "-I- <SpdTsSBGeoBuilder::ConstructGeometry>" 
            << " Geometry is constructed " << endl;
       return kTRUE;
   }
   
   if (!fMasterVolume) {
       cout << "-W- <SpdTsSBGeoBuilder::ConstructGeometry> No master volume " << endl;
       return kFALSE;
   }
   
   if (!fGeoMapper) {
       cout << "-W- <SpdTsSBGeoBuilder::ConstructGeometry> No geomapper " << endl;
       return kFALSE;
   }
   
   if (!fGeoMapper->GetGeoTable()) {
       cout << "-W- <SpdTsSBGeoBuilder::ConstructGeometry> No geotable " << endl;
       return kFALSE;
   }
   
   SpdGeoTable* table = fGeoMapper->GetGeoTable();
   
   cout << "-I- <SpdTsSBGeoBuilder::ConstructGeometry> GEOMETRY LEVEL: " 
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
          
             nn = rec->GetNNodes();
             nf = rec->GetFirstNodeNum();
            
             for (Int_t j(0); j<nn; j++) 
             {
                  matrix = fGeoMapper->GetNodeMatrix(rec,j);	  
                  mother->AddNode(daughter, nf++, matrix);
                  
                  //cout << "<SpdTsSBGeoBuilder::ConstructGeometry>"
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
TGeoVolume* SpdTsSBGeoBuilder::BuildVolume(const SpdGeoVolPars* vpars)
{
   if (!vpars) return 0;
   if (!gGeoManager) return 0;
   
   TGeoVolume* vol = 0;
     
   TString oname = vpars->ClassName();
   
   TGeoMedium* med = GetMedia(vpars->GetMedia());  
   
   if (vpars->GetMedia() != med->GetName()) {
//        cout << "-I- <SpdTsSBGeoBuilder::BuildVolume> "
//             << " Volume's \"" << vpars->GetName() << "\" "
//             << " media is changed : " << vpars->GetMedia() 
//             <<" -> " << med->GetName() << endl;
       const_cast<SpdGeoVolPars*>(vpars)->SetActivity(0); // WARNING      
   }
   
   const_cast<SpdGeoVolPars*>(vpars)->SetMedia(med->GetName()); // WARNING
 
   /* ++++++++++ BUILD MODULE ++++++++++ */
   
   if (oname == "SpdTsBStrawModule") {
    
       const SpdTsBStrawModule* pars = dynamic_cast<const SpdTsBStrawModule*>(vpars);
       
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
   
   /* ++++++++++ BUILD LAYER ++++++++++ */
   
   else if (oname == "SpdTsBStrawLayer") {
         
       const SpdTsBStrawLayer* pars = dynamic_cast<const SpdTsBStrawLayer*>(vpars);
       
       Double_t Length = pars->GetLength();
       Double_t H = pars->GetH(); 
       Double_t Lmin = pars->GetLmin();
       Double_t Lmax = pars->GetLmax();
       
       Double_t v[16];

       v[0]  =  0.5*Lmin; v[1]  = -0.5*H;
       v[2]  = -0.5*Lmin; v[3]  = -0.5*H;
       v[4]  = -0.5*Lmin; v[5]  =  0.5*H;
       v[6]  =  0.5*Lmin; v[7]  =  0.5*H;
  
       v[8]  =  0.5*Lmax; v[9]  = -0.5*H;
       v[10] = -0.5*Lmax; v[11] = -0.5*H;
       v[12] = -0.5*Lmax; v[13] =  0.5*H;
       v[14] =  0.5*Lmax; v[15] =  0.5*H;
       
       vol = gGeoManager->MakeArb8(vpars->GetName(), med, 0.5*Length,v);
   }
   
   /* ++++++++++ BUILD STRAW ++++++++++ */
   
   else if (oname == "SpdTsBStraw") {
     
       const SpdTsBStraw* pars = dynamic_cast<const SpdTsBStraw*>(vpars);
       
       Double_t HLength = pars->GetHalfLength();
       Double_t ORadius = pars->GetRadius(); 
      
       vol = gGeoManager->MakeTube(vpars->GetName(), med, 0, ORadius, HLength); 
   }
   
   /* ++++++++++ BUILD STRAW INTWERNAL ++++++++++ */
   
   else if (oname == "SpdTsBStrawInternal") {
     
       const SpdTsBStrawInternal* pars = dynamic_cast<const SpdTsBStrawInternal*>(vpars);
       
       Double_t HLength = pars->GetHalfLength();
       Double_t IRadius = pars->GetInnerRadius();
       Double_t ORadius = pars->GetOuterRadius();
       
       vol = gGeoManager->MakeTube(vpars->GetName(), med, IRadius, ORadius, HLength); 
   }
 
   else {
     
        cout << "-I- <SpdTsSBGeoBuilder::BuildVolume> " 
             << "Unknown volume type: " << oname << endl;
     
   }
   
   vol->SetLineColor(vpars->GetLineColor());
   vol->SetFillColor(vpars->GetFillColor());
   vol->SetTransparency(vpars->GetTransparency());
  
   return vol; 
}





