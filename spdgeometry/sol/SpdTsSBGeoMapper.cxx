// $Id$
// Author: artur   2018/02/01

//_____________________________________________________________________________
//
// SpdTsSBGeoMapper
//_____________________________________________________________________________

#include "SpdCommonGeoMapper.h"
#include "SpdTsSBGeoMapper.h"
#include "SpdTsBVolPars.h"
#include <TMath.h>

#include <iostream>
using std::cout;
using std::endl;

using std::pair;

using namespace TMath;

ClassImp(SpdTsSBGeoMapper)

SpdTsSBGeoMapper* SpdTsSBGeoMapper::fInstance = 0;

SpdTsSBGeoMapper* SpdTsSBGeoMapper::Instance() 
{ return (fInstance) ? fInstance : new SpdTsSBGeoMapper(); }

SpdTsSBGeoMapper* SpdTsSBGeoMapper::GetMapper() 
{ return fInstance; }
    
//_____________________________________________________________________________
SpdTsSBGeoMapper::SpdTsSBGeoMapper():SpdGeoMapper("TsSB")
{
  if (fInstance) {
      Fatal("SpdTsSBGeoMapper", "Singleton instance already exists.");
      return;
  }
   
  fInstance = this;
  
  fParams = new TObjArray();
  fGeoVolPars = new TObjArray();  
  fGeoTable = new SpdGeoTable(); 
  
  fGeoType = SpdCommonGeoMapper::Instance()->GetTsSBDefGeoType();
  fMasterVolName = SpdCommonGeoMapper::Instance()->GetMasterVolumeName();
  
  fRShift[0] = 0;
  fRShift[1] = 0;
  fRShift[2] = 0;
  
  fZShift[0] = 0;
  fZShift[1] = 0;
  fZShift[2] = 0;
}

//_____________________________________________________________________________
SpdTsSBGeoMapper::SpdTsSBGeoMapper(TString prefix):SpdGeoMapper(prefix)
{
  if (fInstance) {
      Fatal("SpdTsSBGeoMapper", "Singleton instance already exists.");
      return;
  }
   
  fInstance = this;
  
  fParams = new TObjArray();
  fGeoVolPars = new TObjArray();  
  fGeoTable = new SpdGeoTable(); 
  
  fGeoType = SpdCommonGeoMapper::Instance()->GetTsSBDefGeoType();
  fMasterVolName = SpdCommonGeoMapper::Instance()->GetMasterVolumeName();
  
  fRShift[0] = 0;
  fRShift[1] = 0;
  fRShift[2] = 0;
  
  fZShift[0] = 0;
  fZShift[1] = 0;
  fZShift[2] = 0;
}

//_____________________________________________________________________________
SpdTsSBGeoMapper::~SpdTsSBGeoMapper() 
{
   fInstance = 0;
}

//_____________________________________________________________________________
void SpdTsSBGeoMapper::UnsetMaterials(Bool_t precise, TString option)
{
   if (fLockGeometry) {
       cout << "-E- <SpdTsSBGeoMapper::UnsetMaterials> Geometry is locked " << endl;
       return;
   }
   (precise) ? fUnsetMedia = "vacuum2" : fUnsetMedia = "vacuum";
}

//_____________________________________________________________________________
Int_t SpdTsSBGeoMapper::GetNSectors() const
{
   Int_t nsec;
   const SpdParameter* par = GetParameter("TsSBNSectors");
   if (!par) return 0;
   par->Value(nsec);
   return nsec;
}

//_____________________________________________________________________________
Double_t SpdTsSBGeoMapper::GetSecAngle() const
{
   Int_t nsec = GetNSectors();
   return (nsec > 0) ? 360./nsec: 0;
}

//_____________________________________________________________________________
Double_t SpdTsSBGeoMapper::GetSecAngleOver2() const
{
   Int_t nsec = GetNSectors();
   return (nsec > 0) ? 180./nsec: 0;
}

//_____________________________________________________________________________
Double_t SpdTsSBGeoMapper::GetClearance2() const
{
   return 0;
}

//_____________________________________________________________________________
TString SpdTsSBGeoMapper::GetBaseMaterial() const
{
   const SpdParameter* par = GetParameter("TsSBBaseMaterial");
   return (par) ?  par->Value() : "";
}

//_____________________________________________________________________________
TString SpdTsSBGeoMapper::GetBaseStrawMaterial() const
{
   const SpdParameter* par = GetParameter("TsSBBaseStrawMaterial");
   return (par) ?  par->Value() : "";
}

//_____________________________________________________________________________
Int_t SpdTsSBGeoMapper::GetStrawModuleNPacking(Int_t nmodule) const
{
   const SpdParameter* par = GetParameter(Form("TsSBModule%dNPacking",nmodule)); 
   if (!par) return 0;
   Int_t value;
   par->Value(value);
   return value;
}

//_____________________________________________________________________________
Bool_t SpdTsSBGeoMapper::IsGeoTypeDefined(Int_t gtype) const
{ 
  return (gtype > 0 && gtype < 2); 
}

//_____________________________________________________________________________
Bool_t SpdTsSBGeoMapper::InitGeometry(Int_t gtype, Bool_t reinit) 
{ 
   if (fLockGeometry) {
       cout << "-E- <SpdTsSBGeoMapper::InitGeometry> Geometry is locked " << endl;
       return kFALSE;
   }
 
   if (!fParams) return kFALSE;
     
   if (!CheckGeoType(gtype,"TsSBGeoType")) return kFALSE;
    
   static SpdCommonGeoMapper* mapper = SpdCommonGeoMapper::Instance();
   
   SpdParameter* par;
     
   par = GetParameter("TsSBNSectors");
   if (par) { if (reinit) *par = mapper->GetNGeoSectors(); }
   else { fParams->Add(new SpdParameter("TsSBNSectors",mapper->GetNGeoSectors())); } 
  
   par = GetParameter("TsSBBaseMaterial");
   if (par) { if (reinit) *par = mapper->GetTsSBBaseMaterial(); }
   else { fParams->Add(new SpdParameter("TsSBBaseMaterial",mapper->GetTsSBBaseMaterial())); } 
    
   par = GetParameter("TsSBClearance");  
   if (par) { if (reinit) *par = mapper->GetSectorClearance(); }
   else { fParams->Add(new SpdParameter("TsSBClearance",mapper->GetSectorClearance())); } 
       
   //cout << "-I- <SpdTsSBGeoMapper::InitGeometry> reinit: " << reinit << endl;
     
   if (fGeoType == 1) SetParameters_1(reinit);
    
   return kTRUE;
}

//_____________________________________________________________________________
void SpdTsSBGeoMapper::SetParameters_1(Bool_t reinit) // private
{
   static SpdCommonGeoMapper* mapper = SpdCommonGeoMapper::Instance();
  
   SpdParameter* par;
   
   Double_t angle = GetSecAngleOver2()*DegToRad();
   
   Double_t clearance2 = GetClearance2();
   par = GetParameter("TsSBClearance2");  
   if (par) { if (reinit) *par = clearance2; else par->Value(clearance2); }
   else { fParams->Add(new SpdParameter("TsSBClearance2",clearance2,1)); } 
   
   TString mat_straw_base = "arco27030";
   par = GetParameter("TsSBBaseStrawMaterial");
   if (par) { if (reinit) *par = mat_straw_base; else mat_straw_base = par->Value(); }
   else { fParams->Add(new SpdParameter("TsSBBaseStrawMaterial",mat_straw_base)); } 
  
   TString mat_straw_shell = "aluminium";
   par = GetParameter("TsSBStrawShellMaterial");
   if (par) { if (reinit) *par = mat_straw_shell; else mat_straw_shell = par->Value(); }
   else { fParams->Add(new SpdParameter("TsSBStrawShellMaterial",mat_straw_shell)); } 
   
   TString mat_straw_wire = "tungsten";
   par = GetParameter("TsSBStrawWireMaterial");
   if (par) { if (reinit) *par = mat_straw_wire; else mat_straw_wire = par->Value(); }
   else { fParams->Add(new SpdParameter("TsSBStrawWireMaterial",mat_straw_wire)); }
   
   Double_t straw_shell_w = 3.6e-3; // 36 mkm
   par = GetParameter("TsSBStrawShellWidth");
   if (par) { if (reinit) *par = straw_shell_w; else par->Value(straw_shell_w); }
   else { fParams->Add(new SpdParameter("TsSBStrawShellWidth",straw_shell_w)); } 
   
   Double_t straw_wire_r  = 1e-3; // 10 mkm
   par = GetParameter("TsSBStrawWireRadius");
   if (par) { if (reinit) *par = straw_wire_r; else par->Value(straw_wire_r); }
   else { fParams->Add(new SpdParameter("TsSBStrawWireRadius",straw_wire_r)); } 
      
   //------------------------------------
   
   Double_t len = mapper->GetTsSBLen();
   par = GetParameter("TsSBLen");  
   if (par) { if (reinit) *par = len; else par->Value(len); }
   else { fParams->Add(new SpdParameter("TsSBLen",len)); }
    
   Double_t size = mapper->GetTsSBSize();
   par = GetParameter("TsSBSize");  
   if (par) { if (reinit) *par = size; else par->Value(size); }
   else { fParams->Add(new SpdParameter("TsSBSize",size)); }
    
   Double_t width = mapper->GetTsSBWidth();
   par = GetParameter("TsSBWidth");  
   if (par) { if (reinit) *par = width; else par->Value(width); }
   else { fParams->Add(new SpdParameter("TsSBWidth",width)); } 
   
   /* define secondary parameters */
  
   Double_t hmin = size - width;
   par = GetParameter("TsSBHmin");  
   if (par) { *par = hmin; }
   else { fParams->Add(new SpdParameter("TsSBHmin",hmin,1)); }
   
   Double_t lmin = 2*(hmin*Tan(angle) - clearance2);  // < 0 ? 
   par = GetParameter("TsSBHLmin");  
   if (par) { *par = lmin; }
   else { fParams->Add(new SpdParameter("TsSBLmin",lmin,2)); }

   Double_t lmax = 2*(size*Tan(angle) - clearance2);  // < 0 ?
   par = GetParameter("TsSBLmax");  
   if (par) { *par = lmax; }
   else { fParams->Add(new SpdParameter("TsSBLmax",lmax,2)); }
   
   //------------------------------------
   
   fRShift[0] = size - 0.5*width;
   fZShift[0] = 0;

   /* ++++++++++++++++++++++++++++++++++++++++ */
   /* ++++++++++++ DEFINE MODULES ++++++++++++ */ 
   /* ++++++++++++++++++++++++++++++++++++++++ */
   
   SpdTsBStrawModule* module = new SpdTsBStrawModule(AddPrefix("Module1"));
   
   module->SetMedia(GetBaseMaterial());
   module->SetModulePars(len, width, lmin, lmax);
   module->SetActivity(0);
    
   module->SetLineColor(kYellow);
   module->SetFillColor(kYellow);
   module->SetTransparency(30);
   
   /* ATTENTION +++++ INIT MODULE PACKING +++++ ATTENTION */
   
   InitStrawModulePacking(module,1); 

   /* ATTENTION +++++ PACK MODULES +++++ ATTENTION */
   
   module->MakePacking();
   
   /* ATTENTION +++++ Add unique MODULES to the List of VolPars +++++ ATTENTION */
     
   fGeoVolPars->Add(module);
          
   /* ATTENTION +++++ Fill GeoTable (LEVEL, Vol.Name, Mother Vol.Name, First node, Last node) +++++ ATTENTION */
   
   module->SetNCopies(GetNSectors()); // set number of copies "by hand"
   
   fGeoTable->AddTableRec(0,module->GetName(),fMasterVolName,1,module->GetNCopies());
  
   /* -------------------------------------------------------------------------------------- */
   
   TString hash, vname;
   
   TObjArray* Refuse = new TObjArray();
   
   /* ++++++++++++++++++++++++++++++++++++++++ */
   /* ++++++++++++ DEFINE LAYERS ++++++++++++ */
   /* ++++++++++++++++++++++++++++++++++++++++ */

   SetDefaultStrawLayerPars(fGeoType);  // WARNING !!! SET DEFAULT PARMETERS !!! WARNING
  
   std::map<TString,SpdTsBStrawLayer*> table_l;  // set of unique layers
   std::map<TString,SpdTsBStrawLayer*>::iterator it;
   
   SpdTsBStrawLayer* layer;
   
   for (Int_t i(0); i < module->GetNNodes(); i++) {
     
        layer = (SpdTsBStrawLayer*)module->MakeNodeVolPars(i); 
        
        /* ATTENTION +++++ INIT LAYER PACKING +++++ ATTENTION */
        
        InitStrawLayerPacking(1,layer,i); 
        
        hash = layer->HashString();
        it = table_l.find(hash);
        
        if (it != table_l.end()) {
            module->SetVolName(i,it->second->GetName()); // fill module vol. table
            delete layer;
            continue;
        }
        
        vname = AddPrefix(Form("Layer%li",table_l.size()+1));
        
        module->SetVolName(i,vname); // fill module vol. table
        
        layer->SetName(vname);
        layer->SetMedia(GetBaseMaterial());
        layer->SetActivity(0);
        
        layer->SetLineColor(kRed);
        layer->SetFillColor(kRed);
        layer->SetTransparency(30);  
  
        table_l.insert(std::pair<TString,SpdTsBStrawLayer*>(hash,layer));
       
        /* ATTENTION +++++ PACK LAYER +++++ ATTENTION */
        
        layer->MakePacking();
        
        /* ATTENTION +++++ Add unique LAYER to the List of VolPars +++++ ATTENTION */ 

        if (layer->GetNNodes() != 0)  {
            fGeoVolPars->Add(layer);   // WARNING add not empty layers only WARNING	
        }
        else Refuse->Add(layer);
   }
 
   /* ATTENTION +++++ Fill GeoTable (LEVEL, Vol.Name, Mother Vol.Name, First node, Last node) +++++ ATTENTION */
 
   for (it = table_l.begin(); it != table_l.end(); ++it) {
   
        layer = it->second;
	if (!layer) continue;
	
	if (layer->GetNNodes() == 0) continue; // WARNING don't add empty layer to the table WARNING
	
        vname = layer->GetName();

        Int_t nn = module->GetNNodes(vname);
        if (nn < 1) continue; 
       
        fGeoTable->AddTableRec(1, vname, module->GetName(), layer->GetNCopies()+1, nn);
        layer->AddNCopies(nn);                      
   }
 
   /* ++++++++++++++++++++++++++++++++++++++++ */
   /* ++++++++++++ DEFINE STRAWS +++++++++++++ */
   /* ++++++++++++++++++++++++++++++++++++++++ */
   
   std::map<TString,SpdTsBStraw*> table_s;  // set of unique straws
   std::map<TString,SpdTsBStraw*>::iterator it_s;
   
   SpdTsBStraw* straw;
   
   std::map<TString,SpdTsBStraw*> straws;
   
   for (it = table_l.begin(); it != table_l.end(); ++it) {
     
       layer = it->second; 
       
       if (!layer) continue;
       
       for (Int_t i(0); i < layer->GetNNodes(); i++) {
         
            straw = (SpdTsBStraw*)layer->MakeNodeVolPars(i); 
            
            hash = straw->HashString();
            it_s = table_s.find(hash);
                  
            if (it_s != table_s.end()) {
                layer->SetVolName(i,it_s->second->GetName()); // fill layer vol. table
                delete straw;
                continue;
            }
                      
            vname = AddPrefix(Form("Straw%li",table_s.size()+1));
            
            straws.insert(std::pair<TString,SpdTsBStraw*>(vname,straw));    
        
            layer->SetVolName(i,vname); // fill layer vol. table
            
            straw->SetName(vname);
            straw->SetMedia(GetBaseMaterial());
            straw->SetActivity(1);
        
            straw->SetLineColor(kRed);
            straw->SetFillColor(kRed);
            straw->SetTransparency(50);
            
            table_s.insert(std::pair<TString,SpdTsBStraw*>(hash,straw));
            
            /* ATTENTION +++++ Add unique STRAW to the List of VolPars +++++ ATTENTION */ 
         
            fGeoVolPars->Add(straw); 
       }  
   }
   
   for (it = table_l.begin(); it != table_l.end(); ++it) {
     
        layer = it->second; 
	
	if (!layer) continue;
     
        const SpdGeoVolPars_VolNodes& volumes = layer->GetVolNodes();
        SpdGeoVolPars_VolNodes::const_iterator itx = volumes.begin();
   
        for (; itx != volumes.end(); itx++) 
        {
              vname = itx->first;
              
              it_s = straws.find(vname);
              if (it_s == straws.end()) continue;
              
              straw = it_s->second;
              
              Int_t nn = itx->second.size();
              if (nn < 1) continue;
                   
              fGeoTable->AddTableRec(2, vname, layer->GetName(), straw->GetNCopies()+1, nn);
              
              straw->AddNCopies(nn);
        }
   }
   
   /* ++++++++++++++++++++++++++++++++++++++++ */
   /* +++++++++ DEFINE STRAW SHELLS ++++++++++ */
   /* ++++++++++++++++++++++++++++++++++++++++ */
   
   SpdTsBStrawInternal* straw_shell;
   TString shellname;
   Double_t rmin, rmax;
   
   for (it_s = table_s.begin(); it_s != table_s.end(); ++it_s) {
     
        straw = it_s->second; 
        if (!straw) continue;
        
        // ----- make straw shell ----- 
        shellname = straw->GetName();
        shellname += "Shell";
        rmax = straw->GetRadius();
        rmin = rmax - straw_shell_w;
        
        straw_shell = straw->MakeShell(shellname,mat_straw_shell,rmin,rmax);
        straw_shell->SetActivity(0); 
       
        straw_shell->SetLineColor(kMagenta+2);
        straw_shell->SetFillColor(kMagenta+2);
        straw_shell->SetTransparency(60);
            
        fGeoVolPars->Add(straw_shell); // Add Shell to the List of VolPars
        fGeoTable->AddTableRec(3, shellname, straw->GetName(), 1, 1); // Fill GeoTable 
        
        //  ----- make straw wire ----- 
        shellname = straw->GetName();
        shellname += "Wire";
        
        straw_shell = straw->MakeShell(shellname,mat_straw_wire,0,straw_wire_r);
        straw_shell->SetActivity(0);
         
        straw_shell->SetLineColor(kMagenta+5);
        straw_shell->SetFillColor(kMagenta+5);
        straw_shell->SetTransparency(0);
        
        fGeoVolPars->Add(straw_shell); // Add Wire to the List of VolPars
        fGeoTable->AddTableRec(3, shellname, straw->GetName(), 1, 1); // Fill GeoTable 
   }
  
   cout << "-I- <SpdTsSBGeoMapper::SetParameters_1>  Volumes (add/reject): " 
        << fGeoVolPars->GetEntries() << "/" << Refuse->GetEntries() << endl;
   
   Refuse->Delete();
   delete Refuse;
   
   fGeoTable->CloseTable();
}

//_____________________________________________________________________________
void SpdTsSBGeoMapper::SetStrawModulePars(
     Double_t angle /*deg*/, Char_t popt /* 'o' or 'e' */, 
     Double_t size /*cm*/, Double_t gap /*cm*/)
{
   SpdParameter* par;
   TString parname;
   
   Int_t nmodule = 1;

   parname = Form("TsSBModule%dLayerPackAngle",nmodule);
   par = GetParameter(parname);  
   if (par) { *par = angle; }
   else { fParams->Add(new SpdParameter(parname,angle)); } 
   
   parname = Form("TsSBModule%dLayerPackOpt",nmodule);
   par = GetParameter(parname);  
   if (par) { *par = popt; }
   else { fParams->Add(new SpdParameter(parname,popt)); } 
   
   parname = Form("TsSBModule%dLayerPackWidth",nmodule);
   par = GetParameter(parname);  
   if (par) { *par = size; }
   else { fParams->Add(new SpdParameter(parname,size)); } 
   
   parname = Form("TsSBModule%dLayerPackGap",nmodule);
   par = GetParameter(parname);  
   if (par) { *par = gap; }
   else { fParams->Add(new SpdParameter(parname,gap)); } 
}
       
//_____________________________________________________________________________   
Bool_t SpdTsSBGeoMapper::GetStrawModulePars(Int_t nmodule,
       Double_t& angle /*deg*/, Char_t& popt /* 'o' or 'e' */,
       Double_t& size /*cm*/, Double_t& gap /*cm*/) // ->protected
{
   SpdParameter* par;
   TString parname;
   
   // Default settings 
   // WARNING set corresponding values in SpdTsSBGeoMapper::SetDefaultStrawModulePars also

   Double_t angle_def, size_def, gap_def;
   Char_t popt_def;
  
   if (fGeoType == 1) {
     
       if (nmodule == 1) 
       {
           angle_def =  0.;  // degrees
           size_def  =  1.;  // cm
           gap_def   =  0.;  // cm
           popt_def  = 'o'; 
       }
       else 
       {
           cout << "-E- <SpdTsSBGeoMapper::GetStrawModulePars> "
               << " Unknown module number: " << nmodule << endl;
           return kFALSE;
       }
   }
   else 
   {
       cout << "-E- <SpdTsSBGeoMapper::GetStrawModulePars>"
            << " Unknown geometry type: " << fGeoType << endl;
       return kFALSE; 
   }
     
   parname = Form("TsSBModule%dLayerPackAngle",nmodule);
   par = GetParameter(parname); 
   if (par) { par->Value(angle); }
   else { fParams->Add(new SpdParameter(parname,angle_def)); angle = angle_def; } 
   
   parname = Form("TsSBModule%dLayerPackOpt",nmodule);
   par = GetParameter(parname); 
   if (par) { par->Value(popt); }
   else { fParams->Add(new SpdParameter(parname,popt_def)); popt = popt_def; } 
   
   parname = Form("TsSBModule%dLayerPackWidth",nmodule);
   par = GetParameter(parname);  
   if (par) { par->Value(size); }
   else { fParams->Add(new SpdParameter(parname,size_def)); size = size_def; } 
   
   parname = Form("TsSBModule%dLayerPackGap",nmodule);
   par = GetParameter(parname);  
   if (par) { par->Value(gap); }
   else { fParams->Add(new SpdParameter(parname,gap_def)); gap = gap_def; }
   
   return kTRUE;
}

//_____________________________________________________________________________   
void SpdTsSBGeoMapper::SetStrawLayerPars(
     Double_t angle /*deg*/, Char_t popt /* 'o' or 'e' */,
     Double_t size /*cm*/, Double_t gap /*cm*/)
{
   SpdParameter* par;
   TString parname;
   
   Int_t nmodule = 1;
   Int_t npack = 1;

   parname = Form("TsSBModule%dNPacking",nmodule);
   par = GetParameter(parname);  
   
   if (par) { par->Value(npack); *par = ++npack; }
   else { fParams->Add(new SpdParameter(parname,npack)); } 
   
   parname = Form("TsSBModule%dStrawPackAngle%d",nmodule,npack);
   par = GetParameter(parname);  
   if (par) { *par = angle; }
   else { fParams->Add(new SpdParameter(parname,angle)); } 
   
   parname = Form("TsSBModule%dStrawPackOpt%d",nmodule,npack);
   par = GetParameter(parname);  
   if (par) { *par = popt; }
   else { fParams->Add(new SpdParameter(parname,popt)); } 
   
   parname = Form("TsSBModule%dStrawPackWidth%d",nmodule,npack);
   par = GetParameter(parname);  
   if (par) { *par = size; }
   else { fParams->Add(new SpdParameter(parname,size)); } 
   
   parname = Form("TsSBModule%dStrawPackGap%d",nmodule,npack);
   par = GetParameter(parname);  
   if (par) { *par = gap; }
   else { fParams->Add(new SpdParameter(parname,gap)); } 
}
    
//_____________________________________________________________________________    
Bool_t SpdTsSBGeoMapper::GetStrawLayerPars(Int_t nmodule, Int_t npack,
       Double_t& angle /*deg*/, Char_t& popt /* 'o' or 'e' */,
       Double_t& size /*cm*/, Double_t& gap /*cm*/) // ->protected
{
   SpdParameter* par;
   TString parname;
  
   angle = 0;
   size = 0; 
   gap = 0;
   popt = 'e';
   
   Int_t nnpack = GetStrawModuleNPacking(nmodule);
  
   if (npack > nnpack) {
       cout << "-E- <SpdTsSBGeoMapper::GetStrawLayerPars>"
            << " Module straw packing of number " << npack
            << " is not defined. "<< endl;
       return kFALSE;
   }
  
   parname = Form("TsSBModule%dStrawPackAngle%d",nmodule,npack);
   par = GetParameter(parname);  
   if (par) { par->Value(angle); }
   else { 
       cout << "-E- <SpdTsSBGeoMapper::GetStrawLayerPars>"
            << " Module straw packing angle is not defined. "<< endl;
       return kFALSE; 
   } 
   
   parname = Form("TsSBModule%dStrawPackOpt%d",nmodule,npack);
   par = GetParameter(parname);  
   if (par) { par->Value(popt); }
   else { 
       cout << "-E- <SpdTsSBGeoMapper::GetStrawLayerPars>"
            << " Module straw packing angle is not defined. "<< endl;
       return kFALSE; 
   } 
   
   parname = Form("TsSBModule%dStrawPackWidth%d",nmodule,npack);
   par = GetParameter(parname);  
   if (par) { par->Value(size); }
   else { 
       cout << "-E- <SpdTsSBGeoMapper::GetStrawLayerPars>"
            << " Module straw packing size is not defined. "<< endl;
       return kFALSE; 
   } 
   
   parname = Form("TsSBModule%dStrawPackGap%d",nmodule,npack);
   par = GetParameter(parname);  
   if (par) { par->Value(gap); }
   else { 
       cout << "-E- <SpdTsSBGeoMapper::GetStrawLayerPars>"
            << " Module straw packing gap is not defined. "<< endl;
       return kFALSE; 
   } 
   
   return kTRUE;
}

//_____________________________________________________________________________
void SpdTsSBGeoMapper::SetDefaultStrawModulePars(Int_t geotype)
{
   //
   // WARNING set corresponding values in  SpdTsSBGeoMapper::GetStrawModulePars also
   //
   if (geotype == 1) {
       SetStrawModulePars(0., 'e', 1.0, 0);
   }
   else {
       cout << "-W- <SpdTsSBGeoMapper::SetDefaultStrawModulePars>"
            << " Unknown geometry type: " << geotype << endl;
   }
}

//_____________________________________________________________________________
void SpdTsSBGeoMapper::SetDefaultStrawLayerPars(Int_t geotype)
{
   SpdParameter* par;
   TString parname;
   Int_t nnpack;

   if (geotype == 1) {
     
       nnpack = GetStrawModuleNPacking(1);
       
       if (nnpack != 0) {
           //cout << "-I- <SpdTsSBGeoMapper::GetStrawModulePars>"
           //     << " Straw layer packing for module " << 1 
           //     << " is already defined. "<< endl;
           return;   
       }
       
       SetStrawLayerPars( 90., 'e', 1.0, 0.);
       SetStrawLayerPars( 90., 'o', 1.0, 0.);
       SetStrawLayerPars( 45., 'e', 1.0, 0.);
       SetStrawLayerPars( 45., 'o', 1.0, 0.);
       SetStrawLayerPars(  0., 'e', 1.0, 0.); 
       SetStrawLayerPars(  0., 'o', 1.0, 0.); 
       SetStrawLayerPars(-45., 'e', 1.0, 0.);
       SetStrawLayerPars(-45., 'o', 1.0, 0.); 
   }
   else {
      cout << "-W- <SpdTsSBGeoMapper::SetDefaultStrawLayerPars>"
           << " Unknown geometry type: " << geotype << endl;
   }
}

//_____________________________________________________________________________
TGeoMatrix* SpdTsSBGeoMapper::GetNodeMatrix(const SpdGeoTableRec* rec, Int_t loc_i) const
{   
   if (!rec) return 0;
     
   if (rec->GetMotherVolName() != fMasterVolName) {                  
       const SpdGeoVolPars* pars = GetVolPars(rec->GetMotherVolName()); 
       if (!pars) return 0;
       return pars->MakeNodeMatrix(rec->GetVolName(),loc_i);
   }
   
   /* define top level (module) matrix */
   
   TGeoCombiTrans* m = 0;
   
   Double_t angle = GetSecAngle();
   TString mname  = rec->GetVolName();
  
   if (fGeoType == 1) {
    
       if (mname == AddPrefix("Module1")) 
       {
           m = new TGeoCombiTrans(0, fRShift[0], fZShift[0], 0);
           m->RotateZ(angle*loc_i - 90);          
       }     
       else {
           cout << "-E- <SpdTsSBGeoMapper::GetModuleGeoMatrix>"
                << " Unknown module: " << mname << endl;
       }
       
       return m;   
   }
 
   cout << "-E- <SpdTsSBGeoMapper::GetModuleGeoMatrix> "
        << "Unknown geometry type: " << fGeoType << endl;
 
   return m;
}

//_____________________________________________________________________________
Bool_t SpdTsSBGeoMapper::InitStrawModulePacking(SpdGeoVolPars* module, Int_t nmodule)
{
   SpdTsBStrawModule* m = dynamic_cast<SpdTsBStrawModule*>(module);
   if (!m) return kFALSE;
        
   Double_t pangle, pwidth, pgap;
   Char_t popt; 
   
   GetStrawModulePars(nmodule, pangle, popt, pwidth, pgap);
   m->SetPackingPars(pangle, pwidth, pgap, popt);
   
   return kTRUE;
}
   
//_____________________________________________________________________________   
Bool_t SpdTsSBGeoMapper::InitStrawLayerPacking(Int_t nmodule, SpdGeoVolPars* layer, Int_t ilayer)
{
   SpdTsBStrawLayer* l = dynamic_cast<SpdTsBStrawLayer*>(layer);
   if (!l) return kFALSE;
   
   Double_t pangle, pwidth, pgap;
   Char_t popt;
  
   // npack depends on (nmodule, ilayer)
   Int_t np = GetStrawModuleNPacking(nmodule);
   Int_t npack = (ilayer%np)+1;
 
   // module, npack
   GetStrawLayerPars(nmodule, npack, pangle, popt, pwidth, pgap);
   l->SetPackingPars(pangle, pwidth, pgap, popt);
  
   return kTRUE;
}

//_____________________________________________________________________________
void SpdTsSBGeoMapper::Print(Option_t*) const
{
   cout << "-I- <SpdTsSBGeoMapper::Print>" << "\n\n";
   SpdGeoMapper::Print("");
}

//_____________________________________________________________________________
TString SpdTsSBGeoMapper::GetPrintVolParsCap() const
{
   return Form("%3s  %2s  %4s   %9s %9s %9s %9s   %9s %9s  %-20s %-20s %-15s %6s %6s", 
          "I", "A", "NC", 
	  "Length", "Width", "Lmin", "Lmax", 
	  "PAngle", "PShift",
	  "Class", "Name", "Media",
	  "NN", "NV"); 
}



