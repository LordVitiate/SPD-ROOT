//input: file name template(s):
//e.g. there are files recoGamma_0.root, recoGamma_1.root, recoGamma_2.root, recoPi0_0.root, recoPi0_1.root -> input would be {"recoGamma_", "recoPi0_"}
//only processed single-photon events (only one photon in the first generation)


#include "TH1.h"
#include "TH2.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TSystem.h"
#include "TString.h"
#include "TFitResult.h"
#include "TGraphErrors.h"
#include "TGeoNavigator.h"
#include "TVector3.h"
#include "TTree.h"
#include "TStyle.h"
#include "TClonesArray.h"
#include "SpdMCParticle.h"
#include "SpdParSet.h"
#include "SpdEcalRCParticle.h"
#include "SpdEcalRCCluster.h"
#include "SpdEcalMCHit.h"
#include "SpdMCDataIterator.h"
#include "SpdSetParSet.h"
#include "TLegend.h"
#include "TFile.h"
#include "CheckCommon.C"

#include <vector>


using namespace std;

Int_t nMaxEvent = 9e9;

TGeoNavigator* Ntor_ = 0; 
TTree* Tree_ = 0;

SpdMCDataIterator* IT = 0;

const SpdSetParSet* Pars_ = 0;

SpdParSet*         EcalTBPars_;
SpdParSet*         EcalTECPars_;

// pointers to current data objects that we'll need
const TClonesArray*   ParticlesMC_     = 0;    
const TClonesArray*   VerticesMC_      = 0;   
const TClonesArray*   VerticesRC_      = 0;  
const TClonesArray*   EcalParticlesRC_ = 0; 
const TClonesArray*   EcalClustersRC_  = 0; 
const TClonesArray*   EcalHitsMC_      = 0; 

const Double_t nPE_1GeV_scint = 10000;
const Double_t nPE_1GeV_total = nPE_1GeV_scint*0.269;


Double_t fBarrelRadius(-1.);
Double_t fECMinDist(-1.);

TString fVolEC = "ECALENDCAP";
TString fVolBarrel = "EcalBasket";
Int_t nsectors = -1;
Int_t fBarrelBasketNModulesZ = -1;
Int_t fBarrelBasketNModulesPhi = -1;
Int_t fEndcapNModulesX = -1;
Int_t fEndcapNModulesY = -1;
Double_t fEndcapSize = -1;
Double_t fBarrelLength = -1;

void    AnalyseEvent(Int_t ev);

Int_t nBadEvents = 0;


Double_t recoEnergyThr = 0.05;


TH2D* h_nRecoPart_nbarrel_nendcap = new TH2D("h_nRecoPart_nbarrel_nendcap", "Number of particles; In barrel; In endcap", 30, 0, 30, 30, 0, 30);
const Int_t effBinsTheta = 100;
const Int_t effBinsPhi = 100;
TH2D* h_MC_photons_thetaphi = new TH2D("h_MC_photons_thetaphi", "Photon hit; cos(theta); Phi", effBinsTheta, -1, 1, effBinsPhi, -TMath::Pi(), TMath::Pi());
TH2D* h_MC_photons_thetaphi_energyWeighted = new TH2D("h_MC_photons_thetaphi_energyWeighted", "Photon hit, weighted with energy; cos(theta); Phi", effBinsTheta, -1, 1, effBinsPhi, -TMath::Pi(), TMath::Pi());
TH2D* h_MC_photons_zIntersectionBarrel_phi = new TH2D("h_MC_photons_zIntersectionBarrel_phi", "Photon hit; Z @ R_barrel; Phi", 100, -200, 200, 100, -TMath::Pi(), TMath::Pi());
TH2D* h_MC_photons_thetaphi_averageNReco = new TH2D("h_MC_photons_thetaphi_averageNReco", "Average number of reconstructed particles (barrel + endcap) for a SINGLE photon event (`fake`); cos(theta); Phi", effBinsTheta, -1, 1, effBinsPhi, -TMath::Pi(), TMath::Pi());
TH2D* sumBarrelEnergy_vsMCthetaPhi = new TH2D("sumBarrelEnergy_vsMCthetaPhi", "Average energy in barrel for a single photon in event (`fake`); cos(theta); Phi", effBinsTheta, -1, 1, effBinsPhi, -TMath::Pi(), TMath::Pi());
TH2D* sumEndcapEnergy_vsMCthetaPhi = new TH2D("sumEndcapEnergy_vsMCthetaPhi", "Average energy in endcap for a single photon in event (`fake`); cos(theta); Phi", effBinsTheta, -1, 1, effBinsPhi, -TMath::Pi(), TMath::Pi());

TH2D* endcap0_xy_cells = new TH2D("endcap0_xy_cells", "Endcap 0: x/y of cells", 200, -200, 200, 200, -200, 200);
TH2D* endcap1_xy_cells = new TH2D("endcap1_xy_cells", "Endcap 1: x/y of cells", 200, -200, 200, 200, -200, 200);


TH2D* h_phidiff_phitrue = new TH2D("h_phidiff_phitrue", "Phi resolution; MC true Phi; Phi*R difference [cm]", 40, -TMath::Pi(), TMath::Pi(), 200, -5, 5);
TH2D* h_phidiff_MCenergy = new TH2D("h_phidiff_MCenergy", "Phi resolution; MC true photon energy [GeV]; Phi*R difference [cm]", 20, 0, 10, 200, -5, 5);
vector<Double_t> allEnergies;
vector<Double_t> allEnergiesEntries;


TH1D* h_phidiff_MCenergy_energyDisc = new TH1D("h_phidiff_MCenergy_energyDisc", "Phi resolution; Phi*R difference [cm]; Entries", 200, -5, 5);
TH1D* h_ereco_MCenergy_energyDisc = new TH1D("h_ereco_MCenergy_energyDisc", "Reconstructed energy; Reconstructed energy [GeV]; Entries", 2000, 0, 12); //ereco, not ediff -> relative resolution!
TH1D* h_zdiff_MCenergy_energyDisc = new TH1D("h_zdiff_MCenergy_energyDisc", "Z resolution; Z difference [cm]; Entries", 200, -5, 5);
vector<TH1D*> h_phidiff_MCenergy_allEnergies;
vector<TH1D*> h_ereco_MCenergy_allEnergies;
vector<TH1D*> h_ereco_MCenergy_allEnergies_PE;
vector<TH1D*> h_zdiff_MCenergy_allEnergies;


TH1D* hEndcapID = new TH1D("hEndcapID", "Endcap IDs of all cells", 5, -2, 3);


TH2D* h_zdiff_ztrue = new TH2D("h_zdiff_ztrue", "Z resolution; MC true Z [cm]; Z difference [cm]", 40, -200, 200, 400, -20, 20);

TH2D* h_costhetadiff_ztrue = new TH2D("h_costhetadiff_ztrue", "CosTheta resolution; MC true CosTheta; CosTheta difference", 40, -1, 1, 400, -0.1, 0.1);
TH2D* h_ediff_etrue = new TH2D("h_ediff_etrue", "E resolution; MC true E [GeV]; E difference [GeV]", 20, 0, 10, 400, -1.0, 1.0);
TH2D* h_ediff_etrue_barrel = new TH2D("h_ediff_etrue_barrel", "E resolution (barrel); MC true E [GeV]; E difference [GeV]", 20, 0, 10, 400, -1.0, 1.0);
TH2D* h_ediff_etrue_endcap = new TH2D("h_ediff_etrue_endcap", "E resolution (endcap); MC true E [GeV]; E difference [GeV]", 20, 0, 10, 400, -1.0, 1.0);
TH1D* h_allpart_z = new TH1D("h_allpart_z", "All reconstructed particles: Z; Z (cm); Entries", 200, -250, 250);

TH1D* h_gamma_misid_energy_den = new TH1D("h_gamma_misid_energy_den", "All events wit 1 cluster; Reco Photon energy [GeV]; Entries", 50, 0, 10);
TH1D* h_gamma_misid_energy_num = new TH1D("h_gamma_misid_energy_num", "Gamma ID efficiency: events with 1 cluster, PID=22; Reco Energy [GeV]; Efficiency", 50, 0, 10);
TH1D* h_gamma_misid_angle_den = new TH1D("h_gamma_misid_angle_den", "All events wit 1 cluster; Reco Angle [rad]; Entries", 50, 0, TMath::Pi());
TH1D* h_gamma_misid_angle_num = new TH1D("h_gamma_misid_angle_num", "Gamma ID efficiency: events with 1 cluster, PID=22; Reco Angle [rad]; Efficiency", 50, 0, TMath::Pi());


TH1D* hAngles = new TH1D("hAngles", "histograms as a container for angles range; Angle [deg]", 7, 0, 70);
TH1D* hEnergies = new TH1D("hEnergies", "histograms as a container for energies range; Energy [GeV]", 20, 0, 10);
Double_t angleMin = hAngles->GetXaxis()->GetXmin();
Double_t angleMax = hAngles->GetXaxis()->GetXmax();
Double_t angleStep = (angleMax - angleMin)/hAngles->GetNbinsX();
Int_t nAngles = hAngles->GetNbinsX();

Double_t energyMin = hEnergies->GetXaxis()->GetXmin();
Double_t energyMax = hEnergies->GetXaxis()->GetXmax();
Double_t energyStep = (energyMax - energyMin)/hEnergies->GetNbinsX();
Int_t nEnergies = hEnergies->GetNbinsX();


vector<vector<TH1D*> > h_ereco_MCenergy_angleBins_allEnergies (nAngles, vector<TH1D*> ());
vector<vector<TH1D*> > h_ereco_MCenergy_angleBins_allEnergies_PE (nAngles, vector<TH1D*> ());



TH3D* h_ediff_angle_energy_barrel_corr;
TH3D* h_zdiff_energy_angles_barrel_corr;
TH3D* h_zdiff_ztrue_energy_barrel_corr;
TH3D* h_phidiff_angle_energy_barrel_corr;
TH3D* h_ediff_phiEff_energy_barrel_corr;
TH3D* h_ediff_angle_energy_endcap_corr;
TH2D* h_xdiff_xtrue;
TH3D* h_xdiff_xtrue_energy_endcap_corr;
TH2D* h_ydiff_ytrue;
TH3D* h_ydiff_ytrue_energy_endcap_corr;
TH3D* h_ediff_ztrue_energy_barrel_corr;

TH2D* h_ereco_ztrue_barrel_corr;
TH3D* h_ereco_ztrue_angle_barrel_corr;
TH2D* h_ereco_phiEff_barrel_corr;

TH1D* h_eff_num_ztrue;
TH1D* h_eff_den_ztrue;

TH2D* h_eff_num_EC_xytrue;
TH2D* h_eff_den_EC_xytrue;

TH2D* h_eff_num_cosTheta_phi;
TH2D* h_eff_den_cosTheta_phi;


//TF1 * fitmeanEnergy = new TF1("fitmean",[](double*x, double *p){
//        Double_t b1 = p[0];
//        Double_t c1 = p[1];
//        Double_t x0 = p[2];
//        if (x[0] < x0) {
//            return b1*x[0] + c1*x[0]*x[0];
//        }   
//        else {
//            return -c1*x0*x0 + x[0]*(b1 + 2*c1*x0);
//        }
//     }, 0.0, 10, 3); 

TF1 * fitmeanEnergy = new TF1("fitmeanEnergy", "[0] + [1]*x", 0.3, 10);
TF1* fitmeanPosition = new TF1("fitmeanPosition", "[0]*x + [1]*x*x*x", -170, 170);
TF1* fitmeanEnergyDep = new TF1("fitmeanEnergyDep", "TMath::Sqrt([0]*[0] + [1]*[1]/x)", 0, 10);
TF1* fitResolution = new TF1("fitResolution", "gaus(0) + [3]", -10, 10); 









void DrawGraphsSlices(vector<TGraphErrors*> grs, TH1D* bins, TString extra) {
//    if (grs.size() != bins->GetNbinsX()) {
//        cout << "DrawGraphsSlices: number of graphs != number of bins!\n";
//        exit(1);
//    }
    
    if (grs.size() == 0) return;

    for (int i = 0; i < grs.size(); ++i) {
        Color_t col = colordict[i % colordict.size()] + (i / colordict.size());
        grs[i]->SetMarkerColor(col);
        grs[i]->SetLineColor(col);
        grs[i]->SetMarkerStyle(34);
        if (i == 0) grs[i]->Draw("AP");
        else grs[i]->Draw("same P");
    }

    TLegend* tl = new TLegend(0.7, 0.7, 0.9, 0.9);
    for (int i = 0; i < grs.size(); ++i) {
        if (i > bins->GetNbinsX()) continue;
        TString entry = to_string(bins->GetXaxis()->GetBinLowEdge(i+1)) + " - " + to_string(bins->GetXaxis()->GetBinUpEdge(i+1));
        tl->AddEntry(grs[i], entry);
    }
    tl->Draw();

    TString picturename = fDirname + "final/" +  grs[0]->GetName() + "_combinedGraphs_" + extra;
    c1->SaveAs(picturename + ".png", "png");
    c1->SaveAs(picturename + ".root", "root");
    
    delete tl;

}


void Fit2DHistArraySlicesFitHyperparameters(vector<TH2D*> hists, TF1* fitf, TH1D* bins, TString paramfit, Double_t xrightfit, TString subdir) {

  vector<vector<pair<Double_t, Double_t> > > fitparams; //pair = value/error, v1: npar, v2: energy/angle
  vector<TGraphErrors*> graphs_sigmas;
  vector<TGraphErrors*> graphs_sigmasRel;
  for (auto h: hists) {
    DrawProfilesOutput res = DrawProfiles(h, fitf, subdir);
    fitparams.push_back(res.fitparams);
    graphs_sigmas.push_back(res.graphSigma);
    graphs_sigmasRel.push_back(res.graphSigmaRel);
  }

  if (fitparams.size() > 0 && fitparams[0].size() > 0) {
//      vector<TH1D*> h_bias_coeffs;
//      for (Int_t i = 0; i < fitparams[0].size(); ++i) {
//        TString histname = TString("h_bias_") + hists[0]->GetName() + "_" + to_string(i);
//        TString histtitle = "Coefficient " + to_string(i) + " of fit of ? bias vs ?; " + bins->GetXaxis()->GetTitle() + "; Coefficient";
//        TH1D* newhist = new TH1D(histname, histtitle, bins->GetNbinsX(), bins->GetXaxis()->GetXmin(), bins->GetXaxis()->GetXmax());
//        h_bias_coeffs.push_back(newhist);
//      }
//
//      for (Int_t i = 0; i < fitparams.size(); ++i) {
//          for (Int_t ip = 0; ip < fitparams[i].size(); ++ip) {
//                h_bias_coeffs[ip]->SetBinContent(i+1, fitparams[i][ip].first);
//                h_bias_coeffs[ip]->SetBinError(i+1, fitparams[i][ip].second);
//          }
//      }
//
//      for (auto h: h_bias_coeffs) {
//          h->Fit(paramfit, "r", "", 0, xrightfit);
//          DrawHist(h, subdir);
//      }

      DrawGraphsSlices(graphs_sigmas, bins, "sigmas");
      DrawGraphsSlices(graphs_sigmasRel, bins, "sigmasRel");
  }
}


struct resFitResult {
    bool valid;
    double mean;
    double meanErr;
    double sigma;
    double sigmaErr;
    double chi2;
    unsigned int ndf;
};

//________________________________________________________________________________
resFitResult Fit1DResolution(TH1D* hist, TString subdir) {

    TString currdir = subdir + "/fits/";

    double rms = hist->GetRMS();
    double mean = hist->GetMean();
    int maxbin = hist->GetMaximumBin();

    fitResolution->SetRange(mean-10*rms, mean+10*rms);
    fitResolution->SetParameters(hist->GetMaximum(), hist->GetXaxis()->GetBinCenter(maxbin), rms/2, hist->Integral(0, 10)/10 + 1);

    TF1* fClone = (TF1*)fitResolution->Clone("fclone");
    fClone->SetLineColor(kMagenta);

    TFitResultPtr res = hist->Fit(fitResolution, "lsrq");
    DrawHist(hist, fClone, currdir);
    
    if (res >= 0) {
        return resFitResult({res->IsValid(), res->Parameter(1), res->Error(1), fabs(res->Parameter(2)), res->Error(2), res->Chi2(), res->Ndf()});
    }
    else {
        return resFitResult({false, 0, 0, 0, 0, 0, 0});
    }

}


//________________________________________________________________________________
TString GetLabel(TString axisName) {

    vector<TString> tok = SplitString(axisName, " "); 
    if (tok.size() == 0) return "";
    if (tok.size() == 1) return tok[0];

    TString ret = "";
    for (int i = 0; i < tok.size() - 1; ++i) {
        ret += tok[i].ReplaceAll("#", "") + "_";
    }

    return ret;
}


//________________________________________________________________________________
void DrawXYSlices(TH2D* hist, TString subdir, TString ztitle) {

    TString labelX = GetLabel(hist->GetXaxis()->GetTitle());
    if (labelX == "") labelX = "X";
     
    TString dirX = subdir + "/" + labelX + "/";

    for (int ix = 1; ix <= hist->GetNbinsX(); ++ix) {
        TString histname = TString(hist->GetName()) + "_" + labelX + "_" + to_string(ix);
        TH1D* currhist = (TH1D*)hist->ProjectionY(histname, ix, ix);
        TString histtitle = TString(hist->GetTitle()) + ", " + hist->GetXaxis()->GetTitle() + " in [" + hist->GetXaxis()->GetBinLowEdge(ix) + ", " + hist->GetXaxis()->GetBinUpEdge(ix) + "]";
        currhist->SetTitle(histtitle);
        currhist->GetXaxis()->SetTitle(hist->GetYaxis()->GetTitle());
        currhist->GetYaxis()->SetTitle(ztitle);
        DrawHist(currhist, dirX);
    }


    TString labelY = GetLabel(hist->GetYaxis()->GetTitle());
    if (labelY == "") labelY = "Y";
     
    TString dirY = subdir + "/" + labelY + "/";

    for (int iy = 1; iy <= hist->GetNbinsY(); ++iy) {
        TString histname = TString(hist->GetName()) + "_" + labelY + "_" + to_string(iy);
        TH1D* currhist = (TH1D*)hist->ProjectionX(histname, iy, iy);
        TString histtitle = TString(hist->GetTitle()) + ", " + hist->GetYaxis()->GetTitle() + " in [" + hist->GetYaxis()->GetBinLowEdge(iy) + ", " + hist->GetYaxis()->GetBinUpEdge(iy) + "]";
        currhist->SetTitle(histtitle);
        currhist->GetXaxis()->SetTitle(hist->GetXaxis()->GetTitle());
        currhist->GetYaxis()->SetTitle(ztitle);
        DrawHist(currhist, dirY);
    }

}


//________________________________________________________________________________
void Draw2DBinProjections(TH3D* hist, TString subdir) {

    TString labelX = GetLabel(hist->GetXaxis()->GetTitle());
    if (labelX == "") labelX = "X";
     
    TString dirX = subdir + "/" + labelX + "_slices_2D/";

    for (int ix = 1; ix <= hist->GetNbinsX(); ++ix) {
        TString histname = TString(hist->GetName()) + "_" + labelX + "_slice2D_" + to_string(ix);
        TString histtitle = TString(hist->GetTitle()) + ", " + hist->GetXaxis()->GetTitle() + " in [" + hist->GetXaxis()->GetBinLowEdge(ix) + ", " + hist->GetXaxis()->GetBinUpEdge(ix) + "]";
        TH2D* currhist = new TH2D(histname, histtitle, hist->GetNbinsY(), hist->GetYaxis()->GetXmin(), hist->GetYaxis()->GetXmax(), hist->GetNbinsZ(), hist->GetZaxis()->GetXmin(), hist->GetZaxis()->GetXmax());
        currhist->GetXaxis()->SetTitle(hist->GetYaxis()->GetTitle());
        currhist->GetYaxis()->SetTitle(hist->GetZaxis()->GetTitle());

        for (int iix = 1; iix <= hist->GetNbinsY(); ++iix) {
            for (int iiy = 1; iiy <= hist->GetNbinsZ(); ++iiy) {
                currhist->SetBinContent(iix, iiy, hist->GetBinContent(ix, iix, iiy)); 
            }
        }
        DrawHist(currhist, dirX);
        delete currhist;
    }


    TString labelY = GetLabel(hist->GetYaxis()->GetTitle());
    if (labelY == "") labelY = "Y";
     
    TString dirY = subdir + "/" + labelX + "_slices_2D/";

    for (int iy = 1; iy <= hist->GetNbinsY(); ++iy) {
        TString histname = TString(hist->GetName()) + "_" + labelY + "_slice2D_" + to_string(iy);
        TString histtitle = TString(hist->GetTitle()) + ", " + hist->GetYaxis()->GetTitle() + " in [" + hist->GetYaxis()->GetBinLowEdge(iy) + ", " + hist->GetYaxis()->GetBinUpEdge(iy) + "]";
        TH2D* currhist = new TH2D(histname, histtitle, hist->GetNbinsX(), hist->GetXaxis()->GetXmin(), hist->GetXaxis()->GetXmax(), hist->GetNbinsZ(), hist->GetZaxis()->GetXmin(), hist->GetZaxis()->GetXmax());
        currhist->GetXaxis()->SetTitle(hist->GetXaxis()->GetTitle());
        currhist->GetYaxis()->SetTitle(hist->GetZaxis()->GetTitle());

        for (int iix = 1; iix <= hist->GetNbinsX(); ++iix) {
            for (int iiy = 1; iiy <= hist->GetNbinsZ(); ++iiy) {
                currhist->SetBinContent(iix, iiy, hist->GetBinContent(iix, iy, iiy)); 
            }
        }
        DrawHist(currhist, dirY);
        delete currhist;
    }

}


//________________________________________________________________________________
void Fit2DSlicesZ(TH3D* hist) {
    
    TString currdir = TString(hist->GetName()) + "/";

    TString histname = TString(hist->GetName()) + "_ZmeanSlices";
    TH2D* histMeanZSlices = new TH2D(histname, histname, 
            hist->GetNbinsX(), hist->GetXaxis()->GetXmin(), hist->GetXaxis()->GetXmax(), 
            hist->GetNbinsY(), hist->GetYaxis()->GetXmin(), hist->GetYaxis()->GetXmax());

    histname = TString(hist->GetName()) + "_ZsigmaSlices";
    TH2D* histSigmaZSlices = new TH2D(histname, histname, 
            hist->GetNbinsX(), hist->GetXaxis()->GetXmin(), hist->GetXaxis()->GetXmax(), 
            hist->GetNbinsY(), hist->GetYaxis()->GetXmin(), hist->GetYaxis()->GetXmax());


    histMeanZSlices->SetTitle(hist->GetTitle());
    histSigmaZSlices->SetTitle(hist->GetTitle());
    histMeanZSlices->GetXaxis()->SetTitle(hist->GetXaxis()->GetTitle());
    histMeanZSlices->GetYaxis()->SetTitle(hist->GetYaxis()->GetTitle());
    histSigmaZSlices->GetXaxis()->SetTitle(hist->GetXaxis()->GetTitle());
    histSigmaZSlices->GetYaxis()->SetTitle(hist->GetYaxis()->GetTitle());


    for (int ix = 1; ix <= hist->GetNbinsX(); ++ix) {
        for (int iy = 1; iy <= hist->GetNbinsY(); ++iy) {
            TString cname = TString(hist->GetName()) + "_projZ_" + to_string(ix) + "_" + to_string(iy);
            TH1D* cproj = (TH1D*)hist->ProjectionZ(cname, ix, ix, iy, iy);
            resFitResult cres = Fit1DResolution(cproj, currdir);
            if (!cres.valid || TMath::IsNaN(cres.mean) || TMath::IsNaN(cres.sigma) || cres.chi2/cres.ndf > 10 || cres.mean < cproj->GetXaxis()->GetXmin() || cres.mean > cproj->GetXaxis()->GetXmax()) continue;
//            cout << "before bin : " << histMeanZSlices->GetBinContent(ix, iy) << " " << histSigmaZSlices->GetBinContent(ix, iy) << endl;
//            cout << "filling: " << ix << " " << iy << " " << cres.mean << " " << cres.sigma << endl;
            histMeanZSlices->SetBinContent(ix, iy, cres.mean);
            histMeanZSlices->SetBinError(ix, iy, cres.meanErr);
            histSigmaZSlices->SetBinContent(ix, iy, cres.sigma);
            histSigmaZSlices->SetBinError(ix, iy, cres.sigmaErr);
//            cout << "after bin : " << histMeanZSlices->GetBinContent(ix, iy) << " " << histSigmaZSlices->GetBinContent(ix, iy) << endl;
//            DrawHist(cproj, TString(hist->GetName()) + "/");
        }
    }

    Draw2DBinProjections(hist, currdir); 

    DrawHist(histMeanZSlices, currdir);
    DrawHist(histSigmaZSlices, currdir);

    DrawXYSlices(histMeanZSlices, currdir, hist->GetZaxis()->GetTitle());
    DrawXYSlices(histSigmaZSlices, currdir, hist->GetZaxis()->GetTitle());

    delete histMeanZSlices;
    delete histSigmaZSlices;

}

//________________________________________________________________________________
void DrawEfficiency(TH2D* num, TH2D* den) {

    TString currdir = TString(num->GetName()) + "/";
    
    DrawHist(num, currdir);
    DrawHist(den, currdir);

    TString histname = TString(num->GetName()) + TString("_ratio");
    TH2D* ratio = (TH2D*)num->Clone(histname);

    ratio->Divide(den);
    DrawHist(ratio, currdir);

}

//________________________________________________________________________________
void DrawEfficiency(TH1D* num, TH1D* den) {

    TString currdir = TString(num->GetName()) + "/";
    
    DrawHist(num, currdir);
    DrawHist(den, currdir);

    TString histname = TString(num->GetName()) + TString("_ratio");
    TH1D* ratio = (TH1D*)num->Clone(histname);

    ratio->Divide(den);
    DrawHist(ratio, currdir);

}

//________________________________________________________________________________
void Fit1DSlicesZ(TH2D* hist, double regleft, double regright) { //left and right borders of the sensible region, used to set scale of Y axis

    TString currdir = TString(hist->GetName()) + "/";

    TString histname = TString(hist->GetName()) + "_YmeanSigmaRel";
    TH1D* histSigmaRelSlices = new TH1D(histname, histname, 
            hist->GetNbinsX(), hist->GetXaxis()->GetXmin(), hist->GetXaxis()->GetXmax());


    histSigmaRelSlices->SetTitle(hist->GetTitle());
    histSigmaRelSlices->GetXaxis()->SetTitle(hist->GetXaxis()->GetTitle());
    histSigmaRelSlices->GetYaxis()->SetTitle(TString(hist->GetYaxis()->GetTitle()) + TString(": relative error"));

    for (int ix = 1; ix <= hist->GetNbinsX(); ++ix) {
        TString cname = TString(hist->GetName()) + "_projZ_" + to_string(ix);
        TH1D* cproj = (TH1D*)hist->ProjectionY(cname, ix, ix);
        resFitResult cres = Fit1DResolution(cproj, currdir);
        if (!cres.valid || TMath::IsNaN(cres.mean) || TMath::IsNaN(cres.sigma) || cres.chi2/cres.ndf > 10 || fabs(cres.sigma) < 1e-9 || cres.mean < cproj->GetXaxis()->GetXmin() || cres.mean > cproj->GetXaxis()->GetXmax()) continue;
//            cout << "before bin : " << histMeanZSlices->GetBinContent(ix, iy) << " " << histSigmaZSlices->GetBinContent(ix, iy) << endl;
//        cout << "filling: " << ix << " " << cres.mean << " " << cres.sigma << endl;
        histSigmaRelSlices->SetBinContent(ix, cres.sigma/cres.mean);
//        histSigmaRelSlices->SetBinError(ix, iy, cres.meanErr);
//            cout << "after bin : " << histMeanZSlices->GetBinContent(ix, iy) << " " << histSigmaZSlices->GetBinContent(ix, iy) << endl;
//            DrawHist(cproj, TString(hist->GetName()) + "/");
    }

    double locmax = -1000000;
    int binregleft = histSigmaRelSlices->GetXaxis()->FindBin(regleft);
    int binregright = histSigmaRelSlices->GetXaxis()->FindBin(regright);
    for (int ix = binregleft; ix <= binregright; ++ix) {
        if (histSigmaRelSlices->GetBinContent(ix) > locmax) {
            locmax = histSigmaRelSlices->GetBinContent(ix);
        }
    }

    histSigmaRelSlices->GetYaxis()->SetRangeUser(0, 1.4*locmax);
    DrawHist(histSigmaRelSlices, currdir);

    DrawHist(hist, currdir);

    delete histSigmaRelSlices;

}

//________________________________________________________________________________
void InitHistograms() {

    if (nsectors == -1) {
        cout << "Trying to initialize histograms, but nsectors is not determined! Exiting" << endl;
        exit(1);
    }
  Int_t nbinsEnergy = 11;
  Int_t nBinsPhiRange = 20;
  Int_t nBinsEdiff = 1000;
  Int_t nBinsZ = 20;
  Int_t nBinsPhiEff = 20;
  Double_t eDiffMin = -10.;
  Double_t eDiffMax = 2.;

  Double_t phiEffLeft = -TMath::Pi()/nsectors;
  Double_t phiEffRight = TMath::Pi()/nsectors;

  //-------- barrel position/energy resolution for photons (corrected)
  h_ediff_angle_energy_barrel_corr = new TH3D("h_ediff_angle_energy_barrel_corr", "Barrel; Energy [GeV]; Angle [deg]; Reco - True energy [GeV]", nbinsEnergy, 0, 10, nAngles, angleMin, angleMax, nBinsEdiff, eDiffMin, eDiffMax);
  h_zdiff_energy_angles_barrel_corr = new TH3D("h_zdiff_energy_angles_barrel_corr", "Barrel; Energy [GeV]; Angle [deg]; Reco - True Z [cm]", nbinsEnergy, 0, 10, 20, 0, TMath::Pi()/2, 400, -40.0, 40.0);
  h_zdiff_ztrue_energy_barrel_corr = new TH3D("h_zdiff_ztrue_energy_barrel_corr", "Barrel; Energy [GeV]; True Z [cm]; Reco - True Z [cm]", nbinsEnergy, 0, 10, nBinsZ, -200, 200, 400, -40.0, 40.0);
  h_ediff_ztrue_energy_barrel_corr = new TH3D("h_ediff_ztrue_energy_barrel_corr", "Barrel; Energy [GeV]; True Z [cm]; Reco - True E [GeV]", nbinsEnergy, 0, 10, nBinsZ, -200, 200, nBinsEdiff, eDiffMin, eDiffMax);
  h_phidiff_angle_energy_barrel_corr = new TH3D("h_phidiff_angle_energy_barrel_corr", "Barrel; Energy [GeV]; Angle [deg]; Reco - True #phi*R [cm]", nbinsEnergy, 0, 10, nAngles, angleMin, angleMax, 400, -40.0, 40.0);
  h_ediff_phiEff_energy_barrel_corr = new TH3D("h_ediff_phiEff_energy_barrel_corr", "Barrel; Energy [GeV]; Effective #phi [deg]; Reco - True energy [GeV]", nbinsEnergy, 0, 10, nBinsPhiEff, phiEffLeft, phiEffRight, nBinsEdiff, eDiffMin, eDiffMax);

  h_ediff_angle_energy_endcap_corr = new TH3D("h_ediff_angle_energy_endcap_corr", "Endcap; Energy [GeV]; Angle [deg]; Reco - True energy [GeV]", nbinsEnergy, 0, 10, nAngles, angleMin, angleMax, nBinsEdiff, eDiffMin, eDiffMax);
  h_xdiff_xtrue = new TH2D("h_xdiff_xtrue", "Endcap; True X [cm]; Reco - True X [cm]", nbinsEnergy, -200, 200, 200, -20, 20);
  h_xdiff_xtrue_energy_endcap_corr = new TH3D("h_xdiff_xtrue_energy_endcap_corr", "Endcap; Energy [GeV]; True X [cm]; Reco - True X [cm]", nbinsEnergy, 0, 10, 20, -200, 200, 200, -20, 20);
  h_ydiff_ytrue = new TH2D("h_ydiff_ytrue", "Endcap; True X [cm]; Reco - True X [cm]", nbinsEnergy, -200, 200, 200, -20, 20);
  h_ydiff_ytrue_energy_endcap_corr = new TH3D("h_ydiff_ytrue_energy_endcap_corr", "Endcap: Energy [GeV]; True Y [cm]; Reco - True Y [cm]", nbinsEnergy, 0, 10, 20, -200, 200, 200, -20, 20);

  h_ereco_ztrue_barrel_corr = new TH2D("h_ereco_ztrue_barrel_corr", "Barrel; True Z [cm]; Reco E [GeV]", 200, -200, 200, 400, 0.0, 12.0);
  h_ereco_ztrue_angle_barrel_corr = new TH3D("h_ereco_ztrue_angle_barrel_corr", "Barrel; True Z [cm]; Angle [deg]; Reco E [GeV]", nBinsZ, -200, 200, nAngles, angleMin, angleMax, 400, 0.0, 12.0);
  h_ereco_phiEff_barrel_corr = new TH2D("h_ereco_phiEff_barrel_corr", "Barrel; Effective #phi [deg]; Reco energy [GeV]", 200, phiEffLeft, phiEffRight, 400, 0.0, 12.0);


  h_eff_num_ztrue = new TH1D("h_eff_num_ztrue", ("Efficiency (E_{reco} > " + to_string(recoEnergyThr*1000) + " MeV); True Z [cm]; Efficiency").c_str(), 200, -200, 200);
  h_eff_den_ztrue = new TH1D("h_eff_den_ztrue", ("Efficiency (E_{reco} > " + to_string(recoEnergyThr*1000) + " MeV); True Z [cm]; Efficiency").c_str(), 200, -200, 200);

  h_eff_num_EC_xytrue = new TH2D("h_eff_num_EC_xytrue", ("Efficiency (E_{reco} > " + to_string(recoEnergyThr*1000) + " MeV); True X [cm]; True Y [cm]").c_str(), 200, -200, 200, 200, -200, 200);
  h_eff_den_EC_xytrue = new TH2D("h_eff_den_EC_xytrue", ("Efficiency (E_{reco} > " + to_string(recoEnergyThr*1000) + " MeV); True X [cm]; True Y [cm]").c_str(), 200, -200, 200, 200, -200, 200);
  
  h_eff_num_cosTheta_phi = new TH2D("h_eff_num_cosTheta_phi", ("Photon efficiency (E_{reco} > " + to_string(recoEnergyThr*1000) + " MeV); cos(#theta); #phi").c_str(), 40, -1, 1, 40, -TMath::Pi(), TMath::Pi());
  h_eff_den_cosTheta_phi = new TH2D("h_eff_den_cosTheta_phi", ("Photon efficiency (E_{reco} > " + to_string(recoEnergyThr*1000) + " MeV); cos(#theta); #phi").c_str(), 40, -1, 1, 40, -TMath::Pi(), TMath::Pi());

}


//________________________________________________________________________________
void GetSetupParametersUsingIT() {

    Pars_ = IT->GetParameters(); 
    EcalTBPars_ = (SpdParSet*)Pars_->FindParameters("EcalTBParSet");
    EcalTECPars_ = (SpdParSet*)Pars_->FindParameters("EcalTECParSet");
    Double_t barrelOuter, barrelWidth;
    Bool_t ok = EcalTBPars_->GetParameter("EcalBarrelOuterSize", barrelOuter);
    ok = EcalTBPars_->GetParameter("EcalBarrelWidth", barrelWidth);
    fBarrelRadius = barrelOuter - barrelWidth;

    EcalTBPars_->printParams();
    ok = EcalTECPars_->GetParameter("EcalTECMinDist1", fECMinDist);
    if (!ok) {
        cout << "parameter ECMindist not found!" << endl;
        exit(1);
    }
    ok = EcalTECPars_->GetParameter("EcalECNModulesX", fEndcapNModulesX);
    if (!ok) {
        cout << "parameter EcalECNModulesX not found!" << endl;
        exit(1);
    }
    ok = EcalTECPars_->GetParameter("EcalECNModulesY", fEndcapNModulesY);
    if (!ok) {
        cout << "parameter EcalECNModulesY not found!" << endl;
        exit(1);
    }
    ok = EcalTECPars_->GetParameter("EcalTECSize", fEndcapSize);
    if (!ok) {
        cout << "parameter EcalTECSize not found!" << endl;
        exit(1);
    }



    ok = EcalTBPars_->GetParameter("EcalBarrelNSectors", nsectors);
    if (!ok) {
        cout << "parameter EcalBarrelNSectors not found!" << endl;
        exit(1);
    }
    ok = EcalTBPars_->GetParameter("EcalBarrelBasketNModulesZ", fBarrelBasketNModulesZ);
    if (!ok) {
        cout << "parameter EcalBarrelBasketNModulesZ not found!" << endl;
        exit(1);
    }
    ok = EcalTBPars_->GetParameter("EcalBarrelBasketNModulesPhi", fBarrelBasketNModulesPhi);
    if (!ok) {
        cout << "parameter EcalBarrelBasketNModulesPhi not found!" << endl;
        exit(1);
    }
    ok = EcalTBPars_->GetParameter("EcalBarrelLength", fBarrelLength);
    if (!ok) {
        cout << "parameter EcalBarrelLength not found!" << endl;
        exit(1);
    }
    

    cout << "nsec: " << nsectors << "\n";
}


//________________________________________________________________________________
void InitIterator(vector<TString> & fnameTemplates) {

    IT = new SpdMCDataIterator();

    for (auto fn: fnameTemplates) {
        vector<TString> files = GetFilesStartingWith(fn);
        cout << "template: " << fn << " files :";
        for (auto f: files) { 
            cout << f << ", ";
        }
        cout << endl;
        for (auto f: files) {
            IT->AddSourceFile(f.Data());
        }
    }

    IT->ActivateBranch("MCParticles");        
    IT->ActivateBranch("MCVertices");        
    IT->ActivateBranch("RCVertices");  
    IT->ActivateBranch("RCEcalParticles"); 
    IT->ActivateBranch("RCEcalClusters"); 
    IT->ActivateBranch("EcalMCHits"); 
    
    //----------------------------------------------------------
    // Iterator initialization
    IT->Init();
    
    //----------------------------------------------------------
    // Get data pointers
    ParticlesMC_     = IT->GetParticles();
    VerticesMC_      = IT->GetVertices();
    VerticesRC_      = IT->GetVerticesRC();     
    EcalParticlesRC_ = IT->GetEcalParticlesRC();
    EcalClustersRC_ = IT->GetEcalClustersRC();
    EcalHitsMC_ =     IT->GetEcalHits();

    
}


//________________________________________________________________________________
void Finalize() {


    fitmeanEnergy->SetParameters(-0.01, 0.05, 1.0);

//    //------------------------- TESTING HISTOGRAMS
//    //number of particles in barrel and endcap
    DrawHist(h_nRecoPart_nbarrel_nendcap);
//    //MC truth: theta/phi of photons (to test the generator)
    DrawHist(h_MC_photons_thetaphi);
//    //same, but instead of theta, Z of intersection with barrel is plotted
    DrawHist(h_MC_photons_zIntersectionBarrel_phi);
//    //number of reconstructed particles vs theta/phi
    h_MC_photons_thetaphi_averageNReco->Divide(h_MC_photons_thetaphi); 
    sumBarrelEnergy_vsMCthetaPhi->Divide(h_MC_photons_thetaphi);
    sumEndcapEnergy_vsMCthetaPhi->Divide(h_MC_photons_thetaphi);
    DrawHist(h_MC_photons_thetaphi_averageNReco, "final/");
//    //average energy in endcap/barrel vs theta/phi
    DrawHist(sumBarrelEnergy_vsMCthetaPhi);
    DrawHist(sumEndcapEnergy_vsMCthetaPhi);
    DrawHist(endcap0_xy_cells);
    DrawHist(endcap1_xy_cells);
    DrawHist(hEndcapID);
    DrawHist(h_allpart_z);

    TGraph* gr_allEnergies_entries = new TGraph(allEnergies.size(), &(allEnergies[0]), &(allEnergiesEntries[0]));
    DrawGraph(gr_allEnergies_entries); 

        //------------------------- PHI RESOLUTIONS (BARREL)
    //---------- vs phi
    //phi: MC true phi vs (reco-simu) phi
    DrawHist(h_phidiff_phitrue, "final/");
    DrawProfiles(h_phidiff_phitrue, fitmeanPosition, "Phi_resolution_avg");
    //---------- vs energy 
    //phi: MC true energy vs (reco-simu) phi
    DrawHist(h_phidiff_MCenergy, "final/");
    //phi: draw resolution vs particle energy (discrete energies)
    DrawResolutionGraph(h_phidiff_MCenergy_allEnergies, allEnergies, TString(h_phidiff_MCenergy_energyDisc->GetName()), "#phi resolution (photons); MC photon energy [GeV]; R*#phi resolution [cm]");
    //same, but binned energies (continuous distribution in energy)
    DrawProfiles(h_phidiff_MCenergy, fitmeanEnergyDep, "Phi_resolution_avg_energy");
    Fit2DSlicesZ(h_phidiff_angle_energy_barrel_corr);


    //------------------------- Z OR THETA RESOLUTIONS (BARREL)
    //---------- vs Z
    //Z: MC true vs recoSimu difference
    DrawHist(h_zdiff_ztrue, "final/");
    //draw profiles in slices of true Z and plot the resolutions
    DrawProfiles(h_zdiff_ztrue, fitmeanPosition, "Z_resolution_avg");
    //difference in cos(theta) vs MC true Z
    DrawHist(h_costhetadiff_ztrue);
    //draw profiles in slices of true Z and plot the resolutions
    DrawProfiles(h_costhetadiff_ztrue, fitmeanPosition, "Cos_theta_resolution_avg");
    Fit2DSlicesZ(h_zdiff_energy_angles_barrel_corr);
    Fit2DSlicesZ(h_zdiff_ztrue_energy_barrel_corr);
    //---------- vs energy 
    DrawResolutionGraph(h_zdiff_MCenergy_allEnergies, allEnergies, TString(h_zdiff_MCenergy_energyDisc->GetName()), "Z resolution (photons); MC photon energy [GeV]; Z resolution [cm]");



    //------------------------- ENERGY RESOLUTIONS
    //---------- vs energy
    //energy: MC true vs recoSimu difference
    DrawHist(h_ediff_etrue, "final/");
    //same, only for barrel
    DrawHist(h_ediff_etrue_barrel, "final/");
    //same, only for endcap
    DrawHist(h_ediff_etrue_endcap, "final/");
    //energy: draw resolution vs particle energy (discrete energies)
    DrawRelativeResolutionGraph(h_ereco_MCenergy_allEnergies, allEnergies, TString(h_ereco_MCenergy_energyDisc->GetName()));
    vector<TGraphErrors*> resolutions_energyDisc;
    for (int ih = 0; ih < h_ereco_MCenergy_angleBins_allEnergies.size(); ++ih) {
        auto h = h_ereco_MCenergy_angleBins_allEnergies[ih];
        auto gr = DrawRelativeResolutionGraph(h, allEnergies, TString(h_ereco_MCenergy_energyDisc->GetName()) + "_angleBin" + to_string(ih));
        if (ih == 0 || ih == 2 || ih == 4) resolutions_energyDisc.push_back(gr);
    }
    DrawManyGraphs(resolutions_energyDisc, "final/", 0., 0.2);


    DrawRelativeResolutionGraph(h_ereco_MCenergy_allEnergies_PE, allEnergies, TString(h_ereco_MCenergy_energyDisc->GetName()) + "_PE");
    vector<TGraphErrors*> resolutions_energyDisc_PE;
    for (int ih = 0; ih < h_ereco_MCenergy_angleBins_allEnergies_PE.size(); ++ih) {
        auto h = h_ereco_MCenergy_angleBins_allEnergies_PE[ih];
        auto gr = DrawRelativeResolutionGraph(h, allEnergies, TString(h_ereco_MCenergy_energyDisc->GetName()) + "_PE_angleBin" + to_string(ih));
        if (ih == 0 || ih == 2 || ih == 4) resolutions_energyDisc_PE.push_back(gr);
    }
    DrawManyGraphs(resolutions_energyDisc_PE, "final/", 0., 0.2);

    //draw profiles in slices of true energy and plot the resolutions
    DrawProfiles(h_ediff_etrue, fitmeanEnergy, "E_resolution_avg");
    //vs angle and energy
    Fit2DSlicesZ(h_ediff_angle_energy_barrel_corr);


    //---------- vs Z
    //get bias parametrization for barrel (energy)
    Fit2DSlicesZ(h_ediff_ztrue_energy_barrel_corr);
    //reconstructed energy vs Z true and angle
    Fit2DSlicesZ(h_ereco_ztrue_angle_barrel_corr);
    //vs Z true
    Fit1DSlicesZ(h_ereco_ztrue_barrel_corr, -100, 100);


    //---------- vs phi
    //energy resolutions in slices of "effective" phi and energy (within basket)
    Fit2DSlicesZ(h_ediff_phiEff_energy_barrel_corr);
    //vs effective phi
    Fit1DSlicesZ(h_ereco_phiEff_barrel_corr, -0.2, 0.2);


    //------------------------- X/Y RESOLUTIONS (ENDCAP)
    DrawHist(h_xdiff_xtrue);
    Fit2DSlicesZ(h_xdiff_xtrue_energy_endcap_corr);
    DrawHist(h_ydiff_ytrue);
    Fit2DSlicesZ(h_ydiff_ytrue_energy_endcap_corr);


    //------------------------- PHOTON DETECTION EFFICIENCIES
    //---------- vs Z
    DrawEfficiency(h_eff_num_ztrue, h_eff_den_ztrue);
    DrawEfficiency(h_eff_num_EC_xytrue, h_eff_den_EC_xytrue);
    DrawEfficiency(h_eff_num_cosTheta_phi, h_eff_den_cosTheta_phi);


    gStyle->SetOptStat(0);
    //------------------------- PHOTON IDENTIFICATION EFFICIENCIES (PI/GAMMA SEPARATION)
    h_gamma_misid_energy_num->Divide(h_gamma_misid_energy_den);
    h_gamma_misid_angle_num->Divide(h_gamma_misid_angle_den);
    h_gamma_misid_energy_num->GetYaxis()->SetRangeUser(0., 1.);
    h_gamma_misid_angle_num->GetYaxis()->SetRangeUser(0., 1.);
    DrawHist(h_gamma_misid_energy_num);
    DrawHist(h_gamma_misid_energy_den);
    DrawHist(h_gamma_misid_angle_num);
    DrawHist(h_gamma_misid_angle_den);



    delete h_nRecoPart_nbarrel_nendcap;
    delete h_MC_photons_thetaphi;
    delete h_MC_photons_thetaphi_energyWeighted;
    delete h_MC_photons_zIntersectionBarrel_phi;
    delete sumBarrelEnergy_vsMCthetaPhi;
    delete sumEndcapEnergy_vsMCthetaPhi;
    delete h_phidiff_phitrue;
    delete h_phidiff_MCenergy;
    delete h_phidiff_MCenergy_energyDisc;
    delete h_ereco_MCenergy_energyDisc;
    delete h_zdiff_MCenergy_energyDisc;
    delete h_zdiff_ztrue;
    delete h_costhetadiff_ztrue;
    delete h_ediff_etrue;
    delete h_ediff_etrue_barrel;
    delete h_ediff_etrue_endcap;
    delete hAngles;
    delete hEnergies;
//    delete h_ediff_angle_energy_barrel_cor;
//    delete h_zdiff_energy_angles_barrel_cor;
//    delete h_zdiff_ztrue_energy_barrel_cor;
//    delete h_phidiff_angle_energy_barrel_cor;
//    delete h_ediff_phiEff_energy_barrel_cor;
//    delete h_ediff_angle_energy_endcap_cor;
//    delete h_xdiff_xtrue_energy_endcap_cor;
//    delete h_ydiff_ytrue_energy_endcap_cor;
//    delete h_ediff_ztrue_energy_barrel_cor;
//    delete h_ereco_ztrue_barrel_cor;
//    delete h_ereco_ztrue_angle_barrel_cor;
//    delete h_ereco_phiEff_barrel_cor;
//    delete h_eff_num_ztru;
//    delete h_eff_den_ztru;
//    delete h_eff_num_cosTheta_ph;
//    delete h_eff_den_cosTheta_ph;
    delete fitmeanEnergy;
    delete fitmeanPosition;
    delete fitmeanEnergyDep;
    delete fitResolution;

    for (auto h: h_phidiff_MCenergy_allEnergies) {
        delete h;
    }
    for (auto h: h_ereco_MCenergy_allEnergies) {
        delete h;
    }
    for (auto h: h_zdiff_MCenergy_allEnergies) {
        delete h;
    }
    delete IT;

}

//________________________________________________________________________________
void CheckReconstruction(vector<TString> recoFileTemplate = {"reco.root"}, TString pictureDir = "CheckReconstruction/") {

  c1 = new TCanvas("c1", "c1", 1200, 800);

  fDirname = pictureDir;
  gSystem->Exec("mkdir -p " + fDirname);
  

    InitIterator(recoFileTemplate);
    GetSetupParametersUsingIT();
    InitHistograms();

    Int_t ev = 0;
    while (IT->NextEvent()) 
    {
        if (ev % 1000 == 0) cout << "event: " << ev << "\n";
        AnalyseEvent(ev);
        ev += 1;
        if (ev == nMaxEvent) break;
    }

    if (double(nBadEvents)/ev > 0.5) {
        cout << "From " << ev << " events, " << nBadEvents << " were bad!" << endl;
        cout << "Bad event usually means the primary particle is not a photon" << endl;
        exit(1);
    }

    cout << ev << " events processed\n\n\n\n\n";

    Finalize();

}


enum ECALpart {kBarrel, kEndcap};
struct recoInfo {
    TVector3 mom;
    ECALpart part;
    Int_t ncells;
    Int_t pid;
};


TVector3 momSimuGamma;
TVector3 vtx;
Int_t npartMC;
Int_t npartRC;
vector<recoInfo> momRecoAll;
Int_t npartB;
Int_t npartEC;
Double_t eSumBarrel;
Double_t eSumEndcaps;
Int_t irecoGammaMaxEnergy;
Double_t momRecoMax;
Double_t simuEnergy;
Double_t zsimu;
Double_t xsimu;
Double_t ysimu;
Double_t costhetasimu;
Double_t phisimu;




//________________________________________________________________________________
void ResetPerEventVariables() {

    momRecoAll.clear();
    npartMC = 0;
    npartRC = 0;
    npartB = 0;
    npartEC = 0;
    eSumBarrel = 0.;
    eSumEndcaps = 0.;
    irecoGammaMaxEnergy = -1;
    momRecoMax = -1.;
    simuEnergy = -1.;
    xsimu = -999.;
    ysimu = -999.;

}

//________________________________________________________________________________
Bool_t GetMCInfo() {

    npartMC = ParticlesMC_->GetEntriesFast();
    vtx = TVector3(0.,0.,0.); 
    Bool_t gammaFilled = false;

    for (int it = 0; it < npartMC; ++it) {
        SpdMCParticle* part = (SpdMCParticle*)ParticlesMC_->At(it);
        if (part->GetPdgCode() == 22 && part->GetGeneration() == 1) {
            if (gammaFilled) {
                cout << "Info: more than one photon in generation 1!" << endl;
                return false;
            }
            else {
                momSimuGamma = part->GetStartMom();
                vtx = part->GetStartPos();
                zsimu = GetZBarrel(momSimuGamma, vtx, fBarrelRadius);
                costhetasimu = momSimuGamma.CosTheta();
                phisimu = momSimuGamma.Phi();

                auto xysimu = GetXYEndcap(momSimuGamma, vtx, fECMinDist);
                xsimu = xysimu.first;
                ysimu = xysimu.second;
                gammaFilled = true;
            }
        }
    } 
    
    if (!gammaFilled) {
        cout << "Info: no photons in generation 1 were found!" << endl;
        return false;
    }

    simuEnergy = momSimuGamma.Mag();
    
    return true;

}

//________________________________________________________________________________
void FillMCPerEventVariables() {

    h_MC_photons_thetaphi_averageNReco->Fill(momSimuGamma.CosTheta(), momSimuGamma.Phi(), npartRC);
    h_MC_photons_thetaphi->Fill(momSimuGamma.CosTheta(), momSimuGamma.Phi());
    h_MC_photons_thetaphi_energyWeighted->Fill(momSimuGamma.CosTheta(), momSimuGamma.Phi(), simuEnergy);
    h_MC_photons_zIntersectionBarrel_phi->Fill(GetZBarrel(momSimuGamma, vtx, fBarrelRadius), momSimuGamma.Phi());

    h_eff_den_ztrue->Fill(zsimu);
    h_eff_den_cosTheta_phi->Fill(costhetasimu, phisimu);
    h_eff_den_EC_xytrue->Fill(xsimu, ysimu);
}


//_____________________________________________________________________________
Int_t GetCellZInBarrel(Int_t basketID, Int_t moduleZID, Int_t cellID)  
{
    Int_t nBaskets = 2;
    Int_t nModulesBefore = ((basketID - 1) % nBaskets)*fBarrelBasketNModulesZ + (moduleZID - 1); 
    return 2*nModulesBefore + ((cellID-1) % 2) + 1;
}

//_____________________________________________________________________________
Int_t GetCellPhiInBarrel(Int_t basketID, Int_t modulePhiID, Int_t cellID)  
{
    Int_t nBaskets = 2;
    Int_t nModulesBefore = ((basketID - 1) / nBaskets)*fBarrelBasketNModulesPhi + (modulePhiID - 1); 
    return 2*nModulesBefore + ((cellID-1) / 2) + 1;
}


//________________________________________________________________________________
void DrawEventCells(Int_t ev) {

    if (fBarrelBasketNModulesZ == -1) {
        cout << "number of modules in basket not found!" << endl;
        exit(1);
    }

    TString histname = "hcellsEdep_" + to_string(ev) + "_endcap1";
    TH2D* hcellsEdepEndcap1 = new TH2D(histname, "Energy deposition in cells, endcap1", 2*fEndcapNModulesX, -fEndcapSize/2, fEndcapSize/2, 2*fEndcapNModulesY, -fEndcapSize/2, fEndcapSize/2);
    histname = "hcellsEdep_" + to_string(ev) + "_endcap2";
    TH2D* hcellsEdepEndcap2 = new TH2D(histname, "Energy deposition in cells, endcap2", 2*fEndcapNModulesX, -fEndcapSize/2, fEndcapSize/2, 2*fEndcapNModulesY, -fEndcapSize/2, fEndcapSize/2);
    histname = "hcellsEdep_" + to_string(ev) + "_barrel";
    Double_t anglePerCell = TMath::Pi()/(fBarrelBasketNModulesPhi*nsectors);
    Double_t phiCell0 = TMath::Pi()*(1./2 - 1./nsectors);
    Double_t phiOffsetCell0 = phiCell0 - TMath::Pi();
    Int_t nCellsOffset = phiOffsetCell0/anglePerCell;
    TH2D* hcellsEdepBarrel = new TH2D(histname, "Energy deposition in cells, barrel", 4*fBarrelBasketNModulesZ, -fBarrelLength/2, fBarrelLength/2, 16*nsectors, -TMath::Pi(), TMath::Pi());


   for (Int_t ic = 0; ic < EcalHitsMC_->GetEntriesFast(); ++ic) {
        SpdEcalMCHit* hit = (SpdEcalMCHit*)EcalHitsMC_->At(ic);

        Double_t e = hit->GetEnergy();

        if (hit->GetEndcapID() == -1) {

            Int_t basket = hit->GetBasketID();
            Int_t modz = hit->GetModuleZID();
            Int_t modphi = hit->GetModulePhiID();
            Int_t cell = hit->GetCellID();
            Int_t icellZ = GetCellZInBarrel(basket, modz, cell);
            Int_t icellPhi = GetCellPhiInBarrel(basket, modphi, cell);
            Int_t ibinPhi = nCellsOffset + icellPhi;
            if (ibinPhi > hcellsEdepBarrel->GetNbinsY()) ibinPhi -= hcellsEdepBarrel->GetNbinsY();

            hcellsEdepBarrel->SetBinContent(icellZ, ibinPhi, e);
       }

        else {

            Int_t ec = hit->GetEndcapID();
            Int_t mod = hit->GetModuleID();
            Int_t modx = mod % 1000;
            Int_t mody = mod/1000;
            Int_t cell = hit->GetCellID();
            Int_t cellx = (modx-1)*2 + (cell == 2 || cell == 4) + 1;
            Int_t celly = (mody-1)*2 + (cell == 3 || cell == 4) + 1;

            if (ec == 0) hcellsEdepEndcap1->Fill(cellx, celly, e);
            else hcellsEdepEndcap2->Fill(cellx, celly, e);
       }

   }

   c1->Clear();
   c1->Divide(3);
   c1->cd(1);
   hcellsEdepEndcap1->Draw("colz");
   c1->cd(2);
   hcellsEdepBarrel->Draw("colz");
   c1->cd(3);
   hcellsEdepEndcap2->Draw("colz");
    

    gSystem->Exec("mkdir -p " + fDirname + "/eventDraw/");
    TString picturename = fDirname + "/eventDraw/eventCells_" + to_string(ev);
    c1->SaveAs(picturename + ".png", "png");
    c1->SaveAs(picturename + ".root", "root");

    delete hcellsEdepEndcap1;
    delete hcellsEdepEndcap2;
    delete hcellsEdepBarrel;

}


//________________________________________________________________________________
void GetRCInfo() {

    npartRC = EcalParticlesRC_->GetEntriesFast();

    for (Int_t ip = 0; ip < npartRC; ++ip) {
        SpdEcalRCParticle* part = (SpdEcalRCParticle*)EcalParticlesRC_->At(ip);

        Int_t cid = part->GetClusterId();
        SpdEcalRCCluster* cl = (SpdEcalRCCluster*)EcalClustersRC_->At(cid);
        const vector<Int_t> & cells = cl->GetCells();
        Int_t ncells = cells.size();

        TVector3 momReco = part->GetPosition();
        Double_t en1 = part->GetEnergy();
        momReco.SetMag(en1);
        Int_t pid = part->GetPID();
        if (fabs(part->GetPosition().Z()) > fECMinDist-1.0) {
            momRecoAll.push_back(recoInfo({momReco, kEndcap, ncells, pid}));
            npartEC += 1;
        }
        else {
            momRecoAll.push_back(recoInfo({momReco, kBarrel, ncells, pid}));
            npartB += 1;
        }
    }

}

//________________________________________________________________________________
void AnalyzePerEventRCVariables() {


    for (Int_t ireco = 0; ireco < momRecoAll.size(); ++ireco) {
        auto p1 = momRecoAll[ireco];
        if (p1.part == kBarrel) {
            eSumBarrel += p1.mom.Mag();
        }
        else {
            eSumEndcaps += p1.mom.Mag();
        }
        if (momRecoAll[ireco].mom.Mag() > momRecoMax) {
            momRecoMax = momRecoAll[ireco].mom.Mag();
            irecoGammaMaxEnergy = ireco;
        }
    }
    for (Int_t ip = 0; ip < npartRC; ++ip) {
        SpdEcalRCParticle* part = (SpdEcalRCParticle*)EcalParticlesRC_->At(ip);
        TVector3 pos = part->GetPosition();
        h_allpart_z->Fill(pos.Z());
    }

}

//________________________________________________________________________________
void FillRCPerEventVariables() { 
    h_nRecoPart_nbarrel_nendcap->Fill(npartB, npartEC);
    sumBarrelEnergy_vsMCthetaPhi->Fill(momSimuGamma.CosTheta(), momSimuGamma.Phi(), eSumBarrel);
    sumEndcapEnergy_vsMCthetaPhi->Fill(momSimuGamma.CosTheta(), momSimuGamma.Phi(), eSumEndcaps);
    h_MC_photons_thetaphi_averageNReco->Fill(momSimuGamma.CosTheta(), momSimuGamma.Phi(), npartRC);

    if (momRecoAll.size() == 1) {
        Double_t e = momRecoAll[0].mom.Mag();
        Double_t a = momRecoAll[0].mom.Theta();
        h_gamma_misid_energy_den->Fill(e);
        h_gamma_misid_angle_den->Fill(a);
        if (momRecoAll[0].pid == 22) {
            h_gamma_misid_energy_num->Fill(e);
            h_gamma_misid_angle_num->Fill(a);
        }
    }

}

//________________________________________________________________________________
void FillResolutionVariables() {

    Int_t nRecoBarrelThr = 0;
    Bool_t filledEff = false;
    
    for (Int_t ipr = 0; ipr < momRecoAll.size(); ++ipr) {
        if (ipr != irecoGammaMaxEnergy) continue;
        auto pr = momRecoAll[ipr];

        TVector3 momReco = pr.mom;

        Double_t phidiff = momSimuGamma.Phi() - momReco.Phi(); 
        if (phidiff > TMath::Pi()) phidiff -= 2*TMath::Pi();
        if (phidiff < -TMath::Pi()) phidiff += 2*TMath::Pi();
        Double_t rphidiff = fBarrelRadius*phidiff;

        Double_t zreco = GetZBarrel(momReco, vtx, fBarrelRadius);
        Double_t zdiff = zreco - zsimu;

        Double_t ediff = momReco.Mag() - simuEnergy;

        Double_t currangle = -1;

        if (pr.part == kBarrel && momReco.Mag() > recoEnergyThr) {
            nRecoBarrelThr += 1;
            Double_t angle = fabs(TMath::Pi()/2 - momSimuGamma.Theta())*TMath::RadToDeg();
            currangle = angle;

            Int_t ia = int(floor((angle - angleMin)/angleStep));
            h_ediff_angle_energy_barrel_corr->Fill(simuEnergy, angle, momReco.Mag() - simuEnergy);

            h_zdiff_energy_angles_barrel_corr->Fill(simuEnergy, angle, zreco - zsimu);
            h_phidiff_angle_energy_barrel_corr->Fill(simuEnergy, angle, rphidiff);

            h_zdiff_ztrue_energy_barrel_corr->Fill(simuEnergy, zsimu, zreco - zsimu);
            h_ediff_ztrue_energy_barrel_corr->Fill(simuEnergy, zsimu, momReco.Mag() - simuEnergy);
            h_ereco_ztrue_barrel_corr->Fill(zsimu, momReco.Mag());
            h_ereco_ztrue_angle_barrel_corr->Fill(zsimu, angle, momReco.Mag());
            h_ereco_phiEff_barrel_corr->Fill(GetPhiEff(momSimuGamma.Phi(), nsectors), momReco.Mag());
            h_ediff_phiEff_energy_barrel_corr->Fill(simuEnergy, GetPhiEff(momSimuGamma.Phi(), nsectors), momReco.Mag() - simuEnergy);
            
            if (!filledEff) {
                filledEff = true;
                h_eff_num_ztrue->Fill(zsimu);
                h_eff_num_cosTheta_phi->Fill(costhetasimu, phisimu);
            }
        }

        if (pr.part == kEndcap && momReco.Mag() > recoEnergyThr) {

            auto xyreco = GetXYEndcap(momReco, vtx, fECMinDist);
            Double_t currE = simuEnergy;
            Double_t xreco = xyreco.first;
            Double_t yreco = xyreco.second;

            Double_t angle = fabs(momSimuGamma.Theta())*TMath::RadToDeg();
            currangle = angle;
            h_ediff_angle_energy_endcap_corr->Fill(currE, angle, momReco.Mag() - simuEnergy);
            h_xdiff_xtrue->Fill(xsimu, xreco - xsimu);
            h_xdiff_xtrue_energy_endcap_corr->Fill(currE, xsimu, xreco - xsimu);
            h_ydiff_ytrue->Fill(ysimu, yreco - ysimu);
            h_ydiff_ytrue_energy_endcap_corr->Fill(currE, ysimu, yreco - ysimu);
            
            if (!filledEff) {
                filledEff = true;
                h_eff_num_ztrue->Fill(zsimu);
                h_eff_num_cosTheta_phi->Fill(costhetasimu, phisimu);
                h_eff_num_EC_xytrue->Fill(xsimu, ysimu);
            }
        }

        h_phidiff_phitrue->Fill(momSimuGamma.Phi(), rphidiff);
        h_phidiff_MCenergy->Fill(simuEnergy, rphidiff);

        Int_t currie = -1;
        for (int ie = 0; ie < allEnergies.size(); ++ie) {
            if (fabs(allEnergies[ie] - simuEnergy) < 1e-6) {
                currie = ie;
                break;
            }
        }

        if (currie == -1) {
            if (h_phidiff_MCenergy_allEnergies.size() <= 30) {
                currie = h_phidiff_MCenergy_allEnergies.size();
                allEnergies.push_back(simuEnergy);
                allEnergiesEntries.push_back(0.);

                TString histname = TString(h_phidiff_MCenergy_energyDisc->GetName()) + "_" + to_string(currie);
                TH1D* currhist = (TH1D*)h_phidiff_MCenergy_energyDisc->Clone(histname);
                currhist->Reset();
                h_phidiff_MCenergy_allEnergies.push_back(currhist);

                histname = TString(h_ereco_MCenergy_energyDisc->GetName()) + "_" + to_string(currie);
                currhist = (TH1D*)h_ereco_MCenergy_energyDisc->Clone(histname);
                currhist->Reset();
                h_ereco_MCenergy_allEnergies.push_back(currhist);

                histname = TString(h_ereco_MCenergy_energyDisc->GetName()) + "_PE_" + to_string(currie);
                currhist = (TH1D*)h_ereco_MCenergy_energyDisc->Clone(histname);
                currhist->Reset();
                h_ereco_MCenergy_allEnergies_PE.push_back(currhist);


                for (int iangle = 0; iangle < nAngles; ++iangle) {
                    histname = TString(h_ereco_MCenergy_energyDisc->GetName()) + "_angle" + to_string(iangle) + "_energy" + to_string(currie);
                    currhist = (TH1D*)h_ereco_MCenergy_energyDisc->Clone(histname);
                    Double_t minangle = angleMin + iangle*angleStep;
                    Double_t maxangle = angleMin + (iangle+1)*angleStep;
                    TString title = "Energy resolution, angle: " + to_string(minangle) + " < angle < " + to_string(maxangle);
                    currhist->SetTitle(title);
                    currhist->Reset();
                    h_ereco_MCenergy_angleBins_allEnergies[iangle].push_back(currhist);
                }


                for (int iangle = 0; iangle < nAngles; ++iangle) {
                    histname = TString(h_ereco_MCenergy_energyDisc->GetName()) + "_PE_angle" + to_string(iangle) + "_energy" + to_string(currie);
                    currhist = (TH1D*)h_ereco_MCenergy_energyDisc->Clone(histname);
                    Double_t minangle = angleMin + iangle*angleStep;
                    Double_t maxangle = angleMin + (iangle+1)*angleStep;
                    TString title = "Energy resolution with PE statistics, angle: " + to_string(minangle) + " < angle < " + to_string(maxangle);
                    currhist->SetTitle(title);
                    currhist->Reset();
                    h_ereco_MCenergy_angleBins_allEnergies_PE[iangle].push_back(currhist);
                }


                histname = TString(h_zdiff_MCenergy_energyDisc->GetName()) + "_" + to_string(currie);
                currhist = (TH1D*)h_zdiff_MCenergy_energyDisc->Clone(histname);
                currhist->Reset();
                h_zdiff_MCenergy_allEnergies.push_back(currhist);
            }
        }
        if (currie != -1) {
            allEnergiesEntries[currie] += 1.;
            h_zdiff_MCenergy_allEnergies[currie]->Fill(zdiff);
            h_phidiff_MCenergy_allEnergies[currie]->Fill(rphidiff);
            Double_t ereco_PE = momReco.Mag()*(1+gRandom->Gaus(0, 1./sqrt(double(nPE_1GeV_total*momReco.Mag()))));

            h_ereco_MCenergy_allEnergies[currie]->Fill(momReco.Mag());
            if (currangle > 0) {
                Int_t angleBin = int(floor((currangle - angleMin)/angleStep));
                if (angleBin >= 0 && angleBin < nAngles) {
                    h_ereco_MCenergy_angleBins_allEnergies[angleBin][currie]->Fill(momReco.Mag());
                }
            }

            h_ereco_MCenergy_allEnergies_PE[currie]->Fill(ereco_PE);
            if (currangle > 0) {
                Int_t angleBin = int(floor((currangle - angleMin)/angleStep));
                if (angleBin >= 0 && angleBin < nAngles) {
                    h_ereco_MCenergy_angleBins_allEnergies_PE[angleBin][currie]->Fill(ereco_PE);
                }
            }
        }


        h_zdiff_ztrue->Fill(zsimu, zreco - zsimu);
        h_ediff_etrue->Fill(simuEnergy, momReco.Mag() - simuEnergy);
        if (pr.part == kBarrel) h_ediff_etrue_barrel->Fill(simuEnergy, momReco.Mag() - simuEnergy);
        else h_ediff_etrue_endcap->Fill(simuEnergy, momReco.Mag() - simuEnergy);
        h_costhetadiff_ztrue->Fill(momSimuGamma.CosTheta(), momReco.CosTheta() - momSimuGamma.CosTheta());
    }

}
//________________________________________________________________________________
void FillMCHitInfo() {

   for (Int_t ic = 0; ic < EcalHitsMC_->GetEntriesFast(); ++ic) {
        SpdEcalMCHit* hit = (SpdEcalMCHit*)EcalHitsMC_->At(ic);
        hEndcapID->Fill(hit->GetEndcapID());

        TVector3 pos = hit->GetPosition();
        if (hit->GetEndcapID() == 0) endcap0_xy_cells->Fill(pos.X(), pos.Y());
        if (hit->GetEndcapID() == 1) endcap1_xy_cells->Fill(pos.X(), pos.Y());

   }
}

//________________________________________________________________________________
void AnalyseEvent(Int_t ev)
{


    ResetPerEventVariables();
    if (!GetMCInfo()) {
        nBadEvents += 1;
        return;
    }
    FillMCPerEventVariables();
    FillMCHitInfo();
    
    GetRCInfo();
//    if (ev % 100 < 10 && momSimuGamma.Mag() > 8.) {
//    if (ev % 100 == 1) {
//        DrawEventCells(ev);
//    }
    AnalyzePerEventRCVariables();
    FillRCPerEventVariables();

    FillResolutionVariables();


}
















