// $Id$
// Author: artur   2015/09/26


//_____________________________________________________________________________
//
// SpdSttGeoMapper
//_____________________________________________________________________________

#include "SpdSttGeoMapper.h"
#include "SpdSttStrawTubeA.h"
#include "SpdParameter.h"
#include <iostream>
#include <TString.h>

ClassImp(SpdSttGeoMapper)

SpdSttGeoMapper* SpdSttGeoMapper::gInstance = 0;

using std::cout;
using std::endl;

//_____________________________________________________________________________
SpdSttGeoMapper::SpdSttGeoMapper(Int_t GeoType)
{
  if (gInstance) {
      cout << "<SpdSttGeoMapper::SpdSttGeoMapper> Singleton instance already exists." << endl;
      
      if (GeoType != GeoType_) {    
          cout << "<SpdSttGeoMapper::SpdSttGeoMapper> Warning. Different geometry type: " 
               << GeoType_ << endl; 
      }
        
      return;
  }
  
  Tube_ = 0;
  gInstance = this;
  ComplexGeom_ = kFALSE;
  BlockPars_ = 0;
  
  NTotalTubes_ = 0;
  NTubesPerSection_ = 1e9;
  NSections_ = 0;
  
  Params_ = new TObjArray();
  
  if (GeoType == 0) {
      ClearGeometry(); 
      return;
  }
  
  InitGeometry(GeoType,kTRUE);
}

//_____________________________________________________________________________
SpdSttGeoMapper::~SpdSttGeoMapper() 
{
  ClearGeometry();
  
  if (Params_) { 
      Params_->Delete();
      delete Params_; 
      Params_ = 0;
  }
}

//_____________________________________________________________________________
void SpdSttGeoMapper::ClearParameters() 
{
  if (Params_) Params_->Delete();
}

//_____________________________________________________________________________
void SpdSttGeoMapper::ClearComplexGeom() 
{
  if (BlockPars_) { delete [] BlockPars_; BlockPars_ = 0; }
  
  int n;
  
  n = Blocks_.size(); 
  if (n != 0) {
      std::vector<SpdSttGeoMapper*>::iterator it = Blocks_.begin();
      for (; it != Blocks_.end(); ++it) { if (*it) delete *it; }
      Blocks_.clear();
  }
  
  n = BlockLocs_.size();  
  if (n != 0) {
      std::vector<Double_t*>::iterator it = BlockLocs_.begin();
      for (; it != BlockLocs_.end(); ++it) { if (*it) delete [] (*it); }
      BlockLocs_.clear();  
  }
}

//_____________________________________________________________________________
void SpdSttGeoMapper::ClearGeometry()
{
  GeoType_ = 0;
  
  if (Tube_) { delete Tube_; Tube_ = 0; }
  
  GeoId_.clear();
  
  if (!Tubes_.empty()) {
      std::vector<SpdSttStrawTube*>::iterator it = Tubes_.begin(); 
      for (; it != Tubes_.end(); it++) delete *it;
      Tubes_.clear();
  }
 
  if (ComplexGeom_) {
      ClearComplexGeom();
      ComplexGeom_ = kFALSE;
  }
}

//_____________________________________________________________________________
void SpdSttGeoMapper::InitGeometry(Int_t gtype, Bool_t reinit) 
{ 
  if (GeoType_ == gtype && !reinit) return;
  
  ClearGeometry();
    
  if (gtype == 1) {
      if (InitGeometryOfType1()) GeoType_ = gtype;
      else {
        cout << "<SpdSttGeoMapper::InitGeometry>: Bad geometry initialization (type = " << gtype << ") "<< endl;
      }
  }
  else if (gtype == 2) {
      if (InitGeometryOfType2()) GeoType_ = gtype;
      else {
        cout << "<SpdSttGeoMapper::InitGeometry>: Bad geometry initialization (type = " << gtype << ") "<< endl;
      }
  }
  else if (gtype == 0) { 
      cout << "<SpdSttGeoMapper::InitGeometry>: Unset geometry (type = " << gtype << ") "<< endl;
  } 
  else {
      cout << "<SpdSttGeoMapper::InitGeometry>: Unknown geometry type = " << gtype << endl;
  }
}

//_____________________________________________________________________________
SpdSttGeoMapper* SpdSttGeoMapper::GetBlockMapper(Int_t block_type)  
{ 
  if (block_type < 0 || block_type >= Blocks_.size()) return 0;
  return Blocks_[block_type]; 
} 
 
//_____________________________________________________________________________ 
Double_t* SpdSttGeoMapper::GetBlockLocation(Int_t block_num) 
{ 
  if (block_num < 0 || block_num >= BlockLocs_.size()) return 0;
  return BlockLocs_[block_num]; 
} 

//_____________________________________________________________________________ 
Double_t* SpdSttGeoMapper::GetBlockParameters() 
{ 
  return BlockPars_; 
} 

//_____________________________________________________________________________ 
SpdParameter* SpdSttGeoMapper::AddParameter(TString par_name) 
{
  if (!Params_) return 0;
  SpdParameter* par = GetParameter(par_name);
  if (par) return 0;
  Params_->Add(new SpdParameter(par_name));
  return GetParameter(par_name);
}

//_____________________________________________________________________________ 
SpdParameter* SpdSttGeoMapper::GetParameter(TString par_name)
{
  if (!Params_) return 0;
  (SpdParameter*)Params_->FindObject(par_name);
}

//_____________________________________________________________________________ 
TObjArray* SpdSttGeoMapper::GetParameters() 
{ 
  return Params_;
}

//_____________________________________________________________________________ 
Double_t SpdSttGeoMapper::GetDigitPar(const Text_t* par_name)
{
   SpdParameter* par = GetParameter(par_name);
   if (!par) return 0;
   if (par->CheckType(0)) {
      Int_t v;
      par->Value(v);
      return v;
   }
   else if (par->CheckType(0.)) {
      Double_t v;
      par->Value(v);
      return v;
   }
   return 0;
}

//_____________________________________________________________________________ 
void SpdSttGeoMapper::SetTubeSections(Int_t nsec) 
{ 
   // set number of sections in the tube
   SpdParameter* par = GetParameter("n_tube_sections"); 
   if (par) par->SetValue(nsec); 
   else { Params_->Add(new SpdParameter("n_tube_sections",nsec)); }   
}

//_____________________________________________________________________________ 
void SpdSttGeoMapper::SetScale(Double_t scale) 
{
   cout << "<SpdSttGeoMapper::SetScale> The method not implemented. " << endl;
   // set scale factor
   //SpdParameter* par = GetParameter("scale");
   //if (par) par->SetValue(scale);
   //else { Params_->Add(new SpdParameter("scale",scale)); } 
}

//_____________________________________________________________________________ 
void SpdSttGeoMapper::SetTubeStep(Double_t step) 
{
   // set distance between straws (in straw sizes, > 1)
   SpdParameter* par = GetParameter("tube_step");  
   if (par)  par->SetValue(step);
   else { Params_->Add(new SpdParameter("tube_step",step)); } 
}

//_____________________________________________________________________________
SpdSttStrawTube* SpdSttGeoMapper::GetTube(Int_t id, Int_t option) 
{
  if (GeoType_ == 1) {
      if (SetTubeOfType1(id,option)) return Tube_; 
      else {
        cout << "<SpdSttGeoMapper::InitGeometry>: Bad tube initialization (tube = " << id << ") "<< endl;
        return 0;
      }
  }
  if (GeoType_ == 2) {
      if (SetTubeOfType2(id,option)) return Tube_; 
      else {
        cout << "<SpdSttGeoMapper::InitGeometry>: Bad tube initialization (tube = " << id << ") "<< endl;
        return 0;
      }
  }
  if (GeoType_ == 0) {
      cout << "<SpdSttGeoMapper::GetTube> Geometry is not defined (type = " << GeoType_ << ")" << endl;
      return 0;
  }
  else {
      cout << "<SpdSttGeoMapper::GetTube> Unknown geometry type = " << GeoType_ << endl;
      return 0;
  }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++ PRINT GEOMETRY +++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________
void SpdSttGeoMapper::print(Int_t opt) 
{
   cout << "<SpdSttGeoMapper::print(" << opt << ")> " << endl;
   cout << "Geometry type = " << GeoType_ << endl;
   
   cout << endl << "Parameters: " << endl; 
   
   if (!Params_ || Params_->GetEntriesFast() == 0) return;

   Int_t pr(0), np(0), nn(0);
   SpdParameter* par;
   TIter next(Params_);
   
   while (kTRUE) {   
       nn = 0;
       while ((par = (SpdParameter*)next())) {
          if (par && par->GetPriority() == pr) {
              np++;
              printf("%3d.   ",np);
              par->PrintParameter();
              nn++;
             
          }
       }
       if (np == Params_->GetEntries()) break;
       if (nn > 0) cout << endl;
       if (pr > 9) break;
       pr++;
       next.Reset();
   }
   cout << endl;
   
   if (Tube_) cout << "Tube type: " << Tube_->ClassName() << endl;
   else cout << "Tube type: " << "not defined" << endl;
   
   Int_t nmapped = GeoId_.size();
   if (nmapped > 0) nmapped--;
   cout << "Mapped tubes: " << nmapped << endl;
   cout << "Stored tubes: " << Tubes_.size() << endl;
   
   if (opt == 1) { if (Tube_) Tube_->print(0); }
   else if (opt > 1) { if (Tube_) Tube_->print(opt-1); }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++ GEOMETRY OF TYPE 1  +++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________
void SpdSttGeoMapper::SetParametersOfGeoType1(Bool_t reinit_default) 
{
   SpdParameter* par;
   
   /*[1]*/
   par = GetParameter("barrel_rmin");  // barrel min. radius (cm)
   if (par) { if (reinit_default) *par = 25.0; }
   else { Params_->Add(new SpdParameter("barrel_rmin",25.0)); }
   /*[2]*/  
   par = GetParameter("barrel_rmax");  // barrel max. radius (cm) 
   if (par) { if (reinit_default) *par = 100.0; }
   else { Params_->Add(new SpdParameter("barrel_rmax",100.0)); } 
   /*[3]*/
   NSections_ = 1;
   par = GetParameter("n_sections");    // number of sections
   if (par) { if (reinit_default) *par = NSections_; }
   else { Params_->Add(new SpdParameter("nsections",NSections_)); } 
   /*[4]*/
   par = GetParameter("n_tube_sections"); // number of sections in the tube
   if (par) { if (reinit_default) *par = 6; }
   else { Params_->Add(new SpdParameter("n_tube_sections",6)); } 
   /*[5]*/
   par = GetParameter("scale"); // scale factor
   if (par) { if (reinit_default) *par = 10.; }
   else { Params_->Add(new SpdParameter("scale",10.)); } 
   /*[6]*/
   par = GetParameter("tube_step");  // distance between straws (in straw sizes, > 1)
   if (par) { if (reinit_default) *par = 10.; }
   else { Params_->Add(new SpdParameter("tube_step",2.)); } 
   /*[7]*/
   par = GetParameter("straw_length");  // straw length (cm)
   if (par) { if (reinit_default) *par = 150.; }
   else { Params_->Add(new SpdParameter("straw_length",150.0)); } 
   /*[8]*/
   par = GetParameter("straw_rmax");  // // straw max. radius (cm) without scaling
   if (par) { if (reinit_default) *par = 0.250; }
   else { Params_->Add(new SpdParameter("straw_rmax",0.250)); } 
   /*[9]*/
   par = GetParameter("straw_rmin");  // // straw min. radius (cm) without scaling
   if (par) { if (reinit_default) *par = 0.250-0.020; }
   else { Params_->Add(new SpdParameter("straw_rmin",0.250-0.020)); } 
}

//_____________________________________________________________________________
bool SpdSttGeoMapper::InitGeometryOfType1() 
{
   SpdSttStrawTubeA* tube = new SpdSttStrawTubeA();
   if (Tube_) delete Tube_;
   Tube_ = tube;
   
   NTotalTubes_ = 0;
   NTubesPerSection_ = 1e9;
   
   SetParametersOfGeoType1();
   
   // get parameters from the list
   Double_t lstraw;    
   if (!SpdParameter::GetParameter(Params_,"straw_length",lstraw)) return kFALSE;
   
   Double_t scale;
   if (!SpdParameter::GetParameter(Params_,"scale",scale)) return kFALSE;
   
   Double_t rstraw_max;
   if (!SpdParameter::GetParameter(Params_,"straw_rmax",rstraw_max)) return kFALSE;
  
   Double_t rstraw_min;
   if (!SpdParameter::GetParameter(Params_,"straw_rmin",rstraw_min)) return kFALSE;
   
   Double_t tube_step;
   if (!SpdParameter::GetParameter(Params_,"tube_step",tube_step)) return kFALSE;
   
   Int_t n_tube_sections;
   if (!SpdParameter::GetParameter(Params_,"n_tube_sections",n_tube_sections)) return kFALSE;
   
   Double_t barrel_rmin;
   if (!SpdParameter::GetParameter(Params_,"barrel_rmin",barrel_rmin)) return kFALSE;
   
   Double_t barrel_rmax;
   if (!SpdParameter::GetParameter(Params_,"barrel_rmax",barrel_rmax)) return kFALSE;
   
   rstraw_max *= scale;
   rstraw_min *= scale;
   
   Tube_->SetNSections(n_tube_sections);
   Tube_->SetTubleHalfLength(0.5*lstraw);
   Tube_->SetTubeOutRad(rstraw_max);
   Tube_->SetTubeInRad(rstraw_min); 
   
   tube->Scale(2.0*rstraw_max*tube_step);
 
   Bool_t inside;
   Int_t nt(0), nr(1);
  
   GeoId_.push_back(0);
  
   while (true) {
      inside = false;
      for (int i(0); i<6*nr; i++) {
           tube->SetNTube(++nt);               
           if (barrel_rmax >= tube->RadialDistPlus()) {              
               inside = true;                              
               if (barrel_rmin <= tube->RadialDistMinus()) GeoId_.push_back(nt);                                                          
           }
      }
      if (inside) nr++; 
      else break;
   }  
   
   NTotalTubes_ = GeoId_.size()-1;
   NTubesPerSection_ = NTotalTubes_;
  
   SpdParameter* par;
   
   par = GetParameter("n_tubes_total"); 
   if (par) { *par = NTotalTubes_; }
   else { Params_->Add(new SpdParameter("n_tubes_total",NTotalTubes_,1)); } 
   
   par = GetParameter("n_tubes_per_section");
   if (par) { *par = NTubesPerSection_; }
   else { Params_->Add(new SpdParameter("n_tubes_per_section",NTubesPerSection_,1)); } 
   
   cout << "<SpdSttGeoMapper::InitGeometryOfType1> TUBES: " 
        << NTotalTubes_ << "/" << nt << endl;
   
   return kTRUE;  
}

//_____________________________________________________________________________
bool SpdSttGeoMapper::SetTubeOfType1(Int_t id, Int_t option) 
{
   if (!Tube_) return kFALSE;
   if (id < 1 || id > NTotalTubes_ || id >= GeoId_.size()) return kFALSE;
   
   Tube_->SetNTube(GeoId_[id]);
   
   if (option == 1) Tube_->GetNeighboring(0);
   
   return kTRUE;  
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++ GEOMETRY OF TYPE 2  +++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________
void SpdSttGeoMapper::SetParametersOfGeoType2(Bool_t reinit_default) 
{ 
  
   SpdParameter* par;
   
   /*[1]*/
   par = GetParameter("barrel_rmin");  // barrel min. radius (cm)
   if (par) { if (reinit_default) *par = 25.0; }
   else { Params_->Add(new SpdParameter("barrel_rmin",25.0)); }
   /*[2]*/  
   par = GetParameter("barrel_rmax");  // barrel max. radius (cm) 
   if (par) { if (reinit_default) *par = 100.0; }
   else { Params_->Add(new SpdParameter("barrel_rmax",100.0)); } 
   /*[3]*/
   NSections_ = 6;
   par = GetParameter("n_sections");    // number of sections
   if (par) { if (reinit_default) *par = NSections_; }
   else { Params_->Add(new SpdParameter("nsections",NSections_)); } 
   /*[4]*/
   par = GetParameter("n_tube_sections"); // number of sections in the tube
   if (par) { if (reinit_default) *par = 1; }
   else { Params_->Add(new SpdParameter("n_tube_sections",6)); } 
   /*[5]*/
   par = GetParameter("scale"); // scale factor
   if (par) { if (reinit_default) *par = 10.; }
   else { Params_->Add(new SpdParameter("scale",10.)); } 
   /*[6]*/
   par = GetParameter("tube_step");  // distance between straws (in straw sizes, > 1)
   if (par) { if (reinit_default) *par = 10.; }
   else { Params_->Add(new SpdParameter("tube_step",2.)); } 
   /*[7]*/
   Double_t lstraw = 150./NSections_;
   par = GetParameter("straw_length");  // straw length (cm)
   if (par) { if (reinit_default) *par = lstraw; }
   else { Params_->Add(new SpdParameter("straw_length",lstraw)); } 
   /*[8]*/
   par = GetParameter("straw_rmax");  // // straw max. radius (cm) without scaling
   if (par) { if (reinit_default) *par = 0.250; }
   else { Params_->Add(new SpdParameter("straw_rmax",0.250)); } 
   /*[9]*/
   par = GetParameter("straw_rmin");  // // straw min. radius (cm) without scaling
   if (par) { if (reinit_default) *par = 0.250-0.020; }
   else { Params_->Add(new SpdParameter("straw_rmin",0.250-0.020)); } 
}

//_____________________________________________________________________________
bool SpdSttGeoMapper::InitGeometryOfType2() 
{
   SpdSttStrawTubeA* tube = new SpdSttStrawTubeA();
   if (Tube_) delete Tube_;
   Tube_ = tube;
   
   NTotalTubes_ = 0;
   NTubesPerSection_ = 1e9;
   
   SetParametersOfGeoType2();
   
   // get parameters from the list
   Double_t lstraw;    
   if (!SpdParameter::GetParameter(Params_,"straw_length",lstraw)) return kFALSE;
   
   Double_t scale;
   if (!SpdParameter::GetParameter(Params_,"scale",scale)) return kFALSE;
   
   Double_t rstraw_max;
   if (!SpdParameter::GetParameter(Params_,"straw_rmax",rstraw_max)) return kFALSE;
  
   Double_t rstraw_min;
   if (!SpdParameter::GetParameter(Params_,"straw_rmin",rstraw_min)) return kFALSE;
   
   Double_t tube_step;
   if (!SpdParameter::GetParameter(Params_,"tube_step",tube_step)) return kFALSE;
   
   Int_t n_tube_sections;
   if (!SpdParameter::GetParameter(Params_,"n_tube_sections",n_tube_sections)) return kFALSE;
   
   Double_t barrel_rmin;
   if (!SpdParameter::GetParameter(Params_,"barrel_rmin",barrel_rmin)) return kFALSE;
   
   Double_t barrel_rmax;
   if (!SpdParameter::GetParameter(Params_,"barrel_rmax",barrel_rmax)) return kFALSE;
   
   rstraw_max *= scale;
   rstraw_min *= scale;
   
   Tube_->SetNSections(n_tube_sections);
   Tube_->SetTubleHalfLength(0.5*lstraw);
   Tube_->SetTubeOutRad(rstraw_max);
   Tube_->SetTubeInRad(rstraw_min); 
   
   tube->Scale(2.0*rstraw_max*tube_step);
 
   Bool_t inside;
   Int_t nt(0), nr(1);
  
   GeoId_.push_back(0);
 
   while (true) {
      inside = false;
      for (int i(0); i<6*nr; i++) {
           tube->SetNTube(++nt);               
           if (barrel_rmax >= tube->RadialDistPlus()) {              
               inside = true;                              
               if (barrel_rmin<= tube->RadialDistMinus()) GeoId_.push_back(nt);                                                          
           }
      }
      if (inside) nr++; 
      else break;
   }  
      
   NTubesPerSection_ = GeoId_.size()-1;
   NTotalTubes_ = NTubesPerSection_*NSections_;
   
   SpdParameter* par;
   
   par = GetParameter("n_tubes_total"); 
   if (par) { *par = NTotalTubes_; }
   else { Params_->Add(new SpdParameter("n_tubes_total",NTotalTubes_,1)); } 
   
   par = GetParameter("n_tubes_per_section");
   if (par) { *par = NTubesPerSection_; }
   else { Params_->Add(new SpdParameter("n_tubes_per_section",NTubesPerSection_,1)); } 
   
   cout << "<SpdSttGeoMapper::InitGeometryOfType2> TUBES: " 
        <<  NTotalTubes_ << "/" << nt*NSections_ << endl;
    
   return kTRUE;  
}

//_____________________________________________________________________________
bool SpdSttGeoMapper::SetTubeOfType2(Int_t id, Int_t option) 
{
   if (!Tube_) return kFALSE;
   if (id < 1 || id > NTotalTubes_) return kFALSE;
   
   Int_t section = (id-1)/NTubesPerSection_;      // 0,...,NSections_-1
   
   id -= section*NTubesPerSection_;  // 1,...
   
   if (id >= GeoId_.size()) return kFALSE;
     
   Tube_->SetNTube(GeoId_[id]);
   
   Double_t dsh = 0.;
   //dsh = 10;
   
   Double_t z = (Tube_->GetTubeHalfLength()+dsh)*(2*section-NSections_+1);
   Tube_->SetZSource(z);
   
   if (option == 1) Tube_->GetNeighboring(0);
   
   return kTRUE;
}






