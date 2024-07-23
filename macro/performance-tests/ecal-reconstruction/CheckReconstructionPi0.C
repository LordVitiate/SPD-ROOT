
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

const Double_t MassPi0 = 0.134977;

TGeoNavigator* Ntor_ = 0; 
TTree* Tree_ = 0;

SpdMCDataIterator* IT = 0;

const SpdSetParSet* Pars_ = 0;

SpdParSet*         EcalTBPars_;
SpdParSet*         EcalTECPars_;
Double_t barrelRadius;
Double_t ECMindist;

// pointers to current data objects that we'll need
const TClonesArray*   ParticlesMC_     = 0;    
const TClonesArray*   VerticesMC_      = 0;   
const TClonesArray*   VerticesRC_      = 0;  
const TClonesArray*   EcalParticlesRC_ = 0; 
const TClonesArray*   EcalClustersRC_  = 0; 
const TClonesArray*   EcalHitsMC_      = 0; 



Double_t fBarrelRadius(-1.);
Double_t fECMinDist(-1.);

TString fVolEC = "ECALENDCAP";
TString fVolBarrel = "EcalBasket";

void    AnalyseEvent();

TH2D* h_esum_barrel_endcaps;
TH2D* h_mgg_doublebarrel_energy_mgg;
TH2D* h_mgg_doubleendcap_energy_mgg;
TH1D* h_mgg_doublebarrel;
TH1D* h_mgg_doubleendcap;
TH1D* h_mgg_doublebarrel_400mevMin;
TH1D* h_mgg_doubleendcap_400mevMin;
TH1D* h_mgg_doublebarrel_100mevMin;
TH1D* h_mgg_doubleendcap_100mevMin;
TH1D* h_mgg_doubleendcap_100mevMin_innerEndcap;
TH2D* h_nclus_barrel_endcap;
TH2D* h_nclus_barrel_endcap_100mevMin;
TH2D* h_nclus_barrel_endcap_400mevMin;
TH2D* h_nclusSum_100mev_avg_cosTheta_phiSimu;
TH2D* h_prob1cl_cosTheta_esimu;
TH2D* h_prob1cl_100MeV_cosTheta_esimu;
TH2D* h_simuTruecosTheta_phiSimu;
TH2D* h_simuTruecosTheta_energySimu;

TH1D* h_pi0_misid_energy_den = new TH1D("h_pi0_misid_energy_den", "All events with 1 or 2 cluster; Simu Pi0 energy [GeV]; Entries", 50, 0, 10);
TH1D* h_pi0_misid_energy_num = new TH1D("h_pi0_misid_energy_num", "Pi0 ID efficiency: events with 1 or 2 cluster, PID=111 where 1 cluster; Simu Energy [GeV]; Efficiency", 50, 0, 10);
TH1D* h_pi0_misid_angle_den = new TH1D("h_pi0_misid_angle_den", "All events wit 1 or 2 cluster; Simu Angle [rad]; Entries", 50, 0, TMath::Pi());
TH1D* h_pi0_misid_angle_num = new TH1D("h_pi0_misid_angle_num", "Pi0 ID efficiency: events with 1 or 2 cluster, PID=111 where 1 cluster; Simu Angle [rad]; Efficiency", 50, 0, TMath::Pi());

TH1D* h_pi0_2cl_misid_energy_den = new TH1D("h_pi0_2cl_misid_energy_den", "All events with 2 cluster; Simu Pi0 energy [GeV]; Entries", 50, 0, 10);
TH1D* h_pi0_2cl_misid_energy_num = new TH1D("h_pi0_2cl_misid_energy_num", "Pi0 ID efficiency: events with 2 cluster; Simu Energy [GeV]; Efficiency", 50, 0, 10);
TH1D* h_pi0_2cl_misid_angle_den = new TH1D("h_pi0_2cl_misid_angle_den", "All events wit 2 cluster; Simu Angle [rad]; Entries", 50, 0, TMath::Pi());
TH1D* h_pi0_2cl_misid_angle_num = new TH1D("h_pi0_2cl_misid_angle_num", "Pi0 ID efficiency: events with 2 cluster; Simu Angle [rad]; Efficiency", 50, 0, TMath::Pi());


Int_t nsectors = -1;

TAxis* axis_pt_bins;
TAxis* axis_phi_bins;
TAxis* axis_phieff_bins;

vector<vector<TH1D*> > h_mgg_db_bins_pt_phi;
vector<vector<TH1D*> > h_mgg_db_bins_pt_phieff;

TH2D* h_npi0_doublebarrel_pt_phi;
TH2D* h_npi0_doublebarrel_pt_phieff;
TH2D* h_nsimu_doublebarrel_pt_phieff;


hist1DEfficiency heff_statloss_vs_pt_35mm;
//hist1DEfficiency heff_statloss_vs_pt_115mm("heff_statloss_vs_pt_115mm");
//hist1DEfficiency heff_pi0reconstruction_vs_angleGammas("heff_pi0reconstruction_vs_angleGammas");
hist1DPi0Efficiency heff_pi0reconstruction_vs_thetaPi0;
//hist2DEfficiency heff_pi0reconstruction_vs_angleGammas_epi0("heff_pi0reconstruction_vs_angleGammas_epi0");



enum ECALpart {kBarrel, kEndcap};
struct recoInfo {
    TVector3 mom;
    ECALpart part;
    Int_t ncells;
    Int_t pid;
};

//________________________________________________________________________________
void InitHistograms() {


    axis_pt_bins = new TAxis(20, 0, 10);
    axis_phi_bins = new TAxis(20, -TMath::Pi(), TMath::Pi());
    axis_phieff_bins = new TAxis(20, -TMath::Pi()/nsectors, TMath::Pi()/nsectors);


    h_esum_barrel_endcaps = new TH2D("h_esum_barrel_endcaps", "sum of energies in ECAL barrel vs endcap; Barrel [GeV]; Endcaps [GeV]", 100, 0, 10, 100, 0, 10);

    h_mgg_doublebarrel_energy_mgg = new TH2D("h_mgg_doublebarrel_energy_mgg", "#gamma #gamma reconstructed mass, double barrel; Energy sum [GeV]; Mass [GeV]", 100, 0, 10, 200, 0, 1.0);
    h_mgg_doubleendcap_energy_mgg = new TH2D("h_mgg_doubleendcap_energy_mgg", "#gamma #gamma reconstructed mass, double endcap; Energy sum [GeV]; Mass [GeV]", 100, 0, 10, 200, 0, 1.0);

    h_mgg_doublebarrel = new TH1D("h_mgg_doublebarrel", "#gamma #gamma reconstructed mass, double barrel; Mass [GeV]; Entries", 200, 0, 1.0);
    h_mgg_doubleendcap = new TH1D("h_mgg_doubleendcap", "#gamma #gamma reconstructed mass, double endcap; Mass [GeV]; Entries", 200, 0, 1.0);

    h_mgg_doublebarrel_100mevMin = new TH1D("h_mgg_doublebarrel_100mevMin", "#gamma #gamma reconstructed mass, double barrel, both photons > 100 MeV; Mass [GeV]; Entries", 200, 0, 1.0);
    h_mgg_doubleendcap_100mevMin = new TH1D("h_mgg_doubleendcap_100mevMin", "#gamma #gamma reconstructed mass, double endcap, both photons > 100 MeV; Mass [GeV]; Entries", 200, 0, 1.0);
    h_mgg_doubleendcap_100mevMin_innerEndcap = new TH1D("h_mgg_doubleendcap_100mevMin_innerEndcap", "#gamma #gamma reconstructed mass, double inner endcap, both photons > 100 MeV; Mass [GeV]; Entries", 200, 0, 1.0);

    h_mgg_doublebarrel_400mevMin = new TH1D("h_mgg_doublebarrel_400mevMin", "#gamma #gamma reconstructed mass, double barrel, both photons > 400 MeV; Mass [GeV]; Entries", 200, 0, 1.0);
    h_mgg_doubleendcap_400mevMin = new TH1D("h_mgg_doubleendcap_400mevMin", "#gamma #gamma reconstructed mass, double endcap, both photons > 400 MeV; Mass [GeV]; Entries", 200, 0, 1.0);

    h_nclus_barrel_endcap = new TH2D("h_nclus_barrel_endcap", "Number of clusters in barrel and endcaps; N_{clusters, barrel}; N_{clusters, endcaps}", 10, 0, 10, 10, 0, 10);
    h_nclus_barrel_endcap_100mevMin = new TH2D("h_nclus_barrel_endcap_100mevMin", "Number of clusters in barrel and endcaps; N_{clusters, barrel}; N_{clusters, endcaps}", 10, 0, 10, 10, 0, 10);
    h_nclus_barrel_endcap_400mevMin = new TH2D("h_nclus_barrel_endcap_400mevMin", "Number of clusters in barrel and endcaps; N_{clusters, barrel}; N_{clusters, endcaps}", 10, 0, 10, 10, 0, 10);

    h_nclusSum_100mev_avg_cosTheta_phiSimu = new TH2D("h_nclusSum_100mev_avg_cosTheta_phiSimu", "Average number of clusters in barrel and endcaps; cos(#theta); #phi", 50, -1, 1, 50, -TMath::Pi(), TMath::Pi());
    h_simuTruecosTheta_phiSimu = new TH2D("h_simuTruecosTheta_phiSimu", "All MC events: angles; cos(#theta); #phi", 50, -1, 1, 50, -TMath::Pi(), TMath::Pi());
    h_prob1cl_cosTheta_esimu = new TH2D("h_prob1cl_cosTheta_esimu", "Probability to have 1 cluster in event; #cos(#theta); Energy_{simu} [GeV]", 50, -1, 1, 50, 0, 10);
    h_prob1cl_100MeV_cosTheta_esimu = new TH2D("h_prob1cl_100MeV_cosTheta_esimu", "Probability to have 1 cluster above 100 MeV in event; #cos(#theta); Energy_{simu} [GeV]", 50, -1, 1, 50, 0, 10);
    h_simuTruecosTheta_energySimu = new TH2D("h_simuTruecosTheta_energySimu", "All MC events: angles; cos(#theta); Energy [GeV]", 50, -1, 1, 50, 0, 10);

    h_npi0_doublebarrel_pt_phi = new TH2D("h_mgg_doublebarrel_pt_phi", "Number of reconstructed pi0; p_{T} [GeV]; Phi [rad]", axis_pt_bins->GetNbins(), axis_pt_bins->GetXmin(), axis_pt_bins->GetXmax(), axis_phi_bins->GetNbins(), axis_phi_bins->GetXmin(), axis_phi_bins->GetXmax());

    h_npi0_doublebarrel_pt_phieff = new TH2D("h_npi0_doublebarrel_pt_phieff", "Number of reconstructed pi0; p_{T} [GeV]; Phi_{eff} [rad]", axis_pt_bins->GetNbins(), axis_pt_bins->GetXmin(), axis_pt_bins->GetXmax(), axis_phieff_bins->GetNbins(), axis_phieff_bins->GetXmin(), axis_phieff_bins->GetXmax());

    h_nsimu_doublebarrel_pt_phieff = new TH2D("h_nsimu_doublebarrel_pt_phieff", "Number of simulated pi0; p_{T} [GeV]; Phi_{eff} [rad]", axis_pt_bins->GetNbins(), axis_pt_bins->GetXmin(), axis_pt_bins->GetXmax(), axis_phieff_bins->GetNbins(), axis_phieff_bins->GetXmin(), axis_phieff_bins->GetXmax());



   for (Int_t ix = 1; ix <= axis_pt_bins->GetNbins(); ++ix) {
        vector<TH1D*> currv;
        for (Int_t iy = 1; iy <= axis_phi_bins->GetNbins(); ++iy) {
            TString histname = "h_mgg_db_bins_pt_phi_"+to_string(ix) + "_" + to_string(iy);
            Double_t ptlow = axis_pt_bins->GetBinLowEdge(ix);
            Double_t ptup = axis_pt_bins->GetBinUpEdge(ix);
            Double_t philow = axis_phi_bins->GetBinLowEdge(iy);
            Double_t phiup = axis_phi_bins->GetBinUpEdge(iy);
            TString histtitle = "#gamma #gamma reconstructed mass, " + to_string(ptlow) + " GeV < p_{T} < " + to_string(ptup) + " GeV, " + to_string(philow) + " rad < #phi < " + to_string(phiup) + " rad; M [GeV]; Entries";
            TH1D* newhist = new TH1D(histname, histtitle, 200, 0, 1.0);
            currv.push_back(newhist);
        }
        h_mgg_db_bins_pt_phi.push_back(currv);
   } 
   for (Int_t ix = 1; ix <= axis_pt_bins->GetNbins(); ++ix) {
        vector<TH1D*> currv;
        for (Int_t iy = 1; iy <= axis_phieff_bins->GetNbins(); ++iy) {
            TString histname = "h_mgg_db_bins_pt_phieff_"+to_string(ix) + "_" + to_string(iy);
            Double_t ptlow = axis_pt_bins->GetBinLowEdge(ix);
            Double_t ptup = axis_pt_bins->GetBinUpEdge(ix);
            Double_t philow = axis_phieff_bins->GetBinLowEdge(iy);
            Double_t phiup = axis_phieff_bins->GetBinUpEdge(iy);
            TString histtitle = "#gamma #gamma reconstructed mass, " + to_string(ptlow) + " GeV < p_{T} < " + to_string(ptup) + " GeV, " + to_string(philow) + " rad < #phi_{eff} < " + to_string(phiup) + " rad; M [GeV]; Entries";
            TH1D* newhist = new TH1D(histname, histtitle, 200, 0, 1.0);
            currv.push_back(newhist);
        }
        h_mgg_db_bins_pt_phieff.push_back(currv);
   } 

//    heff_statloss_vs_pt_35mm.passed = new TH1D("h_statloss_vs_pt_35mm", "Loss of statistics of pi0 when cutting off 35 mm of gap; p_{T} [GeV]; Efficiency", axis_pt_bins->GetNbins(), axis_pt_bins->GetXmin(), axis_pt_bins->GetXmax());
//    heff_statloss_vs_pt_35mm.total = new TH1D("h_statlossAll_vs_pt_35mm", "Loss of statistics of pi0 when cutting off 35 mm of gap; p_{T} [GeV]; Efficiency", axis_pt_bins->GetNbins(), axis_pt_bins->GetXmin(), axis_pt_bins->GetXmax());

//    heff_statloss_vs_pt_115mm.passed = new TH1D("h_statloss_vs_pt_115mm", "Loss of statistics of pi0 when cutting off 115 mm of gap; p_{T} [GeV]; Efficiency", axis_pt_bins->GetNbins(), axis_pt_bins->GetXmin(), axis_pt_bins->GetXmax());
//    heff_statloss_vs_pt_115mm.total = new TH1D("h_statlossAll_vs_pt_115mm", "Loss of statistics of pi0 when cutting off 115 mm of gap; p_{T} [GeV]; Efficiency", axis_pt_bins->GetNbins(), axis_pt_bins->GetXmin(), axis_pt_bins->GetXmax());

    Double_t minAngleGammas = 0.;
    Double_t maxAngleGammas = 0.2;
    Double_t nbinsAngleGammas = 50;

    Double_t minTheta = 0.;
    Double_t maxTheta = 70.;
    Double_t nbinsTheta = 20;


    TH1D* heff_statloss_vs_pt_35mm_passed = new  
            TH1D("h_statloss_vs_pt_35mm", 
                "Loss of statistics of pi0 when cutting off 35 mm of gap; p_{T} [GeV]; Efficiency", 
                axis_pt_bins->GetNbins(), axis_pt_bins->GetXmin(), axis_pt_bins->GetXmax());
    TH1D* heff_statloss_vs_pt_35mm_total = new  
            TH1D("h_statlossAll_vs_pt_35mm", 
                "Loss of statistics of pi0 when cutting off 35 mm of gap; p_{T} [GeV]; Efficiency", 
                axis_pt_bins->GetNbins(), axis_pt_bins->GetXmin(), axis_pt_bins->GetXmax());



    heff_statloss_vs_pt_35mm = hist1DEfficiency("heff_statloss_vs_pt_35mm", 
            heff_statloss_vs_pt_35mm_passed, heff_statloss_vs_pt_35mm_total);

    TH1D* heff_pi0reconstruction_vs_thetaPi0_passed = new 
            TH1D("h_pi0reconstruction_vs_thetaPi0_passed", 
                "Ratio of #pi^{0} reconstructed; Polar angle of #pi^{0}; Ratio", 
                nbinsTheta, minTheta, maxTheta); 
    TH1D* heff_pi0reconstruction_vs_thetaPi0_total = new 
            TH1D("h_pi0reconstruction_vs_thetaPi0_total", 
                "All #pi^{0}: reconstruction efficiency; Polar angle of #pi^{0}; Ratio", 
                nbinsTheta, minTheta, maxTheta);


    heff_pi0reconstruction_vs_thetaPi0 = hist1DPi0Efficiency("heff_pi0reconstruction_vs_thetaPi0", "Pi0 angle", 
            heff_pi0reconstruction_vs_thetaPi0_passed, heff_pi0reconstruction_vs_thetaPi0_total);


//    heff_pi0reconstruction_vs_angleGammas.passed = new TH1D("h_pi0reconstruction_vs_angleGammas_passed", "Ratio of #pi^{0} reconstructed; Angle between photons; Ratio", nbinsAngleGammas, minAngleGammas, maxAngleGammas);
//    heff_pi0reconstruction_vs_angleGammas.total = new TH1D("h_pi0reconstruction_vs_angleGammas_total", "All #pi^{0}: distance between photons; Angle between photons; Ratio", nbinsAngleGammas, minAngleGammas, maxAngleGammas);
    
//    heff_pi0reconstruction_vs_angleGammas_epi0.passed = new TH2D("h_pi0reconstruction_vs_angleGammas_epi0_passed", "Ratio of #pi^{0} reconstructed; Angle between photons; #pi^{0} true MC energy [GeV]", nbinsAngleGammas, minAngleGammas, maxAngleGammas, 20, 0, 10);
//    heff_pi0reconstruction_vs_angleGammas_epi0.total = new TH2D("h_pi0reconstruction_vs_angleGammas_epi0_total", "All #pi^{0}: distance between photons; Angle between photons; #pi^{0} true MC energy [GeV]", nbinsAngleGammas, minAngleGammas, maxAngleGammas, 20, 0, 10);

}


Double_t fitleft = 0.08;
Double_t fitright = 0.2;
TF1 fit_gaus_bkg("fit_gaus_bkg", "gausn(0) + [3]", fitleft, fitright);

//________________________________________________________________________________
Double_t GetNpi0(TH1D* h) {
    Int_t binleft = h->GetXaxis()->FindBin(fitleft);
    Int_t binright = h->GetXaxis()->FindBin(fitright);
    Int_t binrightmargin = h->GetXaxis()->FindBin(0.3);

    if (binrightmargin == binright) {
        cout << "cannot obtain estimate for background!" << endl;
        exit(1);
    }

    fit_gaus_bkg.SetParameters(h->Integral(binleft, binright)*h->GetXaxis()->GetBinWidth(2), 0.135, 0.1, h->Integral(binright, binrightmargin)/(binrightmargin-binleft));
    TFitResultPtr r = h->Fit(&fit_gaus_bkg, "lsr");
    DrawHist(h, "fits/");
    return r->Parameter(0);
}


//________________________________________________________________________________
void Finalize() {

    gStyle->SetOptStat(0);
    //------------------------- PHOTON IDENTIFICATION EFFICIENCIES (PI/GAMMA SEPARATION)
    h_pi0_misid_energy_num->Divide(h_pi0_misid_energy_den);
    h_pi0_misid_angle_num->Divide(h_pi0_misid_angle_den);
    h_pi0_misid_energy_num->GetYaxis()->SetRangeUser(0., 1.);
    h_pi0_misid_angle_num->GetYaxis()->SetRangeUser(0., 1.);
    DrawHist(h_pi0_misid_energy_num);
    DrawHist(h_pi0_misid_energy_den);
    DrawHist(h_pi0_misid_angle_num);
    DrawHist(h_pi0_misid_angle_den);

    h_pi0_2cl_misid_energy_num->Divide(h_pi0_2cl_misid_energy_den);
    h_pi0_2cl_misid_angle_num->Divide(h_pi0_2cl_misid_angle_den);
    h_pi0_2cl_misid_energy_num->GetYaxis()->SetRangeUser(0., 1.);
    h_pi0_2cl_misid_angle_num->GetYaxis()->SetRangeUser(0., 1.);
    DrawHist(h_pi0_2cl_misid_energy_num);
    DrawHist(h_pi0_2cl_misid_energy_den);
    DrawHist(h_pi0_2cl_misid_angle_num);
    DrawHist(h_pi0_2cl_misid_angle_den);


    //------------------------- PICTURES FOR TESTING
    DrawHist(h_esum_barrel_endcaps);
    DrawHist(h_mgg_doublebarrel_energy_mgg);
    DrawHist(h_mgg_doubleendcap_energy_mgg);
    DrawHistFitPi0(h_mgg_doublebarrel);
    DrawHistFitPi0(h_mgg_doubleendcap);
    DrawHistFitPi0(h_mgg_doublebarrel_100mevMin);
    DrawHistFitPi0(h_mgg_doubleendcap_100mevMin);
    DrawHistFitPi0(h_mgg_doubleendcap_100mevMin_innerEndcap);
    DrawHistFitPi0(h_mgg_doublebarrel_400mevMin);
    DrawHistFitPi0(h_mgg_doubleendcap_400mevMin);
    DrawHist(h_nclus_barrel_endcap);
    DrawHist(h_nclus_barrel_endcap_100mevMin);
    DrawHist(h_nclus_barrel_endcap_400mevMin);

    h_nclusSum_100mev_avg_cosTheta_phiSimu->Divide(h_simuTruecosTheta_phiSimu);
    DrawHist(h_nclusSum_100mev_avg_cosTheta_phiSimu);
    h_prob1cl_cosTheta_esimu->Divide(h_simuTruecosTheta_energySimu);
    DrawHist(h_prob1cl_cosTheta_esimu);
    h_prob1cl_100MeV_cosTheta_esimu->Divide(h_simuTruecosTheta_energySimu);
    DrawHist(h_prob1cl_100MeV_cosTheta_esimu);
    DrawHist(h_simuTruecosTheta_phiSimu);
    DrawHist(h_simuTruecosTheta_energySimu);

    DrawHist(h_nsimu_doublebarrel_pt_phieff);

    for (auto v: h_mgg_db_bins_pt_phi) {
        for (auto h: v) {
            DrawHist(h, "h_mgg_db_bins_pt_phi/");
        }
    }
    for (auto v: h_mgg_db_bins_pt_phieff) {
        for (auto h: v) {
            DrawHist(h, "h_mgg_db_bins_pt_phieff/");
        }
    }

    for (Int_t ipt = 0; ipt < h_mgg_db_bins_pt_phieff.size(); ++ipt) {
        Double_t binpt = ipt + 1;
        Double_t ptlow = axis_pt_bins->GetBinLowEdge(binpt);
        Double_t ptup = axis_pt_bins->GetBinUpEdge(binpt);
        
        TString histname = "h_ptbin_npi0_vs_phieff_" + to_string(ipt);
        TString histtitle = "Number of reconstructed pi0 for " + to_string(ptlow) + " GeV < p_{T} < " + to_string(ptup) + " GeV; #phi_{eff} [rad]; Entries";
        TH1D h_ptbin_npi0_vs_phieff(histname, histtitle, axis_phieff_bins->GetNbins(), axis_phieff_bins->GetXmin(), axis_phieff_bins->GetXmax());

        for (Int_t iphieff = 0; iphieff < h_mgg_db_bins_pt_phieff[ipt].size(); ++iphieff) {
            Double_t npi0 = GetNpi0(h_mgg_db_bins_pt_phieff[ipt][iphieff]);
            Int_t binphieff = iphieff + 1;
            h_ptbin_npi0_vs_phieff.SetBinContent(binphieff, npi0);
        }
        DrawHist(&h_ptbin_npi0_vs_phieff, "h_ptbin_npi0_vs_phieff/");


        histname = "h_ptbin_nsimu_vs_phieff_" + to_string(ipt);
        histtitle = "Number of simulated pi0 for " + to_string(ptlow) + " GeV < p_{T} < " + to_string(ptup) + " GeV; #phi_{eff} [rad]; Entries";
        TH1D h_ptbin_nsimu_vs_phieff(histname, histtitle, axis_phieff_bins->GetNbins(), axis_phieff_bins->GetXmin(), axis_phieff_bins->GetXmax());

        for (Int_t iphieff = 0; iphieff < h_mgg_db_bins_pt_phieff[ipt].size(); ++iphieff) {
            Int_t binphieff = iphieff + 1;
            Double_t npi0 = h_nsimu_doublebarrel_pt_phieff->GetBinContent(ipt, binphieff);
            h_ptbin_nsimu_vs_phieff.SetBinContent(binphieff, npi0);
        }
        DrawHist(&h_ptbin_nsimu_vs_phieff, "h_ptbin_nsimu_vs_phieff/");

        h_ptbin_npi0_vs_phieff.Divide(&h_ptbin_nsimu_vs_phieff);
        histname = "h_ptbin_ratioRecoSimu_vs_phieff_" + to_string(ipt);
        h_ptbin_npi0_vs_phieff.SetName(histname);
        DrawHist(&h_ptbin_npi0_vs_phieff, "h_ptbin_ratioRecoSimu_vs_phieff/");

    }

    heff_statloss_vs_pt_35mm.Finalize();
//    heff_statloss_vs_pt_115mm.Finalize();
//    heff_pi0reconstruction_vs_angleGammas.Finalize();
    heff_pi0reconstruction_vs_thetaPi0.Finalize();
//    heff_pi0reconstruction_vs_angleGammas_epi0.Finalize();


    delete axis_pt_bins;
    delete axis_phi_bins;
    delete axis_phieff_bins;
    delete h_esum_barrel_endcaps;
    delete h_mgg_doublebarrel_energy_mgg;
    delete h_mgg_doubleendcap_energy_mgg;
    delete h_mgg_doublebarrel;
    delete h_mgg_doubleendcap;
    delete h_mgg_doublebarrel_100mevMin;
    delete h_mgg_doubleendcap_100mevMin;
    delete h_mgg_doubleendcap_100mevMin_innerEndcap;
    delete h_mgg_doublebarrel_400mevMin;
    delete h_mgg_doubleendcap_400mevMin;
    delete h_nclus_barrel_endcap;
    delete h_nclus_barrel_endcap_100mevMin;
    delete h_nclus_barrel_endcap_400mevMin;
    delete h_nclusSum_100mev_avg_cosTheta_phiSimu;
    delete h_simuTruecosTheta_phiSimu;
    delete h_npi0_doublebarrel_pt_phi;
    delete h_npi0_doublebarrel_pt_phieff;
    delete h_nsimu_doublebarrel_pt_phieff;

    for (auto v: h_mgg_db_bins_pt_phi) {
        for (auto h: v) {
            delete h;
        }
    }
    for (auto v: h_mgg_db_bins_pt_phieff) {
        for (auto h: v) {
            delete h;
        }
    }

    delete IT;

}

//________________________________________________________________________________
void InitIterator(vector<TString> fname) {

    IT = new SpdMCDataIterator();

    for (auto fn: fname) {
        vector<TString> files = GetFilesStartingWith(fn);
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
void GetSetupParametersUsingIT() {

    //get parameters
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

    ok = EcalTBPars_->GetParameter("EcalBarrelNSectors", nsectors);
    if (!ok) {
        cout << "parameter EcalBarrelNSectors not found!" << endl;
        exit(1);
    }
    cout << "nsec: " << nsectors << "\n";
}


//________________________________________________________________________________
void CheckReconstructionPi0(vector<TString> recoFile = {"recoPi0.root"}, TString pictureDir = "CheckReconstructionPi0/") {

  c1 = new TCanvas("c1", "c1", 1200, 800);

  gStyle->SetOptFit(1);

  fDirname = pictureDir;
  gSystem->Exec("mkdir -p " + fDirname);
  
  InitIterator(recoFile);
  GetSetupParametersUsingIT();
  InitHistograms();


    Int_t ev = 0;
    while (IT->NextEvent()) 
    {
        if (ev % 1000 == 0) cout << "event: " << ev << "\n";
        AnalyseEvent();
        ev += 1;
//        if (ev == 100) break;
    }

    Finalize();

}





Int_t npartMC;
Int_t npartRC;
vector<TVector3> momSimuGammas;
TVector3 momPi0Simu;
TLorentzVector lvPi0Simu;
Double_t phiEffGamma1;
Double_t phiEffGamma2;
Double_t phiEffPi0Simu;

vector<recoInfo> momRecoAll;
Int_t npartB;
Int_t npartEC;

Double_t eSumBarrel;
Double_t eSumEndcaps;

Double_t mgg_reco;

//Bool_t isPi0Reconstructed;


//________________________________________________________________________________
void ResetPerEventVariables() {
    npartMC = 0;
    npartRC = 0;
    momSimuGammas.clear();
    momRecoAll.clear();
    npartB = 0;
    npartEC = 0;
    eSumBarrel = 0.;
    eSumEndcaps = 0.;
//    isPi0Reconstructed = false;
}

//________________________________________________________________________________
void GetMCInfo() {
    npartMC = ParticlesMC_->GetEntriesFast();

    for (int it = 0; it < npartMC; ++it) {
        SpdMCParticle* part = (SpdMCParticle*)ParticlesMC_->At(it);
        if (part->GetPdgCode() == 111) {
//            part->Print();
            momPi0Simu = part->GetStartMom();
            break;
        }
    } 
    for (int it = 0; it < min(npartMC, 10); ++it) {
        SpdMCParticle* part = (SpdMCParticle*)ParticlesMC_->At(it);
        if (part->GetPdgCode() == 22 && part->GetMotherId() == 0) {
            momSimuGammas.push_back(part->GetStartMom());
        }
    } 

    if (momSimuGammas.size() > 0) {
        phiEffGamma1 = GetPhiEff( momSimuGammas[0].Phi(), nsectors );
    }
    if (momSimuGammas.size() > 1) {
        phiEffGamma2 = GetPhiEff( momSimuGammas[1].Phi(), nsectors );
    }


}

//________________________________________________________________________________
void FillMCPerEventVariables() {

    lvPi0Simu.SetVectM(momPi0Simu, MassPi0);

    phiEffPi0Simu = GetPhiEff(lvPi0Simu.Vect().Phi(), nsectors);
    Double_t gapWidthRad = 3.5/fBarrelRadius;
    Double_t gapWidthRad2 = 11.5/fBarrelRadius;

    if (momSimuGammas.size() == 2) {
       heff_statloss_vs_pt_35mm.total->Fill(lvPi0Simu.Vect().Pt()); 
       if (fabs(phiEffGamma1) > gapWidthRad/2 && fabs(phiEffGamma2) > gapWidthRad/2) {
            heff_statloss_vs_pt_35mm.passed->Fill(lvPi0Simu.Vect().Pt());
       }
//       heff_statloss_vs_pt_115mm.total->Fill(lvPi0Simu.Vect().Pt()); 
       if (fabs(phiEffGamma1) > gapWidthRad2/2 && fabs(phiEffGamma2) > gapWidthRad2/2) {
//            heff_statloss_vs_pt_115mm.passed->Fill(lvPi0Simu.Vect().Pt());
       }
    }

    h_nsimu_doublebarrel_pt_phieff->Fill(lvPi0Simu.Vect().Pt(), phiEffPi0Simu);

    h_simuTruecosTheta_phiSimu->Fill(momPi0Simu.CosTheta(), momPi0Simu.Phi());
    h_simuTruecosTheta_energySimu->Fill(momPi0Simu.CosTheta(), momPi0Simu.Mag());

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
        for (auto c: cells) {
            SpdEcalMCHit* hit = (SpdEcalMCHit*)EcalHitsMC_->At(c);
            TVector3 hitpos = hit->GetPosition();
        }

        TVector3 momReco = part->GetPosition();
        Double_t en1 = part->GetEnergy();
        momReco.SetMag(en1);
        if (fabs(part->GetPosition().Z()) > fECMinDist-1.0) {
            momRecoAll.push_back(recoInfo({momReco, kEndcap, ncells, part->GetPID()}));
            npartEC += 1;
        }
        else {
            momRecoAll.push_back(recoInfo({momReco, kBarrel, ncells, part->GetPID()}));
            npartB += 1;
        }
    }
}


//________________________________________________________________________________
void FillPi0Candidates(recoInfo p1, recoInfo p2, TLorentzVector simuPi0) {

    TLorentzVector lv1, lv2;
    lv1.SetVectM(p1.mom, 0.);
    lv2.SetVectM(p2.mom, 0.);

    TLorentzVector lvsum = lv1 + lv2;

    if (p1.part == kBarrel && p2.part == kBarrel) {
        h_mgg_doublebarrel_energy_mgg->Fill(lvsum.E(), lvsum.Mag());
        h_mgg_doublebarrel->Fill(lvsum.Mag());
        if (p1.mom.Mag() > 0.4 && p2.mom.Mag() > 0.4) {
            h_mgg_doublebarrel_400mevMin->Fill(lvsum.Mag());
        }
        if (p1.mom.Mag() > 0.1 && p2.mom.Mag() > 0.1) {
            h_mgg_doublebarrel_100mevMin->Fill(lvsum.Mag());
        }
        Int_t ptbin = axis_pt_bins->FindBin(simuPi0.Vect().Pt()) - 1;
        Int_t phibin = axis_phi_bins->FindBin(simuPi0.Vect().Phi()) - 1;
        Double_t currphi = simuPi0.Vect().Phi();
        Double_t phieff = GetPhiEff(currphi, nsectors);
        Int_t phieffbin = axis_phieff_bins->FindBin(phieff) - 1;

        if (ptbin <= h_mgg_db_bins_pt_phi.size() && ptbin >= 0 && phibin <= h_mgg_db_bins_pt_phi[0].size() && phibin >= 0) {
            h_mgg_db_bins_pt_phi[ptbin][phibin]->Fill(lvsum.Mag());
        }
        if (ptbin <= h_mgg_db_bins_pt_phi.size() && ptbin >= 0 && phieffbin <= h_mgg_db_bins_pt_phieff[0].size() && phieffbin >= 0) {
            h_mgg_db_bins_pt_phieff[ptbin][phieffbin]->Fill(lvsum.Mag());
        }
    }

    if (p1.part == kEndcap && p2.part == kEndcap) {
        h_mgg_doubleendcap_energy_mgg->Fill(lvsum.E(), lvsum.Mag());
        h_mgg_doubleendcap->Fill(lvsum.Mag());
        if (p1.mom.Mag() > 0.4 && p2.mom.Mag() > 0.4) {
            h_mgg_doubleendcap_400mevMin->Fill(lvsum.Mag());
        }
        if (p1.mom.Mag() > 0.1 && p2.mom.Mag() > 0.1) {
            h_mgg_doubleendcap_100mevMin->Fill(lvsum.Mag());
            if ( fabs(p1.mom.CosTheta()) > 0.95 ) h_mgg_doubleendcap_100mevMin_innerEndcap->Fill(lvsum.Mag());
        }
    }
}


//________________________________________________________________________________
void ReconstructPi0Mass() {

    vector<recoInfo> momRecoFilt;
    for (auto m: momRecoAll) {
        if (m.mom.Mag() > 0.1) momRecoFilt.push_back(m);
    }
    if (momRecoFilt.size() < 2) return false;

    sort(momRecoFilt.begin(), momRecoFilt.end(), [](const recoInfo & a, const recoInfo & b) { return a.mom.Mag() > b.mom.Mag(); });

    TLorentzVector lv1, lv2;
    lv1.SetVectM(momRecoFilt[0].mom, 0.);
    lv2.SetVectM(momRecoFilt[1].mom, 0.);

    mgg_reco = (lv1 + lv2).Mag();

}


//________________________________________________________________________________
void AnalyzePerEventRCVariables() {
    
    int nClusBarrel = 0;
    int nClusBarrel_100 = 0;
    int nClusBarrel_400 = 0;
    
    int nClusEndcap = 0;
    int nClusEndcap_100 = 0;
    int nClusEndcap_400 = 0;

    for (auto p1: momRecoAll) {
        if (p1.part == kBarrel) {
            eSumBarrel += p1.mom.Mag();
            nClusBarrel += 1;
            if (p1.mom.Mag() > 0.4) nClusBarrel_400 += 1;
            if (p1.mom.Mag() > 0.1) nClusBarrel_100 += 1;
        }
        else {
            eSumEndcaps += p1.mom.Mag();
            nClusEndcap += 1;
            if (p1.mom.Mag() > 0.4) nClusEndcap_400 += 1;
            if (p1.mom.Mag() > 0.1) nClusEndcap_100 += 1;
        }
    }

    h_nclus_barrel_endcap->Fill(nClusBarrel, nClusEndcap);
    h_nclus_barrel_endcap_100mevMin->Fill(nClusBarrel_100, nClusEndcap_100);
    h_nclus_barrel_endcap_400mevMin->Fill(nClusBarrel_400, nClusEndcap_400);

    h_nclusSum_100mev_avg_cosTheta_phiSimu->Fill(momPi0Simu.CosTheta(), momPi0Simu.Phi(), nClusBarrel_100 + nClusEndcap_100);
    if (nClusBarrel + nClusEndcap == 1) {
        h_prob1cl_cosTheta_esimu->Fill(momPi0Simu.CosTheta(), momPi0Simu.Mag());
    }
    if (nClusBarrel_100 + nClusEndcap_100 == 1) {
        h_prob1cl_100MeV_cosTheta_esimu->Fill(momPi0Simu.CosTheta(), momPi0Simu.Mag());
    }

    for (Int_t i1 = 0; i1 < momRecoAll.size(); ++i1) {
        recoInfo p1 = momRecoAll[i1];
        for (Int_t i2 = i1+1; i2 < momRecoAll.size(); ++i2) {
            recoInfo p2 = momRecoAll[i2];
            FillPi0Candidates(p1, p2, lvPi0Simu);
        }
    }

    ReconstructPi0Mass();

}

//________________________________________________________________________________
void FillRCPerEventVariables() {

    h_esum_barrel_endcaps->Fill(eSumBarrel, eSumEndcaps);

    if (momSimuGammas.size() == 2) {
        Double_t angleGammas = momSimuGammas[0].Angle(momSimuGammas[1]);
//        heff_pi0reconstruction_vs_angleGammas.total->Fill(angleGammas);
//        heff_pi0reconstruction_vs_angleGammas_epi0.total->Fill(angleGammas, momPi0Simu.Mag());
        heff_pi0reconstruction_vs_thetaPi0.FillTotal(TMath::RadToDeg()*fabs(TMath::Pi()/2 - momPi0Simu.Theta()));
//        if (isPi0Reconstructed) {
//            heff_pi0reconstruction_vs_angleGammas.passed->Fill(angleGammas);
//            heff_pi0reconstruction_vs_angleGammas_epi0.passed->Fill(angleGammas, momPi0Simu.Mag());
            heff_pi0reconstruction_vs_thetaPi0.FillPassedMgg(TMath::RadToDeg()*fabs(TMath::Pi()/2 - momPi0Simu.Theta()), mgg_reco);
//        }


        Double_t e = lvPi0Simu.E();
        Double_t a = lvPi0Simu.Vect().Theta();

        if (momRecoAll.size() == 1) {
            h_pi0_misid_energy_den->Fill(e);
            h_pi0_misid_angle_den->Fill(a);
            h_pi0_2cl_misid_energy_den->Fill(e);
            h_pi0_2cl_misid_angle_den->Fill(a);

            if (momRecoAll[0].pid == 111) {
                h_pi0_misid_energy_num->Fill(e);
                h_pi0_misid_angle_num->Fill(a);
            }
        }
        else if (momRecoAll.size() == 2) {
            h_pi0_misid_energy_den->Fill(e);
            h_pi0_misid_angle_den->Fill(a);
            h_pi0_misid_energy_num->Fill(e);
            h_pi0_misid_angle_num->Fill(a);

            h_pi0_2cl_misid_energy_den->Fill(e);
            h_pi0_2cl_misid_angle_den->Fill(a);
            h_pi0_2cl_misid_energy_num->Fill(e);
            h_pi0_2cl_misid_angle_num->Fill(a);
        }


    }

}



//________________________________________________________________________________
void AnalyseEvent()
{
    
    ResetPerEventVariables();
    GetMCInfo();
    FillMCPerEventVariables();
    GetRCInfo();
    AnalyzePerEventRCVariables();
    FillRCPerEventVariables();

}
















