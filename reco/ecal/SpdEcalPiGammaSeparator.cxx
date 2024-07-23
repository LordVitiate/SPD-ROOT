// $Id$
// Author: amaltsev andrii.maltsev@cern.ch  2022/11/28

#include <TClonesArray.h>
#include "SpdEcalPiGammaSeparator.h"
#include "SpdEcalRCParticle.h"
#include "SpdEcalRCCluster.h"
#include "SpdEcalMCHit.h"
#include "SpdEcalPiGammaSeparatorParameters.h"

using std::vector;
using std::pair;
using std::tuple;
using std::make_tuple;
using std::make_pair;
using std::cout;
using std::endl;
using std::get;



//________________________________________________________________________________
Double_t SpdEcalPiGammaSeparator::GetClusterRMS(const vector<Int_t> & cells, const TClonesArray* hits) const {

    Double_t er2 = 0.;
    TVector3 er = TVector3(0.,0.,0.);
    Double_t e = 0.;

    SpdEcalMCHit* hit;
    for (auto c: cells) {
        hit = (SpdEcalMCHit*)hits->At(c);
        Double_t energy = hit->GetEnergy();
        TVector3 pos = hit->GetPosition();
        e += energy;
        er += energy*pos;
    }

    TVector3 cog = er*(1./e);

    for (auto c: cells) {
        hit = (SpdEcalMCHit*)hits->At(c);
        Double_t energy = hit->GetEnergy();
        TVector3 pos = hit->GetPosition();
        er2 += energy*(pos - cog).Mag2();
    }

    return TMath::Sqrt(er2/e);

}

//________________________________________________________________________________
pair<Int_t, Int_t> SpdEcalPiGammaSeparator::GetClusterTwoMaxEnergyIndices(const vector<Int_t> & cells, const TClonesArray* hits) const {

    Int_t imax1 = -1;
    Int_t imax2 = -1;
    Double_t emax1 = 0.;
    Double_t emax2 = 0.;

    SpdEcalMCHit* hit;
    for (auto c: cells) {
        hit = (SpdEcalMCHit*)hits->At(c);
        Double_t energy = hit->GetEnergy();
        if (energy > emax1) {
            if (imax1 != -1) {
                imax2 = imax1;
                emax2 = emax1;
            }
            emax1 = energy;
            imax1 = c;
        }
        else if (energy > emax2) {
            emax2 = energy;
            imax2 = c;
        }
    }
    return make_pair(imax1, imax2);
}

//________________________________________________________________________________
Int_t SpdEcalPiGammaSeparator::GetClusterMaxEnergyCellIndex(const vector<Int_t> & cells, const TClonesArray* hits) const {

    Int_t imax = -1;
    Double_t emax = 0.;

    SpdEcalMCHit* hit;
    for (auto c: cells) {
        hit = (SpdEcalMCHit*)hits->At(c);
        Double_t energy = hit->GetEnergy();
        if (energy > emax) {
            emax = energy;
            imax = c;
        }
    }
    if (imax == -1) {
        for (auto c: cells) {
            hit = (SpdEcalMCHit*)hits->At(c);
            Double_t energy = hit->GetEnergy();
            cout << "energy: " << energy << endl;
        }
        exit(1);
    }
    return imax;
}


//________________________________________________________________________________
Double_t SpdEcalPiGammaSeparator::GetClusterS1(const vector<Int_t> & cells, const TClonesArray* hits) const {

    Int_t imax = GetClusterMaxEnergyCellIndex(cells, hits);
    SpdEcalMCHit* maxHit = (SpdEcalMCHit*)hits->At(imax);

    return maxHit->GetEnergy();

}

//________________________________________________________________________________
Int_t SpdEcalPiGammaSeparator::GetBarrelCellGlobalZ(Int_t basketid, Int_t zid, Int_t cellid) const {
    if (nBasketModulesZ == -1) {
        cout << "please initialize the correct value of nBasketModulesZ before using GetBarrelCellGlobalZ!" << endl;
        exit(1);
    }
    if (nBasketsZ == -1) {
        cout << "please initialize the correct value of nBasketsZ before using GetBarrelCellGlobalZ!" << endl;
        exit(1);
    }
    Int_t nModulesBefore = ((basketid - 1) % nBasketsZ)*nBasketModulesZ + (zid - 1); 
    return 2*nModulesBefore + ((cellid-1) % 2) + 1;

}


//________________________________________________________________________________
Int_t SpdEcalPiGammaSeparator::GetBarrelCellGlobalPhi(Int_t basketid, Int_t phiid, Int_t cellid) const {
    if (nBasketModulesPhi == -1) {
        cout << "please initialize the correct value of nBasketModulesPhi before using GetBarrelCellGlobalPhi!" << endl;
        exit(1);
    }
    Int_t nModulesBefore = ((basketid - 1) / nBasketsZ)*nBasketModulesPhi + (phiid - 1); 
    return 2*nModulesBefore + ((cellid-1) / 2) + 1;
}


//________________________________________________________________________________
pair<Int_t, Int_t> SpdEcalPiGammaSeparator::GetEndcapCellGlobalXY(Int_t modid, Int_t cellid) const {

    Int_t modx = modid % 1000;
    Int_t cellx = (modx-1)*2 + (cellid == 2 || cellid == 4) + 1;
    Int_t mody = modid / 1000;
    Int_t celly = (mody-1)*2 + (cellid == 3 || cellid == 4) + 1;
    return make_pair(cellx, celly);
}

//________________________________________________________________________________
pair<Int_t, Int_t> SpdEcalPiGammaSeparator::GetGlobalCellIndices(SpdEcalMCHit* hit) const {

    if (hit->IsBarrelHit()) {
        Int_t basketid = hit->GetBasketID();
        Int_t modzid = hit->GetModuleZID();
        Int_t modphiid = hit->GetModulePhiID();
        Int_t cellid = hit->GetCellID();
        Int_t zid = GetBarrelCellGlobalZ(basketid, modzid, cellid);
        Int_t phiid = GetBarrelCellGlobalPhi(basketid, modphiid, cellid);

        //for testing
//        TVector3 pos = hit->GetPosition();
//        map_zid_zpos[zid] = pos.Z();
//        map_phiid_phipos[phiid] = pos.Phi();

        return make_pair(zid, phiid);
    }
    else {
        Int_t modid = hit->GetModuleID();
        Int_t cellid = hit->GetCellID();
        pair<Int_t, Int_t> globcellid = GetEndcapCellGlobalXY(modid, cellid); 
        Int_t xid = globcellid.first;
        Int_t yid = globcellid.second;

        //for testing
//        TVector3 pos = hit->GetPosition();
//        if (hit->GetEndcapID() == 0) {
//            map_ec0_xid_xpos[xid] = pos.X();
//            map_ec0_yid_ypos[yid] = pos.Y();
//        }
//        else {
//            map_ec1_xid_xpos[xid] = pos.X();
//            map_ec1_yid_ypos[yid] = pos.Y();
//        }
        return make_pair(xid, yid);
    }

}

//________________________________________________________________________________
Double_t SpdEcalPiGammaSeparator::CellDistance(pair<Int_t, Int_t> idx1, pair<Int_t, Int_t> idx2, Bool_t isbarrel) const {     

   Double_t xdiff = fabs(Double_t(idx1.first - idx2.first)); //x or z
   Double_t ydiff = fabs(Double_t(idx1.second - idx2.second)); //y or phi

   if (isbarrel) {
        ydiff = TMath::Min(ydiff, nCellsPhi - ydiff);
   }

   return TMath::Sqrt(xdiff*xdiff + ydiff*ydiff);
}


//________________________________________________________________________________
Double_t SpdEcalPiGammaSeparator::GetClusterS4(const vector<Int_t> & cells, const TClonesArray* hits) const {

    Int_t imax = GetClusterMaxEnergyCellIndex(cells, hits); //suboptimal??? called many times???
    SpdEcalMCHit* maxHit = (SpdEcalMCHit*)hits->At(imax);

    pair<Int_t, Int_t> maxEidx = GetGlobalCellIndices(maxHit); //zid/phiid for barrel, xid/yid for endcap

    Double_t emain = maxHit->GetEnergy();

    Double_t epp = 0.; //++
    Double_t emm = 0.; //--
    Double_t epm = 0.; //+-
    Double_t emp = 0.; //-+

    SpdEcalMCHit* hit;
    for (auto c: cells) {
        if (c == imax) continue;
        hit = (SpdEcalMCHit*)hits->At(c);
        Double_t e = hit->GetEnergy();
        pair<Int_t, Int_t> currIdx = GetGlobalCellIndices(hit);
        if (CellDistance(maxEidx, currIdx, hit->IsBarrelHit()) < 1.9) {
            if (currIdx.first >= maxEidx.first && currIdx.second >= maxEidx.second) epp += e;
            if (currIdx.first <= maxEidx.first && currIdx.second >= maxEidx.second) emp += e;
            if (currIdx.first >= maxEidx.first && currIdx.second <= maxEidx.second) epm += e;
            if (currIdx.first <= maxEidx.first && currIdx.second <= maxEidx.second) emm += e;
        } 
    }

    return emain + TMath::Max(TMath::Max(TMath::Max(epp, emm), epm), emp);

}



//________________________________________________________________________________
Double_t SpdEcalPiGammaSeparator::GetClusterM2(const vector<Int_t> & cells, const TClonesArray* hits) const {

    pair<Int_t, Int_t> imaxIdx = GetClusterTwoMaxEnergyIndices(cells, hits); //suboptimal??? called many times???
    Int_t imax1 = imaxIdx.first;
    Int_t imax2 = imaxIdx.second;
    if (imax2 == -1) return 0.;

    SpdEcalMCHit* secondHit = (SpdEcalMCHit*)hits->At(imax2);

    return secondHit->GetEnergy();
}


//________________________________________________________________________________
Double_t SpdEcalPiGammaSeparator::GetClusterS6(const vector<Int_t> & cells, const TClonesArray* hits) const {

    pair<Int_t, Int_t> imaxIdx = GetClusterTwoMaxEnergyIndices(cells, hits); //suboptimal??? called many times???
    Int_t imax1 = imaxIdx.first;
    Int_t imax2 = imaxIdx.second;
    if (imax2 == -1) return 0.;
    SpdEcalMCHit* maxHit = (SpdEcalMCHit*)hits->At(imax1);
    SpdEcalMCHit* secondHit = (SpdEcalMCHit*)hits->At(imax2);

    pair<Int_t, Int_t> maxEidx = GetGlobalCellIndices(maxHit); //zid/phiid for barrel, xid/yid for endcap
    pair<Int_t, Int_t> secondEidx = GetGlobalCellIndices(secondHit);

    if (CellDistance(maxEidx, secondEidx, maxHit->IsBarrelHit()) > 1.1) {
        return 0.;
    }

    Double_t emain = maxHit->GetEnergy() + secondHit->GetEnergy();

    Double_t epp = 0.; //++
    Double_t emm = 0.; //--
    Double_t epm = 0.; //+-
    Double_t emp = 0.; //-+

    SpdEcalMCHit* hit;
    for (auto c: cells) {
        if (c == imax1 || c == imax2) continue;
        hit = (SpdEcalMCHit*)hits->At(c);
        Double_t e = hit->GetEnergy();
        pair<Int_t, Int_t> currIdx = GetGlobalCellIndices(hit);
        if (CellDistance(maxEidx, currIdx, hit->IsBarrelHit()) < 1.9 || CellDistance(secondEidx, currIdx, hit->IsBarrelHit()) < 1.9) {
            if ((currIdx.first >= maxEidx.first && currIdx.second >= maxEidx.second) || (currIdx.first >= secondEidx.first && currIdx.second >= secondEidx.second)) epp += e;
            if ((currIdx.first <= maxEidx.first && currIdx.second >= maxEidx.second) || (currIdx.first <= secondEidx.first && currIdx.second >= secondEidx.second)) emp += e;
            if ((currIdx.first >= maxEidx.first && currIdx.second <= maxEidx.second) || (currIdx.first >= secondEidx.first && currIdx.second <= secondEidx.second)) epm += e;
            if ((currIdx.first <= maxEidx.first && currIdx.second <= maxEidx.second) || (currIdx.first <= secondEidx.first && currIdx.second <= secondEidx.second)) emm += e;
        } 
    }

    return emain + TMath::Max(TMath::Max(TMath::Max(epp, emm), epm), emp);

}


//________________________________________________________________________________
Double_t SpdEcalPiGammaSeparator::GetClusterS6ratio(const vector<Int_t> & cells, const TClonesArray* hits) const {

    pair<Int_t, Int_t> imaxIdx = GetClusterTwoMaxEnergyIndices(cells, hits); //suboptimal??? called many times???
    Int_t imax1 = imaxIdx.first;
    Int_t imax2 = imaxIdx.second;
    if (imax2 == -1) return 0.;
    SpdEcalMCHit* maxHit = (SpdEcalMCHit*)hits->At(imax1);
    SpdEcalMCHit* secondHit = (SpdEcalMCHit*)hits->At(imax2);

    pair<Int_t, Int_t> maxEidx = GetGlobalCellIndices(maxHit); //zid/phiid for barrel, xid/yid for endcap
    pair<Int_t, Int_t> secondEidx = GetGlobalCellIndices(secondHit);

    if (CellDistance(maxEidx, secondEidx, maxHit->IsBarrelHit()) > 1.1) {
        return 0.;
    }

    Double_t en = 0.;
    Double_t ed = 0.;

    SpdEcalMCHit* hit;
    for (auto c: cells) {
        if (c == imax1 || c == imax2) continue;
        hit = (SpdEcalMCHit*)hits->At(c);
        Double_t e = hit->GetEnergy();
        pair<Int_t, Int_t> currIdx = GetGlobalCellIndices(hit);
        if (maxEidx.first == secondEidx.first) {
            if (abs(currIdx.first - maxEidx.first) == 1 && currIdx.second == maxEidx.second) {
                ed += e;
            }
            if (abs(currIdx.first - secondEidx.first) == 1 && currIdx.second == secondEidx.second) {
                en += e;
            }
        }
        else if (maxEidx.second == secondEidx.second) {
            if (abs(currIdx.second - maxEidx.second) == 1 && currIdx.first == maxEidx.first) {
               ed += e; 
            } 
            if (abs(currIdx.second - secondEidx.second) == 1 && currIdx.first == secondEidx.first) {
                en += e;
            }
        }
        else {
            cout << "GetClusterS6ratio: cannot be! difference between most and second most energy positions should be 1 in one of dimensions!" << endl;
            exit(1);
        }

    }
    
    return en/(ed+0.5);

}


//________________________________________________________________________________
Double_t SpdEcalPiGammaSeparator::GetClusterS9(const vector<Int_t> & cells, const TClonesArray* hits) const {

    Int_t imax = GetClusterMaxEnergyCellIndex(cells, hits); //suboptimal??? called many times???
    SpdEcalMCHit* maxHit = (SpdEcalMCHit*)hits->At(imax);

    pair<Int_t, Int_t> maxEidx = GetGlobalCellIndices(maxHit); //zid/phiid for barrel, xid/yid for endcap
    Double_t esum = maxHit->GetEnergy();

    SpdEcalMCHit* hit;
    for (auto c: cells) {
        if (c == imax) continue;
        hit = (SpdEcalMCHit*)hits->At(c);
        pair<Int_t, Int_t> currIdx = GetGlobalCellIndices(hit);
        if (CellDistance(maxEidx, currIdx, hit->IsBarrelHit()) < 1.9) {
            esum += hit->GetEnergy(); 
        }
    }

    return esum;

}

//________________________________________________________________________________
Double_t SpdEcalPiGammaSeparator::GetClusterS25(const vector<Int_t> & cells, const TClonesArray* hits) const {

    Int_t imax = GetClusterMaxEnergyCellIndex(cells, hits); //suboptimal??? called many times???
    SpdEcalMCHit* maxHit = (SpdEcalMCHit*)hits->At(imax);

    pair<Int_t, Int_t> maxEidx = GetGlobalCellIndices(maxHit); //zid/phiid for barrel, xid/yid for endcap
    Double_t esum = maxHit->GetEnergy();

    SpdEcalMCHit* hit;
    for (auto c: cells) {
        if (c == imax) continue;
        hit = (SpdEcalMCHit*)hits->At(c);
        pair<Int_t, Int_t> currIdx = GetGlobalCellIndices(hit);
        if (CellDistance(maxEidx, currIdx, hit->IsBarrelHit()) < 2.9) {
            esum += hit->GetEnergy(); 
        }
    }

    return esum;

}

//________________________________________________________________________________
Double_t SpdEcalPiGammaSeparator::GetClusterXCog25(const vector<Int_t> & cells, const TClonesArray* hits) const {

    Int_t imax = GetClusterMaxEnergyCellIndex(cells, hits); //suboptimal??? called many times???
    SpdEcalMCHit* maxHit = (SpdEcalMCHit*)hits->At(imax);
    TVector3 mainpos = maxHit->GetPosition();

    pair<Int_t, Int_t> maxEidx = GetGlobalCellIndices(maxHit); //zid/phiid for barrel, xid/yid for endcap
    Double_t esum = maxHit->GetEnergy();
    Double_t xesum = 0.;

    SpdEcalMCHit* hit;
    for (auto c: cells) {
        if (c == imax) continue;
        hit = (SpdEcalMCHit*)hits->At(c);
        pair<Int_t, Int_t> currIdx = GetGlobalCellIndices(hit);
        if (CellDistance(maxEidx, currIdx, hit->IsBarrelHit()) < 2.9) {
            esum += hit->GetEnergy(); 
            xesum += hit->GetEnergy()*(hit->GetPosition().Z() - mainpos.Z());
        }
    }

    return xesum/esum;

}


//________________________________________________________________________________
Double_t SpdEcalPiGammaSeparator::GetClusterYCog25(const vector<Int_t> & cells, const TClonesArray* hits) const {

    Int_t imax = GetClusterMaxEnergyCellIndex(cells, hits); //suboptimal??? called many times???
    SpdEcalMCHit* maxHit = (SpdEcalMCHit*)hits->At(imax);

    pair<Int_t, Int_t> maxEidx = GetGlobalCellIndices(maxHit); //zid/phiid for barrel, xid/yid for endcap
    Double_t esum = maxHit->GetEnergy();
    TVector3 mainpos = maxHit->GetPosition();
    Double_t yesum = 0.;

    SpdEcalMCHit* hit;
    for (auto c: cells) {
        if (c == imax) continue;
        hit = (SpdEcalMCHit*)hits->At(c);
        pair<Int_t, Int_t> currIdx = GetGlobalCellIndices(hit);
        if (CellDistance(maxEidx, currIdx, hit->IsBarrelHit()) < 2.9) {
            esum += hit->GetEnergy(); 
            TVector3 currpos = hit->GetPosition();
            Double_t phidiff = fabs(currpos.Phi() - mainpos.Phi());
            if (fabs(TMath::TwoPi() - phidiff) < phidiff) {
                if (currpos.Phi() < mainpos.Phi()) {
                    yesum += hit->GetEnergy()*(currpos.Phi() + TMath::TwoPi() - mainpos.Phi());
                }
                else {
                    yesum += hit->GetEnergy()*(currpos.Phi() - TMath::TwoPi() - mainpos.Phi());
                }
            }
            else {
                yesum += hit->GetEnergy()*(hit->GetPosition().Phi() - mainpos.Phi());
            }
        }
    }

    return yesum/esum;

}

//________________________________________________________________________________
tuple<Double_t, Double_t, Double_t> SpdEcalPiGammaSeparator::GetClusterCovPars(const vector<Int_t> & cells, const TClonesArray* hits) const {

    Double_t w0 = 3.;


    Int_t imax = GetClusterMaxEnergyCellIndex(cells, hits); //suboptimal??? called many times???
    SpdEcalMCHit* maxHit = (SpdEcalMCHit*)hits->At(imax);
    TVector3 mainpos = maxHit->GetPosition();
    pair<Int_t, Int_t> maxEidx = GetGlobalCellIndices(maxHit); //zid/phiid for barrel, xid/yid for endcap

    Double_t sumw = 0.;
    Double_t s25 = GetClusterS25(cells, hits);
    Double_t sumzw = 0.;
    Double_t sumphiw = 0.;

    SpdEcalMCHit* hit;
    for (auto c: cells) {
        hit = (SpdEcalMCHit*)hits->At(c);
        pair<Int_t, Int_t> currIdx = GetGlobalCellIndices(hit);
        Double_t e = hit->GetEnergy(); 
        TVector3 currpos = hit->GetPosition();

        if (CellDistance(maxEidx, currIdx, hit->IsBarrelHit()) > 2.9) continue;

        Double_t w = TMath::Max(0., w0 + TMath::Log(e/s25));
        sumw += w;
        sumzw += w*currpos.Z();

        Double_t phidiff = fabs(currpos.Phi() - mainpos.Phi());
        if (fabs(TMath::TwoPi() - phidiff) < phidiff) {
            if (currpos.Phi() < mainpos.Phi()) {
                sumphiw += w*(currpos.Phi() + TMath::TwoPi());
            }
            else {
                sumphiw += w*(currpos.Phi() - TMath::TwoPi());
            }
        }
        else {
            sumphiw += w*hit->GetPosition().Phi();
        }
    }

    Double_t meanz = sumzw/sumw;
    Double_t meanphi = sumphiw/sumw;

    Double_t sumzzw = 0.;
    Double_t sumzphiw = 0.;
    Double_t sumphiphiw = 0.;

    
    for (auto c: cells) {
        hit = (SpdEcalMCHit*)hits->At(c);
        pair<Int_t, Int_t> currIdx = GetGlobalCellIndices(hit);
        Double_t e = hit->GetEnergy(); 
        TVector3 currpos = hit->GetPosition();

        if (CellDistance(maxEidx, currIdx, hit->IsBarrelHit()) > 2.9) continue;

        Double_t w = TMath::Max(0., w0 + TMath::Log(e/s25));
        sumzzw += w*(currpos.Z() - meanz)*(currpos.Z() - meanz);

        Double_t phidiff = fabs(currpos.Phi() - meanphi);
        if (fabs(TMath::TwoPi() - phidiff) < phidiff) {
            if (currpos.Phi() < meanphi) {
                sumzphiw += w*(currpos.Z() - meanz)*(currpos.Phi() + TMath::TwoPi() - meanphi);
                sumphiphiw += w*(currpos.Phi() + TMath::TwoPi() - meanphi)*(currpos.Phi() + TMath::TwoPi() - meanphi);
            }
            else {
                sumzphiw += w*(currpos.Z() - meanz)*(currpos.Phi() - TMath::TwoPi() - meanphi);
                sumphiphiw += w*(currpos.Phi() - TMath::TwoPi() - meanphi)*(currpos.Phi() - TMath::TwoPi() - meanphi);
            }
        }
        else {
            sumzphiw += w*(currpos.Z() - meanz)*(hit->GetPosition().Phi() - meanphi);
            sumphiphiw += w*(hit->GetPosition().Phi() - meanphi)*(hit->GetPosition().Phi() - meanphi);
        }
    }

    Double_t sigmazz = sumzzw/sumw;
    Double_t sigmazphi = sumzphiw/sumw;
    Double_t sigmaphiphi = sumphiphiw/sumw;

    return make_tuple(sigmazz, sigmazphi, sigmaphiphi);
    

}



//________________________________________________________________________________
Double_t SpdEcalPiGammaSeparator::GetClusterThetaLinWeighted(const vector<Int_t> & cells, const TClonesArray* hits) const {

    TVector3 epos = TVector3(0., 0., 0.);
    Double_t esum = 0.;

    SpdEcalMCHit* hit;
    if (cells.size() == 0) return -1.;
    hit = (SpdEcalMCHit*)hits->At(cells[0]);
    bool isBarrel = hit->IsBarrelHit();

    for (auto c: cells) {
        hit = (SpdEcalMCHit*)hits->At(c);
        Double_t energy = hit->GetEnergy();
        TVector3 pos = hit->GetPosition();
        esum += energy;
        epos += energy*pos;
    }

    double thetaAvg = (epos*(1./esum)).Theta();
    double ret = isBarrel ? fabs(TMath::Pi()/2 - thetaAvg) : std::min(thetaAvg, TMath::Pi() - thetaAvg);
    return ret;
}



//________________________________________________________________________________
Int_t SpdEcalPiGammaSeparator::GetParticleType(const SpdEcalRCCluster* cl, const TClonesArray* hits) const {

    const vector<Int_t> & cells = cl->GetCells();

    Double_t clRMS = GetClusterRMS(cells, hits);
    Double_t clS1 = GetClusterS1(cells, hits);
    Double_t clS4 = GetClusterS4(cells, hits);
    Double_t clS6 = GetClusterS6(cells, hits);
    Double_t clS6ratio = GetClusterS6ratio(cells, hits);
    Double_t clS9 = GetClusterS9(cells, hits);
    Double_t clS25 = GetClusterS25(cells, hits);
    Double_t clM2 = GetClusterM2(cells, hits);
    Double_t clXCog25 = GetClusterXCog25(cells, hits);
    Double_t clYCog25 = GetClusterYCog25(cells, hits);

    tuple<Double_t, Double_t, Double_t> clCovPars = GetClusterCovPars(cells, hits);
    Double_t clCovZZ = get<0>(clCovPars);
    Double_t clCovZPhi = get<1>(clCovPars);
    Double_t clCovPhiPhi = get<2>(clCovPars);
    Double_t clThetaLinAvg = GetClusterThetaLinWeighted(cells, hits);


    ClusterInfo inp({clRMS, clS1, clS4, clS6, clS6ratio, clS9, clS25, clM2, clXCog25, clYCog25, clCovZZ, clCovZPhi, clCovPhiPhi, clThetaLinAvg});

    return GetParticleType(inp);
}

//________________________________________________________________________________
Double_t SpdEcalPiGammaSeparator::GetNeuralNetworkOutput(const vector<Double_t> & input) const {


    vector<Double_t> h0(nHidden0, 0.);
    for (int j = 0; j < nHidden0; ++j) {
        for (int i = 0; i < nInputs; ++i) {
            h0[j] += weight0[j][i]*input[i];
        } 
        h0[j] += bias0[j];
    }

    for (int i = 0; i < nHidden0; ++i) {
        h0[i] = batchnorms0_weight[i]*(h0[i] - batchnorms0_running_mean[i])/sqrt(batchnorms0_running_var[i] + eps) + 
                    batchnorms0_bias[i];
    }    
    for (int i = 0; i < nHidden0; ++i) {
        h0[i] = (h0[i] > 0.) ? h0[i] : 0.;
    }    


    vector<Double_t> h1(nHidden1, 0.);
    for (int j = 0; j < nHidden1; ++j) {
        for (int i = 0; i < nHidden0; ++i) {
            h1[j] += weight1[j][i]*h0[i];
        } 
        h1[j] += bias1[j];
    }

    for (int i = 0; i < nHidden1; ++i) {
        h1[i] = batchnorms1_weight[i]*(h1[i] - batchnorms1_running_mean[i])/sqrt(batchnorms1_running_var[i] + eps) + 
                    batchnorms1_bias[i];
    }    
    for (int i = 0; i < nHidden1; ++i) {
        h1[i] = (h1[i] > 0.) ? h1[i] : 0.;
    }    


    double nn_output = 0.;
    for (int i = 0; i < nHidden1; ++i) {
        nn_output += h1[i]*weight2[0][i];
    }    
    nn_output += bias2[0];

    return 1./(1 + exp(-nn_output));

}

//________________________________________________________________________________
Int_t SpdEcalPiGammaSeparator::GetParticleType(const ClusterInfo & rawInput) const {

    double lambdaPlus = 0.5*(rawInput.covZZ + rawInput.covPhiPhi + sqrt(pow(rawInput.covZZ - rawInput.covPhiPhi, 2) + 4*pow(rawInput.covZPhi, 2)));
    double lambdaMinus = 0.5*(rawInput.covZZ + rawInput.covPhiPhi - sqrt(pow(rawInput.covZZ - rawInput.covPhiPhi, 2) + 4*pow(rawInput.covZPhi, 2)));

    vector<Double_t> inp {
        rawInput.xcog25,
        rawInput.covZZ/20,
        rawInput.covPhiPhi/0.0005,
        rawInput.s1/rawInput.s9,
        fabs(rawInput.s25 - rawInput.s1) > 1e-6 ? (rawInput.s9 - rawInput.s1)/(rawInput.s25 - rawInput.s1) : 1.0,
        rawInput.s4/rawInput.s25,
        rawInput.ycog25,
        rawInput.s6ratio,
        rawInput.s6/rawInput.s9,
        fabs(lambdaPlus) > 1e-6 ? lambdaMinus/lambdaPlus : 1.0,
        (rawInput.m2 + rawInput.s1)/rawInput.s9,
        (rawInput.m2 + rawInput.s1)/rawInput.s4,
        rawInput.thetaLinAvg
    };


    if (inp.size() != nInputs) {
        cout << "Error in SpdEcalPiGammaSeparator::GetParticleType : Number of inputs: " << inp.size()  << " not equal to number of inputs in the network topology: " << nInputs << endl;
        return -1;
    }

    for (int i = 0; i < inp.size(); ++i) {
        inp[i] = (inp[i] - scaler_mean[i])/scaler_sigma[i];
    }


    Double_t nn_output = GetNeuralNetworkOutput(inp);
    
    if (nn_output < 0.2) return 22;
    else return 111;

};

