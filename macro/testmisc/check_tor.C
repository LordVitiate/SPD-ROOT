
#include <TGeoManager.h>

#include <TMath.h>

using namespace TMath;

TGeoManager* geom = 0;
TGeoVolume*  Top = 0;

TGeoArb8* MakeArb(const char* name, TGeoMedium* med, 
                  double dz, double angle, double hmin, double hmax, 
                  double cutoff, int col);

void MakeBarrel(const char* name, TGeoMedium* med, int nsec, 
                double dz, double hmin, double hmax, 
                double cutoff, int col);

void MakeBarrel_1(const char* name_c, const char* name_s, TGeoMedium* med, int nsec, 
                  double dz, double hmin_c, double hmin_s, double hmax, 
                  double cutoff , int col);

//_______________________________________________________________________
void check_tor() 
{
  geom = new TGeoManager("Toroidal geometry","Toroidal geometry");

  TGeoMaterial* vacuum = new TGeoMaterial("vacuum",0,0,0);
  TGeoMaterial* Fe = new TGeoMaterial("Fe",55.845, 26, 7.87);
 
  TGeoMedium* Air = new TGeoMedium("Vacuum",0,vacuum);
  TGeoMedium* Iron = new TGeoMedium("Iron",1,Fe);
 
  Top = geom->MakeBox("Top",Air,1000,1000,1000); // cm
  geom->SetTopVolume(Top);
  geom->SetTopVisible(0);
  
  //-----------------------------
  TGeoVolume *Cone = geom->MakeCone("Cone",Air,350+50,0,300+50,0,300+50);
  Cone->SetFillColor(35);
  Cone->SetLineColor(35);
  Cone->SetTransparency(50);
  
  Top->AddNodeOverlap(Cone,1,new TGeoTranslation(0,0,0));
  
  TGeoVolume *Pipe = geom->MakeTube("Pipe",Iron,4.0,5.0,350);
  Pipe->SetFillColor(kBlack);
  Pipe->SetLineColor(kBlack);
  Pipe->SetTransparency(30);
  
   
  Top->AddNodeOverlap(Pipe,1,new TGeoTranslation(0,0,0));
  
  int nseg = 8;
  Double_t coff= 10.;
  
   MakeBarrel_1("Tracker_c","Tracker_s",  Air, nseg, 75, 50, 25, 150, coff, kRed);
   MakeBarrel("Calorimeter", Air, nseg, 75, 160, 250, coff, kBlue);
   MakeBarrel("RangeSystem", Air, nseg, 75, 260, 300, coff, kGreen);
  
  //-----------------------------
  
  geom->CloseGeometry();
 
 
  //Pipe->SetVisibility(0);
  //Cone->SetVisibility(1);
  
  //Top->Draw();
  //Top->Draw("ogl");
 
  geom->SetVisLevel(1);
  geom->GetMasterVolume()->Draw("ogl");
}

//________________________________________________________________________________
TGeoVolume* MakeArb(const char* name, TGeoMedium* med, 
                    double dz, double angle, double hmin, double hmax, double cutoff)
{
  if (!geom) return 0;
    
  angle *= 0.5*DegToRad();
  
  double v[16];
  
  double a = hmin*Tan(angle)-.5*cutoff/Cos(angle);  // < 0 ?
  double b = hmax*Tan(angle)-.5*cutoff/Cos(angle);  // < 0 ?
  
  v[0] =  a; v[1] = hmin;
  v[2] = -a; v[3] = hmin;
  v[4] = -b; v[5] = hmax;
  v[6] =  b; v[7] = hmax;
  
  v[8]  =  a; v[9]  = hmin;
  v[10] = -a; v[11] = hmin;
  v[12] = -b; v[13] = hmax;
  v[14] =  b; v[15] = hmax;

  TGeoVolume* vol = geom->MakeArb8(name,med,dz,v);
  
  return vol;
}

//________________________________________________________________________________
void MakeBarrel(const char* name, TGeoMedium* med,
                int nsec, double dz, double hmin, double hmax, double cutoff, int col)
{
  if (!geom) return;
  if (!Top) return;
  
  if (nsec < 3) return;
  
  double angle = 360./nsec;
 
  TGeoVolume *Arb = MakeArb(name,med,dz,angle,hmin,hmax,cutoff);
  
  Arb->SetFillColor(col);
  Arb->SetLineColor(col);
  Arb->SetTransparency(30);
  
  TGeoRotation* rot;
  int n(0), n1, n2;
  
  for (int i(0); i<nsec; i++) {
       n++;
       rot = new TGeoRotation(Form("t%d",n),i*angle,0,0);
       Top->AddNodeOverlap(Arb,n,rot);
       
       n1 = n+nsec;
       rot = new TGeoRotation(Form("t%d",n1),i*angle,0,0);
       Top->AddNodeOverlap(Arb,n1,new TGeoCombiTrans(0,0,-160,rot));
       
       n2 = n+2*nsec;
       rot = new TGeoRotation(Form("t%d",n2),i*angle,0,0);
       Top->AddNodeOverlap(Arb,n2,new TGeoCombiTrans(0,0,160,rot));
  }
}


//________________________________________________________________________________
void MakeBarrel_1(const char* name_c, const char* name_s, TGeoMedium* med, int nsec, 
                  double dz, double hmin_c, double hmin_s, double hmax, 
                  double cutoff, int col)
{
  if (!geom) return;
  if (!Top) return;
  
  if (nsec < 3) return;
  
  double angle = 360./nsec;
 
  TGeoVolume *Arb_c = MakeArb(name_c,med,dz,angle,hmin_c,hmax,cutoff);
  
  Arb_c->SetFillColor(col);
  Arb_c->SetLineColor(col);
  Arb_c->SetTransparency(30);
  
  TGeoVolume *Arb_s = MakeArb(name_s,med,dz,angle,hmin_s,hmax,cutoff);
  
  Arb_s->SetFillColor(col);
  Arb_s->SetLineColor(col);
  Arb_s->SetTransparency(30);
  
  TGeoRotation* rot;
  int n(0), n1, n2;
  
  for (int i(0); i<nsec; i++) {
       n++;
       rot = new TGeoRotation(Form("t%d",n),i*angle,0,0);
       Top->AddNodeOverlap(Arb_c,n,rot);
       
       n1 = n+nsec;
       rot = new TGeoRotation(Form("t%d",n1),i*angle,0,0);
       Top->AddNodeOverlap(Arb_s,n1,new TGeoCombiTrans(0,0,-160,rot));
       
       n2 = n+2*nsec;
       rot = new TGeoRotation(Form("t%d",n2),i*angle,0,0);
       Top->AddNodeOverlap(Arb_s,n2,new TGeoCombiTrans(0,0,160,rot));
  }
}



