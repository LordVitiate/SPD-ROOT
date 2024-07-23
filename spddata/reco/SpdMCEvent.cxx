// $Id$
// Author: artur   2020/09/22

//_____________________________________________________________________________
//
// SpdMCEvent
//_____________________________________________________________________________

#include "SpdMCEvent.h"

#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdMCEvent)

//_____________________________________________________________________________
SpdMCEvent::SpdMCEvent():TNamed("MCEvent","MCEvent"),
fRunId(0),fEventId(-1), 
fMotherRunId(0),fMotherEventId(-1), 
fNTotalPParticles(0),fNInPParticles(0),fNOutPParticles(0),
fNParticles(0),fNMCTracks(0),
fMaxParticleGeneration(-1),fMaxVertexGeneration(-1),
fInfo(0)
{
 
}

//_____________________________________________________________________________
SpdMCEvent::~SpdMCEvent() 
{
    if (fInfo) { fInfo->Delete(); delete fInfo; }
}

//_____________________________________________________________________________
void SpdMCEvent::Clear(Option_t* option)
{
    fRunId   =  0;
    fEventId = -1; 
    
    fNTotalPParticles = 0;
    fNInPParticles    = 0;
    fNOutPParticles   = 0;
    
    fNParticles = 0;
    fNMCTracks  = 0;
    
    fMaxParticleGeneration = -1;
    fMaxVertexGeneration   = -1;
    
    if (fInfo) fInfo->Delete();
}

//_____________________________________________________________________________
Bool_t SpdMCEvent::AddInfo(TString info_name)
{
    SpdBaseParSet* info;
    
    if (!fInfo) {
        fInfo = new TObjArray();
        fInfo->SetOwner(true);
        info = new SpdBaseParSet(info_name);
        fInfo->Add(info);
        return true;
    }
    
    info = (SpdBaseParSet*)fInfo->FindObject(info_name);
    if (info) return false;
    
    info = new SpdBaseParSet(info_name);
    fInfo->Add(info);
    return true;
}
 
//_____________________________________________________________________________    
SpdBaseParSet* SpdMCEvent::GetInfo(TString info_name)
{
    if (!fInfo || fInfo->GetEntriesFast() < 1) return 0;
    return (SpdBaseParSet*)fInfo->FindObject(info_name);
}

//_____________________________________________________________________________
Bool_t SpdMCEvent::HasFullListOfParticles() const
{
    Int_t nemax = fNMCTracks + (fNTotalPParticles - fNOutPParticles);
    return (nemax == fNParticles);
}

//_____________________________________________________________________________
void SpdMCEvent::PrintEvent(Int_t opt) const
{
    cout << "-I- <SpdMCEvent::PrintEvent> \n\n";  
  
    Int_t nemax = fNMCTracks + (fNTotalPParticles - fNOutPParticles);
    
    cout << "Event, Run (ID):            " << fEventId << "  " << fRunId << endl;
    cout << "Mother Event, Run (ID):     " << fMotherEventId << "  " << fMotherRunId << endl;
    cout << "Total prim. particles:      " << fNTotalPParticles << "  (GENERATION = 0+1) " << endl;
    cout << "Total prim. in.  particles: " << fNInPParticles << "  (MOTHER ID = -1)" <<endl;
    cout << "Total prim. out. particles: " << fNOutPParticles <<  "  (GENERATION = 1) " << endl;
    cout << "Total mc-tracks:            " << fNMCTracks <<  "  (TOTAL INPUT MC-TRACKS) " << endl;
    cout << "Total particles:            " << fNParticles <<  "  (TOTAL OUTPUT MC-PARTICLES, MAX = N(G0)+N(MC-TRACKS) = " 
                                           << nemax << ") "<< endl;
    cout << "Max. particle generation:   " << fMaxParticleGeneration << endl;
    cout << "Max. vertex generation:     " << fMaxVertexGeneration << endl;
    cout << "Full list of particles:     " << ((HasFullListOfParticles()) ? "yes" : "no") << endl;
    cout << endl;
    
    cout << "N(info) = " << GetNInfo() << endl;
    cout << endl;
    
    if (GetNInfo() > 0) {
        TObjArrayIter next(fInfo);
        SpdBaseParSet* pars;
        while((pars = dynamic_cast<SpdBaseParSet*>(next()))) pars->print(opt);
        cout << endl;
    }

} 








