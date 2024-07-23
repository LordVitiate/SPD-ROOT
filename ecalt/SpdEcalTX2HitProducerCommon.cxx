
#include "SpdEcalTX2HitProducerCommon.h"
#include "TGeoManager.h"
#include "TGeoNavigator.h"
#include "TGeoScaledShape.h"
#include "TGeoArb8.h"
#include "TGeoVolume.h"
#include <iostream>

using namespace std;

//_____________________________________________________________________________
bool SpdEcalTX2HitProducerCommon::VerifyVertices(Double_t* vtx) {

    return ( (  vtx[1] ==  vtx[3]) 
            && (vtx[5] ==  vtx[7]) 
            && (vtx[9] ==  vtx[11]) 
            && (vtx[13] == vtx[15]) 
            && (vtx[0] ==  vtx[6]) 
            && (vtx[2] ==  vtx[4]) 
            && (vtx[8] ==  vtx[14]) 
            && (vtx[10] == vtx[12]) 
           );
 }

//_____________________________________________________________________________
TVector3 SpdEcalTX2HitProducerCommon::GetCellPosition(TString nodepath) {
    
    TGeoNavigator* nav = gGeoManager->GetCurrentNavigator();
    nav->cd(nodepath);
    if (!nav) {
        cout << "-W- <SpdEcalTX2HitProducer::GetCellPosition> node not found in TGeoNavigator: " << nodepath << endl;
        return TVector3(0., 0., 0.);
    }
    const TGeoVolume* vol = nav->GetCurrentVolume();
    Double_t ploc[3] = {0, 0, 0};
    //TGeoArb8 * moduleShape = (TGeoArb8*)moduleVolume->GetShape();
    //Double_t* moduleVertices = moduleShape->GetVertices();
    if (TString(vol->GetShape()->ClassName()).Contains("TGeoScaledShape")) {
        TGeoScaledShape* scaledShape = (TGeoScaledShape*)vol->GetShape();
        if (TString(scaledShape->GetShape()->ClassName()).Contains("TGeoArb8")) {
            TGeoArb8* shape = (TGeoArb8*)scaledShape->GetShape();
            Double_t* vtx = shape->GetVertices();
            if (!VerifyVertices(vtx)) {
                cout << "-W- <SpdEcalTX2Reconstruction::Exec> : incorrect shape of cell!" << endl;
            }
            Double_t yCenter = 0.25*(vtx[3]+vtx[5]+vtx[11]+vtx[13]);
            Double_t xCenter = 0.25*(vtx[4]+vtx[6]+vtx[12]+vtx[14]);
            ploc[0] = xCenter;
            ploc[1] = yCenter;
        }
    }
    else if (TString(vol->GetShape()->ClassName()).Contains("TGeoArb8")) {
        TGeoArb8* shape = (TGeoArb8*)vol->GetShape();
        Double_t* vtx = shape->GetVertices();
        if (!VerifyVertices(vtx)) {
            cout << "-W- <SpdEcalTX2Reconstruction::Exec> : incorrect shape of cell!" << endl;
        }
        Double_t yCenter = 0.25*(vtx[3]+vtx[5]+vtx[11]+vtx[13]);
        Double_t xCenter = 0.25*(vtx[4]+vtx[6]+vtx[12]+vtx[14]);
        ploc[0] = xCenter;
        ploc[1] = yCenter;
    }
    Double_t p[3];
    nav->LocalToMaster(ploc,p);
    return TVector3(p[0], p[1], p[2]);
}



