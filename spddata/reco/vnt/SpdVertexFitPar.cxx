// $Id$
// Author: artur   2020/10/01


//_____________________________________________________________________________
//
// SpdVertexFitPar
//_____________________________________________________________________________

#include "SpdVertexFitPar.h"
#include <TMath.h>
#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdVtxFitPar)
ClassImp(SpdPrimVertexFitPar)
ClassImp(SpdVertexFitPar)
ClassImp(SpdVertexFitParDaughterTrack)

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                          SpdPrimVertexFitPar
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________
void SpdPrimVertexFitPar::Print(Option_t* option) const
{
    cout << "\n\t<SpdPrimVertexFitPar::Print> " << "\n\n";
    
    cout << "\tNumber of tracks:  " << fNTracks << endl;
    cout << "\tVertex [cm]:       " << fVertex.X() << ", " << fVertex.Y() << ", " << fVertex.Z() << endl;
    cout << "\tChi2:              " << fChi2 << endl;
    cout << "\tNDF:               " << fNDF << endl;

    Int_t nrows = fVertexCov.GetNrows();
    if (nrows > 0) 
    {
        printf("\n");
        printf("\tCovariation matrix (%dx%d): \n",nrows,nrows);
      
        printf("\n\t%4s |","");
        for (Int_t i(0); i<nrows; i++) printf(" %5d%5s|",i,"");
        printf("\n\t-------------------------------------------\n");
        for (Int_t i(0); i<nrows; i++) {
             printf("\t%3d  |",i);
             for (Int_t j(0); j<nrows; j++) printf(" %10.2e|",fVertexCov(i,j)); 
             printf("\n");
        }
        printf("\n");
    }
    //printf("\n");
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                            SpdVertexFitPar
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________
void SpdVertexFitPar::Print(Option_t* option) const
{
    cout << "\n\t<SpdVertexFitPar::Print> " << "\n\n";
    
    cout << "\tMass od decay particle:        " << fMass    << endl;
    cout << "\tError of mass decay particle:  " << fMassErr << endl;
    cout << "\tMomentum of decay particle:    " << fP       << endl;
    cout << "\tEnergy of decay particle:      " << fEnergy  << endl;
    cout << "\tEta of decay particle:         " << fEta     << endl;
    cout << "\tTheta of decay particle:       " << fTheta   << endl;
    cout << "\tPhi of decay particle:         " << fPhi     << endl;
    cout << "\tDecay length:                  " << fLength  << endl;
    
    cout << endl;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                            SpdVertexFitPar2
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________
void SpdVertexFitParDaughterTrack::print(Int_t n) const
{
    if (n < 0) {
        printf("%3s %12s %12s %12s  %8s  %8s  %8s  %10s\n",
               "","m","p","pt","η","φ","θ","χ2(PV)");  
        return;
    }

    printf("%3d %12.6f %12.6f %12.6f %8.2f %8.2f %8.2f %10.2e\n",
           n, Trk_mass_,Trk_mom_,Trk_pt_,
           Trk_eta_,Trk_phi_*TMath::RadToDeg(),
           Trk_theta_*TMath::RadToDeg(),Trk_chi2PV_);  
}

//_____________________________________________________________________________
SpdVertexFitPar2::SpdVertexFitPar2()
{
    Reset();
}

//_____________________________________________________________________________
void SpdVertexFitPar2::Reset()
{
    fMass      = -99.; 
    fMassErr   = -99.; 
    fMom       = -99.; 
    fMomErr    = -99.; 
    fPt        = -99.; 
    fPtErr     = -99.; 
    fTheta     = -99.; 
    fThetaErr  = -99.; 
    fPhi       = -99.; 
    fPhiErr    = -99.; 
    fEta       = -99.; 
    fEtaErr    = -99.; 
    fLength    = -99.; 
    fLengthErr = -99.; 
    fCtau      = -99.; 
    fCtauErr   = -99.; 
    fChi2PV    = -99.; 
    fChi2SV    = -99.; 
    fQtAlfa1   = -99.; 
    fQtAlfa2   = -99.; 
}

//_____________________________________________________________________________
void SpdVertexFitPar2::Print(Option_t* option) const
{
    cout << "\n\t<SpdVertexFitPar2::Print> " << "\n\n";    
    
    printf("Mother particle: \n\n");
    cout << "\tProduction Vertex [cm]:  " << fProductionVertex.X() << ", " << fProductionVertex.Y() << ", " << fProductionVertex.Z() << endl;
    cout << "\tDecay Vertex [cm]:       " << fProductionVertex.X() << ", " << fProductionVertex.Y() << ", " << fProductionVertex.Z() << endl;
    printf("Mass           :  %12.6f   +-  %12.6f [GeV]\n",fMass,fMassErr);
    printf("P              :  %12.6f   +-  %12.6f [GeV]\n",fMom,fMomErr); 
    printf("Pt             :  %12.6f   +-  %12.6f [GeV]\n",fPt,fPtErr); 
    printf("θ              :  %12.6f   +-  %12.6f [deg]\n",fTheta*TMath::RadToDeg(),fThetaErr*TMath::RadToDeg()); 
    printf("φ              :  %12.6f   +-  %12.6f [deg]\n",fPhi*TMath::RadToDeg(),fPhiErr*TMath::RadToDeg()); 
    printf("η              :  %12.6f   +-  %12.6f      \n",fEta,fEtaErr); 
    printf("Decay length   :  %12.6f   +-  %12.6f [cm] \n",fLength,fLengthErr);
    printf("Life time      :  %12.6f   +-  %12.6f (cτ) \n",fCtau,fCtauErr);
    printf("\n");
    printf("χ2 (PV)        :  %12.6f \n", fChi2PV);
    printf("χ2 (SV)        :  %12.6f \n", fChi2SV);
    printf("\n");       
    printf("qtα1:          :  %12.6f \n", fQtAlfa1);
    printf("qtα2:          :  %12.6f \n", fQtAlfa2);      
    printf("\n");  
    printf("Daughters      :  %12d                     \n",(uint)fDaughters.size());
    printf("\n"); 
    
    if (fDaughters.empty()) return;
    
    fDaughters[0].print(-1);
    printf("\n"); 
    
    Int_t n(0);
    for (auto d : fDaughters) d.print(n++);
    printf("\n"); 
}



