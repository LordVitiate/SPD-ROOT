// $Id$
// Author: artur   2018/01/25

//_____________________________________________________________________________
//
// SpdHybMagnet
//_____________________________________________________________________________

#include "SpdHybMagnet.h"
#include "SpdCommonGeoMapper.h"
#include "SpdPassiveGeoParSet.h"

#include <TGeoManager.h>
#include <TGeoTube.h>
#include <TMath.h>

#include <iostream>
using std::cout;
using std::endl;

using namespace TMath;

ClassImp(SpdHybMagnet)
ClassImp(SpdHybMagnet::RingCoil)
ClassImp(SpdHybMagnet::LoopCoil)

//=============================================================================
SpdHybMagnet::RingCoil::RingCoil():
tube_(0),coil_max_radius_(0),coil_width_(0),coil_thickness_(0),
capacity_(0),mass_(0),density_(0),
fill_color_(kRed+3),line_color_(kRed+3),transparency_(0),
counts_(0)
{  
}

//=============================================================================
void SpdHybMagnet::RingCoil::SetFillPars(Int_t fill, Int_t line, Int_t transp)
{
  fill_color_ = fill;
  line_color_ = line;
  transparency_ = transp;
}

//=============================================================================
void SpdHybMagnet::RingCoil::SetFillPars()
{
  if (!tube_) return;
  
  tube_->SetFillColor(fill_color_);
  tube_->SetLineColor(line_color_);
  tube_->SetTransparency(transparency_);
}

//-----------------------------------------------------------------------------
SpdHybMagnet::LoopCoil::LoopCoil():
box_(0),hxbox_(0),hybox_(0),hzbox_(0),
hxcoil_(0),hycoil_(0),hzcoil_(0),coil_width_(0),inner_rounding_(0),
capacity_(0),mass_(0),density_(0),
fill_color_(kRed+3),line_color_(kRed+3),transparency_(0),
counts_(0)
{  
   for (Int_t i(0); i<4; i++) aparts_[i] = 0;
  
   bparts_[0] = 0;
   bparts_[1] = 0;
}

//-----------------------------------------------------------------------------
void SpdHybMagnet::LoopCoil::SetFillPars(Int_t fill, Int_t line, Int_t transp)
{
  fill_color_ = fill;
  line_color_ = line;
  transparency_ = transp;
  
  SetFillPars();
}

//-----------------------------------------------------------------------------
void SpdHybMagnet::LoopCoil::SetFillPars()
{
   for (Int_t i(0); i < 4; i++) {
        if (aparts_[i]) {
            aparts_[i]->SetFillColor(fill_color_);
            aparts_[i]->SetLineColor(line_color_);
            aparts_[i]->SetTransparency(transparency_);
        }
   }
   
   for (Int_t i(0); i < 2; i++) {
        if (bparts_[i]) {
            bparts_[i]->SetFillColor(fill_color_);
            bparts_[i]->SetLineColor(line_color_);
            bparts_[i]->SetTransparency(transparency_); 
        }
   }
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdHybMagnet                                                               //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

//_____________________________________________________________________________
SpdHybMagnet::SpdHybMagnet():SpdPassiveModule("Magnet (hyb)","SPDHYBMAGNET",kSpdHybMagnet),
fLockGeom(kFALSE)
{
   SetGeoPars(SpdCommonGeoMapper::Instance()->GetHybMagnetDefGeoType());
}

//_____________________________________________________________________________
SpdHybMagnet::SpdHybMagnet(const char* Name, const char* Prefix): 
SpdPassiveModule(Name,Prefix,kSpdHybMagnet),fLockGeom(kFALSE)
{ 
   SetGeoPars(SpdCommonGeoMapper::Instance()->GetHybMagnetDefGeoType());
}

//_____________________________________________________________________________
SpdHybMagnet::~SpdHybMagnet() 
{
 
}

//_____________________________________________________________________________
void SpdHybMagnet::ConstructGeometry()
{
   if (fLockGeom) {
       cout << "-E- <SpdHybMagnet::ConstructGeometry> "
            << " Geometry is already built " << endl;  
       return;
   }
   
   fMasterVolume = SpdCommonGeoMapper::Instance()->GetMasterVolume();
  
   if (!fMasterVolume) {  
       cout << "-E- <SpdHybMagnet::ConstructGeometry> No MASTER volume " << endl;
       return;
   }
   
   if (fGeoType < 1) {
       //cout << "-I- <SpdHybMagnet::ConstructGeometry> No geometry will be built" << endl;
       fGeoType = 0;
       return;
   }
   
        if (fGeoType == 1) fLockGeom = ConstructGeometry_1();
   else if (fGeoType == 2) fLockGeom = ConstructGeometry_2();
   else if (fGeoType == 3) fLockGeom = ConstructGeometry_2();
   else {
       cout << "-E- <SpdHybMagnet::ConstructGeometry> "
            << " Unknown geometry type: " << fGeoType << endl; 
       fGeoType = 0;     
   }
   
   FillParsIn(GetParameters());
}
 
//_____________________________________________________________________________
Bool_t SpdHybMagnet::ConstructGeometry_1() // private
{
   if (!BuildLoopCoil(fLoopCoil)) return kFALSE;
   if (!BuildRingCoil(fRingCoil)) return kFALSE;
   
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
        
        dr = fLoopCoilAxialDistance + fLoopCoil.hybox_;
      
        trans.SetTranslation(dr * Cos(angle*DegToRad()),
                             dr * Sin(angle*DegToRad()),
                             0);
         
        fMasterVolume->AddNode(fLoopCoil.box_,i,new TGeoCombiTrans(trans,rot));
        fLoopCoil.counts_++;
   }
   
   Double_t shift1 = fRingCoilAxialDistanceL1 + 0.5*fRingCoil.coil_width_;
   Double_t shift2 = fRingCoilAxialDistanceL2 + 0.5*fRingCoil.coil_width_;
   
   fMasterVolume->AddNode(fRingCoil.tube_,1,new TGeoTranslation(0,0, shift1));
   fMasterVolume->AddNode(fRingCoil.tube_,2,new TGeoTranslation(0,0, shift2));
   fMasterVolume->AddNode(fRingCoil.tube_,3,new TGeoTranslation(0,0,-shift1));
   fMasterVolume->AddNode(fRingCoil.tube_,4,new TGeoTranslation(0,0,-shift2));
   
   fRingCoil.counts_ = 4;
  
   cout << "-I- <SpdHybMagnet::ConstructGeometry_1> Geometry type: " << fGeoType << endl;
   
   return kTRUE;
}

//_____________________________________________________________________________
Bool_t SpdHybMagnet::ConstructGeometry_2() // private
{
   if (!BuildRingCoil(fRingCoil)) return kFALSE;
   
   Double_t shiftc = fRingCoilAxialDistanceC  + 0.5*fRingCoil.coil_width_;
   Double_t shift1 = fRingCoilAxialDistanceL1 + 0.5*fRingCoil.coil_width_;
   Double_t shift2 = fRingCoilAxialDistanceL2 + 0.5*fRingCoil.coil_width_;
   
   fMasterVolume->AddNode(fRingCoil.tube_,1,new TGeoTranslation(0,0, shiftc));
   fMasterVolume->AddNode(fRingCoil.tube_,2,new TGeoTranslation(0,0, shift1));
   fMasterVolume->AddNode(fRingCoil.tube_,3,new TGeoTranslation(0,0, shift2));
   fMasterVolume->AddNode(fRingCoil.tube_,4,new TGeoTranslation(0,0,-shiftc));
   fMasterVolume->AddNode(fRingCoil.tube_,5,new TGeoTranslation(0,0,-shift1));
   fMasterVolume->AddNode(fRingCoil.tube_,6,new TGeoTranslation(0,0,-shift2));
   
   fRingCoil.counts_ = 6;
  
   cout << "-I- <SpdHybMagnet::ConstructGeometry_2> Geometry type: " << fGeoType << endl;
   
   return kTRUE;
}

//_____________________________________________________________________________
Bool_t SpdHybMagnet::BuildLoopCoil(LoopCoil& coil) // private
{ 
   // create coil box
   TGeoMedium* box_material = FindMedium(coil.base_material_,"air");
   
   if (!box_material) {
       cout << "-E- <SpdHybMagnet::BuildLoopCoil> "
            << "Undefined \"box\" material: " << coil.base_material_ << endl;
       return kFALSE;
   }
  
   TString volname = FullName("LOOPBOX",kFALSE);
  
   coil.box_ = gGeoManager->MakeBox(volname, box_material, 
                                    coil.hxbox_, coil.hybox_, coil.hzbox_);
   // create coil
   TGeoMedium* coil_material = FindMedium(coil.coil_material_,"iron");
   
   coil.density_ = coil_material->GetMaterial()->GetDensity();
   
   if (!coil_material) {
       cout << "-E- <SpdHybMagnet::BuildLoopCoil> "
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
   
        volname = Form("ALOOPCOILSEG%d",i);

        coil.aparts_[i] = new TGeoVolume(volname /*name*/, shape, coil_material);
        
        coil.capacity_ += coil.aparts_[i]->Capacity();
   }
   
   volname = Form("BLOOPCOILBOX%d",1);
   
   coil.bparts_[0] = gGeoManager->MakeBox(volname, coil_material,
                     coil.hxcoil_-rmax, 0.5*coil.coil_width_, coil.hzcoil_);
                  
   volname = Form("BLOOPCOILBOX%d",2); 
   
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

//_____________________________________________________________________________
Bool_t SpdHybMagnet::BuildRingCoil(RingCoil& coil) // private
{ 
   // create coil
   TGeoMedium* coil_material = FindMedium(coil.coil_material_,"iron");
   
   coil.density_ = coil_material->GetMaterial()->GetDensity();
   
   if (!coil_material) {
       cout << "-E- <SpdHybMagnet::BuildRingCoil> "
            << "Undefined \"coil\" material: " << coil.coil_material_ << endl;
       return kFALSE;    
   }
   
   TString volname = FullName("RINGCOIL",kFALSE);
  
   Double_t len = 0.5*coil.coil_width_;
   Double_t rmax = coil.coil_max_radius_;
   Double_t rmin = rmax - coil.coil_thickness_;
   
   coil.tube_ = gGeoManager->MakeTube(volname, coil_material, rmin, rmax, len);
   
   coil.capacity_ = coil.tube_->Capacity();
   coil.mass_ = coil.capacity_*coil.density_;
   
   coil.SetFillPars();
   
   return kTRUE;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________
void SpdHybMagnet::SetGeometryType(Int_t type)
{
   if (fLockGeom) {
       cout << "-E- <SpdHybMagnet::SetGeometryType> "
            << " Geometry is already built " << endl;  
       return;
   }
   
   if (type < 1) {
       //cout << "-I- <SpdHybMagnet::SetGeometryType> No geometry will be built" << endl;
       SetGeoPars(0);
       return;
   }
   else if (type == 1) SetGeoPars(type);
   else if (type == 2) SetGeoPars(type);
   else if (type == 3) SetGeoPars(type);
   else {
       cout << "-E- <SpdHybMagnet::SetGeometryType> "
            << " Unknown geometry type: " << type << endl; 
       SetGeoPars(0); 
   }
}

//_____________________________________________________________________________
void SpdHybMagnet::UnsetMaterials(TString media)
{
   if (fLockGeom) {
       cout << "-E- <SpdHybMagnet::UnsetMaterials> "
            << " Geometry is already built " << endl;  
       return;
   }  
   
   //cout << "-I- <SpdHybMagnet::UnsetMaterials> " << media << endl;
     
   fLoopCoil.base_material_ = media;
   fLoopCoil.coil_material_ = media;   
   fRingCoil.coil_material_ = media;
   
   fUnsetMedia = media;
}

//_____________________________________________________________________________
void SpdHybMagnet::ResetMaterials()
{
   if (fLockGeom) return;
   SetDefaultMaterials(fGeoType);
}

//_____________________________________________________________________________
void SpdHybMagnet::SetCoilsMaterial(TString material)
{
   if (fLockGeom) {
       cout << "-E- <SpdHybMagnet::SetCoilsMaterial> "
            << " Geometry is already built " << endl;  
       return;
   }
   
   TGeoMedium* medium = FindMedium(material,"");
   
   if (!medium) {
       cout << "-E- <SpdHybMagnet::SetCoilsMaterial> " 
            << " The material: \"" << material << "\" is not defined " << endl;
       return;     
   }
  
   fLoopCoil.base_material_ = material;
   fLoopCoil.coil_material_ = material;
   fRingCoil.coil_material_ = material;
}

//_____________________________________________________________________________
void SpdHybMagnet::SetRingCoilThickness(Double_t thickness /*cm*/)
{
   if (fLockGeom) {
       cout << "-E- <SpdHybMagnet::SetRingCoilThickness> "
            << " Geometry is already built " << endl;  
       return;
   }
   
   if (thickness < 0.1) {
       cout << "-W- <SpdHybMagnet::SetRingCoilThickness> "
            << " The thickness should be more than 0.1 [cm] " << endl;  
       thickness = 0.1;   
   }

   fRingCoil.coil_thickness_ = 0.5*thickness;
}

//_____________________________________________________________________________
void SpdHybMagnet::SetRingCoilWidth(Double_t width /*cm*/)
{
   if (fLockGeom) {
       cout << "-E- <SpdHybMagnet::SetRingCoilWidth> "
            << " Geometry is already built " << endl;  
       return;
   }
   
   if (width < 1) {
       cout << "-W- <SpdHybMagnet::SetRingCoilWidth> "
            << " The width should be more than 1 [cm] " << endl;  
       width = 1.;
   }
   
   fRingCoil.coil_width_ = width;
}

//_____________________________________________________________________________
void SpdHybMagnet::SetRingCoilAxialDistanceL1(Double_t d /*cm*/)
{
   if (fLockGeom) {
       cout << "-E- <SpdHybMagnet::SetRingCoilAxialDistanceL1> "
            << " Geometry is already built " << endl;  
       return;
   }

   Double_t dd = 0.5*GetCoilWidth(1);

   if (d < dd) {
       cout << "-W- <SpdHybMagnet::SetRingCoilAxialDistanceL1> "
            << " The distance should be more than coil width/2 " << endl;  
       d = dd;
   }

   fRingCoilAxialDistanceL1 = d;
}

//_____________________________________________________________________________
void SpdHybMagnet::SetRingCoilAxialDistanceL2(Double_t d /*cm*/)
{
   if (fLockGeom) {
       cout << "-E- <SpdHybMagnet::SetRingCoilAxialDistanceL2> "
            << " Geometry is already built " << endl;  
       return;
   }
   
   Double_t dd = fRingCoilAxialDistanceL1 + GetCoilWidth(1);
   
   if (d < dd) {
       cout << "-W- <SpdHybMagnet::SetRingCoilAxialDistanceL2> "
            << " The distance should be more than " << dd << endl;  
       d = dd;
   }

   fRingCoilAxialDistanceL2 = d;
}

//_____________________________________________________________________________
void SpdHybMagnet::SetRingCoilAxialDistanceC(Double_t d /*cm*/)
{
   if (fLockGeom) {
       cout << "-E- <SpdHybMagnet::SetRingCoilAxialDistanceC> "
            << " Geometry is already built " << endl;  
       return;
   }
   
   Double_t dd = 0.5*GetCoilWidth(1);
   
   if (d < dd) {
       cout << "-W- <SpdHybMagnet::SetRingCoilAxialDistanceC> "
            << " The distance should be more than coil width/2 " << endl;  
       d = dd;
   }

   fRingCoilAxialDistanceC = d;
}

//_____________________________________________________________________________
void SpdHybMagnet::SetLoopCoilBaseMaterial(TString material)
{
   if (fLockGeom) {
       cout << "-E- <SpdHybMagnet::SetLoopCoilBaseMaterial> "
            << " Geometry is already built " << endl;  
       return;
   }
   
   TGeoMedium* medium = FindMedium(material,"");
   
   if (!medium) {
       cout << "-E- <SpdHybMagnet::SetLoopCoilBaseMaterial> " 
            << " The material: \"" << material << "\" is not defined " << endl;
       return;     
   }
   
   fLoopCoil.base_material_ = material;
}


//_____________________________________________________________________________
void SpdHybMagnet::SetLoopCoilAxialDistance(Double_t d /*cm*/)
{
   if (fLockGeom) {
       cout << "-E- <SpdHybMagnet::SetLoopCoilAxialDistance> "
            << " Geometry is already built " << endl;  
       return;
   }
   
   if (d < 10) {
       cout << "-W- <SpdHybMagnet::SetLoopCoilAxialDistance> "
            << " The distance should be more than 10 [cm] " << endl;  
       d = 10.;
   }
   
   fLoopCoilAxialDistance = d;
}

//_____________________________________________________________________________
void SpdHybMagnet::SetLoopCoilWidth(Double_t width /*cm*/)
{
   if (fLockGeom) {
       cout << "-E- <SpdHybMagnet::SetLoopCoilWidth> "
            << " Geometry is already built " << endl;  
       return;
   }
   
   if (width < 1) {
       cout << "-W- <SpdHybMagnet::SetLoopCoilWidth> "
            << " The width should be more than 1 [cm] " << endl;  
       width = 1.;
   }
   
   Double_t l;
   
   l = fLoopCoil.hxcoil_ - fLoopCoil.inner_rounding_;
   if (width > l) width = l;
        
   l = fLoopCoil.hycoil_ - fLoopCoil.inner_rounding_;
   if (width > l) width = l;
              
   fLoopCoil.coil_width_ = width;
}

//_____________________________________________________________________________
void SpdHybMagnet::SetLoopCoilThickness(Double_t thickness /*cm*/)
{
   if (fLockGeom) {
       cout << "-E- <SpdHybMagnet::SetLoopCoilThickness> "
            << " Geometry is already built " << endl;  
       return;
   }
   
   if (thickness < 0.1) {
       cout << "-W- <SpdHybMagnet::SetLoopCoilThickness> "
            << " The thickness should be more than 0.1 [cm] " << endl;  
       thickness = 0.1;   
   }
   
   thickness *= 0.5;
   
   if (fLoopCoil.hzbox_ < thickness) fLoopCoil.hzbox_ = thickness;     
   fLoopCoil.hzcoil_ = thickness;
}

//_____________________________________________________________________________
void SpdHybMagnet::SetLoopCoilInnerRounding(Double_t r /*cm*/)
{
   if (fLockGeom) {
       cout << "-E- <SpdHybMagnet::SetLoopCoilRounding> "
            << " Geometry is already built " << endl;  
       return;
   }
   
   if (r < 0) {
       cout << "-W- <SpdHybMagnet::SetLoopCoilRounding> "
            << " The rounding radius should be more than 0 [cm] " << endl;  
       r = 0;     
   }
   
   Double_t d;
   
   d = fLoopCoil.hxcoil_ - fLoopCoil.coil_width_;
   if (r > d)
   {  
       //cout << "-W- <SpdHybMagnet::SetLoopCoilRounding> "
       //     << " The rounding radius for coil of type " << i 
       //     << " should be < " << d << " [cm] "<< endl; 
       r = d;
   }
   
   d = fLoopCoil.hycoil_ - fLoopCoil.coil_width_;
   if (r > d)
   {
       //cout << "-W- <SpdHybMagnet::SetLoopCoilRounding> "
       //     << " The rounding radius for coil of type " << i 
       //     << " should be < " << d << " [cm] "<< endl; 
       r = d;
   }
   
   fLoopCoil.inner_rounding_ = r;
}

//_____________________________________________________________________________
void SpdHybMagnet::StretchLoopCoilAlongR(Double_t dr /*cm*/)
{
   if (fLockGeom) {
       cout << "-E- <SpdHybMagnet::StretchLoopCoilAlongR> "
            << " Geometry is already built " << endl;  
       return;
   }
   
   Double_t r_min = fLoopCoil.inner_rounding_ + fLoopCoil.coil_width_;     
   Double_t r = fLoopCoil.hybox_ + 0.5*dr;
   
   if (r < r_min) 
   {
      //cout << "-W- <SpdHybMagnet::StretchLoopCoilAlongR> "
      //     << " New R-size for the coil of type = " << i 
      //     << " should be > " << r_min  << " [cm] "<< endl; 
           
      fLoopCoil.hybox_  = r_min;
      fLoopCoil.hycoil_ = r_min;
   }
   else {        
      fLoopCoil.hybox_  += 0.5*dr;
      fLoopCoil.hycoil_ += 0.5*dr;  
   }
}
    
//_____________________________________________________________________________   
void SpdHybMagnet::PurseLoopCoilAlongZ(Double_t dz /*cm*/)
{
   if (fLockGeom) {
       cout << "-E- <SpdHybMagnet::PurseLoopCoilAlongZ> "
            << " Geometry is already built " << endl;  
       return;
   }  
   
   if (dz < 0) dz = -dz;
  
   Double_t l_min = fLoopCoil.inner_rounding_ + fLoopCoil.coil_width_;     
   Double_t l = fLoopCoil.hxcoil_ - 0.5*dz;
   
   if (l < l_min) 
   {
      //cout << "-W- <SpdHybMagnet::PurseLoopCoilAlongZ> "
      //     << " New Z_size for the coil of type = " << i 
      //     << " should be > " << l_min  << " [cm] "<< endl; 
   
      fLoopCoil.hxcoil_ = l_min;
   }
   else 
   {        
      fLoopCoil.hxcoil_ -= 0.5*dz;  
   }
}

//_____________________________________________________________________________
Double_t SpdHybMagnet::GetCapacity() const // in cm^3
{ 
   return GetCapacity(0) + GetCapacity(1);
}

//_____________________________________________________________________________  
Double_t SpdHybMagnet::GetMass() const  // in g
{
   return GetMass(0) + GetMass(1);
}

//_____________________________________________________________________________  
Double_t SpdHybMagnet::GetCapacity(Int_t ncoil) const  // cm^3
{
   if (ncoil == 0) {
       if (!fLoopCoil.box_) return 0;
       return fLoopCoil.capacity_ * fLoopCoil.counts_;
   }
   else {
       if (!fRingCoil.tube_) return 0;
       return fRingCoil.capacity_ * fRingCoil.counts_;
   }
}

//_____________________________________________________________________________    
Double_t SpdHybMagnet::GetMass(Int_t ncoil) const  // g
{
   if (ncoil == 0) {
       if (!fLoopCoil.box_) return 0;
       return fLoopCoil.capacity_ * fLoopCoil.density_ * fLoopCoil.counts_;
   }
   else {
       if (!fRingCoil.tube_) return 0;
       return fRingCoil.capacity_ * fRingCoil.density_ * fRingCoil.counts_;
   }
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________    
void SpdHybMagnet::Print(Option_t*) const
{
   cout << "<SpdHybMagnet::Print> Name/Prefix: "  << fName << "/" << fTitle << endl;
   
   TString divider('-',150);
   
   cout << "\n";
   
   cout << "\tGeometry type:           " << fGeoType << endl;
   cout << "\tUnique module Id:        " << fModuleId << endl; 
   cout << "\tUnset media:             " << fUnsetMedia << endl;
   
   if (fGeoType < 1) { cout << endl; return; }
   
   if (fGeoType == 1) {
       cout << "\tDistance to axis (min., toroidal):   " << fLoopCoilAxialDistance  << " [cm] " << endl;
   }
   else if (fGeoType == 2) {
       cout << "\tDistance to vertex (central ring):   " << fRingCoilAxialDistanceC << " [cm] " << endl;
   }
   
   cout << "\tDistance to vertex (lateral ring 1): " << fRingCoilAxialDistanceL1 << " [cm] " << endl;
   cout << "\tDistance to vertex (lateral ring 2): " << fRingCoilAxialDistanceL2 << " [cm] " << endl;  
 
   cout << "\n" << divider.Data() << "\n";
   
   printf("%6s %4s %15s %14s %14s %14s %14s %14s %14s %14s %14s \n\n",
          "Type","N","Material","Dens [g/cm^3]","Volume [m^3]","Mass [kg]",
          "X-size [m]","Y-size [m]","Thick. [cm]","Width [cm]", "Rounding [cm]");
   
   if (fLoopCoil.box_) {
       
       printf("%6s %4d %15s %14.6f %14.6f %14.6f %14.6f %14.6f %14.6f %14.6f %14.6f\n",
               "Loop", fLoopCoil.counts_, fLoopCoil.coil_material_.Data(),
               fLoopCoil.density_, fLoopCoil.capacity_*1e-6, fLoopCoil.mass_*1e-3,
               2*fLoopCoil.hxcoil_*1e-2, 2*fLoopCoil.hycoil_*1e-2, 2*fLoopCoil.hzcoil_,
               fLoopCoil.coil_width_,fLoopCoil.inner_rounding_
             );
       printf("%11s %15s %44s %14.6f %14.6f %14.6f \n"," ",fLoopCoil.base_material_.Data()," ",
               2*fLoopCoil.hxbox_*1e-2, 2*fLoopCoil.hybox_*1e-2, 2*fLoopCoil.hzbox_);
       
       printf("\n%6s %35s %14.3e %14.3e \n"," ","Total:",GetCapacity(0)*1e-6,GetMass(0)*1e-3);
   }
   
   if (fRingCoil.tube_) {
       
       printf("\n");
       printf("%6s %4d %15s %14.6f %14.6f %14.6f %14.6f %14.6f %14.6f %14.6f\n",
               "Ring", fRingCoil.counts_, fRingCoil.coil_material_.Data(),
               fRingCoil.density_, fRingCoil.capacity_*1e-6, fRingCoil.mass_*1e-3,
               fRingCoil.coil_max_radius_*1e-2, fRingCoil.coil_max_radius_*1e-2, 
               fRingCoil.coil_thickness_,
               fRingCoil.coil_width_
             );
       printf("\n%6s %35s %14.3e %14.3e \n"," ","Total:",GetCapacity(1)*1e-6,GetMass(1)*1e-3);
   }
   
   cout << divider.Data() << "\n";
  
   printf("%6s %35s %14.3e %14.3e \n","TOTAL:","",GetCapacity()*1e-6,GetMass()*1e-3);
   
   cout << divider.Data() << "\n";
   
   cout << "\n";
}

//_____________________________________________________________________________
void SpdHybMagnet::SetGeoPars(Int_t geotype) // private
{ 
   //cout <<"<SpdHybMagnet::SetGeoPars> geometry type: " << geotype << endl;
    
   fGeoType = 0;
       
   fLoopCoilAxialDistance   = 0;
   fRingCoilAxialDistanceL1 = 0;
   fRingCoilAxialDistanceL2 = 0;    
   fRingCoilAxialDistanceC  = 0;
       
   if (geotype < 1 && geotype > 3) {
       cout << "-I- <SpdHybMagnet::SetGeoPars> No geometry will be built " << endl;          
       return;
   }
    
   fGeoType = geotype;
   
   if (fGeoType == 1) {
    
       fLoopCoilAxialDistance = SpdCommonGeoMapper::GetHybMagnetDistToAxis();

       fLoopCoil.hxbox_ = SpdCommonGeoMapper::GetHybMagnetLen()/2.;
       fLoopCoil.hybox_ = SpdCommonGeoMapper::GetHybMagnetSize()/2.; 
       fLoopCoil.hzbox_ = SpdCommonGeoMapper::GetHybMagnetCoilThickness()/2.;  
       
       fLoopCoil.hxcoil_ = SpdCommonGeoMapper::GetHybMagnetCoilLen()/2.; 
       fLoopCoil.hycoil_ = fLoopCoil.hybox_; 
       fLoopCoil.hzcoil_ = fLoopCoil.hzbox_;
          
       fLoopCoil.coil_width_     = SpdCommonGeoMapper::GetHybMagnetCoilWidth();
       fLoopCoil.inner_rounding_ = SpdCommonGeoMapper::GetHybMagnetCoilRR();
       
       fLoopCoil.base_material_ = "air";
       fLoopCoil.coil_material_ = SpdCommonGeoMapper::GetHybMagnetCoilMaterial();
       
       //fLoopCoil.fill_color_ = kBlue+3;
       //fLoopCoil.line_color_ = kBlue+3;
       //fLoopCoil.transparency_ = 10;
       
       fRingCoilAxialDistanceL1 = SpdCommonGeoMapper::GetHybMagnetDistToAxis1();
       fRingCoilAxialDistanceL2 = SpdCommonGeoMapper::GetHybMagnetDistToAxis2();
       
       fRingCoil.coil_max_radius_ = SpdCommonGeoMapper::GetHybMagnetRingSize(); 
       fRingCoil.coil_width_      = SpdCommonGeoMapper::GetHybMagnetCoilWidth();
       fRingCoil.coil_thickness_  = SpdCommonGeoMapper::GetHybMagnetCoilThickness();  
       
       fRingCoil.coil_material_   = SpdCommonGeoMapper::GetHybMagnetCoilMaterial();
       
       //fRingCoil.fill_color_ = kBlue+3;
       //fRingCoil.line_color_ = kBlue+3;
       //fRingCoil.transparency_ = 10;
   }
   
   else if (fGeoType == 2) {
     
       fRingCoil.coil_max_radius_ = SpdCommonGeoMapper::GetHybMagnetRingSize(); 
       fRingCoil.coil_width_      = SpdCommonGeoMapper::GetHybMagnetCoilWidth();
       fRingCoil.coil_thickness_  = SpdCommonGeoMapper::GetHybMagnetCoilThickness();  
     
       fRingCoilAxialDistanceC  = SpdCommonGeoMapper::GetHybMagnetDistToAxisC();  
       fRingCoilAxialDistanceL1 = 3*fRingCoilAxialDistanceC + fRingCoil.coil_width_;
       fRingCoilAxialDistanceL2 = 5*fRingCoilAxialDistanceC + 2*fRingCoil.coil_width_;
       
       fRingCoil.coil_material_   = SpdCommonGeoMapper::GetHybMagnetCoilMaterial(); 
       
       //fRingCoil.fill_color_ = kBlue+3;
       //fRingCoil.line_color_ = kBlue+3;
       //fRingCoil.transparency_ = 10;
   }
   
   else if (fGeoType == 3) {
      
       fRingCoil.coil_max_radius_ = SpdCommonGeoMapper::GetHybMagnetRingSize(); 
       fRingCoil.coil_width_      = SpdCommonGeoMapper::GetHybMagnetCoilWidth();
       fRingCoil.coil_thickness_  = SpdCommonGeoMapper::GetHybMagnetCoilThickness();  
     
       fRingCoilAxialDistanceC  = SpdCommonGeoMapper::GetHybMagnetDistToAxis();  
       fRingCoilAxialDistanceL1 = SpdCommonGeoMapper::GetHybMagnetDistToAxis1();  
       fRingCoilAxialDistanceL2 = SpdCommonGeoMapper::GetHybMagnetDistToAxis2();  
       
       fRingCoil.coil_material_   = SpdCommonGeoMapper::GetHybMagnetCoilMaterial(); 
       
       //fRingCoil.fill_color_ = kBlue+3;
       //fRingCoil.line_color_ = kBlue+3;
       //fRingCoil.transparency_ = 10;
   }
}

//_____________________________________________________________________________
void SpdHybMagnet::SetDefaultMaterials(Int_t geotype) // private
{ 
   //cout <<"<SpdHybMagnet::SetDefaultMaterials> geometry type: " << geotype << endl;
    
   if (geotype != 1 && geotype != 2) {
       cout << "-I- <SpdHybMagnet::DefaultMaterials> No geometry will be built " << endl;          
       return;
   }
    
   if (geotype == 1) {
    
       fLoopCoil.base_material_ = "air";
       fLoopCoil.coil_material_ = SpdCommonGeoMapper::GetHybMagnetCoilMaterial();
       
       //fLoopCoil.fill_color_ = kBlue+3;
       //fLoopCoil.line_color_ = kBlue+3;
       //fLoopCoil.transparency_ = 10;
       
       fRingCoil.coil_material_   = SpdCommonGeoMapper::GetHybMagnetCoilMaterial();
       
       //fRingCoil.fill_color_ = kBlue+3;
       //fRingCoil.line_color_ = kBlue+3;
       //fRingCoil.transparency_ = 10;
   }
   
   else if (fGeoType == 2) {
     
       fRingCoil.coil_material_   = SpdCommonGeoMapper::GetHybMagnetCoilMaterial(); 
       
       //fRingCoil.fill_color_ = kBlue+3;
       //fRingCoil.line_color_ = kBlue+3;
       //fRingCoil.transparency_ = 10;
   }
}

//_____________________________________________________________________________
Bool_t SpdHybMagnet::FillParsIn(SpdPassiveGeoParSet* params)
{
   if (!SpdPassiveModule::FillParsIn(params)) return kFALSE;
   return kTRUE;
}

//_____________________________________________________________________________    
Bool_t SpdHybMagnet::LoadParsFrom(SpdPassiveGeoParSet* params)
{
   if (!SpdPassiveModule::LoadParsFrom(params)) return kFALSE;
   return kTRUE;
}



