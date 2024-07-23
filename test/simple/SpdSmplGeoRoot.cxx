

#include <TGeoBBox.h>
#include <TGeoTube.h>
#include <TSystem.h>
#include <TVector3.h>
#include <TList.h>
#include <TMath.h>

#include "SpdSmplGeoRoot.h"

using namespace TMath;
using namespace std;

ClassImp(SpdSmplGeoRoot)

//_____________________________________________________________________________
SpdSmplGeoRoot::SpdSmplGeoRoot(TString modname, TString name, TString title): 
Top_(0),Closed_(false),theTOPname("cave"),theMODULEname(modname),FMedia_(0),FGeom_(0),Delim_('-',70) {
   //   
   //TGeoManager::SetVerboseLevel(-1);
   
   if (!gGeoManager) gGeoManager = new TGeoManager(name,title);
   else gGeoManager->SetNameTitle(name,title); 
    
   SetDefaultMedia();
   
   Delim_ = "//" + Delim_;
}

//_____________________________________________________________________________
SpdSmplGeoRoot::~SpdSmplGeoRoot() {
   //
   if (FMedia_) fclose(FMedia_);
   if (FGeom_)   fclose(FGeom_);
}

//_____________________________________________________________________________
void SpdSmplGeoRoot::MakeMediaFile() {
    //    
    cout << "<SpdSmplGeoRoot::SetMediums> this method not implemented yet, dump media list: " << endl;
    
    TList* lmed = gGeoManager->GetListOfMedia();
    if (!lmed) return;
    
    TListIter iter(lmed);
    TGeoMedium* medium;

    int n(0);
    while( (medium = (TGeoMedium*)iter.Next())) {
        cout << ++n << " " << medium->GetName() << endl;
    }   
}

///_____________________________________________________________________________
bool SpdSmplGeoRoot::Build() {
    //   
    if (Closed_) {
        cout << "GeoOLVE::Build(): geometry has been done." << endl;
        return false;
    }
    
    /// here all length unit is one mm !!!
    
    Top_ = gGeoManager->MakeBox(theTOPname.Data(),FindMedium("vacuum"),20000,20000,20000); 
    if (!Top_) return false;
    gGeoManager->SetTopVolume(Top_);
    
     BuildBarrel(); 
     BuildVertexDetector();
     BuildStrawDetectors(); 
     BuildEcalTOFDetectors();
 //    BuildEnvDetectors();
 
    gGeoManager->CloseGeometry();
    Closed_ = true;
    return Closed_;
}

//_____________________________________________________________________________
void SpdSmplGeoRoot::BuildBarrel() {
    // 
    if (!Top_) return;
 
    int np = 3;  // number of slices (along z-axis)
    int ns = 8;  // number of angular sectors
    int nl = 12; // barrel thickness (number of layers)  
      
    TGeoVolume *yoke1[nl], *yoke2[nl-1];
    TGeoTranslation trans; 
    TGeoRotation    rot; 
     
    double L = 1660.;
    for (int j(0); j < nl-1; j++) {     
        yoke1[j] = gGeoManager->MakeBox(FName(Form("YOKE1_%d",j)),FindMedium("iron"), 30./2, L/2., 1750./2);
        yoke2[j] = gGeoManager->MakeBox(FName(Form("YOKE2_%d",j)),FindMedium("arco27030"), 10./2, L/2., 1720./2); 
        L += 50-j*2; 
    }
    yoke1[nl-1] = gGeoManager->MakeBox(FName(Form("YOKE1_%d",nl-1)),FindMedium("iron"), 30./2, L/2., 1750./2);
   
    double br1, br2, deg;  
    double ddeg = 360./ns;
    
    for (int i(0); i < np; i++) {
         br1 = 2050.; 
         br2 = 2074.;
         for (int j(0); j < nl; j++) {                 
              deg = 0;          
              int k = 0;     
              do {
                 trans.SetTranslation(br1*Cos(deg), br1*Sin(deg), 1770. - i*1770.);
                 AddNode(Top_, yoke1[j], i*nl+k+1, new TGeoCombiTrans(trans,rot)); /// ADD NODE !!!
                 if (j < nl-1) {
                     trans.SetTranslation(br2*Cos(deg), br2*Sin(deg), 1770. - i*1780.);                  
                     AddNode(Top_, yoke2[j], i*nl+k+1, new TGeoCombiTrans(trans,rot)); /// ADD NODE !!!
                 }
                 k++;
                 rot.RotateZ(ddeg);
                 deg = ddeg*k*DegToRad();     
              } 
              while (k < ns);              
              br1 += 50;
              br2 += 50;
         }  
    }
}
//_____________________________________________________________________________
void SpdSmplGeoRoot::BuildVertexDetector() {
    //  
    if (!Top_) return;
    
    double   r[3] = { 160., 240., 340. };    
    double  lx[3] = { 30., 43.7,  63.  };
    double  ly[3] = { 0.5,  0.5,  0.5  };
    double  lz[3] = { 800., 960., 960. }; 
    
    int ns = 36;
    
    TGeoVolume* vol;
    TGeoTranslation trans; 
    TGeoRotation    rot; 
    TGeoCombiTrans* t;
    
    // ++++++++++++++++++ BARREL 
    for (int l(0); l < 3; l++) {
        vol = gGeoManager->MakeBox(FName(Form("VBARREL_%d",l)),FindMedium("silicon"),lx[l]/2.,ly[l]/2.,lz[l]/2.);        
        for (int i(0); i<ns; i++) {
             t = new TGeoCombiTrans(trans,rot);
             t->SetTranslation(0,r[l]+(i%2)*3,0);
             t->RotateZ(i*10);
             AddNode(Top_, vol, i+1, t); /// ADD NODE !!!
        }
        rot.Clear();
    } 
    
    // ++++++++++++++++++ DISKS ++++++++++++++++++
    int nid = 1;
    for (int i(0); i<4; i++) {
         vol = gGeoManager->MakeTubs(FName(Form("VDISK_%d",i)),FindMedium("silicon"), 60, 230+30*i, 1./2, 0, 360);
         AddNode(Top_, vol, nid++, new TGeoTranslation(0.,0.,-440-i*20)); /// ADD NODE !!!
         AddNode(Top_, vol, nid++, new TGeoTranslation(0.,0., 440+i*20)); /// ADD NODE !!!
    }
}

//_____________________________________________________________________________
void SpdSmplGeoRoot::BuildStrawDetectors() {
    //
    if (!Top_) return;
    
    TGeoVolume* vol;
    int nid;
        
    // ++++++++++++++++++ BARREL STRAW (STT) ++++++++++++++++++
    for(int i(0); i<37; i++) {
        vol = gGeoManager->MakeTubs(FName(Form("BSTRAW_%d",i)),FindMedium("silicon"), 380+16*i, 390+16*i, 2300./2, 0, 360);
        AddNode(Top_, vol, i+1, new TGeoTranslation()); /// ADD NODE !!!           
    }
    
    // ++++++++++++++++++ ENDCAP STRAW (ECT) ++++++++++++++++++
    nid = 1;
    vol = gGeoManager->MakeTubs(FName(Form("ESTRAW")),FindMedium("silicon"),350, 1100, 10./2, 0, 360);
    for (int i(0); i<32; i++) {
        AddNode(Top_, vol, nid++, new TGeoTranslation(0.,0., 1630+15*i)); /// ADD NODE !!!  
        AddNode(Top_, vol, nid++, new TGeoTranslation(0.,0.,-1630-15*i)); /// ADD NODE !!!  
    }    
}

//_____________________________________________________________________________
void SpdSmplGeoRoot::BuildEcalTOFDetectors() {
    //
    if (!Top_) return;
    
    TGeoVolume* vol;
  
    // ++++++++++++++++++ BARREL TOF/RPC  ++++++++++++++++++
    vol = gGeoManager->MakeTubs(FName(Form("TOF")),FindMedium("silicon"), 980, 1080, 2300./2, 0, 360);
    AddNode(Top_, vol, 1, new TGeoTranslation()); /// ADD NODE !!!  
    
    // ++++++++++++++++++ BARREL ECAL (ECAl) ++++++++++++++++++
    vol = gGeoManager->MakeTubs(FName(Form("BECAL")),FindMedium("silicon"), 1100, 1600, 3200./2, 0, 360);
    AddNode(Top_, vol, 1, new TGeoTranslation()); /// ADD NODE !!! 
  
    // ++++++++++++++++++ ENDCAP ECAL (ECAl)++++++++++++++++++
    vol = gGeoManager->MakeTubs(FName(Form("EECAL")),FindMedium("silicon"), 350, 1750, 500/2, 0, 360);
    AddNode(Top_, vol, 1, new TGeoTranslation(0., 0., 2380)); /// ADD NODE !!!
    AddNode(Top_, vol, 2, new TGeoTranslation(0., 0.,-2380)); /// ADD NODE !!!             
}

//_____________________________________________________________________________
void SpdSmplGeoRoot::BuildEnvDetectors() {
    //
    if (!Top_) return;
    
    TGeoVolume* vol;
 
    // ++++++++++++++++++ SOLENOID ++++++++++++++++++
    vol = gGeoManager->MakeTubs(FName(Form("SOLENOID")),FindMedium("silicon"), 1650, 2000, 3200./2, 0, 360);
    AddNode(Top_, vol, 1, new TGeoTranslation()); /// ADD NODE !!!
    
    // ++++++++++++++++++++ PIPE ++++++++++++++++++++
    vol = gGeoManager->MakeTubs(FName(Form("ADD")),FindMedium("vacuum"), 0, 149, 2860/2, 0, 360);
    AddNode(Top_, vol, 1, new TGeoTranslation(0,0, 1620)); /// ADD NODE !!!
    AddNode(Top_, vol, 2, new TGeoTranslation(0,0,-1620)); /// ADD NODE !!!
    
    //vol = gGeoManager->MakeTubs(FName(Form("ADD_%d",3)),FindMedium("silicon"),0, 38, 260/2, 0, 360);
    //vol = gGeoManager->MakeTubs(FName(Form("ADD_%d",4)),FindMedium("silicon"),150.1, 2330, 6000/2, 0, 360);    
    //AddNode(Top_, vol, 1, new TGeoTranslation()); /// ADD NODE !!!
    //AddNode(Top_, vol, 1, new TGeoTranslation()); /// ADD NODE !!!
   
}

///+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
///+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
///+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
///+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
///+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________
void SpdSmplGeoRoot::SetDefaultMedia() {
    //    
    DefMPars_[0] = 0.;    // isvol  - Not used
    DefMPars_[1] = 0.;    // ifield - User defined magnetic field
    DefMPars_[2] = 10.;   // fieldm - Maximum field value (in kiloGauss)
    DefMPars_[3] = -20.;  // tmaxfd - Maximum angle due to field deflection 
    DefMPars_[4] = -0.01; // stemax - Maximum displacement for multiple scat 
    DefMPars_[5] = -.3;   // deemax - Maximum fractional energy loss, DLS 
    DefMPars_[6] = .001;  // epsil  - Tracking precision
    DefMPars_[7] = -.8;   // stmin
    
    for (int i(8); i<DEFAULT_MED_PARS; ++i) DefMPars_[i] = 0.;
    
    /// Set materials: Name, A, Z, Density, Unique id, radlen, intlen 
    
    gGeoManager->Material("VACUUM",   0.0000,   0.,  0.000,  0,   0.,  0.);    
    gGeoManager->Material("C",       12.0110,   6.,  2.267,  6,   0.,  0.); //graphite
    gGeoManager->Material("Al",      26.9815,  13.,  2.700, 13,   0.,  0.);
    gGeoManager->Material("Si",      28.0855,  14.,  2.33,  14,   0.,  0.);
    gGeoManager->Material("Fe",      55.845,   26.,  7.874, 26,   0.,  0.);
    gGeoManager->Material("Cu",      63.5460,  29.,  8.960, 29,   0.,  0.);
        
    /// Set mediums: Name, id, material, parameters 
    
    new TGeoMedium("vacuum",0,gGeoManager->GetMaterial("VACUUM"),DefMPars_);
    new TGeoMedium("carbon",6,gGeoManager->GetMaterial("C"),DefMPars_);   
    new TGeoMedium("aluminium",13,gGeoManager->GetMaterial("Al"),DefMPars_);
    new TGeoMedium("silicon",14,gGeoManager->GetMaterial("Si"),DefMPars_);
    new TGeoMedium("iron",26,gGeoManager->GetMaterial("Fe"),DefMPars_);
    new TGeoMedium("copper",29,gGeoManager->GetMaterial("Cu"),DefMPars_);
     
    /// It is not ArCO2 (70/30), just for file records!
    new TGeoMedium("arco27030",1000,gGeoManager->GetMaterial("VACUUM"),DefMPars_);

}

//_____________________________________________________________________________
bool SpdSmplGeoRoot::AddNode(TGeoVolume* mother, TGeoVolume* vol, int node_id, TGeoMatrix* matrix) {
    //
    if (!mother || !vol) return false;

    mother->AddNode(vol,node_id,matrix);  
    
    //FILE* f = stdout;
    FILE* f = FGeom_;
    
    if (!f) return true;
        
    fprintf(f,"%s\n",Delim_.Data());                
    fprintf(f,"%s#%d\n",vol->GetName(),node_id);     // node name
    fprintf(f,"%s\n",mother->GetName());             // mother volume name 
    fprintf(f,"%s\n",GetShape(vol).Data());          // shape type 
    fprintf(f,"%s\n",vol->GetMedium()->GetName());   // medium type 
    
    AddShapeData(vol,f);                             // shape parameters
    
    const double* x = matrix->GetTranslation();      // node translation
    fprintf(f,"%.6f  %.6f  %.6f \n",x[0],x[1],x[2]); 
    
    x = matrix->GetRotationMatrix();                 // node rotation
    fprintf(f,"%.6f  %.6f  %.6f \n",x[0],x[1],x[2]);
    fprintf(f,"%.6f  %.6f  %.6f \n",x[3],x[4],x[5]);
    fprintf(f,"%.6f  %.6f  %.6f \n",x[6],x[7],x[8]);
   
    return true;
}
 
//_____________________________________________________________________________
TGeoMedium* SpdSmplGeoRoot::FindMedium(TString name) { 
    //    
    return gGeoManager->GetMedium(name.Data());       
} 

//_____________________________________________________________________________
TString SpdSmplGeoRoot::GetShape(TGeoVolume* vol) {
    //
    if (!vol) return "UNKNOWN";   

    TString sname = vol->GetShape()->ClassName();
   
    if (sname == "TGeoBBox") return "BOX";
    if (sname == "TGeoTube") return "TUBE";
    if (sname == "TGeoTubeSeg") return "TUBS";
    return "UNKNOWN";
}

//_____________________________________________________________________________
bool SpdSmplGeoRoot::AddShapeData(TGeoVolume* vol, FILE* f) {
    // 
    if (!vol  || !f) return false;     
    TGeoShape* shape = vol->GetShape();
    TString sname = shape->ClassName();
    
    if (sname == "TGeoBBox") {
        double dx = ((TGeoBBox*)shape)->GetDX();
        double dy = ((TGeoBBox*)shape)->GetDY();
        double dz = ((TGeoBBox*)shape)->GetDZ();   
        fprintf(f,"%.6f  %.6f  %.6f \n", dx,-dy,-dz);
        fprintf(f,"%.6f  %.6f  %.6f \n", dx, dy,-dz);
        fprintf(f,"%.6f  %.6f  %.6f \n",-dx, dy,-dz);       
        fprintf(f,"%.6f  %.6f  %.6f \n",-dx,-dy,-dz);
        fprintf(f,"%.6f  %.6f  %.6f \n", dx,-dy, dz);
        fprintf(f,"%.6f  %.6f  %.6f \n", dx, dy, dz);        
        fprintf(f,"%.6f  %.6f  %.6f \n",-dx, dy, dz);
        fprintf(f,"%.6f  %.6f  %.6f \n",-dx,-dy, dz);        
        return true;     
    }
    if (sname == "TGeoTube") {     
        double rmin = ((TGeoTube*)shape)->GetRmin();
        double rmax = ((TGeoTube*)shape)->GetRmax();
        double dz   = ((TGeoTube*)shape)->GetDZ();
        fprintf(f,"%.6f  %.6f  %.6f \n", 0.,0.,-dz);
        fprintf(f,"%.6f  %.6f \n", rmin,rmax);
        fprintf(f,"%.6f  %.6f  %.6f \n", 0.,0.,dz);
        return true;    
    }
    if (sname == "TGeoTubeSeg") {     
        double rmin = ((TGeoTubeSeg*)shape)->GetRmin();
        double rmax = ((TGeoTubeSeg*)shape)->GetRmax();
        double dz   = ((TGeoTubeSeg*)shape)->GetDZ();
        double phi1 = ((TGeoTubeSeg*)shape)->GetPhi1();
        double phi2 = ((TGeoTubeSeg*)shape)->GetPhi2();        
        fprintf(f,"%.6f  %.6f  %.6f \n", 0.,0.,-dz);
        fprintf(f,"%.6f  %.6f \n", rmin,rmax);
        fprintf(f,"%.6f  %.6f  %.6f \n", 0.,0.,dz);
        fprintf(f,"%.6f  %.6f \n", phi1,phi2);
        return true;    
    }
    return false;     
} 

//_____________________________________________________________________________
bool SpdSmplGeoRoot::OpenFile(TString& path, TString& name, TString type) {
    //
    FILE* f;
  
    if (type == "Medium") f = FMedia_;
    else if (type == "Geometry") f = FGeom_;
    else return false;
    
    if (f) { fclose(f); f = 0; }
    
    if (name.IsWhitespace()) {
        cout << "<SpdSmplGeoRoot::OpenFile> bad file name: " << name << endl;
        return false;
    }
    
    if (!path.EndsWith("/")) path += "/";
     
    if (gSystem->AccessPathName(path)) {
        cout << "<SpdSmplGeoRoot::OpenFile> no such directory: " << path << endl;
        return false;
    }
    else {
        name = path + name;    
        f = fopen(name.Data(),"w");
        if (!f) {
            cout << "<SpdSmplGeoRoot::OpenFile> file " << name << " cannot be created." << endl;
            return false;
        }
        if (type == "Medium") FMedia_ = f; 
        else if (type == "Geometry") FGeom_ = f; 
        return true;
    }
 
    return false;
}
















