// $Id$
// Author: andre   2021/03/04

#include "SpdEcalCommon.h"

#include <TGeoManager.h>
#include <TGeoNavigator.h>
#include <TGeoScaledShape.h>
#include <TGeoArb8.h>
#include <TGeoVolume.h>
#include <iostream>

using namespace std;

//_____________________________________________________________________________
Bool_t SpdEcalCommon::VerifyVertices(Double_t* vtx) 
{
    return ( 
        ( (  vtx[1] ==  vtx[3]) 
            && (vtx[5] ==  vtx[7]) 
            && (vtx[9] ==  vtx[11]) 
            && (vtx[13] == vtx[15]) 
            && (vtx[0] ==  vtx[6]) 
            && (vtx[2] ==  vtx[4]) 
            && (vtx[8] ==  vtx[14]) 
            && (vtx[10] == vtx[12]) 
           ) ||
        ( (  vtx[3] ==  vtx[5]) 
            && (vtx[1] ==  vtx[7]) 
            && (vtx[9] ==  vtx[15]) 
            && (vtx[13] == vtx[11]) 
            && (vtx[0] ==  vtx[2]) 
            && (vtx[4] ==  vtx[6]) 
            && (vtx[8] ==  vtx[10]) 
            && (vtx[12] == vtx[14]) 
           )
        );

}

//_____________________________________________________________________________
Bool_t SpdEcalCommon::GetCellPosition(TString nodepath, TVector3& nodepos) 
{ 
    TGeoNavigator* nav = gGeoManager->GetCurrentNavigator();
    
    if (!nav) {
        cout << "-W- <SpdEcalCommon::GetCellPosition> Node not found: " 
             << nodepath << endl;
        return false;
    }
    
    nav->cd(nodepath);

    Double_t ploc[3] = {0,0,0};

    // check if the shape is TGeoArb8
    
    const TGeoVolume* vol = nav->GetCurrentVolume(); 
    TGeoArb8* shape = 0;
    
    TGeoScaledShape* scaled_shape = dynamic_cast<TGeoScaledShape*>(vol->GetShape());
    if (scaled_shape) shape = dynamic_cast<TGeoArb8*>(scaled_shape->GetShape());
    else shape = dynamic_cast<TGeoArb8*>(vol->GetShape());
    
    if (shape) {
        
        Double_t* vtx = shape->GetVertices();
    
        if (!VerifyVertices(vtx)) {
            cout << "-W- <SpdEcalCommon::GetCellPosition> Incorrect shape of cell " << endl;
            return false;
        }
        
        ploc[0] = 0.125*(vtx[0] + vtx[2] + vtx[4] + vtx[6] + vtx[8] + vtx[10] + vtx[12] + vtx[14]); // center_x
        ploc[1] = 0.125*(vtx[1] + vtx[3] + vtx[5] + vtx[7] + vtx[9] + vtx[11] + vtx[13] + vtx[15]); // center_y;
    }

    // get local position in global CS
    Double_t pglob[3];
    nav->LocalToMaster(ploc,pglob);
    nodepos.SetXYZ(pglob[0],pglob[1],pglob[2]);
    
    return true;
}





