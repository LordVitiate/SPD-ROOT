// $Id$
// Author: artur   2018/01/05

#include "FairRun.h"
#include "FairRuntimeDb.h"

#include "SpdFieldCreator.h"
#include "SpdFieldPar.h"
#include "SpdField.h"
#include "SpdConstField.h"
#include "SpdRadialField.h"
#include "SpdFieldMap.h"
#include "SpdFieldMap1_8.h"
#include "SpdAxialFieldMap.h"
#include "SpdMultiField.h"

#include <iostream>
using std::cout;
using std::endl;

ClassImp(SpdFieldCreator)

//static SpdFieldCreator gSpdFieldCreator;

//_____________________________________________________________________________
SpdFieldCreator::SpdFieldCreator():FairFieldFactory(),fFieldPar(0)
{
   fCreator = this;
   //std::cout << "<SpdFieldCreator::SpdFieldCreator> CONSTRUCT FIELD CREATOR" << std::endl;  
}

//_____________________________________________________________________________
SpdFieldCreator::~SpdFieldCreator()
{
}

//_____________________________________________________________________________
void SpdFieldCreator::SetParameters()
{
  FairRun* run = FairRun::Instance();
  if (!run) return;
     
  FairRuntimeDb* rtdb = run->GetRuntimeDb();
  if (!rtdb) return;
 
  fFieldPar = (SpdFieldPar*)rtdb->getContainer("SpdFieldPar");
}

//_____________________________________________________________________________
FairField* SpdFieldCreator::createFairField(SpdFieldPar* Pars)
{
  fFieldPar = Pars;
  return createFairField();
}

//_____________________________________________________________________________
FairField* SpdFieldCreator::createFairField()
{ 
  if (!fFieldPar) {
      cout << "-E- <SpdFieldCreator::createFairField> "
           << "No field parameters is available "<< endl;
      return 0;   
  }
  
  Int_t nf = fFieldPar->GetNFields();
  FairField* MagneticField = 0;
  
  if (nf < 1) return MagneticField;
  else if (nf == 1) {
     TString name;
     Int_t type;
     if (!fFieldPar->GetFieldNameType(name,type)) return MagneticField;
     MagneticField = CreateSingleField(name,type);
  }
  else MagneticField = CreateMultiField();
  
  //MagneticField->Print("");
  return MagneticField;
}

//_____________________________________________________________________________
FairField* SpdFieldCreator::CreateSingleField(TString name, Int_t type)
{
  FairField* MagneticField = 0;
  
  cout << "-I- <SpdFieldCreator::CreateSingleField> "
       << "Create field \"" << name << "\" of type " << type << endl; 
  
  if (type < 0) {
      //cout << "-I- <SpdFieldCreator::createSingleField> "
      //     << "Create \"zero\" field of type: " << -1 << endl;    
      MagneticField = new SpdField(fFieldPar,name);
      return MagneticField;
  }
  else if (type == 0) MagneticField = new SpdConstField(fFieldPar,name);
  else if (type == 1) MagneticField = new SpdRadialField(fFieldPar,name);
  else if (type == 2) MagneticField = new SpdFieldMap(fFieldPar,name);
  else if (type == 3) MagneticField = new SpdAxialFieldMap(fFieldPar,name);
  else if (type == 4) MagneticField = new SpdFieldMap1_8(fFieldPar,name);
  else { 
    
      MagneticField = new SpdField();
      
      cout << "-W- <SpdFieldCreator::CreateSingleField> "
           << "Unknown field type: " << type << endl;
      cout << "-W- <SpdFieldCreator::CreateSingleField> "
           << "Create \"zero\" field of type: " << -1 
           << " and name \"" << MagneticField->GetName() << "\"" << endl;   
           
      return MagneticField;
  }
  
  MagneticField->Init();
                
  return MagneticField;
}

//_____________________________________________________________________________
FairField* SpdFieldCreator::CreateMultiField()
{
  SpdMultiField* MagneticField = new SpdMultiField();
  
  cout << "-I- <SpdFieldCreator::CreateMultiField> "
       << "Create multifield (type = " << MagneticField->GetType() << ") " << endl;
       
  const std::map<std::string,Int_t>& fields = fFieldPar->GetListOfFields();
  std::map<std::string,Int_t>::const_iterator it = fields.begin();
  
  FairField* field;
  for (; it != fields.end(); it++) {
       field = CreateSingleField(it->first.c_str(),it->second);
       MagneticField->AddField(dynamic_cast<SpdField*>(field));
  }
 
  MagneticField->GetParameters(fFieldPar);
  
  MagneticField->Init();
  
  return MagneticField;
}





