// $Id$
// Author: artur   2019/09/09


//_____________________________________________________________________________
//
// SpdGFMeasurementCreator
//_____________________________________________________________________________

#include "GFmeasurements.h"
#include "SpdGFMeasurementCreator.h"

#include <TRandom.h>
#include <TMath.h>

#include <iostream>

using std::cout;
using std::endl;

using namespace genfit;

ClassImp(SpdGFMeasurementCreator)

//_____________________________________________________________________________
SpdGFMeasurementCreator::SpdGFMeasurementCreator():fMeasurementCounter_(0),
fForcePointHitResolution(false),fForceWireHitResolution(false),
fUseMCPointAsPointHit(false),fUseMCPointAsWireHit(false),
fSmearPointHitResolution(false),fSmearWireHitResolution(false)
{   
   SetPointHitResolutionXYZ(0.005,0.005,0.005); // -> 50 mkm
   SetWireHitResolutionRZ(0.005,0.005);         // -> 50 mkm
}

//_____________________________________________________________________________
SpdGFMeasurementCreator::~SpdGFMeasurementCreator() 
{
 
}

//_____________________________________________________________________________
void SpdGFMeasurementCreator::ResetOptions() 
{
   fForcePointHitResolution = false;
   fForceWireHitResolution = false;
   fUseMCPointAsPointHit = false;
   fUseMCPointAsWireHit = false;
   fSmearPointHitResolution = false;
   fSmearWireHitResolution = false;
}

//_____________________________________________________________________________
std::vector<genfit::AbsMeasurement*> SpdGFMeasurementCreator::CreateHit(SpdIdealHit* hit, GFMeasurementType mtype)
{
   SpdIdealSpacepointHit* sphit = dynamic_cast<SpdIdealSpacepointHit*>(hit); 
   if (sphit) return CreateHit(sphit,mtype);
   
   SpdIdealWirepointHit* wphit = dynamic_cast<SpdIdealWirepointHit*>(hit); 
   if (wphit) return CreateHit(wphit,mtype);

   return CreateSpacepointHit(hit->GetPoint(),hit->GetResolution());
}

//_____________________________________________________________________________
std::vector<genfit::AbsMeasurement*> SpdGFMeasurementCreator::CreateHit(SpdIdealSpacepointHit* hit, GFMeasurementType mtype)
{
   if (mtype == pkPixel) return CreatePixelHit(hit);      
   if (mtype == pkSpacepoint) return CreateSpacepointHitP(hit);
   
   // default
   return CreatePixelHit(hit);
}

//_____________________________________________________________________________
std::vector<genfit::AbsMeasurement*> SpdGFMeasurementCreator::CreateHit(SpdIdealWirepointHit* hit, GFMeasurementType mtype)
{
   if (mtype == pkWire) return CreateWireHit(hit);
   if (mtype == pkWirePoint) return CreateWirePointHit(hit);   
   if (mtype == pkSpacepoint) return CreateSpacepointHitW(hit); 

   // default 
   return CreateWireHit(hit);   
}
    
//_____________________________________________________________________________
std::vector<AbsMeasurement*> SpdGFMeasurementCreator::CreateSpacepointHit(TVector3 point, TVector3 res, Char_t spec) 
{   
   std::vector<AbsMeasurement*> mm;

   //cout << "-I- <SpdGFMeasurementCreator::CreateSpacepointHit> " << endl;
   //cout << "\t specifity: " << spec << " resolution: " << res.X() << " " << res.Y() << " " << res.Z() << endl;

   TVectorD hitCoords(3);
   
   if (spec == 'n' || spec == 'p' || fSmearPointHitResolution) { 
       hitCoords(0) = gRandom->Gaus(point.X(),res.X());
       hitCoords(1) = gRandom->Gaus(point.Y(),res.Y());
       hitCoords(2) = gRandom->Gaus(point.Z(),res.Z());
   }
   else {
       hitCoords(0) = point.X();
       hitCoords(1) = point.Y();
       hitCoords(2) = point.Z();
   }
   
   TMatrixDSym hitCov(3);

   hitCov(0,0) = res.X()*res.X();
   hitCov(1,1) = res.Y()*res.Y();
   hitCov(2,2) = res.Z()*res.Z();

   AbsMeasurement* measurement = new SpacepointMeasurement(hitCoords, hitCov, pkSpacepoint, 
                                                           fMeasurementCounter_, nullptr);
   mm.push_back(measurement);

   return mm;
}

//_____________________________________________________________________________
std::vector<AbsMeasurement*> SpdGFMeasurementCreator::CreateSpacepointHitP(SpdIdealSpacepointHit* hit) 
{   
   std::vector<AbsMeasurement*> mm;

   //cout << "-I- <SpdGFMeasurementCreator::CreateSpacepointHitP> " << endl;
      
   TVector3 pos;
   if (fUseMCPointAsPointHit) hit->GetPoint(pos);
   else hit->GetHit(pos);   
   
   TVector3 res;
   if (fForcePointHitResolution) res = fPointHitResolution;
   else res = hit->GetResolution();
   
   Char_t spec = hit->GetSpecifity();
   
   //cout << "-I- <SpdGFMeasurementCreator::CreateSpacepointHitP>  specifity: " << spec 
   //     << " smear hit : " << fSmearPointHitResolution << " point as hit: " << fUseMCPointAsPointHit
   //     << "  position: " << pos.X() << " " << pos.Y() << " " << pos.Z() 
   //     << "  resolution: " << res.X() << " " << res.Y() << " " << res.Z() << endl;

   TVectorD hitCoords(3);
   
   if (spec == 'n' || spec == 'p' || fSmearPointHitResolution) { 
       hitCoords(0) = gRandom->Gaus(pos.X(),res.X());
       hitCoords(1) = gRandom->Gaus(pos.Y(),res.Y());
       hitCoords(2) = gRandom->Gaus(pos.Z(),res.Z());
   }
   else {
       hitCoords(0) = pos.X();
       hitCoords(1) = pos.Y();
       hitCoords(2) = pos.Z();
   }
    
   TMatrixDSym hitCov(3);

   hitCov(0,0) = res.X()*res.X();
   hitCov(1,1) = res.Y()*res.Y();
   hitCov(2,2) = res.Z()*res.Z();

   AbsMeasurement* measurement = new SpacepointMeasurement(hitCoords, hitCov, pkSpacepoint, 
                                                           fMeasurementCounter_, nullptr);
   mm.push_back(measurement);

   return mm;
}

//_____________________________________________________________________________
std::vector<AbsMeasurement*> SpdGFMeasurementCreator::CreateSpacepointHitW(SpdIdealWirepointHit* hit) 
{   
   std::vector<AbsMeasurement*> mm;

   //cout << "-I- <SpdGFMeasurementCreator::CreateSpacepointHitW> " << endl;
      
   TVector3 pos;
   if (fUseMCPointAsPointHit) hit->GetPoint(pos);
   else hit->GetHit(pos);   
   
   TVector3 res;
   if (fForcePointHitResolution) res = fPointHitResolution;
   else res = hit->GetResolution();
   
   Char_t spec = hit->GetSpecifity();
   
//    cout << "\t specifity: " << spec << " detector: " << hit->GetDetId()  
//         << "  position: " << pos.X() << " " << pos.Y() << " " << pos.Z()
//         << "  resolution: " << res.X() << " " << res.Y() << " " << res.Z() << endl;
   
   TVectorD hitCoords(3);
   
   if (spec == 'n' || spec == 'p' || fSmearPointHitResolution) { 
       hitCoords(0) = gRandom->Gaus(pos.X(),res.X());
       hitCoords(1) = gRandom->Gaus(pos.Y(),res.Y());
       hitCoords(2) = gRandom->Gaus(pos.Z(),res.Z());
   }
   else {
       hitCoords(0) = pos.X();
       hitCoords(1) = pos.Y();
       hitCoords(2) = pos.Z();
   }
    
   TMatrixDSym hitCov(3);

   hitCov(0,0) = res.X()*res.X();
   hitCov(1,1) = res.Y()*res.Y();
   hitCov(2,2) = res.Z()*res.Z();

   AbsMeasurement* measurement = new SpacepointMeasurement(hitCoords, hitCov, pkSpacepoint, 
                                                           fMeasurementCounter_, nullptr);
   mm.push_back(measurement);

   return mm;
}

//_____________________________________________________________________________
std::vector<genfit::AbsMeasurement*> SpdGFMeasurementCreator::CreatePixelHit(SpdIdealSpacepointHit* hit)
{
   std::vector<AbsMeasurement*> mm;

   //cout << "-I- <SpdGFMeasurementCreator::CreatePixelHit> " << endl;
   
   Char_t spec = hit->GetSpecifity();
   
   if (spec != 'p' && spec != 'm' && spec != 's') {
       cout << "-E- <SpdGFMeasurementCreator::CreatePixelHit> Unexpected hit specifity: '" << spec 
            << "' , create point-like hit " << endl;  
       return CreateSpacepointHit(hit->GetHit(),hit->GetResolution(),spec);
   }
   
   Double_t pos[2];
   if (fUseMCPointAsPointHit) { pos[0] = hit->GetPointU(); pos[1] = hit->GetPointV(); }
   else { pos[0] = hit->GetHitU(); pos[1] = hit->GetHitV(); }
   
   Double_t res[2];
   if (fForcePointHitResolution) { res[0] = fPointHitResolution.X(); res[1] = fPointHitResolution.Y(); } 
   else { res[0] = hit->GetResolutionU(); res[1] = hit->GetResolutionV(); }
   
   //cout << "-I- <SpdGFMeasurementCreator::CreatePixelHit> specifity: " << spec << " detector: " << hit->GetDetId()  
   //     << " smear hit : " << fSmearPointHitResolution << " point as hit: " << fUseMCPointAsPointHit
   //     << " resolution (U,V): " << res[0] << " " << res[1]
   //     << " hit [U, V]: " << pos[0] << " " << pos[1] << endl;
   
   //hit->PrintHit();
    
   TVectorD hitCoords(2);
      
   if (fSmearPointHitResolution) { 
       hitCoords(0) = gRandom->Gaus(pos[0],res[0]);
       hitCoords(1) = gRandom->Gaus(pos[1],res[1]);
   }
   else {
       hitCoords(0) = pos[0];
       hitCoords(1) = pos[1]; 
   }
       
   TMatrixDSym hitCov(2);
   
   hitCov(0,0) = res[0]*res[0];
   hitCov(1,1) = res[1]*res[1];

   PlanarMeasurement* measurement = new PlanarMeasurement(hitCoords, hitCov, hit->GetDetId(), fMeasurementCounter_, nullptr);
   
   SharedPlanePtr plane(new DetPlane(hit->GetDetPos(),hit->GetU(),hit->GetV()));
   
   measurement->setPlane(plane, fMeasurementCounter_);
   
   mm.push_back(measurement);
   
   return mm; 
}

//_____________________________________________________________________________
std::vector<genfit::AbsMeasurement*> SpdGFMeasurementCreator::CreateWireHit(SpdIdealWirepointHit* hit)
{
    std::vector<AbsMeasurement*> mm;
    
    //cout << "-I- <SpdGFMeasurementCreator::CreateWireHit> " << endl;

    Char_t spec = hit->GetSpecifity();
   
    if (spec != 'w') {
        cout << "-E- <SpdGFMeasurementCreator::CreateWireHit> Unexpected hit specifity: '" << spec 
            << "' , create point-like hit " << endl;   
        return CreateSpacepointHit(hit->GetHit(),hit->GetResolution(),spec);
    }
    
    Double_t rdrift;
    if (fUseMCPointAsWireHit) rdrift = hit->GetPointRdrift();
    else rdrift = hit->GetRdrift();
   
    Double_t res;
    if (fForceWireHitResolution) res = fWireHitResolution.X();
    else res = hit->GetResolutionR();
    
    //cout << "\t specifity: " << spec << " detector: " << hit->GetDetId() 
    //     << " resolution (R): " << res << " [R]: " << rdrift << endl;

    TVector3 wirepoint;
    
    TVectorD hitCoords(7);

    wirepoint = hit->GetWirePoint1();
    
    hitCoords(0) = wirepoint.X();
    hitCoords(1) = wirepoint.Y();
    hitCoords(2) = wirepoint.Z();
    
    wirepoint = hit->GetWirePoint2();

    hitCoords(3) = wirepoint.X();
    hitCoords(4) = wirepoint.Y();
    hitCoords(5) = wirepoint.Z();

    Double_t rmax = hit->GetWireMaxRadius();
    
    if (fSmearWireHitResolution) 
    {
        static Double_t ktol = 0.99999;
    
        if (res < rmax/TMath::Sqrt(12)) {
            hitCoords(6) = rdrift + gRandom->Gaus(0,res);
            if (hitCoords(6) < 0) hitCoords(6) = 0;
            else if (!(hitCoords(6) < rmax)) hitCoords(6) = ktol*rmax;
        }
        else hitCoords(6) = gRandom->Uniform(0,ktol*rmax); 
    }
    else hitCoords(6) = rdrift; 

    TMatrixDSym hitCov(7);
    
    hitCov(6,6) = res*res;

    WireMeasurement* measurement = new WireMeasurement(hitCoords, hitCov, hit->GetDetId(), fMeasurementCounter_, nullptr);
      
    measurement->setMaxDistance(rmax);
    //measurement->setLeftRightResolution(hit->GetLRtype());
    
    mm.push_back(measurement);
    
    return mm;
}

//_____________________________________________________________________________
std::vector<genfit::AbsMeasurement*> SpdGFMeasurementCreator::CreateWirePointHit(SpdIdealWirepointHit* hit)
{
    std::vector<AbsMeasurement*> mm;
    
    //cout << "-I- <SpdGFMeasurementCreator::CreateWirePointHit> " << endl;
    
    Char_t spec = hit->GetSpecifity();
   
    if (spec != 'w') {
        cout << "-E- <SpdGFMeasurementCreator::CreateWirePointHit> Unexpected hit specifity: '" << spec 
            << "' , create point-like hit " << endl;   
        return CreateSpacepointHit(hit->GetHit(),hit->GetResolution(),spec);
    }
    
    Double_t rdrift, zwire;
    if (fUseMCPointAsWireHit) { rdrift = hit->GetPointRdrift(); zwire = hit->GetPointZwire(); }
    else { rdrift = hit->GetRdrift(); zwire = hit->GetPointZwire(); }
   
    Double_t resR, resZ;
    if (fForceWireHitResolution) { resR = fWireHitResolution.X(); resZ = fWireHitResolution.Y(); }
    else { resR = hit->GetResolutionR(); resR = hit->GetResolutionZ(); }
    
    //cout << "\t specifity: " << spec << " detector: " << hit->GetDetId() 
    //     << " resolution (R,Z): " << resR << " " << resZ
    //     << "\t [R, Z]: " << rdrift << " " << zwire << " Wire length: " << hit->GetWireLength() << endl;   
         
    TVectorD hitCoords(8);
    
    TVector3 wirepoint;
    
    wirepoint = hit->GetWirePoint1();
    
    hitCoords(0) = wirepoint.X();
    hitCoords(1) = wirepoint.Y();
    hitCoords(2) = wirepoint.Z();
    
    wirepoint = hit->GetWirePoint2();

    hitCoords(3) = wirepoint.X();
    hitCoords(4) = wirepoint.Y();
    hitCoords(5) = wirepoint.Z();
    
    Double_t rmax = hit->GetWireMaxRadius();
    Double_t zmax = hit->GetWireLength();
    
    if (fSmearWireHitResolution) 
    {
        static Double_t ktol = 0.99999;
        
        if (resR < rmax/TMath::Sqrt(12)) {
            hitCoords(6) = rdrift + gRandom->Gaus(0,resR);
            if (hitCoords(6) < 0) hitCoords(6) = 0;
            else if (!(hitCoords(6) < rmax)) hitCoords(6) = ktol*rmax;
        }
        else hitCoords(6) = gRandom->Uniform(0,ktol*rmax);
        
        if (resZ < zmax/TMath::Sqrt(12)) {
            hitCoords(7) = zwire + gRandom->Gaus(0,resZ);
            if (hitCoords(7) < 0) hitCoords(7) = 0;
            else if (!(hitCoords(7) < zmax)) hitCoords(7) = ktol*zmax;
        }
        else hitCoords(7) = gRandom->Uniform(0,ktol*zmax); 
    }
    else {
        hitCoords(6) = rdrift;
        hitCoords(7) = zwire;        
    }
        
    TMatrixDSym hitCov(8);
    
    hitCov(6,6) = resR*resR;
    hitCov(7,7) = resZ*resZ;

    WirePointMeasurement* measurement = new WirePointMeasurement(hitCoords, hitCov, hit->GetDetId(), fMeasurementCounter_, nullptr);
      
    measurement->setMaxDistance(rmax);
    //measurement->setLeftRightResolution(hit->GetLRtype());
    
    mm.push_back(measurement);   
         
   return mm;      
}

// //_____________________________________________________________________________
// std::vector<genfit::AbsMeasurement*> SpdGFMeasurementCreator::CreateWireHit2(SpdIdealWirepointHit* hit)
// {
//     std::vector<AbsMeasurement*> mm;
//     
//     Char_t spec = hit->GetSpecifity();
//    
//     if (spec != 'w') {
//         cout << "<SpdGFMeasurementCreator::CreateWireHit2> Unexpected hit specifity: '" << spec 
//             << "' , create point-like hit " << endl;   
//         return CreateSpacepointHit(hit->GetHit(),hit->GetResolution(),spec);
//     }
//     
// //     cout << "\t specifity: " << spec << " detector: " << hit->GetDetId() 
// //         << " resolution (R): " << hit->GetResolutionR()  
// //         << " [R]: " << hit->GetRdrift() << endl;
// 
//     WireMeasurementNew* measurement = new WireMeasurementNew(hit->GetRdrift(),hit->GetResolutionR(),
//                                                              hit->GetWirePoint1(),hit->GetWirePoint2(), 
//                                                              hit->GetDetId(), fMeasurementCounter_, nullptr);
//     
//     measurement->setMaxDistance(hit->GetWireMaxRadius());
//     //measurement->setLeftRightResolution(hit->GetLRtype());
//     
//     mm.push_back(measurement);
//             
//     return mm;      
// }

