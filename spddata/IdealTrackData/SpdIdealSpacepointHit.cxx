// $Id$
// Author: artur   2019/09/06

//_____________________________________________________________________________
//
// SpdIdealSpacepointHit
//_____________________________________________________________________________

#include "SpdIdealSpacepointHit.h"

#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdIdealSpacepointHit)

//_____________________________________________________________________________
SpdIdealSpacepointHit::SpdIdealSpacepointHit():SpdIdealHit(),
fLn(1),fLu(1,0,0),fLv(0,1,0)
{
}

//_____________________________________________________________________________
TVector3 SpdIdealSpacepointHit::GetN(Bool_t unit) const 
{ 
  TVector3 N = fLu.Cross(fLv);  
  if (unit) return N.Unit(); 
  N.SetMag(fLn); 
  return N;
}

//_____________________________________________________________________________
Double_t SpdIdealSpacepointHit::GetHitU() const
{
  TVector3 u = fLu.Unit(); 
  TVector3 p = fHit - fDetPos;
  return p.Dot(u);
}

//_____________________________________________________________________________   
Double_t SpdIdealSpacepointHit::GetHitV() const
{
  TVector3 v = fLv.Unit();
  TVector3 p = fHit - fDetPos;
  return p.Dot(v);
}

//_____________________________________________________________________________    
Double_t SpdIdealSpacepointHit::GetHitN() const
{
  TVector3 n = GetN(kTRUE);
  TVector3 p = fHit - fDetPos;
  return p.Dot(n);
}

//_____________________________________________________________________________    
void SpdIdealSpacepointHit::GetHitUV(Double_t& xu, Double_t& xv) const
{
  TVector3 p = fHit - fDetPos;
  TVector3 u = fLu.Unit(), v = fLv.Unit();
  xu = p.Dot(u);
  xv = p.Dot(v);
}

//_____________________________________________________________________________       
void SpdIdealSpacepointHit::GetHitUVN(Double_t& xu, Double_t& xv, Double_t& xn) const
{
  TVector3 p = fHit - fDetPos;
  TVector3 u = fLu.Unit(), v = fLv.Unit(), n = GetN(kTRUE);
  xu = p.Dot(u);
  xv = p.Dot(v);
  xn = p.Dot(n);
}

//_____________________________________________________________________________
Double_t SpdIdealSpacepointHit::GetPointU() const
{
  TVector3 u = fLu.Unit(); 
  TVector3 p = fPoint - fDetPos;
  return p.Dot(u);
}

//_____________________________________________________________________________   
Double_t SpdIdealSpacepointHit::GetPointV() const
{
  TVector3 v = fLv.Unit();
  TVector3 p = fPoint - fDetPos;
  return p.Dot(v);
}

//_____________________________________________________________________________    
Double_t SpdIdealSpacepointHit::GetPointN() const
{
  TVector3 n = GetN(kTRUE);
  TVector3 p = fPoint - fDetPos;
  return p.Dot(n);
}

//_____________________________________________________________________________    
void SpdIdealSpacepointHit::GetPointUV(Double_t& xu, Double_t& xv) const
{
  TVector3 p = fPoint - fDetPos;
  TVector3 u = fLu.Unit(), v = fLv.Unit();
  xu = p.Dot(u);
  xv = p.Dot(v);
}

//_____________________________________________________________________________       
void SpdIdealSpacepointHit::GetPointUVN(Double_t& xu, Double_t& xv, Double_t& xn) const
{
  TVector3 p = fPoint - fDetPos;
  TVector3 u = fLu.Unit(), v = fLv.Unit(), n = GetN(kTRUE);
  xu = p.Dot(u);
  xv = p.Dot(v);
  xn = p.Dot(n);
}

//_____________________________________________________________________________    
void SpdIdealSpacepointHit::PrintHit(Bool_t print_point, Bool_t print_res) const
{
   cout << "\t<" << this->ClassName() << "::PrintHit>" << "\n\n";
   cout << "\tHit unique ID:       " << fId << endl;
   cout << "\tDetector id:         "; for (Int_t i(0); i<fDetId.size(); i++) { cout << fDetId[i] << " "; } cout << endl;
   cout << "\tTrack id:            "; for (Int_t i(0); i<fTrackId.size(); i++) { cout << fTrackId[i] << " "; } cout << endl;
   cout << "\tInfo, mcpoint:       " << fInfoNum << ", " << fMotherPointNum << endl;
   cout << "\tEdep, Time:          " << fEdep << " [GeV],  " << fTimeStamp << " [ns] " << endl;
   cout << "\tSpecifity:           " << fSpecifity << endl;

   if (print_point) {
       cout << "\n";
       cout << "\tPoint(x,y,z):        " 
            << fPoint.X() << ", " << fPoint.Y() << ", " << fPoint.Z() << " [cm] " << endl;
       cout << "\tHit(x,y,z):          " 
            << fHit.X()   << ", " << fHit.Y() << ", " << fHit.Z() << " [cm] " << endl;            
   }
   if (print_res) {
       cout << "\tResolution:          " 
            << fResolution.X()*1e4 << ", " << fResolution.Y()*1e4  << ", " << fResolution.Z()*1e4  << " [mkm] " << endl;
   }
   
   TVector3 v;
 
   printf("\n");
   v = GetU();
   printf("\tu:    [%8.3f, %8.3f, %8.3f]  l(u) = %8.3f [cm]\n",v.X(),v.Y(),v.Z(),GetLu());
   v = GetV();
   printf("\tv:    [%8.3f, %8.3f, %8.3f]  l(v) = %8.3f [cm]\n",v.X(),v.Y(),v.Z(),GetLv());
   v = GetN();
   printf("\tn:    [%8.3f, %8.3f, %8.3f]  l(n) = %8.3f [cm]\n",v.X(),v.Y(),v.Z(),GetLn());
  
   printf("\n");
   printf("\tdet:  [%8.3f, %8.3f, %8.3f]  Dr   = %8.3f [cm]\n",fDetPos.X(),fDetPos.Y(),fDetPos.Z(),
                                                               fDetPos.XYvector().Mod());
   printf("\n");
}

