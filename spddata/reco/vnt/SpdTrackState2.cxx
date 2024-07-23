// $Id$
// Author: artur   2021/12/21


//_____________________________________________________________________________
//
// SpdTrackState2
//_____________________________________________________________________________

#include "SpdTrackState2.h"

#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdTrackState2)

//_____________________________________________________________________________
SpdTrackState2::SpdTrackState2(const SpdTrackState2& state):TObject(state),
fTime(state.fTime),fPointId(state.fPointId)
{
    if (state.fMomentum) 
    {
        if (fMomentum) *fMomentum = *state.fMomentum;
        else fMomentum = new TVector3(*state.fMomentum);
    }
    else ClearMomentum();
  
    if (state.fPosition) 
    {
        if (fPosition) *fPosition = *state.fPosition;
        else fPosition = new TVector3(*state.fPosition);
    }
    else ClearPosition();
   
    if (state.fCovariance) 
    {
        if (!fCovariance) fCovariance = new TMatrixDSym(*state.fCovariance); 
        else {
            fCovariance->ResizeTo(*state.fCovariance); 
           *fCovariance = *state.fCovariance;
        }
    }
    else ClearCovariance();
}

//_____________________________________________________________________________
SpdTrackState2::~SpdTrackState2() 
{ 
    if (fMomentum)   delete fMomentum;
    if (fPosition)   delete fPosition; 
    if (fCovariance) delete fCovariance; 
}

//_____________________________________________________________________________
void SpdTrackState2::ClearState()
{
    if (fMomentum)   { delete fMomentum;   fMomentum   = 0; }
    if (fPosition)   { delete fPosition;   fPosition   = 0; }
    if (fCovariance) { delete fCovariance; fCovariance = 0; }
    
    fTime    = -1;
    fPointId = -1;
}

//_____________________________________________________________________________
SpdTrackState* SpdTrackState2::CreateTrackState()
{
    if (!fMomentum || !fPosition) return 0;
    
    SpdTrackState* state = new SpdTrackState();
    
    state->SetMomentum(*fMomentum);
    state->SetPosition(*fPosition); 
    state->SetTime(fTime); 
     
    if (fCovariance) state->SetCovariance(*fCovariance); 
    
    return state;
}

//_____________________________________________________________________________
void SpdTrackState2::Print(Option_t* option) const
{
    cout << "\t<SpdTrackState2::Print> " << option << "\n\n";
   
    printf("\tPoint ID:            %12d\n\n",fPointId);
    
    printf("\tTime     [ns]:       %12.6f\n",fTime);
    
    if (fMomentum) {
        printf("\tMomentum [GeV/c]:    %12.6f, %12.6f, %12.6f;  |p| = %-12f \n",
               fMomentum->X(),fMomentum->Y(),fMomentum->Z(),fMomentum->Mag());
    }
    else printf("\tMomentum [GeV/c]:       undefined\n");
    
    if (fPosition) {
        printf("\tPosition [cm]:       %12.6f, %12.6f, %12.6f;  D0  = %-12f \n",
               fPosition->X(),fPosition->Y(),fPosition->Z(),fPosition->Mag());   
    }
    else printf("\tPosition [cm]:          undefined\n");
    
    if (!fCovariance) { 
        printf("\n\tCovariation matrix:     undefined\n\n"); 
        return; 
    }
        
    Int_t nrows = fCovariance->GetNrows();
    if (nrows > 0) 
    {
        printf("\n");
        printf("\tCovariation matrix (%dx%d): \n",nrows,nrows);
      
        printf("\n\t%4s |","");
        for (Int_t i(0); i<nrows; i++) printf(" %5d%5s|",i,"");
        printf("\n\t--------------------------------------");
        printf("-----------------------------------------\n");
        for (Int_t i(0); i<nrows; i++) {
             printf("\t%3d  |",i);
             for (Int_t j(0); j<nrows; j++) printf(" %10.2e|",(*fCovariance)(i,j)); 
             printf("\n");
        }
        printf("\n");
    }
    printf("\n");
}


