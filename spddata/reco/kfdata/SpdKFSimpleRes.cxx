// $Id$
// Author: artur   2019/09/18


//_____________________________________________________________________________
//
// SpdKFSimpleRes
//_____________________________________________________________________________

#include "SpdKFSimpleRes.h"
#include <TDatabasePDG.h>

#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdKFSimpleRes)

//_____________________________________________________________________________
SpdKFSimpleRes::SpdKFSimpleRes():TNamed("KF data","KF data"),fRunId(0),fEventId(-1),
fNVertexTracks(0),fNSelectedTracks(0),fVertexChi2overNDF(0),fVertexRECcov(3,3)
{
}

//_____________________________________________________________________________
SpdKFSimpleRes::~SpdKFSimpleRes() 
{
}

//_____________________________________________________________________________
void SpdKFSimpleRes::ClearData() 
{
    fRunId = 0;
    fEventId = -1;
    
    fNVertexTracks = 0;
    fNSelectedTracks = 0;
    
    fVertexMC.SetXYZ(0,0,0);
    fVertexREC.SetXYZ(0,0,0);

    ClearVertexData();
    
    fTracks.clear();
}

//_____________________________________________________________________________
void SpdKFSimpleRes::ClearVertexData()
{
    fVertexRECcov.Clear();
    fVertexRECcov.ResizeTo(3,3);
    fVertexChi2overNDF = 0;
}

//_____________________________________________________________________________
void SpdKFSimpleRes::AddErrorMessage(Int_t i, const Char_t* msg)  
{ 
    if (!FindTrack(i)) return;
    fTracks[i].ErrorFlag = 1; 
    if (fTracks[i].ErrorMessage.IsWhitespace()) fTracks[i].ErrorMessage = msg;
    else { fTracks[i].ErrorMessage += "/"; fTracks[i].ErrorMessage += msg; }
}

//_____________________________________________________________________________
Int_t SpdKFSimpleRes::AddTrack(Int_t ntrack, Int_t pdg)
{ 
    fTracks.push_back(SpdKFSimpleResTrack(ntrack,pdg));
    return fTracks.size()-1;
}

//_____________________________________________________________________________
Int_t SpdKFSimpleRes::AddTrack(Int_t ntrack, Int_t pdg, TVector3& Pmc, TVector3& Prec)
{ 
    fTracks.push_back(SpdKFSimpleResTrack(ntrack,pdg,Pmc,Prec));
    return fTracks.size()-1;
}

//_____________________________________________________________________________
Bool_t SpdKFSimpleRes::GetIsGood(Int_t i) const
{
    if (!FindTrack(i)) return false;
    if (!fTracks[i].IsFitted) return false;
    if (fTracks[i].Convergency != 1) return false; 
    if (fTracks[i].ErrorFlag != 0) return false;
    return true;
}

//_____________________________________________________________________________
Int_t SpdKFSimpleRes::GetNTracks(Int_t& nfitted, Int_t& nconverged, Int_t& noerrors) const
{
    nfitted = 0;
    nconverged = 0;
    noerrors = 0;
    
    for (Int_t i(0); i<fTracks.size(); i++) { 
         if (fTracks[i].IsFitted) nfitted++;
         if (fTracks[i].Convergency == 1) nconverged++;
         if (fTracks[i].ErrorFlag == 0) noerrors++;
    }
    
    return fTracks.size();
}

//_____________________________________________________________________________
Int_t SpdKFSimpleRes::GetNGoodTracks() const
{
    Int_t n = 0;
    for (Int_t i(0); i<fTracks.size(); i++) { 
         if (!fTracks[i].IsFitted) continue;
         if (fTracks[i].Convergency != 1) continue; 
         if (fTracks[i].ErrorFlag != 0) continue;
    }
    
    return n;
}

//_____________________________________________________________________________
Int_t SpdKFSimpleRes::GetNTracks(Bool_t without_errors, Bool_t converged, Bool_t fitted) const
{
    Int_t n = 0;

    for (Int_t i(0); i<fTracks.size(); i++) { 
         if (fitted && !fTracks[i].IsFitted) continue;
         if (converged && fTracks[i].Convergency != 1) continue;
         if (without_errors && fTracks[i].ErrorFlag != 0) continue;
         n++;
    }
    return n;
}

//_____________________________________________________________________________
Double_t SpdKFSimpleRes::GetMomRes(Bool_t without_errors, Bool_t converged) const 
{ 
    Double_t res(0.), m;
    Int_t n(0);
    
    for (Int_t i(0); i<fTracks.size(); i++) { 
         if (!fTracks[i].IsFitted) continue;
         if (converged && fTracks[i].Convergency != 1) continue;
         if (without_errors && fTracks[i].ErrorFlag != 0) continue;
         m = fTracks[i].MomentumMC.Mag();
         if (!(m > 0)) continue;
         res += TMath::Abs(m-fTracks[i].MomentumREC.Mag())/m;
         n++;
    }
    
    return (n > 1) ? res/n : 0.;
}

//_____________________________________________________________________________
Double_t SpdKFSimpleRes::GetMomResN(Int_t& nres, Bool_t without_errors, Bool_t converged) const
{
    nres = 0;
    
    if (!fTracks.size() < 1) return 0.;
        
    Double_t  res(0), m;
    for (Int_t i(0); i<fTracks.size(); i++) { 
         if (!fTracks[i].IsFitted) continue;
         if (converged && fTracks[i].Convergency != 1) continue;
         if (without_errors && fTracks[i].ErrorFlag != 0) continue;
         m = fTracks[i].MomentumMC.Mag();
         if (!(m > 0)) continue;
         res += TMath::Abs(m-fTracks[i].MomentumREC.Mag())/m;
         nres++;
    }
    
    return (nres > 1) ? res/nres : 0.;
}

//_____________________________________________________________________________
Double_t SpdKFSimpleRes::GetMomTRes(Bool_t without_errors, Bool_t converged) const 
{ 
    Double_t res(0.), m;
    Int_t n(0);
    
    for (Int_t i(0); i<fTracks.size(); i++) { 
         if (!fTracks[i].IsFitted) continue;
         if (converged && fTracks[i].Convergency != 1) continue;
         if (without_errors && fTracks[i].ErrorFlag != 0) continue;
         m = fTracks[i].MomentumMC.Perp();
         if (!(m > 0)) continue;
         res += TMath::Abs(m-fTracks[i].MomentumREC.Perp())/m;
         n++;
    }
    
    return (n > 1) ? res/n : 0.;
}

//_____________________________________________________________________________
Double_t SpdKFSimpleRes::GetMomTResN(Int_t& nres, Bool_t without_errors, Bool_t converged) const
{
    nres = 0;
    
    if (!fTracks.size() < 1) return 0.;
        
    Double_t  res(0), m;
    for (Int_t i(0); i<fTracks.size(); i++) { 
         if (!fTracks[i].IsFitted) continue;
         if (converged && fTracks[i].Convergency != 1) continue;
         if (without_errors && fTracks[i].ErrorFlag != 0) continue;
         m = fTracks[i].MomentumMC.Perp();
         if (!(m > 0)) continue;
         res += TMath::Abs(m-fTracks[i].MomentumREC.Perp())/m;
         nres++;
    }
    
    return (nres > 1) ? res/nres : 0.;
}

//_____________________________________________________________________________
Int_t SpdKFSimpleRes::GetRes(Double_t& rp, Double_t& rpt, Double_t& rpz, Double_t& rtheta, Double_t& rphi,
                             Bool_t without_errors, Bool_t converged) const 
{
    return 0;
}

//_____________________________________________________________________________                     
Int_t SpdKFSimpleRes::GetRes(Double_t& rp, Double_t& rpx, Double_t& rpy,  Double_t& rpz,
                    Bool_t without_errors, Bool_t converged) const
{
    return 0; 
}
                    
//_____________________________________________________________________________
Double_t SpdKFSimpleRes::GetGammaBetaMC(Int_t i) const
{   
    if (!FindTrack(i)) return 0;

    if (fTracks[i].TrackPdg == 0) return 0;
    TParticlePDG* part = TDatabasePDG::Instance()->GetParticle(fTracks[i].TrackPdg);
    if (!part) return 0;
    Double_t mass = part->Mass();
    return (mass > 0) ? fTracks[i].MomentumMC.Mag()/mass : 0;
}

//_____________________________________________________________________________
Double_t SpdKFSimpleRes::GetGammaBetaREC(Int_t i) const
{   
    if (!FindTrack(i)) return 0;

    if (fTracks[i].TrackPdg == 0) return 0;
    TParticlePDG* part = TDatabasePDG::Instance()->GetParticle(fTracks[i].TrackPdg);
    if (!part) return 0;
    Double_t mass = part->Mass();
    return (mass > 0) ? fTracks[i].MomentumREC.Mag()/mass : 0;
}

//_____________________________________________________________________________
Bool_t SpdKFSimpleRes::GetSeedCov(Int_t i, TMatrixDSym& c) const
{
    if (FindTrack(i)) { 
        c.ResizeTo(fTracks[i].SeedCov);
        c = fTracks[i].SeedCov; 
        return true;
    }
    return false;
}

//_____________________________________________________________________________
Bool_t SpdKFSimpleRes::GetCovMom(Int_t i, TMatrixDSym& c) const 
{   
    if (FindTrack(i)) { 
        c.ResizeTo(fTracks[i].CovMom);
        c = fTracks[i].CovMom; 
        return true;
    }
    return false;
}

//_____________________________________________________________________________
Double_t SpdKFSimpleRes::GetMomSigma(Int_t i, Int_t axis /*1(px),2(py),3(pz)*/) const
{
    if (axis < 1 || axis > 3) return -11;
  
    TMatrixDSym c;
    if (!GetCovMom(i,c)) return -10;
    
    Double_t disp = -1;
    switch (axis) {
       case 1: disp = c(0,0); break;
       case 2: disp = c(1,1); break;
       case 3: disp = c(2,2); break;
       default : break;
    }

    return (disp > 0) ? TMath::Sqrt(disp) : -1;  
}

//_____________________________________________________________________________
Double_t SpdKFSimpleRes::GetMomDeltaDivSigma(Int_t i, Int_t axis /*1(px),2(py),3(pz)*/) const
{
    if (axis < 1 || axis > 3) return -1.;
  
    TMatrixDSym c;
    if (!GetCovMom(i,c)) return -1.;
    
    Double_t sigma = -1;
    switch (axis) {
       case 1: sigma = c(0,0); break;
       case 2: sigma = c(1,1); break;
       case 3: sigma = c(2,2); break;
       default : break;
    }
    
    if (sigma < 0) return -1.;
    
    sigma = TMath::Sqrt(sigma);
    if (sigma < 1e-6) return -10.;
    
    if (axis == 1) return GetDPx(i)/sigma;
    if (axis == 2) return GetDPy(i)/sigma; 
    if (axis == 3) return GetDPz(i)/sigma; 
    
    return -1;
}


//_____________________________________________________________________________
Double_t SpdKFSimpleRes::GetPSigma(Int_t i) const
{
    return -1.;
}

//_____________________________________________________________________________
Double_t SpdKFSimpleRes::GetPtSigma(Int_t i) const
{
    return -1.;
}

//_____________________________________________________________________________
Double_t SpdKFSimpleRes::GetPDeltaDivSigma(Int_t i) const
{
    return -1;
}

//_____________________________________________________________________________
Double_t SpdKFSimpleRes::GetPtDeltaDivSigma(Int_t i) const
{
    return -1;
}

//_____________________________________________________________________________
Double_t SpdKFSimpleRes::GetVertexSigma(Int_t axis /*1(x),2(y),3(z)*/) const
{
    if (axis < 1 || axis > 3) return -11;
  
    if (GetIsVertexRECcovEmpty()) return -10;
        
    Double_t disp = -1;
    switch (axis) {
       case 1: disp = fVertexRECcov(0,0); break;
       case 2: disp = fVertexRECcov(1,1); break;
       case 3: disp = fVertexRECcov(2,2); break;
       default : break;
    }

    return (disp > 0) ? TMath::Sqrt(disp) : -1;  
}

//_____________________________________________________________________________
void SpdKFSimpleRes::PrintData(Int_t opt) const
{
    cout << "<SpdKFSimpleRes::PrintData> Event: " << fEventId << "; is fully well fitted: " << IsWellFittedEvent() << "\n\n"; 
    
    cout << "Tracks (vertex/selected/info): " 
         << fNVertexTracks << "/" << fNSelectedTracks << "/" << fTracks.size()  << "\n\n";
    
    printf("|dR|, |R_mc|, |R_rec|:  %10.6f %10.6f %10.6f  [cm] : \n\n",
           GetVertexDMag(),fVertexMC.Mag(),fVertexREC.Mag());
    
    printf("Vertex_mc(x,y,z):       %10.6f %10.6f %10.6f  [cm]\n",fVertexMC.X(),fVertexMC.Y(),fVertexMC.Z());
    printf("Vertex_rec(x,y,z):      %10.6f %10.6f %10.6f  [cm]\n",fVertexREC.X(),fVertexREC.Y(),fVertexREC.Z());
    printf("dR(dx,dy,dz):           %10.6f %10.6f %10.6f  [cm]\n",GetVertexDX(),GetVertexDY(),GetVertexDZ());
    printf("Sigma(x,y,z):           %10.6f %10.6f %10.6f  [cm]\n",GetVertexSigma(1),GetVertexSigma(2),GetVertexSigma(3));
    
    if (GetIsVertexRECcovNotEmpty()) {
        printf("\n");
        printf("Vertex covariation matrix (chi2/ndf = %6.3f): ",GetVertexChi2overNDF());
        fVertexRECcov.Print();
    }
    
    printf("\n"); 
    
    Int_t ntracks, nfitted, nconverged, nnoerrors;
    ntracks = GetNTracks(nfitted,nconverged,nnoerrors);
    
    printf("Tracks (good): %d\n",GetNTracks(true,true,true));
    printf("Tracks (fitted, converged, without errors):     %d/%d/%d \n",nfitted,nconverged,nnoerrors);
    printf("\n");
    
    if (opt == 0) {
        printf("%5s %5s %10s %7s %7s %7s %5s %5s %5s %5s %8s %8s %8s %8s %8s %17s %17s %15s \n\n",
               "N","Track","pdg","NH(V)","NH(TB)","NH(TEC)",
               "Mat.","Fit.","Err.","Cnv.","chi2/ndf",
               "p/m(mc)","p(mc)","p(rec)",
               "|dp/p|","pt [mc/rec]","pz [mc/rec]","theta [mc/rec]");
                
        for (Int_t i(0); i<ntracks; i++) { 
             printf("%5d %5d %10d %7d %7d %7d %5d %5d %5d %5d %8.3f %8.4f %8.4f %8.4f %8.4f  %+8.4f/%-+8.4f %+8.4f/%-+8.4f %7.2f/%-7.2f \n",
                    i,fTracks[i].TrackNum,fTracks[i].TrackPdg,fTracks[i].NVhits,fTracks[i].NTBhits,fTracks[i].NTEChits,
                    fTracks[i].MaterialEffects,fTracks[i].IsFitted,fTracks[i].ErrorFlag,fTracks[i].Convergency,GetChi2toNDF(i),
                    GetGammaBetaMC(i),fTracks[i].MomentumMC.Mag(),fTracks[i].MomentumREC.Mag(),GetMomRes(i),
                    GetMCPt(i),GetRECPt(i),GetMCPz(i),GetRECPz(i),
                    GetMCPtheta(i),GetRECPtheta(i)
                    );
        }        
    }
    else if (opt == 1) {
        printf("%5s %5s %10s %7s %7s %7s %5s %5s %5s %5s %8s %8s %8s %8s %17s %17s %15s %15s\n\n",
               "N","Track","pdg","NH(V)","NH(TB)","NH(TEC)",
               "Mat.","Fit.","Err.","Cnv.",
               "p/m(mc)","p(mc)","p(rec)",
               "|dp/p|","pt [mc/rec]","pz [mc/rec]","theta [mc/rec]","phi [mc/rec]");
                
        for (Int_t i(0); i<ntracks; i++) { 
             printf("%5d %5d %10d %7d %7d %7d %5d %5d %5d %5d %8.4f %8.4f %8.4f %8.4f  %+8.4f/%-+8.4f %+8.4f/%-+8.4f %7.2f/%-7.2f %7.2f/%-7.2f\n",
                    i,fTracks[i].TrackNum,fTracks[i].TrackPdg,fTracks[i].NVhits,fTracks[i].NTBhits,fTracks[i].NTEChits,
                    fTracks[i].MaterialEffects,fTracks[i].IsFitted,fTracks[i].ErrorFlag,fTracks[i].Convergency,
                    GetGammaBetaMC(i),fTracks[i].MomentumMC.Mag(),fTracks[i].MomentumREC.Mag(),GetMomRes(i),
                    GetMCPt(i),GetRECPt(i),GetMCPz(i),GetRECPz(i),
                    GetMCPtheta(i),GetRECPtheta(i),GetMCPphi(i),GetRECPphi(i)
                    );
        }
    }
    else {
        printf("%5s %5s %10s  %7s %7s %7s  %5s %5s %5s %5s  %8s %8s %8s %8s  %17s %17s %17s\n\n",
               "N","Track","pdg","NH(V)","NH(TB)","NH(TEC)",
               "Mat.","Fit.","Err.","Cnv.",
               "p/m(mc)","p(mc)","p(rec)",
               "|dp/p|","p(x) [mc/rec]","p(y) [mc/rec]","p(z) [mc/rec]");
                
        for (Int_t i(0); i<ntracks; i++) { 
             printf("%5d %5d %10d  %7d %7d %7d  %5d %5d %5d %5d  %8.4f %8.4f %8.4f %8.4f   %+8.4f/%-+8.4f %+8.4f/%-+8.4f %+8.4f/%-+8.4f\n",
                    i,fTracks[i].TrackNum,fTracks[i].TrackPdg,fTracks[i].NVhits,fTracks[i].NTBhits,fTracks[i].NTEChits,
                    fTracks[i].MaterialEffects,fTracks[i].IsFitted,fTracks[i].ErrorFlag,fTracks[i].Convergency,
                    GetGammaBetaMC(i),fTracks[i].MomentumMC.Mag(),fTracks[i].MomentumREC.Mag(),GetMomRes(i),
                    fTracks[i].MomentumMC.X(),fTracks[i].MomentumREC.X(),
                    fTracks[i].MomentumMC.Y(),fTracks[i].MomentumREC.Y(),
                    fTracks[i].MomentumMC.Z(),fTracks[i].MomentumREC.Z()
                    );
        }
        
        printf("\n%5s %8s %8s   %8s %8s %8s %8s %8s   %8s %8s %8s %8s %8s   %8s %8s %8s %8s %8s\n\n",
               "N","chi2/ndf","|dp|/s","px(mc)","px(rec)","dpx  ","sigma(x)","|dpx|/sx",
                                       "py(mc)","py(rec)","dpy  ","sigma(y)","|dpy|/sy",
                                       "pz(mc)","pz(rec)","dpz  ","sigma(z)","|dpz|/sz");
             
        for (Int_t i(0); i<ntracks; i++) { 
             printf("%5d %8.3f %8.4f   %8.4f %8.4f %8.4f %8.4f %8.4f   %8.4f %8.4f %8.4f %8.4f %8.4f   %8.4f %8.4f %8.4f %8.4f %8.4f\n",
                    i,GetChi2toNDF(i),GetPDeltaDivSigma(i),
                    GetMCPx(i),GetRECPx(i),GetDPx(i,true),GetMomSigma(i,1),GetMomDeltaDivSigma(i,1),
                    GetMCPy(i),GetRECPy(i),GetDPy(i,true),GetMomSigma(i,2),GetMomDeltaDivSigma(i,2),
                    GetMCPz(i),GetRECPz(i),GetDPz(i,true),GetMomSigma(i,3),GetMomDeltaDivSigma(i,3)
                   ); 
        }
    }    
    
    printf("\n averaged values : \n");
    printf("\n |dp/p| (fitted):                          %8.3f [%%] (%d)",GetMomRes(false,false)*100,GetNTracks(false,false));
    printf("\n |dp/p| (fitted + converged):              %8.3f [%%] (%d)",GetMomRes(false,true)*100,GetNTracks(false,true));
    printf("\n |dp/p| (fitted + converged + noerrors):   %8.3f [%%] (%d)",GetMomRes(true,true)*100,GetNTracks(true,true));
    
    printf("\n");
    
    printf("\n |dpt/pt| (fitted):                        %8.3f [%%] (%d)",GetMomTRes(false,false)*100,GetNTracks(false,false));
    printf("\n |dpt/pt| (fitted + converged):            %8.3f [%%] (%d)",GetMomTRes(false,true)*100,GetNTracks(false,true));
    printf("\n |dpt/pt| (fitted + converged + noerrors): %8.3f [%%] (%d)",GetMomTRes(true,true)*100,GetNTracks(true,true));

    printf("\n\n");
}


