// $Id$
// Author: artur   2018/02/01

//_____________________________________________________________________________
//
// SpdTsTBHitProducer
//_____________________________________________________________________________

#include "SpdTsTBHitProducer.h"

#include <TMath.h>
#include <TClonesArray.h>
#include <TGeoManager.h>

#include "FairRuntimeDb.h"
#include "FairRootManager.h"
#include "FairRun.h"

//#include "SpdTsTBGeoMapper.h"
//#include "SpdTsTBGeoBuilder.h"
#include "SpdTsTBParSet.h"

#include "SpdTsTBPoint.h"
#include "SpdTsTBHit.h"

#include "SpdMCTrack.h"
#include "SpdGeoFactory.h"
#include "SpdFieldCreator.h"
#include "SpdFieldPar.h"

#include <vector>
#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdTsTBHitProducer)

//_____________________________________________________________________________
SpdTsTBHitProducer::SpdTsTBHitProducer(TString params):
FairTask("Ideal TsTB hit Producer"),fEventHeader(0),
fParsName(params),fParameters(0),
fGeoMapper(0),fGeoBuilder(0),fField(0),fSaveDetIdOption(0),
fPointsArray(0),fTracksArray(0),fDigitsArray(0)
{

}

//_____________________________________________________________________________
SpdTsTBHitProducer::~SpdTsTBHitProducer() 
{

}

//_____________________________________________________________________________
void SpdTsTBHitProducer::SetParameters() 
{   
    FairRuntimeDb* rtdb = FairRun::Instance()->GetRuntimeDb();
    fParameters = (SpdParSet*)rtdb->getContainer(fParsName);
    ((FairParSet*)fParameters)->init();

    //fParameters->printParams();
    //fParameters->printByLabel(-1);
    //fParameters->printByLabel(-2);

    TString mapper;
    fParameters->GetParameter("Mapper",mapper); 
    fGeoMapper = SpdGeoFactory::Instance()->Mapper(mapper);
    
    if (fGeoMapper) {
      
        fGeoMapper->LoadParametersFrom(fParameters);
    
        fGeoMapper->InitGeometry();
        fGeoMapper->LockGeometry();
    
        fGeoMapper->Print("");
    }
    
    TString builder;
    fParameters->GetParameter("Builder",builder); 
    fGeoBuilder = SpdGeoFactory::Instance()->Builder(builder);
    
    if (fGeoBuilder) {
      
        fGeoBuilder->LoadParametersFrom(fParameters);
        
        fGeoBuilder->SetMapper(fGeoMapper);   
        fGeoBuilder->LockGeometry();
    
        fGeoBuilder->Print("");  
    }
   
    fParameters->GetParameter("Detector/SaveDetIdOption",fSaveDetIdOption);
}

//_____________________________________________________________________________
void SpdTsTBHitProducer::SetField() 
{         
    if (FairFieldFactory::Instance()) {
        fField = (SpdField*)FairRun::Instance()->GetField();
        return;
    }
    
    FairRuntimeDb* rtdb = FairRun::Instance()->GetRuntimeDb();  
    SpdFieldCreator* fieldCreator = new SpdFieldCreator();    
    SpdFieldPar *fpars = (SpdFieldPar*)rtdb->getContainer("SpdFieldPar");
    
    if (fpars) {
       ((FairParSet*)fpars)->init();
        fField = (SpdField*)fieldCreator->createFairField(fpars);
        //((SpdFieldPar*)fpars)->print(1);
    }
    
    if (fField) fField->Print(""); 
}

//_____________________________________________________________________________
InitStatus SpdTsTBHitProducer::Init() 
{   
    // get RootManager
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman) {
        cout << "-E- <SpdTsTBHitProducer::Init> "
             << "RootManager not instantiated!" << endl;
        return kFATAL;
    }

    // get Geometry parameters
    SetParameters();
    
    // get Field parameters
    SetField();
    
    // get Event header
    fEventHeader = (SpdMCEventHeader*)ioman->GetObject("MCEventHeader.");
   
    if (!fEventHeader) {
        cout << "-E- <SpdTsTBHitProducer::Init>" << "No HEADER object inside " << endl;
        return kERROR;
    }
    
    // get Tracks array
    fTracksArray = (TClonesArray*)ioman->GetObject("SpdMCTrack");
    
    if (!fTracksArray) {
        cout << "-W- <SpdTsTBHitProducer::Init> " << "No MCTrack array!" << endl;
        return kERROR;
    }
    
    // get Points array
    fPointsArray = (TClonesArray*)ioman->GetObject("SpdTsTBPoint");
    
    if (!fPointsArray) {
        cout << "-W- <SpdTsTBHitProducer::Init> " << "No TsPoint array!" << endl;
        return kERROR;
    }

    // Create and register output array of Digits
    fDigitsArray = new TClonesArray("SpdTsTBHit",100);
    ioman->Register("SpdTsTBHit", "TsTB", fDigitsArray, kTRUE);
                           
    cout << "-I- <SpdTsTBHitProducer:Init> Intialization successfull " << endl;
    return kSUCCESS;
}

//________________________________________________________________________________________
void SpdTsTBHitProducer::Exec(Option_t* opt) 
{
    if (!fPointsArray) return;
  
    cout << "-I- <SpdTsTBHitProducer::Exec> Started with " 
         << "tracks/points = ";
    
    if (fPointsArray && fPointsArray->GetEntriesFast() > 1) {
        cout << fTracksArray->GetEntriesFast() << "/"
             << fPointsArray->GetEntriesFast() << endl;
    }
    else {
        cout << "-W- <SpdTsTBHitProducer::Exec> Empty array of points " 
             << endl;
        return;
    }
    
    if (!fDigitsArray) {
        Fatal("SpdTsTBHitProducer::Exec", "No array of digits");
    }
    
    fDigitsArray->Delete();
    
    SpdTsTBHit* hit;  
    
    Int_t ne = fDigitsArray->GetEntriesFast();
    hit = new((*fDigitsArray)[ne]) SpdTsTBHit(0);
    
    SpdGeoTable* geotable = 0;
    if (fGeoMapper) geotable = fGeoMapper->GetGeoTable();
        
    Double_t total_dep = 0;
    
    Long_t tid;
    
    TString volpath;
    Double_t pos[3], dir[3], pos_in[3], pos_out[3];
    Double_t field[3] = {0, 0, 0};
    
    Double_t r_in, r_out;
    
    SpdTsTBPoint* pnt = 0;
    
    for (Int_t i(0); i<fPointsArray->GetEntriesFast(); i++) {
         
         pnt = (SpdTsTBPoint*)fPointsArray->At(i); 
       
         cout << ">>>>>>>>>>>>>>>>>>>> POINT: " << i << "\n\n";
       
         pnt->Print("");
         
         pnt->GetPositionIn(pos_in);
         pnt->GetPositionOut(pos_out);
       
         tid  = pnt->GetDetectorTID();
         
         volpath = "MASTER";
         
         if (fSaveDetIdOption == 1) {
             volpath = fParameters->GetNodeFullName(tid);
             if (fGeoBuilder) fGeoBuilder->SetNodePath(volpath); // set current node
         }
         else if (fSaveDetIdOption == 2) 
         {          
            if (geotable) volpath = geotable->GetNodeGeoPath(tid);
            if (fGeoBuilder) fGeoBuilder->SetNodePath(tid);  // set current node
         }
         
         cout << " path:    " << volpath  << endl;
         
         hit->AddPoint(i);
         hit->AddDeposit(pnt->GetEnergyLoss());  
                
         total_dep += pnt->GetEnergyLoss();

         if (!fGeoBuilder) continue;
           
         fGeoBuilder->GetDetectorPosition(pos);
         fGeoBuilder->GetDetectorOrientation(dir,'z');
       
         if (fField) fField->GetField(pos,field);
          
         cout << " detpos:  " << pos[0]   << " " << pos[1] << " " << pos[2] << " [cm] " << endl;
         cout << " detaxis: " << dir[0]   << " " << dir[1] << " " << dir[2] << endl;
         cout << " field:   " << field[0] << " " << field [1] << " " << field[2] << " [kG] " <<endl; 
         cout << endl;
        
         fGeoBuilder->GlobalToLocalP(pos_in,pos);
         r_in = TMath::Sqrt(pos[0]*pos[0] + pos[1]*pos[1]);
         
         cout << " pos_in (local): " << pos[0] << " " << pos[1] << " " << pos[2] << " [cm] " << endl;
         cout << " r_in = " << r_in << endl;
            
         fGeoBuilder->GlobalToLocalP(pos_out,pos);
         r_out = TMath::Sqrt(pos[0]*pos[0] + pos[1]*pos[1]);
         
         cout << " pos_out (local): " << pos[0] << " " << pos[1] << " " << pos[2] << " [cm] " << endl;
         cout << " r_out = " <<  r_out << endl;
         cout << endl;    
   }  
     
    cout << "-I- <SpdTsTBHitProducer::Exec> Hits/Points/Tracks = " 
         << fDigitsArray->GetEntriesFast() 
         << "/" << fPointsArray->GetEntriesFast() 
         << "/" << fTracksArray->GetEntriesFast() << endl; 
  
    cout << "-I- <SpdTsTBHitProducer::Exec> TOTAL DEPOSIT = " 
         << total_dep << endl;    
    
    //if (fEventHeader) fEventHeader->Print(1);
    
    cout << "\n ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ " << endl;
}

//________________________________________________________________________________________
void SpdTsTBHitProducer::Finish() 
{
    cout << "-I- <SpdTsTBHitProducer::Finish>" << endl;
}

//________________________________________________________________________________________
void SpdTsTBHitProducer::PrintInput()
{
    cout << "\n\n-I- <SpdTsTBHitProducer::PrintInput> " << endl;
    
    cout << " Parameters: " << fParsName << endl;
   
    if (fGeoMapper) {
        fGeoMapper->Print("");
        //fGeoMapper->PrintVolPars();
        //fGeoMapper->PrintGeoTable();
    }
    cout << "\n";
    
    if (fField) fField->Print(""); 
    cout << "\n";
}




