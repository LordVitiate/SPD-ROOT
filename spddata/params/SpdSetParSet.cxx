// $Id$
// Author: artur   2021/01/20

//_____________________________________________________________________________
//
// SpdSetParSet
//_____________________________________________________________________________

#include "SpdSetParSet.h"

#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdSetParSet)

//_____________________________________________________________________________
SpdSetParSet::SpdSetParSet():FairParGenericSet("Spd Parameters","An assembly of sets",""),
fFieldPars(0),fPrimGenPars(0),fPassiveGeoPars(0),fActiveGeoPars(new TObjArray),
fHitMakerPars(new TObjArray),fObjMakerPars(new TObjArray)
{
}

//_____________________________________________________________________________
SpdSetParSet::~SpdSetParSet() 
{
 
}

//_____________________________________________________________________________
void SpdSetParSet::AddActiveGeoPars(FairParGenericSet* pars)
{
   if (!fActiveGeoPars) return;
   SpdParSet* pp =  dynamic_cast<SpdParSet*>(pars);    
   if (pp) fActiveGeoPars->Add(pp);
}

//_____________________________________________________________________________
SpdParSet* SpdSetParSet::GetActiveGeoPars(TString pars_name) const
{
   return (SpdParSet*)fActiveGeoPars->FindObject(pars_name);
}

//_____________________________________________________________________________
void SpdSetParSet::AddHitMakerPars(FairParGenericSet* pars)
{
   if (!fHitMakerPars) return;
   SpdParSet* pp =  dynamic_cast<SpdParSet*>(pars);    
   if (pp) fHitMakerPars->Add(pp);
}

//_____________________________________________________________________________
SpdParSet* SpdSetParSet::GetHitMakerPars(TString pars_name) const
{
   return dynamic_cast<SpdParSet*>(fHitMakerPars->FindObject(pars_name));
}

//_____________________________________________________________________________
void SpdSetParSet::AddObjMakerPars(FairParGenericSet* pars)
{
   if (!fObjMakerPars) return;
   SpdParSet* pp =  dynamic_cast<SpdParSet*>(pars);    
   if (pp) fObjMakerPars->Add(pp);
}

//_____________________________________________________________________________
SpdParSet* SpdSetParSet::GetObjMakerPars(TString pars_name) const
{
   return dynamic_cast<SpdParSet*>(fObjMakerPars->FindObject(pars_name));
}

//_____________________________________________________________________________
FairParGenericSet* SpdSetParSet::FindParameters(TString pars_name) const
{
   if (fFieldPars && pars_name == fFieldPars->GetName()) return fFieldPars;
   if (fPrimGenPars && pars_name == fPrimGenPars->GetName()) return fPrimGenPars;
   if (fPassiveGeoPars && pars_name == fPassiveGeoPars->GetName()) return fPassiveGeoPars;
   
   FairParGenericSet* pars = 0;
   
   if (fActiveGeoPars->GetEntriesFast() > 0) {
       pars = static_cast<FairParGenericSet*>(fActiveGeoPars->FindObject(pars_name));
       if (pars) return pars;
   }
   if (fHitMakerPars->GetEntriesFast() > 0) {
       pars = static_cast<FairParGenericSet*>(fHitMakerPars->FindObject(pars_name));
       if (pars) return pars;
   }
   if (fObjMakerPars->GetEntriesFast() > 0) {
       pars = static_cast<FairParGenericSet*>(fObjMakerPars->FindObject(pars_name));
       if (pars) return pars;
   }
   
   return pars;
}

//_____________________________________________________________________________
void SpdSetParSet::PrintParameters(Int_t opt) const
{
   cout << "\n\n<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" <<"\n";
   cout << "-I- <SpdSetParSet::PrintParameters> "
        << fName << " / " << fTitle << "\n\n";
   
   cout << "Magnetic field:     " 
        << ((fFieldPars) ? Form("yes (\"%s\")",fFieldPars->GetName()) : "no") << endl;
   cout << "Primary generator:  " 
        << ((fPrimGenPars) ? Form("yes (\"%s\")",fPrimGenPars->GetName()) : "no") << endl;
   cout << "Passive geometry:   " 
        << ((fPassiveGeoPars) ? Form("yes (\"%s\")",fPassiveGeoPars->GetName()) : "no") << endl; 
   cout << "Active geometry:    " << fActiveGeoPars->GetEntriesFast() << endl;
   cout << "Hit makers:         " << fHitMakerPars->GetEntriesFast() << endl;
   cout << "Obj makers:         " << fObjMakerPars->GetEntriesFast() << endl;
   cout << endl; 

   if (fFieldPars && opt > 0)      fFieldPars->print(1);
   if (fPrimGenPars && opt > 0)    fPrimGenPars->print(1);
   if (fPassiveGeoPars && opt > 0) fPassiveGeoPars->print(1);
    
   if (fActiveGeoPars->GetEntriesFast() > 0) 
   {
       TObjArrayIter next(fActiveGeoPars);
       SpdParSet* pars;
       Int_t n(0);
       if (opt < 1) cout << "Active geometry (name, title): \n";
       while ((pars = (SpdParSet*)next())) {
          if (pars) {
              if (opt < 1) cout << "\t" << ++n << ". \"" << pars->GetName() << "\"  \"" << pars->GetTitle() << "\"\n";   
              else { pars->print(1); cout << endl; }
          }
       }
   }
   
   if (fHitMakerPars->GetEntriesFast() > 0) 
   {
       TObjArrayIter next(fHitMakerPars);
       SpdParSet* pars;
       Int_t n(0);
       if (opt < 1) cout << "Hit producer (name, title): \n";
       while ((pars = (SpdParSet*)next())) {
          if (pars) {
              if (opt < 1) cout << "\t" << ++n << ". \"" << pars->GetName() << "\"  \"" << pars->GetTitle() << "\"\n";   
              else { pars->print(1); cout << endl; }
          }
       }
   }
   
   if (fObjMakerPars->GetEntriesFast() > 0) 
   {
       TObjArrayIter next(fObjMakerPars);
       SpdParSet* pars;
       Int_t n(0);
       if (opt < 1) cout << "Object maker (name, title): \n";
       while ((pars = (SpdParSet*)next())) {
          if (pars) {
              if (opt < 1) cout << "\t" << ++n << ". \"" << pars->GetName() << "\"  \"" << pars->GetTitle() << "\"\n";   
              else { pars->print(1); cout << endl; }
          }
       }
   }
   
   cout << "\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" <<"\n";
}



