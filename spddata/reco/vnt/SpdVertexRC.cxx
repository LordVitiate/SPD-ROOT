// $Id$
// Author: artur   2021/02/20


//_____________________________________________________________________________
//
// SpdVertexRC
//_____________________________________________________________________________

#include "SpdVertexRC.h"
#include "SpdDetectorList.h"
#include "SpdVertexFitPar.h"

#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdVertexRC)

//_____________________________________________________________________________
SpdVertexRC::SpdVertexRC():
fId(-1),fVertexType(-1),fFitPars(0) {}

//_____________________________________________________________________________
SpdVertexRC::SpdVertexRC(Int_t vtype):
fId(-1),fFitPars(0) 
{
    SetVertexType(vtype);
}

//_____________________________________________________________________________
SpdVertexRC::~SpdVertexRC() 
{
    if (fFitPars) delete fFitPars;
}

//_____________________________________________________________________________
void SpdVertexRC::DeleteFitPars() 
{ 
    if (fFitPars) {
        delete fFitPars; 
        fFitPars = 0; 
    }
}

//_____________________________________________________________________________
void SpdVertexRC::Clear(Option_t* option)
{
    DeleteFitPars();
}

//_____________________________________________________________________________
SpdVtxFitPar* SpdVertexRC::FitPars() 
{ 
    if (fFitPars) return fFitPars; 
    
    switch (fVertexType) {
        case 0 : { fFitPars = new SpdPrimVertexFitPar(); break; }
        case 1 : { fFitPars = new SpdVertexFitPar();     break; } 
        case 2 : { fFitPars = new SpdVertexFitPar2();    break; }
        default: {
            cout << "\n-W- <SpdVertexRC::FitPars> Unknown vertex type: " << fVertexType;
            return 0;
        }
    }
    return fFitPars; 
}

//_____________________________________________________________________________
Int_t SpdVertexRC::GetTrack(Int_t i) const
{
    if (i == 0) return *fTracks.begin();
    if (i < 0 || i > fTracks.size()) return -1;
    std::set<Int_t>::const_iterator it = fTracks.begin();
    Int_t n(0);
    while (true) { if (i == n++) return *it; else it++; }
    return -1;
}

//_____________________________________________________________________________
void SpdVertexRC::Print(Option_t* option) const
{
    cout << "\n<SpdVertexRC::Print> ----------------------------------\n\n";
    
    cout << "Unique Id:                " << fId << endl;
    cout << "Vertex (parameters) type: " << fVertexType << endl;
    cout << "Number of vertex tracks:  " << fTracks.size() << endl;
    cout << "Tracks (id):              "; 
    for (const int& i : fTracks) cout << Form("%-4d ",i);
    cout << endl;
    
    if (fFitPars && TString(option).Contains("fitpars")) {
        cout << endl;
        fFitPars->Print();
    }

    cout << endl;
}




