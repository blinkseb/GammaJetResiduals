//
// Package:    GammaJetFilter
// Class:      GammaJetFilter
// 
/**\class GammaJetFilter GammaJetFilter.cc JetMETCorrections/GammaJetFilter/src/GammaJetFilter.cc

Description: [one line class summary]

Implementation:
[Notes on implementation]
*/
//
// Original Author:  Sébastien Brochet
//         Created:  Thu Mar 15 11:27:48 CET 2012
// $Id$
//
//


// system include files
#include <cmath>
#include <cstdio>
#include <fstream>
#include <map>
#include <unordered_map>
#include <memory>
#include <string>

// Boost
#include "boost/shared_ptr.hpp"

// user include files
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "CommonTools/Utils/interface/PtComparator.h"
#include "CommonTools/Utils/interface/StringCutObjectSelector.h"

#include "FWCore/Common/interface/TriggerNames.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDFilter.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/LuminosityBlock.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/Run.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/Exception.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "DataFormats/Candidate/interface/CandidateFwd.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/Math/interface/deltaPhi.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/Common/interface/Ref.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Photon.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"

#include "Geometry/CaloTopology/interface/CaloTopology.h"
#include "Geometry/Records/interface/CaloTopologyRecord.h"

#include "PhysicsTools/SelectorUtils/interface/JetIDSelectionFunctor.h"

#include "EGamma/EGammaAnalysisTools/interface/PFIsolationEstimator.h"

#include "RecoEcal/EgammaCoreTools/interface/EcalClusterTools.h"

#include "RecoEgamma/EgammaTools/interface/ConversionTools.h"

#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"

#include "JetMETCorrections/Objects/interface/JetCorrector.h"
#include "JetMETCorrections/GammaJetFilter/interface/json/json.h"

#include <TParameter.h>
#include <TTree.h>
#include <TClonesArray.h>
#include <TLorentzVector.h>

#include <boost/regex.hpp>

//
// class declaration
//

enum JetAlgorithm {
  AK5,
  AK7
};

struct JetInfos {
  JetAlgorithm algo;
  edm::InputTag inputTag;
};

#define FOREACH(x) for (std::vector<std::string>::const_iterator it = x.begin(); it != x.end(); ++it)

class GammaJetFilter : public edm::EDFilter {
  public:
    explicit GammaJetFilter(const edm::ParameterSet&);
    ~GammaJetFilter();

    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

  private:
    virtual void beginJob();
    virtual bool filter(edm::Event&, const edm::EventSetup&);
    virtual void endJob();

    virtual bool beginRun(edm::Run&, edm::EventSetup const&);
    virtual bool endRun(edm::Run&, edm::EventSetup const&);
    virtual bool beginLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&);
    virtual bool endLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&);

    void correctJets(pat::JetCollection& jets, edm::Event& iEvent, const edm::EventSetup& iSetup);
    void extractRawJets(pat::JetCollection& jets);
    void processJets(const pat::PhotonRef& photon, pat::JetCollection& jets, const JetAlgorithm algo, edm::Handle<edm::ValueMap<float>>& qgTagMLP, edm::Handle<edm::ValueMap<float>>& qgTagLikelihood, const edm::Handle<pat::JetCollection>& handleForRef, std::vector<TTree*>& trees);

    void correctMETWithTypeI(const pat::MET& rawMet, pat::MET& met, const pat::JetCollection& jets);

    //const EcalRecHitCollection* getEcalRecHitCollection(const reco::BasicCluster& cluster);
    bool isValidPhotonEB(const pat::Photon& photon, const double rho, const EcalRecHitCollection* recHits, const CaloTopology& topology);
    bool isValidPhotonEB2012(const pat::PhotonRef& photonRef, edm::Event& event);
    //bool isValidPhotonEE(const pat::Photon& photon, const double rho);
    //bool isValidPhotonEB(const pat::Photon& photon, const double rho);
    bool isValidJet(const pat::Jet& jet);

    void readJSONFile();
    void readCSVFile();
    void updateLuminosity(const edm::LuminosityBlock& lumiBlock);

    // ----------member data ---------------------------
    bool mIsMC;
    bool mFilterData;
    std::string mJSONFile;
    std::string mCSVFile;
    boost::shared_ptr<Json::Value> mValidRuns;
    boost::shared_ptr<Json::Value> mCurrentRunValidLumis;
    std::map<std::pair<unsigned int, unsigned int>, double> mLumiByLS;
    bool mIsValidLumiBlock;

    // Photon ID
    PFIsolationEstimator mPFIsolator;

    bool mRedoTypeI;
    bool mDoJEC;
    bool mJECFromRaw;
    std::string mCorrectorLabel;
    GreaterByPt<pat::Jet> mSorter;

    bool mFirstJetPtCut;
    double mFirstJetThreshold;

    std::vector<std::string> mJetCollections;
    std::map<std::string, JetInfos> mJetCollectionsData;

    // Input Tags
    edm::InputTag mPhotonsIT;
    edm::InputTag mJetsAK5PFlowIT;
    edm::InputTag mJetsAK7PFlowIT;
    edm::InputTag mJetsAK5CaloIT;
    edm::InputTag mJetsAK7CaloIT;

    boost::shared_ptr<JetIDSelectionFunctor> mCaloJetID;
    pat::strbitset mCaloJetIDRet;

    double mPtHatMin;
    double mPtHatMax;

    // Trees
    void createTrees(const std::string& rootName, TFileService& fs);
    TTree* mGenParticlesTree;
    TTree* mPhotonTree;
    TTree* mPhotonGenTree;
    TTree* mAnalysisTree;
    TTree* mElectronsTree;
    TTree* mMuonsTree;
    TParameter<double>*    mTotalLuminosity;
    float                  mEventsWeight;
    TParameter<long long>* mProcessedEvents;
    TParameter<long long>* mSelectedEvents;

    std::map<std::string, std::vector<TTree*> > mJetTrees;
    std::map<std::string, std::vector<TTree*> > mMETTrees;
    std::map<std::string, TTree*>               mMiscTrees;

    // TParameters for storing current config (JEC, correctorLabel, Treshold, etc...
    TParameter<bool>*             mJECRedone;
    TParameter<bool>*             mJECFromRawParameter;
    TNamed*                       mJECCorrectorLabel;
    TParameter<bool>*             mFirstJetPtCutParameter;
    TParameter<double>*           mFirstJetThresholdParameter;

    // DEBUG
    TH1F* mFirstJetPhotonDeltaPhi;
    TH1F* mFirstJetPhotonDeltaR;
    TH1F* mFirstJetPhotonDeltaPt;
    TH2F* mFirstJetPhotonDeltaPhiDeltaR;

    TH1F* mSelectedFirstJetIndex;
    TH1F* mSelectedSecondJetIndex;

    TH1F* mSecondJetPhotonDeltaPhi;
    TH1F* mSecondJetPhotonDeltaR;
    TH1F* mSecondJetPhotonDeltaPt;

    TH1F* mSelectedFirstJetPhotonDeltaPhi;
    TH1F* mSelectedFirstJetPhotonDeltaR;

    TH1F* mSelectedSecondJetPhotonDeltaPhi;
    TH1F* mSelectedSecondJetPhotonDeltaR;

    // For B / C jets neutrinos
    TClonesArray* mNeutrinos;
    TClonesArray* mNeutrinosPDG;

    // Cache for MC particles
    bool mDumpAllMCParticles;
    std::unordered_map<const reco::Candidate*, int> mParticlesIndexes;

    void particleToTree(const reco::Candidate* particle, TTree* t, std::vector<boost::shared_ptr<void> >& addresses);
    
    void updateBranch(TTree* tree, void* address, const std::string& name, const std::string& type = "F");
    template<typename U>
      void updateBranch(TTree* tree, std::vector<U>*& address, const std::string& name);

    void updateBranchArray(TTree* tree, void* address, const std::string& name, const std::string& size, const std::string& type = "F");

    void photonToTree(const pat::PhotonRef& photon, const edm::Event& event);
    void metsToTree(const pat::MET& met, const pat::MET& rawMet, const std::vector<TTree*>& trees);
    void metToTree(const pat::MET* met, TTree* tree, TTree* genTree);
    void jetsToTree(const pat::Jet* firstJet, const pat::Jet* secondJet, const std::vector<TTree*>& trees);
    void jetToTree(const pat::Jet* jet, bool findNeutrinos, TTree* tree, TTree* genTree);
    void electronsToTree(const edm::Handle<pat::ElectronCollection>& electrons, const reco::Vertex& pv);
    void muonsToTree(const edm::Handle<pat::MuonCollection>& muons, const reco::Vertex& pv);

    int getMotherIndex(const edm::Handle<reco::GenParticleCollection>& genParticles, const reco::Candidate* mother);
    void genParticlesToTree(const edm::Handle<reco::GenParticleCollection>& genParticles);
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
GammaJetFilter::GammaJetFilter(const edm::ParameterSet& iConfig):
  mIsMC(false), mIsValidLumiBlock(false)
{

  mIsMC = iConfig.getUntrackedParameter<bool>("isMC", "false");

  if (! mIsMC) {
    mJSONFile = iConfig.getParameter<std::string>("json");
    mCSVFile = iConfig.getParameter<std::string>("csv");
    mFilterData = iConfig.getUntrackedParameter<bool>("filterData", true);
  }

  mPhotonsIT = iConfig.getUntrackedParameter<edm::InputTag>("photons", edm::InputTag("selectedPatPhotons"));
  mJetsAK5PFlowIT = iConfig.getUntrackedParameter<edm::InputTag>("jetsAK5PFlow", edm::InputTag("selectedPatJetsPFlowAK5"));
  mJetsAK7PFlowIT = iConfig.getUntrackedParameter<edm::InputTag>("jetsAK7PFlow", edm::InputTag("selectedPatJetsPFlowAK7"));
  mJetsAK5CaloIT = iConfig.getUntrackedParameter<edm::InputTag>("jetsAK5Calo", edm::InputTag("selectedPatJets"));
  mJetsAK7CaloIT = iConfig.getUntrackedParameter<edm::InputTag>("jetsAK7Calo", edm::InputTag("selectedPatJetsCaloAK7"));
  mDoJEC         = iConfig.getUntrackedParameter<bool>("doJetCorrection", false);
  mRedoTypeI     = iConfig.getUntrackedParameter<bool>("redoTypeIMETCorrection", false);

  if (mDoJEC) {
    mJECFromRaw = iConfig.getUntrackedParameter<bool>("correctJecFromRaw", false);
    mCorrectorLabel = iConfig.getUntrackedParameter<std::string>("correctorLabel", "ak5PFResidual");
  }

  mFirstJetPtCut = iConfig.getUntrackedParameter<bool>("firstJetPtCut", true);
  mFirstJetThreshold = iConfig.getUntrackedParameter<double>("firstJetThreshold", 0.3);

  bool runOnCHS    = iConfig.getUntrackedParameter<bool>("runOnCHS", true);
  bool runOnNonCHS = iConfig.getUntrackedParameter<bool>("runOnNonCHS", true);

  bool runOnPFAK5    = iConfig.getUntrackedParameter<bool>("runOnPFAK5", true);
  bool runOnPFAK7    = iConfig.getUntrackedParameter<bool>("runOnPFAK7", true);
  bool runOnCaloAK5  = iConfig.getUntrackedParameter<bool>("runOnCaloAK5", true);
  bool runOnCaloAK7  = iConfig.getUntrackedParameter<bool>("runOnCaloAK7", true);

  edm::InputTag jetsAK5PFlowITchs = iConfig.getUntrackedParameter<edm::InputTag>("jetsAK5PFlowchs", edm::InputTag("selectedPatJetsPFlowAK5chs"));
  edm::InputTag jetsAK7PFlowITchs = iConfig.getUntrackedParameter<edm::InputTag>("jetsAK7PFlowchs", edm::InputTag("selectedPatJetsPFlowAK7chs"));

  if (! mIsMC && mFilterData) {
    // Load JSON file of good runs
    readJSONFile();
    readCSVFile();
  }

  edm::Service<TFileService> fs;
  mPhotonTree = fs->make<TTree>("photon", "photon tree");
  
  if (mIsMC)
    mPhotonGenTree = fs->make<TTree>("photon_gen", "photon gen tree");
  else
    mPhotonGenTree = nullptr;

  mAnalysisTree = fs->make<TTree>("analysis", "analysis tree");
  mMuonsTree = fs->make<TTree>("muons", "muons tree");
  mElectronsTree = fs->make<TTree>("electrons", "electrons tree");

  mTotalLuminosity = fs->make<TParameter<double> >("total_luminosity", 0.);

  mEventsWeight = 1.;
  mPtHatMin     = -1.;
  mPtHatMax     = -1.;

  if (mIsMC) {
    // Read cross section and number of generated events
    double crossSection = iConfig.getParameter<double>("crossSection");
    unsigned long long generatedEvents = iConfig.getParameter<unsigned long long>("generatedEvents");
    mEventsWeight = crossSection / (float) generatedEvents;

    mPtHatMin = iConfig.getUntrackedParameter<double>("ptHatMin", -1.);
    mPtHatMax = iConfig.getUntrackedParameter<double>("ptHatMax", -1.);
  }

  if (runOnNonCHS) {
    if (runOnPFAK5) {
      mJetCollections.push_back("PFlowAK5");
      mJetCollectionsData["PFlowAK5"] = {AK5, mJetsAK5PFlowIT};
    }
    if (runOnPFAK7) {
      mJetCollections.push_back("PFlowAK7");
      mJetCollectionsData["PFlowAK7"] = {AK7, mJetsAK7PFlowIT};
    }
  }

  if (runOnCHS) {
    if (runOnPFAK5) {
      mJetCollections.push_back("PFlowAK5chs");
      mJetCollectionsData["PFlowAK5chs"] = {AK5, jetsAK5PFlowITchs};
    }
    if (runOnPFAK7) {
      mJetCollections.push_back("PFlowAK7chs");
      mJetCollectionsData["PFlowAK7chs"] = {AK7, jetsAK7PFlowITchs};
    }
  }

  if (runOnCaloAK5) {
    mJetCollections.push_back("CaloAK5");
    mJetCollectionsData["CaloAK5"]  = {AK5, mJetsAK5CaloIT};
  }

  if (runOnCaloAK7) {
    mJetCollections.push_back("CaloAK7");
    mJetCollectionsData["CaloAK7"]  = {AK7, mJetsAK7CaloIT};
  }

  FOREACH(mJetCollections) {
    createTrees(*it, *fs);
  }

  mProcessedEvents = fs->make<TParameter<long long> >("total_events", 0);
  mSelectedEvents = fs->make<TParameter<long long> >("passed_events", 0);

  mJECRedone = fs->make<TParameter<bool> >("jec_redone", mDoJEC, '*');
  mFirstJetPtCutParameter = fs->make<TParameter<bool> >("cut_on_first_jet_pt", mFirstJetPtCut, '*');
  if (mDoJEC) {
    mJECFromRawParameter = fs->make<TParameter<bool> >("jec_from_raw_jet", mJECFromRaw, '*');
    mJECCorrectorLabel = fs->make<TNamed>("jec_corrector_label", mCorrectorLabel);
  }

  if (mFirstJetPtCut) {
    mFirstJetThresholdParameter = fs->make<TParameter<double> >("cut_on_first_jet_treshold", mFirstJetThreshold);
  }

  mFirstJetPhotonDeltaPhi = fs->make<TH1F>("firstJetPhotonDeltaPhi", "firstJetPhotonDeltaPhi", 50, 0., M_PI);
  mFirstJetPhotonDeltaR = fs->make<TH1F>("firstJetPhotonDeltaR", "firstJetPhotonDeltaR", 80, 0, 10);
  mFirstJetPhotonDeltaPt = fs->make<TH1F>("firstJetPhotonDeltaPt", "firstJetPhotonDeltaPt", 100, 0, 50);
  mFirstJetPhotonDeltaPhiDeltaR = fs->make<TH2F>("firstJetPhotonDeltaPhiDeltaR", "firstJetPhotonDeltaPhiDeltaR", 50, 0, M_PI, 80, 0, 10);

  mSelectedFirstJetIndex = fs->make<TH1F>("selectedFirstJetIndex", "selectedFirstJetIndex", 20, 0, 20);
  mSelectedSecondJetIndex = fs->make<TH1F>("selectedSecondJetIndex", "selectedSecondJetIndex", 20, 0, 20);

  mSecondJetPhotonDeltaPhi = fs->make<TH1F>("secondJetPhotonDeltaPhi", "secondJetPhotonDeltaPhi", 50, 0., M_PI);
  mSecondJetPhotonDeltaR = fs->make<TH1F>("secondJetPhotonDeltaR", "secondJetPhotonDeltaR", 80, 0, 10);
  mSecondJetPhotonDeltaPt = fs->make<TH1F>("secondJetPhotonDeltaPt", "secondJetPhotonDeltaPt", 100, 0, 50);

  mSelectedFirstJetPhotonDeltaPhi = fs->make<TH1F>("selectedFirstJetPhotonDeltaPhi", "selectedFirstJetPhotonDeltaPhi", 50, 0., M_PI);
  mSelectedFirstJetPhotonDeltaR = fs->make<TH1F>("selectedFirstJetPhotonDeltaR", "selectedFirstJetPhotonDeltaR", 80, 0, 10);

  mSelectedSecondJetPhotonDeltaPhi = fs->make<TH1F>("selectedSecondJetPhotonDeltaPhi", "selectedSecondJetPhotonDeltaPhi", 50, 0., M_PI);
  mSelectedSecondJetPhotonDeltaR = fs->make<TH1F>("selectedSecondJetPhotonDeltaR", "selectedSecondJetPhotonDeltaR", 80, 0, 10);

  mPFIsolator.initializePhotonIsolation(true);
  mPFIsolator.setConeSize(0.3);

  mNeutrinos = NULL;
  mNeutrinosPDG = NULL;
  if (mIsMC) {
    mNeutrinos = new TClonesArray("TLorentzVector", 3);
    mNeutrinosPDG = new TClonesArray("TParameter<int>", 3);
  }
}


GammaJetFilter::~GammaJetFilter()
{

  // do anything here that needs to be done at desctruction time
  // (e.g. close files, deallocate resources etc.)

  delete mNeutrinos;
  delete mNeutrinosPDG;
}

void GammaJetFilter::createTrees(const std::string& rootName, TFileService& fs) {

  TFileDirectory dir = fs.mkdir(rootName);
  std::vector<TTree*>& trees = mJetTrees[rootName];

  trees.push_back(dir.make<TTree>("first_jet", "first jet tree"));
  trees.push_back(dir.make<TTree>("second_jet", "second jet tree"));

  trees.push_back(dir.make<TTree>("first_jet_raw", "first raw jet tree"));
  trees.push_back(dir.make<TTree>("second_jet_raw", "second raw jet tree"));

  if (mIsMC) {
    trees.push_back(dir.make<TTree>("first_jet_gen", "first gen jet tree"));
    trees.push_back(dir.make<TTree>("second_jet_gen", "second gen jet tree"));
  } else {
    trees.push_back(nullptr);
    trees.push_back(nullptr);
  }

  // MET
  std::vector<TTree*>& met = mMETTrees[rootName];
  met.push_back(dir.make<TTree>("met", "met tree"));
  met.push_back(dir.make<TTree>("met_raw", "met raw tree"));

  if (mIsMC)
    met.push_back(dir.make<TTree>("met_gen", "met gen tree"));
  else
    met.push_back(nullptr);

  // Misc
  mMiscTrees[rootName] = dir.make<TTree>("misc", "misc tree");
}

void GammaJetFilter::updateBranch(TTree* tree, void* address, const std::string& name, const std::string& type/* = "F"*/) {
  TBranch* branch = tree->GetBranch(name.c_str());
  if (branch == NULL) {
    branch = tree->Branch(name.c_str(), address, std::string(name + "/" + type).c_str()); 
  } else {
    branch->SetAddress(address);
  }
}

template<typename U> void GammaJetFilter::updateBranch(TTree* tree, std::vector<U>*& address, const std::string& name) {
  TBranch* branch = tree->GetBranch(name.c_str());
  if (branch == NULL) {
    branch = tree->Branch(name.c_str(), &address); 
  } else {
    branch->SetAddress(&address);
  }
}

void GammaJetFilter::updateBranchArray(TTree* tree, void* address, const std::string& name, const std::string& size, const std::string& type/* = "F"*/) {
  TBranch* branch = tree->GetBranch(name.c_str());
  if (branch == NULL) {
    branch = tree->Branch(name.c_str(), address, std::string(name + "[" + size + "]/" +type).c_str()); 
  } else {
    branch->SetAddress(address);
  }
}

//
// member functions
//

// ------------ method called on each new Event  ------------
bool GammaJetFilter::filter(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace edm;

  mProcessedEvents->SetVal(mProcessedEvents->GetVal() + 1);

  if (! mIsMC && mFilterData && ! mIsValidLumiBlock) {
    return false;
  }

  // Vertex
  edm::Handle<reco::VertexCollection> vertices;
  iEvent.getByLabel("goodOfflinePrimaryVertices", vertices);

  // Keep events with at least one vertex
  if (!vertices.isValid() || vertices->size() == 0 || vertices->front().isFake())
    return false;

  const reco::Vertex& primaryVertex = vertices->at(0);

  double generatorWeight = 1.;

  if (mIsMC) {
    edm::Handle<GenEventInfoProduct> eventInfos;
    iEvent.getByLabel("generator", eventInfos);

    if (eventInfos.isValid() && eventInfos->hasBinningValues()) {
      double genPt = eventInfos->binningValues()[0];

      if (mPtHatMin >= 0. && genPt < mPtHatMin)
        return false;

      if (mPtHatMax >= 0. && genPt > mPtHatMax)
        return false;
    }

    generatorWeight = eventInfos->weight();
    if (generatorWeight == 0.) {
      generatorWeight = 1.;
    }
  }

  edm::Handle<double> pFlowRho;
  iEvent.getByLabel(edm::InputTag("kt6PFJets", "rho"), pFlowRho); // For photon ID

  // Necesseray collection for calculate sigmaIPhiIPhi
  // 2011 Photon ID
  /*
  edm::Handle<EcalRecHitCollection> recHits;
  iEvent.getByLabel(edm::InputTag("reducedEcalRecHitsEB"), recHits);
  const EcalRecHitCollection* pRecHits = (recHits.isValid()) ? recHits.product() : NULL;
  */

  edm::ESHandle<CaloTopology> topology;
  iSetup.get<CaloTopologyRecord>().get(topology);

  edm::Handle<pat::PhotonCollection> photons;
  iEvent.getByLabel(mPhotonsIT, photons);

  pat::PhotonRefVector photonsRef;

  pat::PhotonCollection::const_iterator it = photons->begin();
  uint32_t index = 0;
  for (; it != photons->end(); ++it, index++) {
    //if (fabs(it->eta()) <= 1.3 && isValidPhotonEB(*it, *pFlowRho, pRecHits, *topology)) {

    pat::PhotonRef photon(photons, index);
    if (fabs(it->eta()) <= 1.3 && isValidPhotonEB2012(photon, iEvent)) {
      photonsRef.push_back(photon);
    }
  }

  // Only one good photon per event
  if (photonsRef.size() != 1)
    return false;

  const pat::PhotonRef& photon = photonsRef[0];

  // Process jets

  edm::Handle<pat::JetCollection> jetsHandle;

  FOREACH(mJetCollections) {

    JetInfos infos = mJetCollectionsData[*it];

    iEvent.getByLabel(infos.inputTag, jetsHandle);
    pat::JetCollection jets = *jetsHandle;
    if (mDoJEC) {
      correctJets(jets, iEvent, iSetup);
    } else {
      extractRawJets(jets);
    }

    edm::Handle<edm::ValueMap<float>>  qgTagHandleMLP;
    edm::Handle<edm::ValueMap<float>>  qgTagHandleLikelihood;
    iEvent.getByLabel("QGTagger" + *it,"qgMLP", qgTagHandleMLP);
    iEvent.getByLabel("QGTagger" + *it,"qgLikelihood", qgTagHandleLikelihood);


    processJets(photon, jets, infos.algo, qgTagHandleMLP, qgTagHandleLikelihood, jetsHandle, mJetTrees[*it]);

    // MET
    edm::Handle<pat::METCollection> metsHandle;
    iEvent.getByLabel(std::string("patMETs" + ((*it == "AK5Calo") ? "" : *it)), metsHandle);

    edm::Handle<pat::METCollection> rawMets;
    iEvent.getByLabel(std::string("patPFMet" + ((*it == "AK5Calo") ? "" : *it)), rawMets);

    pat::METCollection mets = *metsHandle;
    pat::MET& met = mets[0];
    const pat::MET& rawMet = rawMets->at(0);

    if (mDoJEC || mRedoTypeI) {
      correctMETWithTypeI(rawMet, met, jets);
    }

    if (rawMets.isValid())
      metsToTree(met, rawMet, mMETTrees[*it]);
    else {
      pat::MET emptyRawMet = pat::MET();
      metsToTree(met, emptyRawMet, mMETTrees[*it]);
    }

    // Rho
    edm::Handle<double> rhos;
    if (it->find("Calo") != std::string::npos)
      iEvent.getByLabel(edm::InputTag("kt6CaloJets", "rho"), rhos);
    else
      iEvent.getByLabel(edm::InputTag("kt6PFJets", "rho"), rhos);

    double rho = *rhos;
    updateBranch(mMiscTrees[*it], &rho, "rho", "D");

    mMiscTrees[*it]->Fill();
  }

  // Number of vertices for pu reweighting
  edm::Handle<std::vector<PileupSummaryInfo> > puInfos;
  iEvent.getByLabel(edm::InputTag("addPileupInfo"), puInfos);

  float nTrueInteractions = -1;
  int nPUVertex = -1;
  unsigned int nVertex = vertices->size();

  edm::EventID eventId = iEvent.id();
  EventNumber_t event = eventId.event();
  RunNumber_t run = eventId.run();
  LuminosityBlockNumber_t lumiBlock = eventId.luminosityBlock();

  if (mIsMC) {
    for (std::vector<PileupSummaryInfo>::const_iterator it = puInfos->begin(); it != puInfos->end();
        ++it) {

      int BX = it->getBunchCrossing();
      if (BX == 0) {
        nPUVertex = it->getPU_NumInteractions();
        nTrueInteractions = it->getTrueNumInteractions();
        break;
      }
    }

    if (nPUVertex < 0) {
      throw cms::Exception("PUReweighting") << "No in-time beam crossing found!" << std::endl;
    }
  }
  updateBranch(mAnalysisTree, &run, "run", "i");
  updateBranch(mAnalysisTree, &lumiBlock, "lumi_block", "i");
  updateBranch(mAnalysisTree, &event, "event", "i");
  updateBranch(mAnalysisTree, &nVertex, "nvertex", "i");
  updateBranch(mAnalysisTree, &nTrueInteractions, "ntrue_interactions");
  updateBranch(mAnalysisTree, &nPUVertex, "pu_nvertex", "I");
  updateBranch(mAnalysisTree, &mEventsWeight, "event_weight"); // Only valid for binned samples
  updateBranch(mAnalysisTree, &generatorWeight, "generator_weight", "D"); // Only valid for flat samples

  // Triggers
  edm::Handle<edm::TriggerResults> triggerResults;
  iEvent.getByLabel(edm::InputTag("TriggerResults", "", "HLT"), triggerResults);

  std::vector<std::string>* trigNames = new std::vector<std::string>();
  std::vector<bool>* trigResults = new std::vector<bool>();

  if (triggerResults.isValid()) {
    static std::vector<boost::regex> validTriggers = { boost::regex("HLT_.*Photon.*", boost::regex_constants::icase) };
    const edm::TriggerNames& triggerNames = iEvent.triggerNames(*triggerResults);

    size_t size = triggerResults->size();

    for (size_t i = 0; i < size; i++) {
      std::string triggerName = triggerNames.triggerName(i);
      bool isValid = false;
      for (boost::regex& validTrigger: validTriggers) {
        if (boost::regex_match(triggerName, validTrigger)) {
          isValid = true;
          break;
        }
      }

      if (!isValid)
        continue;

      unsigned int index = triggerNames.triggerIndex(triggerName);
      bool passed = triggerResults->accept(index);

      trigResults->push_back(passed);
      trigNames->push_back(triggerName);
    }
  }

  // Create branches, even if they're empty
  updateBranch(mAnalysisTree, trigNames, "trigger_names");
  updateBranch(mAnalysisTree, trigResults, "trigger_results");

  mAnalysisTree->Fill();

  delete trigNames;
  delete trigResults;

  photonToTree(photon, iEvent);

  // Electrons
  edm::Handle<pat::ElectronCollection> electrons;
  iEvent.getByLabel("selectedPatElectronsPFlowAK5chs", electrons);
  electronsToTree(electrons, primaryVertex);

  // Muons
  edm::Handle<pat::MuonCollection> muons;
  iEvent.getByLabel("selectedPatMuonsPFlowAK5chs", muons);
  muonsToTree(muons, primaryVertex);

  mSelectedEvents->SetVal(mSelectedEvents->GetVal() + 1);
  return true;
}

void GammaJetFilter::correctJets(pat::JetCollection& jets, edm::Event& iEvent, const edm::EventSetup& iSetup) {

  // Get Jet corrector
  const JetCorrector* corrector = JetCorrector::getJetCorrector(mCorrectorLabel, iSetup);

  // Correct jets
  for (pat::JetCollection::iterator it = jets.begin(); it != jets.end(); ++it)  {
    pat::Jet& jet = *it;

    // Store raw jet, it's not possible to get it after corrections
    pat::Jet rawJet = jet.correctedJet("Uncorrected");
    jet.addUserData("rawJet", rawJet, true); // Store raw jet inside our jet. This allow us to correctly sort the resulting collection
    pat::Jet L1Jet  = jet.correctedJet("L1FastJet");
    jet.addUserData("L1Jet", L1Jet, true); // Embed L1 corrected jet for TypeI correction

    if (mJECFromRaw) {
      double toRaw = jet.jecFactor("Uncorrected");
      jet.setP4(jet.p4() * toRaw); // It's now a raw jet
    }

    double corrections = corrector->correction(jet, iEvent, iSetup);
    jet.scaleEnergy(corrections);
  }

  // Sort collection by pt
  std::sort(jets.begin(), jets.end(), mSorter);
}

void GammaJetFilter::correctMETWithTypeI(const pat::MET& rawMet, pat::MET& met, const pat::JetCollection& jets) {
  double deltaPx = 0., deltaPy = 0.;
  //static StringCutObjectSelector<reco::Muon> skipMuonSelection("isGlobalMuon | isStandAloneMuon");

  // See https://indico.cern.ch/getFile.py/access?contribId=1&resId=0&materialId=slides&confId=174324 slide 4
  // and http://cmssw.cvs.cern.ch/cgi-bin/cmssw.cgi/CMSSW/JetMETCorrections/Type1MET/interface/PFJetMETcorrInputProducerT.h?revision=1.8&view=markup
  for (pat::JetCollection::const_iterator it = jets.begin(); it != jets.end(); ++it) {
    const pat::Jet& jet = *it;

    if (jet.pt() > 10) {

      const pat::Jet* rawJet = jet.userData<pat::Jet>("rawJet");
      const pat::Jet* L1Jet  = jet.userData<pat::Jet>("L1Jet");

      double emEnergyFraction = rawJet->chargedEmEnergyFraction() + rawJet->neutralEmEnergyFraction();
      if (emEnergyFraction > 0.90)
        continue;

      //reco::Candidate::LorentzVector rawJetP4 = rawJet->p4();
      reco::Candidate::LorentzVector L1JetP4  = L1Jet->p4();

      // Skip muons
      /*std::vector<reco::PFCandidatePtr> cands = rawJet->getPFConstituents();
      for (std::vector<reco::PFCandidatePtr>::const_iterator cand = cands.begin(); cand != cands.end(); ++cand) {
        if ((*cand)->muonRef().isNonnull() && skipMuonSelection(*(*cand)->muonRef())) {
          reco::Candidate::LorentzVector muonP4 = (*cand)->p4();
          rawJetP4 -= muonP4;
        }
      }*/


      deltaPx += (jet.px() - L1JetP4.px());
      deltaPy += (jet.py() - L1JetP4.py());
    }
  }

  double correctedMetPx = rawMet.px() - deltaPx;
  double correctedMetPy = rawMet.py() - deltaPy;
  double correctedMetPt = sqrt(correctedMetPx * correctedMetPx + correctedMetPy * correctedMetPy);

  met.setP4(reco::Candidate::LorentzVector(correctedMetPx, correctedMetPy, 0., correctedMetPt));
}

void GammaJetFilter::extractRawJets(pat::JetCollection& jets) {

  for (pat::JetCollection::iterator it = jets.begin(); it != jets.end(); ++it) {
    pat::Jet& jet = *it;

    const pat::Jet rawJet = jet.correctedJet("Uncorrected");
    jet.addUserData("rawJet", rawJet, true);
    const pat::Jet L1Jet  = jet.correctedJet("L1FastJet");
    jet.addUserData("L1Jet", L1Jet, true); // Embed L1 corrected jet for TypeI correction
  }

}

void GammaJetFilter::processJets(const pat::PhotonRef& photon, pat::JetCollection& jets, const JetAlgorithm algo, edm::Handle<edm::ValueMap<float>>& qgTagMLP, edm::Handle<edm::ValueMap<float>>& qgTagLikelihood, const edm::Handle<pat::JetCollection>& handleForRef, std::vector<TTree*>& trees) {

  pat::JetCollection selectedJets;

  pat::JetCollection::iterator it = jets.begin();
  uint32_t index = 0;
  uint32_t goodJetIndex = -1;
  for (; it != jets.end(); ++it, index++) {

    if (! isValidJet(*it))
      continue;

    goodJetIndex++;

    if (goodJetIndex == 0) {
      mFirstJetPhotonDeltaPhi->Fill(fabs(reco::deltaPhi(*photon, *it)));
      mFirstJetPhotonDeltaR->Fill(reco::deltaR(*photon, *it));
      mFirstJetPhotonDeltaPt->Fill(fabs(photon->pt() - it->pt()));

      mFirstJetPhotonDeltaPhiDeltaR->Fill(fabs(reco::deltaPhi(*photon, *it)), reco::deltaR(*photon, *it));
    } else if (goodJetIndex == 1) {
      mSecondJetPhotonDeltaPhi->Fill(fabs(reco::deltaPhi(*photon, *it)));
      mSecondJetPhotonDeltaR->Fill(reco::deltaR(*photon, *it));
      mSecondJetPhotonDeltaPt->Fill(fabs(photon->pt() - it->pt()));
    }

    // Extract Quark Gluon tagger value
    pat::JetRef jetRef(handleForRef, index);
    it->addUserFloat("qgTagMLP", (*qgTagMLP)[jetRef]);
    it->addUserFloat("qgTagLikelihood", (*qgTagLikelihood)[jetRef]);

    const double deltaR_threshold = (algo == AK5) ? 0.5 : 0.7;

    if (selectedJets.size() == 0) {
      // First jet selection

      if (index > 1) {
        // It's the third jet of the event. We only want to consider the first two jets for our leadind jet,
        // so, throw this event
        break;
      }

      const double deltaPhi = reco::deltaPhi(*photon, *it);
      if (fabs(deltaPhi) < M_PI / 2.)
        continue; // Only back 2 back event are interesting

      const double deltaR = reco::deltaR(*photon, *it);
      if (deltaR < deltaR_threshold) // This jet is inside the photon. This is probably the photon mis-reconstructed as a jet
        continue;

      // Jet are ordered by pt value.
      // Events are supposed to be balanced between Jet and Gamma
      // If the leading jet has less than 30% of the Photon pt,
      // dump the event as it's not interesting
      if (mFirstJetPtCut && (it->pt() < photon->pt() * mFirstJetThreshold))
        break;

      mSelectedFirstJetIndex->Fill(goodJetIndex);
      selectedJets.push_back(*it);

    } else {

      // Second jet selection
      const double deltaR = reco::deltaR(*photon, *it);

      if (deltaR > deltaR_threshold) {
        mSelectedSecondJetIndex->Fill(goodJetIndex);
        selectedJets.push_back(*it);
      } else {
        continue;
      }

      break;
    }

  }

  const pat::Jet* firstJet = NULL;
  const pat::Jet* secondJet = NULL;

  if (selectedJets.size() > 0) {

    firstJet = &selectedJets[0];
    mSelectedFirstJetPhotonDeltaPhi->Fill(fabs(reco::deltaPhi(*photon, *firstJet)));
    mSelectedFirstJetPhotonDeltaR->Fill(reco::deltaR(*photon, *firstJet));

    if (selectedJets.size() > 1) {
      secondJet = &selectedJets[1];

      mSelectedSecondJetPhotonDeltaPhi->Fill(fabs(reco::deltaPhi(*photon, *secondJet)));
      mSelectedSecondJetPhotonDeltaR->Fill(reco::deltaR(*photon, *secondJet));
    }
  }

  jetsToTree(firstJet, secondJet, trees);

  return;
}

// ------------ method called once each job just before starting event loop  ------------
void GammaJetFilter::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void GammaJetFilter::endJob() {
}

// ------------ method called when starting to processes a run  ------------
bool GammaJetFilter::beginRun(edm::Run& run, edm::EventSetup const&)
{
  if (! mIsMC && mFilterData) {
    // Check if this run is valid
    std::stringstream stream;
    stream << run.run();

    if (! mValidRuns->isMember(stream.str()))
      return false; // Drop run

    mCurrentRunValidLumis.reset(new Json::Value((*mValidRuns)[stream.str()]));
  }

  return true;
}

// ------------ method called when ending the processing of a run  ------------
bool GammaJetFilter::endRun(edm::Run& run, edm::EventSetup const&)
{
  return true;
}

// ------------ method called when starting to processes a luminosity block  ------------
bool GammaJetFilter::beginLuminosityBlock(edm::LuminosityBlock& lumiBlock, edm::EventSetup const&)
{
  if (! mIsMC && mFilterData) {

    mIsValidLumiBlock = false;

    if (! mCurrentRunValidLumis.get())
      return false;

    // Check if this lumi block is valid
    assert(mCurrentRunValidLumis->isArray());
    for (Json::ArrayIndex i = 0; i < mCurrentRunValidLumis->size(); i++) {
      Json::Value lumiRange = (*mCurrentRunValidLumis)[i];

      assert(lumiRange.size() == 2);
      edm::LuminosityBlockNumber_t lumi = lumiBlock.luminosityBlock();
      if (lumi >= lumiRange[0].asUInt64() && lumi <= lumiRange[1].asUInt64()) {
        mIsValidLumiBlock = true;
        return true;
      }
    }

    return false;
  }

  return true;
}

// ------------ method called when ending the processing of a luminosity block  ------------
bool GammaJetFilter::endLuminosityBlock(edm::LuminosityBlock& lumiBlock, edm::EventSetup const&)
{
  if (! mIsMC && mFilterData && mIsValidLumiBlock) {
    updateLuminosity(lumiBlock);
  }
  return true;
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void GammaJetFilter::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

bool GammaJetFilter::isValidJet(const pat::Jet& jet) {
  // First, check if this pat::Jet has a gen jet
  if (mIsMC && !jet.genJet()) {
    return false;
  }

  if (jet.isPFJet()) {

    // Jet ID
    // From https://twiki.cern.ch/twiki/bin/view/CMS/JetID

    // Jet ID works on uncorrected jets. *EnergyFraction take that into account when calculating the fraction,
    // so there's *NO* need to use an uncorrected jet
    bool isValid = jet.neutralHadronEnergyFraction() < 0.99;
    isValid &= jet.neutralEmEnergyFraction() < 0.99;
    //isValid &= jet.getPFConstituents().size() > 1;
    if (fabs(jet.eta()) < 2.4) {
      isValid &= jet.chargedHadronEnergyFraction() > 0.;
      isValid &= jet.chargedMultiplicity() > 0;
      isValid &= jet.chargedEmEnergyFraction() < 0.99;
    }

    return isValid;

  } else if (jet.isCaloJet() || jet.isJPTJet()) {

    if (! mCaloJetID.get()) {
      mCaloJetID.reset(new JetIDSelectionFunctor(JetIDSelectionFunctor::PURE09, JetIDSelectionFunctor::LOOSE));
      mCaloJetIDRet = mCaloJetID->getBitTemplate();
    }

    mCaloJetIDRet.set(false);
    return (*mCaloJetID)(jet, mCaloJetIDRet);

  } else {
    throw cms::Exception("UnsupportedJetType")
      << "Only PF and Calo jets are supported at this time" << std::endl;
  }

  return false;
}

enum class IsolationType {
  CHARGED_HADRONS,
  NEUTRAL_HADRONS,
  PHOTONS
};

float getEffectiveArea(float eta, IsolationType type) {
  eta = fabs(eta);
  switch (type) {
    case IsolationType::CHARGED_HADRONS:
      if (eta < 1.0)
        return 0.012;
      else if (eta < 1.479)
        return 0.010;
      else if (eta < 2.0)
        return 0.014;
      else if (eta < 2.2)
        return 0.012;
      else if (eta < 2.3)
        return 0.016;
      else if (eta < 2.4)
        return 0.020;
      else
        return 0.012;
      break;

    case IsolationType::NEUTRAL_HADRONS:
      if (eta < 1.0)
        return 0.030;
      else if (eta < 1.479)
        return 0.057;
      else if (eta < 2.0)
        return 0.039;
      else if (eta < 2.2)
        return 0.015;
      else if (eta < 2.3)
        return 0.024;
      else if (eta < 2.4)
        return 0.039;
      else
        return 0.072;
      break;

    case IsolationType::PHOTONS:
      if (eta < 1.0)
        return 0.148;
      else if (eta < 1.479)
        return 0.130;
      else if (eta < 2.0)
        return 0.112;
      else if (eta < 2.2)
        return 0.216;
      else if (eta < 2.3)
        return 0.262;
      else if (eta < 2.4)
        return 0.260;
      else
        return 0.266;
      break;
  }

  return -1;
}

double getCorrectedPFIsolation(double isolation, double rho, float eta, IsolationType type) {
  float effectiveArea = getEffectiveArea(eta, type);

  return std::max(isolation - rho * effectiveArea, 0.);
}

// See https://twiki.cern.ch/twiki/bin/viewauth/CMS/CutBasedPhotonID2012
bool GammaJetFilter::isValidPhotonEB2012(const pat::PhotonRef& photonRef, edm::Event& event) {
  if (mIsMC && !photonRef->genPhoton())
    return false;

  bool isValid = true;

  isValid &= photonRef->hadTowOverEm() < 0.05;
  isValid &= photonRef->sigmaIetaIeta() < 0.011;

  if (! isValid)
    return false;

  edm::Handle<double> rhos;
  event.getByLabel(edm::InputTag("kt6PFJets", "rho", "RECO"), rhos);
  double rho = *rhos;

  // Isolations are produced at PAT level by the PḧotonPFIsolation producer
  edm::Handle<edm::ValueMap<bool>> hasMatchedPromptElectronHandle;
  event.getByLabel(edm::InputTag("photonPFIsolation", "hasMatchedPromptElectron", "PAT"), hasMatchedPromptElectronHandle);

  isValid &= ! (*hasMatchedPromptElectronHandle)[photonRef];

  if (! isValid)
    return false;

  // Now, isolations
  edm::Handle<edm::ValueMap<double>> chargedHadronsIsolationHandle;
  event.getByLabel(edm::InputTag("photonPFIsolation", "chargedHadronsIsolation", "PAT"), chargedHadronsIsolationHandle);

  edm::Handle<edm::ValueMap<double>> neutralHadronsIsolationHandle;
  event.getByLabel(edm::InputTag("photonPFIsolation", "neutralHadronsIsolation", "PAT"), neutralHadronsIsolationHandle);

  edm::Handle<edm::ValueMap<double>> photonIsolationHandle;
  event.getByLabel(edm::InputTag("photonPFIsolation", "photonIsolation", "PAT"), photonIsolationHandle);

  isValid &= getCorrectedPFIsolation((*chargedHadronsIsolationHandle)[photonRef], rho, photonRef->eta(), IsolationType::CHARGED_HADRONS) < 0.7;
  isValid &= getCorrectedPFIsolation((*neutralHadronsIsolationHandle)[photonRef], rho, photonRef->eta(), IsolationType::NEUTRAL_HADRONS) < (0.4 + 0.04 * photonRef->pt());
  isValid &= getCorrectedPFIsolation((*photonIsolationHandle)[photonRef], rho, photonRef->eta(), IsolationType::PHOTONS) < (0.5 + 0.005 * photonRef->pt());

  return isValid;
}
bool GammaJetFilter::isValidPhotonEB(const pat::Photon& photon, const double rho, const EcalRecHitCollection* recHits, const CaloTopology& topology) {
  if (mIsMC && !photon.genPhoton())
    return false;

  bool isValid = ! photon.hasPixelSeed();
  isValid &= photon.hadronicOverEm() < 0.05;
  isValid &= photon.sigmaIetaIeta() < 0.011;
  isValid &= photon.trkSumPtHollowConeDR04() < (2.0 + 0.001 * photon.et() + 0.0167 * rho);
  isValid &= photon.ecalRecHitSumEtConeDR04() < (4.2 + 0.006 * photon.et() + 0.183 * rho);
  isValid &= photon.hcalTowerSumEtConeDR04() < (2.2 + 0.0025 * photon.et() + 0.062 * rho);

  if (isValid) {
    // Spike cleaning

    isValid &= photon.sigmaIetaIeta() > 0.001;

    do {
      // Get sigmaIPhiIPhi for photon
      const float w0 = 4.7; // See http://cmslxr.fnal.gov/lxr/source/RecoEcal/EgammaCoreTools/interface/EcalClusterLazyTools.h#087
      if (! recHits)
        break;

      reco::SuperClusterRef superCluster = photon.superCluster();
      if (superCluster.isNull())
        break;

      reco::CaloClusterPtr caloCluster = superCluster->seed();
      if (caloCluster.isNull())
        break;

      std::vector<float> covariances = EcalClusterTools::localCovariances(*caloCluster, recHits, &topology, w0);
      float sigmaIPhiIPhi = sqrt(covariances[2]);

      isValid &= sigmaIPhiIPhi > 0.001;
    } while (false);
  }

  return isValid;
}

/*bool GammaJetFilter::isValidPhotonEE(const pat::Photon& photon, const double rho) {
  if (mIsMC && !photon.genPhoton())
  return false;

  bool isValid = ! photon.hasPixelSeed();
  isValid &= photon.hadronicOverEm() < 0.05;
  isValid &= photon.sigmaIetaIeta() < 0.011;
  isValid &= photon.trkSumPtHollowConeDR04() < (2.0 + 0.001 * photon.et() + 0.032 * rho);
  isValid &= photon.ecalRecHitSumEtConeDR04() < (4.2 + 0.006 * photon.et() + 0.090 * rho);
  isValid &= photon.hcalTowerSumEtConeDR04() < (2.2 + 0.0025 * photon.et() + 0.180 * rho);

  return isValid;
  }*/

void GammaJetFilter::readJSONFile() {
  Json::Value root;
  Json::Reader reader;
  std::ifstream file(mJSONFile.c_str());
  if (! reader.parse(file, root)) {
    throw cms::Exception("ReadError")
      << "Failed to parse luminosity JSON file '" << mJSONFile << "'" << std::endl;
  }

  mValidRuns.reset(new Json::Value(root));
}

void GammaJetFilter::readCSVFile() {
  FILE* iff = fopen(mCSVFile.c_str(), "r");

  if(iff == 0) {
    throw cms::Exception("ReadError")
      << "Failed to parse luminosity CSV file '" << mCSVFile << "'" << std::endl;
  }

  int run = 0, fill = 0;
  int lumiSection_left = 0, lumiSection_right = 0;
  double lumiRecorded = 0.;


  /* lumiCalc2 format :
   * Run:Fill,LS,UTCTime,Beam Status,E(GeV),Delivered(/ub),Recorded(/ub),avgPU
   * use 'lumiCalc2.py -i lumiSummary.json -o output.csv -b stable lumibyls' to generate file
   */

  // Skip header line
  char buffer[1024];
  fgets(buffer, 1024, iff);

  while (fscanf(iff, "%d:%d,%d:%d,%*[^,],%*[^,],%*f,%*f,%lf,%*f", &run, &fill, &lumiSection_left, &lumiSection_right, &lumiRecorded) > 0 ) {

    if (lumiSection_right == 0)
      continue;

    mLumiByLS[std::pair<unsigned int, unsigned int>(run, lumiSection_right)] = lumiRecorded; //in mb^(-1)
  }

  fclose(iff);

  assert(mLumiByLS.size() > 0);
}

void GammaJetFilter::updateLuminosity(const edm::LuminosityBlock& lumiBlock) {
  double eventLumi = mLumiByLS[std::pair<unsigned int, unsigned int>(lumiBlock.id().run(), lumiBlock.id().luminosityBlock())];
  double newLumi = mTotalLuminosity->GetVal() + eventLumi;
  mTotalLuminosity->SetVal(newLumi);
}

void GammaJetFilter::particleToTree(const reco::Candidate* particle, TTree* t, std::vector<boost::shared_ptr<void> >& addresses) {
  addresses.clear();

  addresses.push_back(boost::shared_ptr<void>(new int((particle) ? 1 : 0)));
  addresses.push_back(boost::shared_ptr<void>(new float((particle) ? particle->et() : 0)));
  addresses.push_back(boost::shared_ptr<void>(new float((particle) ? particle->pt() : 0)));
  addresses.push_back(boost::shared_ptr<void>(new float((particle) ? particle->eta() : 0)));
  addresses.push_back(boost::shared_ptr<void>(new float((particle) ? particle->phi() : 0)));
  addresses.push_back(boost::shared_ptr<void>(new float((particle) ? particle->px() : 0)));
  addresses.push_back(boost::shared_ptr<void>(new float((particle) ? particle->py() : 0)));
  addresses.push_back(boost::shared_ptr<void>(new float((particle) ? particle->pz() : 0)));
  addresses.push_back(boost::shared_ptr<void>(new float((particle) ? particle->energy() : 0)));

  updateBranch(t, addresses[0].get(), "is_present", "I");
  updateBranch(t, addresses[1].get(), "et");
  updateBranch(t, addresses[2].get(), "pt");
  updateBranch(t, addresses[3].get(), "eta");
  updateBranch(t, addresses[4].get(), "phi");
  updateBranch(t, addresses[5].get(), "px");
  updateBranch(t, addresses[6].get(), "py");
  updateBranch(t, addresses[7].get(), "pz");
  updateBranch(t, addresses[8].get(), "e");
}

void GammaJetFilter::photonToTree(const pat::PhotonRef& photon, const edm::Event& event) {
  std::vector<boost::shared_ptr<void> > addresses;

  particleToTree(&(*photon), mPhotonTree, addresses);
  
  bool hasPixelSeed = photon->hasPixelSeed();
  updateBranch(mPhotonTree, &hasPixelSeed, "has_pixel_seed", "O");

  // Photon ID related
  float hadTowOverEm = photon->hadTowOverEm();
  updateBranch(mPhotonTree, &hadTowOverEm, "hadTowOverEm");

  float sigmaIetaIeta = photon->sigmaIetaIeta();
  updateBranch(mPhotonTree, &sigmaIetaIeta, "sigmaIetaIeta");

  edm::Handle<double> rhos;
  event.getByLabel(edm::InputTag("kt6PFJets", "rho", "RECO"), rhos);
  float rho = *rhos;
  updateBranch(mPhotonTree, &rho, "rho");

  // Isolations are produced at PAT level by the PḧotonPFIsolation producer
  edm::Handle<edm::ValueMap<bool>> hasMatchedPromptElectronHandle;
  event.getByLabel(edm::InputTag("photonPFIsolation", "hasMatchedPromptElectron", "PAT"), hasMatchedPromptElectronHandle);

  bool hasMatchedPromptElectron = (*hasMatchedPromptElectronHandle)[photon];
  updateBranch(mPhotonTree, &hasMatchedPromptElectron, "hasMatchedPromptElectron", "O");

  // Now, isolations
  edm::Handle<edm::ValueMap<double>> chargedHadronsIsolationHandle;
  event.getByLabel(edm::InputTag("photonPFIsolation", "chargedHadronsIsolation", "PAT"), chargedHadronsIsolationHandle);

  edm::Handle<edm::ValueMap<double>> neutralHadronsIsolationHandle;
  event.getByLabel(edm::InputTag("photonPFIsolation", "neutralHadronsIsolation", "PAT"), neutralHadronsIsolationHandle);

  edm::Handle<edm::ValueMap<double>> photonIsolationHandle;
  event.getByLabel(edm::InputTag("photonPFIsolation", "photonIsolation", "PAT"), photonIsolationHandle);

  float chargedHadronsIsolation = getCorrectedPFIsolation((*chargedHadronsIsolationHandle)[photon], rho, photon->eta(), IsolationType::CHARGED_HADRONS);
  float neutralHadronsIsolation = getCorrectedPFIsolation((*neutralHadronsIsolationHandle)[photon], rho, photon->eta(), IsolationType::NEUTRAL_HADRONS);
  float photonIsolation = getCorrectedPFIsolation((*photonIsolationHandle)[photon], rho, photon->eta(), IsolationType::PHOTONS);

  updateBranch(mPhotonTree, &chargedHadronsIsolation, "chargedHadronsIsolation");
  updateBranch(mPhotonTree, &neutralHadronsIsolation, "neutralHadronsIsolation");
  updateBranch(mPhotonTree, &photonIsolation, "photonIsolation");

  mPhotonTree->Fill();

  if (mIsMC) {
    particleToTree(photon->genPhoton(), mPhotonGenTree, addresses);
    mPhotonGenTree->Fill();
  }
}

void GammaJetFilter::jetsToTree(const pat::Jet* firstJet, const pat::Jet* secondJet, const std::vector<TTree*>& trees) {
  jetToTree(firstJet, mIsMC, trees[0], trees[4]);
  jetToTree(secondJet, false, trees[1], trees[5]);

  // Raw jets
  const pat::Jet* rawJet = (firstJet) ? firstJet->userData<pat::Jet>("rawJet") : NULL;
  jetToTree(rawJet, false, trees[2], NULL);

  rawJet = (secondJet) ? secondJet->userData<pat::Jet>("rawJet") : NULL;
  jetToTree(rawJet, false, trees[3], NULL);
}

void findNeutrinos(const reco::Candidate* parent, std::vector<const reco::Candidate*>& neutrinos) {

  int pdg_id = abs(parent->pdgId());
  if (pdg_id == 12 || pdg_id == 14 || pdg_id == 16) {
    if (std::find_if(neutrinos.begin(), neutrinos.end(), [parent] (const reco::Candidate* candidate) -> bool {
          static double EPSILON = 0.0001;
          bool same = (candidate->pdgId() == parent->pdgId());
          same &= (fabs(candidate->px() - candidate->px()) < EPSILON);
          same &= (fabs(candidate->py() - candidate->py()) < EPSILON);
          same &= (fabs(candidate->pz() - candidate->pz()) < EPSILON);

          return same;

      }) == neutrinos.end()) {
      neutrinos.push_back(parent);
    }
    return;
  }

  for (unsigned int i = 0; i < parent->numberOfDaughters(); i++) {
    const reco::Candidate* d = parent->daughter(i);
    findNeutrinos(d, neutrinos);
  }
}

void GammaJetFilter::jetToTree(const pat::Jet* jet, bool _findNeutrinos, TTree* tree, TTree* genTree) {
  std::vector<boost::shared_ptr<void> > addresses;
  particleToTree(jet, tree, addresses);

  if (mIsMC) {
    mNeutrinos->Clear("C");
    mNeutrinosPDG->Clear("C");
  }

  if (jet) {
    float area = jet->jetArea();
    updateBranch(tree, &area, "jet_area");

    // B-Tagging
    float tcHighEfficiency = jet->bDiscriminator("trackCountingHighEffBJetTags");
    float tcHighPurity = jet->bDiscriminator("trackCountingHighPurBJetTags");

    float ssvHighEfficiency = jet->bDiscriminator("simpleSecondaryVertexHighEffBJetTags");
    float ssvHighPurity = jet->bDiscriminator("simpleSecondaryVertexHighPurBJetTags");

    float jetProbability = jet->bDiscriminator("jetProbabilityBJetTags");
    float jetBProbability = jet->bDiscriminator("jetBProbabilityBJetTags");

    // New 2012
    float csv = jet->bDiscriminator("combinedSecondaryVertexBJetTags");

    // Quark Gluon tagging
    float qgTagMLP = jet->userFloat("qgTagMLP");
    float qgTagLikelihood = jet->userFloat("qgTagLikelihood");

    updateBranch(tree, &tcHighEfficiency, "btag_tc_high_eff");
    updateBranch(tree, &tcHighPurity, "btag_tc_high_pur");
    updateBranch(tree, &ssvHighEfficiency, "btag_ssv_high_eff");
    updateBranch(tree, &ssvHighPurity, "btag_ssv_high_pur");
    updateBranch(tree, &jetProbability, "btag_jet_probability");
    updateBranch(tree, &jetBProbability, "btag_jet_b_probability");
    updateBranch(tree, &csv, "btag_csv");
    updateBranch(tree, &qgTagMLP, "qg_tag_mlp");
    updateBranch(tree, &qgTagLikelihood, "qg_tag_likelihood");

    tree->Fill(); // This Fill() must be called inside the {} block, otherwise it'll crash. Don't move it!
  } else {
    tree->Fill();
  }

  if (genTree) {
    particleToTree((jet) ? jet->genJet() : NULL, genTree, addresses);

    if (_findNeutrinos) {
      static bool init = false;
      if (! init) {
        genTree->Branch("neutrinos", &mNeutrinos, 32000, 0);
        genTree->Branch("neutrinos_pdg_id", &mNeutrinosPDG, 32000, 0);
        init = true;
      }
    }

    if (jet && _findNeutrinos) {
      const reco::Candidate* parton = (jet) ? jet->genParton() : NULL;

      if (parton) {
        if (abs(parton->pdgId()) == 5 || abs(parton->pdgId()) == 4) {

          std::vector<const reco::Candidate*> neutrinos;
          findNeutrinos(parton, neutrinos);

          if (neutrinos.size() > 0) {
            // Build TCloneArray of TLorentzVector
            unsigned int index = 0;
            for (const reco::Candidate* neutrino: neutrinos) {
              TLorentzVector* p4 = (TLorentzVector*) mNeutrinos->ConstructedAt(index);
              p4->SetPxPyPzE(neutrino->px(), neutrino->py(), neutrino->pz(), neutrino->energy());

              TParameter<int>* pdg_id = (TParameter<int>*) mNeutrinosPDG->ConstructedAt(index++);
              pdg_id->SetVal(neutrino->pdgId());
            }
          }
        }

      }
    }

    // Add parton id and pt
    const reco::Candidate* parton = (jet) ? jet->genParton() : NULL;
    int pdgId = (parton) ? parton->pdgId() : 0;
    updateBranch(genTree, &pdgId, "parton_pdg_id", "I");

    TLorentzVector parton_p4;
    if (parton) {
      parton_p4.SetPxPyPzE(parton->px(), parton->py(), parton->pz(), parton->energy());
    }
    TLorentzVector* p_parton_p4 = &parton_p4;

    TBranch* branch = genTree->GetBranch("parton_p4");
    if (branch == NULL) {
      branch = genTree->Branch("parton_p4", &p_parton_p4);
    } else {
      branch->SetAddress(&p_parton_p4);
    }

    int flavour = (jet) ? jet->partonFlavour() : 0;
    updateBranch(genTree, &flavour, "parton_flavour", "I");

    genTree->Fill();
  }
}

void GammaJetFilter::metsToTree(const pat::MET& met, const pat::MET& rawMet, const std::vector<TTree*>& trees) {
  metToTree(&met, trees[0], trees[2]);
  metToTree(&rawMet, trees[1], NULL);
}

void GammaJetFilter::metToTree(const pat::MET* met, TTree* tree, TTree* genTree) {
  std::vector<boost::shared_ptr<void> > addresses;
  particleToTree(met, tree, addresses);

  tree->Fill();

  if (genTree) {
    particleToTree((met) ? met->genMET() : NULL, genTree, addresses);
    genTree->Fill();
  }
}

void GammaJetFilter::electronsToTree(const edm::Handle<pat::ElectronCollection>& electrons, const reco::Vertex& pv) {

  int n = electrons->size();
  static int   id[30];
  static float isolation[30];
  static float pt[30];
  static float px[30];
  static float py[30];
  static float pz[30];
  static float eta[30];
  static float phi[30];
  static int   charge[30];

  int i = 0;
  for (pat::ElectronCollection::const_iterator it = electrons->begin(); it != electrons->end(); ++it, i++) {
    const pat::Electron& electron = *it;

    if (i >= 30)
      break;

    // See https://twiki.cern.ch/twiki/bin/view/CMS/TopLeptonPlusJetsRefSel_el
    bool elecID = fabs(pv.z() - it->vertex().z()) < 1.;
    elecID     &= it->et() > 30.;
    elecID     &= fabs(it->eta()) < 2.5 && (it->superCluster()->eta() > 1.4442 && it->superCluster()->eta() < 1.5660);
    elecID     &= it->dB() < 0.02;
    elecID     &= ((int) it->electronID("eidLoose") & 0x1);

    float iso     = (it->dr03TkSumPt() + it->dr03EcalRecHitSumEt() + it->dr03HcalTowerSumEt()) / it->et();

    id[i]         = elecID;
    isolation[i]  = iso;
    pt[i]         = electron.pt();
    px[i]         = electron.px();
    py[i]         = electron.py();
    pz[i]         = electron.pz();
    eta[i]        = electron.eta();
    phi[i]        = electron.phi();
    charge[i]     = electron.charge();
  }

  updateBranch(mElectronsTree, &n, "n", "I");
  updateBranchArray(mElectronsTree, id, "id", "n", "I");
  updateBranchArray(mElectronsTree, isolation, "isolation", "n");
  updateBranchArray(mElectronsTree, pt, "pt", "n");
  updateBranchArray(mElectronsTree, px, "px", "n");
  updateBranchArray(mElectronsTree, py, "py", "n");
  updateBranchArray(mElectronsTree, pz, "pz", "n");
  updateBranchArray(mElectronsTree, eta, "eta", "n");
  updateBranchArray(mElectronsTree, phi, "phi", "n");
  updateBranchArray(mElectronsTree, charge, "charge", "n", "I");

  mElectronsTree->Fill();
}

void GammaJetFilter::muonsToTree(const edm::Handle<pat::MuonCollection>& muons, const reco::Vertex& pv) {

  int n = muons->size();
  static int   id[30];
  static float isolation[30];
  static float delta_beta_isolation[30];
  static float pt[30];
  static float px[30];
  static float py[30];
  static float pz[30];
  static float eta[30];
  static float phi[30];
  static int   charge[30];

  int i = 0;
  for (pat::MuonCollection::const_iterator it = muons->begin(); it != muons->end(); ++it, i++) {
    const pat::Muon& muon = *it;

    if (i >= 30)
      break;

    // See https://twiki.cern.ch/twiki/bin/view/CMS/TopLeptonPlusJetsRefSel_mu
    bool muonID = it->isGlobalMuon();
    //FIXME: reco::Tracks need to be keept in PF2PAT.
    //It's not the case right now, so muon ID will be incorrect
    if (it->globalTrack().isNull() || it->innerTrack().isNull() || it->muonBestTrack().isNull() || it->track().isNull()) {
      muonID = false;
    } else {
      muonID     &= it->globalTrack()->normalizedChi2() < 10.;
      muonID     &= it->globalTrack()->hitPattern().numberOfValidMuonHits() > 0;
      muonID     &= it->numberOfMatchedStations() > 1;
      muonID     &= it->dB() < 0.2;
      muonID     &= fabs(it->muonBestTrack()->dz(pv.position())) < 0.5;
      muonID     &= it->innerTrack()->hitPattern().numberOfValidPixelHits() > 0;
      muonID     &= it->track()->hitPattern().trackerLayersWithMeasurement() > 5;
    }

    float relIso = (it->chargedHadronIso() + it->neutralHadronIso() + it->photonIso()) / it->pt();
    float deltaBetaRelIso = (it->chargedHadronIso() + std::max((it->neutralHadronIso() + it->photonIso()) - 0.5 * it->puChargedHadronIso(), 0.0)) / it->pt();

    id[i]          = muonID;
    isolation[i]   = relIso;
    delta_beta_isolation[i] = deltaBetaRelIso;
    pt[i]          = muon.pt();
    px[i]          = muon.px();
    py[i]          = muon.py();
    pz[i]          = muon.pz();
    eta[i]         = muon.eta();
    phi[i]         = muon.phi();
    charge[i]      = muon.charge();
  }

  updateBranch(mMuonsTree, &n, "n", "I");
  updateBranchArray(mMuonsTree, id, "id", "n", "I");
  updateBranchArray(mMuonsTree, isolation, "relative_isolation", "n");
  updateBranchArray(mMuonsTree, delta_beta_isolation, "delta_beta_relative_isolation", "n");
  updateBranchArray(mMuonsTree, pt, "pt", "n");
  updateBranchArray(mMuonsTree, px, "px", "n");
  updateBranchArray(mMuonsTree, py, "py", "n");
  updateBranchArray(mMuonsTree, pz, "pz", "n");
  updateBranchArray(mMuonsTree, eta, "eta", "n");
  updateBranchArray(mMuonsTree, phi, "phi", "n");
  updateBranchArray(mMuonsTree, charge, "charge", "n", "I");

  mMuonsTree->Fill();
}

//define this as a plug-in
DEFINE_FWK_MODULE(GammaJetFilter);
