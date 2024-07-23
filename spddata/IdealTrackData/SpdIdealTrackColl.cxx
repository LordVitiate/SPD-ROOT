// $Id$
// Author: artur   2019/09/06


//_____________________________________________________________________________
//
// SpdIdealTrackColl
//_____________________________________________________________________________

#include "SpdIdealTrackColl.h"
#include "SpdIdealDataInfo.h"
#include "SpdIdealTrack.h"

#include <TDatabasePDG.h>

#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdIdealTrackColl)

//_____________________________________________________________________________
SpdIdealTrackColl::SpdIdealTrackColl():TNamed("Ideal event", "Ideal event"),
fRunId(0),fEventId(-1),fVertexTime(0),fErrorFlag(0)
{
   fDataInfo = new TObjArray();
   fTracks   = new TObjArray();
}

//_____________________________________________________________________________
SpdIdealTrackColl::~SpdIdealTrackColl() 
{
  if (fDataInfo) {
      fDataInfo->Delete();
      delete fDataInfo;
      fDataInfo = 0;
  }
  
  if (fTracks) {
      fTracks->Delete();
      delete fTracks;
      fTracks = 0;
  }
}

//_____________________________________________________________________________
void SpdIdealTrackColl::DeleteAll()
{
  fRunId = 0;
  fEventId = -1;
    
  if (fDataInfo) {
      fDataInfo->Delete();
      delete fDataInfo;
      fDataInfo = new TObjArray();
  }
  
  if (fTracks) {
      fTracks->Delete();
      delete fTracks;
      fTracks = new TObjArray();
  }
  
  fVertex.SetXYZ(0,0,0);
  fVertexTime = 0.;
    
  fErrorFlag = 0;
    
  fPrimTrackMarks.clear();
}

//_____________________________________________________________________________
void SpdIdealTrackColl::ClearData()
{
    fRunId = 0;
    fEventId = -1;
        
    if (fTracks && GetNTracks() > 0) {
        fTracks->Delete();
        delete fTracks;
        fTracks = 0;
    }
    
    if (!fTracks) fTracks = new TObjArray();
    
    SpdIdealDataInfo* dinfo;
    for (Int_t i(0); i<GetNDataInfo(); i++) {
         dinfo = (SpdIdealDataInfo*)fDataInfo->At(i);
         if (dinfo) dinfo->ClearData();
    }
    
    fVertex.SetXYZ(0,0,0);
    fVertexTime = 0.;
    
    fErrorFlag = 0;
    
    fPrimTrackMarks.clear();
}

//_____________________________________________________________________________
Int_t SpdIdealTrackColl::AddDataInfo(SpdIdealDataInfo* datainfo)
{
    fDataInfo->Add(datainfo);
    return fDataInfo->GetEntriesFast()-1;
}

//_____________________________________________________________________________
Int_t SpdIdealTrackColl::AddTrack(SpdIdealTrack* track)
{
    fTracks->Add(track);
    return fTracks->GetEntriesFast()-1;
}
//_____________________________________________________________________________
Int_t SpdIdealTrackColl::AddTrackMark(Int_t label)
{ 
    fPrimTrackMarks.push_back(label); 
    return fPrimTrackMarks.size(); 
} 

//_____________________________________________________________________________
void SpdIdealTrackColl::SetInfoNTracks(Int_t n)
{ 
    if (!fDataInfo) return;
    
    Int_t nde = fDataInfo->GetEntriesFast();
    SpdIdealDataInfo* dinfo;

    for (Int_t i(0); i<nde; i++) {
        dinfo = (SpdIdealDataInfo*)fDataInfo->At(i);
        dinfo->SetNTracks(n);
    }
}

//_____________________________________________________________________________
SpdIdealDataInfo* SpdIdealTrackColl::FindDataInfo(Int_t detId)
{
    TObjArrayIter iter(fDataInfo);
    SpdIdealDataInfo* dinfo;
    while(dinfo = ((SpdIdealDataInfo*)iter.Next())) {
      if (dinfo->GetDetectorId() == detId) return dinfo;
    }
    return 0;
}

//_____________________________________________________________________________
SpdIdealDataInfo* SpdIdealTrackColl::GetDataInfo(Int_t i)
{
    return (i < 0 || i >= GetNDataInfo()) ? 0 : (SpdIdealDataInfo*)fDataInfo->At(i);
}

//_____________________________________________________________________________   
SpdIdealTrack* SpdIdealTrackColl::GetTrack(Int_t i)
{
    return (i < 0 || i >= GetNTracks()) ? 0 : (SpdIdealTrack*)fTracks->At(i);
}

//_____________________________________________________________________________
void SpdIdealTrackColl::PrintData() const
{
    cout << "-----------------------------------------------------------------";
    cout << "-----------------------------------------------------------------" << endl;
    
    cout << "-I- <SpdIdealTrackColl::PrintData> Event/Run: " << fEventId << "/" << fRunId 
        << " (errors: " << fErrorFlag << ") " << "\n\n";
    
    cout << "Tracks (vertex/selected): " << GetNPrimTracks() << "/" << fTracks->GetEntriesFast()  << "\n\n";
    
    cout << "Vertex (x,t):  (" << fVertex.X() << "," << fVertex.Y() << "," << fVertex.Z() << ") [cm]  " 
                               << fVertexTime << " [ns] " << "\n\n";  
    
    if (!fDataInfo) return;
    if (!fTracks) return;   
        
    Int_t nde = fDataInfo->GetEntriesFast();
    SpdIdealDataInfo* dinfo;
    
    Int_t tot_hits1 = 0;
    
    if (fDataInfo) {
        for (Int_t i(0); i<nde; i++) {
             dinfo = (SpdIdealDataInfo*)fDataInfo->At(i);
             dinfo->PrintInfo("short");
             tot_hits1 += dinfo->GetNHits();
        }
    }
    printf("\n");

    printf("%-6s %6s %6s %6s %10s %10s %10s %10s %10s %10s %6s %6s",
           "Num","Track","M(1)","M(2)","PDG","p/m","Mom[GeV]","VDist[cm]","Time[ns]","Edep[MeV]","Points","Hits:");
    for (Int_t i(0); i<nde; i++) {
         dinfo = (SpdIdealDataInfo*)fDataInfo->At(i);
         printf(" %8s",Form("Det(%d)",dinfo->GetDetectorId())); 
    }
    printf("\n\n");
    
    Int_t nt = fPrimTrackMarks.size();
    SpdIdealTrack* track;
    
    Int_t tot_hits2 = 0;
    TParticlePDG* part;
    Double_t mass, bg;
    
    Int_t i(0);
    for (Int_t k(0); k<nt; k++) {
         if (fPrimTrackMarks[k] != kAccepted) {
             printf("%-6d [reject] reason: %s\n",k,SpdIdealTrack::GetTrackSelectionMarkStr(fPrimTrackMarks[k]));
             continue;
         }
         track = (SpdIdealTrack*)fTracks->At(i);
         bg = 0;
         part = TDatabasePDG::Instance()->GetParticle(track->GetPdgCode());
         if (part) {
             mass = part->Mass();
             if (mass > 0) bg = track->GetStartMomentumMag()/mass; 
         }
         printf("%-6d %6d %6d %6d %10d %10.3f %10.2e %10.2e %10.2e %10.2e %6d %6d",k,
               track->GetTrackId(),track->GetMother1TrackId(),track->GetMother2TrackId(),
               track->GetPdgCode(),bg,
               track->GetStartMomentumMag(),track->GetPrimVertexDist(),track->GetStartTime(),
               track->GetHitsEdep()*1e3,track->GetNPoints(),track->GetNHits());
         for (Int_t j(0); j<nde; j++) {
              dinfo = (SpdIdealDataInfo*)fDataInfo->At(j);
              printf(" %8d",dinfo->GetTrackHits(i)); 
              tot_hits2 += dinfo->GetTrackHits(i);
         }
         i++;
         printf("\n");
    }
    printf("\nTotal hits: %d/%d \n", tot_hits1, tot_hits2);
    
    printf("\n");
}

