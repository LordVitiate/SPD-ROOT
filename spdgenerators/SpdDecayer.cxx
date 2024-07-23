// $Id$
// Author: artur   2018/12/19


//_____________________________________________________________________________
//
// SpdDecayer
//_____________________________________________________________________________

#include "SpdDecayer.h"
#include "SpdPythia6Decayer.h"
#include "SpdPythia8Decayer.h"

#include "SpdMCEventHeader.h"
#include "SpdPrimGenParSet.h"
#include "SpdPrimGenData.h"

#include "FairRuntimeDb.h"
#include "FairRun.h"

#include <TPythia6.h>
#include <TPythia8.h>

#include <TParticle.h>
#include <TLorentzVector.h>
#include <TClonesArray.h>
#include <TVirtualMC.h>
#include <iostream>

using std::cout;
using std::endl;

SpdDecayer* SpdDecayer::fInstance = 0;

SpdDecayer* SpdDecayer::Instance()
{
   return (fInstance) ? fInstance : new SpdDecayer();
}

SpdDecayer* SpdDecayer::Decayer()
{
   return fInstance;
}


ClassImp(SpdDecayer)

//_____________________________________________________________________________
SpdDecayer::SpdDecayer(Int_t storage_index):TNamed("Spd decayer","unknown"),
fStorageIndex(storage_index),
fDecayer(0),fDecayerOption(0),
fEHeader(0),fIsInit(kFALSE)
{
   if (fInstance) {
       cout << "-F- <SpdDecayer::SpdDecayer> "
            << " There is already an instance " << endl;
       exit(1);
   }
   
   fInstance = this;
}

//_____________________________________________________________________________
SpdDecayer::~SpdDecayer() 
{
   if (fDecayer) {
       delete fDecayer;
       fDecayer = 0;
   }
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  
//_____________________________________________________________________________  
void SpdDecayer::FillDecay(const Int_t& idpart, const TLorentzVector* p) 
{
   if (!fEHeader) return;
   
   SpdPrimGenData* data = fEHeader->GetDecayerData();
 
   if (!data) return;
   
   TParticle* part = data->AddNewParticle();
   
   part->SetPdgCode(idpart);
   part->SetMomentum(*p);
   
   part->SetFirstDaughter(-1);
   part->SetFirstMother(-1);
   part->SetLastDaughter(-1);
   part->SetLastMother(-1);
   
   //data->Print(" Decayer "); 
}

//_____________________________________________________________________________
void SpdDecayer::FillParticles(const TClonesArray *particles) 
{
   if (!fEHeader || !particles) return;
}

//_____________________________________________________________________________
void SpdDecayer::FillEventData()
{
   if (!fEHeader) return;
   
   SpdPrimGenData* data = fEHeader->GetDecayerData();
   if (!data) return;
   
   data->SetGeneratorType(fTitle);
   data->SetGeneratorIndex(fStorageIndex);
 
   std::vector<UInt_t>& seeds = data->GetGenSeeds();
  
   /* set seeds */
   
   SpdPythia8Decayer* decayerPy8 = dynamic_cast<SpdPythia8Decayer*>(fDecayer);
   if (decayerPy8) { seeds.push_back(decayerPy8->GetSeed()); return; }
    
   SpdPythia6Decayer* decayerPy6 = dynamic_cast<SpdPythia6Decayer*>(fDecayer);
   if (decayerPy6) { seeds.push_back(decayerPy6->GetSeed()); return; }
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++   

//_____________________________________________________________________________
void SpdDecayer::Reset()
{
    if (fDecayer) {
        delete fDecayer;
        fDecayer = 0;
    }
    
    fDecayerOption = 0;
    fIsInit = kFALSE;
}

//_____________________________________________________________________________
void SpdDecayer::SetDecayer(TString decayer, Int_t option)
{
    if (fDecayer) {
        delete fDecayer;
        fDecayer = 0;
    }
    
    fIsInit = kFALSE;
  
    if (decayer == "SpdPythia8Decayer") {
        fTitle = decayer;
        fDecayerOption = option;
        fDecayer = new SpdPythia8Decayer(this);
    }
    else if (decayer == "SpdPythia6Decayer") {
        fTitle = decayer;
        fDecayerOption = option;
        fDecayer = new SpdPythia6Decayer(this);
    }
    else {
        fTitle = "unknown";
        fDecayerOption = 0;
        cout << "-W- <SpdDecayer::SetDecayer> "
             << "Unknown decayer type: " << decayer << endl; 
    }
}

//_____________________________________________________________________________
void SpdDecayer::SetEventHeader(FairMCEventHeader* header) 
{ 
    fEHeader = dynamic_cast<SpdMCEventHeader*>(header);
}

//_____________________________________________________________________________
void SpdDecayer::Init()
{
    if (fIsInit || !fDecayer) return;
   
    fIsInit = Configure(fTitle);
}

//_____________________________________________________________________________
Bool_t SpdDecayer::FillParsIn(SpdPrimGenParSet* params)
{
    if (!params) return kFALSE;
    
    params->SetParameter("DecayerType",fTitle,fStorageIndex);
    params->SetParameter("DecayerOption",fDecayerOption,fStorageIndex);
  
    if (fTitle == "SpdPythia8Decayer") {
        SpdPythia8Decayer* decayer = dynamic_cast<SpdPythia8Decayer*>(fDecayer);
        if (decayer) return decayer->FillParsIn(params,fStorageIndex); 
    }
    
    if (fTitle == "SpdPythia6Decayer") { 
        SpdPythia6Decayer* decayer = dynamic_cast<SpdPythia6Decayer*>(fDecayer);
        if (decayer) return decayer->FillParsIn(params,fStorageIndex); 
    }
    
    cout << "-W- <SpdDecayer::FillParsIn> "
             << "Unknown decayer type: " << fTitle << endl; 
             
    return kTRUE;
}

//_____________________________________________________________________________
Bool_t SpdDecayer::LoadParsFrom(SpdPrimGenParSet* params)
{
    Reset();
  
    if (!params) return kFALSE;
    
    TString dtype;
    if (params->GetParameter("DecayerType",dtype,fStorageIndex));
    
    Int_t dopt;
    params->GetParameter("DecayerOption",dopt,fStorageIndex);
    
    SetDecayer(dtype,dopt);
    
    if (!fDecayer) return kFALSE;
    
    if (fTitle == "SpdPythia8Decayer") {
        SpdPythia8Decayer* decayer = dynamic_cast<SpdPythia8Decayer*>(fDecayer);
        if (decayer) return decayer->LoadParsFrom(params,fStorageIndex); 
    }
    
    if (fTitle == "SpdPythia6Decayer") {
        SpdPythia6Decayer* decayer = dynamic_cast<SpdPythia6Decayer*>(fDecayer);
        if (decayer) return decayer->LoadParsFrom(params,fStorageIndex); 
    }

    return kTRUE;
}

//_____________________________________________________________________________________
void SpdDecayer::Print(Option_t* opt) const
{
    cout << "\t<SpdDecayer::Print>" << "\n\n";
    
    cout << "\tInit:             " << fIsInit << endl;
    cout << "\tStorage index:    " << fStorageIndex << endl;
    cout << "\tDecayer:          " << GetDecayerType() << endl; 
    cout << "\tDecayer option:   " << fDecayerOption << endl; 
   
    if (fEHeader) cout << "\tEvent header:     " << "yes" << endl;  
    else cout << "\tEvent header:     " << "no" << endl;   
    
    cout << "\n";
}

//_____________________________________________________________________________
Bool_t SpdDecayer::Configure(TString decayer_type)
{
    if (decayer_type == "SpdPythia8Decayer") return ConfigureSpdPythia8Decayer();
    if (decayer_type == "SpdPythia6Decayer") return ConfigureSpdPythia6Decayer();
    return false;
}

//_____________________________________________________________________________
Bool_t SpdDecayer::ConfigureSpdPythia6Decayer() 
{
    SpdPythia6Decayer* decayer = dynamic_cast<SpdPythia6Decayer*>(fDecayer);
    if (!decayer) return kFALSE; 
    
    if (fDecayerOption == 0) {
        if (gMC) gMC->SetExternalDecayer(decayer);
        return kTRUE; 
    }
   
    decayer->SetForceDecay(TPythia6Decayer::kAll);     
    decayer->Init();
    
    if (gMC) gMC->SetExternalDecayer(decayer);
    
    if (fDecayerOption == 1) {
       
        /* Pythia6 settings */
        
        TPythia6& pythia6 = *(decayer->GetPythia());
 
        // - activate decay in pythia
        
        pythia6.SetMDCY(pythia6.Pycomp(313),1,1);   // K*0
        pythia6.SetMDCY(pythia6.Pycomp(-313),1,1);  // K*0bar
        
        pythia6.SetMDCY(pythia6.Pycomp(323),1,1);   // K*+
        pythia6.SetMDCY(pythia6.Pycomp(-323),1,1);  // K*+bar
          
        pythia6.SetMDCY(pythia6.Pycomp(311),1,1);   // K0
        pythia6.SetMDCY(pythia6.Pycomp(-311),1,1);  // K0bar

        pythia6.SetMDCY(pythia6.Pycomp(443),1,1);   // J/psi
        
        // - force external decayer for particle of a such type
        
        if (gMC) {
            gMC->SetUserDecay(311);  // K0
            gMC->SetUserDecay(-311); // K0_bar
            gMC->SetUserDecay(443);  // J/psi
        }
        
        return kTRUE; 
    }
    
    return kFALSE; 
}

//_____________________________________________________________________________
Bool_t SpdDecayer::ConfigureSpdPythia8Decayer() 
{
    SpdPythia8Decayer* decayer = dynamic_cast<SpdPythia8Decayer*>(fDecayer);
    if (!decayer) return kFALSE; 
    
    if (fDecayerOption == 0) {
        if (gMC) gMC->SetExternalDecayer(decayer);
        return kTRUE; 
    }
   
    decayer->Init();
    
    if (gMC) gMC->SetExternalDecayer(decayer);
    
    if (fDecayerOption == 1) {
       
        /* Pythia6 settings */
        SpdPythia8* pythia8 = decayer->GetPythia();
        
        // - activate decay in pythia
        
        // By default, decays for 313, 323, 311, 443 are active 
        // See also SpdPythia8Decayer::SetMayDecay(Int_t pdg)
        // ....
         
        // - force external decayer for particle of a such type
        
        if (gMC) {
            //gMC->SetUserDecay(311);  // K0
            //gMC->SetUserDecay(-311); // K0_bar
            gMC->SetUserDecay(443);  // J/psi
        }
    }
    
    return kTRUE; 
}




