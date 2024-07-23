
//#define _COMPILE_MACRO_

#if defined(_COMPILE_MACRO_) 

#include <TRint.h>
#include <TStyle.h>
#include <TMath.h>
#include <TCanvas.h>
#include <TH2D.h>
#include "SpdFields.hh"

#endif

void DrawHistos(Int_t ni1 = 5, Int_t ni2 = -2);

void CheckPhiSym(Int_t ni1 = 5);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Draw-functions parameters: 
//
// Z [cm]:     XY plane coordinate;
// Phi [deg]:  polar angle in XY-plane;
// fvtype:     field value type: B, Bz, By, Bx, BT = SQRT(Bx*Bx+By*By), BL = |Bz|;
// in:         binning: in = 0 => nb = 1,
//                      in > 0 => nb->nb*in,
//                      in < 0 => nb->nb/in;
// rcut [cm]:  radial cut (don't fill histo if r > rcut).
// zcut [cm]:  Z-size cut (don't fill histo if |z| > zcut).
//-------------------------------------------------------------------

TH2D* MakeZPlane(Double_t Z, TString fvtype, Int_t in = 1, Double_t rcut = 1000., TH1D* hh = 0);
TH2D* MakePhiPlane(Double_t Phi, TString fvtype, Int_t in = 1, Double_t rcut = 1000., TH1D* hh = 0);
TH2D* MakeRPlane(Double_t R, TString fvtype, Int_t in = 1, Double_t zcut = 1000., TH1D* hh = 0);
TH2D* MakePhiAveraged(Double_t Rmax, TString fvtype, Int_t in = 1, Double_t zcut = 1000., TH1D* hh = 0);
TH2D* MakeZAveraged(Double_t Zmax, TString fvtype, Int_t in = 1, Double_t rcut = 1000., TH1D* hh = 0);


#define AXIAL_FIELD // !!!FIXME!!! FIXME!!! FIXME!!!


TString      FieldMap_;

#ifndef AXIAL_FIELD
SpdFieldMap* MagField_ = 0;
#else
SpdAxialFieldMap* MagField_ = 0;
#endif

TString GetFtype(TString fvtype);

//--------------------------------------------------------------------------
void SetStyle()
{
    gStyle->SetPalette(53);
    //gStyle->SetPalette(56);
    //gStyle->SetPalette(51);
  
    gStyle->SetOptStat(0);
    
    gStyle->SetLabelSize(0.025,"X");
    gStyle->SetLabelSize(0.025,"Y");
    gStyle->SetLabelSize(0.025,"Z");
    
    gStyle->SetTitleH(0.035);
    gStyle->SetTitleY(0.97);
    
    //gStyle->SetPadGridX(true);
    //gStyle->SetPadGridY(true);
}

//__________________________________________________________________________
void DrawFieldMap() 
{  
    
#ifndef AXIAL_FIELD  
    
    //FieldMap_ = "map_tor_rot.bin";       // toroid (rotated), grid = 5 cm
    //FieldMap_ = "map_hyb_1T2cm.bin";     // hybrid HQ, B = 1 T,  grid = 2 cm
    //FieldMap_ = "map_hyb_1T5cm.bin";     // hybrid LQ, B = 1 T,  grid = 5 cm 
    //FieldMap_ = "map_hyb_2T5cm.bin";     // hybrid LQ, B = 2 T,  grid = 5 cm
    //FieldMap_ = "map_sol_6cls5cm.bin";   // qsolenoid [3+3-],  grid = 5 cm
    FieldMap_ = "map_sol_6cls5cm2.bin";  // qsolenoid [6+],    grid = 5 cm
      
    MagField_ = new SpdFieldMap("Hybrid field");
    MagField_->InitData(FieldMap_);
    MagField_->PrintData();
    
    Double_t Bstat[10];
    MagField_->GetMinMaxStat(Bstat);
    
    cout << "\t Field map statistics (nodes): " << "\n\n"; 
    cout << "\t B  (min,max) = " << Bstat[0] << " " << Bstat[1] << " [kG] " << endl;
    cout << "\t BT (min,max) = " << Bstat[2] << " " << Bstat[3] << " [kG] " << endl;
    cout << "\t Bz (min,max) = " << Bstat[4] << " " << Bstat[5] << " [kG] " << endl;
    cout << "\t By (min,max) = " << Bstat[6] << " " << Bstat[7] << " [kG] " << endl;
    cout << "\t Bx (min,max) = " << Bstat[8] << " " << Bstat[9] << " [kG] " << endl;
    
    cout << endl;

#else    
    
    FieldMap_ = "map_qsolRZ_6cls2cm.bin"; // qsolenoid [6+],    grid = 2 cm
 
    MagField_ = new SpdAxialFieldMap("QSolenoidal field");
    MagField_->InitData(FieldMap_);  
    MagField_->PrintData();
     
    Double_t Bstat[8];
    MagField_->GetMinMaxStat(Bstat);
    
    cout << "\t Field map statistics (nodes): " << "\n\n"; 
    cout << "\t B  (min,max) = " << Bstat[0] << " " << Bstat[1] << " [kG] " << endl;
    cout << "\t Bz (min,max) = " << Bstat[2] << " " << Bstat[3] << " [kG] " << endl;
    cout << "\t Br (min,max) = " << Bstat[4] << " " << Bstat[5] << " [kG] " << endl;
    
    cout << endl;
 
#endif
    
    SetStyle();
        
    DrawHistos();
    
    //CheckPhiSym();
}

//__________________________________________________________________________
void DrawHistos(Int_t ni1, Int_t ni2) 
{  
    TCanvas* cRa1 = new TCanvas("cRa1","cRa1",1000,800);
   
    TH2D* hRa1 = MakePhiAveraged(250, "B", ni1, 1000);
    //hRa1->SetMaximum(15.);
    //hRa1->SetMinimum(-15.);
    hRa1->Draw("colz");
    
    TCanvas* cZa1 = new TCanvas("cZa1","cZa1",800,800);
    
    TH2D* hZa1 = MakeZAveraged(290, "B", ni1, 220);
    //hZa1->SetMaximum(15.);
    //hZa1->SetMinimum(-15.);
    hZa1->Draw("colz");
  
//return;  

    TCanvas* cZ1 = new TCanvas("cZ1","cZ1",800,800);
       
    TH2D* hZ1 = MakeZPlane(0.,"B", ni1, 220.); 
    //hZ1->SetMaximum(15.);
    //hZ1->SetMinimum(-15.);
    hZ1->Draw("colz");
 
    TCanvas* cZ2 = new TCanvas("cZ2","cZ2",800,800);
    
    TH2D* hZ2 = MakeZPlane(-75.,"Bz", ni2, 220); 
    //hZ2->SetMaximum(15.);
    //hZ2->SetMinimum(-15.);
    hZ2->Draw("surf1");
    
    TCanvas* cPhi1 = new TCanvas("cPhi1","cPhi1",930,800);
       
    TH2D* hPhi1 = MakePhiPlane(0,"B", ni1, 1000.); 
    //hPhi1->SetMaximum(2.);
    //hPhi1->SetMinimum(-2.);
    hPhi1->Draw("colz");
    
//return;    
       
    TCanvas* cPhi2 = new TCanvas("cPhi2","cPhi2",930,800);
    
    TH2D* hPhi2 = MakePhiPlane(22.5,"B", ni1, 220.); 
    //hPhi2->SetMaximum(2.);
    //hPhi2->SetMinimum(-2.);
    hPhi2->Draw("colz");
    
    TCanvas* cR1 = new TCanvas("cR1","cR1",1000,800);
     
    TH2D* hR1 = MakeRPlane(75.,"B", ni1, 300.); 
    //hR1->SetMaximum(2.);
    //hR1->SetMinimum(-2.);
    hR1->Draw("colz");
    
    TCanvas* cR2 = new TCanvas("cR2","cR2",1000,800);
    cR2->cd();
   
    TH2D* hR2 = MakeRPlane(100.,"Bz", ni1, 300.); 
    //hR2->SetMaximum(2.);
    //hR2->SetMinimum(-2.);
    hR2->Draw("colz");
}

//__________________________________________________________________________
void CheckPhiSym(Int_t ni1) 
{
    TString vtype = "Bz";
    
    //Double_t pp = 48.578; // an arbitrary value
    Double_t pp = 0; // an arbitrary value
    
    const int N = 4;
    
    Double_t phi[N] = {pp, pp+90, pp+180, pp+270};
    //Double_t phi[N] = {pp, pp+57, pp+134, pp+242};
          
    TCanvas* cc[N];
    TH1D* hh[N];
    TH2D* hp[N]; 
    
    TH1D* hha = 0;
    TH2D* hpa = 0;
    
    Int_t cols[N] = {kRed,kBlue,kGreen,kOrange};
    
    TCanvas* ccc = new TCanvas("ccc","ccc",800,600);

    for (Int_t i(0); i<N; i++) {
         
         hh[i] = new TH1D(Form("h%d",i+1),Form("h%d",i+1),120,-20,20);
         hp[i] = MakePhiPlane(phi[i], vtype, ni1, 1000., hh[i]); 
         
         cc[i] = new TCanvas(Form("cc%d",i+1),Form("cc%d",i+1),800,600);  
         hp[i]->Draw("colz");
        
         ccc->cd(0);
          
         if (i == 0) {
             hha = new TH1D("hha",Form("Field %s^{(a)}",GetFtype(vtype).Data()),120,-20,20);
             hpa = MakePhiAveraged(250, vtype, ni1, 1000, hha);
             hha->SetLineColor(kBlack);
             hha->SetFillColor(kGray);
             //hha->SetLineWidth(12);
             hha->Draw();
         }
         
         hh[i]->SetLineColor(cols[i]);
         hh[i]->SetLineWidth(2*(4-i));         
                
         hh[i]->Draw("same");
    }
 
    ccc->SetLogy();
    
    if (hpa) {
        TCanvas* ca = new TCanvas("ca","ca",800,600);
        hpa->Draw("colz");
    }
}

//__________________________________________________________________________
TH2D* MakeZPlane(Double_t Z, TString fvtype, Int_t in, Double_t rcut, TH1D* hh) 
{
    Double_t nn;
 
    if (in == 0) nn = 1;
    else nn = (in > 0) ? in : 1./TMath::Abs(in);
    
    cout << "-I- <MakeZPlane> (Z,type,in,rcut): " 
         << Z << " " << fvtype << " " << in << " " << rcut << endl;
    
    Double_t xmin = MagField_->GetXmin(), xmax = MagField_->GetXmax();
    Double_t ymin = MagField_->GetYmin(), ymax = MagField_->GetYmax();
    
    Int_t nx = (MagField_->GetNx()-1)*nn;
    Int_t ny = (MagField_->GetNy()-1)*nn;
  
    Double_t xstep = MagField_->GetXstep()/nn;
    Double_t ystep = MagField_->GetYstep()/nn;
    
    Double_t x0 = xmin-0.5*xstep;
    Double_t y0 = ymin-0.5*ystep;
    
    // prepare histo
    
    TString hname = Form("%s,z,%s,%-10.6f,%d,%-10.6f",FieldMap_.Data(),GetFtype(fvtype).Data(),Z,in,rcut);
    TString htitle = Form("Field %s [kG],  Z = %-6.3f [cm]",GetFtype(fvtype).Data(),Z);
    
    TH2D* h = new TH2D(hname,htitle,nx,xmin,xmax,ny,ymin,ymax);
    
    h->GetXaxis()->SetTitle("X  [cm]");
    h->GetYaxis()->SetTitle("Y  [cm]");
    
    h->GetYaxis()->SetTitleOffset(1.2);
    
    cout << "-I- <MakeZPlane> [nbins(x), nbins(y)]: " << nx << "," << ny << endl;
    
    // fill histo 
    
    Double_t x, y, z, f;
    Int_t n1(0), n2(0);
    
    z = Z;
    y = y0;
    
    Double_t r; 
    
    for (Int_t j(0); j<ny; j++) 
    {
         y += ystep;
         x = x0;
         
         for (Int_t i(0); i<nx; i++) 
         {
            x += xstep;
            f = MagField_->Value(x,y,z,fvtype);
            
            n1++;          
            r = TMath::Sqrt(x*x+y*y);
            if (r > rcut) continue;
            n2++;
            
            h->Fill(x,y,f);
            if (hh) hh->Fill(f);
         }
    }

    cout << "-I- <MakeZPlane> N(entries) = " << n2 << "/" << n1 << "\n\n";
    
    return h;
}

//__________________________________________________________________________
TH2D* MakePhiPlane(Double_t Phi, TString fvtype, Int_t in, Double_t rcut, TH1D* hh) 
{
    Double_t nn;
 
    if (in == 0) nn = 1;
    else nn = (in > 0) ? in : 1./TMath::Abs(in);
   
    cout << "-I- <MakePhiPlane> (Phi,type,in,rcut): " 
         << Phi << " " << fvtype << " " << in << " " << rcut << endl;
    
    Double_t xmin = MagField_->GetXmin(), xmax = MagField_->GetXmax();
    Double_t zmin = MagField_->GetZmin(), zmax = MagField_->GetZmax();
    
    Int_t nx = (MagField_->GetNx()-1)*nn;
    Int_t nz = (MagField_->GetNz()-1)*nn;
       
    Double_t xstep = MagField_->GetXstep()/nn;
    Double_t zstep = MagField_->GetZstep()/nn;
    
    Double_t x0 = xmin-0.5*xstep;
    Double_t z0 = zmin-0.5*zstep;
       
    // prepare histo
    
    TString hname = Form("%s,phi,%s,%-10.6f,%d,%-10.6f",FieldMap_.Data(),GetFtype(fvtype).Data(),Phi,in,rcut);
    TString htitle = Form("Field %s [kG],  #phi = %-6.2f [deg]",GetFtype(fvtype).Data(),Phi);
    
    TH2D* h = new TH2D(hname,htitle,nz,zmin,zmax,nx,xmin,xmax);
    
    h->GetXaxis()->SetTitle("Z  [cm]");
    h->GetYaxis()->SetTitle("R  [cm]");
    
    h->GetYaxis()->SetTitleOffset(1.2);
    
    cout << "-I- <MakePhiPlane> [nbins(z), nbins(x)]: " << nz << "," << nx << endl;
    
    // fill histo 
    
    Double_t x, y, z, f;
    Int_t n1(0), n2(0);
    
    Double_t yy = 0;
    Double_t xx = x0;
         
    Double_t ss = TMath::Sin(TMath::DegToRad()*Phi);
    Double_t cc = TMath::Cos(TMath::DegToRad()*Phi);
    
    Double_t r;
       
    for (Int_t i(0); i<nx; i++) 
    {
         xx += xstep;
 
         x = cc*xx - ss*yy;
         y = ss*xx + cc*yy;
         z = z0;
              
         for (Int_t k(0); k<nz; k++) 
         {
              z += zstep;
 
              f = MagField_->Value(x,y,z,fvtype);

              n1++;               
              r = TMath::Abs(xx);
              if (r > rcut) continue;
              n2++;
        
              h->Fill(z,xx,f);
              if (hh) hh->Fill(f);
              
         }
    }
    cout << "-I- <MakePhiPlane> N(entries) = " << n2 << "/" << n1 << "\n\n";
    
    return h;
}

//__________________________________________________________________________
TH2D* MakeRPlane(Double_t R, TString fvtype, Int_t in, Double_t zcut, TH1D* hh)
{
    Double_t nn;
 
    if (in == 0) nn = 1;
    else nn = (in > 0) ? in : 1./TMath::Abs(in);
    
    cout << "-I- <MakeRPlane> (R,type,in,zcut): " 
         << R << " " << fvtype << " " << in << " " << zcut << endl;
     
    Double_t zmin  = MagField_->GetZmin(), zmax = MagField_->GetZmax();
    Int_t    nz    = (MagField_->GetNz()-1)*nn;    
    Double_t zstep = MagField_->GetZstep()/nn;
    Double_t z0    = zmin-0.5*zstep;
    
    Double_t pmin  = 0;
    Double_t pmax  = 360;
    Double_t npd   = TMath::TwoPi()*R/zstep;
    Int_t    np    = ((npd-Int_t(npd)*zstep) < (Int_t(npd+1)*zstep-npd)) ? Int_t(npd) : Int_t(npd+1);     
    Double_t pstep = 360./np;
    Double_t p0    = pmin-0.5*pstep;
    
    // prepare histo
    
    TString hname = Form("%s,R,%s,%-10.6f,%d,%-10.6f",FieldMap_.Data(),GetFtype(fvtype).Data(),R,in,zcut);
    TString htitle = Form("Field %s [kG],  R = %-6.3f [cm]",GetFtype(fvtype).Data(),R);
    
    TH2D* h = new TH2D(hname,htitle,nz,zmin,zmax,np,pmin,pmax);
    
    h->GetXaxis()->SetTitle("Z  [cm]");
    h->GetYaxis()->SetTitle("#phi  [deg]");
    
    h->GetYaxis()->SetTitleOffset(1.2);
    
    cout << "-I- <MakeRPlane> [nbins(z), nbins(phi)]: " << nz << "," << np 
         << "; step(z [cm], r[cm], phi[deg]): " 
         << "(" << zstep << " " << TMath::TwoPi()*R/np << " " << pstep << ") " << endl;
    
    
    // fill histo 
         
    Double_t x, y, z, f;
    Int_t n1(0), n2(0);
          
    z = z0;
    
    Double_t phi;
     
    for (Int_t k(0); k<nz; k++) 
    {
         z += zstep;
        
         if (TMath::Abs(z) > zcut) { n1+= np; continue; }
 
         phi = p0;
         for (Int_t i(0); i<np; i++) 
         {
              phi += pstep;
              
              x = R*TMath::Cos(phi*TMath::DegToRad());
              y = R*TMath::Sin(phi*TMath::DegToRad());
              
              f = MagField_->Value(x,y,z,fvtype);
              
              h->Fill(z,phi,f);
              if (hh) hh->Fill(f);
               
              n1++; n2++;         
         }
    }
     
    cout << "-I- <MakeRPlane> N(entries) = " << n2 << "/" << n1 << "\n\n";
    
    return h;
}

//__________________________________________________________________________
TH2D* MakePhiAveraged(Double_t Rmax, TString fvtype, Int_t in, Double_t zcut, TH1D* hh)
{
    Double_t nn;
 
    if (in == 0) nn = 1;
    else nn = (in > 0) ? in : 1./TMath::Abs(in);
    
    Rmax = TMath::Abs(Rmax);
    
    cout << "-I- <MakePhiAveraged> (Rmax,type,in,zcut): " 
         << Rmax << " " << fvtype << " " << in << " " << zcut << endl;
     
    Double_t zmin  = MagField_->GetZmin(), zmax = MagField_->GetZmax();
    Int_t    nz    = (MagField_->GetNz()-1)*nn;    
    Double_t zstep = MagField_->GetZstep()/nn;
    Double_t z0    = zmin-0.5*zstep;
    
    Double_t nrd   = Rmax/zstep;
    Int_t    nr    = ((nrd-Int_t(nrd)*zstep) < (Int_t(nrd+1)*zstep-nrd)) ? Int_t(nrd) : Int_t(nrd+1);   
    Double_t rstep = Rmax/nr;
    
    // prepare histo
    
    TString hname = Form("%s,Rmax,%s,%-10.6f,%d,%-10.6f",FieldMap_.Data(),GetFtype(fvtype).Data(),Rmax,in,zcut);
    TString htitle = Form("Field %s^{(phi)} [kG] ",GetFtype(fvtype).Data());
    
    TH2D* h = new TH2D(hname,htitle,nz,zmin,zmax,2*nr,-Rmax,Rmax);
    
    h->GetXaxis()->SetTitle("Z  [cm]");
    h->GetYaxis()->SetTitle("R  [cm]");
    
    h->GetYaxis()->SetTitleOffset(1.2);
    
    cout << "-I- <MakePhiAveraged> [nbins(z), nbins(r)]: " << nz << "," << 2*nr 
         << "; step(z [cm], r[cm]): " 
         << "(" << zstep << " " << rstep << ") " << endl;
         
    // fill histo 
         
    Double_t x, y, z, f;
    Int_t n1(0), n2(0);     
       
    Double_t npd, pstep, phi;
    Int_t np;
    
    Double_t r = -0.5*rstep; 
    
    Double_t step = 2*MagField_->GetZstep(); // ATTENTION
    
    for (Int_t j(0); j<nr; j++) 
    {
         r += rstep;
        
         npd   = TMath::Pi()*r/step;
         np    = ((npd-Int_t(npd)*step) < (Int_t(npd+1)*step-npd)) ? Int_t(npd) : Int_t(npd+1);
         
         if (np < 4) np = 4; // ATTENTION
         
         pstep = 180./np;
         
         cout << "-I- <MakePhiAveraged> r = " << r << "/" << Rmax << ", points(phi) = " << np 
              << " d(phi) = " << pstep << ", dl = " <<  TMath::Pi()*r/np 
              << "\r";
              //<< endl;
              
         z = z0;
         
         for (Int_t k(0); k<nz; k++) 
         {
              z += zstep;
        
              n1 += 2;
              if (TMath::Abs(z) > zcut) continue; 
                                   
              phi = -0.5*pstep;
              f = 0;                               
              for (Int_t i(0); i<np; i++) 
              {
                   phi += pstep;        
                   x = r*TMath::Cos(phi*TMath::DegToRad());
                   y = r*TMath::Sin(phi*TMath::DegToRad());
                   f += MagField_->Value(x,y,z,fvtype);                
              }       
              f /= Double_t(np);
                     
              h->Fill(z,r,f);  
              if (hh) hh->Fill(f);
              
              n2++;
         
              //cout << "[r+, z] = [" << r << ", " << z << "] f = " << f << " n = " << np << endl;
        
              phi = 0.5*pstep;
              f = 0;             
              for (Int_t i(0); i<np; i++) 
              {
                   phi -= pstep;        
                   x = r*TMath::Cos(phi*TMath::DegToRad());
                   y = r*TMath::Sin(phi*TMath::DegToRad());
                   f += MagField_->Value(x,y,z,fvtype);     
              }             
              f/= Double_t(np);        
                
              h->Fill(z,-r,f);
              if (hh) hh->Fill(f);
              
              n2++;
           
              //cout << "[r+, z] = [" << r << ", " << z << "] f = " << f << " n = " << np << endl;                         
         }      
    }
        
    cout << endl;     
    cout << "-I- <MakePhiAveraged> N(entries) = " << n2 << "/" << n1 << "\n\n";  

    return h;
}

//__________________________________________________________________________
TH2D* MakeZAveraged(Double_t Zmax, TString fvtype, Int_t in, Double_t rcut, TH1D* hh) 
{
    Double_t nn;
 
    if (in == 0) nn = 1;
    else nn = (in > 0) ? in : 1./TMath::Abs(in);
    
    Zmax = TMath::Abs(Zmax);
    
    cout << "-I- <MakeZAveraged> (Zmax,type,in,rcut): " 
         << Zmax << " " << fvtype << " " << in << " " << rcut << endl;
    
    Double_t xmin = MagField_->GetXmin(), xmax = MagField_->GetXmax();
    Double_t ymin = MagField_->GetYmin(), ymax = MagField_->GetYmax();
    
    Int_t nx = (MagField_->GetNx()-1)*nn;
    Int_t ny = (MagField_->GetNy()-1)*nn;
          
    Double_t xstep = MagField_->GetXstep()/nn;
    Double_t ystep = MagField_->GetYstep()/nn;
    
    Double_t x0 = xmin-0.5*xstep;
    Double_t y0 = ymin-0.5*ystep;
    
    Double_t zstep = MagField_->GetZstep();
    Double_t nzd   = 2*Zmax/zstep;
    Int_t    nz    = ((nzd-Int_t(nzd)*zstep) < (Int_t(nzd+1)*zstep-nzd)) ? Int_t(nzd) : Int_t(nzd+1);   
        
    zstep = 2*Zmax/nz;  
    
    Double_t z0 = -Zmax-0.5*zstep;
  
    // prepare histo
    
    TString hname = Form("%s,z,%s,%-10.6f,%d,%-10.6f",FieldMap_.Data(),GetFtype(fvtype).Data(),Zmax,in,rcut);
    TString htitle = Form("Field %s^{(z)} [kG], |Z|_{max} = %-6.3f [cm]",GetFtype(fvtype).Data(),Zmax);
    
    TH2D* h = new TH2D(hname,htitle,nx,xmin,xmax,ny,ymin,ymax);
    
    h->GetXaxis()->SetTitle("X  [cm]");
    h->GetYaxis()->SetTitle("Y  [cm]");
    
    h->GetYaxis()->SetTitleOffset(1.2);
    
    cout << "-I- <MakeZAveraged> [nbins(x), nbins(y), nbins(z)]: " << nx << "," << ny << " " << nz 
         << "; step(x,y,z) [cm]: " << "(" << xstep << " " << ystep << " " << zstep << ") " << endl;
    
    // fill histo 
    
    Double_t x, y, z, f;
    Int_t n1(0), n2(0);
    
    y = y0;
    
    Double_t r; 
    
    for (Int_t j(0); j<ny; j++) 
    {
         y += ystep;
         x = x0;
         
         for (Int_t i(0); i<nx; i++) 
         {
            x += xstep;
                    
            n1++; 
            
            r = TMath::Sqrt(x*x+y*y);           
            if (r > rcut) continue;
            
            cout << "-I- <MakeZAveraged> l = " << y << "/" << rcut << " " 
                 << "\r";
               //<< endl;
            
            z = z0;
            f = 0;
            for (Int_t k(0); k<nz; k++) {
                 z += zstep;
                 f += MagField_->Value(x,y,z,fvtype);
            }
            f /= Double_t(nz);
            
            h->Fill(x,y,f);
            if (hh) hh->Fill(f);
            
            n2++;
         }
    }
    
    cout << endl;
    cout << "-I- <MakeZAveraged> N(entries) = " << n2 << "/" << n1 << "\n\n";
    
    return h;
}

//__________________________________________________________________________
TString GetFtype(TString fvtype)
{
    if (fvtype == "B")  return "|B|";
    if (fvtype == "Bz") return "B_{z}";
    if (fvtype == "BT") return "B_{T}";
    if (fvtype == "Bx") return "B_{x}";
    if (fvtype == "By") return "B_{y}";
    if (fvtype == "BL") return "B_{L}";
    return "";
}
