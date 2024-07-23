
#include "SpdGeoCave.h"

#include "FairGeoBasicShape.h"  
#include "FairGeoMedia.h"       
#include "FairGeoMedium.h"      
#include "FairGeoNode.h"        
#include "FairGeoShapes.h"      

#include <TList.h>              

#include <string.h>             
#include <iostream>             

using namespace std;

ClassImp(SpdGeoCave)

//_____________________________________________________________________________
SpdGeoCave::SpdGeoCave():FairGeoSet(),Name_("cave")
{
  fName = "cave";
  maxModules = 1;
}

//_____________________________________________________________________________
void SpdGeoCave::addRefNodes()
{
  FairGeoNode* volu = getVolume(Name_);
  if (volu) { masterNodes->Add(new FairGeoNode(*volu)); }
}

//_____________________________________________________________________________
Bool_t SpdGeoCave::read(std::fstream& fin, FairGeoMedia* media)
{
  // Reads the geometry from file
  
  if (!media) { return kFALSE; }
  
  const Int_t maxbuf = 256;
  char buf[maxbuf];
  
  FairGeoNode* volu = 0;
  FairGeoMedium* medium;
  
  Bool_t rc = kTRUE;
  
  do {
    
    fin.getline(buf,maxbuf);

    if (buf[0] != '\0' && buf[0] != '/' && !fin.eof()) {
        if (strcmp(buf,Name_) == 0) {
            
            volu = new FairGeoNode;
            volu->SetName(buf);
            volu->setVolumeType(kFairGeoTopNode);
            volu->setActive();
            
            fin.getline(buf,maxbuf);
            
            TString shape(buf);
            FairGeoBasicShape* sh = pShapes->selectShape(shape);
            
            if (sh) { volu->setShape(sh); }
            else { rc = kFALSE; }
        
            fin.getline(buf,maxbuf);
            
            medium = media->getMedium(buf);
            if (!medium) {
                medium = new FairGeoMedium();
                media->addMedium(medium);
            }
            volu->setMedium(medium);
            
            Int_t n = 0;
            if (sh) { n = sh->readPoints(&fin,volu); }
            if (n <= 0) { rc = kFALSE; }
        }
        else { 
            rc = kFALSE; 
        }
    }
  } while (rc && !volu && !fin.eof());
  
  if (volu && rc) {
      volumes->Add(volu);
      masterNodes->Add(new FairGeoNode(*volu));
  } 
  else {
      delete volu;
      volu = 0;
      rc = kFALSE;
  }
  
  return rc;
}

//_____________________________________________________________________________
void SpdGeoCave::write(std::fstream& fout)
{
  // Writes the geometry to file
    
  fout.setf(ios::fixed,ios::floatfield);
  
  FairGeoNode* volu = getVolume(Name_);
  
  if (volu) {
      FairGeoBasicShape* sh = volu->getShapePointer();
      FairGeoMedium* med = volu->getMedium();
      if (sh && med) {
          fout << volu->GetName() << '\n' 
               << sh->GetName()   << '\n' 
               << med->GetName()  << '\n';
          sh->writePoints(&fout,volu);
      }
  }
}

//_____________________________________________________________________________
void SpdGeoCave::print()
{
  // Prints the geometry
    
  FairGeoNode* volu = getVolume(Name_);
  
  if (volu) {
      FairGeoBasicShape* sh = volu->getShapePointer();
      FairGeoMedium* med = volu->getMedium();
      if (sh && med) {
          cout << volu->GetName() << '\n' 
               << sh->GetName()   << '\n'
               << med->GetName()  << '\n';
          sh->printPoints(volu);
      }
  }
}

