#include <Riostream.h>
#include <TCanvas.h>
#include <TH2D.h>
#include <TMarker.h>
#include <TArc.h>
#include <TH2D.h>
#include <TArrayI.h>
#include <TMath.h>
#include <vector>
#include <algorithm>

//#include "SpdSttGeoMapper.h"

SpdSttGeoMapper* mapper = 0;

void CheckGeo();

void CheckSttMapper() {

    //mapper = new SpdSttGeoMapper(1);
    //mapper->GetTube(1,1);
    //mapper->print(1);
      
    mapper = new SpdSttGeoMapper();
   
    mapper->SetScale(10);  
    mapper->SetTubeStep(1);
   
    mapper->InitGeometry(1);
    
    mapper->print(1);
    
    CheckGeo();
}

//____________________________________________________
void CheckGeo() 
{
    if (!mapper) return; 
    cout << "<CheckGeo1> " << endl;
    
    Int_t NT = mapper->GetTotalTubes();   
    if (NT == 0) return;
     
    SpdSttStrawTube* tube = mapper->GetTube(); 
    if (!tube) return;
    
      
    TCanvas *cv = new TCanvas("cv", "cv",900, 900);
    cv->Draw();
    
    TH2D *h2 = new TH2D("h2", "", 220, -110, 110, 220, -110, 110);
    h2->SetStats(kFALSE);
    h2->Draw();
    
    Double_t BarrelRmin, BarrelRmax;
    
    SpdParameter* par;
    
    par = mapper->GetParameter("barrel_rmin");
    if (par) par->Value(BarrelRmin);
    
    par = mapper->GetParameter("barrel_rmax");
    if (par) par->Value(BarrelRmax);
    
    TArc *arc2 = new TArc(0, 0, BarrelRmax);
    arc2->Draw("SAME");   
    
    TArc *arc1 = new TArc(0, 0, BarrelRmin);
    arc1->Draw("SAME");   
  
    TMarker *mrk = 0;
    TArc *arc = 0;
    
    Int_t cols[2] = {kYellow,kOrange+1};
    
    Double_t x, y;
    Double_t R = tube->GetTubeOutRad();
    
    for (Int_t i(1); i<=NT; i++) {
         tube = mapper->GetTube(i);
         x = tube->GetXSource();
         y = tube->GetYSource();
          
         arc = new TArc(x, y, R);
         arc->SetFillColor(cols[i%2]);
         arc->Draw("SAME");
         
         mrk = new TMarker(x,y,6);
         mrk->SetMarkerColor(3);
         mrk->Draw("SAME");
    }
    
    cv->Draw();
 
}

