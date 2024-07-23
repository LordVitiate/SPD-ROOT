

void Print(SpdGeopathParser& ppath);

void check_nodepath() 
{
    
 SpdGeopathParser ppath;
 TString nodepath;
 
 nodepath = "/cave_1/SideSVol_8/LayerSideSVol_2/L4930EnvelopeVol_5/L4930MDTSVol_1/L4930CellVol_3/L4930GasCellVol_1";

 ppath.ParsePath(nodepath);
 cout << "MODULE NUMBER: " << ppath.Num(2) << endl;
  
 Print(ppath);
 
 nodepath = "/cave_1/SideSVol_3/LayerSideSVol_2/L4930EnvelopeVol_5/L4930MDTSVol_1/L4930CellVol_3/L4930GasCellVol_1";

 ppath.ParsePath(nodepath);
 cout << "MODULE NUMBER: " << ppath.Num(2) << endl;
  
 Print(ppath);
 
 nodepath = "/cave_1/TsTBModule3_6/TsTBLayer127_25/TsTBStraw89_2/TsTBStraw89Shell_1";
 
 ppath.ParsePath(nodepath);
 cout << "MODULE NUMBER: " << ppath.Num(2) << endl;
  
 Print(ppath);
 
}

void Print(SpdGeopathParser& ppath)
{
   Int_t max_geo_level = ppath.GetCurrentPathLevel();
   cout << ppath.GetCurrentPathString() << endl;
   
   for (Int_t i(1); i<=max_geo_level; i++) {
        cout << i << " " <<  ppath.Num(i) << " "<< ppath.VNum(i) << " "<< ppath.Name(i) << "    :: " << ppath.Path(i) << endl; 
       
   }
   
  
    
}
