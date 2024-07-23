
#include "RsGFMeasurementMaker.h"
#include "GFmeasurements.h"

#include "SpdRsMCHit.h"

#include <TMath.h>

#include <iostream>

using std::cout;
using std::endl;

using namespace genfit;

ClassImp(RsGFMeasurementMaker)

//_____________________________________________________________________________
RsGFMeasurementMaker::RsGFMeasurementMaker():fLastHitId(-1) 
{
}

//_____________________________________________________________________________
std::vector<genfit::AbsMeasurement*> RsGFMeasurementMaker::ProcessHit(const SpdRsMCHit* hit)
{
   std::vector<genfit::AbsMeasurement*> meas;
   
   TVector3 pos = hit->GetHitPos();

   TVectorD hitCoords(7);
   
   if (hit->IsBarrelHit()) {
       hitCoords(0) = pos.X();
       hitCoords(1) = pos.Y();
       hitCoords(2) = -240;

       hitCoords(3) = pos.X();
       hitCoords(4) = pos.Y();
       hitCoords(5) =  240;
   }

   if (hit->IsEndcapHit()) {
       hitCoords(0) = -1000;
       hitCoords(1) = pos.Y();
       hitCoords(2) = pos.Z();

       hitCoords(3) = 1000;
       hitCoords(4) = pos.Y();
       hitCoords(5) = pos.Z();
   }
       
   //hitCoords(6) = hit->GetRdrift();  // <-- change to zero
   hitCoords(6) = 0;

   TMatrixDSym hitCov(7);
   
   //hitCov(6,6) = hit->GetResolutionR()*hit->GetResolutionR(); // Change
   hitCov(6,6) = 1.*1/12; // Change
   
   fLastHitId++;
     
   WireMeasurement* m = new WireMeasurement(hitCoords, hitCov, hit->GetModId(), 
                                            fLastHitId, nullptr);
     
   m->setMaxDistance(1.5); // cm
   //m->setLeftRightResolution(hit->GetPointLRtype());
   
   meas.push_back(m);

   return meas;
}

