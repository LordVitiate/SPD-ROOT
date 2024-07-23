// $Id$
// Author: artur   2021/11/24


//_____________________________________________________________________________
//
// SpdMCSiliconHit
//_____________________________________________________________________________

#include "SpdMCSiliconHit.h"

#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdMCSiliconHit)

//_____________________________________________________________________________
SpdMCSiliconHit::SpdMCSiliconHit():SpdMCTrackHit(),
fU(0),fV(0),fResU(-1.e-4),fResV(-1.e-4),
fLn(1),fLu(1,0,0),fLv(0,1,0),fDelta(-1),fDetLayer(-1),
fSegLength(0)
{
 
}

//_____________________________________________________________________________
void SpdMCSiliconHit::Reset()
{
    SpdMCTrackHit::Reset();
    
    fU = 0.;
    fV = 0.;
    
    fResU = -1.e-4; // 1mkm
    fResV = -1.e-4; // 1mkm
    
    fLn = 1;
    fLu.SetXYZ(1,0,0);
    fLv.SetXYZ(0,1,0);
    
    fDetPos.SetXYZ(0,0,0);

    fDetLayer = -1;
    
    fDelta = -1;
}

//=============================================================================
// Hit
//=============================================================================

//_____________________________________________________________________________
void SpdMCSiliconHit::SetHitU(TVector3 hit) 
{
    TVector3 u = fLu.Unit(); 
    TVector3 p = hit - fDetPos;
    fU = p.Dot(u);
}

//_____________________________________________________________________________   
void SpdMCSiliconHit::SetHitV(TVector3 hit)
{
    TVector3 v = fLv.Unit();
    TVector3 p = hit - fDetPos;
    fV = p.Dot(v);
}

//_____________________________________________________________________________   
void SpdMCSiliconHit::SetHitUV(TVector3 hit)
{
    TVector3 p = hit - fDetPos;
    TVector3 x = fLu.Unit();
    fU = p.Dot(x);
    x = fLv.Unit();
    fV = p.Dot(x);
}

//_____________________________________________________________________________       
TVector3 SpdMCSiliconHit::GetHit() const 
{ 
    TVector3 p = fDetPos, u = fLu.Unit(), v = fLv.Unit();
    p += fU*u;
    p += fV*v; 
    return p;
}

//_____________________________________________________________________________       
Double_t SpdMCSiliconHit::GetHitZ() const 
{ 
    TVector3 p = fDetPos, u = fLu.Unit(), v = fLv.Unit();
    p += fU*u;
    p += fV*v; 
    return p.Z();
}

//=============================================================================
// Point
//=============================================================================

//_____________________________________________________________________________
Double_t SpdMCSiliconHit::GetPointU() const
{
    TVector3 u = fLu.Unit(); 
    TVector3 p = fMCPoint - fDetPos;
    return p.Dot(u);
}

//_____________________________________________________________________________   
Double_t SpdMCSiliconHit::GetPointV() const
{
    TVector3 v = fLv.Unit();
    TVector3 p = fMCPoint - fDetPos;
    return p.Dot(v);
}

//_____________________________________________________________________________    
Double_t SpdMCSiliconHit::GetPointN() const
{
    TVector3 n = GetN(kTRUE);
    TVector3 p = fMCPoint - fDetPos;
    return p.Dot(n);
}

//_____________________________________________________________________________    
void SpdMCSiliconHit::GetPointUV(Double_t& xu, Double_t& xv) const
{
    TVector3 p = fMCPoint - fDetPos;
    TVector3 u = fLu.Unit(), v = fLv.Unit();
    xu = p.Dot(u);
    xv = p.Dot(v);
}   

//_____________________________________________________________________________       
void SpdMCSiliconHit::GetPointUVN(Double_t& xu, Double_t& xv, Double_t& xn) const
{
    TVector3 p = fMCPoint - fDetPos;
    TVector3 u = fLu.Unit(), v = fLv.Unit(), n = GetN(kTRUE);
    xu = p.Dot(u);
    xv = p.Dot(v);
    xn = p.Dot(n);
}

//=============================================================================
// Detector
//=============================================================================

//_____________________________________________________________________________
TVector3 SpdMCSiliconHit::GetN(Bool_t unit) const 
{ 
    TVector3 N = fLu.Cross(fLv);  
    if (unit) return N.Unit(); 
    N.SetMag(fLn); 
    return N;
}

//_____________________________________________________________________________    
void SpdMCSiliconHit::Print(Option_t* option) const
{
    cout << "\t<" << this->ClassName() << "::Print> ---------------------------------- " << "\n\n";
    cout << "\tHit unique ID:         " << fId << endl;
    cout << "\tModule (detector) id:  " << fModId  << endl;
    cout << "\tDetector id:           "; for (Int_t i(0); i<fDetId.size(); i++) { cout << fDetId[i] << "  "; } cout << endl;
    cout << "\tDetector layer:        " << fDetLayer  << endl;
    cout << "\tHit time (error):      " << GetHitTime() << " (" << GetTimeStampError() << ")  [ns]" << endl;
    cout << "\tResponse:              " << fResp*1e6 << " [KeV] " << endl;
    cout << "\tWeight:                " << fWeight << endl;
    cout << "\tSpecifity:             " << fSpecifity << endl;
    cout << "\tMCPoint(id):           " << fMCPointId << endl;
    cout << "\tMCPoint(x,y,z):        " << fMCPoint.X() << ", " << fMCPoint.Y() << ", " << fMCPoint.Z() 
                                        << " (D0 = " << fMCPoint.Mag() << ") [cm] " << endl;
    cout << "\tHit(u,v):              " << fU << ", " << fV << " [cm] " << endl;
    cout << "\tResolution(u,v):       " << fResU*1e4 << ", " << fResV*1e4 << " [mkm] " << endl;
    cout << "\tDelta(h-p):            " << fDelta*1e4 << " [mkm] " << endl;
    cout << "\tDelta(h-p)(u,v):       " << GetDeltaU()*1e4 << ", " << GetDeltaV()*1e4 << " [mkm] " << endl;
    
    printf("\n");
 
    if (fMCTruth) fMCTruth->print();
    else cout << "\tPARTICLE: unknown " << endl;      
    
    TVector3 v;
    
    printf("\n\tDetector (position, local axes & dimensions): \n\n");
    v = GetU();
    printf("\tu:    [%8.3f, %8.3f, %8.3f]  l(u) = %8.3f [cm]\n",v.X(),v.Y(),v.Z(),GetLu());
    v = GetV();
    printf("\tv:    [%8.3f, %8.3f, %8.3f]  l(v) = %8.3f [cm]\n",v.X(),v.Y(),v.Z(),GetLv());
    v = GetN();
    printf("\tn:    [%8.3f, %8.3f, %8.3f]  l(n) = %8.3f [cm]\n",v.X(),v.Y(),v.Z(),GetLn());
    printf("\n");
    printf("\tdet:  [%8.3f, %8.3f, %8.3f]  R(0) = %8.3f [cm]\n",fDetPos.X(),fDetPos.Y(),fDetPos.Z(),
                                                                fDetPos.XYvector().Mod());
    printf("\n");
}


