// $Id$
// Author: artur   2016/01/25


//_____________________________________________________________________________
//
// SpdSttStrawTubeA
//_____________________________________________________________________________

#include "SpdSttStrawTubeA.h"
#include <TMath.h>
#include <iostream>
#include <algorithm>

using std::cout;
using std::endl;

ClassImp(SpdSttStrawTubeA)

//_____________________________________________________________________________
SpdSttStrawTubeA::SpdSttStrawTubeA():SpdSttStrawTube(),nr_(1),ns_(0),nt_(0),Z_(0)
{   
    double c = 0.5*TMath::Sqrt(3);
    
    V_[0][0] =  1.0;  V_[0][1] =  0;
    V_[1][0] =  0.5;  V_[1][1] =  c;
    V_[2][0] = -0.5;  V_[2][1] =  c;
    V_[3][0] = -1.0;  V_[3][1] =  0;
    V_[4][0] = -0.5;  V_[4][1] = -c;
    V_[5][0] =  0.5;  V_[5][1] = -c;
    
    K_[0][0] = -0.5;  K_[0][1] =  c; // => V_[2]
    K_[1][0] = -1.0;  K_[1][1] =  0; // => V_[3]
    K_[2][0] = -0.5;  K_[2][1] = -c; // => V_[4]
    K_[3][0] =  0.5;  K_[3][1] = -c; // => V_[5]
    K_[4][0] =  1.0;  K_[4][1] =  0; // => V_[0]
    K_[5][0] =  0.5;  K_[5][1] =  c; // => V_[1]  
}

//_____________________________________________________________________________
void SpdSttStrawTubeA::Scale(Double_t d) 
{
    for (int i(0); i<6; i++) {
         V_[i][0] *= d; 
         V_[i][1] *= d;
         K_[i][0] *= d; 
         K_[i][1] *= d;
    }
}

//_____________________________________________________________________________
void SpdSttStrawTubeA::SetNTube(Int_t ntube) 
{
    if (ntube == NTube_) return;
    
    NTube_ = ntube;
     
    nr_ = 0.5*(TMath::Sqrt(1+4*(NTube_-1)/3.)+1); // number of ring in the section  
    Int_t nn = NTube_-3*nr_*(nr_-1)-1;            // local numer of tube in the ring (0,...,6*nr_-1)
    ns_ = nn/nr_;                                 // number of sector
    nt_ = nn-ns_*nr_;                             // number of tube in the sector   
}

//_____________________________________________________________________________
void SpdSttStrawTubeA::GetSource(Double_t& x, Double_t& y, Double_t& z) 
{
    x = nr_ * V_[ns_][0] + nt_ * K_[ns_][0];
    y = nr_ * V_[ns_][1] + nt_ * K_[ns_][1];
    z = Z_;
}
    
//_____________________________________________________________________________
Double_t SpdSttStrawTubeA::GetXSource() 
{ 
    return nr_ * V_[ns_][0] + nt_ * K_[ns_][0];
}

//_____________________________________________________________________________
Double_t SpdSttStrawTubeA::GetYSource() 
{ 
    return nr_ * V_[ns_][1] + nt_ * K_[ns_][1];
}

//_____________________________________________________________________________
Double_t SpdSttStrawTubeA::RadialDist() 
{
    Double_t x = GetXSource();
    Double_t y = GetYSource();
    return TMath::Sqrt(x*x+y*y);
}

//_____________________________________________________________________________
Double_t SpdSttStrawTubeA::RadialDistPlus() 
{
    return RadialDist()+TubeRadOut_;
}

//_____________________________________________________________________________
Double_t SpdSttStrawTubeA::RadialDistMinus() 
{
    return RadialDist()-TubeRadOut_;
}
    

//_____________________________________________________________________________
Bool_t SpdSttStrawTubeA::MakeNeighboring(Int_t i) 
{ 
   if (i != 0) return kFALSE;
   if (NTube_ < 1) return kFALSE;
  
   std::vector<Int_t>* v = 0;
   
   if (i < Neighborings_.size()) {
       v = Neighborings_[i];
       v->clear();
   }
   else { 
       Neighborings_.resize(i+1,0);
       v = new std::vector<Int_t>;  
       v->assign(6,0);
       Neighborings_[i] = v;
       
   }
 
   std::vector<Int_t>& nn = *v; 
   
   Int_t Nnext = 6*(nr_+1); 
   Int_t Nthis = 6*nr_;     
   Int_t Nprev = 6*(nr_-1); 
 
   if (IsVertex()) {
     
       if (nr_ == 1) nn[0] = 0;
       else nn[0] = NTube_-Nprev-ns_;
       
       nn[1] = NTube_+1;       
       nn[3] = NTube_+Nthis+ns_;
       nn[4] = NTube_+Nthis+ns_+1;
         
       if (ns_ != 0) {
          nn[2] = NTube_-1;
          nn[5] = NTube_+Nthis+ns_-1; 
       }
       else {
          nn[2] = NTube_+Nthis-1;
          nn[5] = NTube_+Nthis+Nnext-1; 
       }
   }
   else {
       nn[0] = NTube_-Nprev-ns_-1;
       nn[1] = NTube_-Nprev-ns_;
       nn[2] = NTube_+1;
       nn[3] = NTube_-1;
       nn[4] = NTube_+Nthis+ns_;
       nn[5] = NTube_+Nthis+ns_+1; 
   }
   
   std::sort(nn.begin(),nn.end());
   
   return kTRUE;  
}

//_____________________________________________________________________________
void SpdSttStrawTubeA::print(Int_t opt) 
{
  cout << "<SpdSttStrawTubeA::print[" << opt << "]> " << endl;
  
  cout << "Tube half length  = " << TubeHalfLength_ << " cm " << endl;
  cout << "Tube inner radius = " << TubeRadIn_ << " cm " << endl;
  cout << "Tube outer radius = " << TubeRadOut_ << " cm " << endl;
  cout << "Tube sections     = " << NSections_; 
  print_sections();
  cout << endl;
//        if (LSections_.size() == 0) cout << " (EQ: " << 0 << ") " << endl;
//        else cout << "(EQ: " << 1 << ") " << endl;
  cout << "Distance between tubes = " <<  GetScaleFactor() << " cm " << endl;
  cout << "Tube position: (" << GetXSource() << "," << GetYSource() << "," << GetZSource() << ") cm" << endl;
  
  cout << "Tube number = " << NTube_ << endl;
  
  if (NTube_ > 0) {
      cout << "Tube [Ring,Sector,Num] = " << nr_ << ", " << ns_ << ", "  << nt_ << endl;
  }
  
  if (opt == 1) {
      if (!Neighborings_.empty() && Neighborings_[0] && Neighborings_[0]->size() > 0) {
          std::vector<Int_t>& v = *Neighborings_[0]; 
          Int_t i(0);
          cout << "Tube neighborings [1]: "; 
          for (; i<v.size();) {
               cout << v[i];
               if (++i % 15 == 0) cout << "\n";
               else cout << " ";
          }
          if (i % 15 != 0) cout << endl;
      }
  }
}


 
