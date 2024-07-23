// $Id$
// Author: artur   2018/01/25

//_____________________________________________________________________________
//
// SpdTorMagnet
//_____________________________________________________________________________

#include "SpdTorMagnet.h"
#include "SpdCommonGeoMapper.h"

#include <TGeoManager.h>
#include <TGeoTube.h>
#include <TMath.h>

#include <iostream>
using std::cout;
using std::endl;

using namespace TMath;

ClassImp(SpdTorMagnet)
ClassImp(SpdTorMagnet::tmcoil_)

//-----------------------------------------------------------------------------
SpdTorMagnet::tmcoil_::tmcoil_():
box_(0),hxbox_(0),hybox_(0),hzbox_(0),
hxcoil_(0),hycoil_(0),hzcoil_(0),coil_width_(0),inner_rounding_(0),
capacity_(0),mass_(0),density_(0),
fill_color_(kRed+3),line_color_(kRed+3),transparency_(10),
counts_(0)
{  
   for (Int_t i(0); i<4; i++) aparts_[i] = 0;
  
   bparts_[0] = 0;
   bparts_[1] = 0;
}

//-----------------------------------------------------------------------------
void SpdTorMagnet::tmcoil_::SetFillPars(Int_t fill, Int_t line, Int_t transp)
{
  fill_color_ = fill;
  line_color_ = line;
  transparency_ = transp;
  
  SetFillPars();
           
}

//-----------------------------------------------------------------------------
void SpdTorMagnet::tmcoil_::SetFillPars()
{
   for (Int_t i(0); i<4; i++) {
        if (aparts_[i]) {
            aparts_[i]->SetFillColor(fill_color_);
            aparts_[i]->SetLineColor(line_color_);
            aparts_[i]->SetTransparency(transparency_);
        }
   }
   
   for (Int_t i(0); i<2; i++) {
        if (bparts_[i]) {
            bparts_[i]->SetFillColor(fill_color_);
            bparts_[i]->SetLineColor(line_color_);
            bparts_[i]->SetTransparency(transparency_); 
        }
   }
}

//_____________________________________________________________________________
SpdTorMagnet::SpdTorMagnet():SpdPassiveModule("Magnet (tor)","SPDTORMAGNET",kSpdTorMagnet),
fLockGeom(kFALSE)
{
   SetGeoPars(SpdCommonGeoMapper::Instance()->GetTorMagnetDefGeoType());
}

//_____________________________________________________________________________
SpdTorMagnet::SpdTorMagnet(const char* Name, const char* Prefix): 
SpdPassiveModule(Name,Prefix,kSpdTorMagnet),fLockGeom(kFALSE)
{ 
   SetGeoPars(SpdCommonGeoMapper::Instance()->GetTorMagnetDefGeoType());
}

//_____________________________________________________________________________
SpdTorMagnet::~SpdTorMagnet() 
{
 
}

//_____________________________________________________________________________
void SpdTorMagnet::ConstructGeometry()
{
   if (fLockGeom) {
       cout << "-E- <SpdTorMagnet::ConstructGeometry> "
            << " Geometry is already built " << endl;  
       return;
   }
   
   fMasterVolume = SpdCommonGeoMapper::Instance()->GetMasterVolume();
  
   if (!fMasterVolume) {  
       cout << "-E- <SpdTorMagnet::ConstructGeometry> No MASTER volume " << endl;
       return;
   }
   
   if (fGeoType < 1) {
       //cout << "-I- <SpdTorMagnet::ConstructGeometry> No geometry will be built" << endl;
       fGeoType = 0;
       return;
   }
   
   if (fGeoType == 1) fLockGeom = ConstructGeometry_1();
   else if (fGeoType == 2) fLockGeom = ConstructGeometry_2();
   else {
       cout << "-E- <SpdTorMagnet::ConstructGeometry> "
            << " Unknown geometry type: " << fGeoType << endl; 
       fGeoType = 0;     
   }
   
   FillParsIn(GetParameters());
}

//_____________________________________________________________________________
Bool_t SpdTorMagnet::ConstructGeometry_1() // private
{ 
   if (!BuildCoil(0)) return kFALSE;
   if (!BuildCoil(1)) return kFALSE;
  
   TGeoTranslation trans; 
   TGeoRotation rot; 
   
   rot.RotateY(90);
   rot.RotateZ(-90.);  
   
   Int_t nsectors = SpdCommonGeoMapper::Instance()->GetNGeoSectors();
   
   Double_t dangle = 360./nsectors;
   Double_t dr, dz;
   
   Double_t angle = -dangle/2.;
   rot.RotateZ(-dangle/2.);
   
   for (int i(1); i<=nsectors; i++) {
     
        angle += dangle;
        rot.RotateZ(dangle); 
        
        // +++++ add central coils +++++
        
        // radial and longitudinal shifts
        dr = fAxialDistance + 2*fCoil[1].hybox_-fCoil[0].hybox_;  // radial shift 
        dz = 0;
        
        trans.SetTranslation(dr * Cos(angle*DegToRad()),
                             dr * Sin(angle*DegToRad()),
                             dz);
         
        fMasterVolume->AddNode(fCoil[0].box_,i,new TGeoCombiTrans(trans,rot));
        fCoil[0].counts_++;
            
        // +++++ add lateral coils +++++
        
         // radial and longitudinal shifts
        dr = fAxialDistance + fCoil[1].hybox_;         // radial shift 
        dz = fCoil[0].hxbox_ + fCoil[1].hxbox_;  

        // z-
        trans.SetTranslation(dr * Cos(angle*DegToRad()),
                             dr * Sin(angle*DegToRad()),
                             dz);
        
        fMasterVolume->AddNode(fCoil[1].box_,i,new TGeoCombiTrans(trans,rot));
        fCoil[1].counts_++;
        
        // z+
        trans.SetTranslation(dr * Cos(angle*DegToRad()),
                             dr * Sin(angle*DegToRad()),
                            -dz);
        
        fMasterVolume->AddNode(fCoil[1].box_,i+nsectors,new TGeoCombiTrans(trans,rot));
        fCoil[1].counts_++;
   }
   
   //cout << "<SpdTorMagnet::ConstructGeometry_1> "<< endl;
   
   return kTRUE;
}
 
//_____________________________________________________________________________
Bool_t SpdTorMagnet::ConstructGeometry_2() // private
{
   if (!BuildCoil(2)) return kFALSE;
   
   TGeoTranslation trans; 
   TGeoRotation rot;
   
   rot.RotateY(90);
   rot.RotateZ(-90.); 
   
   Int_t nsectors = SpdCommonGeoMapper::Instance()->GetNGeoSectors();
   
   Double_t dangle = 360./nsectors;
   Double_t dr, dz;
   
   Double_t angle = -dangle/2.;
   rot.RotateZ(-dangle/2.);
  
   for (int i(1); i<=nsectors; i++) {
       
        angle += dangle;
        rot.RotateZ(dangle); 
        
        // +++++ add coils +++++
        
        dr = fAxialDistance + fCoil[2].hybox_;
      
        trans.SetTranslation(dr * Cos(angle*DegToRad()),
                             dr * Sin(angle*DegToRad()),
                             0);
         
        fMasterVolume->AddNode(fCoil[2].box_,i,new TGeoCombiTrans(trans,rot));
        fCoil[2].counts_++;
   }
   
   //cout << "<SpdTorMagnet::ConstructGeometry_2> "<< endl;
   
   return kTRUE;
}

//_____________________________________________________________________________
Bool_t SpdTorMagnet::BuildCoil(Int_t ncoil) // private
{ 
   tmcoil_& coil = fCoil[ncoil];
   TString volname;
  
   // create box
   TGeoMedium* box_material = FindMedium(coil.base_material_,"air");
   
   if (!box_material) {
       cout << "-E- <SpdTorMagnet::BuildCoil> "
            << "Undefined \"box\" material: " << coil.base_material_ << endl;
       return kFALSE;
   }
  
   volname = FullName(Form("BOX%d",ncoil),kFALSE);
  
   coil.box_ = gGeoManager->MakeBox(volname, box_material, 
                                    coil.hxbox_, coil.hybox_, coil.hzbox_);
   coil.box_->SetTransparency(80);
    
   // create coil
   TGeoMedium* coil_material = FindMedium(coil.coil_material_,"iron");
   
   coil.density_ = coil_material->GetMaterial()->GetDensity();
   
   if (!coil_material) {
       cout << "-E- <SpdTorMagnet::BuildCoil> "
            << "Undefined \"coil\" material: " << coil.coil_material_ << endl;
       return kFALSE;    
   }
   
   TGeoShape* shape;
   
   Double_t rmax = coil.inner_rounding_ + coil.coil_width_;
   
   Double_t x0 = coil.hxcoil_ - rmax;
   Double_t y0 = coil.hycoil_ - rmax;
   
   coil.capacity_ = 0;
   coil.mass_ = 0;
   
   for (Int_t i(0); i<4; i++) {
     
        shape = new TGeoTubeSeg(coil.inner_rounding_, rmax, /*r[min,max]*/
                                coil.hzcoil_, /*width*/
                                90*i,(i+1)*90 /*phi[min,max], deg*/);
   
        volname = Form("ACOILSEG%d",i);

        coil.aparts_[i] = new TGeoVolume(volname /*name*/, shape, coil_material);
        
        coil.capacity_ += coil.aparts_[i]->Capacity();
   }
   
   volname = Form("BCOILBOX%d",1);
   
   coil.bparts_[0] = gGeoManager->MakeBox(volname, coil_material,
                     coil.hxcoil_-rmax, 0.5*coil.coil_width_, coil.hzcoil_);
                  
   volname = Form("BCOILBOX%d",2); 
   
   coil.bparts_[1] = gGeoManager->MakeBox(volname,coil_material,
                     0.5*coil.coil_width_, coil.hycoil_-rmax, coil.hzcoil_);
 
   coil.capacity_ += 2*coil.bparts_[0]->Capacity();
   coil.capacity_ += 2*coil.bparts_[1]->Capacity();
  
   coil.mass_ = coil.capacity_*coil.density_;
     
   coil.box_->AddNode(coil.aparts_[0],1,new TGeoTranslation( x0, y0,0)); 
   coil.box_->AddNode(coil.aparts_[1],1,new TGeoTranslation(-x0, y0,0));
   coil.box_->AddNode(coil.aparts_[2],1,new TGeoTranslation(-x0,-y0,0));
   coil.box_->AddNode(coil.aparts_[3],1,new TGeoTranslation( x0,-y0,0));
   
   x0 = coil.hxcoil_-0.5*coil.coil_width_;
   y0 = coil.hycoil_-0.5*coil.coil_width_;
   
   coil.box_->AddNode(coil.bparts_[0],1,new TGeoTranslation(  0, y0, 0));
   coil.box_->AddNode(coil.bparts_[0],2,new TGeoTranslation(  0,-y0, 0));
   coil.box_->AddNode(coil.bparts_[1],1,new TGeoTranslation( x0,  0 ,0));
   coil.box_->AddNode(coil.bparts_[1],2,new TGeoTranslation(-x0,  0, 0));
   
   coil.SetFillPars();
   
   return kTRUE;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________
void SpdTorMagnet::SetGeometryType(Int_t type)
{
   if (fLockGeom) {
       cout << "-E- <SpdTorMagnet::SetGeometryType> "
            << " Geometry is already built " << endl;  
       return;
   }
   
   if (type < 1) {
       //cout << "-I- <SpdTorMagnet::SetGeometryType> No geometry will be built" << endl;
       SetGeoPars(0);
       return;
   }
   else if (type == 1) SetGeoPars(1);
   else if (type == 2) SetGeoPars(2); 
   else {
       cout << "-E- <SpdTorMagnet::SetGeometryType> "
            << " Unknown geometry type: " << type << endl; 
       SetGeoPars(0); 
   }
}

//_____________________________________________________________________________
void SpdTorMagnet::SetAxialDistance(Double_t d /*cm*/)
{
   if (fLockGeom) {
       cout << "-E- <SpdTorMagnet::SetAxialDistance> "
            << " Geometry is already built " << endl;  
       return;
   }
   
   if (d < 10) {
       cout << "-W- <SpdTorMagnet::SetAxialDistance> "
            << " The distance should be more than 10 [cm] " << endl;  
       d = 10.;
   }
   
   fAxialDistance = d;
}

//_____________________________________________________________________________
void SpdTorMagnet::SetCoilsInnerRounding(Double_t r /*cm*/)
{
   if (fLockGeom) {
       cout << "-E- <SpdTorMagnet::SetCoilsRounding> "
            << " Geometry is already built " << endl;  
       return;
   }
   
   if (r < 0) {
       cout << "-W- <SpdTorMagnet::SetCoilsRounding> "
            << " The rounding radius should be more than 0 [cm] " << endl;  
       r = 0;     
   }
   
   Double_t d, rr;
   
   for (int i(0); i<SPD_TORMAGNET_NDCOILS; i++) {
        
        rr = r;
        d = fCoil[i].hxcoil_ - fCoil[i].coil_width_;
        
        if (rr > d)
        {  
            //cout << "-W- <SpdTorMagnet::SetCoilsRounding> "
            //     << " The rounding radius for coil of type " << i 
            //     << " should be < " << d << " [cm] "<< endl; 
            rr = d;
        }
        
        d = fCoil[i].hycoil_ - fCoil[i].coil_width_;
        if (rr > d)
        {
            //cout << "-W- <SpdTorMagnet::SetCoilsRounding> "
            //     << " The rounding radius for coil of type " << i 
            //     << " should be < " << d << " [cm] "<< endl; 
            rr = d;
        }
        
        fCoil[i].inner_rounding_ = rr;
   }
}

//_____________________________________________________________________________
void SpdTorMagnet::SetCoilsWidth(Double_t width /*cm*/)
{
   if (fLockGeom) {
       cout << "-E- <SpdTorMagnet::SetCoilsWidth> "
            << " Geometry is already built " << endl;  
       return;
   }
   
   if (width < 1) {
       cout << "-W- <SpdTorMagnet::SetCoilsWidth> "
            << " The width should be more than 1 [cm] " << endl;  
       width = 1.;
   }
   
   Double_t w, l;
   
   for (int i(0); i<SPD_TORMAGNET_NDCOILS; i++) {
        w = width;
        
        l = fCoil[i].hxcoil_ - fCoil[i].inner_rounding_;
        if (w > l) w = l;
        
        l = fCoil[i].hycoil_ - fCoil[i].inner_rounding_;
        if (w > l) w = l;
              
        fCoil[i].coil_width_ = w;
   }
}

//_____________________________________________________________________________
void SpdTorMagnet::SetCoilsThickness(Double_t thickness /*cm*/)
{
   if (fLockGeom) {
       cout << "-E- <SpdTorMagnet::SetCoilsThickness> "
            << " Geometry is already built " << endl;  
       return;
   }
   
   if (thickness < 0.1) {
       cout << "-W- <SpdTorMagnet::SetCoilsThickness> "
            << " The thickness should be more than 0.1 [cm] " << endl;  
       thickness = 0.1;   
   }
   
   thickness *= 0.5;
   
   for (int i(0); i<SPD_TORMAGNET_NDCOILS; i++) { 
        if (fCoil[i].hzbox_ < thickness) fCoil[i].hzbox_ = thickness;     
        fCoil[i].hzcoil_ = thickness;
   }
}

//_____________________________________________________________________________
void SpdTorMagnet::SetCoilsMaterial(TString material)
{
   if (fLockGeom) {
       cout << "-E- <SpdTorMagnet::SetCoilsMaterial> "
            << " Geometry is already built " << endl;  
       return;
   }
   
   TGeoMedium* medium = FindMedium(material,"");
   
   if (!medium) {
       cout << "-E- <SpdTorMagnet::SetCoilsMaterial> " 
            << " The material: \"" << material << "\" is not defined " << endl;
       return;     
   }
   
   for (int i(0); i<SPD_TORMAGNET_NDCOILS; i++) fCoil[i].coil_material_ = material;
}

//_____________________________________________________________________________
void SpdTorMagnet::SetBaseMaterial(TString material)
{
   if (fLockGeom) {
       cout << "-E- <SpdTorMagnet::SetBaseMaterial> "
            << " Geometry is already built " << endl;  
       return;
   }
   
   TGeoMedium* medium = FindMedium(material,"");
   
   if (!medium) {
       cout << "-E- <SpdTorMagnet::SetBaseMaterial> " 
            << " The material: \"" << material << "\" is not defined " << endl;
       return;     
   }
   
   for (int i(0); i<SPD_TORMAGNET_NDCOILS; i++) fCoil[i].base_material_ = material;
}

//_____________________________________________________________________________
void SpdTorMagnet::StretchCoilsAlongR(Double_t dr /*cm*/)
{
   if (fLockGeom) {
       cout << "-E- <SpdTorMagnet::StretchCoilsAlongR> "
            << " Geometry is already built " << endl;  
       return;
   }
   
   Double_t r_min, r;
    
   for (int i(0); i<SPD_TORMAGNET_NDCOILS; i++) {
     
        r_min = fCoil[i].inner_rounding_ + fCoil[i].coil_width_;     
        r = fCoil[i].hybox_ + 0.5*dr;
        
        if (r < r_min) 
        {
           //cout << "-W- <SpdTorMagnet::StretchCoilsAlongR> "
           //     << " New R-size for the coil of type = " << i 
           //     << " should be > " << r_min  << " [cm] "<< endl; 
                
           fCoil[i].hybox_  = r_min;
           fCoil[i].hycoil_ = r_min;
        }
        else {        
           fCoil[i].hybox_  += 0.5*dr;
           fCoil[i].hycoil_ += 0.5*dr;  
        }
   }
}
    
//_____________________________________________________________________________   
void SpdTorMagnet::PurseCoilsAlongZ(Double_t dz /*cm*/)
{
   if (fLockGeom) {
       cout << "-E- <SpdTorMagnet::PurseCoilsAlongZ> "
            << " Geometry is already built " << endl;  
       return;
   }  
   
   if (dz < 0) dz = -dz;
  
   Double_t l_min, l;
    
   for (int i(0); i<SPD_TORMAGNET_NDCOILS; i++) {
     
        l_min = fCoil[i].inner_rounding_ + fCoil[i].coil_width_;     
        l = fCoil[i].hxcoil_ - 0.5*dz;
   
        if (l < l_min) 
        {
           //cout << "-W- <SpdTorMagnet::PurseCoilsAlongZ> "
           //     << " New Z_size for the coil of type = " << i 
           //     << " should be > " << l_min  << " [cm] "<< endl; 
        
           fCoil[i].hxcoil_ = l_min;
        }
        else {        
      
           fCoil[i].hxcoil_ -= 0.5*dz;  
        }
   }
}

//_____________________________________________________________________________
void SpdTorMagnet::UnsetMaterials(TString media)
{
   if (fLockGeom) {
       cout << "-E- <SpdTorMagnet::UnsetMaterials> "
            << " Geometry is already built " << endl;  
       return;
   }  
   
   for (int i(0); i<SPD_TORMAGNET_NDCOILS; i++) {
        fCoil[i].base_material_ = media;
        fCoil[i].coil_material_ = media;
   }
   
   fUnsetMedia = media;
}

//_____________________________________________________________________________
void SpdTorMagnet::ResetMaterials()
{
   if (fLockGeom) return;
   SetDefaultMaterials(fGeoType);
}

//_____________________________________________________________________________
Double_t SpdTorMagnet::GetCapacity() const // in cm^3
{ 
   Double_t capacity_ = 0;
   for (int i(0); i<SPD_TORMAGNET_NDCOILS; i++) {     
       if (fCoil[i].box_) capacity_ += fCoil[i].capacity_ * fCoil[i].counts_;
   }       
   return capacity_;
}

//_____________________________________________________________________________  
Double_t SpdTorMagnet::GetMass() const  // in g
{
   Double_t mass_ = 0;
   for (int i(0); i<SPD_TORMAGNET_NDCOILS; i++) {     
       if (fCoil[i].box_) mass_ += fCoil[i].capacity_ * fCoil[i].density_ * fCoil[i].counts_;
   }  
   return mass_;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________    
void SpdTorMagnet::Print(Option_t*) const
{
   cout << "<SpdTorMagnet::Print> Name/Prefix: "  << fName << "/" << fTitle << endl;
      
   TString divider('-',150);
   
   cout << "\n";
   
   cout << "\tGeometry type:           " << fGeoType << endl;
   cout << "\tUnique module Id:        " << fModuleId << endl; 
   cout << "\tUnset media:             " << fUnsetMedia << endl;
   
   if (fGeoType < 1) { cout << endl; return; }
   
   cout << "\tDistance to axis (min):  " << fAxialDistance << " [cm] " << endl;
   
   cout << "\n" << divider.Data() << "\n";
   
   printf("%6s %4s %15s %14s %14s %14s %14s %14s %14s %14s %14s \n\n",
          "Type","N","Material","Dens [g/cm^3]","Volume [m^3]","Mass [kg]",
          "X-size [m]","Y-size [m]","Thick. [cm]","Width [cm]", "Rounding [cm]"
         );
   
   for (int i(0); i<SPD_TORMAGNET_NDCOILS; i++) {
     
       if (!fCoil[i].box_) continue;
       
       printf("%6d %4d %15s %14.6f %14.6f %14.6f %14.6f %14.6f %14.6f %14.6f %14.6f\n",
               i, fCoil[i].counts_, fCoil[i].coil_material_.Data(),
               fCoil[i].density_, fCoil[i].capacity_*1e-6, fCoil[i].mass_*1e-3,
               2*fCoil[i].hxcoil_*1e-2, 2*fCoil[i].hycoil_*1e-2, 2*fCoil[i].hzcoil_,
               fCoil[i].coil_width_,fCoil[i].inner_rounding_
             );
       printf("%11s %15s %44s %14.6f %14.6f %14.6f \n"," ",fCoil[i].base_material_.Data()," ",
              2*fCoil[i].hxbox_*1e-2, 2*fCoil[i].hybox_*1e-2, 2*fCoil[i].hzbox_);
   }
   
   cout << divider.Data() << "\n";
  
   printf("%6s %35s %14.3e %14.3e \n","TOTAL:","",GetCapacity()*1e-6,GetMass()*1e-3);
   
   cout << divider.Data() << "\n";
   
   cout << "\n";
}

//_____________________________________________________________________________
void SpdTorMagnet::SetGeoPars(Int_t geotype) // private
{ 
   //cout <<"<SpdTorMagnet::SetGeoPars> " << geotype << endl;
    
   if (geotype < 1 || geotype > 2) {
       //cout << "-I- <SpdTorMagnet::SetGeoPars> No geometry will be built " << endl;
     
       fGeoType = 0;
       fAxialDistance = 0;
       return;
   }
    
   fGeoType = geotype;
    
   if (geotype == 1) fAxialDistance = SpdCommonGeoMapper::GetTorMagnetDistToAxis1();
   else fAxialDistance = SpdCommonGeoMapper::GetTorMagnetDistToAxis2();

   fCoil[0].hxbox_ = SpdCommonGeoMapper::GetTorMagnetLen11()/2.;
   fCoil[0].hybox_ = SpdCommonGeoMapper::GetTorMagnetSize11()/2.; 
   fCoil[0].hzbox_ = SpdCommonGeoMapper::GetTorMagnetCoilThickness()/2.;  
   
   fCoil[0].hxcoil_ = SpdCommonGeoMapper::GetTorMagnetCoilLen11()/2.; 
   fCoil[0].hycoil_ = fCoil[0].hybox_; 
   fCoil[0].hzcoil_ = fCoil[0].hzbox_;
      
   fCoil[0].coil_width_     = SpdCommonGeoMapper::GetTorMagnetCoilWidth();
   fCoil[0].inner_rounding_ = SpdCommonGeoMapper::GetTorMagnetCoilRR();
   
   fCoil[0].base_material_ = "air";
   fCoil[0].coil_material_ = SpdCommonGeoMapper::GetTorMagnetCoilMaterial();
   
   //fCoil[0].fill_color_ = kBlue+3;
   //fCoil[0].line_color_ = kBlue+3;
   //fCoil[0].transparency_ = 10;
   
   //------------------------------------
   
   fCoil[1].hxbox_ = SpdCommonGeoMapper::GetTorMagnetLen12()/2.; 
   fCoil[1].hybox_ = SpdCommonGeoMapper::GetTorMagnetSize12()/2.; 
   fCoil[1].hzbox_ = SpdCommonGeoMapper::GetTorMagnetCoilThickness()/2.;
   
   fCoil[1].hxcoil_ = SpdCommonGeoMapper::GetTorMagnetCoilLen12()/2.;
   fCoil[1].hycoil_ = fCoil[1].hybox_; 
   fCoil[1].hzcoil_ = fCoil[1].hzbox_;
   
   fCoil[1].coil_width_     = SpdCommonGeoMapper::GetTorMagnetCoilWidth();
   fCoil[1].inner_rounding_ = SpdCommonGeoMapper::GetTorMagnetCoilRR();
   
   fCoil[1].base_material_ = "air";
   fCoil[1].coil_material_ =  SpdCommonGeoMapper::GetTorMagnetCoilMaterial();
   
   //fCoil[1].fill_color_ = kBlue+3;
   //fCoil[1].line_color_ = kBlue+3;
   //fCoil[1].transparency_ = 10;   
   
   //------------------------------------
 
   fCoil[2].hxbox_ = SpdCommonGeoMapper::GetTorMagnetLen2()/2.;
   fCoil[2].hybox_ = SpdCommonGeoMapper::GetTorMagnetSize2()/2.; 
   fCoil[2].hzbox_ = SpdCommonGeoMapper::GetTorMagnetCoilThickness()/2.;
   
   fCoil[2].hxcoil_ = SpdCommonGeoMapper::GetTorMagnetCoilLen2()/2.;
   fCoil[2].hycoil_ = fCoil[2].hybox_; 
   fCoil[2].hzcoil_ = fCoil[2].hzbox_;
   
   fCoil[2].coil_width_     = SpdCommonGeoMapper::GetTorMagnetCoilWidth();
   fCoil[2].inner_rounding_ = SpdCommonGeoMapper::GetTorMagnetCoilRR();
  
   fCoil[2].base_material_ = "air";
   fCoil[2].coil_material_ =  SpdCommonGeoMapper::GetTorMagnetCoilMaterial();
      
   //fCoil[2].fill_color_ = kBlue+3;
   //fCoil[2].line_color_ = kBlue+3;
   //fCoil[2].transparency_ = 10;
}

//_____________________________________________________________________________
void SpdTorMagnet::SetDefaultMaterials(Int_t geotype) // private
{ 
   //cout <<"<SpdTorMagnet::SetDefaultMaterials> " << geotype << endl;
    
   if (geotype < 1 || geotype > 2) return;
   
   fCoil[0].base_material_ = "air";
   fCoil[0].coil_material_ = SpdCommonGeoMapper::GetTorMagnetCoilMaterial();
   
   //fCoil[0].fill_color_ = kBlue+3;
   //fCoil[0].line_color_ = kBlue+3;
   //fCoil[0].transparency_ = 10;
   
   //------------------------------------
   
   fCoil[1].base_material_ = "air";
   fCoil[1].coil_material_ =  SpdCommonGeoMapper::GetTorMagnetCoilMaterial();
   
   //fCoil[1].fill_color_ = kBlue+3;
   //fCoil[1].line_color_ = kBlue+3;
   //fCoil[1].transparency_ = 10;   
   
   //------------------------------------
 
   fCoil[2].base_material_ = "air";
   fCoil[2].coil_material_ =  SpdCommonGeoMapper::GetTorMagnetCoilMaterial();
      
   //fCoil[2].fill_color_ = kBlue+3;
   //fCoil[2].line_color_ = kBlue+3;
   //fCoil[2].transparency_ = 10;
}

//_____________________________________________________________________________
Bool_t SpdTorMagnet::FillParsIn(SpdPassiveGeoParSet* params)
{
   if (!SpdPassiveModule::FillParsIn(params)) return kFALSE;
   return kTRUE;
}

//_____________________________________________________________________________    
Bool_t SpdTorMagnet::LoadParsFrom(SpdPassiveGeoParSet* params)
{
   if (!SpdPassiveModule::LoadParsFrom(params)) return kFALSE;
   return kTRUE;
}



