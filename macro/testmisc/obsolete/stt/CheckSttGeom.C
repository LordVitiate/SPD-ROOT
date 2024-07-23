
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

struct TubeDat {

 TubeDat(int nn, int nr, double xx, double yy): nnode(nn),nring(nr),x(xx),y(yy){ SetNeighbourings(); }
  int nnode;
  int nring;
  double x;
  double y;  
  std::vector<int> nn;
  
  void print(int n) {
      if (nring != Ring()) cout << "error!!! " << nnode << endl;
    
      cout << n << " nn = " << nnode  << " nr = " << nring 
           << " >>> " << Ring() 
           << " Sector = " << Sector() 
           << " Is V? = " << IsVertex() << ":: " << nnode << " " << Nprev() << " " << Sector() << " ==";
           for (int i(0); i<6; i++) {
               cout << nn[i] << " ";
           }
           cout << endl;
  } 
  
  int  Ring()     { return 0.5*(TMath::Sqrt(1+4*(nnode-1)/3.)+1); }
  int  Sector()   { int n = Ring(); return (nnode-3*n*(n-1)-1)/n; }
  bool IsVertex() { int n = Ring(); return !((nnode-3*n*(n-1)-1) % n); }
  
  int  Nnext()    { int n = Ring(); return 6*(n+1); }
  int  Nthis()    { int n = Ring(); return 6*n; }
  int  Nprev()    { int n = Ring(); return 6*(n-1); }
  
  bool IsNeighbouring(int v) { return std::find(nn.begin(),nn.end(),v) != nn.end(); }
  
  void SetNeighbourings() {
      int n = Ring();
      int ns = Sector();
      nn.assign(6,0);
 
      if (IsVertex()) {
          nn[0] = nnode-Nprev()-ns;
          nn[1] = nnode+1;       
          nn[3] = nnode+Nthis()+ns;
          nn[4] = nnode+Nthis()+ns+1;
            
          if (ns != 0) {
             nn[2] = nnode-1;
             nn[5] = nnode+Nthis()+ns-1; 
          }
          else {
             nn[2] = nnode+Nthis()-1;
             nn[5] = nnode+Nthis()+Nnext()-1; 
          }
      }
      else {
          nn[0] = nnode-Nprev()-ns-1;
          nn[1] = nnode-Nprev()-ns;
          nn[2] = nnode+1;
          nn[3] = nnode-1;
          nn[4] = nnode+Nthis()+ns;
          nn[5] = nnode+Nthis()+ns+1; 
      }
     
      std::sort(nn.begin(),nn.end());

  }
  
};

int NRmin_;
int NRmax_;

vector <TubeDat*> Tubes_;

//________________________________________________________________________________
void CheckSttGeom() {
    //
    double cc = 10;
    
    double rstraw = 0.25*cc; // straw radius (cm)
    double lstraw = 200.0;   // straw length (cm)
    double l = 1.0*cc;       // distance between straws ( >= 2*rstraw )

    double Rmin = 25.0;      // min barrel radius (cm)
    double Rmax = 100.;      // max barrel radius (cm)
 
    const double c = 0.5*TMath::Sqrt(3);
    
    double k[6][2] = { {-0.5,c}, {-1.0,0}, {-0.5,-c}, {0.5,-c}, { 1.0, 0}, {0.5, c} };
    double V[6][2] = { { 1.0,0}, { 0.5,c}, {-0.5, c}, {-1.0,0}, {-0.5,-c}, {0.5,-c} };
    
    int n = Rmin/l;
    int m(0), nm(0);
    double x, y, r_plus, r_minus;
    bool inside;
    

    int nmin = -1, nmax(0);
    int mmin = -1, mmax(0);
    int s = n;
    int nd = 0;
    
    nm = 3*n*(n-1);
    
    TCanvas *cv = new TCanvas("cv", "cv",900, 900);
    cv->Draw();
    TH2D *h2 = new TH2D("h2", "", 220, -110, 110, 220, -110, 110);
    h2->SetStats(kFALSE);
    h2->Draw();
    
    TArc *arc2 = new TArc(0, 0, Rmax);
    arc2->Draw("SAME");   
    
    TArc *arc1 = new TArc(0, 0, Rmin);
    arc1->Draw("SAME");   
  
    TMarker *mrk = 0;
    TArc *arc = 0;
    
    
    TubeDat* tdat;
    
    int cols[2] = {kYellow,kOrange+1};
    
    while (true) {
         m = 0;
       
         inside = false;
         for (int i(0); i<6; i++) {    
              for (int j(0); j<s; j++) {
                   x = l*(s*V[i][0]+j*k[i][0]);
                   y = l*(s*V[i][1]+j*k[i][1]);
                   r_minus = TMath::Sqrt(x*x+y*y)-rstraw;
                   r_plus =  TMath::Sqrt(x*x+y*y)+rstraw;
                   nm++;
                   cout << nm << " ";
                   if (r_plus <= Rmax) {
                       inside = true;
                       if (r_minus >= Rmin) {
                           nd++;
                           arc = new TArc(x, y, rstraw);
                           arc->SetFillColor(cols[s%2]);
                           arc->Draw("SAME");
                           mrk = new TMarker(x,y,6);
                           mrk->SetMarkerColor(3);
                           mrk->Draw("SAME");
                           
                           tdat = new TubeDat(nm,s,x,y);
                           Tubes_.push_back(tdat);
                           
                           if (mmin < 0) mmin = nm;
                           mmax = nm;
                           m++;
                           
                           cout << "("<< nm << ") ";                   
                       }
                   }              
              }
         }
         cout << endl;
         if ( m > 0 ) {
              nmax = s;
              if ( nmin < 0 ) nmin = s; 
         }
         if (inside) {
             cout << " >>>> " << s << " " << m  << " " << nm << endl;    
             s++;
         }
         else break;
    }
    
    cout << "n = " << n << endl;
    cout << "nodes inside = " << nd << "/" << nm<< endl;
    cout << "min. node number = " << mmin << " max. node number = " << mmax << endl;
    cout << "min. ring = " << nmin << " max. ring = " << nmax << endl;
    
    NRmin_ = nmin;
    NRmax_ = nmax;
    
    //cout << Tubes_.size() << endl;
    //for (uint i(0); i<Tubes_.size(); i++) {
    //     Tubes_[i]->print(i+1);
    //}
    
    int K = 109;
    
    
    while ( K != -1 ) {
    
       cout << "WHICH tube do you want to test? [to exit type -1] " << endl;
       cout << "allowable node numbers = [ 1 , " << nd << "] " << endl; 
       cin >> K;
       if(K == -1) exit(0);
       cout << "TESTING tube " << K << endl;
       
       if (K < 1 || K > int(Tubes_.size()) ) {
         cout << "this tube does not exist" << endl; 
         continue; 
       }
       
         
       Tubes_[K-1]->print(K);
  
       
       arc = new TArc(Tubes_[K-1]->x, Tubes_[K-1]->y, rstraw);
       arc->SetFillColor(kRed);
       arc->Draw("SAME");
   
       for (uint i(0); i<Tubes_.size(); i++) {
           if (Tubes_[K-1]->IsNeighbouring(Tubes_[i]->nnode)) {
              Tubes_[i]->print(i+1);
              x = Tubes_[i]->x;
              y = Tubes_[i]->y;
              arc = new TArc(x, y, rstraw);
              arc->SetFillColor(kBlack);
              arc->Draw("SAME");
              mrk = new TMarker(x,y,6);
              mrk->SetMarkerColor(3);
              mrk->Draw("SAME");
           }
       }
       
       cout << endl;
      
       cv->Update();
       cv->Modified();
       cv->Draw();
    
    }
    
}


