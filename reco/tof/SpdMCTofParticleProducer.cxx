// $Id$
// Author: artur   2021/11/19

//_____________________________________________________________________________
//
// SpdMCTofParticleProducer
//_____________________________________________________________________________

#include "SpdMCTofParticleProducer.h"

#include "TRandom.h"
#include <TString.h>
#include <TVector3.h>
#include <TClonesArray.h>
#include <TGeoManager.h>

#include "SpdRunAna.h"
#include "SpdCommonGeoMapper.h"
#include "SpdGeoFactory.h"
#include "SpdTofBGeoMapper.h"
#include "SpdTofECGeoMapper.h"

#include "SpdSetParSet.h"
#include "SpdParSet.h"

#include "SpdMCEvent.h"
#include "SpdMCParticle.h"
#include "SpdTrackMC.h"
#include "SpdTofMCHit.h"
#include "SpdTofParticle.h"

#include "SpdMCEventHelper.h"
#include "SpdParticlesAndProcesses.h"
#include "SpdGeopathParser.h"

#include "SpdTrackPropagatorGF.h"

#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdMCTofParticleProducer)

    //_____________________________________________________________________________
    SpdMCTofParticleProducer::SpdMCTofParticleProducer() : fPropagator(0),
                                                           fDistanceToEndcap(-1.0), fBarrelMinRadius(-1.0),
                                                           fEvent(0), fParticles(0), fHits(0), fTracks(0),
                                                           fTofParticles(0),
                                                           fSaveParticles(true),
                                                           fVerboseLevel(1)
{
    fGeoMapper[0] = 0;
    fGeoMapper[1] = 0;
    fParsName[0] = "TofBParSet";
    fParsName[1] = "TofECParSet";
    fParameters[0] = 0;
    fParameters[1] = 0;
    fsigmaTimeTof = 60;
    fpmin = 0;
    fpmax = 10;
}

//_____________________________________________________________________________
SpdMCTofParticleProducer::~SpdMCTofParticleProducer()
{
    if (fPropagator)
        delete fPropagator;
}

//_____________________________________________________________________________
Bool_t SpdMCTofParticleProducer::LoadGeometry() // private
{
    SpdRunAna *run = (SpdRunAna *)FairRunAna::Instance();
    if (!gGeoManager && run)
        run->GetParameters("FairGeoParSet");
    if (!gGeoManager)
        return false;

    SpdSetParSet *pars;
    TString mapper;

    for (Int_t i(0); i < 2; i++)
    {
        fParameters[i] = dynamic_cast<SpdParSet *>(run->GetParameters(fParsName[i]));

        if (!fParameters[i])
        {
            pars = (SpdSetParSet *)run->GetParameters("SpdSetParSet");
            if (pars)
                fParameters[i] = pars->GetActiveGeoPars(fParsName[i]);
        }

        if (!fParameters[i])
        {
            cout << "-W- <SpdMCTofParticleProducer::LoadGeometry> No geometry parameters "
                 << "(\"" << fParsName[i] << "\")" << endl;
            return true;
        }

        // fParameters[i]->print(1);

        fParameters[i]->GetParameter("Mapper", mapper);
        fGeoMapper[i] = SpdGeoFactory::Instance()->SearchForMapper(mapper);

        if (!fGeoMapper[i])
        {
            cout << "-W- <SpdMCTofParticleProducer::LoadGeometry> Mapper not found "
                 << "(\"" << mapper << "\")" << endl;
            return true;
        }

        // fGeoMapper[i]->Print("");
    }

    fParameters[0]->GetParameter("TofECRmin", fBarrelMinRadius);
    fParameters[1]->GetParameter("TofECMinDist", fDistanceToEndcap);

    return true;
}

//_____________________________________________________________________________
InitStatus SpdMCTofParticleProducer::Init()
{
    // get RootManager

    FairRootManager *ioman = FairRootManager::Instance();
    if (!ioman)
    {
        cout << "-F- <SpdMCTofParticleProducer::Init> RootManager not instantiated " << endl;
        return kFATAL;
    }

    //========================== LOAD GEOMETRY ==============================/

    if (!LoadGeometry())
    {
        cout << "-W- <SpdMCTofParticleProducer::Init> Geometry not defined " << endl;
    }

    //===================== INIT FIELD PROPAGATOR ===========================/

    fPropagator = new SpdTrackPropagatorGF();
    if (!fPropagator->Init())
    {
        cout << "-F- <SpdMCTsParticleProducer::Init> "
             << "Track propagator not initialized properly" << endl;
        return kFATAL;
    }

    //===================== LOAD INPUT DATA OBJECTS =========================/

    // get Event

    fEvent = (SpdMCEvent *)ioman->GetObject(SpdDataBranchName(kMCEvent));
    if (!fEvent)
        return kFATAL;

    // get Particles array

    fParticles = (TClonesArray *)ioman->GetObject(SpdDataBranchName(kMCParticles));
    if (!fParticles)
        return kFATAL;

    // get TOF hits array

    fHits = (TClonesArray *)ioman->GetObject(SpdDataBranchName(kMCTofHits));
    if (!fHits)
        return kFATAL;

    // --- get Tracks array ---

    fTracks = (TClonesArray *)ioman->GetObject(SpdDataBranchName(kMCTracks));
    if (!fTracks)
        return kFATAL;

    //============== CREATE AND REGISTER OUTPUT DATA OBJECTS ================/

    fTofParticles = new TClonesArray("SpdTofParticle");
    ioman->Register(SpdDataBranchName(kTofParticles), "SpdTofParticle", fTofParticles, fSaveParticles);

    //============== PASS DATA TO EVENTHELPER ================================/

    SpdMCEventHelper *helper = SpdMCEventHelper::Instance();

    if (!helper->GetEvent())
        helper->SetEvent(fEvent);
    if (!helper->GetParticles())
        helper->SetParticles(fParticles);
    if (!helper->GetTofHits())
        helper->SetTofHits(fHits);
    if (!helper->GetTracks())
        helper->SetTracks(fTracks);

    helper->SetTofParticles(fTofParticles);

    if (fVerboseLevel > -1)
        cout << "-I- <SpdMCTofParticleProducer::Init> Intialization successfull " << endl;

    return kSUCCESS;
}

//________________________________________________________________________________________
void SpdMCTofParticleProducer::Exec(Option_t *opt)
{
    assert(fEvent);
    assert(fParticles);
    assert(fHits);
    assert(fTracks);

    if (fVerboseLevel > 0)
    {
        cout << "\n-I- <SpdMCTofParticleProducer::Exec> Process event ... "
             << fEvent->GetEventId();
        if (fVerboseLevel > 1)
            cout << "\n";
    }

    fTofParticles->Delete();

    // >>>>> Create paricles <<<<<

    SpdTrackMC *track;
    SpdTrackFitPar *fitpar;
    SpdTofMCHit *hit;
    SpdTofParticle *part;

    SpdMCParticle *mcpart;
    Int_t hitId;

    Double_t trackLen, hitTime, tofMass2;
    Int_t hitLocation;

    Int_t ntracks = fTracks->GetEntriesFast();
    for (Int_t i(0); i < ntracks; i++)
    {
        track = (SpdTrackMC *)fTracks->At(i);
        if (!track)
            continue;

        fitpar = track->GetFitPars();
        if (!fitpar)
            continue;

        SpdTrackState *trklaststate = fitpar->GetLastState();
        if (!trklaststate)
            continue;

        //    SpdTrackState *trkfinalstate = fitpar->GetFinalState();
        //   if (!trkfinalstate) continue;

        SpdTrackState *trkfirststate = fitpar->GetFirstState();
        if (!trkfirststate)
            continue;

        // searching for TOF hit which corresponds to the track using mc-info
        mcpart = (SpdMCParticle *)fParticles->At(track->GetParticleId());
        hitId = mcpart->GetTofParticleId();
        if (hitId < 0)
            continue;

        hit = (SpdTofMCHit *)fHits->At(hitId);

        hitLocation = 0;
        if (hit->IsBarrelHit())
            hitLocation = 1;
        else if (hit->IsEndcapHit())
            hitLocation = 2;

        // create TS-particle
        part = AddParticle();

        part->SetHitId(hit->GetId()); // same as hitId
        part->SetTrackId(track->GetId());

        // ATTENTION mc-particle info redefinition (hit_id -> particle_id) ATTENTION

        mcpart->SetTofParticleId(part->GetId());

        trackLen = hit->GetTrackLen();
        hitTime = hit->GetHitTime();

        TVector3 lastStateMom = trklaststate->GetMomentum();
        TVector3 firstStateMom = trkfirststate->GetMomentum();
        double p_mean = (lastStateMom.Mag() + firstStateMom.Mag()) / 2.;

        tofMass2 = CalculateTofMass2(hitTime, trackLen, p_mean);

        part->SetTofMass2(tofMass2);
        part->SetMomentum(p_mean);
        part->SetSigma(CalculateSigma(p_mean, hitLocation));
        part->SetMean(CalculateMean(p_mean, hitLocation));

        part->SetDistances(CalculateDistances(tofMass2, p_mean, hitLocation));

        part->SetLikelihoods(CalculateLikelihoods(tofMass2, p_mean, hitLocation));

        part->SetPosteriorProbs(CalculatePosteriorProbs(tofMass2, p_mean));

        // CheckParticle(part); //FIXME
    }

    // cout << "\n >>>>> -I- <SpdMCTofParticleProducer::Exec> Particles: " << fTofParticles->GetEntriesFast() << endl;
}

std::vector<Double_t> SpdMCTofParticleProducer::CalculatePosteriorProbs(Double_t tofMass2, Double_t momentum)
{
    std::vector<Double_t> vWi;
    std::vector<Double_t> vPDF;

    vWi.push_back(-1);
    vWi.push_back(-1);
    vWi.push_back(-1);
    vWi.push_back(-1);

    return vWi;

    /*
        //cout << "don't work now" << endl;

        Double_t vpriorProbability[3];

        if (momentum > 0.000 && momentum < 0.250)
        {
            vpriorProbability[0] = 1.000;
            vpriorProbability[1] = 0.017;
            vpriorProbability[2] = 0.006;
        }
        if (momentum > 0.250 && momentum < 0.500)
        {
            vpriorProbability[0] = 1.000;
            vpriorProbability[1] = 0.042;
            vpriorProbability[2] = 0.022;
        }
        if (momentum > 0.500 && momentum < 0.750)
        {
            vpriorProbability[0] = 1.000;
            vpriorProbability[1] = 0.074;
            vpriorProbability[2] = 0.050;
        }
        if (momentum > 0.750 && momentum < 1.000)
        {
            vpriorProbability[0] = 1.000;
            vpriorProbability[1] = 0.099;
            vpriorProbability[2] = 0.087;
        }
        if (momentum > 1.000 && momentum < 1.250)
        {
            vpriorProbability[0] = 1.000;
            vpriorProbability[1] = 0.111;
            vpriorProbability[2] = 0.119;
        }
        if (momentum > 1.250 && momentum < 1.500)
        {
            vpriorProbability[0] = 1.000;
            vpriorProbability[1] = 0.119;
            vpriorProbability[2] = 0.157;
        }
        if (momentum > 1.500 && momentum < 1.750)
        {
            vpriorProbability[0] = 1.000;
            vpriorProbability[1] = 0.132;
            vpriorProbability[2] = 0.193;
        }
        if (momentum > 1.750 && momentum < 2.000)
        {
            vpriorProbability[0] = 1.000;
            vpriorProbability[1] = 0.135;
            vpriorProbability[2] = 0.236;
        }
        if (momentum > 2.000 && momentum < 2.250)
        {
            vpriorProbability[0] = 1.000;
            vpriorProbability[1] = 0.142;
            vpriorProbability[2] = 0.296;
        }
        if (momentum > 2.250 && momentum < 2.500)
        {
            vpriorProbability[0] = 1.000;
            vpriorProbability[1] = 0.163;
            vpriorProbability[2] = 0.353;
        }
        if (momentum > 2.500 && momentum < 2.750)
        {
            vpriorProbability[0] = 1.000;
            vpriorProbability[1] = 0.183;
            vpriorProbability[2] = 0.417;
        }
        if (momentum > 2.750 && momentum < 3.000)
        {
            vpriorProbability[0] = 1.000;
            vpriorProbability[1] = 0.251;
            vpriorProbability[2] = 0.526;
        }
        if (momentum > 3.000 && momentum < 3.250)
        {
            vpriorProbability[0] = 1.000;
            vpriorProbability[1] = 0.301;
            vpriorProbability[2] = 0.610;
        }
        if (momentum > 3.250 && momentum < 3.500)
        {
            vpriorProbability[0] = 1.000;
            vpriorProbability[1] = 0.324;
            vpriorProbability[2] = 0.758;
        }
        if (momentum > 3.500 && momentum < 10.000)
        {
            vpriorProbability[0] = 1.000;
            vpriorProbability[1] = 0.324;
            vpriorProbability[2] = 0.758;
        }

        vPDF = CalculateLikelihoods(tofMass2, momentum, 0);

        for (auto iparticle = 0; iparticle < name_particles.size(); ++iparticle)
        {

            double sum_prob_Ci = 0;

            for (auto ips = 0; ips < name_particles.size(); ++ips)
            {
                sum_prob_Ci += vPDF[ips] * vpriorProbability[ips];
            }
            if (momentum > fpmin && momentum < fpmax)
            {

                double results = (vPDF[iparticle] * vpriorProbability[iparticle]) / sum_prob_Ci;
                if (sum_prob_Ci == 0)
                {

                    vWi.push_back(-1);
                }
                else
                {
                    vWi.push_back(results);
                }
            }

            else
            {
                vWi.push_back(-1);
            }
        }

        return vWi;

        */
}

std::vector<Double_t> SpdMCTofParticleProducer::CalculateLikelihoods(Double_t tofMass2, Double_t momentum, Int_t hitLocation)
{

    std::vector<Double_t> vpdf;
    for (auto iparticle = 0; iparticle < name_particles.size(); ++iparticle)
    {

        Double_t sigma_p_fit, mass_fit, pdf;

        mass_fit = GetFitMass(momentum, name_particles[iparticle], hitLocation);
        sigma_p_fit = GetFitSigma_p(momentum, name_particles[iparticle], hitLocation);

        if (mass_fit == -1 || sigma_p_fit == -1)
        {
            vpdf.push_back(-1);
        }
        else
        {
            vpdf.push_back(TMath::Gaus(tofMass2, mass_fit, sigma_p_fit, true));
        }
    }

    return vpdf;
}

//________________________________________________________________________________________
Double_t SpdMCTofParticleProducer::CalculateTofMass2(Double_t timeTof, Double_t trackLen, Double_t momentum)
{

    const double timeResolution = fsigmaTimeTof * 1E-12;

    const double c = TMath::C(); //  velocity of light
    timeTof = 1E-9 * timeTof;
    trackLen = 0.01 * fabs(trackLen);
    Double_t timeTof_smearing = gRandom->Gaus(timeTof, timeResolution);

    return ((momentum * momentum)) * ((timeTof_smearing * timeTof_smearing * c * c) / (trackLen * trackLen) - 1);
}

std::vector<Double_t> SpdMCTofParticleProducer::CalculateSigma(Double_t momentum, Int_t hitLocation)
{
    std::vector<Double_t> vsigma;

    for (auto iparticle = 0; iparticle < name_particles.size(); ++iparticle)
    {

        vsigma.push_back(GetFitSigma_p(momentum, name_particles[iparticle], hitLocation));
    }

    return vsigma;
}

std::vector<Double_t> SpdMCTofParticleProducer::CalculateMean(Double_t momentum, Int_t hitLocation)
{
    std::vector<Double_t> vmean;

    for (auto iparticle = 0; iparticle < name_particles.size(); ++iparticle)
    {

        vmean.push_back(GetFitMass(momentum, name_particles[iparticle], hitLocation));
    }

    return vmean;
}

std::vector<Double_t> SpdMCTofParticleProducer::CalculateDistances(Double_t TofMass2, Double_t momentum, Int_t hitLocation)
{
    std::vector<Double_t> vdistances;

    Double_t dist = 0;
    for (auto iparticle = 0; iparticle < name_particles.size(); ++iparticle)
    {

        dist = fabs(TofMass2 - GetFitMass(momentum, name_particles[iparticle], hitLocation)) / GetFitSigma_p(momentum, name_particles[iparticle], hitLocation);

        vdistances.push_back(dist);
    }

    return vdistances;
}

Double_t func_sigma_p(Double_t *x, Double_t *par)
{
    //    return par[4] * x[0] * x[0] * x[0] * x[0] + par[3] * x[0] * x[0] * x[0] + par[0] * x[0] * x[0] + par[1] * x[0] + par[2];

    return par[0] * pow(x[0], par[1] + par[2] * x[0]) + x[0] * x[0] * par[3] + par[4];
}

TF1 *SpdMCTofParticleProducer::GetSigma_P(Double_t xmin, Double_t xmax, std::vector<Double_t> vfit_sigma)
{

    Int_t npar = 5;
    TF1 *f1 = new TF1("sigma_p", func_sigma_p, xmin, xmax, npar);

    for (auto ipar = 0; ipar < vfit_sigma.size(); ++ipar)
    {
        f1->SetParameter(ipar, vfit_sigma[ipar]);
    }

    return f1;
}

Double_t func_mean_mass(Double_t *x, Double_t *par)
{
    return par[0];
}

TF1 *SpdMCTofParticleProducer::GetMean(Double_t xmin, Double_t xmax, Double_t fit_mean)
{

    Int_t npar = 1;
    TF1 *f1 = new TF1("mean", func_mean_mass, xmin, xmax, npar);
    f1->SetParameter(0, fit_mean);

    return f1;
}

std::vector<Double_t> vfit_sigma_p_pion_B = {0.00331468, 0.898094, -2.76434, 0.0304563, -0.00406465};
std::vector<Double_t> vfit_sigma_p_kaon_B = {0.176405, 2.02268, -0.0018956, -0.150022, 0.00798679};
std::vector<Double_t> vfit_sigma_p_proton_B = {0.00909763, -1.42789, 0.0650257, 0.0301272, 0.0119396};
std::vector<Double_t> vfit_sigma_p_deuteron_B = {0.0846426, -1.46068, 0.0443884, 0.0304383, 0.0482678};

std::vector<Double_t> vfit_sigma_p_pion_E = {0.0319704, 0.0471512, -0.0890425, 0.0181072, -0.0315866};
std::vector<Double_t> vfit_sigma_p_kaon_E = {-1.42063, 0.00541639, -0.00360618, 0.0161866, 1.43065};
std::vector<Double_t> vfit_sigma_p_proton_E = {0.0172945, -1.58569, -0.650922, 0.0171401, 0.0339656};
std::vector<Double_t> vfit_sigma_p_deuteron_E = {0.232118, -1.29831, -0.52155, 0.0163479, 0.165365};

Double_t fit_mean_mass2_pion_B = 0.0195;
Double_t fit_mean_mass2_kaon_B = 0.243998;
Double_t fit_mean_mass2_proton_B = 0.8806;
Double_t fit_mean_mass2_deuteron_B = 3.49909;

Double_t fit_mean_mass2_pion_E = 0.0195841;
Double_t fit_mean_mass2_kaon_E = 0.243735;
Double_t fit_mean_mass2_proton_E = 0.881315;
Double_t fit_mean_mass2_deuteron_E = 3.49579;

Double_t SpdMCTofParticleProducer::GetFitSigma_p(Double_t momentum, std::string name_particle, Int_t hitLocation)
{

    if (momentum < fpmin)
        return -1;

    if (momentum > fpmax)
        return -1;

    if (hitLocation == 0)
        return -1;

    if (name_particle == "pion")
    {
        if (hitLocation == 1) // barrel
            return (GetSigma_P(fpmin, fpmax, vfit_sigma_p_pion_B))->Eval(momentum);
        if (hitLocation == 2) // end-cap
            return (GetSigma_P(fpmin, fpmax, vfit_sigma_p_pion_E))->Eval(momentum);
    }
    else if (name_particle == "kaon")
    {
        if (hitLocation == 1) // barrel
            return (GetSigma_P(fpmin, fpmax, vfit_sigma_p_kaon_B))->Eval(momentum);
        if (hitLocation == 2) // end-cap
            return (GetSigma_P(fpmin, fpmax, vfit_sigma_p_kaon_E))->Eval(momentum);
    }
    else if (name_particle == "proton")
    {

        if (hitLocation == 1) // barrel
            return (GetSigma_P(fpmin, fpmax, vfit_sigma_p_proton_B))->Eval(momentum);
        if (hitLocation == 2) // end-cap
            return (GetSigma_P(fpmin, fpmax, vfit_sigma_p_proton_E))->Eval(momentum);
    }
    else if (name_particle == "deuteron")
    {

        if (hitLocation == 1) // barrel
            return (GetSigma_P(fpmin, fpmax, vfit_sigma_p_deuteron_B))->Eval(momentum);
        if (hitLocation == 2) // end-cap
            return (GetSigma_P(fpmin, fpmax, vfit_sigma_p_deuteron_E))->Eval(momentum);
    }
    else
    {
        return -1;
    }
}

Double_t SpdMCTofParticleProducer::GetFitMass(Double_t momentum, std::string name_particle, Int_t hitLocation)
{

    if (momentum < fpmin)
        return -1;

    if (momentum > fpmax)
        return -1;

    if (hitLocation == 0)
        return -1;

    if (name_particle == "pion")
    {

        if (hitLocation == 1) // barrel
            return GetMean(fpmin, fpmax, fit_mean_mass2_pion_B)->Eval(momentum);
        if (hitLocation == 2) // end-cap
            return GetMean(fpmin, fpmax, fit_mean_mass2_pion_E)->Eval(momentum);
    }
    else if (name_particle == "kaon")
    {
        if (hitLocation == 1) // barrel
            return GetMean(fpmin, fpmax, fit_mean_mass2_kaon_B)->Eval(momentum);
        if (hitLocation == 2) // end-cap
            return GetMean(fpmin, fpmax, fit_mean_mass2_kaon_E)->Eval(momentum);
    }
    else if (name_particle == "proton")
    {

        if (hitLocation == 1) // barrel
            return GetMean(fpmin, fpmax, fit_mean_mass2_proton_B)->Eval(momentum);
        if (hitLocation == 2) // end-cap
            return GetMean(fpmin, fpmax, fit_mean_mass2_proton_E)->Eval(momentum);
    }
    else if (name_particle == "deuteron")
    {

        if (hitLocation == 1) // barrel
            return GetMean(fpmin, fpmax, fit_mean_mass2_deuteron_B)->Eval(momentum);
        if (hitLocation == 2) // end-cap
            return GetMean(fpmin, fpmax, fit_mean_mass2_deuteron_E)->Eval(momentum);
    }
    else
    {
        return -1;
    }
}

//________________________________________________________________________________________
void SpdMCTofParticleProducer::CheckParticle(SpdTofParticle *part)
{
    if (!part)
        return;

    cout << "\n__________________________________________________" << endl;
    part->Print();

    //------------------------------------------------------------------
    // check initialization

    SpdTofMCHit *hit = (SpdTofMCHit *)fHits->At(part->GetHitId());
    SpdTrackMC *track = (SpdTrackMC *)fTracks->At(part->GetTrackId());

    if (!hit || !track)
        return;

    SpdTrackFitPar *fitpars = track->GetFitPars();
    if (!fitpars || !fitpars->LastState())
        return;

    if (!fPropagator || !fPropagator->IsInit())
        return;

    //------------------------------------------------------------------

    // WARNING here we use ideal particle pdg!
    fPropagator->InitTrack(track->GetParticlePdg());

    SpdTrackState start_state = *(fitpars->FirstState()); // start_state;

    // WARNING don't propagate covariance matrix (for more quick calculations)!
    start_state.ClearCovariance();

    TVector3 hit_pos(hit->GetHitPos()), pos;
    Double_t delta; // cm

    SpdTrackState final_state;
    Double_t covered_distance; // cm

    //------------------------------------------------------------------

    // MC data
    SpdMCParticle *mcpart = (SpdMCParticle *)fParticles->At(track->GetParticleId());
    TVector3 start_mom = mcpart->GetStartMom();
    TVector3 fitted_mom = start_state.GetMomentum();
    Double_t dp = TMath::Abs(start_mom.Mag() - fitted_mom.Mag()) / start_mom.Mag();

    cout << ">>> SpdMCTofParticleProducer::CheckParticle <<< " << endl;
    cout << "particle: " << track->GetParticlePdg()
         << ",  p_mc = " << start_mom.Mag()
         << ",  p_rec = " << fitted_mom.Mag()
         << ",  |p_rec-p_mc|/p_mc = " << dp << endl;

    if (hit->IsBarrelHit())
    {
        cout << ">>> barrel hit case <<< " << endl;
        cout << ">>> [hit]   pos: " << hit_pos.X() << " " << hit_pos.Y() << " " << hit_pos.Z() << endl;

        covered_distance = fPropagator->ExtrapolateToCylinder(hit_pos.Perp(), start_state, final_state);

        pos = final_state.GetPosition();
        delta = (hit_pos - pos).Mag();

        cout << ">>> [cyl]   pos: " << pos.X() << " " << pos.Y() << " " << pos.Z()
             << " delta(pos): " << delta << " dist = " << covered_distance << endl;
    }
    else if (hit->IsEndcapHit())
    {
        cout << ">>> endcap hit case <<< " << endl;
        cout << ">>> [hit]   pos: " << hit_pos.X() << " " << hit_pos.Y() << " " << hit_pos.Z() << endl;

        TVector3 ppoint(0, 0, hit_pos.Z()), pnorm(0, 0, 1);

        covered_distance = fPropagator->ExtrapolateToPlane(ppoint, pnorm, start_state, final_state);

        pos = final_state.GetPosition();
        delta = (hit_pos - pos).Mag();

        cout << ">>> [plane] pos: " << pos.X() << " " << pos.Y() << " " << pos.Z()
             << " delta(pos): " << delta << " dist = " << covered_distance << endl;
    }

    covered_distance = fPropagator->ExtrapolateToPoint(hit_pos, start_state, final_state);

    pos = final_state.GetPosition();
    delta = (hit_pos - pos).Mag();

    cout << ">>> [point] pos: " << pos.X() << " " << pos.Y() << " " << pos.Z()
         << " delta(pos): " << delta << " dist = " << covered_distance << endl;
}

//________________________________________________________________________________________
void SpdMCTofParticleProducer::Finish()
{
    if (fVerboseLevel > -1)
    {
        cout << "\n-I- <SpdMCTofParticleProducer::Finish>  "; // << endl;
    }
}

//________________________________________________________________________________________
SpdTofParticle *SpdMCTofParticleProducer::AddParticle()
{
    if (!fTofParticles)
        return 0;
    Int_t size = fTofParticles->GetEntriesFast();
    SpdTofParticle *particle = new ((*fTofParticles)[size]) SpdTofParticle();
    particle->SetId(size);
    return particle;
}
