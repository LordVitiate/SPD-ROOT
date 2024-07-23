// $Id$
// Author: artur   2020/10/01

//_____________________________________________________________________________
//
// SpdVertexMC
//_____________________________________________________________________________

#include "SpdVertexMC.h"
#include "SpdDetectorList.h"
#include "SpdVertexFitPar.h"
#include <TMCProcess.h>
#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdVertexMC)

//_____________________________________________________________________________
SpdVertexMC::SpdVertexMC():fId(-1),fGeneration(-1),
fProcessId(kPNoProcess),fDetectorId(kSpdUndefined),fIsSingleProcessType(true),
fX(-1.e10),fY(-1.e10),fZ(-1.e10),fRegionSize(-1),
fNTracks(-1),fFitPars(0)
{
 
}

//_____________________________________________________________________________
SpdVertexMC::~SpdVertexMC() 
{
   if (fFitPars) delete fFitPars;
}

//_____________________________________________________________________________
void SpdVertexMC::DeleteFitPars() 
{ 
    if (fFitPars) {
        delete fFitPars; 
        fFitPars = 0; 
    }
}

//_____________________________________________________________________________
void SpdVertexMC::Clear(Option_t* option)
{
    DeleteFitPars();
}

//_____________________________________________________________________________
void SpdVertexMC::RemoveParticles(Option_t* option)
{
    if (!strcmp(option,"all")) {
        fInParticles.clear();
        fOutParticles.clear();
        fInnerParticles.clear();
        return;
    }
    else if (!strcmp(option,"in"))    { fInParticles.clear();    return; }
    else if (!strcmp(option,"out"))   { fOutParticles.clear();   return; }
    else if (!strcmp(option,"inner")) { fInnerParticles.clear(); return; }
    
    fInParticles.clear();
    fOutParticles.clear();
    fInnerParticles.clear();
    
    return;
}

//_____________________________________________________________________________
SpdVtxFitPar* SpdVertexMC::FitPars() 
{ 
    if (fFitPars) return fFitPars; 
    if (fGeneration == 0) fFitPars = new SpdPrimVertexFitPar();
    else if (fGeneration > 0) fFitPars = new SpdVertexFitPar();
    else fFitPars = 0;
    return fFitPars; 
}

//_____________________________________________________________________________
const Char_t* SpdVertexMC::GetProcessName() const
{
    if (fProcessId < 0 || fProcessId > kPNoProcess) return TMCProcessName[kPNoProcess];
    return TMCProcessName[fProcessId]; 
}

//_____________________________________________________________________________
Int_t SpdVertexMC::GetInParticle(Int_t i) const
{
    if (i == 0) return *fInParticles.begin();
    if (i < 0 || i > fInParticles.size()) return -1;
    std::set<Int_t>::const_iterator it = fInParticles.begin();
    Int_t n(0);
    while (true) { if (i == n++) return *it; else it++; }
    return -1;
}

//_____________________________________________________________________________   
Int_t SpdVertexMC::GetOutParticle(Int_t i) const
{
    if (i == 0) return *fOutParticles.begin();
    if (i < 0 || i > fOutParticles.size()) return -1;
    std::set<Int_t>::const_iterator it = fOutParticles.begin();
    Int_t n(0);
    while (true) { if (i == n++) return *it; else it++; }
    return -1;
}

//_____________________________________________________________________________  
Int_t SpdVertexMC::GetInnerParticle(Int_t i) const
{ 
    if (i == 0) return *fInnerParticles.begin();
    if (i < 0 || i > fInnerParticles.size()) return -1;
    std::set<Int_t>::const_iterator it = fInnerParticles.begin();
    Int_t n(0);
    while (true) { if (i == n++) return *it; else it++; }
    return -1; 
}

//_____________________________________________________________________________
void SpdVertexMC::Print(Option_t* option) const
{
    cout << "\n<SpdVertexMC::Print> " << "\n\n";
    
    cout << "Unique Id:          " << fId << endl;
    cout << "Generation:         " << fGeneration << endl;
    cout << "Process:            " << GetProcessName() << " (id = " << fProcessId << ") " << endl;
    cout << "Single proc. type:  " << (fIsSingleProcessType ? "yes" : "no") << endl;
    cout << "Detector Id:        " << fDetectorId << endl;
    cout << "Number of tracks:   " << fNTracks << endl;
     
    return;
    
    if (IsVertexPosDefined()) {
        cout << "Position:           " << fX << ", " << fY << ", " << fZ << " [cm] " 
                                   << " (D(0) = " << GetPos().Mag() << " cm) " << endl;
    }
    else cout << "Position:           " << "undefined" << endl;
    
    cout << "Region size:        " << fRegionSize << " [cm] " << endl;
    
    Int_t n;
    
    cout << "\nVertex (total) particles [" << GetNInOut()+GetNInner() << "]: \n";
    
    n = 0;
    cout << Form("\nIn    %8s:   ",Form("[%d]",(Int_t)fInParticles.size())) ; 
    for (const Int_t& i : fInParticles) {
         if (++n % 10 == 0) cout << Form("\n%18s","");
         else cout << Form("%6d",i);
    }
    
    cout << "\n";
    n = 0;
    cout << Form("\nOut   %8s:   ",Form("[%d]",(Int_t)fOutParticles.size())) ; 
    for (const Int_t& i : fOutParticles) {
         if (++n % 10 == 0) cout << Form("\n%18s","");
         else cout << Form("%6d",i);
    }
    
    cout << "\n";
    if (fInnerParticles.size() > 0) {
        n = 0;
        cout << Form("\nInner %8s:   ",Form("[%d]",(Int_t)fInnerParticles.size())) ; 
        for (const Int_t& i : fInnerParticles) {
             if (++n % 10 == 0) cout << Form("\n%18s","");
             else cout << Form("%6d",i);
        }
        cout << "\n";
    }
    
    if (fFitPars && TString(option).Contains("fitpars")) {
        cout << endl;
        fFitPars->Print();
    }
    
    cout << endl;
}




