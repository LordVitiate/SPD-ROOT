
void CustomizeGeometry1();
void CustomizeGeometry2();

//______________________________________________________________
void ConstructMvd() 
{
   SpdCommonGeoMapper::Instance()->OpenGeometry();
  
   FairModule* Cave = new SpdCave();
   Cave->SetGeometryFileName("cave.geo");
   Cave->ConstructGeometry();
   
   SpdCommonGeoMapper::Instance()->DefineQslGeometrySet();
   
   SpdPipe* Pipe = new SpdPipe();
   Pipe->SetPipeMaterial("steel",0);
   //Pipe->SetPipeMaterial("steel",1);
   Pipe->ConstructGeometry();
   Pipe->Print("");
   
   Int_t geo_type = 3; //1,2,3
   
   // return or create+return new one
   SpdMvdGeoMapper* mapper = SpdMvdGeoMapper::Instance(); 
   
   // here we can redefine active material (by default = "argon") 
   //mapper->SetActiveMaterial("argon");
   
   if (geo_type == 1 || geo_type == 2) mapper->SetGeoType(geo_type);
   else {
       mapper->SetGeoType(3);
       CustomizeGeometry1();     // this is equivalent to geo_type = 1 
       //CustomizeGeometry2();   // this is equivalent to geo_type = 2 
   }
       
   SpdMvd* mvd = new SpdMvd();
   mvd->ConstructGeometry();

   mapper->Print("");   
   
   mvd->PrintGeometry("");

   /* ~~~~~~~~~~~~~ DRAW GEOMETRY  ~~~~~~~~~~~~~~~~~ */
   
   TGeoManager *geoMan = gGeoManager;
   geoMan->SetVisLevel(4);
  
   //geoMan->GetMasterVolume()->SetVisContainers();
   geoMan->GetMasterVolume()->Draw("ogl");
   return;
}

//---------------------------------------------------------------
void CustomizeGeometry1()
{
   cout << "-I- <ConstructMvd:CustomizeGeometry1>" << endl;   
   
   SpdMvdGeoMapper* mapper = SpdMvdGeoMapper::Instance();
   
   mapper->ClearGeometry();
   
   //---------------------------------------
   // BUILD LAYERS
   
   Int_t l0, l1;
   
   l0 = mapper->DefineLayer(5.0,80.0);             mapper->SetLayerActivity(l0,true);   
   
        mapper->AddSublayer(l0,0.01750,"FR4");
        mapper->AddSublayer(l0,0.00190,"copper");
        mapper->AddSublayer(l0,0.01350,"kapton2");
        mapper->AddSublayer(l0,0.40000,"argon");
        mapper->AddSublayer(l0,0.00055,"copper");
        mapper->AddSublayer(l0,0.02400,"kapton2");
   
   l1 = mapper->DefineLayerCopy(l0,5.5);  
   l1 = mapper->DefineLayerCopy(l0,6.0);   
  
   l1 = mapper->DefineLayerCopy(l0,12.0);          mapper->SetLayerActivity(l1,true);
   l1 = mapper->DefineLayerCopy(l0,12.5);

   l1 = mapper->DefineLayerCopy2(l0,19.0,160.0);   mapper->SetLayerActivity(l1,true);
   l1 = mapper->DefineLayerCopy2(l0,19.5,160.0);
}

//---------------------------------------------------------------
void CustomizeGeometry2()
{
   cout << "-I- <ConstructMvd:CustomizeGeometry1>" << endl;   
   
   SpdMvdGeoMapper* mapper = SpdMvdGeoMapper::Instance();
   
   mapper->ClearGeometry();
    
   //---------------------------------------
   // BUILD LAYERS
   
   Int_t l0, l1;
   
   l0 = mapper->DefineLayer(5.0,80.0);            mapper->SetLayerActivity(l0,true);   
   
        mapper->AddSublayer(l0,0.01750,"FR4");
        mapper->AddSublayer(l0,0.00190,"copper");
        mapper->AddSublayer(l0,0.01350,"kapton2");
        mapper->AddSublayer(l0,0.40000,"argon");
        mapper->AddSublayer(l0,0.00055,"copper");
        mapper->AddSublayer(l0,0.02400,"kapton2");
   
   l1 = mapper->DefineLayerCopy(l0,5.5);  
   l1 = mapper->DefineLayerCopy(l0,6.0);   

   l1 = mapper->DefineLayerCopy2(l0,19.0,160.0);  mapper->SetLayerActivity(l1,true);
   l1 = mapper->DefineLayerCopy2(l0,19.5,160.0); 
   l1 = mapper->DefineLayerCopy2(l0,20.0,160.0); 
   
}



