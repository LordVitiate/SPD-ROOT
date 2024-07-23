// $Id$
// Author: artur   2019/09/06


//_____________________________________________________________________________
//
// SpdIdealTrack
//_____________________________________________________________________________

#include "SpdIdealTrack.h"
#include "SpdIdealHit.h"
#include "SpdIdealSpacepointHit.h"

#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdIdealTrack)

static const Char_t* SpdTrackSelectionMarkStr[kFullySpecified+1] = 
{ "Defined","Accepted",
  "VertexParticle", "Identity", "Charge", 
  "Momentum", "BetaGamma", 
  "VDHits", "TrackerHits", "TBHits", "TECHitsLim",
  "kFullySpecified"
};

const Char_t* SpdIdealTrack::GetTrackSelectionMarkStr(UInt_t i) // static 
{ return (i >= 0 && i < UInt_t(kFullySpecified)) ? SpdTrackSelectionMarkStr[i] : ""; }

const Char_t* SpdIdealTrack::GetTrackSelectionMarkStr(SpdTrackSelectionMark mark) // static 
{ return (mark >= kDefined && mark < kFullySpecified) ? SpdTrackSelectionMarkStr[UInt_t(mark)] : ""; }

// void   SpdIdealTrack::SetMark(UInt_t& selection, SpdTrackSelectionMark mark) { selection = selection | (1 << UInt_t(mark)); }
// void   SpdIdealTrack::SetMark(UInt_t& selection, UInt_t mark) { selection = selection | (1 << mark); }
// Bool_t SpdIdealTrack::GetMark(const UInt_t selection, SpdTrackSelectionMark mark) { return (selection >> UInt_t(mark)) & 1; }
// Bool_t SpdIdealTrack::GetMark(const UInt_t selection, UInt_t mark) { return (selection >> mark) & 1; }
// Bool_t SpdIdealTrack::IsDefined(const UInt_t selection) { return (selection >> UInt_t(kDefined)) & 1; }
// Bool_t SpdIdealTrack::IsAccepted(const UInt_t selection) { return (selection >> UInt_t(kAccepted)) & 1; }
// Bool_t SpdIdealTrack::IsFullySpecified(const UInt_t selection) { return (selection >> UInt_t(kFullySpecified)) & 1; }
   
//_____________________________________________________________________________
SpdIdealTrack::SpdIdealTrack():fTrackId(-10),fMother1TrackId(-10),fMother2TrackId(-10),
fPdgCode(0),fCharge(0),fVProcess(0),fStartTime(0),fNPoints(0),fErrorFlag(0)
{
   fHits = new TObjArray();
}

//_____________________________________________________________________________
SpdIdealTrack::~SpdIdealTrack() 
{
   if (fHits) {
       fHits->Delete();
       delete fHits;
       fHits = 0;
   }
}

//_____________________________________________________________________________
void SpdIdealTrack::ClearData()
{
    fTrackId = -10;
    fMother1TrackId = -10;
    fMother2TrackId = -10;
    
    fPdgCode = 0;
    fCharge = 0;
    fVProcess = -1;
    
    fPrimVertex.SetXYZ(0,0,0);
    fStartMom.SetXYZ(0,0,0);
    
    fStartTime = 0.;
    
    fNPoints = 0;
    fErrorFlag = 0;
       
    if (fHits && GetNHits() > 0) {
        fHits->Delete();
        delete fHits;
    }
    
    if (!fHits) fHits = new TObjArray();
}

//_____________________________________________________________________________
Int_t SpdIdealTrack::AddHit(SpdIdealHit* hit)
{
    fHits->Add(hit);
    return fHits->GetEntriesFast()-1;
}

//_____________________________________________________________________________   
SpdIdealHit* SpdIdealTrack::GetHit(Int_t i)
{
    return (i < 0 || i >= GetNHits()) ? 0 : (SpdIdealHit*)fHits->At(i);
}

//_____________________________________________________________________________   
Double_t SpdIdealTrack::GetHitsEdep() const
{
    Int_t nh = fHits->GetEntriesFast();
    Double_t edep = 0;
    for (Int_t i(0); i<nh; i++) edep += ((SpdIdealHit*)fHits->At(i))->GetEdep();
    return edep;
}

//_____________________________________________________________________________
void SpdIdealTrack::PrintTrack(Bool_t print_hits) const
{
    cout << "-I- <SpdIdealTrack::PrintData> \n";
    
    printf("ID [track,mother(1,2)]:  %d (%d,%d)\n",fTrackId,fMother1TrackId,fMother2TrackId);
    printf("Pdg number:              %d\n",fPdgCode);  
    printf("Charge:                  %-2.0f\n",fCharge);  
    printf("Prim. process:           %d\n",fVProcess);
    printf("Prim. vertex:            (%f,%f,%f)\n", fPrimVertex.X(),fPrimVertex.Y(),fPrimVertex.Z());
    printf("Start momentum:          (%f,%f,%f) %f\n", fStartMom.X(),fStartMom.Y(),fStartMom.Z(),fStartMom.Mag());
    printf("Start time [ns]:         %f [ns]\n",fStartTime);
    printf("Number of mc-points:     %d\n",fNPoints);
    printf("Number of hits:          %d\n",GetNHits());
    printf("Total deposit (hits):    %f [GeV]\n",GetHitsEdep());
    printf("Error flag:              %d \n",GetErrorFlag());
  
    printf("\n");
    
    if (!print_hits) return;
    
    Int_t nh = fHits->GetEntriesFast();
    for (Int_t i(0); i<nh; i++) {
         cout << "\tHit number: " << i << "/" << GetNHits() << "\n";
         ((SpdIdealHit*)fHits->At(i))->PrintHit();
    }
}


