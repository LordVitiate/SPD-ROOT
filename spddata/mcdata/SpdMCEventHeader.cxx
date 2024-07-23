// $Id$
// Author: artur   2016/01/10


//_____________________________________________________________________________
//
// SpdMCEventHeader
//_____________________________________________________________________________

#include "FairRootManager.h"

#include <TObjArray.h>

#include "SpdPrimGenData.h"
#include "SpdMCEventHeader.h"

#include <iostream>

using std::cout;
using std::endl;

using std::vector;

ClassImp(SpdMCEventHeader)

//_____________________________________________________________________________
SpdMCEventHeader::SpdMCEventHeader():FairMCEventHeader(),
fDecayerData(0),fPrimGenDataList(0)
{
 
}

//_____________________________________________________________________________
SpdMCEventHeader::~SpdMCEventHeader() 
{
  if (fPrimGenDataList) {
      fPrimGenDataList->Delete();
      delete fPrimGenDataList;
  }
  
  if (fDecayerData) delete fDecayerData;
}

//_____________________________________________________________________________
void SpdMCEventHeader::Reset() 
{
   FairMCEventHeader::Reset();
   
   if (fPrimGenDataList) fPrimGenDataList->Delete();
   if (fDecayerData) fDecayerData->Clear();
}

//_____________________________________________________________________________
void SpdMCEventHeader::Register()
{
   FairRootManager::Instance()->Register("MCEventHeader.", "Event", this, kTRUE);
}

//_____________________________________________________________________________
SpdPrimGenData* SpdMCEventHeader::AddGenData(const Char_t* generator)
{
   if (!fPrimGenDataList) GetPrimData(); // add primary previously
   
   SpdPrimGenData* data = new SpdPrimGenData(generator);  
   fPrimGenDataList->Add(data);

   return data;  
}

//_____________________________________________________________________________
Int_t SpdMCEventHeader::GetNPrimGenData() const
{
   return (fPrimGenDataList) ? fPrimGenDataList->GetEntriesFast() : 0;
}

//_____________________________________________________________________________
SpdPrimGenData* SpdMCEventHeader::GetPrimData(TString type) 
{
   SpdPrimGenData* data;
  
   if (!fPrimGenDataList || fPrimGenDataList->IsEmpty()) {
       fPrimGenDataList = new TObjArray();
       data = new SpdPrimGenData(type);  
       data->SetGeneratorIndex(0);
       fPrimGenDataList->Add(data);
       return data;
   }
   
   data = (SpdPrimGenData*)fPrimGenDataList->At(0);
   return (type == data->GetGenType()) ? data : 0;
}

//_____________________________________________________________________________
SpdPrimGenData* SpdMCEventHeader::GetGenData(Int_t index) const
{
   if (index < 0 || index >= GetNPrimGenData()) return 0;
   
   SpdPrimGenData* data = (SpdPrimGenData*)fPrimGenDataList->At(index);
   if (data && index == data->GetGenIndex()) return data;
   
   TIter next(fPrimGenDataList);
   while ((data = (SpdPrimGenData*)next())) {
      if (data && index == data->GetGenIndex()) return data;
   }
   
   return 0;
}

//_____________________________________________________________________________
SpdPrimGenData* SpdMCEventHeader::GetGenData(TString generator) const
{
   if (!fPrimGenDataList) return 0;
   return (SpdPrimGenData*)fPrimGenDataList->FindObject(generator);
}

//_____________________________________________________________________________
TParticle* SpdMCEventHeader::FindParticle(Int_t ipart) const
{
   if (!fPrimGenDataList || ipart < 0) return 0;
   
   int size = fPrimGenDataList->GetEntriesFast();
   if (size < 2) return 0;
   
   SpdPrimGenData* gendata;
   Int_t np0(0), np(0);
   
   for (Int_t i(1); i < size; i++) {
        gendata = (SpdPrimGenData*)fPrimGenDataList->At(i);
        if (gendata) {
            np += gendata->GetNParticles();
            if (gendata && ipart < np) return gendata->GetParticle(ipart-np0);
            np0 = np;
        }
   }
  
   return 0;
}

//_____________________________________________________________________________    
SpdPrimGenData* SpdMCEventHeader::FindGenData(Int_t ipart) const
{
   if (!fPrimGenDataList || ipart < 0) return 0;
   
   int size = fPrimGenDataList->GetEntriesFast();
   if (size < 2) return 0;

   SpdPrimGenData* gendata;
   Int_t np(0);
   
   for (Int_t i(1); i < size; i++) {
        gendata = (SpdPrimGenData*)fPrimGenDataList->At(i);
        if (gendata) {
            np += gendata->GetNParticles();
            if (gendata && ipart < np) return gendata;
        }
   }
   
   return 0;
}

//_____________________________________________________________________________
SpdPrimGenData* SpdMCEventHeader::GetDecayerData() 
{
   if (!fDecayerData) fDecayerData = new SpdPrimGenData();
   return fDecayerData;
}

//_____________________________________________________________________________
void SpdMCEventHeader::Print(int opt) 
{
  if (opt < 0) return;
  
  cout << "\n-I- <SpdMCEventHeader::Print> ";
 
  Int_t ngen = GetNPrimGenData();
  
  cout << "Run: " << fRunId << ", "
       << "Event: " << fEventId << ", "
       << "Primaries: " << fNPrim << ", "
       << "Generators: " << ngen-1
       << endl; 
        
  cout << "\n\tVertex, cm:      (" << fX << ","  << fY << "," << fZ << ") " << " Time: " << fT << endl; 
  cout << "\tBeam angle, rad: (" << fRotX << ","  << fRotY << "," << fRotZ << ") [XZ,YZ,PhiZ] " << endl; 
  
  if (fDecayerData) fDecayerData->Print(" Decayer "); 
  
  if (opt < 1)  { cout << endl; return; } 
  if (ngen < 1) { cout << endl; return; } 
  
  SpdPrimGenData* data;
  
  for (Int_t i(0); i<ngen; i++) {
       data = (SpdPrimGenData*)fPrimGenDataList->At(i);
       data->Print("Generator"); 
  } 
  cout << endl;
}


