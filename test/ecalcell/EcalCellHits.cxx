// $Id$
// Author: artur   2015/10/15


//_____________________________________________________________________________
//
// EcalCellHits
//
//_____________________________________________________________________________

#include <TH1D.h>
#include <TH2D.h>

#include "EcalCellHits.h"
#include "SpdGeopathParser.h"

ClassImp(EcalCellHits)

#include <iostream>
using std::cout;
using std::endl;
using std::pair;

//_____________________________________________________________________________
EcalCellHits::EcalCellHits():
TNamed("EcalCellHits",""),
EventId_(-1),NZ_(1),NXY_(1)
{
}

//_____________________________________________________________________________
EcalCellHits::~EcalCellHits() 
{
}

//_____________________________________________________________________________
void EcalCellHits::Clear() 
{
   EventId_ = -1;
   fTitle = "";
   Edep_.clear();
}

//*****************************************************************************
//*****************************************************************************
//*****************************************************************************
//*****************************************************************************
   
//_____________________________________________________________________________  
void EcalCellHits::SetEdep(TString node, double edep)
{
   std::map<TString,double>::iterator it = Edep_.find(node);
   if (it != Edep_.end()) it->second = edep;
   else Edep_.insert(pair<TString,Double_t>(node,edep));
}

//_____________________________________________________________________________    
void EcalCellHits::AddEdep(TString node, double edep)
{
   //cout << "-I- <EcalCellHits::AddEdep> Node: " 
   //       << node << " Edep: " << edep << endl;
  
   std::map<TString,double>::iterator it = Edep_.find(node);
   if (it != Edep_.end()) it->second += edep;
   else Edep_.insert(pair<TString,Double_t>(node,edep));
}
    
//_____________________________________________________________________________    
double EcalCellHits::GetEdep(TString node)
{
   std::map<TString,double>::iterator it = Edep_.find(node);
   return (it != Edep_.end()) ? it->second : 0;
}

//_____________________________________________________________________________
Double_t EcalCellHits::SumEdep() const
{
   if (Edep_.empty()) return 0;
   double sum(0);
   std::map<TString,double>::const_iterator it = std::begin(Edep_);
   for ( ; it != std::end(Edep_); it++) sum += it->second;
   return sum;
}
 
//_____________________________________________________________________________
TH1D* EcalCellHits::GetHZ(TH1D* h, Bool_t reset) 
{
   if (!h) h = new TH1D("(Z)","(Z)",NZ_,0,NZ_);
   else { if (reset) h->Reset(); }
   
   SpdGeopathParser parser;
   
   Int_t i(0);
   std::map<TString,double>::const_iterator it = std::begin(Edep_);
   for ( ; it != std::end(Edep_); it++) {
        parser.ParsePath(it->first);
        h->Fill(parser.Num(1),it->second);
   }
        
   return h;
}

//_____________________________________________________________________________
TH2D* EcalCellHits::GetHXY(TH2D* h, Bool_t reset) 
{
   if (!h) h = new TH2D("(XY)","(XY)",NXY_,0,NXY_,NXY_,0,NXY_);
   else { if (reset) h->Reset(); }
   
   SpdGeopathParser parser;
   
   Int_t i(0), nx, ny;

   std::map<TString,double>::const_iterator it = std::begin(Edep_);
   for ( ; it != std::end(Edep_); it++) {
        parser.ParsePath(it->first);
        GetIJ(parser.Num(3),nx,ny);
        h->Fill(nx-1,ny-1,it->second);
   }
   
   return h;
}

//_____________________________________________________________________________
TH1D* EcalCellHits::GetHZ(TH1D* h, TString vertex, Bool_t reset) 
{
   if (!h) h = new TH1D("(Z/V)","(Z/V)",NZ_,0,NZ_);
   else { if (reset) h->Reset(); }
   
   SpdGeopathParser parser;
   
   parser.ParsePath(vertex);
   Int_t shift = 5-(parser.Num(3));
 
   Int_t i(0);
   std::map<TString,double>::const_iterator it = std::begin(Edep_);
   for ( ; it != std::end(Edep_); it++) {
        parser.ParsePath(it->first);
        h->Fill(parser.Num(1)+shift,it->second);
   }
        
   return h;
}

//_____________________________________________________________________________
void EcalCellHits::Print(int opt) const 
{ 
   cout << "-I- <EcalCellHits::Print>";
   cout << " Event: " << EventId_ << " ";
   cout << " Nodes: " << Edep_.size() << " ";
   cout << " Edep(total): " << SumEdep() << " ";
   cout << " N(Z,XY): (" << NZ_ << "," << NXY_ << ") ";
   cout << endl;
   
   if (opt < 2) return;
   
   SpdGeopathParser parser;
   
   Int_t i(0);
   std::map<TString,double>::const_iterator it = std::begin(Edep_);
   for ( ; it != std::end(Edep_); it++) {
        parser.ParsePath(it->first);
        printf("%5d  %12.6e  [%4d %4d %4d]  %s\n",++i,it->second,
               parser.Num(1,true),parser.Num(2,true),parser.Num(3,true),
               it->first.Data());
   }
   cout << endl;
}




