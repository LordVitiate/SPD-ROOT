// $Id$
// Author: artur   2018/05/29


//_____________________________________________________________________________
//
// SpdTsTBGeoBuilder
//_____________________________________________________________________________

#include <TGeoManager.h>
#include "SpdTsTBGeoBuilder.h"
#include "SpdTsBVolPars.h"

#include <TGeoXtru.h>

#include <iostream>
using std::cout;
using std::endl;

ClassImp(SpdTsTBGeoBuilder)

SpdTsTBGeoBuilder* SpdTsTBGeoBuilder::fInstance = 0;

SpdTsTBGeoBuilder* SpdTsTBGeoBuilder::Instance() 
{ return (fInstance) ? fInstance : new SpdTsTBGeoBuilder(); }

SpdTsTBGeoBuilder* SpdTsTBGeoBuilder::GetBuilder() 
{ return fInstance; }

//_____________________________________________________________________________
SpdTsTBGeoBuilder::SpdTsTBGeoBuilder():SpdGeoBuilder(),fGeoMapper(0)
{
   if (fInstance) {
       Fatal("-F- <SpdTsTBGeoBuilder::SpdTsTBGeoBuilder>", 
             "Singleton instance already exists.");
       return;
   }
   
   fInstance = this;
   
   //fMaxBuildGeoLevel = 5;
}

//_____________________________________________________________________________
SpdTsTBGeoBuilder::~SpdTsTBGeoBuilder() 
{
   fInstance = 0;
}

//_____________________________________________________________________________
void SpdTsTBGeoBuilder::SetMapper(SpdGeoMapper* mapper) 
{ 
   fGeoMapper = mapper;
}

//_____________________________________________________________________________
SpdGeoTable* SpdTsTBGeoBuilder::GetTable() const 
{ 
   if (!fGeoMapper) return 0; 
   return fGeoMapper->GetGeoTable();
}

//_____________________________________________________________________________
Bool_t SpdTsTBGeoBuilder::ConstructGeometry()
{
   if (fLockGeometry) {
       cout << "-I- <SpdTsTBGeoBuilder::ConstructGeometry>" 
            << " Geometry is constructed " << endl;
       return kTRUE;
   }
   
   if (!fMasterVolume) {
       cout << "-W- <SpdTsTBGeoBuilder::ConstructGeometry> No master volume " << endl;
       return kFALSE;
   }
   
   if (!fGeoMapper) {
       cout << "-W- <SpdTsTBGeoBuilder::ConstructGeometry> No geomapper " << endl;
       return kFALSE;
   }
   
   if (!fGeoMapper->GetGeoTable()) {
       cout << "-W- <SpdTsTBGeoBuilder::ConstructGeometry> No geotable " << endl;
       return kFALSE;
   }
  
   SpdGeoTable* table = fGeoMapper->GetGeoTable();
   
   cout << "-I- <SpdTsTBGeoBuilder::ConstructGeometry> GEOMETRY LEVEL: " 
        << table->GetMaxGeoLevel() << endl;
    
   ConstructVolumes(table);
    
   const SpdGeoTableRec* rec;
   TGeoVolume *mother, *daughter;
   TGeoMatrix* matrix;
  
   TString mname, dname;
   Int_t n, nn, nf;

   Int_t maxgeolevel = table->GetMaxGeoLevel();
 
   for (Int_t level(maxgeolevel-1); level > -1; level--) {  
 
        n = table->GetNRec(level);  
       
        for (Int_t i(0); i<n; i++) {
            
             rec = table->GetRec(level,i);
             
             mname = rec->GetMotherVolName();                    
             mother = GetVolume(mname);               
             if (!mother) continue;
             
             dname = rec->GetVolName();              
             daughter = GetVolume(dname);
             if (!daughter) continue;
             
             if (RejectVolume(dname)) continue;
             
             //if (mname == "cave" && dname == "TsTBModule1") continue;
             //if (mname == "cave" && dname == "TsTBModule2") continue;
             //if (mname == "cave" && dname == "TsTBModule3") continue;
   
             nn = rec->GetNNodes();
             nf = rec->GetFirstNodeNum();
            
             for (Int_t j(0); j<nn; j++) 
             {
                  matrix = fGeoMapper->GetNodeMatrix(rec,j);	  
                  mother->AddNode(daughter, nf++, matrix);
                       
                  //cout << "<SpdTsTBGeoBuilder::ConstructGeometry>"
                  //     << " add node: " << nf-1 << " " << daughter->GetName() 
                  //     << " to " << mother->GetName() << endl;
             }
       }
   }
  
   return kTRUE;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*/////////////////////////////////////////////////////////////////////////////
////////////////////////////// REJECT VOLUMES ///////////////////////////////// 
/////////////////////////////////////////////////////////////////////////////*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//_____________________________________________________________________________
Bool_t SpdTsTBGeoBuilder::IsGeometryChanged() // public
{
   if (fCheckedVolumes.empty()) return kFALSE;  
   std::map< TString, Int_t >::const_iterator it = fCheckedVolumes.begin();
   for (; it != fCheckedVolumes.end(); it++) { if (it->second > 0) return kTRUE; }
   return kTRUE;
}

//_____________________________________________________________________________
Bool_t SpdTsTBGeoBuilder::RejectVolume(TString volname) // protected
{
   if (fRejection.empty()) return kFALSE;
  
   // check if the volume is already checked
   
   std::map< TString, Int_t >::iterator ic = fCheckedVolumes.find(volname);
   if (ic != fCheckedVolumes.end()) return (ic->second > 0);
     
   // search for volume in the rejection table
  
   TString pattern;
   Int_t vnum;
   Bool_t found = kFALSE; 
   
   std::map< TString, std::vector<Int_t> >::iterator it = fRejection.begin();
   for (; it != fRejection.end(); it++) {
   
        if (!volname.Contains(it->first)) continue;
                 
        /* get/check volume number */
        
        pattern = volname;       
        pattern.Remove(0,volname.Index(it->first)+it->first.Length());
  
        if (!pattern.IsDigit()) {
            fCheckedVolumes.insert(std::pair<TString,Int_t>(volname,0));
            return kFALSE;
        }
        
        vnum = pattern.Atoi();
        
        //cout << "-I- <SpdTsTBGeoBuilder::RejectVolume> volume name: " 
        //     << volname << " vnum: " << vnum << endl;
        
        /* if volume number is not defined (first, last numbers == 0), remove them all */
        
        if (it->second[1] == 0) 
        {
            //cout << "<SpdTsTBGeoBuilder::RejectVolume>  \"" << volname << "\"" << endl;
                    
            fCheckedVolumes.insert(std::pair<TString,Int_t>(volname,vnum));
            return kTRUE;
        }
             
        /* check if volume with a such number should be rejected */
  
        Int_t mod = it->second[0];
        Int_t vn = vnum;
        if (mod > 1) {
            vn = vnum % mod;
            if (vn == 0) vn = mod;
        }
  
        if (vn >= it->second[1] && vn <= it->second[2]) {
        
            //cout << "-I- <SpdTsTBGeoBuilder::RejectVolume>  \"" << volname << "\""   
            //     << " n: " << pattern.Atoi() << " " << vnum << endl;
        
            fCheckedVolumes.insert(std::pair<TString,Int_t>(volname,vnum));
            return kTRUE;
        }   
        
        found = kTRUE;
   }
  
   if (found) fCheckedVolumes.insert(std::pair<TString,Int_t>(volname,-vnum));
   else fCheckedVolumes.insert(std::pair<TString,Int_t>(volname,0));
   
   return kFALSE;
}

//_____________________________________________________________________________
Bool_t SpdTsTBGeoBuilder::AddRejection(TString pattern, Int_t vf, Int_t vl, Int_t mod) // public
{  
   if (vl < vf) { Int_t n = vl; vl = vf; vf = n; }  
     
   if (vf < 1) {
      if (vl > 1) { vf = vl; }
      else { vf = 0; vl = 0; mod = 1; }
   }
   
   if (mod < 2) mod = 1;
     
   std::vector<Int_t> v(3);
   
   v[0] = mod;
   v[1] = vf;
   v[2] = vl;
   
   fRejection.insert(std::pair< TString, std::vector<Int_t> > (pattern,v));
   
   cout <<"-I- <SpdTsTBGeoBuilder::AddRejection> Pattern: \"" << pattern << "\""   
        << "; mod: " << mod << "; (f,l): " << vf << " " << vl << endl;  

   return kTRUE;
}

//_____________________________________________________________________________
void  SpdTsTBGeoBuilder::PrintRejection(Int_t opt) const // public
{
   cout << "-I- <SpdTsTBGeoBuilder::PrintRejection> Table size: " << fCheckedVolumes.size() << endl;
  
   if (fCheckedVolumes.empty()) return;
   
   Int_t nm(0), np(0), n(1);
      
   std::map< TString, Int_t >::const_iterator it;
   
   if (opt == 0) {
     
       printf("\n%7s %7s %7s %s \n\n","N","Nacc","Nrej","        Volume name");
       
       for (it = fCheckedVolumes.begin(); it != fCheckedVolumes.end() ; it++) {
         
            if (it->second == 0) continue; 
            
            printf("%7d ",n++);
       
            if (it->second < 0) printf("%7d %7s    +  ",++np,"");
            else printf("%7s %7d   [-] ","",++nm);
            
            printf("  %s\n",it->first.Data());
       }

       return;
   }
   
   std::multimap< Int_t, TString > CheckedVolumesInv;
   TString str;
   n = 0;
   
   for (it = fCheckedVolumes.begin(); it != fCheckedVolumes.end() ; it++) 
   {
        if (it->second == 0) continue;
        if (it->second < 0) str = Form("  +     %s",it->first.Data());
        else str = Form(" [-]    %s",it->first.Data());
        CheckedVolumesInv.insert(std::pair<Int_t,TString>(abs(it->second),str));
   }
   
   printf("\n%7s %7s %7s %s \n\n","N","Nacc","Nrej","        Volume name");
   
   std::multimap< Int_t, TString >::const_iterator its;
   for (its = CheckedVolumesInv.begin(); its != CheckedVolumesInv.end() ; its++) {   
        if (its->second.BeginsWith("  +    ")) {     
            printf("%7d %7d %7s %s\n",++n,++np,"",its->second.Data());
        }
        else {
            printf("%7d %7s %7d %s\n",++n,"",++nm,its->second.Data());
        }
   }
   
   cout << endl;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*/////////////////////////////////////////////////////////////////////////////
////////////////////////////// BUILD VOLUMES ////////////////////////////////// 
/////////////////////////////////////////////////////////////////////////////*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//_____________________________________________________________________________
TGeoVolume* SpdTsTBGeoBuilder::BuildVolume(const SpdGeoVolPars* vpars)
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
       
       //cout << "-W- <SpdTsTBGeoBuilder::BuildVolume> "
       //     << " Volume's \"" << vpars->GetName() << "\" "
       //     << " media has been changed : " << vpars->GetMedia() 
       //     <<" -> " << med->GetName() << endl;
       
       //const_cast<SpdGeoVolPars*>(vpars)->SetActivity(0); // WARNING      
   }
   
   const_cast<SpdGeoVolPars*>(vpars)->SetMedia(med->GetName()); // WARNING
 
   /* ++++++++++ BUILD MODULE ++++++++++ */
   
   if (oname == "SpdTsBStrawModuleH") {
       
       const SpdTsBStrawModuleH* pars = dynamic_cast<const SpdTsBStrawModuleH*>(vpars);
       
       Double_t length = pars->GetLength();
       
       Double_t llbot = 0.5*pars->GetLbot();
       Double_t llmid = 0.5*pars->GetLmid();
       Double_t lltop = 0.5*pars->GetLtop();
       Double_t h  = 0.5*(pars->GetHbot() + pars->GetHtop());
       Double_t dh = 0.5*(pars->GetHbot() - pars->GetHtop());
       Double_t x[6] = {+llbot, +llmid, +lltop, -lltop, -llmid, -llbot};
       Double_t y[6] = {-h,      dh,    +h,     +h,      dh,    -h    };
       
       vol = gGeoManager->MakeXtru(vpars->GetName(), med, 2);
       
       TGeoXtru *xtru = (TGeoXtru*) vol->GetShape();
       xtru->DefinePolygon(6, x, y);
       xtru->DefineSection(0, -0.5*length);
       xtru->DefineSection(1,  0.5*length);
   }
   
   /* ++++++++++ BUILD DLAYER ++++++++++ */
   
   else if (oname == "SpdTsBStrawDLayer") {
         
       const SpdTsBStrawDLayer* pars = dynamic_cast<const SpdTsBStrawDLayer*>(vpars);
       
       Double_t length = pars->GetLength();
       Double_t h = pars->GetHeight();
       Double_t wb = pars->GetWidthBottom();
       Double_t wt = pars->GetWidthTop();
       
       Double_t v[16];
       
       v[0]  = +0.5*wb; v[1]  = -0.5*h;
       v[2]  = -0.5*wb; v[3]  = -0.5*h;
       v[4]  = -0.5*wt; v[5]  = +0.5*h;
       v[6]  = +0.5*wt; v[7]  = +0.5*h;
       
       v[8]  = v[0];    v[9]  = v[1];
       v[10] = v[2];    v[11] = v[3];
       v[12] = v[4];    v[13] = v[5];
       v[14] = v[6];    v[15] = v[7];
       
       vol = gGeoManager->MakeArb8(vpars->GetName(), med, 0.5*length, v);
   }
       
   /* ++++++++++ BUILD STRAW ++++++++++ */
   
   else if (oname == "SpdTsBStraw") {
     
       const SpdTsBStraw* pars = dynamic_cast<const SpdTsBStraw*>(vpars);
       
       Double_t HLength = pars->GetHalfLength();
       Double_t ORadius = pars->GetRadius(); 
      
       vol = gGeoManager->MakeTube(TString(vpars->GetName()), med, 0., ORadius, HLength); 
   }
   
   /* ++++++++++ BUILD STRAW INTERNAL ++++++++++ */
   
   else if (oname == "SpdTsBStrawInternal") {
     
       const SpdTsBStrawInternal* pars = dynamic_cast<const SpdTsBStrawInternal*>(vpars);
       
       Double_t HLength = pars->GetHalfLength();
       Double_t IRadius = pars->GetInnerRadius();
       Double_t ORadius = pars->GetOuterRadius();
       
       vol = gGeoManager->MakeTube(vpars->GetName(), med, IRadius, ORadius, HLength); 
       //cout << "<" << vpars->GetName() << "> " << med->GetName() << " "<<  ORadius << " " << HLength << endl;
       
   }
 
   else {
        cout << "-I- <SpdTsTBGeoBuilder::BuildVolume> " 
             << "Unknown volume type: " << oname << endl;
   }
   
   if (vol) {
       vol->SetLineColor(vpars->GetLineColor());
       vol->SetFillColor(vpars->GetFillColor());
       vol->SetTransparency(vpars->GetTransparency());
   }
   
   return vol; 
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*/////////////////////////////////////////////////////////////////////////////
////////////////////////// GET/SET PARAMETERS ///////////////////////////////// 
/////////////////////////////////////////////////////////////////////////////*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//_____________________________________________________________________________
void SpdTsTBGeoBuilder::FillParametersIn(SpdParSet* params)
{
  if (!params) return; 
  SpdGeoBuilder::FillParametersIn(params);
  
  Int_t nrej = 0;
  if (!fRejection.empty()) nrej = fRejection.size();
 
  params->SetParameter("Builder/Rejections",nrej,-3);
  
  if (nrej < 1) return;
  
  TString volname;
  Int_t mod, vf, vl;
  
  std::map< TString, std::vector<Int_t> >::iterator it = fRejection.begin();
  
  Int_t n(0);
  for (; it != fRejection.end(); it++) {
       volname = it->first;
       mod = it->second[0];
       vf  = it->second[1];
       vl  = it->second[2];
       n++;
       params->SetParameter(Form("Builder/Rejection%dVolName",n),volname,-3);
       params->SetParameter(Form("Builder/Rejection%dVolNumF",n),vf,-3);
       params->SetParameter(Form("Builder/Rejection%dVolNumL",n),vl,-3);
       params->SetParameter(Form("Builder/Rejection%dMod",n),mod,-3);
  }
   
}
    
//_____________________________________________________________________________
void SpdTsTBGeoBuilder::LoadParametersFrom(SpdParSet* params)
{
  if (!params) return; 
  SpdGeoBuilder::LoadParametersFrom(params);
  
  Int_t nrej = 0;
  params->GetParameter("Builder/Rejections",nrej);
  
  if (nrej < 1) return;
  
  TString volname;
  Int_t mod, vf, vl;
  
  for (Int_t n=1; n<=nrej; n++) {
       if (!params->GetParameter(Form("Builder/Rejection%dVolName",n),volname)) continue;
       if (!params->GetParameter(Form("Builder/Rejection%dVolNumF",n),vf)) continue;
       if (!params->GetParameter(Form("Builder/Rejection%dVolNumL",n),vl)) continue;
       if (!params->GetParameter(Form("Builder/Rejection%dMod",n),mod)) continue;
  
       AddRejection(volname,vf,vl,mod);
  }
}



