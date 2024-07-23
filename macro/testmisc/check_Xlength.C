//
// SpdRoot ROOT script.
// 

//MpdEmcGeoParWrapper* geo_;

Double_t GetNRadiationLength(Int_t iz, Int_t iphi); 

//______________________________________________________________________________
void check_Xlength() {
  //  
  TString fname = "geofile_full.root";

  TFile* f = new TFile(fname);
  if (!f) return;
  
  cout << "<check_Xlength> FILE: Ok! " << endl;
 
  (TGeoManager*)f->Get("FAIRGeom");
  
  //gGeoManager->SetVisLevel(4);
  //gGeoManager->GetMasterVolume()->Draw();
  
  if (!gGeoManager) return;
  
  cout << "<check_Xlength> GeoManager: Ok! " << endl;
   
  geo_ = new MpdEmcGeoParWrapper(new MpdEmcGeoPar,kTRUE);
  
  //cout << "<check_Xlength> Emc geopars: Ok! " << endl;
  
   if (!geo_) return;
  
  geo_->Print();
  
  cout << "\nPhi cell numbers (min, max):   0, " << geo_->GetNPhi()-1 << endl;
  cout << "  Z cell numbers (min, max):   0, " << geo_->GetNZ()-1 << "\n\n";
 
   Double_t rl;
  
  //rl = GetNRadiationLength(138/2-1 /*z*/, 0 /*phi*/);
  //rl = GetNRadiationLength(138/2   /*z*/, 0 /*phi*/);
  rl = GetNRadiationLength(56   /*z*/, 44 /*phi*/);
 
}

//______________________________________________________________________________
Double_t GetNRadiationLength(Int_t iz, Int_t iphi) 
{
   TGeoNavigator* ntor = gGeoManager->GetCurrentNavigator();
   if (!ntor) return 0;
   //cout << "<check_X1> GeoNavigator: Ok! " << endl;
       
   Float_t X0(0.0), Y0(0.0), Z0(0.0);  // !!! first point !!!
   Float_t X1, Y1, Z1;           // !!! last point  !!!
   
   Int_t uid = iphi + iz * geo_->GetNPhi();
   
   // cell position:
   geo_->GetModPos(uid, X1, Y1, Z1);   // -> set last point 
   
   
   TVector3 Dir(X1-X0,Y1-Y0,Z1-Z0);
   
   cout << "-----------------------------------------------------" 
        << "-----------------------------------------------------" << endl;
   cout << "cell id = " << uid << ";  cell [z,phi] = [" << iz << "," << iphi << "]; "
        << "  cell position (cm) =  (" << X1 << ", " << Y1 << ", " << Z1 << ") " << endl; 
   cout << "Distance to cell from the vertex: " << Dir.Mag() << " cm \n";
   
   //Dir.SetXYZ(1,0,0);
   
   Dir.SetMag(1);
   
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
   Double_t len(0), step, radlen;
   const Double_t* cpoint;
   
   printf("\n%-8s %-30s %-15s  %8s %8s  %8s  %8s %8s %8s\n\n", 
          "N_STEP","NODE","MEDIUM","LENGTH","STEP","RADLEN","POS(x)","POS(y)","POS(z)");
   
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
      
      // mat->GetRadLen() returns radiation length in cm !
      if (mat->GetRadLen() > 0) radlen += step/mat->GetRadLen(); 
      
      //cout << "medium: " << mname << "  " << mat->GetRadLen() << endl;
      
      len += step;
      
      printf("%-8d %-30s %-15s  %8.3f %8.3f  %8.3f  %8.3f %8.3f %8.3f\n",
             ++n,nname.Data(),mname.Data(),
             len, step, radlen, 
             cpoint[0],cpoint[1],cpoint[2]);
              
      if (cpath.Contains("emc")) {
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



