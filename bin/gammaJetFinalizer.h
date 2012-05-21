#pragma once

#include "Tree/AnalysisTree.h"
#include "Tree/PhotonTree.h"
#include "Tree/LeptonTree.h"
#include "Tree/JetTree.h"
#include "Tree/METTree.h"
#include "Tree/MiscTree.h"

#include "etaBinning.h"
#include "ptBinning.h"
#include "extrapBinning.h"

#include <vector>

namespace fwlite {
  class TFileService;
}

namespace pat {
  class Photon;
  class Jet;
}

namespace edm {
  class LumiReWeighting;
  class EventBase;
}

class TTree;
class TChain;
class TFileDirectory;

enum JetAlgo {
  AK5,
  AK7
};

enum JetType {
  PF,
  CALO
};

template <typename T>
struct ExtrapolationVectors {
    typedef std::vector<std::vector<std::vector<T*> > > type;
};

class GammaJetFinalizer
{
  public:
    GammaJetFinalizer();
    ~GammaJetFinalizer();

    void setInputFiles(const std::vector<std::string>& files) {
      mInputFiles = files;
      checkInputFiles();
    }

    void setDatasetName(const std::string& name) {
      mDatasetName = name;
    }
    
    void setJetAlgo(const std::string& jetType, const std::string& jetAlgo) {
      if (jetType == "pf") {
        mJetType = PF;
      } else {
        mJetType = CALO;
      }

      if (jetAlgo == "ak5") {
        mJetAlgo = AK5;
      } else {
        mJetAlgo = AK7;
      }
    }

    void setMC(bool isMC) {
      mIsMC = isMC;
    }

    void setMCComparison(bool mcComparison) {
      mDoMCComparison = mcComparison;
    }

    void setUseExternalJEC(bool useExternalJEC) {
      mUseExternalJECCorrecion = useExternalJEC;
    }

    void setBatchJob(int currentJob, int totalJobs) {
      mIsBatchJob = true;
      mCurrentJob = currentJob;
      mTotalJobs = totalJobs;
    }

    void setUncutTrees(bool uncutTrees) {
      mUncutTrees = uncutTrees;
    }

    void runAnalysis();

  private:
    void checkInputFiles();
    void loadFiles(TChain& chain);

    void doSecondJetExtrapolation();

    void computePUWeight();
 
    template<typename T>
      std::vector<std::vector<T*> > buildEtaVector(TFileDirectory dir, const std::string& branchName, int nBins, double xMin, double xMax);
    template<typename T>
      std::vector<T*> buildVector(TFileDirectory dir, const std::string& branchName, const std::string& etaName, int nBins, double xMin, double xMax);
    template<typename T>
      std::vector<std::vector<std::vector<T*> > > buildExtrapolationEtaVector(TFileDirectory dir, const std::string& branchName, int nBins, double xMin, double xMax);
    template<typename T>
      std::vector<std::vector<T*> > buildExtrapolationVector(TFileDirectory dir, const std::string& branchName, const std::string& etaName, int nBins, double xMin, double xMax);

    void cloneTree(TTree* from, TTree*& to);

    std::string buildPostfix();

    // Datas from step 2
    AnalysisTree analysis;
    PhotonTree photon;
    GenTree genPhoton;
    MuonTree muons;
    ElectronTree electrons;

    JetTree firstJet;
    JetTree firstRawJet;
    GenTree firstGenJet;

    JetTree secondJet;
    JetTree secondRawJet;
    GenTree secondGenJet;

    METTree MET;
    GenTree genMET;
    METTree rawMET;

    MiscTree misc;

    EtaBinning mEtaBinning;
    PtBinning mPtBinning;
    ExtrapBinning mExtrapBinning;

    std::vector<std::string> mInputFiles;
    std::string mDatasetName;
    JetType mJetType;
    JetAlgo mJetAlgo;
    bool mNoPUReweighting;
    bool mIsMC;
    bool mIsBatchJob;
    int mTotalJobs;
    int mCurrentJob;
    bool mUseExternalJECCorrecion;
    bool mUncutTrees;

    float  mSecondJetPtThreshold;
    bool   mDoMCComparison;

    edm::LumiReWeighting* mLumiReWeighter;
    float mPUWeight;

    float respMPF;

    // Balancing
    float respBalancing;
    float respBalancingGen;
    float respBalancingRaw;
    float respBalancingRawGen;

    // For DATA/MC comparison
    float respGenPhoton;
    float respGenGamma;
    float respPhotonGamma;
};
