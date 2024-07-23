//
// SpdRoot ROOT script.
// 

Double_t GetNRadiationLength(Double_t maxlen /*cm*/, 
                             Double_t theta /*deg*/ , Double_t phi /*deg*/, Double_t z = 0); 

//______________________________________________________________________________
void check_Xlength2() {
  //  
  TString fname = "geofile_full.root";

  TFile* f = new TFile(fname);
  if (!f) return;
  
  cout << "<check_Xlength> FILE: Ok! " << endl;
 
  (TGeoManager*)f->Get("FAIRGeom");
  
  Double_t rl;
  
  rl = GetNRadiationLength(400,80,5,0);
  //rl = GetNRadiationLength(400,90,22.5,0);
 
}

//______________________________________________________________________________
Double_t GetNRadiationLength(Double_t maxlen, Double_t theta, Double_t phi, Double_t z) 
{
   TGeoNavigator* ntor = gGeoManager->GetCurrentNavigator();
   if (!ntor) return 0;
   
   //cout << "<check_X1> GeoNavigator: Ok! " << endl;
       
   Double_t X0(0.0), Y0(0.0), Z0(z);  // !!! first point !!!
  
   theta *= TMath::DegToRad();
   phi   *= TMath::DegToRad();
  
   TVector3 Dir;
   Dir.SetMagThetaPhi(1,theta,phi);

   //Dir.SetXYZ(1,0,0);
   
   cout << "-----------------------------------------------------" 
        << "-----------------------------------------------------" << endl;
   cout << "start position (cm) =  (" << X0 << ", " << Y0 << ", " << Z0 << ") " << endl; 
   cout << "distance: " << maxlen << " cm \n";
   
   TGeoNode *cnode, *next;
   TGeoMaterial* mat;
   TString nname, mname, cpath;
   
   ntor->SetCurrentPoint(X0,Y0,Z0);
   
   cnode = ntor->SearchNode();
   
   if (!cnode) {
       cout << " Point (" << X0 << ", " << Y0 << ", " << Z0
            << ") is out of boundaries " << endl;
            return 0;      
   }
   else {
       cout << "START POINT: (" << X0 << ", " << Y0 << ", " << Z0 << ") " << endl;
       cout << "START NODE: " << cnode->GetName() << " PATH: " << ntor->GetPath() << endl;     
   }
   
   ntor->SetCurrentDirection(Dir.X(),Dir.Y(),Dir.Z());
    
   int n(0);
   Double_t len(0), step, radlen, dens;
   const Double_t* cpoint;
   
   printf("\n%-8s %-25s %-15s  %8s %8s %8s  %8s  %8s %8s %8s %8s \n\n", 
          "N_STEP","NODE","MEDIUM","DENSITY","LENGTH","STEP","RADLEN","POS(x)","POS(y)","POS(z)","PATH");
   
   radlen = 0;
   while (true) {
      
      ntor->SetCurrentDirection(Dir.X(),Dir.Y(),Dir.Z());
     
      next = ntor->FindNextBoundaryAndStep(); // *** !!! *** 
     
      cpath  = ntor->GetPath();
      cpoint = ntor->GetCurrentPoint(); 
       
      nname = cnode->GetName();
      mname = cnode->GetMedium()->GetName();
      
      mat = cnode->GetMedium()->GetMaterial();
      step = ntor->GetStep();
      
      dens = mat->GetDensity();
      
      // mat->GetRadLen() returns radiation length in cm !
      if (mat->GetRadLen() > 0) radlen += step/mat->GetRadLen(); 
      
      //cout << "medium: " << mname << "  " << mat->GetRadLen() << endl;
      
      len += step;
      
      printf("%-8d %-25s %-15s  %8.3e %8.3f %8.3f  %8.3f  %8.3f %8.3f %8.3f   %s\n",
             ++n,nname.Data(),mname.Data(),
             dens, len, step, radlen, 
             cpoint[0],cpoint[1],cpoint[2],cpath.Data());
            
      if (len > maxlen) {
          cout << "\nLAST NODE: " << cpath 
               << "  LAST POSITION: (" << cpoint[0] << ", " << cpoint[1] << ", " << cpoint[2] << ") "<< endl;
          cout << "TOTAL NUMBER OF RAD.L. = " << radlen << endl;     
          break;
      }
            
      if (!next) break;  
      
      cnode = next;
   }
   
   cout << "\n";
   
   return radlen;
   
}



