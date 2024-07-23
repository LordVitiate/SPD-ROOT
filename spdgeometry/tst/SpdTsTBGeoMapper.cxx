// $Id$
// Author: artur   2018/02/01
//         ruslan  2023/02/21

//_____________________________________________________________________________
//
// SpdTsTBGeoMapper
//_____________________________________________________________________________

#include "SpdCommonGeoMapper.h"
#include "SpdTsTBGeoMapper.h"
#include "SpdTsBVolPars.h"

#include <TMath.h>

#include <iostream>

using std::cout;
using std::endl;

using std::pair;

using namespace TMath;

ClassImp(SpdTsTBGeoMapper)

SpdTsTBGeoMapper* SpdTsTBGeoMapper::fInstance = 0;

SpdTsTBGeoMapper* SpdTsTBGeoMapper::Instance() 
{ return (fInstance) ? fInstance : new SpdTsTBGeoMapper(); }

SpdTsTBGeoMapper* SpdTsTBGeoMapper::GetMapper() 
{ return fInstance; }
 
//_____________________________________________________________________________
SpdTsTBGeoMapper::SpdTsTBGeoMapper():SpdGeoMapper("TsTB")
{
  if (fInstance) {
      Fatal("SpdTsTBGeoMapper", "Singleton instance already exists.");
      return;
  }
   
  fInstance = this;
  
  fParams = new TObjArray();
  fGeoVolPars = new TObjArray();  
  fGeoTable = new SpdGeoTable(); 
  
  fGeoType = SpdCommonGeoMapper::Instance()->GetTsTBDefGeoType();
  fMasterVolName = SpdCommonGeoMapper::Instance()->GetMasterVolumeName();
  
  fRShift = 0;
  fZShift = 0;
}

//_____________________________________________________________________________
SpdTsTBGeoMapper::SpdTsTBGeoMapper(TString prefix):SpdGeoMapper(prefix)
{
  if (fInstance) {
      Fatal("SpdTsTBGeoMapper", "Singleton instance already exists.");
      return;
  }
   
  fInstance = this;
  
  fParams = new TObjArray();
  fGeoVolPars = new TObjArray();  
  fGeoTable = new SpdGeoTable(); 
  
  fGeoType = SpdCommonGeoMapper::Instance()->GetTsTBDefGeoType();
  fMasterVolName = SpdCommonGeoMapper::Instance()->GetMasterVolumeName();
  
  fRShift = 0;
  fZShift = 0;
}

//_____________________________________________________________________________
SpdTsTBGeoMapper::~SpdTsTBGeoMapper() 
{
   fInstance = 0;
}

//_____________________________________________________________________________
void SpdTsTBGeoMapper::UnsetMaterials(Bool_t precise, TString option)
{
   if (fLockGeometry) {
       cout << "-E- <SpdTsTBGeoMapper::UnsetMaterials> Geometry is locked " << endl;
       return;
   }
   
        if (option.IsWhitespace()) { fUnsetMedia = "vacuumX1"; }
   else if (option == "base")      { (precise) ? fUnsetMedia = "vacuum2" : fUnsetMedia = "vacuumX1"; }
   else if (option == "vacuum")    { (precise) ? fUnsetMedia = "vacuum2" : fUnsetMedia = "vacuumX1"; }
   else if (option == "air")       { fUnsetMedia = "airX1";    } 
   else                            { fUnsetMedia = option;     }
}

//_____________________________________________________________________________
Int_t SpdTsTBGeoMapper::GetNSectors() const
{
   const SpdParameter* par = GetParameter("TsTBNSectors");
   if (!par) return 0;
   Int_t nsec;
   par->Value(nsec);
   return nsec;
}

//_____________________________________________________________________________
Double_t SpdTsTBGeoMapper::GetSecAngle() const
{
   Int_t nsec = GetNSectors();
   return (nsec > 0) ? 360./nsec: 0;
}

//_____________________________________________________________________________
Double_t SpdTsTBGeoMapper::GetSecAngleOver2() const
{
   Int_t nsec = GetNSectors();
   return (nsec > 0) ? 180./nsec: 0;
}

//_____________________________________________________________________________
Double_t SpdTsTBGeoMapper::GetClearance() const
{
   const SpdParameter* par = GetParameter("TsTBClearance");
   if (!par) return 0;
   
   Double_t clearance;
   par->Value(clearance);
   
   return clearance;
}

//_____________________________________________________________________________
Double_t SpdTsTBGeoMapper::GetClearance2() const
{
   const SpdParameter* par = GetParameter("TsTBClearance");
   if (!par) return 0;
   
   Double_t clearance2;
   par->Value(clearance2);
   
   Double_t angle = GetSecAngleOver2()*DegToRad();
   return clearance2/Cos(angle);
}

//_____________________________________________________________________________
TString SpdTsTBGeoMapper::GetBaseMaterial() const
{
   const SpdParameter* par = GetParameter("TsTBBaseMaterial");
   return (par) ?  par->Value() : "";
}

//_____________________________________________________________________________
TString SpdTsTBGeoMapper::GetBaseStrawMaterial() const
{
   const SpdParameter* par = GetParameter("TsTBBaseStrawMaterial");
   return (par) ?  par->Value() : "";
}

//_____________________________________________________________________________
Int_t SpdTsTBGeoMapper::GetStrawModuleNPacking(Int_t nmodule) const
{
   const SpdParameter* par = GetParameter(Form("TsTBModule%dNPacking",nmodule)); 
   if (!par) return 0;
   Int_t value;
   par->Value(value);
   return value;
}

//_____________________________________________________________________________
Bool_t SpdTsTBGeoMapper::IsGeoTypeDefined(Int_t gtype) const
{ 
  return (gtype == 3);  // 1 and 2 were used for obsolete toroidal versions of geometry 
                        // (now removed from code of this class, but still present in SpdCommonGeoMapper)
}

//_____________________________________________________________________________
Bool_t SpdTsTBGeoMapper::InitGeometry(Int_t gtype, Bool_t reinit) 
{ 
   if (fLockGeometry) {
       cout << "-E- <SpdTsTBGeoMapper::InitGeometry> Geometry is locked " << endl;
       return kFALSE;
   }
 
   if (!fParams) return kFALSE;
   
   if (fGeoTable) fGeoTable->Clear();
   if (fGeoVolPars) fGeoVolPars->Clear();
      
   if (!CheckGeoType(gtype,"TsTBGeoType")) return kFALSE;
    
   static SpdCommonGeoMapper* mapper = SpdCommonGeoMapper::Instance();
     
   SpdParameter* par;
   
   par = GetParameter("TsTBNSectors");
   if (par) { if (reinit) *par = mapper->GetNGeoSectors(); }
   else { fParams->Add(new SpdParameter("TsTBNSectors",mapper->GetNGeoSectors())); } 
  
   par = GetParameter("TsTBBaseMaterial");
   if (par) { if (reinit) *par = mapper->GetTsTBBaseMaterial(); }
   else { fParams->Add(new SpdParameter("TsTBBaseMaterial",mapper->GetTsTBBaseMaterial())); } 
    
   par = GetParameter("TsTBClearance");  
   if (par) { if (reinit) *par = mapper->GetSectorClearance(); }
   else { fParams->Add(new SpdParameter("TsTBClearance",mapper->GetSectorClearance())); } 
   
   if (fGeoType == 3) SetParameters_3(reinit);
   
   return kTRUE;
}


//_____________________________________________________________________________
void SpdTsTBGeoMapper::SetParameters_3(Bool_t reinit) // private
{
   static SpdCommonGeoMapper* mapper = SpdCommonGeoMapper::Instance();
  
   SpdParameter* par;
   
   Double_t angle = GetSecAngleOver2()*DegToRad();
   
   Bool_t offset = mapper->GetTsTBMakeOffset();
   par = GetParameter("TsTBMakeOffset");
   if (par) { if (reinit) *par = offset; else par->Value(offset); }
   else { fParams->Add(new SpdParameter("TsTBMakeOffset", offset)); }
  
   Double_t clearance2 = GetClearance2();
   par = GetParameter("TsTBClearance2");  
   if (par) { if (reinit) *par = clearance2; else par->Value(clearance2); }
   else { fParams->Add(new SpdParameter("TsTBClearance2", clearance2, 1)); } 
   
   Double_t lclearance = offset ? clearance2     : 0;
   Double_t clearance  = offset ? GetClearance() : 0;
   
   TString mat_straw_base = "arco27030";
   par = GetParameter("TsTBBaseStrawMaterial");
   if (par) { if (reinit) *par = mat_straw_base; else mat_straw_base = par->Value(); }
   else { fParams->Add(new SpdParameter("TsTBBaseStrawMaterial", mat_straw_base)); } 
  
   TString mat_straw_shell = "PET";
   par = GetParameter("TsTBStrawShellMaterial");
   if (par) { if (reinit) *par = mat_straw_shell; else mat_straw_shell = par->Value(); }
   else { fParams->Add(new SpdParameter("TsTBStrawShellMaterial", mat_straw_shell)); } 
   
   TString mat_straw_wire = "tungsten";
   par = GetParameter("TsTBStrawWireMaterial");
   if (par) { if (reinit) *par = mat_straw_wire; else mat_straw_wire = par->Value(); }
   else { fParams->Add(new SpdParameter("TsTBStrawWireMaterial", mat_straw_wire)); }
   
   Double_t straw_shell_w = 3.6e-3; // 36 mkm
   par = GetParameter("TsTBStrawShellWidth");
   if (par) { if (reinit) *par = straw_shell_w; else par->Value(straw_shell_w); }
   else { fParams->Add(new SpdParameter("TsTBStrawShellWidth", straw_shell_w)); } 
   
   Double_t straw_wire_r  = 1.5e-3; // 15 mkm
   par = GetParameter("TsTBStrawWireRadius");
   if (par) { if (reinit) *par = straw_wire_r; else par->Value(straw_wire_r); }
   else { fParams->Add(new SpdParameter("TsTBStrawWireRadius", straw_wire_r)); } 
      
   //------------------------------------
   
   // Length along z axis
   Double_t len = mapper->GetTsTBLen();
   par = GetParameter("TsTBLen");  
   if (par) { if (reinit) *par = len; else par->Value(len); }
   else { fParams->Add(new SpdParameter("TsTBLen", len)); }
    
   // Distance from the center to the side of outer border polygon
   Double_t size = mapper->GetTsTBSize();
   par = GetParameter("TsTBSize");  
   if (par) { if (reinit) *par = size; else par->Value(size); }
   else { fParams->Add(new SpdParameter("TsTBSize", size)); }
   
   // Distance between the sides of inner border polygon and outer border polygon
   Double_t width = mapper->GetTsTBWidth();
   par = GetParameter("TsTBWidth");  
   if (par) { if (reinit) *par = width; else par->Value(width); }
   else { fParams->Add(new SpdParameter("TsTBWidth", width)); } 
   
   /* define secondary parameters */
  
   Double_t hmin = size - width;
   par = GetParameter("TsTBHmin");
   if (par) { *par = hmin; }
   else { fParams->Add(new SpdParameter("TsTBHmin", hmin, 1)); }
   
   Double_t Rmin = hmin;
   par = GetParameter("TsTBRmin");  
   if (par) { *par = Rmin; }
   else { fParams->Add(new SpdParameter("TsTBRmin", Rmin, 1)); }
   
   Double_t Rmax = size/Cos(angle/2.);
   par = GetParameter("TsTBRmax");  
   if (par) { *par = Rmax; }
   else { fParams->Add(new SpdParameter("TsTBRmax", Rmax, 1)); }
   
   Double_t lbot = 2*(hmin*Tan(angle) - lclearance);
   par = GetParameter("TsTBLbot");  
   if (par) { *par = lbot; }
   else { fParams->Add(new SpdParameter("TsTBLbot", lbot, 2)); }
   
   Double_t ltop = 2*size*Tan(angle/2.);
   par = GetParameter("TsTBLtop");
   if (par) { *par = ltop; }
   else { fParams->Add(new SpdParameter("TsTBLtop", ltop, 2)); }
   
   Double_t lmid = ltop*(1.+Cos(angle)) - 2.*clearance*Cos(angle);
   par = GetParameter("TsTBLmid");
   if (par) { *par = lmid; }
   else { fParams->Add(new SpdParameter("TsTBLmid", lmid, 2)); }
   
   Double_t hmid = size - (ltop/2. - clearance)*Sin(angle);
   par = GetParameter("TsTBHmid");
   if (par) { *par = hmid; }
   else { fParams->Add(new SpdParameter("TsTBHmid", hmid, 2)); }

   //------------------------------------
   
   fRShift = size - 0.5*width;
   fZShift = 0;

   /* ++++++++++++++++++++++++++++++++++++++++ */
   /* ++++++++++++ DEFINE MODULES ++++++++++++ */ 
   /* ++++++++++++++++++++++++++++++++++++++++ */
   
   SpdTsBStrawModuleH* module = new SpdTsBStrawModuleH(AddPrefix("Module1"));
   
   module->SetMedia(GetBaseMaterial());
   Double_t hbot = hmid - hmin;
   Double_t htop = size - hmid;
   module->SetModulePars(len, hbot, htop, lbot, lmid, ltop);
   module->SetActivity(0);
    
   module->SetLineColor(kYellow);
   module->SetFillColor(kYellow);
   module->SetTransparency(30);
   
   /* ATTENTION +++++ INIT MODULE PACKING +++++ ATTENTION */
   
   Char_t popt; 
   Double_t pwidth, pgap;
   GetStrawModulePars(1, pwidth, pgap, popt);
   module->SetPackingPars(pwidth, pgap, popt);

   /* ATTENTION +++++ PACK MODULES +++++ ATTENTION */
   
   module->MakePacking();
   
   /* ATTENTION +++++ Add unique MODULES to the List of VolPars +++++ ATTENTION */
     
   fGeoVolPars->Add(module);
          
   /* ATTENTION +++++ Fill GeoTable (LEVEL, Vol.Name, Mother Vol.Name, First node, Last node) +++++ ATTENTION */
   
   module->SetNCopies(GetNSectors()); // set number of copies "by hand"
   
   fGeoTable->AddTableRec(0, module->GetName(), fMasterVolName, 1, module->GetNCopies());
  
   /* -------------------------------------------------------------------------------------- */
   
   TString hash, vname;
   
   TObjArray* Refuse = new TObjArray();
   
   /* ++++++++++++++++++++++++++++++++++++++++ */
   /* ++++++++++++ DEFINE DLAYERS ++++++++++++ */
   /* ++++++++++++++++++++++++++++++++++++++++ */
   
   SetStrawLayerPars(fGeoType);  // WARNING !!! SET DEFAULT PARAMETERS !!! WARNING
  
   std::map<TString, SpdTsBStrawDLayer*> table_l;  // set of unique layers
   std::map<TString, SpdTsBStrawDLayer*>::iterator it;
   
   SpdTsBStrawDLayer* layer;
   
   for (Int_t i(0); i < module->GetNNodes(); i++) {
     
        layer = (SpdTsBStrawDLayer*)module->MakeNodeVolPars(i); 
        
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
        
        layer->SetLineColor(kRed+1);
        layer->SetFillColor(kRed+1);
        layer->SetTransparency(50);  
        
        table_l.insert(std::pair<TString,SpdTsBStrawDLayer*>(hash,layer));
       
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
            straw->SetMedia(GetBaseStrawMaterial());
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
  
      
   cout << "-I- <SpdTsTBGeoMapper::SetParameters_3>  Volumes (add/reject): " 
        << fGeoVolPars->GetEntries() << "/" << Refuse->GetEntries() << endl;
   
   Refuse->Delete();
   delete Refuse;
   
   fGeoTable->CloseTable();
}

//_____________________________________________________________________________
void SpdTsTBGeoMapper::SetStrawModulePars(Int_t nmodule, 
     Double_t size /*cm*/, Double_t gap /*cm*/, Char_t popt /* 'o' or 'e' */)
{
   SpdParameter* par;
   TString parname;
   
   parname = Form("TsTBModule%dLayerPackWidth",nmodule);
   par = GetParameter(parname);  
   if (par) { *par = size; }
   else { fParams->Add(new SpdParameter(parname,size)); } 
   
   parname = Form("TsTBModule%dLayerPackGap",nmodule);
   par = GetParameter(parname);  
   if (par) { *par = gap; }
   else { fParams->Add(new SpdParameter(parname,gap)); }
   
   parname = Form("TsTBModule%dLayerPackOpt",nmodule);
   par = GetParameter(parname);  
   if (par) { *par = popt; }
   else { fParams->Add(new SpdParameter(parname,popt)); } 
}

//_____________________________________________________________________________   
Bool_t SpdTsTBGeoMapper::GetStrawModulePars(Int_t nmodule,
       Double_t& size /*cm*/, Double_t& gap /*cm*/, Char_t& popt /* 'o' or 'e' */) // ->protected
{
   SpdParameter* par;
   TString parname;
   
   // Default settings 
   // WARNING set corresponding values in SpdTsTBGeoMapper::SetStrawModulePars also

   Double_t size_def;
   Double_t gap_def;
   Char_t popt_def;
  
   if (fGeoType == 3) {
     
       if (nmodule == 1) 
       {
           size_def =  GetLayerWidthFromStrawDiameter(1.0);
           gap_def  =  0.;
           popt_def = 'o'; 
       }
       else 
       {
           cout << "-E- <SpdTsTBGeoMapper::GetStrawModulePars> "
               << " Unknown module number: " << nmodule << endl;
           return kFALSE;
       }
   }
   else 
   {
       cout << "-E- <SpdTsTBGeoMapper::GetStrawModulePars>"
            << " Unknown geometry type: " << fGeoType << endl;
       return kFALSE; 
   }
     
   parname = Form("TsTBModule%dLayerPackWidth",nmodule);
   par = GetParameter(parname);  
   if (par) { par->Value(size); }
   else { fParams->Add(new SpdParameter(parname,size_def)); size = size_def; } 
   
   parname = Form("TsTBModule%dLayerPackGap",nmodule);
   par = GetParameter(parname);  
   if (par) { par->Value(gap); }
   else { fParams->Add(new SpdParameter(parname,gap_def)); gap = gap_def; }
   
   parname = Form("TsTBModule%dLayerPackOpt",nmodule);
   par = GetParameter(parname); 
   if (par) { par->Value(popt); }
   else { fParams->Add(new SpdParameter(parname,popt_def)); popt = popt_def; } 
   
   return kTRUE;
}

//_____________________________________________________________________________   
void SpdTsTBGeoMapper::SetStrawLayerPars(Int_t nmodule, 
     Double_t angle /*deg*/, Double_t size /*cm*/, 
     Double_t gap /*cm*/, Double_t subgap /*cm*/,
     Char_t popt /* 'o' or 'e' */)
{
   SpdParameter* par;
   TString parname;
   
   Int_t npack = 1;
   
   parname = Form("TsTBModule%dNPacking",nmodule);
   par = GetParameter(parname);  
   if (par) { par->Value(npack); *par = ++npack; }
   else { fParams->Add(new SpdParameter(parname,npack)); } 
   
   parname = Form("TsTBModule%dStrawPackAngle%d",nmodule,npack);
   par = GetParameter(parname);  
   if (par) { *par = angle; }
   else { fParams->Add(new SpdParameter(parname,angle)); } 
   
   parname = Form("TsTBModule%dStrawPackWidth%d",nmodule,npack);
   par = GetParameter(parname);  
   if (par) { *par = size; }
   else { fParams->Add(new SpdParameter(parname,size)); } 
   
   parname = Form("TsTBModule%dStrawPackGap%d",nmodule,npack);
   par = GetParameter(parname);  
   if (par) { *par = gap; }
   else { fParams->Add(new SpdParameter(parname,gap)); } 
   
   parname = Form("TsTBModule%dStrawPackSublayerGap%d",nmodule,npack);
   par = GetParameter(parname);  
   if (par) { *par = subgap; }
   else { fParams->Add(new SpdParameter(parname,gap)); } 
   
   parname = Form("TsTBModule%dStrawPackOpt%d",nmodule,npack);
   par = GetParameter(parname);  
   if (par) { *par = popt; }
   else { fParams->Add(new SpdParameter(parname,popt)); } 
   
}
    
//_____________________________________________________________________________    
Bool_t SpdTsTBGeoMapper::GetStrawLayerPars(Int_t nmodule, Int_t npack,
       Double_t& angle /*deg*/, Double_t& size /*cm*/, 
       Double_t& gap /*cm*/, Double_t& subgap /*cm*/,
       Char_t& popt /* 'o' or 'e' */ ) // ->protected
{
   SpdParameter* par;
   TString parname;
  
   angle = 0;
   size = 0; 
   gap = 0;
   subgap = 0;
   popt = 'e';
   
   Int_t nnpack = GetStrawModuleNPacking(nmodule);
  
   if (npack > nnpack) {
       cout << "-E- <SpdTsTBGeoMapper::GetStrawLayerPars>"
            << " Module straw packing of number " << npack
            << " is not defined. "<< endl;
       return kFALSE;
   }
  
   parname = Form("TsTBModule%dStrawPackAngle%d",nmodule,npack);
   par = GetParameter(parname);  
   if (par) { par->Value(angle); }
   else { 
       cout << "-E- <SpdTsTBGeoMapper::GetStrawLayerPars>"
            << " Module straw packing angle is not defined. "<< endl;
       return kFALSE; 
   } 
   
   parname = Form("TsTBModule%dStrawPackWidth%d",nmodule,npack);
   par = GetParameter(parname);  
   if (par) { par->Value(size); }
   else { 
       cout << "-E- <SpdTsTBGeoMapper::GetStrawLayerPars>"
            << " Module straw packing size is not defined. "<< endl;
       return kFALSE; 
   } 
   
   parname = Form("TsTBModule%dStrawPackGap%d",nmodule,npack);
   par = GetParameter(parname);  
   if (par) { par->Value(gap); }
   else { 
       cout << "-E- <SpdTsTBGeoMapper::GetStrawLayerPars>"
            << " Module straw packing gap is not defined. "<< endl;
       return kFALSE; 
   } 
   
   parname = Form("TsTBModule%dStrawPackSublayerGap%d",nmodule,npack);
   par = GetParameter(parname);  
   if (par) { par->Value(subgap); }
   else { 
       cout << "-E- <SpdTsTBGeoMapper::GetStrawLayerPars>"
            << " Module straw packing sublayer gap is not defined. "<< endl;
       return kFALSE; 
   }
   
   parname = Form("TsTBModule%dStrawPackOpt%d",nmodule,npack);
   par = GetParameter(parname);  
   if (par) { par->Value(popt); }
   else { 
       cout << "-E- <SpdTsTBGeoMapper::GetStrawLayerPars>"
            << " Module straw packing option is not defined. "<< endl;
       return kFALSE; 
   } 
   
   return kTRUE;
}

//_____________________________________________________________________________
void SpdTsTBGeoMapper::SetStrawModulePars(Int_t g)
{
   //
   // WARNING set corresponding values in  SpdTsTBGeoMapper::GetStrawModulePars also
   //
   if (g == 3) {
       SetStrawModulePars(1, GetLayerWidthFromStrawDiameter(1.0), 0., 'o');
   }
   else {
       cout << "-W- <SpdTsTBGeoMapper::SetStrawModulePars>"
            << " Unknown geometry set: " << g << endl;
   }
}

//_____________________________________________________________________________
void SpdTsTBGeoMapper::SetStrawLayerPars(Int_t g)
{
   SpdParameter* par;
   TString parname;
   Int_t nnpack;
  
   if (g == 3) {
     
       Int_t nmodule = 1;  // only 1 module in this geometry type
       nnpack = GetStrawModuleNPacking(nmodule);
       
       if (nnpack != 0) {
           //cout << "-I- <SpdTsTBGeoMapper::GetStrawLayerPars>"
           //     << " Straw layer packing for module " << nmodule
           //     << " is already defined. "<< endl;
           return;   
       }
       
       TString module_name = AddPrefix(Form("Module%d", nmodule));
       SpdGeoVolPars* module = (SpdGeoVolPars*) fGeoVolPars->FindObject(module_name);
       if (!module) {
            cout << "-W- <SpdTsTBGeoMapper::SetStrawLayerPars>"
                 << " " << module_name << " is not found." << endl;
            return;
       }

       Double_t size = 1.;   // straw diameter
       Double_t gap  = 0.;   // gap between straws in the sublayer
       Double_t sgap = 0.;   // gap between the sublayers
       
       Double_t angle_z =  0.;  // straw inclination angle in z layers [degrees]
       Double_t angle_u =  3.;  // straw inclination angle in u layers [degrees]
       Double_t angle_v = -3.;  // straw inclination angle in v layers [degrees]
       std::vector<Double_t> angles;
       Int_t ilayer = 1;
       while (ilayer <= 27) {
           angles.push_back(angle_z);
           angles.push_back(angle_u);
           angles.push_back(angle_v);
           ilayer += 3;
       }
       while (ilayer <= 31) {
           angles.push_back(angle_z);
           ilayer++;
       }
       // cout << "-I- <SpdTsTBGeoMapper::SetStrawLayerPars>" << "  Angles:" << endl;
       // for(int i=0; i < angles.size(); i++) printf("\t%2d\t% g\n", i+1, angles[i]);
          
       Int_t nnodes = module->GetNNodes();
       Int_t nmax = Min(nnodes, Int_t(angles.size()));
       // cout << "-I- <SpdTsTBGeoMapper::SetStrawLayerPars>" << " nnodes = " << nnodes << ", nmax = " << nmax << endl;
       for(Int_t i(0); i < nmax; i++) {
           SetStrawLayerPars(nmodule, angles[i], size, gap, sgap, 'o');
       }

   }
   else {
      cout << "-W- <SpdTsTBGeoMapper::SetStrawLayerPars>"
           << " Unknown geometry set: " << g << endl;
   }
}

//_____________________________________________________________________________
TGeoMatrix* SpdTsTBGeoMapper::GetNodeMatrix(const SpdGeoTableRec* rec, Int_t loc_i) const
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
  
   if (fGeoType == 3) {
    
       if (mname == AddPrefix("Module1")) 
       {
           m = new TGeoCombiTrans(0, fRShift, fZShift, 0);
           m->RotateZ(angle*loc_i - 90);          
       }     
       else {
           cout << "-E- <SpdTsTBGeoMapper::GetModuleGeoMatrix>"
                << " Unknown module: " << mname << endl;
       }
       
       return m;   
   }
 
   cout << "-E- <SpdTsTBGeoMapper::GetModuleGeoMatrix> "
        << "Unknown geometry type: " << fGeoType << endl;
 
   return m;
}

   
//_____________________________________________________________________________   
Bool_t SpdTsTBGeoMapper::InitStrawLayerPacking(Int_t nmodule, SpdGeoVolPars* layer, Int_t ilayer)
{
   SpdTsBStrawDLayer* l = dynamic_cast<SpdTsBStrawDLayer*>(layer);
   if (!l) return kFALSE;
   
   Double_t pangle, pwidth, pgap, psubgap;
   Char_t popt;
    
   // npack depends on (nmodule, ilayer)
   Int_t np = GetStrawModuleNPacking(nmodule);
   Int_t npack = (ilayer%np)+1;
   
   // module, npack
   GetStrawLayerPars(nmodule, npack, pangle, pwidth, pgap, psubgap, popt);
   l->SetPackingPars(pangle, pwidth, pgap, psubgap, popt);
  
   return kTRUE;
}

//_____________________________________________________________________________
Double_t SpdTsTBGeoMapper::GetLayerWidthFromStrawDiameter(Double_t d, Double_t gap, Double_t sgap)
{
    return d * sqrt(1. - pow(1+gap/d, 2)/4.) + sgap + d;
}

//_____________________________________________________________________________
void SpdTsTBGeoMapper::Print(Option_t*) const
{
   cout << "-I- <SpdTsTBGeoMapper::Print>" << "\n\n";
   SpdGeoMapper::Print("");
}

//_____________________________________________________________________________
TString SpdTsTBGeoMapper::GetPrintVolParsCap() const
{
   return Form("%3s  %2s  %4s   %9s %9s %9s %9s   %9s %9s  %-20s %-20s %-15s %6s %6s", 
          "I", "A", "NC", 
          "Length", "Width", "Lmin", "Lmax", 
          "PAngle", "PShift",
          "Class", "Name", "Media",
          "NN", "NV"); 
}
