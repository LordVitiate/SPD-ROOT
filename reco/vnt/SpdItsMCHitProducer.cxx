// $Id$
// Author: artur   2021/02/01

//_____________________________________________________________________________
//
// SpdItsMCHitProducer
//_____________________________________________________________________________

#include "SpdItsMCHitProducer.h"
#include "SpdSetParSet.h"

#include "SpdRunAna.h"
#include "SpdCommonGeoMapper.h"
#include "SpdGeoFactory.h"

#include "SpdSetParSet.h"
#include "SpdParSet.h"

#include "SpdItsGeoMapperX.h"
#include "SpdItsGeoBuilder.h"

#include "SpdMCEvent.h"
#include "SpdMCParticle.h"
#include "SpdMCSiliconHit.h"
#include "SpdItsPoint.h"

#include "SpdMCEventHelper.h"
#include "SpdParticlesAndProcesses.h"

#include <TGeoManager.h>
#include <TRandom3.h>

#include <iostream>

using std::cout;
using std::endl;

# define mkm_ 1.e-4 // mkm -> cm 

ClassImp(SpdItsMCHitProducer)

//_____________________________________________________________________________
SpdItsMCHitProducer::SpdItsMCHitProducer():
fGeoMapper(0),fGeoBuilder(0),
fParsName("ItsParSet"),fParameters(0),
fSaveDetIdOption(0),fSaveEmptyHits(0),fDataType(""),
fEvent(0),fParticles(0),
fHitProductionMetod(0),
fSaveHits(true),
fPointsArray(0),fHits(0),fNFailedHits(0),fNTotalHits(0),
fVerboseLevel(1)
{
   SpdParticlesAndProcesses::AddParticlesToDatabasePDG();
   
   // set hit default resolution
   SetHitResolution('n', 100*mkm_, 100*mkm_); 
   SetHitResolution('p', 10*mkm_,   10*mkm_);
   SetHitResolution('s', 81.26*mkm_,   27.4*mkm_); // TDR v29
   SetHitResolution('m', 10*mkm_,   10*mkm_);
}

//_____________________________________________________________________________
SpdItsMCHitProducer::~SpdItsMCHitProducer() 
{
   if (fGeoMapper) delete fGeoMapper;
   if (fGeoBuilder) delete fGeoBuilder;
   if (fHits) { fHits->Delete(); delete fHits; }

   //SpdMCEventHelper::Instance()->SetItsHits(0);
}

//_____________________________________________________________________________
void SpdItsMCHitProducer::LoadGeometry()
{
   SpdRunAna* run = (SpdRunAna*)FairRunAna::Instance(); 
      
   if (!gGeoManager) run->GetParameters("FairGeoParSet");

   if (!gGeoManager) {
       cout << "-F- <SpdItsMCHitProducer::LoadGeometry> Geometry is not defined " << endl;
       exit(1);
   }
   
   fParameters = dynamic_cast<SpdParSet*>(run->GetParameters(fParsName));
   
   if (!fParameters) {
       SpdSetParSet* pars = (SpdSetParSet*)run->GetParameters("SpdSetParSet");
       if (pars) fParameters = pars->GetActiveGeoPars(fParsName);
   }
   
   if (!fParameters) {
       cout << "-F- <SpdItsMCHitProducer::LoadGeometry> No geometry parameters " 
            << "(\"" << fParsName << "\")" << endl;
       exit(1); 
   }
   
   //fParameters->print(1);
   
   TString mapper;
   fParameters->GetParameter("Mapper",mapper); 
   fGeoMapper = SpdGeoFactory::Instance()->Mapper(mapper);
    
   if (fGeoMapper) {
       fGeoMapper->LoadParametersFrom(fParameters);
       fGeoMapper->InitGeometry();
       fGeoMapper->LockGeometry();
       //fGeoMapper->Print("");
   }
   
   TString builder;
   fParameters->GetParameter("Builder",builder); 
   fGeoBuilder = SpdGeoFactory::Instance()->Builder(builder);
   
   if (fGeoBuilder) {
       fGeoBuilder->LoadParametersFrom(fParameters);
       fGeoBuilder->SetMapper(fGeoMapper);   
       fGeoBuilder->LockGeometry();
       //fGeoBuilder->Print("");  
   }
   
   fParameters->GetParameter("Detector/SaveDetIdOption",fSaveDetIdOption);
   fParameters->GetParameter("Detector/SaveEmptyHits",fSaveEmptyHits);
   fParameters->GetParameter("OutData_1",fDataType);
}

//_____________________________________________________________________________
void SpdItsMCHitProducer::SetHitResolution(Char_t spec, Double_t resU, Double_t resV)
{
    resU = TMath::Abs(resU);
    resV = TMath::Abs(resV);
    if (spec == 'p' || spec == 'n') fResolution[spec] = TVector3(resU,resU,0.);
    else fResolution[spec] = TVector3(resU,resV,0.);
}

//_____________________________________________________________________________
Double_t SpdItsMCHitProducer::GetHitResolutionU(Char_t spec)
{
    auto x = fResolution.find(spec);
    return (x != fResolution.end()) ? (x->second)[0] : 100*mkm_;
}

//_____________________________________________________________________________
Double_t SpdItsMCHitProducer::GetHitResolutionV(Char_t spec)
{
    auto x = fResolution.find(spec);
    return (x != fResolution.end()) ? (x->second)[1] : 100*mkm_;
}

//_____________________________________________________________________________
InitStatus SpdItsMCHitProducer::Init() 
{   
   // get RootManager
   
   FairRootManager* ioman = FairRootManager::Instance();
   if (!ioman) {
       cout << "-F- <SpdItsMCHitProducer::Init> RootManager not instantiated " << endl;
       return kFATAL;
   }
   
   //========================== LOAD GEOMETRY ==============================/
   
   LoadGeometry();
   
   //========================== ADD PARAMETERS =============================/
   
   AddParameters();
   
   //===================== LOAD INPUT DATA OBJECTS =========================/
   
   // get Event 
   
   fEvent = (SpdMCEvent*)ioman->GetObject(SpdDataBranchName(kMCEvent));
   if (!fEvent) return kFATAL;
   
   // get Particles array
   
   fParticles = (TClonesArray*)ioman->GetObject(SpdDataBranchName(kMCParticles));
   if (!fParticles) return kFATAL;
   
   SpdMCEventHelper* helper = SpdMCEventHelper::Instance();
   
   // searching for ID-table 
   
   if (!helper->GetIdTable()) return kFATAL;
   
   // get Points array
   fPointsArray = (TClonesArray*)ioman->GetObject(fDataType);
    
   if (!fPointsArray) {
       cout << "-F- <SpdItsMCHitProducer::Init> No MC-POINTS: " << fDataType << endl;
       return kFATAL;
   }
   
   //============== CREATE AND REGISTER OUTPUT DATA OBJECTS ================/
   
   fHits = new TClonesArray("SpdMCSiliconHit");
   ioman->Register(SpdDataBranchName(kMCItsHits),"SpdMCSiliconHit", fHits, fSaveHits);
   
   //============== PASS DATA TO EVENTHELPER ================================/   
   
   helper->SetItsHits(fHits);
   
   if (!helper->GetEvent()) helper->SetEvent(fEvent);
   if (!helper->GetParticles()) helper->SetParticles(fParticles);
   
   if (fVerboseLevel > -1) cout << "-I- <SpdItsMCHitProducer::Init> Intialization successfull " << endl;
   
   return kSUCCESS;
}

//________________________________________________________________________________________
void SpdItsMCHitProducer::Exec(Option_t* opt) 
{
   assert(fEvent);
   assert(fParticles);
   assert(fPointsArray);
    
   if (fVerboseLevel > 0) {
       cout << "\n-I- <SpdItsMCHitProducer::Exec> Process event ... " 
            << fEvent->GetEventId();// << "\n\n";
       if (fVerboseLevel > 1) cout << "\n";    
   }
   
   fHits->Delete();
          
   // >>>>> Fill hits <<<<< 
         
   Int_t npoints = fPointsArray->GetEntriesFast();
   
   SpdItsPoint*     point;
   SpdMCSiliconHit* hit;
   
   for (Int_t i(0); i<npoints; i++) 
   {
        point = dynamic_cast<SpdItsPoint*>(fPointsArray->At(i));
        if (!AcceptPoint(point)) continue;
        hit = CreateItsHit(point);
        
        if (!hit) return;
            
        // set mc-point id
        hit->SetMCPointId(i);
        
        // add mc-truth
        AddMCTruth(point,hit);
   }
   
   fNTotalHits += fHits->GetEntriesFast();
   
   if (fVerboseLevel > 1) 
   {
       cout << "\n-I- <SpdItsMCHitProducer::Exec> Print ... " << endl; 
       
       if (fVerboseLevel > 2) {
           
           cout << "Print hits [first,last]: " << endl; 
           
           Int_t nn = fHits->GetEntriesFast();
           if (nn > 0) {
               hit = dynamic_cast<SpdMCSiliconHit*>(fHits->At(0));
               if (hit) hit->Print();
               if (nn > 1) {
                   hit = dynamic_cast<SpdMCSiliconHit*>(fHits->At(nn-1));
                   if (hit) hit->Print();
               }
           }
       }
       
       cout << "Production method:  " << fHitProductionMetod << endl;
       cout << "Points/hits:        " << fPointsArray->GetEntriesFast() << "/" 
                                      << fHits->GetEntriesFast() 
                                      << endl;
   }
}

//________________________________________________________________________________________
SpdMCSiliconHit* SpdItsMCHitProducer::CreateItsHit(SpdItsPoint* p)
{
    if (!p) return 0;
    
    //WARNING SpdGeoBuilder -> SpdItsGeoBuilder WARNING
    SpdItsGeoBuilder* builder = dynamic_cast<SpdItsGeoBuilder*>(fGeoBuilder);
    if (!builder) return 0;
    
    Int_t id = p->GetNodeId();
    
    if (!builder->SetNodePath(id)) return 0;
    
    SpdMCSiliconHit* hit = AddHit();
    
    // set hit "weight" 
    hit->SetWeight(1);

    TVector3 mc_point;  // mc-point position
    Double_t t;         // hit time
    
    p->GetPosTime(mc_point,t);  // average of "in" and and "out" positions

    // set mc-point position
    hit->SetMCPoint(mc_point); 
    
    // set hit time & time error
    hit->SetHitTime(t);
    hit->SetHitTimeError(0.);
    
    // set total signal value FIXME
    hit->SetResp(p->GetEnergyLoss());
    
    // set segment length
    hit->SetSegLength(p->GetSegLength());
   
    // check the detector part (barrel or endcap) ATTENTION
    TString path = builder->GetNodePath();
    Bool_t barrel_part = true;
    if (path.Contains("EClayer")) barrel_part = false; 
    
    // get detector layer
    SpdGeopathParser parser;
    parser.ParsePath(path);
    Int_t det_layer = parser.VNum(2);    // get layer
    hit->SetDetLayer(det_layer);
    
    //----------------------------------------------------------
    // set DETECTOR parameters
    
    Double_t x[3];
    
    // set chip position
    if (builder->GetDetectorPosition(x)) hit->SetDetPos(x[0],x[1],x[2]);
    
    // set chip zero-level id (super module id) 
    hit->SetModId(p->GetDetectorID());
        
    // set chip 1st-level id: backsward(=-1)/forward(=1) endcap or barrel(=0)
    if (barrel_part) hit->SetDetId(0,0);
    else { (x[2] < 0) ? hit->SetDetId(0,-1) : hit->SetDetId(0,1); }
  
    // set chip 2nd-level id 
    hit->SetDetId(1,id);
    
    // WARNING set chip local axes (u,v,z) and chip sizes WARNING
    if (barrel_part) {
        // BARREL: (z->u, x->v, y->n)
        if (builder->GetDetectorOrientation(x,'z')) hit->SetU(TVector3(x[0],x[1],x[2]));
        if (builder->GetDetectorOrientation(x,'x')) hit->SetV(TVector3(x[0],x[1],x[2]));
        if (builder->GetNodeSize(x)) hit->SetL(x[2],x[0],x[1]); //u,v,n-sizes
    }
    else {
        // Simplified descriptin of Endcap as a single disc instead of trapezpidal modules
        // U - along r in disc
        // V - perp to r in disc
        // n - perp
        TVector3 vr(mc_point.X(), mc_point.Y(), 0.);
        vr = vr.Unit();
        hit->SetU(vr);
        hit->SetV(vr.Cross(TVector3(0, 0, 1)));
        hit->SetL(250, 250, 300./2*mkm_); // u,v,n-sizes, UPPER boundaries are set here. The last one should not be used (set to a half?)?
    }
    
    // Check if necessary to save empty hits. Why would one need that? Any use of these results?
    // Keep it to not break something. // Igor

    Bool_t nehits = !fSaveEmptyHits;
    
    // check if hit is empty
    if (nehits && !(p->GetEnergyLoss() > 0)) {
        hit->SetSpecifity('n');  // specification = "undefined"
        hit->SetHitUV(mc_point); // ATTENTION set HIT position = MC-POINT 
        hit->SetResolutionU(GetHitResolutionU('n'));
        hit->SetResolutionV(GetHitResolutionV('n'));
        hit->SetDelta(0);
        hit->SetWeight(0);
        fNFailedHits++;
        return hit;      
    }

    // check hit specification

    if (barrel_part)
    {
        Char_t spec = 'n';

        SpdGeoVVolumeBox2D *vvol = dynamic_cast<SpdGeoVVolumeBox2D *>(builder->GetNodeVVolume());

        if (vvol)
        {
            TString vvolname = vvol->GetName();
            if (vvolname.Contains("DSSD"))
                spec = 's';
            else if (vvolname.Contains("MAPS"))
                spec = 'm';
        }

        // SHOULD NEVER HAPPEN
        if (spec == 'n')
        {
            hit->SetSpecifity('n');  // specification = "undefined"
            hit->SetHitUV(mc_point); // ATTENTION set HIT position = MC-POINT
            hit->SetResolutionU(GetHitResolutionU('n'));
            hit->SetResolutionV(GetHitResolutionV('n'));
            hit->SetDelta(0);
            hit->SetWeight(0);
            fNFailedHits++;
            // p->Print("");
            return hit;
        }

        if (fHitProductionMetod == 0) /************************************************************************************/
        {
            Double_t u = hit->GetPointU();
            Double_t v = hit->GetPointV();
            Double_t lu = hit->GetLu();
            Double_t lv = hit->GetLv();

            if ((TMath::Abs(u) > lu) || (TMath::Abs(v) > lv))
            {
                hit->SetSpecifity('n'); // specification = "undefined"
                hit->SetHitU(u);        // ATTENTION set HIT position = MC-POINT
                hit->SetHitV(v);        // ATTENTION set HIT position = MC-POINT
                hit->SetResolutionU(GetHitResolutionU('n'));
                hit->SetResolutionV(GetHitResolutionV('n'));
                hit->SetDelta(0);
                hit->SetWeight(0);
                fNFailedHits++;
                // p->Print("");
                return hit;
            }

            Double_t ures = GetHitResolutionU(spec);
            Double_t vres = GetHitResolutionV(spec);

            const Double_t ktol = 0.99999;
            const Int_t nmaxit = 100;
            Bool_t is_ok = true;

            Double_t xx;
            Int_t nit(0);

            if (ures < 2 * lu / TMath::Sqrt(12))
            {
                while (true)
                {
                    xx = gRandom->Gaus(u, ures);
                    if (xx > -lu && xx < lu)
                    {
                        u = xx;
                        break;
                    }
                    if (++nit == nmaxit)
                    {
                        is_ok = false;
                        break;
                    }
                }
            }
            else
                u = gRandom->Uniform(-ktol * lu, ktol * lu);

            if (is_ok)
            {
                nit = 0;
                if (vres < 2 * lv / TMath::Sqrt(12))
                {
                    while (true)
                    {
                        xx = gRandom->Gaus(v, vres);
                        if (xx > -lv && xx < lv)
                        {
                            v = xx;
                            break;
                        }
                        if (++nit == nmaxit)
                        {
                            is_ok = false;
                            break;
                        }
                    }
                }
                else
                    v = gRandom->Uniform(-ktol * lv, ktol * lv);
            }

            if (!is_ok)
            {
                hit->SetSpecifity('n');         // specification = "undefined"
                hit->SetHitU(hit->GetPointU()); // ATTENTION set HIT position = MC-POINT
                hit->SetHitV(hit->GetPointV()); // ATTENTION set HIT position = MC-POINT
                hit->SetResolutionU(ures);
                hit->SetResolutionV(vres);
                hit->SetDelta(0);
                hit->SetWeight(0);
                fNFailedHits++;
                // p->Print("");
                return hit;
            }

            //-----------------------------------------------------------------
            // ATTENTION set hit position, specification & resolution ATTENTION
            //-----------------------------------------------------------------
            hit->SetSpecifity(spec); // specification = 'm' (MAPS) or 's' (DSSD)
            hit->SetHitU(u);
            hit->SetHitV(v);
            hit->SetResolutionU(ures);
            hit->SetResolutionV(vres);
        } /***********************************************************************************************************/
        else if (fHitProductionMetod == 1)
        { //######################################################################

            Double_t hit_pos[3] = {0, 0, 0}; // chip channel global position (x,y,z)

            // standard hit specifications (DSSD = 's' or MAPS = 'm')

            Int_t nchan = p->GetVDataSize(); // number of hitted channels in the chip
            Int_t vid;                       // chip channel id
            if (nchan < 1)
                spec = 'n';
            else if (nchan == 1)
            {
                if (nehits && !(p->GetVed(0) > 0))
                    spec = 'n';
                else
                {
                    vid = p->GetVid(0);
                    if (vid < 0)
                        spec = 'n';
                    else
                    {
                        builder->SetVid(vid); // set channel C.S.
                        if (!builder->GetDetectorPosition(hit_pos))
                            spec = 'n';
                    }
                }
            }
            else
            {
                Int_t iz_ch, iphi_ch; // chip channel id's: iz_ch = id1, iphi_ch = id2
                Int_t nc(0), az_ch(0), aphi_ch(0);
                for (Int_t j(0); j < nchan; j++)
                {
                    vid = p->GetVid(j);
                    if (vid < 0)
                        continue;
                    if (nehits && !(p->GetVed(j) > 0))
                        continue;
                    vvol->CellId(vid, iz_ch, iphi_ch);
                    az_ch += iz_ch;
                    aphi_ch += iphi_ch;
                    nc++;
                    // cout << j << "  (z,phi):  " << iz_ch << "  " << iphi_ch << endl;
                }
                // cout << "  a(z,phi):  " << az_ch/Double_t(nc) << "  " << aphi_ch/Double_t(nc) << " nc = " << nc << endl;
                if (nc == 0)
                {
                    Double_t dep = 0;
                    if (p->IsSpecialPoint(vid, dep))
                    { // special case: very short (~several mkm) track segment
                        if (nehits && !(dep > 0))
                            spec = 'n';
                        else
                        {
                            builder->SetVid(vid); // set channel C.S.
                            if (!builder->GetDetectorPosition(hit_pos))
                                spec = 'n';
                            // cout << "spec: " << spec << " vid = " << vid << " dep =  " << dep << endl;
                        }
                    }
                    else
                        spec = 'n';
                }
                else if (nc == 1)
                {
                    vid = vvol->CellId(az_ch, iphi_ch);
                    builder->SetVid(vid); // set channel C.S.
                    if (!builder->GetDetectorPosition(hit_pos))
                        spec = 'n';
                }
                else
                {
                    Double_t pos[3];
                    vvol->GetPos(az_ch / Double_t(nc), aphi_ch / Double_t(nc), pos);
                    builder->SetVid(-1); // set chip C.S.
                    if (!builder->LocalToGlobalP(pos, hit_pos))
                        spec = 'n';
                }
            }

            // cout << "\t vol.name = " << vvol->GetName() << ";  specifity: " << spec << ";  sub_mod: " << submod_id
            //      << ";  nchannels: " << nchan << ";  " << builder->GetNodePath() << "\n\n";
            // printf("\t its point (1): %14.8f  %14.8f  %14.8f\n",mc_point.X(),mc_point.Y(),mc_point.Z());
            // printf("\t its point (2): %14.8f  %14.8f  %14.8f\n",hit_pos[0],hit_pos[1],hit_pos[2]);
            // printf("\n");

            if (spec == 'n')
            {
                hit->SetSpecifity('n');  // specification = "undefined"
                hit->SetHitUV(mc_point); // ATTENTION set HIT position = MC-POINT
                hit->SetResolutionU(GetHitResolutionU('n'));
                hit->SetResolutionV(GetHitResolutionV('n'));
                hit->SetDelta(0);
                hit->SetWeight(0);
                fNFailedHits++;
                // p->Print("");
                return hit;
            }

            TVector3 position(hit_pos[0], hit_pos[1], hit_pos[2]);

            //---------------------------------------------
            // Checking (1)

            TVector3 dpos = position - hit->GetDetPos();
            TVector3 ndet = hit->GetN(true);
            Double_t dd = dpos.Dot(ndet);

            if (TMath::Abs(dd) > 1e-14)
            {
                hit->SetSpecifity('n');
                hit->SetHitUV(mc_point); // ATTENTION set HIT position = MC-POINT
                hit->SetResolutionU(GetHitResolutionU('n'));
                hit->SetResolutionV(GetHitResolutionV('n'));
                hit->SetDelta(0);
                hit->SetWeight(0);
                fNFailedHits++;
                // cout << "-W- <SpdItsMCHitProducer::CreateItsHit> Specifity: "<< hit->GetSpecifity() << " diff. = " << dd
                //      << " U, V, N: " << hit->GetHitU() << " " << hit->GetHitV() << " " << hit->GetPointN() << endl;
                return hit;
            }

            //-----------------------------------------------------------------
            // ATTENTION set hit position, specification & resolution ATTENTION
            //-----------------------------------------------------------------
            hit->SetSpecifity(spec); // specification = 'm' (MAPS) or 's' (DSSD)
            hit->SetHitUV(position);
            hit->SetResolutionU(GetHitResolutionU(spec));
            hit->SetResolutionV(GetHitResolutionV(spec));

            // cout << "\n hit position: " << position.X() << " " << position.Y()  << " " << position.Z() << "\n";
        } //#####################################################################################################
    } else { // ENDCAPS
        // CURRENTLY ONLY DSSD OPTION IS IMPLEMENTED AND ASSUMED.
        // Numbers from TDR draft v46.

        Double_t u = hit->GetPointU();
        Double_t v = hit->GetPointV();

        Double_t ures = 275 * mkm_/TMath::Sqrt(12.);
        Double_t pitch_v = u * TMath::Tan(25.71/2*TMath::DegToRad()) / 320;
        Double_t vres = pitch_v / TMath::Sqrt(12.);

        Double_t u_meas, v_meas;
        Double_t r;
        do {
            u_meas = gRandom->Gaus(u, ures);
            v_meas = gRandom->Gaus(v, vres);
            r = TMath::Sqrt(u_meas*u_meas + v_meas*v_meas);
        } while( r > 22.0 || r < 3.5 ); // approx from Zamyatin, 21.04.22

        hit->SetSpecifity('s'); // specification = 'm' (MAPS) or 's' (DSSD)
        hit->SetHitU(u_meas);
        hit->SetHitV(v_meas);
        hit->SetResolutionU(ures);
        hit->SetResolutionV(vres);
        // Delta is not calculated for endcaps
    }

    //---------------------------------------------
    // Checking (2)
    
    Double_t xpos[3], pos[3];
    
    // Correct for barrel only
    // BARREL: (v->x, y = 0, u->z) 
    xpos[0] = hit->GetHitV();
    xpos[1] = 0.;
    xpos[2] = hit->GetHitU();
    
    builder->LocalToGlobalP(xpos,pos);
    
    //cout << endl;
    //cout << pos[0] << " " << pos[1] << " " << pos[2] << endl;
    //cout << mc_point.X() << " " << mc_point.Y()  << " " << mc_point.Z() << "\n\n";
    
    Double_t ds = (pos-mc_point).Mag();
    
    hit->SetDelta(ds);
     
    return hit;
}

//________________________________________________________________________________________  
Bool_t SpdItsMCHitProducer::AcceptPoint(const SpdItsPoint* point) const
{
   if (!point) return false;
   return true;   
}

//________________________________________________________________________________________
void SpdItsMCHitProducer::AddMCTruth(SpdItsPoint* point, SpdMCSiliconHit* hit)
{
   Int_t ipart = SpdMCEventHelper::Instance()->GetParticleId(point->GetTrackID()); 
   if (ipart < 0) return; 
   hit->AddMCTruth(ipart); 
   SpdMCParticle* part = (SpdMCParticle*)fParticles->At(ipart);
   part->SetChar('+');
}

//________________________________________________________________________________________  
SpdMCSiliconHit* SpdItsMCHitProducer::AddHit()
{
   if (!fHits) return 0;
   Int_t size = fHits->GetEntriesFast();
   SpdMCSiliconHit* hit = new ((*fHits)[size]) SpdMCSiliconHit();
   hit->SetId(size);
   return hit;
}

//________________________________________________________________________________________  
void SpdItsMCHitProducer::Finish()
{
   if (fVerboseLevel > -1) {
       cout << "\n-I- <SpdItsMCHitProducer::Finish> [Hits] "
            << "total: " << fNTotalHits << ", failed: " << fNFailedHits;// << endl;
   }
}

//________________________________________________________________________________________
void SpdItsMCHitProducer::AddParameters()
{
   SpdRunAna* run = (SpdRunAna*)FairRunAna::Instance();
   SpdSetParSet* params = run->GetParSetAssembly();
   FairParGenericSet* pars = run->GetParameters(fParsName);
   params->AddActiveGeoPars(pars);
}


