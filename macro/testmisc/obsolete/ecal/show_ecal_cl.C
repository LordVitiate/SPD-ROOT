
void show_event(TFile* , Int_t ne);
void draw_cl(Int_t type, Int_t nevent, TFile* f);

//______________________________________________________________________________
void show_ecal_cl(Int_t event_num = 1, TString fname = "ecal_cl.root") {
  //  
  TFile* f = new TFile(fname);
  if (!f) return;
  
  cout << "Ok! " << endl;
  
  show_event(f,event_num);
}

//______________________________________________________________________________
void show_event(TFile* f, Int_t nevent) {
   //       
   cout << "+++++++++++++++++ <show_event> ++++++++++++++++++" << endl; 
   cout << "Event = " << nevent << endl;
   
   if (nevent < 1) return;
   
   int mc_cl[3] = {0,0,0};
   int rc_cl[3] = {0,0,0};
     
   TTree* t = (TTree*)f->Get("cbmsim");  
   
   TBranch* branch;
   
   branch = t->GetBranch("SpdEcalClusterInfo0");
   
   if (!branch) {
       cout << "No branch: \"SpdEcalClusterInfo0\"  " << endl;
       return;
   }
   else {
       int bentries =  branch->GetEntries();
       //cout << "Branch entries: \"Event Data\" " << bentries << endl;
       
       TClonesArray* fH;
       t->SetBranchAddress("SpdEcalClusterInfo0",&fH);
       
       if (nevent > 0 || nevent <= bentries) {
           branch->GetEntry(nevent-1);
           cout << "\n============= Print MC-Clusters: " << fH->GetEntriesFast() << "\n\n";
           
           SpdEcalClusterInfo0* cl = 0;
           for (Int_t i(0); i<fH->GetEntriesFast(); i++) {
             
                cl = (SpdEcalClusterInfo0*)fH->At(i);
                
                int type = cl->GetType();
                if (type == -1)  mc_cl[0]++;    // Z- cap
                else if (type == 0) mc_cl[1]++; // barrel
                else if (type == 1) mc_cl[2]++; // Z+ cap
                
                //if (cl->GetType() != -1) continue;
                
                //cout << "MC-CLUSTER: " << i << endl; 
                
                //cl->Print();
                //cl->Print("elements");
                
                if (i == 0) cl->Print("hshortlist",0);
                cl->Print("shortlist",i+1);
                //cl->CheckClusterId();
           } 
           
           if (cl) cl->CheckInfo(fH);
       }     
   }
   
   branch = t->GetBranch("SpdEcalClusterInfo");
   
   if (!branch) {
       cout << "No branch: \"SpdEcalClusterInfo\"  " << endl;
       return;
   }
   else {
       int bentries =  branch->GetEntries();
       //cout << "Branch entries: \"Event Data\" " << bentries << endl;
       
       TClonesArray* fH;
       t->SetBranchAddress("SpdEcalClusterInfo",&fH);
       
       if (nevent > 0 || nevent <= bentries) {
           branch->GetEntry(nevent-1);
           cout << "\n============= Print RC-Clusters: " << fH->GetEntriesFast() << "\n\n";
           
           SpdEcalClusterInfo* cx;
   
           for (Int_t i(0); i<fH->GetEntriesFast(); i++) {
                                        
                cx = (SpdEcalClusterInfo*)fH->At(i);
                
                int type = cx->GetType();
                if (type == -1)  rc_cl[0]++;     // Z- cap
                else if (type == 0) rc_cl[1]++;  // barrel
                else if (type == 1) rc_cl[2]++;  // Z+ cap
                                       
                //cout << "RC-CLUSTER: " << i << endl; 
                //cl->Print();
                //cx->Print("elements");
                
//                if (i == 0) cx->Print("hshortlist",0);
//                cx->Print("shortlist",i+1);
   
        
//                 Float_t* data = cx->GetPoints();
//                 if (!data) continue;
//                 
//                 cout << "------------------------------------" << endl;
//                
//                 for (Int_t j(0), k(0); j<cx->GetNPoints(); j++, k+=3) {
//                      printf("%3d [%4d, %4d] %12.6f\n",
//                             j+1, Int_t(data[k]), Int_t(data[k+1]), data[k+2]); 
//           
//                 }
//                 
//                 delete [] data;
                
//                 Float_t* data = cx->GetPointsData();
//                 if (!data) continue;
//                 
//                 cout << "------------------------------------" << endl; 
//                
//                 for (Int_t j(0), k(0); j<cx->GetNPoints(); j++, k+=4) {
//                      printf("%3d [%4d, %4d] %12.6f %8.3f\n",
//                             j+1, Int_t(data[k]), Int_t(data[k+1]), data[k+2], data[k+3]); 
//           
//                 }
//                  
//                 delete [] data;            
//                 
           }
       }     
   }
   
   printf("\n");
   printf("------------------------------------------------------------------\n");
   printf("BARREL [DET_ID =  0] CLUSTRES(MC,RC): %3d  %3d\n",mc_cl[1],rc_cl[1]);
   printf("CAP Z- [DET_ID = -1] CLUSTRES(MC,RC): %3d  %3d\n",mc_cl[0],rc_cl[0]);
   printf("CAP Z+ [DET_ID = +1] CLUSTRES(MC,RC): %3d  %3d\n",mc_cl[2],rc_cl[2]);
   printf("\n");
   
   draw_cl(0,nevent,f);
   draw_cl(1,nevent,f);
   draw_cl(-1,nevent,f);
}

//______________________________________________________________________________  
void draw_cl(Int_t type, Int_t nevent, TFile* f) 
{ 
   const Char_t* st = Form("[%2d]",type);
  
   TH2D* h1 = (TH2D*)f->Get(Form("%s Hitmap %d",st,nevent)); 
   TH2D* h2 = (TH2D*)f->Get(Form("%s Hitmap selected %d",st,nevent)); 
   TH2D* h3 = (TH2D*)f->Get(Form("%s Hitmap clustered %d",st,nevent));
    
   TH2D* h4 = (TH2D*)f->Get(Form("%s MC-Clusters %d",st,nevent)); 
   if (h4) {   
       h4->SetMarkerStyle(4);
       h4->SetMarkerSize(1.5);
       h4->SetMarkerColor(kRed);
   }
   
   TH2D* h5 = (TH2D*)f->Get(Form("%s RC-Clusters %d",st,nevent)); 
   if (h5) {   
       h5->SetMarkerStyle(5);
       h5->SetMarkerSize(1);
       h5->SetMarkerColor(kBlack);
       h5->SetMinimum(0);
   }
   
   gStyle->SetOptStat(0);
   //gStyle->SetOptStat("ermuo");
   //gStyle->SetOptStat("erm");
   
   TCanvas* cc;
   if (type != 0) cc = new TCanvas(Form("%s Clusters",st),Form("%s Clusters",st),900,800);
   else cc = new TCanvas(Form("%s Clusters",st),Form("%s Clusters",st),1200,800);
   
   cc->Divide(2,2);
   
   //----------------------------------------------------------
   cc->cd(1);
   
   if (h1) h1->Draw("colz");
   
   //----------------------------------------------------------
   cc->cd(2);
  
   if (h2) {
       h2->Draw("colz");
       if (h4) h4->Draw("P same");
   }
   else {
       if (h4) h4->Draw("P");
   }
   
   //h2->Draw("");
   //h4->Draw("box");

   //----------------------------------------------------------
   
   cc->cd(3);
  
   if (h3) {
       h3->Draw("colz");
       if (h5) h5->Draw("P same");
   }
   else {
       if (h5) h5->Draw("P");
   }
  
   //h5->Draw("P");
   
   //----------------------------------------------------------
   cc->cd(4);
   
   if (h1) h1->Draw("colz");
   
   if (h1) {
       h1->Draw("colz");
       if (h4) h4->Draw("P same");
       if (h5) h5->Draw("P same");
   }
   else {
       if (h4) {
           h4->Draw("P");
           if (h5) h5->Draw("P same");
       }
       else {
           if (h5) h5->Draw("P same");
       }
   }
   
   cc->cd(0);
}