// $Id$
// Author: artur   2015/09/26


//_____________________________________________________________________________
//
// SpdSttStrawTube
//_____________________________________________________________________________

#include "SpdSttStrawTube.h"
#include <iostream>
#include <algorithm>

using std::cout;
using std::endl;

ClassImp(SpdSttStrawTube)

//_____________________________________________________________________________
SpdSttStrawTube::SpdSttStrawTube(): NTube_(-1),
TubeHalfLength_(0), TubeRadIn_(0), TubeRadOut_(0),
NSections_(1)
{
}

//_____________________________________________________________________________
SpdSttStrawTube::~SpdSttStrawTube() 
{
  if (!Neighborings_.empty()) {
      std::vector<std::vector<Int_t>*>::iterator it = Neighborings_.begin();
      for (; it != Neighborings_.end(); it++) delete *it;
      //Neighborings_.clear();
  }
}

//_____________________________________________________________________________
void SpdSttStrawTube::SetNSections(Int_t n, Double_t* v) {
  //
  if (n < 1) {
      cout << "<SpdSttStrawTube::SetNSections> Bad number of sections: " << n << endl;
      return;
  }
 
  NSections_ = n;
  
  if (!v || NSections_ == 1) {   
      LSections_.resize(0);
      return; 
  }
  
  if (v[0] < 0 || v[n-2] > 2*TubeHalfLength_) {
      cout << "<SpdSttStrawTube::SetNSections> Bad sections values " << endl;
      return;
  }
  
  LSections_.resize(n+1);
  LSections_[0] = -TubeHalfLength_;
  LSections_[n] =  TubeHalfLength_;
  for (Int_t i(1); i<n; i++) LSections_[i] = v[i-1]-TubeHalfLength_;
}

//_____________________________________________________________________________
void SpdSttStrawTube::GetSource(Double_t& x, Double_t& y, Double_t& z) 
{ 
  x = GetXSource(); 
  y = GetYSource(); 
  z = GetZSource(); 
}

//_____________________________________________________________________________
void SpdSttStrawTube::GetSectionSource(int n, Double_t& x, Double_t& y, Double_t& z) 
{
  x = GetXSource(); 
  y = GetYSource(); 
  z = GetSectionSource(n);
}

//_____________________________________________________________________________
Double_t SpdSttStrawTube::GetSectionSource(int n) 
{
  if (NSections_ == 1 || n >= NSections_ || n < 0) return GetZSource();
  if (LSections_.size() != 0) return 0.5*(LSections_[n]+LSections_[n+1])+GetZSource();
  return TubeHalfLength_*(2*n-NSections_+1)/NSections_+GetZSource();
}

//_____________________________________________________________________________
Double_t SpdSttStrawTube::GetSectionHalfLength(int n) 
{
  if (NSections_ == 1) return TubeHalfLength_;
  if (n >= NSections_ || n < 0) return -1.;
  if (LSections_.size() != 0) return 0.5*(LSections_[n+1]-LSections_[n]);
  return TubeHalfLength_/NSections_;
}

//_____________________________________________________________________________
std::vector<Int_t>* SpdSttStrawTube::GetNeighboring(Int_t i) 
{ 
   if (i < 0) return 0;
   if (i < Neighborings_.size() &&  Neighborings_[i] && Neighborings_[i]->size() > 0) 
       return Neighborings_[i];
   if (MakeNeighboring(i)) return Neighborings_[i];
   return 0;
}

//_____________________________________________________________________________
void SpdSttStrawTube::ClearNeighborings() 
{
  if (!Neighborings_.empty()) {
      std::vector<std::vector<Int_t>*>::iterator it = Neighborings_.begin();
      for (; it != Neighborings_.end(); it++) (*it)->clear();
  }
}

//_____________________________________________________________________________
void SpdSttStrawTube::ClearNeighboring(Int_t i)
{
  if (!Neighborings_.empty() && i > 0 && i < Neighborings_.size() && Neighborings_[i]) 
       Neighborings_[i]->clear();
}

//_____________________________________________________________________________
void SpdSttStrawTube::print(Int_t opt) 
{
  cout << "<SpdSttStrawTube::print>" << endl;
  cout << "Tube number = " << NTube_ << endl;
  cout << "Tube half length  = " << TubeHalfLength_ << " cm " << endl;
  cout << "Tube inner radius = " << TubeRadIn_ << " cm " << endl;
  cout << "Tube outer radius = " << TubeRadOut_ << " cm " << endl;
  cout << "Tube sections     = " << NSections_ << endl;
}

//_____________________________________________________________________________
void SpdSttStrawTube::print_sections() 
{
  if (NSections_ == 1) {
      cout << "[ 0 ] ";
      return;
  }
  
  cout << " [ ";
  for (Int_t i(0); i<NSections_; i++) {
       cout << GetSectionSource(i) << "("<< 2*GetSectionHalfLength(i) << ") ";
  }
  cout << "] ";
  
}


