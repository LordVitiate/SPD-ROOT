// $Id$
// Author: artur   2017/11/28

//_____________________________________________________________________________
//
// SpdField
//_____________________________________________________________________________

#include "FairRun.h"
#include "FairRuntimeDb.h"

#include "SpdField.h"
#include "SpdFieldPar.h"
#include "SpdRegion.h"

#include <TMath.h>
#include <iostream>

using std::cout;
using std::endl;
using std::set;

ClassImp(SpdField)

TString SpdField::fFieldParType = "SpdFieldPar";

void    SpdField::SetFieldParType(TString partype) { fFieldParType = partype; }
TString SpdField::GetFieldParType() { return fFieldParType; }

set<TString>* SpdField::fUFieldNames = 0;

//_____________________________________________________________________________
SpdField::SpdField():FairField(""),
fFieldRegion(0),fSwitchFieldRegion(kFALSE),
fNpars(0),fParams(0)
{
   fType = -1;
   SetTitle("Undefined");
   if (!fUFieldNames) fUFieldNames = new set<TString>;
   AssignFieldUniqueName("SpdField");
}

//_____________________________________________________________________________
SpdField::SpdField(const Char_t* name):FairField(""),
fFieldRegion(0),fSwitchFieldRegion(kFALSE),
fNpars(0),fParams(0)
{
   fType = -1;
   SetTitle("Undefined");
   if (!fUFieldNames) fUFieldNames = new set<TString>;
   AssignFieldUniqueName(name);
}

//_____________________________________________________________________________
SpdField::SpdField(SpdFieldPar* fieldPar, const Char_t* name):FairField(),
fFieldRegion(0),fSwitchFieldRegion(kFALSE),
fNpars(0),fParams(0)
{
   fType = -1;
   SetTitle("Undefined");
   if (!fUFieldNames) fUFieldNames = new set<TString>;
   AssignFieldUniqueName(name);
  
   if (!fieldPar) {
       cout << "-E- <SpdField::SpdField> Empty parameter container " << endl;
       return;
   }
 
   GetFieldParameters(fieldPar);
   GetRegionParameters(fieldPar); 
}

//_____________________________________________________________________________
SpdField::~SpdField()
{
   if (fFieldRegion) delete fFieldRegion; 
  
   if (fUFieldNames) {
       set<TString>::iterator it = fUFieldNames->find(fName);
       if (it != fUFieldNames->end()) fUFieldNames->erase(it);
       if (fUFieldNames->empty()) {
           delete fUFieldNames;
           fUFieldNames = 0;
       }
   }
  
   if (fParams) delete [] fParams;
}

//_____________________________________________________________________________
void SpdField::AssignFieldUniqueName(TString name) // protected
{
   TString newname = name;
   int n = 1;
   set<TString>::iterator it;
   while (true) {
      it = fUFieldNames->find(newname);
      if (it == fUFieldNames->end()) break;
      newname = name + Form("_%d",n++);
   }
   fUFieldNames->insert(newname);
   //cout << "<SpdField::AssignFieldUniqueName> set newname : <" << newname << "> " << endl;
   FairField::SetName(newname);
}

//_____________________________________________________________________________
Bool_t SpdField::FindFieldName(TString name) // public, static
{
   if (!fUFieldNames) return kFALSE;
   set<TString>::const_iterator it = fUFieldNames->find(name);
   return (it != fUFieldNames->end());
}

//_____________________________________________________________________________
void SpdField::ResetParameters()
{
   if (fNpars < 1 || !fParams) return;
   memset(fParams,0,sizeof(Double_t)*fNpars);
}

//_____________________________________________________________________________
void SpdField::SetNFieldParameters(Int_t n) // protected
{
   if (n <= 0) { // delete parameters
       if (fParams) {
           delete [] fParams;
           fParams = 0;
       }
       fNpars = 0;
       return;
   }
   
   if (n == fNpars) { // clear parameters
       memset(fParams,0,sizeof(Double_t)*fNpars);
       return;
   }
   
   if (fParams) delete [] fParams;
   
   fNpars = n;
   fParams = new Double_t[fNpars];
   memset(fParams,0,sizeof(Double_t)*fNpars);
}

//_____________________________________________________________________________
void SpdField::SetFieldParameter(Int_t n, Double_t value)
{
   if (n < 0 || n >= fNpars) return;
   fParams[n] = value;
}

//_____________________________________________________________________________
SpdRegion* SpdField::CreateFieldRegion(TString type)
{
   if (fFieldRegion) {
       delete fFieldRegion; 
       fFieldRegion = 0;
   }
        if (type == "box")  fFieldRegion = new SpdBoxRegion();
   else if (type == "tube") fFieldRegion = new SpdTubeRegion();
   else if (type == "physical") fFieldRegion = new SpdPhysicalRegion();
   else {
       cout << "-W- <SpdField::CreateFieldRegion> Unknown region type: " 
            << "\"" << type << "\"" << endl;
   }
   
   if (fFieldRegion) fSwitchFieldRegion = kTRUE;
   
   return fFieldRegion;
}

//_____________________________________________________________________________
TString SpdField::GetFieldRegionType() const
{ 
   return (fFieldRegion) ? fFieldRegion->GetRegionType() : "Undefined"; 
}

//_____________________________________________________________________________
Bool_t SpdField::SwitchFieldRegion(Bool_t in) 
{ 
   (fFieldRegion && fFieldRegion->IsRegionInit()) ? fSwitchFieldRegion = in 
                                                  : fSwitchFieldRegion = kFALSE; 
   return fSwitchFieldRegion;
}

//_____________________________________________________________________________
Bool_t SpdField::IsInsideRegion(Double_t x, Double_t y, Double_t z) 
{
   if (!fFieldRegion) return kTRUE;
   return (fSwitchFieldRegion) ? fFieldRegion->IsInside(x,y,z) : kFALSE; 
}

//_____________________________________________________________________________
Bool_t SpdField::IsInsideRegion(Double_t z, Double_t r) 
{
   if (!fFieldRegion) return kTRUE;
   return (fSwitchFieldRegion) ? fFieldRegion->IsInside(r,z) : kFALSE;                                            
}

//_____________________________________________________________________________
Double_t SpdField::GetBx(Double_t x, Double_t y, Double_t z) 
{ 
   static Double_t F;
   return (GetBx(F,x,y,z)) ? F : 0;
}

//_____________________________________________________________________________
Double_t SpdField::GetBy(Double_t x, Double_t y, Double_t z) 
{ 
   static Double_t F;
   return (GetBy(F,x,y,z)) ? F : 0;
} 

//_____________________________________________________________________________    
Double_t SpdField::GetBz(Double_t x, Double_t y, Double_t z) 
{ 
   static Double_t F;
   return (GetBz(F,x,y,z)) ? F : 0;
} 

//_____________________________________________________________________________
void SpdField::GetFieldValue(const Double_t point[3], Double_t* bField)
{
   if (GetField(point,bField)) return;
   
   bField[0] = 0; 
   bField[1] = 0;
   bField[2] = 0;
}

//_____________________________________________________________________________
void SpdField::GetBxyz(const Double_t point[3], Double_t* bField) 
{
   GetFieldValue(point,bField);
}

//_____________________________________________________________________________
Double_t SpdField::Value(Double_t x, Double_t y, Double_t z, TString value, Double_t eps)
{
   Double_t f = 0.;
   
   if (value == "B") {
       static Double_t p[3], b[3];
       p[0] = x; p[1] = y; p[2] = z;
       f = (GetField(p,b)) ? TMath::Sqrt(b[0]*b[0]+b[1]*b[1]+b[2]*b[2]) : 0;
   }
   else if (value == "Bz") f = GetBz(x,y,z); 
   else if (value == "By") f = GetBy(x,y,z);
   else if (value == "Bx") f = GetBx(x,y,z);
   else if (value == "BT") {
       static Double_t p[3], b[2];
       p[0] = x; p[1] = y; p[2] = z;
       f = (GetField(p,b)) ? TMath::Sqrt(b[0]*b[0]+b[1]*b[1]) : 0;
   } 
   else if (value == "BL") f = TMath::Abs(GetBz(x,y,z)); 
   
   if (eps < 1e-9) return f;
   
   f = Int_t(f/eps)*eps;
     
   return f;
}

//_____________________________________________________________________________
Bool_t SpdField::GetField(const Double_t point[3], Double_t* bField)
{
   if (!IsInsideRegion(point[0],point[1],point[2])) return kFALSE;
    
   bField[0] = 0; 
   bField[1] = 0;
   bField[2] = 0;
    
   return kTRUE;
}

//_____________________________________________________________________________
SpdFieldMapData* SpdField::CreateFieldMapData(SpdFieldMapData* mdata,
                                              Int_t Nx, Int_t Ny, Int_t Nz)
{
   if (Nx < 2 || Ny < 2 || Nz < 2) {
       cout << "-E- <SpdField:GetFieldMapData> Bad grid parameters: ";
       cout << "N(x,y,z) = " << Nx << ", " << Ny << ", " << Nz << endl;
       return mdata;
   }
  
   Double_t xmin, ymin, zmin;
   
   if (!mdata) mdata = new SpdFieldMapData("",GetFieldTypeName());
     
   if (mdata->GetInitLevel() < 1) {
     
       if (GetFieldRegionType() != "box") {
           cout << "-W- <SpdField::GetFieldMapData> "
                << "Inappropriate type of field region: " 
                << GetFieldRegionType() << endl;
           return mdata;              
       }
       
       xmin = fFieldRegion->GetXmin();     
       ymin = fFieldRegion->GetYmin();   
       zmin = fFieldRegion->GetZmin();
         
       mdata->InitData(xmin, fFieldRegion->GetXmax(), Nx, 
                       ymin, fFieldRegion->GetYmax(), Ny, 
                       zmin, fFieldRegion->GetZmax(), Nz);
   }  
   else {   
     
       xmin = mdata->GetXmin();       
       ymin = mdata->GetYmin();      
       zmin = mdata->GetZmin();
                 
       Nx = mdata->GetNx();
       Ny = mdata->GetNy();
       Nz = mdata->GetNz();   
   }
   
   Double_t dx = mdata->GetXstep();
   Double_t dy = mdata->GetYstep();
   Double_t dz = mdata->GetZstep();
   
   if (!mdata->IsDataInit()) {
       cout << "-E- <SpdField:GetFieldMapData> No data ";
       return mdata;
   }
   
   TArrayF* Bx = mdata->fBx;
   TArrayF* By = mdata->fBy;
   TArrayF* Bz = mdata->fBz;
   
   Double_t P[3], F[3];
   
   Int_t index;
   
   cout << "-I- <SpdField:CreateFieldMapData> Create field map data of points: " 
        << mdata->GetNTotal() << endl;
   
   //cout << "\n<SpdField::CreateFieldMapData> write node: \n";
   
   for (Int_t ix(0); ix < Nx; ix++) {
   for (Int_t iy(0); iy < Ny; iy++) {
   for (Int_t iz(0); iz < Nz; iz++) {
    
        P[0] = xmin + ix*dx;
        P[1] = ymin + iy*dy;
        P[2] = zmin + iz*dz;
        
        GetFieldValue(P,F);
    
        index = ix*Ny*Nz + iy*Nz + iz;
        
        Bx->SetAt(F[0],index);
        By->SetAt(F[1],index);
        Bz->SetAt(F[2],index);
        
        //cout << index+1 << "\r";
   }}}
   
   //cout << endl;
   
   return mdata; 
}

//_____________________________________________________________________________
void SpdField::Print(Option_t* option) const
{
   cout << "<SpdField::Print>" << endl;
  
   cout << "Field Name:    " << GetName() << endl;
   cout << "Field Type:    " << GetType() << "  (" << GetFieldTypeName() << ")" << endl;
   cout << "Region Type:   " << GetFieldRegionType() << endl;
   cout << "Region on/off: " << fSwitchFieldRegion << endl;
   
   cout << endl;
   
   PrintFieldParameters();
   
   if (fFieldRegion) {
       fFieldRegion->Print(); 
       cout << endl;
   }
}

//_____________________________________________________________________________
void SpdField::PrintListOfFields() const
{
   cout << "<SpdField::PrintListOfFields>" << endl;
 
   std::set<TString>::const_iterator it = fUFieldNames->begin();
   for (; it != fUFieldNames->end(); it++) {
          cout << "            " << *it << endl;  
   }
   cout << endl;
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________
void SpdField::FillParContainer()
{
   FairRun* run = FairRun::Instance();
   if (!run) return;
     
   FairRuntimeDb* rtdb = run->GetRuntimeDb();
   if (!rtdb) return;
   
   SpdFieldPar* Pars = (SpdFieldPar*)rtdb->getContainer(fFieldParType);
   if (!Pars) return;
   
   FillParContainer(Pars);
     
   Pars->setInputVersion(run->GetRunId(),1);
   Pars->setChanged();
}

//_____________________________________________________________________________
void SpdField::FillParContainer(SpdFieldPar* Pars)
{
   PutFieldParameters(Pars);
   PutRegionParameters(Pars); 
}

//_____________________________________________________________________________
Bool_t SpdField::PutFieldParameters(SpdFieldPar* pars) // protected
{
   if (!pars) return kFALSE;
   
   // put field name & type
   pars->AddField(fName,fType);
   
   pars->SetParameter(Add_Name("npars"),fNpars);
   
   if (fNpars > 0) {
       for (Int_t i(0); i<fNpars; i++) 
            pars->SetParameter(Add_Name(Form("par.%d",i)),fParams[i]);
   }
   
   return PutParameters(pars);
}

//_____________________________________________________________________________
Bool_t SpdField::GetFieldParameters(SpdFieldPar* pars) // protected
{
   if (!pars) return kFALSE;
   
   Int_t type;
   
   // check field name & type
   if (!pars->FindField(fName,type)) {
       cout << "-W- <SpdField::GetRegionParameters> "
            << "No parameters for a such field \"" << fName << "\""<< endl;
       return kFALSE;
   }
   
   if (type != fType) {
       cout << "-W- <SpdField::GetRegionParameters> "
            << "No parameters for a field \"" << fName << "\""
            << "of type = " << fType << endl;
       return kFALSE;
   }  
   
   Int_t np(0);
   pars->GetParameter(Add_Name("npars"),np);
 
   SetNFieldParameters(np);
   
   if (fNpars > 0) {
       for (Int_t i(0); i<fNpars; i++) 
            pars->GetParameter(Add_Name(Form("par.%d",i)),fParams[i]);
   }
   
   return GetParameters(pars);
}

//_____________________________________________________________________________
Bool_t SpdField::GetRegionParameters(SpdFieldPar* pars) // protected
{
   if (!pars) return kFALSE;
  
   TString rtype("");
   
   // check field region type
   if (!pars->GetParameter(Add_Name("R.type"),rtype) || 
       rtype.IsWhitespace())
   {
       if (fFieldRegion) {
           delete fFieldRegion; 
           fFieldRegion = 0;
       }
       return kFALSE;
   }    
        
   if (!CreateFieldRegion(rtype)) return kFALSE;
   
   //cout << "-I- <SpdField::GetRegionParameters> Create field region of type: " 
   //     << rtype << endl;
        
   Int_t init = 0;
  
   if (!pars->GetParameter(Add_Name("R.init"),init) || !init) {
       cout << "-W- <SpdField::GetRegionParameters> "
            << "region is not initialized "<< endl;
       return kFALSE;    
   }
   
   Int_t on;
   (pars->GetParameter(Add_Name("R.on"),on)) ? fSwitchFieldRegion = on
                                             : fSwitchFieldRegion = kTRUE;
   
   // set region parameters
   if (rtype == "box") {
     
       Double_t xmin(0), xmax(0), ymin(0), ymax(0), zmin(0), zmax(0);      
      
       pars->GetParameter(Add_Name("R.box_xmin"),xmin);
       pars->GetParameter(Add_Name("R.box_xmax"),xmax);
       pars->GetParameter(Add_Name("R.box_ymin"),ymin);
       pars->GetParameter(Add_Name("R.box_ymax"),ymax);
       pars->GetParameter(Add_Name("R.box_zmin"),zmin);
       pars->GetParameter(Add_Name("R.box_zmax"),zmax);  
       
       fFieldRegion->SetBoxRegion(xmin,xmax,ymin,ymax,zmin,zmax);
       
       return kTRUE;
   }
   
   if (rtype  == "tube") {
     
       Double_t rmin(0), rmax(0), zmin(0), zmax(0);
      
       pars->GetParameter(Add_Name("R.tube_rmin"),rmin);
       pars->GetParameter(Add_Name("R.tube_rmax"),rmax);
       pars->GetParameter(Add_Name("R.tube_zmin"),zmin);
       pars->GetParameter(Add_Name("R.tube_zmax"),zmax);
      
       fFieldRegion->SetTubeRegion(rmin,rmax,zmin,zmax);
       
       return kTRUE;
   }
   
   if (rtype  == "physical") {
     
       TString region(""), type("");
       
       pars->GetParameter(Add_Name("R.physical_region"),region);
       pars->GetParameter(Add_Name("R.physical_type"),type);
       
       fFieldRegion->SetPhysicalRegion(region,type);
       
       return kTRUE;     
   }
   
   return kFALSE;
}
  
//_____________________________________________________________________________  
Bool_t SpdField::PutRegionParameters(SpdFieldPar* pars) // protected
{
   if (!pars || !fFieldRegion) return kFALSE;
   
   TString rtype = fFieldRegion->GetRegionType();
   pars->SetParameter(Add_Name("R.type"),rtype);
  
   Int_t init = fFieldRegion->IsRegionInit();
   pars->SetParameter(Add_Name("R.init"),init);
   
   if (!init) return kFALSE;
   
   pars->SetParameter(Add_Name("R.on"),Int_t(fSwitchFieldRegion));
     
   // put region parameters
   if (rtype == "box") {
     
       Double_t xmin, xmax, ymin, ymax, zmin, zmax;      
       fFieldRegion->GetBoxRegion(xmin,xmax,ymin,ymax,zmin,zmax);
       
       pars->SetParameter(Add_Name("R.box_xmin"),xmin);
       pars->SetParameter(Add_Name("R.box_xmax"),xmax);
       pars->SetParameter(Add_Name("R.box_ymin"),ymin);
       pars->SetParameter(Add_Name("R.box_ymax"),ymax);
       pars->SetParameter(Add_Name("R.box_zmin"),zmin);
       pars->SetParameter(Add_Name("R.box_zmax"),zmax);       
     
       return kTRUE;
   }
   
   if (rtype == "tube") {
     
       Double_t rmin, rmax, zmin, zmax;
       fFieldRegion->GetTubeRegion(rmin,rmax,zmin,zmax);
       
       pars->SetParameter(Add_Name("R.tube_rmin"),rmin);
       pars->SetParameter(Add_Name("R.tube_rmax"),rmax);
       pars->SetParameter(Add_Name("R.tube_zmin"),zmin);
       pars->SetParameter(Add_Name("R.tube_zmax"),zmax);
      
       return kTRUE;
   }
   
   if (rtype  == "physical") {
     
       TString region(""), type("");
       fFieldRegion->GetPhysicalRegion(region,type);
        
       pars->SetParameter(Add_Name("R.physical_region"),region);
       pars->SetParameter(Add_Name("R.physical_type"),type);
       
       return kTRUE;     
   }
   
   return kFALSE;
}





