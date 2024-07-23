#include <TGeoManager.h>

#include <TMath.h>

using namespace TMath;

TGeoManager* geom = 0;
TGeoVolume*  Top = 0;

//_______________________________________________________________________
void check_tsec() 
{
  geom = new TGeoManager("TSEC","TSEC");

  TGeoMaterial* vacuum = new TGeoMaterial("vacuum",0,0,0);
  TGeoMaterial* Fe = new TGeoMaterial("Fe",55.845, 26, 7.87);
 
  TGeoMedium* Air = new TGeoMedium("Vacuum",0,vacuum);
  TGeoMedium* Iron = new TGeoMedium("Iron",1,Fe);
 
  Top = geom->MakeBox("Top",Air,1000,1000,1000); // cm
  geom->SetTopVolume(Top);
  geom->SetTopVisible(0);
  
  //-----------------------------
  Double_t Width  = 50.;
   
  Double_t MaxR   = 170.;
  Double_t dR     = 0; 
  Double_t BoreXh = 7.;
  Double_t BoreYh = BoreXh; // ?
  
  const Int_t NP = 10;
  Double_t xp[10], yp[10];
  
  Double_t phi0 = 0.125*TwoPi();
  Double_t h0 = dR*Cos(0.5*phi0); // ?
  
  xp[0] = BoreXh+h0; yp[0] = BoreYh+h0;
  xp[1] = BoreXh+h0; yp[1] = h0;
  xp[2] = (MaxR-dR)*Cos(0.5*phi0); yp[2] = h0;
  xp[3] = (MaxR-dR)*Cos(0.5*phi0); yp[3] = (MaxR-dR)*Sin(0.5*phi0);
  xp[4] = (MaxR-dR)*Cos(1.5*phi0); yp[4] = (MaxR-dR)*Sin(1.5*phi0); 
  xp[5] = -xp[4]; yp[5] = yp[4];
  xp[6] = -xp[3]; yp[6] = yp[3];
  xp[7] = -xp[2]; yp[7] = yp[2];
  xp[8] = -xp[1]; yp[8] = yp[1];
  xp[9] = -xp[0]; yp[9] = yp[0];
  
  for (Int_t i(0); i<NP; i++) {
       cout << i << " " << xp[i] << " " << yp[i] << endl; 
  }

  TGeoXtru* xtru;  
  TGeoVolume* module;
  
  module = geom->MakeXtru("Module",Air,2);
  
  //module->SetFillColor(kRed);
  module->SetLineColor(kRed);
  module->SetTransparency(70);
  
  xtru = (TGeoXtru*)module->GetShape();
  xtru->DefinePolygon(NP,xp,yp);
  xtru->DefineSection(0,-0.5*Width);
  xtru->DefineSection(1, 0.5*Width);
  
  Top->AddNodeOverlap(module,1,new TGeoTranslation(0,0,0));
 
  //-----------------------------
  
  geom->CloseGeometry();
 
 
  //Pipe->SetVisibility(0);
  //Cone->SetVisibility(1);
  
  //Top->Draw();
  //Top->Draw("ogl");
 
  geom->SetVisLevel(1);
  geom->GetMasterVolume()->Draw("ogl");
}
