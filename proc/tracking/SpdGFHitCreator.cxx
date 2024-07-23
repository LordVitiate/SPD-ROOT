// $Id$
// Author: artur   2021/02/08

//_____________________________________________________________________________
//
// SpdGFHitCreator
//_____________________________________________________________________________


#include "SpdGFHitCreator.h"
#include "GFmeasurements.h"

#include <TRandom.h>
#include <TMath.h>

#include <iostream>

using std::cout;
using std::endl;

using namespace genfit;

ClassImp(SpdGFHitCreator)

//_____________________________________________________________________________
SpdGFHitCreator::SpdGFHitCreator():fHitCounter(0),fLastHitId(0) 
{
   ClearCounter();
}

//_____________________________________________________________________________
SpdGFHitCreator::~SpdGFHitCreator() 
{
 
}

//_____________________________________________________________________________
void SpdGFHitCreator::ClearCounter()
{
   memset(fMeasCounter,0,5*sizeof(Int_t));
   memset(fMeasCounterErrors,0,5*sizeof(Int_t));  
}

//_____________________________________________________________________________
std::vector<genfit::AbsMeasurement*> SpdGFHitCreator::CreateHit(const SpdHit* hit)
{
   gf_measurements_ meas;
   
        if (dynamic_cast<const SpdMCSiliconHit*>(hit)) CreatePlanarHit((const SpdMCSiliconHit*)hit,meas);
   else if (dynamic_cast<const SpdMCStrawHit1D*>(hit)) CreateWireHit1D((const SpdMCStrawHit1D*)hit,meas);
   else if (dynamic_cast<const SpdMCStrawHit2D*>(hit)) CreateWireHit2D((const SpdMCStrawHit2D*)hit,meas);

   return meas;
}

//--------------------------------------
// Greate GF-measurements
//--------------------------------------

//_____________________________________________________________________________
void SpdGFHitCreator::CreatePlanarHit(const SpdMCSiliconHit* hit, gf_measurements_& meas)
{
   //cout << "-I- <SpdGFMeasurementCreator::CreatePlanarHit> " << endl;
   
   Char_t spec = hit->GetSpecifity();
   
   if (spec != 'p' && spec != 'm' && spec != 's') {
       cout << "-E- <SpdGFHitCreator::CreatePlanarHit> Unexpected hit specifity: " 
            << spec << endl;  
       fMeasCounterErrors[0]++;      
       fMeasCounterErrors[1]++;     
       return;
   }
   
   //hit->PrintHit();
    
   TVectorD hitCoords(2);
      
   hitCoords(0) = hit->GetPointU();
   hitCoords(1) = hit->GetPointV(); 
       
   TMatrixDSym hitCov(2);
   
   hitCov(0,0) = hit->GetResolutionU()*hit->GetResolutionU();
   hitCov(1,1) = hit->GetResolutionV()*hit->GetResolutionV();

   fMeasCounter[0]++;
   fMeasCounter[1]++; 
   
   fHitCounter++;
   fLastHitId++;
   
   PlanarMeasurement* m = new PlanarMeasurement(hitCoords, hitCov, hit->GetModId(), 
                                                fLastHitId, nullptr);
   
   SharedPlanePtr plane(new DetPlane(hit->GetDetPos(),hit->GetU(),hit->GetV()));
   m->setPlane(plane,fMeasCounter[0]);
   
   meas.push_back(m);
}

//_____________________________________________________________________________   
void SpdGFHitCreator::CreateWireHit1D(const SpdMCStrawHit1D* hit, gf_measurements_& meas)
{
   //cout << "-I- <SpdGFHitCreator::CreateWireHit1D> " << endl;

   Char_t spec = hit->GetSpecifity();
   
   if (spec != 'v') {
       cout << "-E- <SpdGFHitCreator::CreateWireHit1D> Unexpected hit specifity: " 
            << spec  << endl;
       fMeasCounterErrors[0]++;     
       fMeasCounterErrors[2]++;         
       return;
   }
   
   //hit->PrintHit();
   
   TVectorD hitCoords(7);

   TVector3 wirepoint = hit->GetWirePoint1();
   
   hitCoords(0) = wirepoint.X();
   hitCoords(1) = wirepoint.Y();
   hitCoords(2) = wirepoint.Z();
   
   wirepoint = hit->GetWirePoint2();

   hitCoords(3) = wirepoint.X();
   hitCoords(4) = wirepoint.Y();
   hitCoords(5) = wirepoint.Z();
       
   hitCoords(6) = hit->GetRdrift(); 

   TMatrixDSym hitCov(7);
   
   hitCov(6,6) = hit->GetResolutionR()*hit->GetResolutionR();
   
   fMeasCounter[0]++;
   fMeasCounter[2]++; 
   
   fHitCounter++;
   fLastHitId++;
     
   WireMeasurement* m = new WireMeasurement(hitCoords, hitCov, hit->GetModId(), 
                                            fLastHitId, nullptr);
     
   m->setMaxDistance(hit->GetMaxRadius());
   //m->setLeftRightResolution(hit->GetPointLRtype());
   
   meas.push_back(m);
}

//_____________________________________________________________________________    
void SpdGFHitCreator::CreateWireHit2D(const SpdMCStrawHit2D* hit, gf_measurements_& meas) 
{
    //cout << "-I- <SpdGFHitCreator::CreateWireHit2D> " << endl;
    
    Char_t spec = hit->GetSpecifity();
   
    if (spec != 'w') {
        cout << "-E- <SpdGFHitCreator::CreateWireHit2D> Unexpected hit specifity: " 
             << spec << endl;
        fMeasCounterErrors[0]++;      
        fMeasCounterErrors[3]++;     
        return;
    }
    
    //hit->PrintHit();
    
    TVectorD hitCoords(8);
    
    TVector3 wirepoint = hit->GetWirePoint1();
    
    hitCoords(0) = wirepoint.X();
    hitCoords(1) = wirepoint.Y();
    hitCoords(2) = wirepoint.Z();
    
    wirepoint = hit->GetWirePoint2();

    hitCoords(3) = wirepoint.X();
    hitCoords(4) = wirepoint.Y();
    hitCoords(5) = wirepoint.Z();
    
    TMatrixDSym hitCov(8);
    
    hitCov(6,6) = hit->GetResolutionR()*hit->GetResolutionR();
    hitCov(7,7) = hit->GetResolutionZ()*hit->GetResolutionZ();
    
    fMeasCounter[0]++;
    fMeasCounter[3]++;
    
    fHitCounter++;
    fLastHitId++;     

    WirePointMeasurement* m = new WirePointMeasurement(hitCoords, hitCov, hit->GetModId(), 
                                                       fLastHitId, nullptr);
      
    m->setMaxDistance(hit->GetMaxRadius());
    //m->setLeftRightResolution(hit->GetPointLRtype());
    
    meas.push_back(m);   
}

//_____________________________________________________________________________
std::vector<genfit::AbsMeasurement*> SpdGFHitCreator::CreateSpacepointHit(const SpdHit* hit, TVector3 res)
{
    if (dynamic_cast<const SpdMCTrackHit*>(hit)) return CreateSpacepointHit((const SpdMCTrackHit*)hit,res);
    gf_measurements_ meas;
    return meas;
}

//_____________________________________________________________________________
std::vector<genfit::AbsMeasurement*> SpdGFHitCreator::CreateSpacepointHit(const SpdMCTrackHit* hit, TVector3 res)
{
   //cout << "-I- <SpdGFHitCreator::CreateSpacepointHit> [SpdMCTrackHit] " << endl;

   gf_measurements_ meas;
   
   Char_t spec = hit->GetSpecifity();
   
   if (spec != 'n' && spec == 'p') {
       cout << "-E- <SpdGFHitCreator::CreateWireHit2D> Unexpected hit specifity: " 
             << spec << endl;
       fMeasCounterErrors[0]++;      
       fMeasCounterErrors[4]++;     
       return meas;    
   }
   
   TVectorD hitCoords(3);
   
   hitCoords(0) = hit->GetPointX();
   hitCoords(1) = hit->GetPointY();
   hitCoords(2) = hit->GetPointZ();
    
   TMatrixDSym hitCov(3);

   hitCov(0,0) = res.X()*res.X();
   hitCov(1,1) = res.Y()*res.Y();
   hitCov(2,2) = res.Z()*res.Z();
   
   fMeasCounter[0]++;
   fMeasCounter[4]++;
   
   fHitCounter++;
   fLastHitId++;   

   AbsMeasurement* m = new SpacepointMeasurement(hitCoords, hitCov, hit->GetModId(), fLastHitId, nullptr);
   meas.push_back(m);

   return meas;
}

//_____________________________________________________________________________    
std::vector<genfit::AbsMeasurement*> SpdGFHitCreator::CreateSpacepointHit(TVector3 point, TVector3 res)
{
   //cout << "-I- <SpdGFHitCreator::CreateSpacepointHit> [TVector3]" << endl;
  
   TVectorD hitCoords(3);
   
   hitCoords(0) = point.X();
   hitCoords(1) = point.Y();
   hitCoords(2) = point.Z();
    
   TMatrixDSym hitCov(3);

   hitCov(0,0) = res.X()*res.X();
   hitCov(1,1) = res.Y()*res.Y();
   hitCov(2,2) = res.Z()*res.Z();

   fMeasCounter[0]++;
   fMeasCounter[4]++;
   
   fHitCounter++;
   fLastHitId++; 

   AbsMeasurement* m = new SpacepointMeasurement(hitCoords, hitCov, -1, fLastHitId, nullptr);
   
   gf_measurements_ meas;
   meas.push_back(m);
   
   return meas;
}

//_____________________________________________________________________________    
void SpdGFHitCreator::PrintCounter()
{  
   cout << "-I- <SpdGFHitCreator::PrintCounter> \n\n";
   
   printf("%5s %8s %8s\n\n","I","N meas.","Errors"); 
   for (Int_t i(0); i<5; i++) {
        printf("%5d %8d %8d\n",i,fMeasCounter[i],fMeasCounterErrors[i]);   
   }
   printf("\n");
}

