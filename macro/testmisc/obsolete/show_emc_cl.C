
void show_event(TFile* , Int_t ne);

//______________________________________________________________________________
void show_emc_cl(Int_t event_num = 1, TString fname = "emc_cl.root") {
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
     
   TTree* t = (TTree*)f->Get("cbmsim") ;  
   
   TBranch* branch;
   
   branch = t->GetBranch("EmcClusterInfo0");
   
   if (!branch) {
       cout << "No branch: \"EmcClusterInfo0\"  " << endl;
       return;
   }
   else {
       int bentries =  branch->GetEntries();
       //cout << "Branch entries: \"Event Data\" " << bentries << endl;
       
       TClonesArray* fH;
       t->SetBranchAddress("EmcClusterInfo0",&fH);
       
       if (nevent > 0 || nevent <= bentries) {
           branch->GetEntry(nevent-1);
           cout << "MC-Clusters: " << fH->GetEntriesFast() << endl;
           
           EmcClusterInfo0* cl;
           for (Int_t i(0); i<fH->GetEntriesFast(); i++) {
                cout << "MC-CLUSTER: " << i << endl; 
                cl = (EmcClusterInfo0*)fH->At(i);
                cl->Print();
           }
           
       }     
   }
   
   branch = t->GetBranch("EmcClusterInfo");
   
   if (!branch) {
       cout << "No branch: \"EmcClusterInfo\"  " << endl;
       return;
   }
   else {
       int bentries =  branch->GetEntries();
       //cout << "Branch entries: \"Event Data\" " << bentries << endl;
       
       TClonesArray* fH;
       t->SetBranchAddress("EmcClusterInfo",&fH);
       
       if (nevent > 0 || nevent <= bentries) {
           branch->GetEntry(nevent-1);
           cout << "RC-Clusters: " << fH->GetEntriesFast() << endl;
           
           EmcClusterInfo* cx;
   
           for (Int_t i(0); i<fH->GetEntriesFast(); i++) {
                cout << "RC-CLUSTER: " << i << endl; 
                cx = (EmcClusterInfo*)fH->At(i);
                cx->Print();
   
        
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
                
   
                
           }
       }     
   }
   
   
   TH2D* h1 = (TH2D*)f->Get(Form("Hitmap %d",nevent)); 
   TH2D* h2 = (TH2D*)f->Get(Form("Hitmap selected %d",nevent)); 
   TH2D* h3 = (TH2D*)f->Get(Form("Hitmap clustered %d",nevent)); 
   
   TH2D* h4 = (TH2D*)f->Get(Form("MC-Clusters %d",nevent)); 
   if (h4) {   
       h4->SetMarkerStyle(4);
       h4->SetMarkerSize(1.5);
       h4->SetMarkerColor(kRed);
   }
   
   TH2D* h5 = (TH2D*)f->Get(Form("RC-Clusters %d",nevent)); 
   if (h5) {   
       h5->SetMarkerStyle(5);
       h5->SetMarkerSize(1);
       h5->SetMarkerColor(kBlack);
       h5->SetMinimum(0);
   }
   
   gStyle->SetOptStat(0);
   //gStyle->SetOptStat("ermuo");
   
   TCanvas* cc = new TCanvas("Clusters","Clusters",1400,800);
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


