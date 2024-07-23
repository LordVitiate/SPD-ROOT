
void ReadGeometry();
void ReadModules();
void ReadFieldPars();
void ReadPassivePars();
void ReadActivePars(TString stype);

TString inParFile;
TFile* fp;

//________________________________________________________________________________
void CheckOutputParams(TString path = "", TString stype = "") {
  //
  if (!path.IsWhitespace() && !path.EndsWith("/")) path += "/";
  
  // Input data & parameters files 
  if (stype.IsWhitespace()) 
  {
      inParFile = path+"params.root";
  }  
  else if (stype == "tor" || stype == "hyb") 
  { 
      inParFile = path+"params_tor.root";
  }
  else if (stype == "sol") 
  {  
      inParFile = "params_sol.root";
  }
  else {
      cout << "<CheckOutputParams> Unknown data type: " << stype << endl;
      return;
  }
  
  fp = new TFile(inParFile);
  if (!fp) return;
  
  fp->ls();
  
  ReadGeometry();
  ReadModules();
  ReadFieldPars();
  ReadPassivePars();
  ReadActivePars(stype); 
  
  //fp->Close();
}

//________________________________________________________________________
void ReadGeometry()
{
  FairGeoParSet* geoset = (FairGeoParSet*)fp->Get("FairGeoParSet");
  
  gGeoManager->GetMasterVolume()->Draw("ogl"); 
 
//   TObjArray* nodes = geoset->GetGeoNodes();
  
//   cout << "Name: " << geoset->GetName() << " Title: " << geoset->GetTitle() << endl; 
//   cout << "Geometry: " << gGeoManager->GetName() << endl;
//   cout << "Detector: " <<  geoset->getDetectorName() << endl;
//   cout << "Nodes in the set: " << nodes->GetEntries() << endl;
}


//________________________________________________________________________
void ReadModules()
{
  FairBaseParSet* geoset = (FairBaseParSet*)fp->Get("FairBaseParSet");
  SpdCommonGeoMapper::Instance()->PrintGeometryList();
}

//________________________________________________________________________
void ReadFieldPars()
{
  SpdFieldPar* field_pars = (SpdFieldPar*)fp->Get("SpdFieldPar");
  if (!field_pars) return;
  
  field_pars->print(1);  
  
  SpdFieldCreator* fieldCreator = new SpdFieldCreator();    
  SpdField* field = (SpdField*)fieldCreator->createFairField(field_pars);
  if (!field) return; 
      
  field->Print(""); 
}

//________________________________________________________________________
void ReadPassivePars()
{
  SpdPassiveGeoParSet* passive_pars = (SpdPassiveGeoParSet*)fp->Get("PassiveGeoParSet");
  if (passive_pars) passive_pars->print(1);  
}

//________________________________________________________________________
void ReadActivePars(TString stype)
{
  if (stype.IsWhitespace() || stype == "tor" || stype == "hyb") 
  { 
     /* TS BARREL */
     SpdParSet* tstb_pars = (SpdParSet*)fp->Get("TsTBParSet");
     if (tstb_pars) tstb_pars->print(1);
     
    /* TS ENDCAPS */
     SpdParSet* tstec_pars = (SpdParSet*)fp->Get("TsTECParSet");
     if (tstec_pars) tstec_pars->print(1);
     
     /* TOF BARREL */
     SpdParSet* tofb_pars = (SpdParSet*)fp->Get("TofBParSet");
     if (tofb_pars) tofb_pars->print(1);
     
     /* TOF ENDCAPS */
     SpdParSet* tofec_pars = (SpdParSet*)fp->Get("TofECParSet");
     if (tofb_pars) tofec_pars->print(1);
     
     /* ECAL BARREL */
     SpdParSet* ecaltb_pars = (SpdParSet*)fp->Get("EcalTBParSet");
     if (ecaltb_pars) ecaltb_pars->print(1);
     
     /* ECAL ENDCAPS */
     SpdParSet* ecaltec_pars = (SpdParSet*)fp->Get("EcalTECParSet");
     if (ecaltec_pars) ecaltec_pars->print(1);
     
     /* RS BARREL */
     SpdParSet* rstb_pars = (SpdParSet*)fp->Get("RsTBParSet");
     if (rstb_pars) rstb_pars->print(1);
     
     /* RS ENDCAPS */
     SpdParSet* rstec_pars = (SpdParSet*)fp->Get("RsTECParSet");
     if (rstec_pars) rstec_pars->print(1);
     
     /* INNER TRACKER SYSTEM (ITS) */
     SpdParSet *its_pars = (SpdParSet*)fp->Get("ItsParSet");
     if (its_pars) its_pars->print(1);
     
     /* BEAM-BEAM COUNTER (BBC) */
     SpdParSet *bbc_pars = (SpdParSet*)fp->Get("BbcParSet");
     if (bbc_pars) bbc_pars->print(1);
     
     /* AEROGEL (AEG) */
     SpdParSet *aeg_pars = (SpdParSet*)fp->Get("AegParSet");
     if (aeg_pars) aeg_pars->print(1);
     
     /* ZERO-DEGREE CALORIMETER (ZDC) */
     SpdParSet *zdc_pars = (SpdParSet*)fp->Get("ZdcParSet");
     if (zdc_pars) zdc_pars->print(1);
  }
  else if (stype == "sol") {
    
     /* TS BARREL */
     SpdParSet *tssb_pars = (SpdParSet*)fp->Get("TsSBParSet");
     if (tssb_pars) tssb_pars->print(1);
     
     /* ECAL BARREL */
     SpdParSet *ecalsb_pars = (SpdParSet*)fp->Get("EcalSBParSet");
     if (ecalsb_pars) ecalsb_pars->print(1);

     /* RS BARREL */
     SpdParSet *rssb_pars = (SpdParSet*)fp->Get("RsSBParSet");
     if (rssb_pars) rssb_pars->print(1);
     
     /* TS ENDCAPS */
     SpdParSet *tssec_pars = (SpdParSet*)fp->Get("TsSECParSet");
     if (tssec_pars) tssec_pars->print(1);
     
     /* ECAL ENDCAPS */
     SpdParSet *ecalsec_pars = (SpdParSet*)fp->Get("EcalSECParSet");
     if (ecalsec_pars) ecalsec_pars->print(1);
     
     /* RS ENDCAPS */
     SpdParSet *rssec_pars = (SpdParSet*)fp->Get("RsSECParSet");
     if (rssec_pars) rssec_pars->print(1);
  }
  
  /* PRIMARY GENERATOR */
  SpdPrimGenParSet *gen_pars = (SpdPrimGenParSet*)fp->Get("PrimGenParSet");
  if (gen_pars) gen_pars->print(1);
}


