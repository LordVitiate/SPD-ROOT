// $Id$
// Author: artur   2018/05/07


//_____________________________________________________________________________
//
// SpdGeoVolPars
//_____________________________________________________________________________

#include "SpdGeoVolPars.h"

#include <iostream>
using std::cout;
using std::endl;

#include <set>
#include <algorithm>

#include <TMath.h>
using namespace TMath;

ClassImp(SpdGeoVolPars)

// cm -> mm
Double_t SpdGeoVolPars::fRoundingCM   = 1e-1;
Double_t SpdGeoVolPars::fIRoundingCM  = 1e1;

// deg -> mcdeg
Double_t SpdGeoVolPars::fRoundingDEG  = 1e-3; 
Double_t SpdGeoVolPars::fIRoundingDEG = 1e3;

// cm -> mkm
Double_t SpdGeoVolPars::fRoundingCMBase  = 1e-4; 
Double_t SpdGeoVolPars::fIRoundingCMBase = 1e4;

// cm -> nm
Double_t SpdGeoVolPars::fRoundingCMBase2  = 1e-7; 
Double_t SpdGeoVolPars::fIRoundingCMBase2 = 1e7;

//_____________________________________________________________________________
SpdGeoVolPars::SpdGeoVolPars():TNamed("",""),
fMedia("air"),fActivity(0),fNCopies(0),
fLineColor(0),fFillColor(0),fTransparency(0)
{
 
}

//_____________________________________________________________________________
SpdGeoVolPars::SpdGeoVolPars(const Char_t* volname):TNamed(volname,""),
fMedia("air"),fActivity(0),fNCopies(0),
fLineColor(0),fFillColor(0),fTransparency(0)
{
 
}

//_____________________________________________________________________________
SpdGeoVolPars::~SpdGeoVolPars() 
{
 
}

//_____________________________________________________________________________
void SpdGeoVolPars::Clear() 
{ 
   fMedia = "air"; 
   fActivity = 0;
   fNCopies = 0;
   
   ClearNodes();
}

//_____________________________________________________________________________
void SpdGeoVolPars::ClearNodes() 
{ 
   fNodesPars.clear();
   fVolNodes.clear();
}

//_____________________________________________________________________________
TGeoMatrix* SpdGeoVolPars::MakeNodeMatrix(TString volname, Int_t loc_i) const
{
   std::map< TString, std::vector<Int_t> >::const_iterator it;
   it = fVolNodes.find(volname); 
 
   if (it == fVolNodes.end()) return 0;
   if (!(loc_i < it->second.size())) return 0;
 
   return MakeNodeMatrix(it->second[loc_i]);
}

//_____________________________________________________________________________
void SpdGeoVolPars::SetVolName(Int_t nnode, TString volname)
{
   if (nnode >= fNodesPars.size()) return;
   
   if (fNodesPars[nnode].size() == 0) return;
   
   if (fNodesPars[nnode][0] == 1) {
      cout << "-W- <SpdGeoVolPars::SetVolName> Node " << nnode 
           << " is already added " << endl; 
   }
   else fNodesPars[nnode][0] = 1; 
  
   std::map< TString, std::vector<Int_t> >::iterator it;
   
   it = fVolNodes.find(volname);
  
   if (it != fVolNodes.end()) it->second.push_back(nnode);
   else {
       std::vector<Int_t> v;
       v.push_back(nnode);
       fVolNodes.insert(std::pair< TString, std::vector<Int_t> >(volname,v));
   }
}

//_____________________________________________________________________________
Int_t SpdGeoVolPars::GetNNodes() const 
{ 
   return fNodesPars.size(); 
}

//_____________________________________________________________________________  
Int_t SpdGeoVolPars::GetNVolumes() const 
{ 
   return fVolNodes.size();  
}

//_____________________________________________________________________________
Int_t SpdGeoVolPars::GetNNodes(TString volname) const
{
   if (fVolNodes.empty()) return 0;
   std::map< TString, std::vector<Int_t> >::const_iterator it;
   it = fVolNodes.find(volname);
   return (it != fVolNodes.end()) ? it->second.size() : 0;
}

//_____________________________________________________________________________
TString SpdGeoVolPars::GetVolName(Int_t nnode) const
{
   if (fVolNodes.empty()) return "";
   std::map< TString, std::vector<Int_t> >::const_iterator it = fVolNodes.begin();
   for ( ; it != fVolNodes.end(); it++) {
        std::vector<Int_t>& vn = const_cast<std::vector<Int_t>&>(it->second);
        std::vector<Int_t>::iterator itv = find(vn.begin(),vn.end(),0);
        if (itv != it->second.end()) return it->first;
   }
   return "";
}

//_____________________________________________________________________________
void SpdGeoVolPars::Prn(Int_t i, Bool_t endn) const
{
   printf("%3d  %2d  %4d  %-25s  %-20s  %-20s %6d  %6d", 
           i, fActivity, fNCopies, ClassName(), fName.Data(), fMedia.Data(),
           GetNNodes(), GetNVolumes()
          );
   if (endn) printf("\n");
}

//_____________________________________________________________________________
void SpdGeoVolPars::PrintGeoVolPars(Int_t option) const
{
   cout << "<SpdGeoVolPars::PrintGeoVolPars> " << endl;
  
   cout << "Volume parameters type:   " << ClassName()  << endl;
   cout << "Volume name:              " << fName.Data() << endl;
   cout << "Medium:                   " << fMedia.Data() << endl;
   cout << "Activity:                 " << fActivity << endl;
   cout << "Total number of copies:   " << fNCopies << endl;
   cout << "U. Volumes inside (reg.): " << GetNVolumes() << endl;
   cout << "Nodes inside (reg.):      " << GetNNodes() << endl;
  
   cout << endl;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*/////////////////////////////////////////////////////////////////////////////
////////////////////////// STATIC BUILD METHODS /////////////////////////////// 
/////////////////////////////////////////////////////////////////////////////*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//_____________________________________________________________________________
void SpdGeoVolPars::Packing(std::vector< std::vector<Double_t> >& pars,
                            Double_t vertex[], 
                            Double_t min_size, Double_t ds,
                            Double_t angle, Double_t w, Double_t dw)
{
   pars.clear();
   
   //cout << "-I- <SpdGeoVolPars::Packing>\n" <<endl;
   
   //for (Int_t i(0); i<4; i++) {
   //     cout << "\t V[" << i+1 << "]: " 
   //          << vertex[2*i] << ", "<< vertex[2*i+1] << endl;
   //}
   
   //cout << "\n";
   //cout << "\t ds        = " << ds << endl;
   //cout << "\t min. size = " << min_size << endl;
   //cout << "\t angle     = " << angle << endl;
   //cout << "\t width     = " << w << endl;
   //cout << "\t gap       = " << dw << endl;
   //cout << "\n";
   
   angle *= DegToRad(); 
   
   Double_t x, y, vmin(1e20), vmax(-1e20);
    
   for (Int_t i(0); i<4; i++) {
        
        x = vertex[2*i]; 
        y = vertex[2*i+1]; 

        vertex[2*i]    =   Cos(angle)*x + Sin(angle)*y;
        vertex[2*i+1]  = - Sin(angle)*x + Cos(angle)*y;
        
        if (vertex[2*i+1] < vmin) vmin = vertex[2*i+1];
        if (vertex[2*i+1] > vmax) vmax = vertex[2*i+1];
   }
    
   Double_t ax[4], ay[4], az[4], vlim[8];
   Double_t x1, x2, y1, y2;
   Int_t ii, jj;
   
   for (Int_t i(0); i<4; i++) {
        
        ii = 2*((i+1)%4);
        jj = 2*i;
        
        x1 = vertex[jj+1];
        x2 = vertex[ii+1];
	
        y1 = vertex[jj];
        y2 = vertex[ii];
	
        if (x1 < x2) { 
	        vlim[2*i]   = x1;
	        vlim[2*i+1] = x2; 
	    }
	    else  { 
	        vlim[2*i]   = x2; 
	        vlim[2*i+1] = x1; 
        }
            
        ax[i] = x2 - x1;
        
        if (Abs(ax[i]) > 1e-10 )  {        
            ay[i] = y2 - y1;                       
	    (Abs(ay[i]) > 1e-10) ? ay[i] /= ax[i] : ay[i] = 0;	    
            az[i] = (x2*y1 - y2*x1)/ax[i];        
	    ax[i] = 1;
        }
        else {
	    ax[i] = -1;  
	    ay[i] =  1e100;
	    az[i] =  0;    
	}
     
        //cout << "\t " << i << ":  ax = " << ax[i] << " "  
        //     << " ay = " << ay[i] << " az = " << az[i] << " "
        //     << "x(2,1) = vertex[" << ii << ", " << jj << "] " 
        //     << "y(2,1) = vertex[" << ii+1 << ", " << jj+1 << "]  "
	//     << "lim = [" << vlim[2*i] << ", " << vlim[2*i+1] << "] "<< endl;
   }
   
   //cout << "\n\t vmin = " << vmin << " vmax = " << vmax << "\n\n"; 
   
   Double_t x0 = -0.5*w + ds; 
   Double_t l = w + dw;
    
   Int_t nm(0);
   Double_t xx(x0);
   
   if (xx > vmin) {
       while (1) {
          xx -= l;
          if (xx >= vmin) nm--; 
          else break;
       }
   }
   else {
       while (1) {
          xx += l;
          if (xx <= vmin) nm++; 
          else break;
       }
   }
  
   Int_t np = nm;   
    
   Double_t lmin, lmax, h, yy;
    
   std::multiset<Double_t> ll;
   std::multiset<Double_t>::const_iterator il;
 
   while (1) {
     
       x1 = x0 + np*l;
       x2 = x1 + w;
       
       if (x2 > vmax) break;
         
       np++; 
       
       for (Int_t j(0); j<4; j++) {
    
            if (ax[j] > 0)  {
	      
                if ( (vlim[2*j] <= x1) && (x1 <= vlim[2*j+1]) ) ll.insert(ay[j]*x1 + az[j]);
                if ( (vlim[2*j] <= x2) && (x2 <= vlim[2*j+1]) ) ll.insert(ay[j]*x2 + az[j]); 
		
		//cout << "x1, x2 = " << x1 << " " << x2 << " ";
		//cout << "y1, y2 = " << ay[j]*x1 + az[j] << " " << ay[j]*x2 + az[j]  << " ";
		//cout << "v1, v2 = " << vlim[2*j] << " " << vlim[2*j+1] << endl;
	    }
       }
       
       //for (il = ll.begin(); il != ll.end(); il++) cout << " ll = " << *il << endl;
       
       il = ll.begin();
       
       lmin = *(++il);
       lmax = *(++il);
       
       ll.clear();
          
       xx = 0.5*(lmin + lmax);
       yy = 0.5*(x1 + x2);
       
       x = Cos(angle)*xx - Sin(angle)*yy; // -> z
       y = Sin(angle)*xx + Cos(angle)*yy; // -> y
       
       h = lmax - lmin;
       
       if (Abs(x) < 1e-12) x = 0;
       if (Abs(y) < 1e-12) y = 0;
         
       if (h >= min_size) {
           std::vector<Double_t> v(4,0);
           v[0] = -1; // "status"
           v[1] = x;  // -> z,z
           v[2] = y;  // -> y,x
           v[3] = h;
           pars.push_back(v);
       }
       
       //cout << "\t " << np-1 << ":  [" << x1 << " " << x2 << "]  h = " << h 
       //     << "  lmin = " << lmin << " lmax = " << lmax 
       //     << "  x = " << x << " y = " << y << " \n"; 
   }
   
   //cout << "\n\t nm = " << nm  << " np = " << np-1 << " (" << np-nm << ")"<< endl;
   //cout << "\t added nodes: " << pars.size() << "\n\n";
   
}


