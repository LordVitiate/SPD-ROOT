// $Id$
// Author: amaltsev andrii.maltsev@cern.ch  2022/11/28

#ifndef __SPDECALPIGAMMASEPARATOR_H__
#define __SPDECALPIGAMMASEPARATOR_H__

#include <TClonesArray.h>
#include <vector>
#include "SpdEcalRCParticle.h"
#include "SpdEcalRCCluster.h"
#include "SpdEcalMCHit.h"

using std::vector;
using std::pair;
using std::tuple;
using std::make_pair;
using std::cout;
using std::endl;

class SpdEcalPiGammaSeparator {

    public:

        SpdEcalPiGammaSeparator() :
            nBasketModulesZ(-1), nBasketModulesPhi(-1), nBasketsZ(-1), nCellsPhi(-1)
            {}; 
        SpdEcalPiGammaSeparator(Int_t fnBasketModulesZ, Int_t fnBasketModulesPhi, Int_t fnBasketsZ, Int_t fnCellsPhi) :
            nBasketModulesZ(fnBasketModulesZ), nBasketModulesPhi(fnBasketModulesPhi), nBasketsZ(fnBasketsZ), 
            nCellsPhi(fnCellsPhi)
            {}; 

        struct ClusterInfo {
            Double_t rms;
            Double_t s1;
            Double_t s4;
            Double_t s6;
            Double_t s6ratio;
            Double_t s9;
            Double_t s25;
            Double_t m2;
            Double_t xcog25;
            Double_t ycog25;
            Double_t covZZ;
            Double_t covZPhi;
            Double_t covPhiPhi;
            Double_t thetaLinAvg;
        };



        Double_t GetClusterRMS(const vector<Int_t> & cells, const TClonesArray* hits) const;
        pair<Int_t, Int_t> GetClusterTwoMaxEnergyIndices(const vector<Int_t> & cells, const TClonesArray* hits) const;
        Int_t GetClusterMaxEnergyCellIndex(const vector<Int_t> & cells, const TClonesArray* hits) const;
        Double_t GetClusterS1(const vector<Int_t> & cells, const TClonesArray* hits) const;
        Int_t GetBarrelCellGlobalZ(Int_t basketid, Int_t zid, Int_t cellid) const;
        Int_t GetBarrelCellGlobalPhi(Int_t basketid, Int_t phiid, Int_t cellid) const;
        pair<Int_t, Int_t> GetEndcapCellGlobalXY(Int_t modid, Int_t cellid) const;
        pair<Int_t, Int_t> GetGlobalCellIndices(SpdEcalMCHit* hit) const;
        Double_t CellDistance(pair<Int_t, Int_t> idx1, pair<Int_t, Int_t> idx2, Bool_t isbarrel) const;
        Double_t GetClusterS4(const vector<Int_t> & cells, const TClonesArray* hits) const;
        Double_t GetClusterM2(const vector<Int_t> & cells, const TClonesArray* hits) const;
        Double_t GetClusterS6(const vector<Int_t> & cells, const TClonesArray* hits) const;
        Double_t GetClusterS6ratio(const vector<Int_t> & cells, const TClonesArray* hits) const;
        Double_t GetClusterS9(const vector<Int_t> & cells, const TClonesArray* hits) const;
        Double_t GetClusterS25(const vector<Int_t> & cells, const TClonesArray* hits) const;
        Double_t GetClusterXCog25(const vector<Int_t> & cells, const TClonesArray* hits) const;
        Double_t GetClusterYCog25(const vector<Int_t> & cells, const TClonesArray* hits) const;
        Double_t GetClusterThetaLinWeighted(const vector<Int_t> & cells, const TClonesArray* hits) const;
        tuple<Double_t, Double_t, Double_t> GetClusterCovPars(const vector<Int_t> & cells, const TClonesArray* hits) const;

        Int_t GetParticleType(const SpdEcalRCCluster* cl, const TClonesArray* hits) const;
        Int_t GetParticleType(const ClusterInfo & inp) const;
        Double_t GetNeuralNetworkOutput(const vector<Double_t> & input) const;

    private:
        Int_t nBasketModulesZ;
        Int_t nBasketModulesPhi;
        Int_t nBasketsZ;
        Int_t nCellsPhi;

        static const Int_t nInputs = 13;
        static const Int_t nHidden0 = 64;
        static const Int_t nHidden1 = 64;
        static constexpr double eps = 1e-5;

        static vector<Double_t> scaler_mean;
        static vector<Double_t> scaler_sigma;

        static vector<vector<Double_t> > weight0; //nHidden0 x nInputs
        static vector<Double_t> bias0;
        static vector<vector<Double_t> > weight1;
        static vector<Double_t> bias1;
        static vector<vector<Double_t> > weight2;
        static vector<Double_t> bias2;

        static vector<Double_t> batchnorms0_weight;
        static vector<Double_t> batchnorms0_bias;
        static vector<Double_t> batchnorms0_running_mean;
        static vector<Double_t> batchnorms0_running_var;

        static vector<Double_t> batchnorms1_weight;
        static vector<Double_t> batchnorms1_bias;
        static vector<Double_t> batchnorms1_running_mean;
        static vector<Double_t> batchnorms1_running_var;




};

#endif  /* __SPDECALPIGAMMASEPARATOR_H__ */
