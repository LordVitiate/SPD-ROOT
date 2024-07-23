// $Id$
// Author: artur   2019/03/07


//_____________________________________________________________________________
//
// SpdItsGeoBuilder
//_____________________________________________________________________________


#include <TGeoManager.h>

#include "SpdItsGeoBuilder.h"
#include "SpdItsVolPars.h"
#include "SpdGeoVVolume.h"

#include <iostream>
using std::cout;
using std::endl;

ClassImp(SpdItsGeoBuilder)

SpdItsGeoBuilder* SpdItsGeoBuilder::fInstance = 0;

SpdItsGeoBuilder* SpdItsGeoBuilder::Instance() 
{ return (fInstance) ? fInstance : new SpdItsGeoBuilder(); }

SpdItsGeoBuilder* SpdItsGeoBuilder::GetBuilder() 
{ return fInstance; }

ClassImp(SpdItsGeoBuilder)

//_____________________________________________________________________________
SpdItsGeoBuilder::SpdItsGeoBuilder():SpdGeoBuilder(),fGeoMapper(0),fVid(-1)
{
   if (fInstance) {
       Fatal("-F- <SpdItsGeoBuilder::SpdItsGeoBuilder>", 
             "Singleton instance already exists.");
       return;
   }
   
   fInstance = this;
}

//_____________________________________________________________________________
SpdItsGeoBuilder::~SpdItsGeoBuilder() 
{
   fInstance = 0;
}

//_____________________________________________________________________________
void SpdItsGeoBuilder::SetMapper(SpdGeoMapper* mapper) 
{ 
   fGeoMapper = mapper;
}

//_____________________________________________________________________________
SpdGeoTable* SpdItsGeoBuilder::GetTable() const 
{ 
   if (!fGeoMapper) return 0; 
   return fGeoMapper->GetGeoTable();
}
//_____________________________________________________________________________
Bool_t SpdItsGeoBuilder::ConstructGeometry()
{
   if (fLockGeometry) {
       cout << "-I- <SpdItsGeoBuilder::ConstructGeometry>" 
            << " Geometry is constructed " << endl;
       return kTRUE;
   }
   
   if (!fMasterVolume) {
       cout << "-W- <SpdItsGeoBuilder::ConstructGeometry> No master volume " << endl;
       return kFALSE;
   }
   
   if (!fGeoMapper) {
       cout << "-W- <SpdItsGeoBuilder::ConstructGeometry> No geomapper " << endl;
       return kFALSE;
   }
   
   if (!fGeoMapper->GetGeoTable()) {
       cout << "-W- <SpdItsGeoBuilder::ConstructGeometry> No geotable " << endl;
       return kFALSE;
   }
   
   SpdGeoTable* table = fGeoMapper->GetGeoTable();
   
   cout << "-I- <SpdItsGeoBuilder::ConstructGeometry> GEOMETRY LEVEL: " 
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
             
             //cout << "<SpdItsGeoBuilder::ConstructGeometry>"
             //     << " add node: " << daughter->GetName() 
             //     << " to " << mother->GetName() << endl;
          
             //TString mname = mother->GetName();
             //TString dname = daughter->GetName();

             //if (mname == "cave" && dname == "ItsLayer1") continue;
             //if (mname == "cave" && dname == "ItsLayer2") continue;
             //if (mname == "cave" && dname == "ItsLayer3") continue;
             //if (mname == "cave" && dname == "ItsLayer4") continue;
             //if (mname == "cave" && dname == "ItsLayer5") continue;
                   
             nn = rec->GetNNodes();
             nf = rec->GetFirstNodeNum();
            
             for (Int_t j(0); j<nn; j++) 
             {
                  matrix = fGeoMapper->GetNodeMatrix(rec,j);      
                  mother->AddNode(daughter, nf++, matrix);
                  
                  //cout << "<SpdItsGeoBuilder::ConstructGeometry>"
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
TGeoVolume* SpdItsGeoBuilder::BuildVolume(const SpdGeoVolPars* vpars)
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
       
       cout << "-W- <SpdItsGeoBuilder::BuildVolume> "
            << " Volume's \"" << vpars->GetName() << "\" "
            << " media has been changed : " << vpars->GetMedia() 
            <<" -> " << med->GetName() << endl;
       
       //const_cast<SpdGeoVolPars*>(vpars)->SetActivity(0); // WARNING      
   }
   
   const_cast<SpdGeoVolPars*>(vpars)->SetMedia(med->GetName()); // WARNING
 
   /* ++++++++++ BUILD LAYER ++++++++++ */
   
   if (oname == "SpdItsLayer") {
    
       const SpdItsLayer* pars = dynamic_cast<const SpdItsLayer*>(vpars);
       
       Double_t Length = pars->GetLength();
       Double_t Rmin = pars->GetRmin(); 
       Double_t Rmax = pars->GetRmax();
       
       vol = gGeoManager->MakeTube(vpars->GetName(), med, Rmin, Rmax, 0.5*Length);
   }
   
   /* ++++++++++ BUILD LADDER ++++++++++ */
   
   else if (oname == "SpdItsLadder") {
         
       const SpdItsLadder* pars = dynamic_cast<const SpdItsLadder*>(vpars);
       
       Double_t Length = pars->GetLength();
       Double_t Lphi = pars->GetLphi(); 
       Double_t Lr = pars->GetLr();
          
       vol = gGeoManager->MakeBox(vpars->GetName(), med, 0.5*Lphi, 0.5*Lr, 0.5*Length);
   }
   
   /* ++++++++++ BUILD CHIP ++++++++++ */
   
   else if (oname == "SpdItsChip") {
    
       const SpdItsChip* pars = dynamic_cast<const SpdItsChip*>(vpars);
       
       Double_t Lz   = pars->GetLz();
       Double_t Lphi = pars->GetLphi(); 
       Double_t Lr   = pars->GetLr();
         
       vol = gGeoManager->MakeBox(vpars->GetName(), med, 0.5*Lphi, 0.5*Lr, 0.5*Lz);
   }
   
   /* ++++++++++ BUILD EC-LAYER ++++++++++ */
   
   else if (oname == "SpdItsEClayer") {
    
       const SpdItsEClayer* pars = dynamic_cast<const SpdItsEClayer*>(vpars);
       
       Double_t Length = pars->GetLength();
       Double_t Rmin = pars->GetRmin(); 
       Double_t Rmax = pars->GetRmax();
       
       vol = gGeoManager->MakeTube(vpars->GetName(), med, Rmin, Rmax, 0.5*Length);
   }

   else {
     
        cout << "-I- <SpdItsGeoBuilder::BuildVolume> " 
             << "Unknown volume type: " << oname << endl;
     
   }
   
   if (vol) {
       vol->SetLineColor(vpars->GetLineColor());
       vol->SetFillColor(vpars->GetFillColor());
       vol->SetTransparency(vpars->GetTransparency());
   }
   
   return vol; 
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~ GEOMETRY NAVIGATION ~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

//_____________________________________________________________________________
TGeoNavigator* SpdItsGeoBuilder::SetNodePath(const TString& path)
{
  fVid = -1;
  return SpdGeoBuilder::SetNodePath(path);
}

//_____________________________________________________________________________
TGeoNavigator* SpdItsGeoBuilder::SetNodePath(Long_t detTID)
{
  fVid = -1;
  return SpdGeoBuilder::SetNodePath(detTID);
}

//_____________________________________________________________________________
TGeoNavigator* SpdItsGeoBuilder::SetNodePath(const TString& path, Int_t vid)
{
  fVid = vid;
  return SpdGeoBuilder::SetNodePath(path);
}

//_____________________________________________________________________________
TGeoNavigator* SpdItsGeoBuilder::SetNodePath(Long_t detTID, Int_t vid) 
{
  fVid = vid;
  return SpdGeoBuilder::SetNodePath(detTID);
}
    
//_____________________________________________________________________________
SpdGeoVVolume* SpdItsGeoBuilder::GetNodeVVolume()
{
  if (!fGeoMapper) return 0;
  TGeoVolume* vol = GetNodeVolume();
  return (vol) ? fGeoMapper->GetVVolume(vol->GetName()) : 0;
}

//_____________________________________________________________________________
Bool_t SpdItsGeoBuilder::GetDetectorPosition(Double_t* p)
{
  if (fVid < 0) return SpdGeoBuilder::GetDetectorPosition(p);
 
  TGeoNavigator* ntor = gGeoManager->GetCurrentNavigator();
  if (!ntor) return kFALSE;
  
  SpdGeoVVolume* vvol = GetNodeVVolume();
  if (!vvol) return kFALSE;
    
  Double_t ploc[3] = {0, 0, 0};
 
  vvol->LocalToMaster(fVid,ploc[0],ploc[1],ploc[2]);
  ntor->LocalToMaster(ploc,p);
  
  return kTRUE;
}     

//_____________________________________________________________________________
Bool_t SpdItsGeoBuilder::GlobalToLocalP(const Double_t* gpnt, Double_t* lpnt)
{
  if (fVid < 0) return SpdGeoBuilder::GlobalToLocalP(gpnt,lpnt);
     
  SpdGeoVVolume* vvol = GetNodeVVolume();
  if (!vvol) return kFALSE;
  
  TGeoNavigator* ntor = gGeoManager->GetCurrentNavigator();
  if (!ntor) return kFALSE;
  
  ntor->MasterToLocal(gpnt,lpnt);
  vvol->MasterToLocal(fVid,lpnt[0],lpnt[1],lpnt[2]);
  
  return kTRUE;
}

//_____________________________________________________________________________   
Bool_t SpdItsGeoBuilder::LocalToGlobalP(const Double_t* lpnt, Double_t* gpnt) 
{ 
  if (fVid < 0) return SpdGeoBuilder::LocalToGlobalP(lpnt,gpnt);
  
  SpdGeoVVolume* vvol = GetNodeVVolume();
  if (!vvol) return kFALSE;
  
  TGeoNavigator* ntor = gGeoManager->GetCurrentNavigator();
  if (!ntor) return kFALSE;
  
  Double_t ploc[3] = {lpnt[0], lpnt[1], lpnt[2]};
  
  vvol->LocalToMaster(fVid,ploc[0],ploc[1],ploc[2]);
  ntor->LocalToMaster(ploc,gpnt);
 
  return kTRUE;
}



    
    
