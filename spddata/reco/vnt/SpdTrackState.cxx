// $Id$
// Author: artur   2021/02/10


//_____________________________________________________________________________
//
// SpdTrackState
//_____________________________________________________________________________

#include "SpdTrackState.h"

#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdTrackState)

//_____________________________________________________________________________
SpdTrackState::SpdTrackState(const SpdTrackState& state):TObject(state),
fMomentum(state.fMomentum),fPosition(state.fPosition),
fTime(state.fTime),fMomSigma(state.fMomSigma),fPosDelta(state.fPosDelta)
{
    fCovariance.ResizeTo(state.fCovariance); 
    fCovariance = state.fCovariance;
}

//_____________________________________________________________________________
void SpdTrackState::ClearState()
{
    fMomentum.SetXYZ(0,0,0);
    fPosition.SetXYZ(0,0,0);
    fCovariance.ResizeTo(0,0); 
    fTime = -1;
    fMomSigma = -1;
    fPosDelta = -1;
}

//_____________________________________________________________________________
void SpdTrackState::Print(Option_t* option) const
{
    cout << "\t<SpdTrackState::Print> " << option << "\n\n";
    
    printf("\tMomentum [GeV]:   %12.6f, %12.6f, %12.6f;  |p| = %-12f  +-  %-12f\n",
           fMomentum.X(),fMomentum.Y(),fMomentum.Z(),fMomentum.Mag(),fMomSigma);
    printf("\tPosition  [cm]:   %12.6f, %12.6f, %12.6f;  D0  = %-12f, d = %-12f\n",
           fPosition.X(),fPosition.Y(),fPosition.Z(),fPosition.Mag(),fPosDelta);   
    printf("\tTime      [ns]:   %12.6f\n",fTime);         
     
    Int_t nrows = fCovariance.GetNrows();
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
             for (Int_t j(0); j<nrows; j++) printf(" %10.2e|",fCovariance(i,j)); 
             printf("\n");
        }
        printf("\n");
    }
    printf("\n");
}


