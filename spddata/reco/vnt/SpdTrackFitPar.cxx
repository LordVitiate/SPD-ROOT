// $Id$
// Author: artur   2020/10/01


//_____________________________________________________________________________
//
// SpdTrackFitPar
//_____________________________________________________________________________

#include "SpdTrackFitPar.h"

#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdTrackFitPar)

//_____________________________________________________________________________
SpdTrackFitPar::SpdTrackFitPar():
fFirstState(0),fLastState(0),fFinalState(0),fImStates(0),
fFirstPointId(-1),fLastPointId(-1),
fTrackLength(0.),fIsTrackChargeChanged(false),
fTrackId(-1),fTrackPdgCode(-1),fNPoints(0),
fMaterialEffects(0),fChi2(0),fNDF(1),fDChi2(0),
fErrorFlag(kSpdTrUndefined),fErrorMesg(""),fNFailedHits(0),
fConvergencyGF(0)
{

}
 
//_____________________________________________________________________________
SpdTrackFitPar::~SpdTrackFitPar() 
{
    if (fFirstState) delete fFirstState;
    if (fLastState)  delete fLastState;
    if (fFinalState) delete fFinalState;
    
    ClearImStates();
}

//_____________________________________________________________________________
void SpdTrackFitPar::ClearPars()
{
    if (fFirstState) { delete fFirstState; fFirstState = 0; }
    if (fLastState)  { delete fLastState;  fLastState  = 0; }
    if (fFinalState) { delete fFinalState; fFinalState = 0; } 
    
    ClearImStates();
}

//_____________________________________________________________________________
void SpdTrackFitPar::ClearImStates()
{
    if (!fImStates) return;
    if (fImStates->empty()) delete fImStates; 
    else {
       for (SpdTrackState2* state : *fImStates) { if (state) delete state; }
       delete fImStates; 
    }
    fImStates = 0;
}

//_____________________________________________________________________________
void SpdTrackFitPar::RemoveImStates()
{
    if (!fImStates) return;
    if (fImStates->empty()) return; 
    for (SpdTrackState2* state : *fImStates) { if (state) delete state; }
    fImStates->clear(); 
}

//_____________________________________________________________________________
void SpdTrackFitPar::Clear(Option_t* option)
{
    ClearPars();
}

//_____________________________________________________________________________
SpdTrackState* SpdTrackFitPar::FirstState() 
{ 
    if (!fFirstState) fFirstState = new SpdTrackState(); 
    return fFirstState; 
}

//_____________________________________________________________________________
SpdTrackState* SpdTrackFitPar::LastState() 
{ 
    if (!fLastState) fLastState = new SpdTrackState(); 
    return fLastState; 
}

//_____________________________________________________________________________
SpdTrackState* SpdTrackFitPar::FinalState() 
{ 
    if (!fFinalState) fFinalState = new SpdTrackState(); 
    return fFinalState; 
}

//_____________________________________________________________________________
std::vector<SpdTrackState2*>& SpdTrackFitPar::ImStates() 
{  
    if (!fImStates) fImStates = new std::vector<SpdTrackState2*>; 
    return *fImStates; 
}
    
//_____________________________________________________________________________
Double_t SpdTrackFitPar::GetDPos() const
{
    if (!fFirstState) return -1.;
    if (!fLastState) return -1.;    
    TVector3 d = fFirstState->GetPosition()-fLastState->GetPosition();
    return d.Mag();
}

//_____________________________________________________________________________
SpdTrackState2* SpdTrackFitPar::FindImState(Int_t point_id)
{
    if (fImStates->empty()) return 0;
    if (point_id <= fFirstPointId || point_id >= fLastPointId) return 0;   
    
    Int_t id = point_id-1;
    if (id > -1 || id < fImStates->size()) {
        SpdTrackState2* state = fImStates->at(id);
        if (point_id == state->GetPointId()) return state; 
    }
    
    for (SpdTrackState2* st : *fImStates) 
    { if (point_id == st->GetPointId()) return st; }
    
    return 0;
}

//_____________________________________________________________________________
void SpdTrackFitPar::Print(Option_t* option) const
{
    cout << "\n\t<SpdTrackFitPar::Print> \n\n";
    
    cout << "\tTrack id:                 " << fTrackId << endl;
    cout << "\tTrack pdg:                " << fTrackPdgCode << endl;
    cout << "\tTrack charge is changed:  " << ((fIsTrackChargeChanged) ? "yes" : "no") << endl;
    cout << "\tNumber of points (total): " << fNPoints << endl;
    cout << "\tIntermediate states:      " << GetNImStates() << endl;
    cout << "\tFirst point id:           " << fFirstPointId << endl;
    cout << "\tLast point id:            " << fLastPointId << endl;
    cout << "\tTrack length (F-L):       " << fTrackLength << " (>" << GetDPos() << ") " << endl;
    cout << endl;
    
    cout << "\tMaterial effects:         " << fMaterialEffects << endl;
    cout << "\tchi2/NDF:                 " << ((fNDF > 0) ? (fChi2/fNDF) : -1) << endl;
    cout << "\tNDF:                      " << fNDF << endl;
    cout << "\td(chi2):                  " << fDChi2 << endl;
    cout << endl;
    
    cout << "\tFatal error flag:         " << ((!fErrorFlag) ? "no" : Form("%d",fErrorFlag)) << endl;
    cout << "\tHas error messages:       " << (HasErrorMesg() ? "yes" : "no") << endl;
    cout << "\tFailed hits:              " << fNFailedHits << endl;
    cout << "\tConvergency:              " << fConvergencyGF << endl;
    cout << endl;
    
    fInitialState.Print("Initial state");
    
    if (fFirstState) fFirstState->Print("First state");
    if (fLastState)  fLastState->Print("Last state");
    if (fFinalState) fFinalState->Print("Final state");
    
    if (!fImStates || fImStates->empty()) return;
    
    if (TString(option).Contains("imstates0")) 
    { 
        Int_t i(0);
        TString spos, smom, scov;
        const TVector3* pos, *mom;  
         
        printf("\t INTERMEDIATE TRACK STATES: \n\n");
        printf("\n\t%5s %5s %12s %12s %12s %12s %12s %12s %12s %12s %8s\n\n","I","ID",
               "p [GeV/c]","px","py","pz","x [cm]","y [cm]","z [cm]","t [ns]","COV");
         
        for (SpdTrackState2* state : *fImStates) 
        { 
            if (!state) continue;
            
            pos = state->Position();
            if (pos) spos = Form("%12.3e %12.3e %12.3e",pos->X(),pos->Y(),pos->Z());
            else spos = Form("%12s %12s %12s","---","---","---");
            
            mom = state->Momentum();
            if (mom) smom = Form("%12.3e %12.3e %12.3e %12.3e",mom->Mag(),mom->X(),mom->Y(),mom->Z());
            else smom = Form("%12s %12s %12s %12s","---","---","---","---");
            
            scov = (state->Covariance()) ? "yes" : "no";
                
            printf("\t%5d %5d %51s %38s %12.3e %8s\n",++i,state->GetPointId(),
                   smom.Data(),spos.Data(),state->GetTime(),scov.Data());
        }
    
        printf("\n");
        
        return;
    }
    
    if (TString(option).Contains("imstates")) 
    { 
        printf("\t INTERMEDIATE TRACK STATES: \n\n");
        for (SpdTrackState2* state : *fImStates) { if (state) state->Print(); }
        return;
    }
}



