
#include "TRandom3.h"
#include "TH1.h"
#include "TStyle.h"
#include "TSystem.h"
#include "TLegend.h"
#include "TH2.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TFitResult.h"
#include "TMath.h"
#include "TF1.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include "TCanvas.h"
#include "TSystemDirectory.h"
#include "TGeoNavigator.h"
#include "TTree.h"
#include "TObjString.h"
#include "TH3.h"
#include "TClonesArray.h"

#include <vector>

using namespace std;

vector<int> colordict {kRed, kBlue, kBlack, kGreen, kMagenta, kCyan, kOrange};
int GetHistColor(int i) {
    return colordict[i % colordict.size()] + 2*(i / colordict.size());
}


TCanvas * c1;
//TCanvas * c1 = new TCanvas("c1", "c1", 1200, 800);
TString fDirname;

TF1* fFitPi0 = new TF1("fFitPi0", "gaus(0) + [3] + [4]*x", 0.09, 0.2);



//________________________________________________________________________________
template <typename T>
std::string to_string_wp(const T a_value, const int n = 6)
{
    std::ostringstream out;
    out.precision(n);
    out << std::fixed << a_value;
    return out.str();
}

//________________________________________________________________________________
Double_t GetPhiEff(Double_t currphi, Int_t nsec) {
//    cout << "hpieff: " << currphi << " nsec : " << nsec << endl;
//    Double_t phieff = currphi - (TMath::Pi()*2/nsec)*int(floor((currphi + TMath::Pi()/nsec)/(TMath::Pi()*2/nsec))); //this way, assuming gaps are at phi=+- pi/nsec (not at phi=0), gaps will be at +-pi/nsec
    Double_t phieff = currphi - (TMath::Pi()*2/nsec)*int(floor((currphi)/(TMath::Pi()*2/nsec))) - TMath::Pi()/nsec; //this way, gaps will be at phiEff = 0 (at the center)
//    cout << phieff << endl;
    return phieff;
}

//________________________________________________________________________________
void DrawGraph(TGraph* hist, TString subdir="") {
 
    c1->Clear();
    if (subdir != "") gSystem->Exec("mkdir -p " + fDirname + "/" + subdir);
    gStyle->SetOptStat(111111);
    hist->GetHistogram()->SetMinimum(0.);
    //hist->GetHistogram()->SetMaximum(0.2);
    hist->SetMarkerStyle(20);
    hist->Draw("AP*");
    TString picturename = fDirname + "/" + subdir + "/" + hist->GetName() + ".png";
    c1->SaveAs(picturename, "png");
    picturename = fDirname + "/" + subdir + "/" + hist->GetName() + ".root";
    c1->SaveAs(picturename, "root");
    picturename = fDirname + "/" + subdir + "/" + hist->GetName() + ".pdf";
    c1->SaveAs(picturename, "pdf");
    
}



//________________________________________________________________________________
void DrawGraph(TGraphErrors* hist, TString subdir="") {
 
    c1->Clear();
    if (subdir != "") gSystem->Exec("mkdir -p " + fDirname + "/" + subdir);
    gStyle->SetOptStat(111111);
    hist->GetHistogram()->SetMinimum(0.);
    //hist->GetHistogram()->SetMaximum(0.2);
    hist->SetMarkerStyle(20);
    hist->Draw("AP");
    TString picturename = fDirname + "/" + subdir + "/" + hist->GetName() + ".png";
    c1->SaveAs(picturename, "png");
    picturename = fDirname + "/" + subdir + "/" + hist->GetName() + ".root";
    c1->SaveAs(picturename, "root");
    picturename = fDirname + "/" + subdir + "/" + hist->GetName() + ".pdf";
    c1->SaveAs(picturename, "pdf");
    
}


//________________________________________________________________________________
void DrawManyGraphs(vector<TGraphErrors*> grs, TString subdir="", Double_t ymin=0.0, Double_t ymax=1.0) {
 
    if (grs.size() == 0) {
        cout << "no I will not draw a zero-sized vector of graphs >:()" << endl;
        exit(1); 
    }
    
    c1->Clear();
    if (subdir != "") gSystem->Exec("mkdir -p " + fDirname + "/" + subdir);
    gStyle->SetOptStat(111111);
    for (int ig = 0; ig < grs.size(); ++ig) {
        auto gr = grs[ig];
        gr->SetMarkerStyle(20);
        gr->SetMarkerColor(colordict[ig]);
        if (ig == 0) {
            gr->GetHistogram()->SetMinimum(ymin);
            gr->GetHistogram()->SetMaximum(ymax);
            gr->Draw("AP");
        }
        else {
            gr->Draw("same P");
        }
    }
    TString picturename = fDirname + "/" + subdir + "/" + grs[0]->GetName() + "_combined.png";
    c1->SaveAs(picturename, "png");
    picturename = fDirname + "/" + subdir + "/" + grs[0]->GetName() + "_combined.root";
    c1->SaveAs(picturename, "root");
    picturename = fDirname + "/" + subdir + "/" + grs[0]->GetName() + "_combined.pdf";
    c1->SaveAs(picturename, "pdf");
    
}


//________________________________________________________________________________
void DrawHistFitPi0(TH1D* hist, TString subdir="") {
    c1->Clear();
    gStyle->SetOptFit(1);
    if (subdir != "") gSystem->Exec("mkdir -p " + fDirname + "/" + subdir);
    hist->Draw();
    Double_t valLeft = hist->GetBinContent(hist->GetXaxis()->FindBin(0.1));
    Double_t valPeak = hist->GetBinContent(hist->GetXaxis()->FindBin(0.135));
    Double_t valRight = hist->GetBinContent(hist->GetXaxis()->FindBin(0.2));
    fFitPi0->SetParameters( valPeak - 0.5*(valLeft + valRight), 0.135, 0.015, valLeft - (valRight - valLeft), (valRight - valLeft)/0.1);  

    hist->Fit(fFitPi0, "lr");
    TString picturename = fDirname + subdir + "/" + TString(hist->GetName());
    c1->SaveAs(picturename + ".png", "png");
    c1->SaveAs(picturename + ".root", "root");
    c1->SaveAs(picturename + ".pdf", "pdf");
}


//________________________________________________________________________________
void DrawHist(TH1D* hist, TString subdir="") {
    c1->Clear();
    if (subdir != "") gSystem->Exec("mkdir -p " + fDirname + "/" + subdir);
    hist->Draw();
    TString picturename = fDirname + subdir + "/" + TString(hist->GetName());
    c1->SaveAs(picturename + ".png", "png");
    c1->SaveAs(picturename + ".root", "root");
    c1->SaveAs(picturename + ".pdf", "pdf");
}

//________________________________________________________________________________
void DrawHist(TH1D* hist, TF1* fun, TString subdir="") {
    c1->Clear();
    if (subdir != "") gSystem->Exec("mkdir -p " + fDirname + "/" + subdir);
    hist->Draw();
    fun->Draw("same");
    TString picturename = fDirname + subdir + "/" + TString(hist->GetName());
    c1->SaveAs(picturename + ".png", "png");
    c1->SaveAs(picturename + ".root", "root");
    c1->SaveAs(picturename + ".pdf", "pdf");
}

//________________________________________________________________________________
void DrawHist(TH2D* hist, TString subdir="", TString opt="") {
    c1->Clear();
    if (subdir != "") gSystem->Exec("mkdir -p " + fDirname + "/" + subdir);
    if (opt == "") hist->Draw("colz");
    else hist->Draw("colz " + opt);
    TString picturename = fDirname + subdir + "/" + TString(hist->GetName());
    c1->SaveAs(picturename + ".png", "png");
    c1->SaveAs(picturename + ".root", "root");
    c1->SaveAs(picturename + ".pdf", "pdf");
}

//________________________________________________________________________________
void Draw2DHistAs1DSlicesY(TH2D* hist, TString subdir="", TString opt="") {

    vector<TH1D*> hists;

    auto GetAxisSliceTitle = [](TAxis* ax, int ibin) {
        Double_t binmin = ax->GetBinLowEdge(ibin);
        Double_t binmax = ax->GetBinUpEdge(ibin);
        TString ret = TString(to_string_wp(binmin, 2)) + " < " + ax->GetTitle() + " < " + to_string_wp(binmax, 2);
        return ret;
    };

    Double_t maxhist = -9e9;
    Double_t minhist = 9e9;

    for (int iy = 1; iy <= hist->GetNbinsY(); ++iy) {
        TString histname = TString(hist->GetName()) + "_projectionBin" + to_string(iy);
        TH1D* currhist = (TH1D*)hist->ProjectionX(histname, iy, iy);
        TString title = GetAxisSliceTitle(hist->GetYaxis(), iy);
        currhist->SetTitle(title);
        DrawHist(currhist, subdir);
        hists.push_back(currhist);

        Double_t cmax = currhist->GetMaximum();
        Double_t cmin = currhist->GetMinimum();

        if (cmax > maxhist) maxhist = cmax;
        if (cmin < minhist) minhist = cmin;
    }

    Double_t rangemax = maxhist + 0.1*(maxhist - minhist);
    Double_t rangemin = minhist - 0.1*(maxhist - minhist);

    TLegend* tl = new TLegend(0.7, 0.7, 0.9, 0.9);
    for (int i = 0; i < hists.size(); ++i) {
        hists[i]->SetLineColor(GetHistColor(i));
        if (i == 0) {
            hists[i]->GetYaxis()->SetRangeUser(rangemin, rangemax);
            hists[i]->Draw();
        }
        else hists[i]->Draw("same");

        TString title = hists[i]->GetTitle();
        tl->AddEntry(hists[i], title);

    } 
    tl->Draw("same");

    TString picturename = fDirname + subdir + "/" + TString(hist->GetName()) + "_allslices";
    c1->SaveAs(picturename + ".png", "png");
    c1->SaveAs(picturename + ".root", "root");
    c1->SaveAs(picturename + ".pdf", "pdf");

}

//________________________________________________________________________________
TGraphErrors* HistToGraph(TH1D* hist, TString subdir="") {
    
    vector<Double_t> x;
    vector<Double_t> y;
    vector<Double_t> yerr;
    
    for (int i = 1; i < hist->GetNbinsX(); ++i) {
        if (hist->GetBinContent(i) != 0.) {
            x.push_back(hist->GetXaxis()->GetBinCenter(i));
            y.push_back(hist->GetBinContent(i));
            yerr.push_back(hist->GetBinError(i));
        }
    }
    
    TGraphErrors* gr = new TGraphErrors(x.size(), &x[0], &y[0], 0, &yerr[0]);
    TString graphname = TString(hist->GetName()) + "_graph";
    gr->SetName(graphname);
    gr->SetTitle(hist->GetTitle());
    gr->GetXaxis()->SetTitle(hist->GetXaxis()->GetTitle());
    gr->GetYaxis()->SetTitle(hist->GetYaxis()->GetTitle());
    
    DrawGraph(gr, subdir);

    return gr;
    
}

struct DrawProfilesOutput {
    vector<pair<Double_t, Double_t> > fitparams;
    TGraphErrors* graphSigmaRel;
    TGraphErrors* graphSigma;
};


//// see math/mathcore/src/PdfFuncMathCore.cxx in ROOT 6.x
//Double_t crystalball_function(Double_t x, Double_t alpha, Double_t n, Double_t sigma, Double_t mean) {
//  // evaluate the crystal ball function
//  if (sigma < 0.)     return 0.;
//  Double_t z = (x - mean)/sigma; 
//  if (alpha < 0) z = -z; 
//  Double_t abs_alpha = std::abs(alpha);
//  // Double_t C = n/abs_alpha * 1./(n-1.) * std::exp(-alpha*alpha/2.);
//  // Double_t D = std::sqrt(M_PI/2.)*(1.+ROOT::Math::erf(abs_alpha/std::sqrt(2.)));
//  // Double_t N = 1./(sigma*(C+D));
//  if (z  > - abs_alpha)
//    return std::exp(- 0.5 * z * z);
//  else {
//    //Double_t A = std::pow(n/abs_alpha,n) * std::exp(-0.5*abs_alpha*abs_alpha);
//    Double_t nDivAlpha = n/abs_alpha;
//    Double_t AA =  std::exp(-0.5*abs_alpha*abs_alpha);
//    Double_t B = nDivAlpha -abs_alpha;
//    Double_t arg = nDivAlpha/(B-z);
//    return AA * std::pow(arg,n);
//  }
//}

//Double_t crystalball_function(const Double_t *x, const Double_t *p) {
//  // if ((!x) || (!p)) return 0.; // just a precaution
//  // [Constant] * ROOT::Math::crystalball_function(x, [Alpha], [N], [Sigma], [Mean])
//  return (p[0] * crystalball_function(x[0], p[3], p[4], p[2], p[1]));
//}


//________________________________________________________________________________
DrawProfilesOutput DrawProfiles(TH2D* hist, TF1* fitf, TString profDirName, Double_t firparSigma=0.04, Double_t firparConst=0.05) {
 
    DrawHist(hist, profDirName);

    gStyle->SetOptFit(1);
    gSystem->Exec("mkdir -p " + fDirname + "/" + profDirName);
    gSystem->Exec("mkdir -p " + fDirname + "/" + profDirName + "/" + string(hist->GetName()));

    TString histname = TString(hist->GetName()) + "_sigma";
    TH1D* hsigma = new TH1D(histname, histname, hist->GetNbinsX(), hist->GetXaxis()->GetXbins()->GetArray());
    hsigma->SetTitle(hist->GetTitle());
    hsigma->GetXaxis()->SetTitle(hist->GetXaxis()->GetTitle());
    string ytitle = string(hist->GetYaxis()->GetTitle()) + " (Absolute error)";
    hsigma->GetYaxis()->SetTitle(ytitle.c_str());
    hsigma->GetXaxis()->SetLimits(hist->GetXaxis()->GetXmin(), hist->GetXaxis()->GetXmax());

    histname = TString(hist->GetName()) + "_mean";
    TH1D* hmean = new TH1D(histname, histname, hist->GetNbinsX(), hist->GetXaxis()->GetXbins()->GetArray());
    hmean->SetTitle(hist->GetTitle());
    hmean->GetXaxis()->SetTitle(hist->GetXaxis()->GetTitle());
    hmean->GetYaxis()->SetTitle(hist->GetYaxis()->GetTitle());
    hmean->GetXaxis()->SetLimits(hist->GetXaxis()->GetXmin(), hist->GetXaxis()->GetXmax());

    histname = TString(hist->GetName()) + "_sigmaRel";
    TH1D* hsigmaRel = new TH1D(histname, histname, hist->GetNbinsX(), hist->GetXaxis()->GetXbins()->GetArray());
    hsigmaRel->SetTitle(hist->GetTitle());
    hsigmaRel->GetXaxis()->SetTitle(hist->GetXaxis()->GetTitle());
    ytitle = string(hist->GetYaxis()->GetTitle()) + ", Relative error (width/position ratio) (bin errors are estimate)";
    hsigmaRel->GetYaxis()->SetTitle(ytitle.c_str());
    hsigmaRel->GetXaxis()->SetLimits(hist->GetXaxis()->GetXmin(), hist->GetXaxis()->GetXmax());
    
    for (int i = 1; i <= hist->GetNbinsX(); ++i) {
        TH1D* currhist = (TH1D*)hist->ProjectionY(TString(hist->GetName()) + "_profX" + to_string(i), i, i);
        if (currhist->Integral() == 0.) continue;
        string title = string(hist->GetTitle()) + ", from " + to_string(hist->GetXaxis()->GetBinLowEdge(i)) + " to " + to_string(hist->GetXaxis()->GetBinUpEdge(i)) + " " + hist->GetXaxis()->GetTitle();
        currhist->SetTitle(title.c_str());
        Double_t mean = currhist->GetMean();
        Double_t sigma = currhist->GetRMS();
        Double_t xmin = mean - 3*sigma;
        Double_t xmax = mean + 3*sigma; 

        if (TString(hist->GetName()).Contains("h_ediff_etrue_angles_barrel_") == true) {

//            TF1* fitf2 = new TF1("fitf2", "[&](Double_t *x, Double_t *p){return crystalball_function(x,p);}", -4, 4, 5);
            TF1* fitf2 = new TF1("fitf2","[&](Double_t *x, Double_t *p){ Double_t g = p[0]*exp(-0.5*( pow( (x[0]-p[1])/p[2], 2) )); if (x[0] < p[1]) return g + p[3]; else return g; }", -3, 3, 4);
//            fitf2->SetParameters(currhist->GetMaximum(), currhist->GetXaxis()->GetBinCenter(currhist->GetMaximumBin()), currhist->GetRMS()/3,  1.0, 5.0);
            fitf2->SetParameters(currhist->GetMaximum(), currhist->GetXaxis()->GetBinCenter(currhist->GetMaximumBin()), currhist->GetRMS()/3,  firparConst*currhist->Integral()/currhist->GetNbinsX());
            TFitResultPtr r = currhist->Fit(fitf2, "ls", "");
//            cout << "\n\n\n\n\n fitting! npar: " << fitf2->GetNpar() << "\n";
//            TH1D* hcb = new TH1D("hcb", "hcb", 200, -6, 6);
//            TRandom3* rng = new TRandom3(0);
//            hcb->FillRandom("fitf2", 10000);
            DrawHist(currhist, profDirName + "/" + TString(hist->GetName()));


//            TF1* fitf3 = new TF1("fitf3", "gaus(0)");
//            fitf3->SetParameters(hcb->GetMaximum(), hcb->GetXaxis()->GetBinCenter(hcb->GetMaximumBin()), hcb->GetRMS());
//            TFitResultPtr r2 = hcb->Fit(fitf3, "ls", "");

            hsigma->SetBinContent(i, fabs(r->Parameter(2)));
            hsigma->SetBinError(i, r->Error(2));
            hmean->SetBinContent(i, r->Parameter(1));
            hmean->SetBinError(i, r->Error(1));
            if (r->Parameter(1) == 0.) {
                hsigmaRel->SetBinContent(i, 0);
                hsigmaRel->SetBinError(i, 0);
            } 
            else {
                hsigmaRel->SetBinContent(i, fabs(r->Parameter(2)/r->Parameter(1)));
                Double_t widthRelerr = r->Error(2)/r->Parameter(2);
                Double_t meanRelerr = r->Error(1)/r->Parameter(1);
                Double_t ratioRelerr = sqrt(pow(widthRelerr, 2) + pow(meanRelerr, 2));
                hsigmaRel->SetBinError(i, ratioRelerr*fabs(r->Parameter(2)/r->Parameter(1)));
            }

        }
        else if (TString(hist->GetName()).Contains("h_zdiff_energy_angles_barrel_4") == true && fDirname.Contains("projgeom") == true) {
            TF1* fitf2 = new TF1("fitf2", "gaus(0) + [3]", 0, 20);
    //        TF1* fitf = new TF1("fitf","[&](Double_t *x, Double_t *p){ Double_t g = p[0]*exp(-0.5*( pow( (x[0]-p[1])/p[2], 2) )); if (x[0] < p[1]) return g + p[3]; else return g; }", -3, 3, 4);
            fitf2->SetParameters(currhist->GetMaximum(), currhist->GetXaxis()->GetBinCenter(currhist->GetMaximumBin()), firparSigma, firparConst*currhist->Integral()/currhist->GetNbinsX());
            TFitResultPtr r = currhist->Fit(fitf2, "lsr", "");
            DrawHist(currhist, profDirName + "/" + TString(hist->GetName()));
            hsigma->SetBinContent(i, fabs(r->Parameter(2)));
            hsigma->SetBinError(i, r->Error(2));
            hmean->SetBinContent(i, r->Parameter(1));
            hmean->SetBinError(i, r->Error(1));
            if (r->Parameter(1) == 0.) {
                hsigmaRel->SetBinContent(i, 0);
                hsigmaRel->SetBinError(i, 0);
            } 
            else {
                hsigmaRel->SetBinContent(i, fabs(r->Parameter(2)/r->Parameter(1)));
                Double_t widthRelerr = r->Error(2)/r->Parameter(2);
                Double_t meanRelerr = r->Error(1)/r->Parameter(1);
                Double_t ratioRelerr = sqrt(pow(widthRelerr, 2) + pow(meanRelerr, 2));
                hsigmaRel->SetBinError(i, ratioRelerr*fabs(r->Parameter(2)/r->Parameter(1)));
            }
        }
        else {

//            cout << TString(hist->GetName()).Contains("h_zdiff_energy_angles_barrel_4") << " " << fDirname.Contains("projgeom")  << "\n";
//            cout << hist->GetName() << " " << fDirname << "\n";


            TF1* fitf2 = new TF1("fitf2", "gaus(0) + [3]");
    //        TF1* fitf = new TF1("fitf","[&](Double_t *x, Double_t *p){ Double_t g = p[0]*exp(-0.5*( pow( (x[0]-p[1])/p[2], 2) )); if (x[0] < p[1]) return g + p[3]; else return g; }", -3, 3, 4);
            fitf2->SetParameters(currhist->GetMaximum(), currhist->GetXaxis()->GetBinCenter(currhist->GetMaximumBin()), firparSigma, firparConst*currhist->Integral()/currhist->GetNbinsX());
            TFitResultPtr r = currhist->Fit(fitf2, "ls", "");
            DrawHist(currhist, profDirName + "/" + TString(hist->GetName()));
            hsigma->SetBinContent(i, fabs(r->Parameter(2)));
            hsigma->SetBinError(i, r->Error(2));
            hmean->SetBinContent(i, r->Parameter(1));
            hmean->SetBinError(i, r->Error(1));
            if (r->Parameter(1) == 0.) {
                hsigmaRel->SetBinContent(i, 0);
                hsigmaRel->SetBinError(i, 0);
            } 
            else {
                hsigmaRel->SetBinContent(i, fabs(r->Parameter(2)/r->Parameter(1)));
                Double_t widthRelerr = r->Error(2)/r->Parameter(2);
                Double_t meanRelerr = r->Error(1)/r->Parameter(1);
                Double_t ratioRelerr = sqrt(pow(widthRelerr, 2) + pow(meanRelerr, 2));
                hsigmaRel->SetBinError(i, ratioRelerr*fabs(r->Parameter(2)/r->Parameter(1)));
            }
        }
        
    }
    

    TFitResultPtr fitr = hmean->Fit(fitf, "rs");

    DrawHist(hsigma, profDirName);
    DrawHist(hsigmaRel, profDirName);
    DrawHist(hmean, profDirName);
    
    TGraphErrors* graphSigma = HistToGraph(hsigma, profDirName);
    TGraphErrors* graphSigmaRel = HistToGraph(hsigmaRel, profDirName);
    HistToGraph(hmean, profDirName);
    
    vector<pair<Double_t, Double_t> > retvec;
    if ((int)fitr != -1) {
        for (Int_t i = 0; i < fitr->NPar(); ++i) {
            retvec.push_back(make_pair(fitr->Parameter(i), fitr->Error(i)));
        } 
    }
    return DrawProfilesOutput({retvec, graphSigmaRel, graphSigma});

}

void DrawProfileX(TH2D* hist) {

    TH1D* profx = (TH1D*)hist->ProfileX((string(hist->GetName()) + "_profx").c_str());
    DrawHist(profx);

}


// Find the points of intersection of line and circle
// cx,cy,radius - center and radius of circle, point1,point2 -> signify the line, outpoint -> (output)
// return value = number of intersections
int FindLineCircleIntersections(Double_t cx, Double_t cy, Double_t radius, pair<Double_t, Double_t> point1, pair<Double_t, Double_t> point2, pair<Double_t, Double_t> & outpoint)
{
    Double_t dx, dy, A, B, C, det, t;

    dx = point2.first - point1.first;
    dy = point2.second - point1.second;

    A = dx * dx + dy * dy;
    B = 2 * (dx * (point1.first - cx) + dy * (point1.second - cy));
    C = (point1.first - cx) * (point1.first - cx) +
        (point1.second - cy) * (point1.second - cy) -
        radius * radius;

    det = B * B - 4 * A * C;
    if ((A <= 0.0000001) || (det < 0))
    {
        // No real solutions.
        outpoint = make_pair(-1., -1.);
        return 0;
    }
    else if (det == 0)
    {
        // One solution.
        t = -B / (2 * A);
        outpoint = make_pair(point1.first + t * dx, point1.second + t * dy);
        return 1;
    }
    else
    {
        Bool_t firstInside = (sqrt( (point1.first - cx)*(point1.first - cx) + (point1.second - cy)*(point1.second - cy) ) < radius);

        if (firstInside) {
            // Two solutions. (Return the "positive" one)
            t = (Double_t)((-B + TMath::Sqrt(det)) / (2 * A));
            outpoint = make_pair(point1.first + t * dx, point1.second + t * dy);
            return 2;
        }
        else {
            // Two solutions. (Return the closest one)
            Double_t tneg = (Double_t)((-B - TMath::Sqrt(det)) / (2 * A));
            Double_t tpos = (Double_t)((-B + TMath::Sqrt(det)) / (2 * A));
            pair<Double_t, Double_t> pointNeg = make_pair(point1.first + tneg*dx, point1.second + tneg*dy);
            pair<Double_t, Double_t> pointPos = make_pair(point1.first + tpos*dx, point1.second + tpos*dy);
            Double_t distNeg = sqrt( (pointNeg.first - point1.first)*(pointNeg.first - point1.first) + (pointNeg.second - point1.second)*(pointNeg.second - point1.second) );
            Double_t distPos = sqrt( (pointPos.first - point1.first)*(pointPos.first - point1.first) + (pointPos.second - point1.second)*(pointPos.second - point1.second) );
            
            if (distNeg <= distPos) {
                outpoint = pointNeg;
            }
            else {
                outpoint = pointPos;
            }
            
            return 2;
        }
    }
}


//________________________________________________________________________________
int GetXYBarrel(TVector3 mom, TVector3 vtx, Double_t radius, pair<Double_t, Double_t> & out) {
    
    int ninter = FindLineCircleIntersections(0., 0., radius, make_pair(vtx.X(), vtx.Y()), make_pair((vtx+mom).X(), (vtx+mom).Y()), out);
    return ninter;

}

//________________________________________________________________________________
Double_t GetZBarrel(TVector3 mom, TVector3 vtx, Double_t radius) {

    if (radius < 0) {
        cout << "GetZBarrel: radius not set or less than zero!" << endl;
        exit(1);
    }

    pair<Double_t, Double_t> xy;
    int ninter = GetXYBarrel(mom, vtx, radius, xy);
    if (ninter == 0) return 1e9;

    Double_t deltaR = sqrt(pow(xy.first - vtx.Z(), 2) + pow(xy.second - vtx.Y(), 2));
    
    return (mom.Pz()*deltaR/mom.Pt() + vtx.Z());

}


//________________________________________________________________________________
pair<Double_t, Double_t> GetXYEndcap(TVector3 mom, TVector3 vtx, Double_t mindist) {

    if (mindist < 0) {
        cout << "GetXYEndcap: distance to endcaps not set or less than zero!" << endl;
        exit(1);
    }
    
    Double_t ZEC = (mom.Z() > 0) ? mindist : -mindist;
    Double_t x = (ZEC - vtx.Z())*mom.Px()/mom.Pz() + vtx.X();
    Double_t y = (ZEC - vtx.Z())*mom.Py()/mom.Pz() + vtx.Y();
    return make_pair(x,y);

}


//________________________________________________________________________________
struct hist1DEfficiency {
    hist1DEfficiency(TString fname, TH1D* fpassed, TH1D* ftotal) : name(fname), passed(fpassed), total(ftotal) {}
    hist1DEfficiency() {}
    TString name;
    TH1D* passed;
    TH1D* total;
    vector<TH1D*> v_mgg;
    void Finalize() {
        if (!passed) { 
            cout << "hist1DEfficiency::Finalize() (" << name << ") : histogram \"passed\" not initialized!" << endl;
            return;
        }
        if (!total) { 
            cout << "hist1DEfficiency::Finalize() (" << name << "): histogram \"total\" not initialized!" << endl;
            return;
        }
        DrawHist(passed);
        DrawHist(total);
        TString passedName = passed->GetName();
        TH1D* hratio = (TH1D*)passed->Clone(passedName + TString("_ratio"));
        hratio->Divide(total);
        hratio->GetYaxis()->SetRangeUser(0., 1.);
        DrawHist(hratio);
    }
};

//________________________________________________________________________________
class hist1DPi0Efficiency {
    public:
        hist1DPi0Efficiency(TString fname, TString fvarname, TH1D* fpassed, TH1D* ftotal) : name(fname), varname(fvarname), passed(fpassed), total(ftotal) {
            v_mgg.clear();
            mpi0Xmin = 0.;
            mpi0Xmax = 1.0;
            mpi0Nbins = 200;
            mpi0XPeakLeft = 0.12;
            mpi0XPeakRight = 0.15;
            mpi0BinPeakLeft = mpi0Nbins*((mpi0XPeakLeft - mpi0Xmin)/(mpi0Xmax - mpi0Xmin));
            mpi0BinPeakRight = mpi0Nbins*((mpi0XPeakRight - mpi0Xmin)/(mpi0Xmax - mpi0Xmin));

            for (int i = 1; i <= passed->GetNbinsX(); ++i) {
                TString histname = "h_mgg_bin" + to_string(i);
                TString histtitle = "Two photon reconstructed mass for events with " 
                    + varname + " > " + passed->GetXaxis()->GetBinLowEdge(i) + 
                    " && " + varname + " < " + passed->GetXaxis()->GetBinUpEdge(i);
                TH1D* newhist = new TH1D(histname, histtitle, mpi0Nbins, mpi0Xmin, mpi0Xmax);
                v_mgg.push_back(newhist);
            } 
            fitpi0 = new TF1("fitpi0", "gausn(0) + [3]", 0.04, 0.28);
        }
        hist1DPi0Efficiency() {}
        void FillPassedMgg(Double_t value, Double_t mgg) {
           Int_t ibin = passed->GetXaxis()->FindBin(value); 
           Int_t idx = ibin - 1;
           if (idx < 0 || idx >= v_mgg.size()) return;
           v_mgg[idx]->Fill(mgg);
        }
        void FillTotal(Double_t value) {
            total->Fill(value);
        }

        void FillPassedHistogram() {
           for (int i = 0; i < v_mgg.size(); ++i) {
                Int_t ibin = i + 1;
                passed->SetBinContent(ibin, GetNpi0(v_mgg[i]));
           } 
        }

        Double_t GetNpi0(TH1D* hist) {
            Double_t npi0_est = hist->Integral(mpi0BinPeakLeft, mpi0BinPeakRight)/2;
            fitpi0->SetParameters(npi0_est, 0.135, 0.02, npi0_est/(mpi0BinPeakRight - mpi0BinPeakLeft));
            TFitResultPtr r = hist->Fit(fitpi0, "lsr");
            return r->Parameter(0)/hist->GetXaxis()->GetBinWidth(1);
        }
        
        void Finalize() {
            FillPassedHistogram();
            if (!passed) { 
                cout << "hist1DEfficiency::Finalize() (" << name << ") : histogram \"passed\" not initialized!" << endl;
                return;
            }
            if (!total) { 
                cout << "hist1DEfficiency::Finalize() (" << name << "): histogram \"total\" not initialized!" << endl;
                return;
            }
            for (auto h: v_mgg) {
                DrawHist(h, name + "/");
            }
            DrawHist(passed);
            DrawHist(total);
            TString passedName = passed->GetName();
            TH1D* hratio = (TH1D*)passed->Clone(passedName + TString("_ratio"));
            hratio->Divide(total);
            hratio->GetYaxis()->SetRangeUser(0., 1.);
            DrawHist(hratio);
        }
    private:
        TH1D* passed;
        TH1D* total;
        TF1* fitpi0;
        vector<TH1D*> v_mgg;
        TString name;
        TString varname;
        Double_t mpi0XPeakLeft;
        Double_t mpi0XPeakRight;
        Int_t mpi0BinPeakLeft;
        Int_t mpi0BinPeakRight;
        Int_t mpi0Nbins;
        Double_t mpi0Xmax;
        Double_t mpi0Xmin;
};


//________________________________________________________________________________
struct hist2DEfficiency {
    hist2DEfficiency(TString fname) : name(fname) {}
    TString name;
    TH2D* passed;
    TH2D* total;
    void Finalize() {
        if (!passed) { 
            cout << "hist2DEfficiency::Finalize() (" << name << "): histogram \"passed\" not initialized!" << endl;
            return;
        }
        if (!total) { 
            cout << "hist2DEfficiency::Finalize() (" << name << "): histogram \"total\" not initialized!" << endl;
            return;
        }
        DrawHist(passed, "", "colz text");
        DrawHist(total, "", "colz text");
        TString passedName = passed->GetName();
        TH2D* hratio = (TH2D*)passed->Clone(passedName + TString("_ratio"));
        hratio->Divide(total);
//        passed->GetZaxis()->SetRangeUser(0., 1.);
        DrawHist(hratio, "", "colz text");
    }
};


//________________________________________________________________________________
//how confusing is this
void DrawResolutionGraph(vector<TH1D*> hists, vector<Double_t> values, TString subdir, TString title) {

    vector<Double_t> res;
    vector<Double_t> res_err;

    TF1* f1 = new TF1("f1", "gaus(0)", -1, 1);

    for (int ih = 0; ih < hists.size(); ++ih) {
        Double_t rms = hists[ih]->GetRMS();
        Double_t mean = hists[ih]->GetMean();
        f1->SetParameters(hists[ih]->GetMaximum(), 0, rms);
        f1->SetRange(mean-3*rms, mean+3*rms);
        TFitResultPtr cr = hists[ih]->Fit(f1, "lsr");

        res.push_back(cr->Parameter(2));
        res_err.push_back(cr->Error(2));

        DrawHist(hists[ih], subdir);
    }

    TGraphErrors* gr = new TGraphErrors(hists.size(), &(values[0]), &(res[0]), 0, &(res_err[0]));
    gr->SetTitle(title);
    TString grname = subdir + "_graph";
    gr->SetName(grname);

    DrawGraph(gr, "final/");    

};


//________________________________________________________________________________
TGraphErrors* DrawRelativeResolutionGraph(vector<TH1D*> hists, vector<Double_t> values, TString subdir) {

    gStyle->SetOptFit(1);

    vector<Double_t> res;
    vector<Double_t> res_err;
    vector<Double_t> mean;
    vector<Double_t> mean_err;

    TF1* f1 = new TF1("f1", "gaus(0)", -1, 1);

    for (int ih = 0; ih < hists.size(); ++ih) {
        Double_t rms = hists[ih]->GetRMS();
        Double_t m = hists[ih]->GetMean();
        Double_t xmax = hists[ih]->GetXaxis()->GetBinCenter(hists[ih]->GetMaximumBin());

        Double_t numRMS = 1.;
        f1->SetParameters(hists[ih]->GetMaximum(), 0, rms);
        f1->SetRange(xmax-numRMS*rms, xmax+numRMS*rms);
        TFitResultPtr cr = hists[ih]->Fit(f1, "lsr");

        res.push_back(cr->Parameter(2));
        res_err.push_back(cr->Error(2));

        mean.push_back(cr->Parameter(1));
        mean_err.push_back(cr->Error(1));

        DrawHist(hists[ih], subdir);
    }

    vector<Double_t> resRel;
    vector<Double_t> resRel_err;

    for (int i = 0; i < res.size(); ++i) {
        resRel.push_back(res[i]/mean[i]);
        resRel_err.push_back(res_err[i]/mean[i]);
    }

    TGraphErrors* gr = new TGraphErrors(hists.size(), &(values[0]), &(resRel[0]), 0, &(resRel_err[0]));
    TString grname = subdir + "_graph";
    gr->SetName(grname);
    gr->SetTitle(hists[0]->GetTitle());

    TF1* fr = new TF1("fr", "TMath::Sqrt(TMath::Power([0], 2) + TMath::Power([1]/TMath::Sqrt(x), 2))", 0, 10);
    fr->SetParName(0, "Const");
    fr->SetParName(1, "Stoch");
    gr->Fit(fr, "r");

    gr->GetHistogram()->SetMinimum(0.0);
    gr->GetHistogram()->SetMaximum(0.2);
    DrawGraph(gr, "final/");    

    return gr;

};



//________________________________________________________________________________
vector<TString> GetListOfFiles(const char *dirname, const char *ext=".root", const char *extveto="abc23r2bf2f38r2r32") { 

    TSystemDirectory dir(dirname, dirname); 
    TList *files = dir.GetListOfFiles(); 

    vector<TString> vfiles;

    if (files) { 
        TSystemFile *file; 
        TString fname; 
        TIter next(files); 
        while ((file=(TSystemFile*)next())) { 
            fname = file->GetName(); 
            if (fname == "." || fname == "..") continue;
            if (!file->IsDirectory() && fname.EndsWith(ext) && !fname.EndsWith(extveto)) { 
//                cout << fname.Data() << endl; 
                vfiles.push_back(fname);
            } 
        } 
    } 

    return vfiles;
}


//________________________________________________________________________________
vector<TString> SplitString(TString x, TString delim) {
    TObjArray *tx = x.Tokenize(delim);
    vector<TString> out;
    for (Int_t i = 0; i < tx->GetEntries(); i++) {
        out.push_back(((TObjString *)(tx->At(i)))->String());
    }

    return out;
}



//________________________________________________________________________________
vector<TString> GetFilesStartingWith(const char *firstpart, const char *ext=".root", const char *extveto="abc23r2bf2f38r2r32") { 

    TString dirname = "./";
    TString localName = firstpart;

    if (TString(firstpart).Contains("/")) {
        dirname = "/";
        vector<TString> pathParsed = SplitString(TString(firstpart), "/");
        for (int i = 0; i < pathParsed.size()-1; ++i) {
            dirname += pathParsed[i] + "/";
        }
        localName = pathParsed[pathParsed.size()-1];
    }

    vector<TString> allFiles = GetListOfFiles(dirname.Data(), ext, extveto);

    vector<TString> ret;
    for (auto f: allFiles) {
        if (f.BeginsWith(localName) && f.EndsWith(ext) && !f.EndsWith(extveto)) {
            ret.push_back(dirname + f);
        }
    }

    return ret;

}








