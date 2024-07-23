// $Id$
// Author: alver   2020/07/19

//_____________________________________________________________________________
//
// SpdRsTB2HitProducer
//_____________________________________________________________________________

#include "SpdRsTB2HitProducer.h"

#include <TMath.h>
#include <TClonesArray.h>
#include <TGeoManager.h>

#include "FairRuntimeDb.h"
#include "FairRun.h"

//#include "SpdRsTB2GeoMapper.h"
#include "SpdRsTBParSet.h"

#include "SpdRsTB2Point.h"
#include "SpdRsTB2Hit.h"
#include "SpdRsTID.h"

#include "SpdMCTrack.h"
#include "SpdGeoFactory.h"

#include <vector>
#include <iostream>
#include <TGeoNavigator.h>


using std::map;
using std::cout;
using std::endl;

ClassImp(SpdRsTB2HitProducer)

//_____________________________________________________________________________
SpdRsTB2HitProducer::SpdRsTB2HitProducer(TString params):
SpdTask("Ideal RsTB2 hit Producer"),
fGeoMapper(0),fGeoNavigator(0),fEventHeader(0),
fParsName(params),fParameters(0),fSaveDetIdOption(0),
fPointsArray(0),fTracksArray(0),fHitsArray(0)
{

}

//_____________________________________________________________________________
SpdRsTB2HitProducer::~SpdRsTB2HitProducer() 
{

}

//_____________________________________________________________________________
void SpdRsTB2HitProducer::SetParameters() 
{   
    FairRuntimeDb* rtdb = FairRun::Instance()->GetRuntimeDb();
    fParameters = (SpdParSet*)rtdb->getContainer(fParsName);
    ((FairParSet*)fParameters)->init();
  
    TString mapper;
    fParameters->GetParameter("Mapper",mapper); 
    fGeoMapper = SpdGeoFactory::Instance()->Mapper(mapper);

    if (fGeoMapper) {
        
        fGeoMapper->LoadParametersFrom(fParameters);
       
        fGeoMapper->InitGeometry();
        fGeoMapper->LockGeometry();
  
        fGeoMapper->Print("");
    }
    
    fParameters->GetParameter("Detector/SaveDetIdOption",fSaveDetIdOption);
}

//_____________________________________________________________________________
InitStatus SpdRsTB2HitProducer::Init() 
{   
    // get RootManager
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman) {
        cout << "-E- <SpdRsTB2HitProducer::Init> "
             << "RootManager not instantiated!" << endl;
        return kFATAL;
    }

    // get Geometry parameters
    SetParameters();
    
    // get Event header
    fEventHeader = (SpdMCEventHeader*)ioman->GetObject("MCEventHeader.");
   
    if (!fEventHeader) {
        cout << "-E- <SpdRsTB2HitProducer::Init>" << "No HEADER object inside " << endl;
        return kERROR;
    }

    // get Points array
    fPointsArray = (TClonesArray*)ioman->GetObject("SpdRsTB2Point");
    
    if (!fPointsArray) {
        //cout << "-W- <SpdRsTB2HitProducer::Init> " << "No RsPoint array!" << endl;
        return kERROR;
    }
    
    // get Tracks array
    fTracksArray = (TClonesArray*)ioman->GetObject("SpdMCTrack");
    
    if (!fTracksArray) {
        cout << "-W- <SpdRsTB2HitProducer::Init> " << "No MCTrack array!" << endl;
        return kERROR;
    }

    // Create and register output array of Digits
    fHitsArray = new TClonesArray("SpdRsTB2Hit",100);
    ioman->Register("SpdRsTB2Hit", "RsTB", fHitsArray, kTRUE);
                           
    cout << "-I- <SpdRsTB2HitProducer:Init> Intialization successfull " << endl;
    
    TClonesArray* a1 = (TClonesArray*)ioman->GetObject("SpdRsTB2Hit");
    TClonesArray* a2 = (TClonesArray*)ioman->GetObject("SpdRsTEC2Hit");
    
    cout << "\n +++++++++++++++++ -I- <SpdRsTB2HitProducer::Exec> TB, TEC = " << a1 << " " << a2 << endl;
   // << " entries:  " << a1->GetEntriesFast() <<  " " <<  a2->GetEntriesFast() << endl;

    fGeoNavigator = gGeoManager->GetCurrentNavigator();
    return kSUCCESS;
}

//________________________________________________________________________________________
void SpdRsTB2HitProducer::Finish() 
{
//    cout << "-I- <SpdRsTB2HitProducer::Finish>" << endl;
}

//________________________________________________________________________________________
void SpdRsTB2HitProducer::Exec(Option_t* opt) 
{ 
    cout << "\n +++++++++++++++++ -I- <SpdRsTB2HitProducer::Exec> +++++++++++++++ " << endl;
    
//     FairRootManager* ioman = FairRootManager::Instance();
//     TClonesArray* a1 = (TClonesArray*)ioman->GetObject("SpdRsTB2Hit");
//     TClonesArray* a2 = (TClonesArray*)ioman->GetObject("SpdRsTEC2Hit");
//     
//     cout << "\n +++++++++++++++++ -I- <SpdRsTB2HitProducer::Exec> TB, TEC = " << a1 << " " << a2 
//     << " entries:  " << a1->GetEntriesFast() <<  " " <<  a2->GetEntriesFast() << endl;
    
    //cout << "-I- <SpdRsTB2HitProducer::Exec> Started with " 
    //     << "tracks/points = ";
    cout<<fEventHeader->GetEventID()<<endl; //event number
    if (fPointsArray && fPointsArray->GetEntriesFast() > 1) {
    //     cout << fTracksArray->GetEntriesFast() << "/"
    //         << fPointsArray->GetEntriesFast() << endl;
    }
    else {
    //    cout << "-W- <SpdRsTB2HitProducer::Exec> Empty array of points " 
    //         << endl;
        return;
    }
    
    if (!fHitsArray) {
        Fatal("SpdRsTB2HitProducer::Exec", "No array of digits");
    }
    
    fHitsArray->Delete();

    map<Int_t, std::vector <Int_t> > mapBoxPoint;   // PointIds with same DetId - box
    map<Int_t, TVector3> mapBoxPos;                 // Position for box
    mapBoxPoint.clear();
    mapBoxPos.clear();

    TVector3 pos(0., 0., 0.);
    Double_t detpos[3];           // hit node position in global CS
    TString node_path;
        
    Int_t    NPoints = 0;    
    SpdRsTB2Point* pnt = 0;
    
    for (Int_t i(0); i<fPointsArray->GetEntriesFast(); i++) {
          
        pnt = (SpdRsTB2Point*)fPointsArray->At(i); 
         
	    if ( ((pnt->GetPosIn()-pnt->GetPosOut()).Mag() < 0.2 ) && 
	          (pnt->GetEnergyLoss()*1.0e6 < 0.2) ) continue;	 
        node_path = pnt->GetHitGeoPath();
        //cout<<node_path<<endl;
         
        DetectorPosition(detpos, node_path);
        //cout << "\t position:   " << detpos[0] << " " << detpos[1] << " " << detpos[2] << " " << endl; 

	    NPoints++;
        pos.SetXYZ(detpos[0],detpos[1],detpos[2]);
        //cout << "\t position:   " << pos.X() << " " << pos.Y() << " " << pos.Z() << " " << endl;
    
        SpdGeopathParser ppath;
        ppath.ParsePath(node_path);
        Int_t detID = SpdRsTID::DetectorId(ppath.Num(2), ppath.Num(3), ppath.Num(4), ppath.Num(6)); 
        //cout << ppath.Num(2) << "  " << ppath.Num(3) << "   " << ppath.Num(4) << "   " << ppath.Num(6) << " = " << detID << endl;

        mapBoxPoint[detID].push_back(i);
        mapBoxPos[detID] = pos;
        //cout<<"detID: "<<detID<<endl;        
    }

    map<Int_t, std::vector <Int_t> >::const_iterator boxIter;
    for( boxIter = mapBoxPoint.begin(); boxIter != mapBoxPoint.end(); ++boxIter)
    {
        AddHit((*boxIter).first,  mapBoxPos[(*boxIter).first], (*boxIter).second);
    } 

    /* 
    cout << "-I- <SpdRsTB2HitProducer::Exec> Hits/Points = " 
         << fHitsArray->GetEntriesFast() 
         << "/" << fPointsArray->GetEntriesFast() << endl; 
    */
    SpdRsTB2Hit* hit = 0;
    for (Int_t i(0); i<fHitsArray->GetEntriesFast(); i++) {
        hit = (SpdRsTB2Hit*)fHitsArray->At(i); 
        //hit->Print();
        cout<<hit->GetLayer()<<", "<<hit->GetPosition().X()<<", "<<hit->GetPosition().Y()<<", "<<hit->GetPosition().Z() << endl;
    //    cout << hit->GetSector() << "  " << hit->GetLayer() << "  " << hit->GetMDT() << "  " << hit->GetCell() << endl;
    //    cout << hit->GetPoints()[0] << "   " << hit->GetPoints()[1] << endl;
    //    pnt = (SpdRsTB2Point*)fPointsArray->At(hit->GetPoints()[0]);
    //    cout << pnt->GetEnergyLoss() << endl;
    }

    //cout << "-I- <SpdRsTB2HitProducer::Exec> Number of points = "
    //     << NPoints << endl;

}

//________________________________________________________________________________________
SpdRsTB2Hit* SpdRsTB2HitProducer::AddHit(Int_t detID, TVector3& pos, std::vector<Int_t> pointList)
{ 
    // Private method AddHit
    TClonesArray& clref = *fHitsArray;
    Int_t size = clref.GetEntriesFast();
    return new(clref[size]) SpdRsTB2Hit(detID, pos, pointList);
}

//________________________________________________________________________________________
Bool_t SpdRsTB2HitProducer::DetectorPosition(Double_t* p, TString node_path)
{  
    fGeoNavigator->cd(node_path);
    Double_t ploc[3] = {0, 0, 0};
    fGeoNavigator->LocalToMaster(ploc,p);
    return true;
}
