// $Id$
// Author: artur   2017/12/20

//_____________________________________________________________________________
//
// SpdMultiField
//_____________________________________________________________________________

#include "SpdMultiField.h"

#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "SpdFieldPar.h"

#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdMultiField)

//_____________________________________________________________________________
SpdMultiField::SpdMultiField():FairField("SpdMultiField"),
fIsOverlapped(kFALSE),fIsFieldsOwner(kTRUE),fOverlapSumMode(0)
{
   fType = 3;
   SetTitle("SpdMultiField");
}

//_____________________________________________________________________________
SpdMultiField::~SpdMultiField()
{
   if (fIsFieldsOwner) {
       fFieldsIter = fFields.begin();
       for ( ; fFieldsIter != fFields.end(); fFieldsIter++) 
            delete fFieldsIter->second;
   }
}

//_____________________________________________________________________________
Bool_t SpdMultiField::AddField(SpdField* field, Bool_t switch_on)
{
   if (!field) return kFALSE;
     
   if (fFields.empty()) {
       fFields.insert(std::pair<Int_t,SpdField*>(!switch_on,field));
       return kTRUE;
   }
   
   SpdField* f = GetField(field->GetName());
   if (!f) {
       fFields.insert(std::pair<Int_t,SpdField*>(!switch_on,field));
       return kTRUE;
   }
   
   cout << "<SpdMultiField::AddField> Field \"" << field->GetName() << "\""
        << " already added " << endl;
        
   return kFALSE;
}

//_____________________________________________________________________________
Bool_t SpdMultiField::SwitchField(TString name, Bool_t switch_on)
{
   std::multimap<Int_t, SpdField*>::iterator FieldsIter = fFields.begin();
   SpdField* f;
   
   for ( ; FieldsIter != fFields.end(); FieldsIter++) {
        f = FieldsIter->second;
        if (name == f->GetName()) {
            if (FieldsIter->first == !switch_on) return kFALSE;
                          
            fFields.erase(FieldsIter); 
            fFields.insert(std::pair<Int_t,SpdField*>(!switch_on,f));
            
            cout << "<SpdMultiField::SwitchField> Change switch parameter for \"" 
                 << f->GetName() << "\" to " << ((switch_on) ? "'on'" : "'off'") << endl;
                 
            return kTRUE;
        }
   }
   return kFALSE;
}

//_____________________________________________________________________________
SpdField* SpdMultiField::GetField(TString name)
{
   std::multimap<Int_t, SpdField*>::iterator FieldsIter = fFields.begin();
   for ( ; FieldsIter != fFields.end(); FieldsIter++) {
        if (name == FieldsIter->second->GetName()) return FieldsIter->second;
   }
   return 0; 
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________
Bool_t SpdMultiField::IsInsideRegion(Double_t x, Double_t y, Double_t z) 
{
   if (fFields.empty()) return kTRUE;
   
   fFieldsIter = fFields.begin();
   while (fFieldsIter->first != 1) {
          if (fFieldsIter->second->IsInsideRegion(x,y,z)) return kTRUE;         
          if (++fFieldsIter == fFields.end()) return kFALSE;
   }
 
   return kFALSE;
}

//_____________________________________________________________________________
Bool_t SpdMultiField::IsInsideRegion(Double_t r, Double_t z) 
{
   if (fFields.empty()) return kTRUE;
   
   fFieldsIter = fFields.begin();
   while (fFieldsIter->first != 1) {
          if (fFieldsIter->second->IsInsideRegion(r,z)) return kTRUE;         
          if (++fFieldsIter == fFields.end()) return kFALSE;
   }
 
   return kFALSE;
}

//_____________________________________________________________________________
void SpdMultiField::GetFieldValue(const Double_t point[3], Double_t* bField)
{
    if (fFields.empty()) return;

    fFieldsIter = fFields.begin();
    
    if (!fIsOverlapped) {
        while (fFieldsIter->first != 1) {
               if (fFieldsIter->second->GetField(point,bField)) return;
               if (++fFieldsIter == fFields.end()) {
                   bField[0] = 0; 
                   bField[1] = 0;
                   bField[2] = 0;
               }
        }
    }
    else {
        
        bField[0] = 0; 
        bField[1] = 0;
        bField[2] = 0;   
        static Double_t F[3];
        
        if (fOverlapSumMode < 1) {
            Int_t n(0);
            while (fFieldsIter->first != 1) {
               if (fFieldsIter->second->GetField(point,F)) {
                   bField[0] += F[0]; 
                   bField[1] += F[1];
                   bField[2] += F[2];  
                   n++;
               }
               if (++fFieldsIter == fFields.end()) break;
            }
            if (n > 1) {
                Double_t nn = 1./n; 
                bField[0] *= nn; 
                bField[1] *= nn;
                bField[2] *= nn;  
            }
            return;
        }
        else {
            while (fFieldsIter->first != 1) {
               if (fFieldsIter->second->GetField(point,F)) {
                   bField[0] += F[0]; 
                   bField[1] += F[1];
                   bField[2] += F[2];  
               }
               if (++fFieldsIter == fFields.end()) return;
            }
        }      
    }
}

//_____________________________________________________________________________
Double_t SpdMultiField::GetBx(Double_t x, Double_t y, Double_t z)
{
    if (fFields.empty()) return 0;

    fFieldsIter = fFields.begin();
    static Double_t F;
    
    if (!fIsOverlapped) {
        while (fFieldsIter->first != 1) {
               if (fFieldsIter->second->GetBx(F,x,y,z)) return F;
               if (++fFieldsIter == fFields.end()) return 0;
        }
    }
    else {
        Double_t B = 0;
        if (fOverlapSumMode < 1) {
            Int_t n = 0;
            while (fFieldsIter->first != 1) {
               if (fFieldsIter->second->GetBx(F,x,y,z)) { B += F; n++; }
               if (++fFieldsIter == fFields.end()) return (n > 1) ? B/n : B;
            }
            return (n > 1) ? B/n : B;
        }
        else {
            while (fFieldsIter->first != 1) {
               if (fFieldsIter->second->GetBx(F,x,y,z)) B += F;
               if (++fFieldsIter == fFields.end()) return B;
            }
            return B;
        }
    }
}

//_____________________________________________________________________________
Double_t SpdMultiField::GetBy(Double_t x, Double_t y, Double_t z)
{
    if (fFields.empty()) return 0;

    fFieldsIter = fFields.begin();
    static Double_t F;
    
    if (!fIsOverlapped) {
        while (fFieldsIter->first != 1) {
               if (fFieldsIter->second->GetBy(F,x,y,z)) return F;
               if (++fFieldsIter == fFields.end()) return 0;
        }
    }
    else {
        Double_t B = 0;
        if (fOverlapSumMode < 1) {
            Int_t n = 0;
            while (fFieldsIter->first != 1) {
               if (fFieldsIter->second->GetBy(F,x,y,z)) { B += F; n++; }
               if (++fFieldsIter == fFields.end()) return (n > 1) ? B/n : B;
            }
            return (n > 1) ? B/n : B;
        }
        else {
            while (fFieldsIter->first != 1) {
               if (fFieldsIter->second->GetBy(F,x,y,z)) B += F;
               if (++fFieldsIter == fFields.end()) return B;
            }
            return B;
        }
    }
}

//_____________________________________________________________________________
Double_t SpdMultiField::GetBz(Double_t x, Double_t y, Double_t z)
{
    if (fFields.empty()) return 0;

    fFieldsIter = fFields.begin();
    static Double_t F;
    
    if (!fIsOverlapped) {
        while (fFieldsIter->first != 1) {
               if (fFieldsIter->second->GetBz(F,x,y,z)) return F;
               if (++fFieldsIter == fFields.end()) return 0;
        }
    }
    else {
        Double_t B = 0;
        if (fOverlapSumMode < 1) {
            Int_t n = 0;
            while (fFieldsIter->first != 1) {
               if (fFieldsIter->second->GetBz(F,x,y,z)) { B += F; n++; }
               if (++fFieldsIter == fFields.end()) return (n > 1) ? B/n : B;
            }
            return (n > 1) ? B/n : B;
        }
        else {
            while (fFieldsIter->first != 1) {
               if (fFieldsIter->second->GetBz(F,x,y,z)) B += F;
               if (++fFieldsIter == fFields.end()) return B;
            }
            return B;
        }
    }  
}

//_____________________________________________________________________________
void SpdMultiField::GetBxyz(const Double_t point[3], Double_t* bField)
{
    GetFieldValue(point,bField);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________
void SpdMultiField::GetParameters(SpdFieldPar* Pars)
{
   if (!Pars) return;
   
   fIsOverlapped = kFALSE;
   fIsFieldsOwner = kTRUE;
   
   Int_t x;
 
   if (Pars->GetParameter("MF.Fields.overlap",x)) fIsOverlapped = x;
   if (Pars->GetParameter("MF.Fields.owner",x)) fIsFieldsOwner = x;
   
   if (fIsOverlapped) {
       if (Pars->GetParameter("MF.Fields.osummode",x)) fOverlapSumMode = x;
   }
   
   if (fFields.empty()) return;
   
   TString field;
   Int_t switch_off;
    
   std::multimap<Int_t, SpdField*>::iterator FieldsIter = fFields.begin();
      
   while (FieldsIter != fFields.end()) {
     
        field = FieldsIter->second->GetName();
        field = "MF.Field.switch_on." + field;
      
        if (Pars->GetParameter(field.Data(),switch_off)) {                            
            if (SwitchField(FieldsIter->second->GetName(),!switch_off)) 
                FieldsIter = fFields.begin();
            else 
                FieldsIter++;
        }
        else FieldsIter++;
   }
}

//_____________________________________________________________________________
void SpdMultiField::FillParContainer()
{
   FairRun* run = FairRun::Instance();
   if (!run) return;
     
   FairRuntimeDb* rtdb = run->GetRuntimeDb();
   if (!rtdb) return;
   
   SpdFieldPar* Pars = (SpdFieldPar*)rtdb->getContainer("SpdFieldPar");
   if (!Pars) return;
   
   FillParContainer(Pars);
   
   Pars->setInputVersion(run->GetRunId(),1);
   Pars->setChanged();
}

//_____________________________________________________________________________
void SpdMultiField::FillParContainer(SpdFieldPar* Pars)
{
   if (!Pars) return;
 
   Pars->SetParameter("MF.Fields.overlap",Int_t(fIsOverlapped));
   Pars->SetParameter("MF.Fields.owner",Int_t(fIsFieldsOwner));
   
   if (fIsOverlapped) Pars->SetParameter("MF.Fields.osummode",fOverlapSumMode);
   
   TString field;
   
   fFieldsIter = fFields.begin();
   for ( ; fFieldsIter != fFields.end(); fFieldsIter++) {
        fFieldsIter->second->FillParContainer(Pars);
        field = fFieldsIter->second->GetName();
        field = "MF.Field.switch_on." + field;
        Pars->SetParameter(field.Data(),Int_t(fFieldsIter->first));
   }
}

//_____________________________________________________________________________
void SpdMultiField::Print(Option_t* option) const
{
   cout << "<SpdMultiField::Print>" << endl;
   
   cout << "MF.Fields.overlap  = " << fIsOverlapped  << endl;
   cout << "MF.Fields.owner    = " << fIsFieldsOwner << endl;
   
   if (fIsOverlapped) cout << "MF.Fields.osummode = " << fOverlapSumMode << endl;
   
   cout << "\nPrint fields table: \n\n";
   
   int n(0);
   const char* c;
   
   std::multimap<Int_t,SpdField*>::const_iterator FieldsIter = fFields.begin();
   
   for ( ; FieldsIter != fFields.end(); FieldsIter++) {
        (FieldsIter->first == 0) ? c = "on" : c = "off";
        printf("%4d  [%3s]  %4d   %s\n",++n, c, FieldsIter->second->GetType(),
                                               FieldsIter->second->GetName());  
   }
   
   cout << endl;
   
   FieldsIter = fFields.begin();
   for ( ; FieldsIter != fFields.end(); FieldsIter++) {
        FieldsIter->second->Print(option);
   }
}





