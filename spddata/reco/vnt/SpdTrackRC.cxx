// $Id$
// Author: vladimir   2021/11/28


//_____________________________________________________________________________
//
// SpdTrackRC
//_____________________________________________________________________________

#include "SpdTrackRC.h"
#include "SpdDetectorList.h"

#include <iostream>

using std::cout;
using std::endl;

using std::vector;
using std::pair;

ClassImp(SpdTrackRC)

//_____________________________________________________________________________
SpdTrackRC::SpdTrackRC():fId(-1),fParticleId(-1),fPdgCode(-1),fVertexId(-1),
fFitPars(0)  
{
   memset(fNHits,0,3*sizeof(Int_t));
}

//_____________________________________________________________________________
SpdTrackRC::~SpdTrackRC() 
{
   if (fFitPars) delete fFitPars; 
}

//_____________________________________________________________________________
void SpdTrackRC::DeleteFitPars()
{
    if (fFitPars) delete fFitPars;
    fFitPars = 0;
}

//_____________________________________________________________________________
void SpdTrackRC::Clear(Option_t* option)
{
    DeleteFitPars();
}
 
//_____________________________________________________________________________
void SpdTrackRC::ClearAll() 
{
    fId = -1;
    fParticleId = -1;
    fVertexId   = -1;
    
    memset(fNHits,0,3*sizeof(Int_t));
    
    fPoints.clear();
    
    if (fFitPars) {
        delete fFitPars;
        fFitPars = new SpdTrackFitPar();
    }
}

//_____________________________________________________________________________
SpdTrackFitPar* SpdTrackRC::FitParameters()
{ 
    if (fFitPars) return fFitPars; 
    fFitPars = new SpdTrackFitPar();
    return fFitPars; 
}

//_____________________________________________________________________________
void SpdTrackRC::Print(Option_t* option) const
{
    cout << "\n<SpdTrackRC::Print> ----------------------------------\n\n";
    
    cout << "Id:                   " << fId << endl;
    cout << "Particle (id):        " << fParticleId << endl;
    cout << "Particle (pdg):       " << fPdgCode << endl;
    cout << "Vertex (id):          " << fVertexId << endl;
    cout << "Number of points:     " << fPoints.size() << endl;
    cout << "Hits (Its/TsB,TsEC):  " << GetNHits() 
         << "   (" << fNHits[0] << " / " << fNHits[1] << " / "<< fNHits[2] << ") "<< endl;
    
    if (TString(option).Contains("points") && fPoints.size() > 0) {
        Int_t i(0);
        printf("\n\t%5s  %7s   %s \n\n","I","HIT ID","BRANCH (NUM)");
        for (auto p : fPoints) {
             printf("\t%5d  %7d   %s (%d)\n",i++,p.first,SpdDataBranchName(p.second),p.second);
        }
    }
    
    if (fFitPars && TString(option).Contains("fitpars")) {
        cout << endl;
        fFitPars->Print();
    }
    
    //cout << endl;
}



