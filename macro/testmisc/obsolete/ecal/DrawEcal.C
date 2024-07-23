
void draw_geom(SpdEcalGeoMapper* mapper, Bool_t draw_nums);

void draw_cell_1(SpdEcalGeoMapper* mapper, int mid, int sub_mid, int color);
void draw_cell_2(SpdEcalGeoMapper* mapper, int mid, int sub_mid, int color);

Double_t GetNRadiationLength(Float_t X1, Float_t Y1, Float_t Z1);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void DrawEcal() 
{ 
   TString fname = "geofile_full.root";

   TFile* f = new TFile(fname);
   if (!f) return;

   (TGeoManager*)f->Get("FAIRGeom");
   if (!gGeoManager) return;
  
   TString parFile = "params.root";
   
   FairParRootFileIo* parInput = new FairParRootFileIo(); 
   parInput->open(parFile.Data());
   FairParRootFile* parF = parInput->getParRootFile();
   TKey* key = parF->GetKey("SpdEcalGeoPar");
    
   SpdEcalGeoPar *pars = new SpdEcalGeoPar();      
   key->Read(pars); 
   
   //pars->printParams();
  
   SpdEcalGeoMapper* mapper = SpdEcalGeoMapper::Instance();
   pars->FillParameters(mapper->GetParameters());
   mapper->InitGeometry(pars->GetGeometryType());
  
   mapper->print(0);
   
   draw_geom(mapper,1);  // START!

}

//__________________________________________________________________
void draw_geom(SpdEcalGeoMapper* mapper, Bool_t draw_nums)
{ 
  gStyle->SetPadBottomMargin(0.05);
  gStyle->SetPadLeftMargin(0.05);
  gStyle->SetPadRightMargin(0.02);
  gStyle->SetPadTopMargin(0.02);
   
  gStyle->SetGridStyle(1);
  
  Int_t nbm = mapper->GetCapModulesXY();
  
  TH2D* h = new TH2D("ECAL end-cap","",nbm,0,nbm,nbm,0,nbm);
  TH2D* hh = 0;
  if (draw_nums) hh = new TH2D("ECAL end-cap (n)","",nbm,0,nbm,nbm,0,nbm);
  
  h->GetXaxis()->CenterLabels();
  h->GetXaxis()->SetLabelSize(0.017);
  h->GetXaxis()->SetNdivisions(nbm,false);  
  h->GetYaxis()->CenterLabels();
  h->GetYaxis()->SetNdivisions(nbm,false);
  h->GetYaxis()->SetLabelSize(0.017);
 
   
  Int_t n = mapper->GetCapModules();
  Int_t ix,  iy;
  for (int j(0); j<n; j++) {
       mapper->GetCapModuleId(j,ix,iy);
       h->SetBinContent(ix+1,iy+1,1);
       if (hh) hh->SetBinContent(ix+1,iy+1,j);
  }
  
  h->SetStats(0);
  h->SetFillColor(kGray+3);
  
  if (hh) hh->SetMarkerSize(0.6);
  
  
  TCanvas *cv1 = new TCanvas("ECAL ENDCAP (1)", "ECAL ENDCAP (1)",900, 900);
  cv1->Draw();
  cv1->SetGrid();
  
  h->Draw("box");
  
  draw_cell_1(mapper,338,1,kRed);
  draw_cell_1(mapper,567,2,kRed);
  draw_cell_1(mapper,588,0,kRed);
  draw_cell_1(mapper,134,8,kRed);
  draw_cell_1(mapper,914,7,kRed);
  
  
  draw_cell_1(mapper,712,0,kRed);
  draw_cell_1(mapper,712,1,kRed);
  draw_cell_1(mapper,712,2,kRed);
  draw_cell_1(mapper,712,3,kRed);
  draw_cell_1(mapper,712,4,kRed);
  draw_cell_1(mapper,712,5,kRed);
  draw_cell_1(mapper,712,6,kRed);
  draw_cell_1(mapper,712,7,kRed);
  draw_cell_1(mapper,712,8,kRed);
  
  
  if (hh) {
      hh->SetMarkerColor(kWhite);
      hh->Draw("same text");
  }
  
  TCanvas *cv2 = new TCanvas("ECAL ENDCAP (2)", "ECAL ENDCAP (2)",900, 900);
  cv2->Draw();
  cv2->SetGrid();
  
  h->Draw("box");
  
  draw_cell_2(mapper,338,1,kCyan); 
  draw_cell_2(mapper,567,2,kCyan);
  draw_cell_2(mapper,588,0,kCyan);
  draw_cell_2(mapper,134,8,kCyan);
  draw_cell_2(mapper,914,7,kCyan);

  
  draw_cell_2(mapper,712,0,kCyan);
  draw_cell_2(mapper,712,1,kCyan);
  draw_cell_2(mapper,712,2,kCyan);
  draw_cell_2(mapper,712,3,kCyan);
  draw_cell_2(mapper,712,4,kCyan);
  draw_cell_2(mapper,712,5,kCyan);
  draw_cell_2(mapper,712,6,kCyan);
  draw_cell_2(mapper,712,7,kCyan);
  draw_cell_2(mapper,712,8,kCyan);
  
  if (hh) {
      hh->SetMarkerColor(kWhite);
      hh->Draw("same text");
  }
}

//__________________________________________________________________
void draw_cell_1(SpdEcalGeoMapper* mapper, int mid, int sub_mid, int color) 
{  
  /*
       mid = [0,...,951] - module number
     sub_mid = [0,...,8] - submodule number 
  */
  
  if (mid < 0 || mid >= mapper->GetCapModules()) return;   
  if (sub_mid < 0 || sub_mid >= mapper->GetCapSubModules()) return; 
    
  const int    nb = mapper->GetCapModulesXY();
  const double lsize = mapper->GetCapSizeXY();
  const double scale = nb/lsize;
  
  const double msize = 0.5 * scale * mapper->GetDigitPar("modcell_xysize");
  
  //cout << "scale = " << scale << " size = " << msize << " lsize/2 = " << 0.5*lsize 
  //     << " nb = " << nb <<  endl;
  
  double x, y; // submodule center position!
  
  
  /* -------   !!! GET SUBMODULE CENTER POSITION  !!!  ----*/
  mapper->GetCapSubModuleGPos(mid, sub_mid, x, y); 
  
  x = (x + 0.5 * lsize) * scale;
  y = (y + 0.5 * lsize) * scale;
  
  //cout << " x,y = " << x << " " << y << endl;
  
  const double d = 0.75 * msize;
  TBox* pb = new TBox(x - d, y - d, x + d, y + d);
  
  pb->SetFillColor(color);
  pb->Draw("same");
}


//__________________________________________________________________
void draw_cell_2(SpdEcalGeoMapper* mapper, int mid, int sub_mid, int color) 
{
  /*
     mid = [0,...,951]   - module number
     sub_mid = [0,...,8] - submodule number 
  
     id1 = [0,...,36] - submodule x-index
     id2 = [0,...,36] - submodule y-index 
  */  
  
  int id1, id2;
  
  /* -------   !!! (mid,sub_mid) -> (id1, id2) !!!  ----*/
  mapper->GetCapSubModuleGIPos(mid, sub_mid, id1, id2);
  
  //cout << id1 << " " << id2 << " " << mapper->GetCapNXY() << endl;
  
  if (id1 < 0 || id1 >= mapper->GetCapNXY()) return;   
  if (id2 < 0 || id2 >= mapper->GetCapNXY()) return; 

  /* -------   !!! (id1, id2) -> (mid,sub_mid) !!!  ----*/
  
  //cout << mid << " " << sub_mid << " -> ";
  
  mapper->GetCapSubModuleGIPosRev(id1, id2, mid, sub_mid);
  
  //cout << mid << " " << sub_mid << " "  << endl;
  
  //===========================================================================
  
  if (mid < 0 || mid >= mapper->GetCapModules()) return;   
  if (sub_mid < 0 || sub_mid >= mapper->GetCapSubModules()) return; 
    
  const int    nb = mapper->GetCapModulesXY();
  const double lsize = mapper->GetCapSizeXY();
  const double scale = nb/lsize;
  
  const double msize = 0.5 * scale * mapper->GetDigitPar("modcell_xysize");
  
  //cout << "scale = " << scale << " size = " << msize << " lsize/2 = " << 0.5*lsize 
  //     << " nb = " << nb <<  endl;
  
  double x, y, z; // submodule center position
  
  /* -------   !!! GET SUBMODULE CENTER POSITION !!!  ----*/
  mapper->GetCapSubModuleGPos(mid, sub_mid, x, y); 
  
  Double_t rad_len = 0;

  /* GET RADIATION LENGTH FOR Z- ENDCAP */ 

  z = mapper->GetCapZposition(-1) + 0.5 * mapper->GetDigitPar("cap_thick"); 
  
  rad_len = GetNRadiationLength(x,y,z);
  
  cout << "Z- ENDCAP [M,SM] = [" << mid << " " << sub_mid << "]";
  cout << "  RADIATION LENGTH (Z- CAP) = " << rad_len << endl;
 
   
  /* GET RADIATION LENGTH FOR Z+ ENDCAP */ 
  
  z = mapper->GetCapZposition(1) - 0.5 * mapper->GetDigitPar("cap_thick");
  
  rad_len = GetNRadiationLength(x,y,z);
  
  cout << "Z+ ENDCAP [M,SM] = [" << mid << " " << sub_mid << "]";
  cout << "  RADIATION LENGTH (Z+ CAP) = " << rad_len << endl;
   
  /* DRAW CELLS */
 
  x = (x + 0.5 * lsize) * scale;
  y = (y + 0.5 * lsize) * scale;
  
  //cout << " x,y = " << x << " " << y << endl;
  
  const double d = 0.75 * msize;
  TBox* pb = new TBox(x - d, y - d, x + d, y + d);
  
  pb->SetFillColor(color);
  pb->Draw("same");
   
}

//______________________________________________________________________________
Double_t GetNRadiationLength(Float_t X1, Float_t Y1, Float_t Z1) 
{
   TGeoNavigator* ntor = gGeoManager->GetCurrentNavigator();
   if (!ntor) return 0;
   //cout << "<check_X1> GeoNavigator: Ok! " << endl;
       
   Float_t X0(0.0), Y0(0.0), Z0(0.0);  // !!! first point !!!
   
   TVector3 Dir(X1-X0,Y1-Y0,Z1-Z0);
   
//    cout << "-----------------------------------------------------" 
//         << "-----------------------------------------------------" << endl;
//  
//   cout << "cell position (cm) =  (" << X1 << ", " << Y1 << ", " << Z1 << ") " << endl; 
//   cout << "Distance to cell from the vertex: " << Dir.Mag() << " cm \n";
   
   Dir.SetMag(1);
   
//   cout << "Direction:  (" << Dir.X() << ", " << Dir.Y() << ", " << Dir.Z() << ") " << endl; 
   
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
//       cout << "START POINT: (" << X0 << ", " << Y0 << ", " << Z0 << ") " << endl;
//       cout << "START NODE: " << cnode->GetName() << " PATH: " << ntor->GetPath() << endl;     
   }
   
   ntor->SetCurrentDirection(Dir.X(),Dir.Y(),Dir.Z());
    
   int n(0);
   Double_t len(0), step, radlen;
   const Double_t* cpoint;
   
//   printf("\n%-8s %-35s %-15s  %8s %8s  %8s  %8s %8s %8s\n\n", 
//          "N_STEP","NODE","MEDIUM","LENGTH","STEP","RADLEN","POS(x)","POS(y)","POS(z)");
   
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
      
//       printf("%-8d %-35s %-15s  %8.3f %8.3f  %8.3f  %8.3f %8.3f %8.3f\n",
//              ++n,nname.Data(),mname.Data(),
//              len, step, radlen, 
//              cpoint[0],cpoint[1],cpoint[2]);
              
      if (cpath.Contains("SpdEcal")) {
//           cout << "\nLAST NODE: " << cpath 
//                << "  LAST POSITION: (" << cpoint[0] << ", " << cpoint[1] << ", " << cpoint[2] << ") "
//                << endl;
//           cout << "TOTAL NUMBER OF RAD.L. = " << radlen << endl;     
           break;
      }
              
      if (!next) break;  
      
      cnode = next;
   }
   
   //cout << "\n";
   
   return radlen;  
}





