
//_____________________________________________________________________________
//
// SpdMCTrack
//_____________________________________________________________________________

#include "SpdMCTrack.h"

#include "TDatabasePDG.h"  
#include "TParticle.h"          
#include "TParticlePDG.h"
#include "TMCProcess.h"

#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdMCTrack)

//_____________________________________________________________________________________
SpdMCTrack::SpdMCTrack() : 
   TObject(),
   fPdgCode(0),
   fMotherId(-1),
   fSecondMotherId(-1),
   fVProcess(-1),
   fPx(0.),
   fPy(0.),
   fPz(0.),
   fStartX(0.),
   fStartY(0.),
   fStartZ(0.),
   fStartT(0.),
   fVModule(kSpdUndefined),
   fVNodeName(""),
   fNPoints(0)
{
}

//_____________________________________________________________________________________
SpdMCTrack::SpdMCTrack(Int_t pdgCode, Int_t motherId, Double_t px,
                       Double_t py, Double_t pz, Double_t x, Double_t y,
                       Double_t z, Double_t t, Int_t nPoints = 0) :
   TObject(),
   fPdgCode(pdgCode),
   fMotherId(motherId),
   fSecondMotherId(motherId),
   fVProcess(-1),
   fPx(px),
   fPy(py),
   fPz(pz),
   fStartX(x),
   fStartY(y),
   fStartZ(z),
   fStartT(t),
   fVModule(kSpdUndefined),
   fVNodeName(""),
   fNPoints(nPoints)
{
}

//_____________________________________________________________________________________
SpdMCTrack::SpdMCTrack(TParticle* part) :
   TObject(),
   fPdgCode(part->GetPdgCode()),
   fMotherId(part->GetMother(0)),
   fSecondMotherId(part->GetMother(1)),
   fVProcess(part->GetUniqueID()),
   fPx(part->Px()),
   fPy(part->Py()),
   fPz(part->Pz()),
   fStartX(part->Vx()),
   fStartY(part->Vy()),
   fStartZ(part->Vz()),
   fStartT(part->T()*1e09),
   fVModule(kSpdUndefined),
   fVNodeName(""),
   fNPoints(0)
{
  memset(fPoints,0,sizeof(Int_t)*NSPDDETECTORS);
}

//_____________________________________________________________________________________
SpdMCTrack::SpdMCTrack(const SpdMCTrack& track) :
   TObject(track),
   fPdgCode(track.fPdgCode),
   fMotherId(track.fMotherId),
   fSecondMotherId(track.fSecondMotherId),
   fVProcess(track.fVProcess),
   fPx(track.fPx),
   fPy(track.fPy),
   fPz(track.fPz),
   fStartX(track.fStartX),
   fStartY(track.fStartY),
   fStartZ(track.fStartZ),
   fStartT(track.fStartT),
   fVModule(kSpdUndefined),
   fVNodeName(""),
   fNPoints(track.fNPoints)
{
  memcpy(fPoints,track.fPoints,sizeof(Int_t)*NSPDDETECTORS);
}

//_____________________________________________________________________________________
SpdMCTrack::~SpdMCTrack() 
{ 
}

//_____________________________________________________________________________________
Double_t SpdMCTrack::GetMass() const
{
   if (TDatabasePDG::Instance()) {
       TParticlePDG* particle = TDatabasePDG::Instance()->GetParticle(fPdgCode);
       if (particle) return particle->Mass();
       else return 0.;
   }
   return 0.;
}

//_____________________________________________________________________________________
Int_t SpdMCTrack::GetNPoints(DetectorId detId) const
{
   Int_t i = Int_t(detId);
   return (!(i < 0) && i < NSPDDETECTORS) ? fPoints[i] : 0;
}

//_____________________________________________________________________________________
void SpdMCTrack::SetNPoints(Int_t iDet, Int_t nPoints)
{
   if ( iDet < 0 || iDet >= NSPDDETECTORS) return;
   
   fPoints[Int_t(iDet)] = nPoints;
   fNPoints += nPoints;   
//    cout << "<SpdMCTrack::SetNPoints> "
//         << " Mother: " << fMotherId 
//         << " Particle: " << fPdgCode
//         << " Detector: " << iDet 
//         << " Points: " << fPoints[Int_t(iDet)] << "/" << fNPoints   << endl;
}

//_____________________________________________________________________________________
void SpdMCTrack::Print(Int_t trackId) const
{
   cout << "Track " << trackId 
        << ", m(1,2) : " << fMotherId << ", " << fSecondMotherId << ", "
        << "PDG: " << fPdgCode << ", Vmodule: " << fVModule << " (" << fVNodeName << "), "
        << " V (" << fStartX << ", " << fStartY << ", " << fStartZ << ") cm; "
        << " P (" << fPx << ", " << fPy << ", "<< fPz << ") GeV; "
        << " np: " << fNPoints  << ", " 
        << " proc: " << TMCProcessName[fVProcess] 
        << endl;
}






