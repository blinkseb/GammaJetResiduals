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
#include <memory>
#include <string>

// Boost
#include "boost/shared_ptr.hpp"

// user include files
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "CommonTools/Utils/interface/PtComparator.h"

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDFilter.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/LuminosityBlock.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/Run.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/Utilities/interface/Exception.h"

#include "FWCore/ServiceRegistry/interface/Service.h"

#include "DataFormats/Math/interface/deltaPhi.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/Common/interface/Ref.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Photon.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"

#include "PhysicsTools/SelectorUtils/interface/JetIDSelectionFunctor.h"

#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"

#include "JetMETCorrections/Objects/interface/JetCorrector.h"
#include "JetMETCorrections/GammaJetFilter/interface/json/json.h"

#include <TParameter.h>
#include <TTree.h>

//
// class declaration
//

enum JetAlgorithm {
  AK5,
  AK7
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
      bool processJets(const pat::Photon& photon, const pat::JetCollection& jets, const JetAlgorithm algo, std::vector<TTree*>& trees);

      bool isValidPhotonEE(const pat::Photon& photon, const double rho);
      bool isValidPhotonEB(const pat::Photon& photon, const double rho);
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

      bool mDoJEC;
      bool mJECFromRaw;
      std::string mCorrectorLabel;
      GreaterByPt<pat::Jet> mSorter;

      bool mFirstJetPtCut;
      double mFirstJetThreshold;
      
      std::vector<std::string> mJetCollections;

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
      TParameter<int>*              mJECRedone;
      TParameter<int>*              mJECFromRawParameter;
      TNamed*                       mJECCorrectorLabel;
      TParameter<int>*              mFirstJetPtCutParameter;
      TParameter<double>*           mFirstJetThresholdParameter;

      // DEBUG
      TH1F* mDeltaPhi;

      void particleToTree(const reco::Candidate* particle, TTree* t, std::vector<boost::shared_ptr<void> >& addresses);
      void updateBranch(TTree* tree, void* address, const std::string& name, const std::string& type = "F");
      void updateBranchArray(TTree* tree, void* address, const std::string& name, const std::string& size, const std::string& type = "F");
      
      void photonToTree(const pat::Photon& photon);
      void metsToTree(const pat::MET& met, const std::vector<TTree*>& trees);
      void metToTree(const pat::MET* met, TTree* tree, TTree* genTree);
      void jetsToTree(const pat::Jet& firstJet, const pat::Jet* secondJet, const std::vector<TTree*>& trees);
      void jetToTree(const pat::Jet* jet, TTree* tree, TTree* genTree);
      void electronsToTree(const edm::Handle<pat::ElectronCollection>& electrons, const reco::Vertex& pv);
      void muonsToTree(const edm::Handle<pat::MuonCollection>& muons, const reco::Vertex& pv);
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
  mJetsAK5PFlowIT = iConfig.getUntrackedParameter<edm::InputTag>("jetsAK5PFlow", edm::InputTag("selectedPatJetsAK5PFlow"));
  mJetsAK7PFlowIT = iConfig.getUntrackedParameter<edm::InputTag>("jetsAK7PFlow", edm::InputTag("selectedPatJetsAK7PFlow"));
  mJetsAK5CaloIT = iConfig.getUntrackedParameter<edm::InputTag>("jetsAK5Calo", edm::InputTag("selectedPatJets"));
  mJetsAK7CaloIT = iConfig.getUntrackedParameter<edm::InputTag>("jetsAK7Calo", edm::InputTag("selectedPatJetsAK7Calo"));
  mDoJEC = iConfig.getUntrackedParameter<bool>("doJetCorrection", false);

  if (mDoJEC) {
    mJECFromRaw = iConfig.getUntrackedParameter<bool>("correctJecFromRaw", false);
    mCorrectorLabel = iConfig.getUntrackedParameter<std::string>("correctorLabel", "ak5PFResidual");
  }

  mFirstJetPtCut = iConfig.getUntrackedParameter<bool>("firstJetPtCut", true);
  mFirstJetThreshold = iConfig.getUntrackedParameter<double>("firstJetThreshold", 0.3);

  if (! mIsMC && mFilterData) {
    // Load JSON file of good runs
    readJSONFile();
    readCSVFile();
  }

   edm::Service<TFileService> fs;
   mPhotonTree = fs->make<TTree>("photon", "photon tree");
   mPhotonGenTree = fs->make<TTree>("photon_gen", "photon gen tree");
   mAnalysisTree = fs->make<TTree>("analysis", "analysis tree");
   mMuonsTree = fs->make<TTree>("muons", "muons tree");
   mElectronsTree = fs->make<TTree>("electrons", "electrons tree");

   mTotalLuminosity = fs->make<TParameter<double> >("total_luminosity", 0.);
   if (mIsMC) {
     // Read cross section and number of generated events
     double crossSection = iConfig.getParameter<double>("crossSection");
     unsigned long long generatedEvents = iConfig.getParameter<unsigned long long>("generatedEvents");
     mEventsWeight = crossSection / (float) generatedEvents;

     mPtHatMin = iConfig.getParameter<double>("ptHatMin");
     mPtHatMax = iConfig.getParameter<double>("ptHatMax");
   } else {
     mEventsWeight = 1.;
   }

   mJetCollections.push_back("AK5PFlow");
   mJetCollections.push_back("AK7PFlow");
   mJetCollections.push_back("AK5Calo");
   mJetCollections.push_back("AK7Calo");

   FOREACH(mJetCollections) {
     createTrees(*it, *fs);
   }

   mProcessedEvents = fs->make<TParameter<long long> >("total_events", 0);
   mSelectedEvents = fs->make<TParameter<long long> >("passed_events", 0);

   mJECRedone = fs->make<TParameter<int> >("jec_redone", mDoJEC);
   mFirstJetPtCutParameter = fs->make<TParameter<int> >("cut_on_first_jet_pt", mFirstJetPtCut);
   if (mDoJEC) {
     mJECFromRawParameter = fs->make<TParameter<int> >("jec_from_raw_jet", mJECFromRaw);
     mJECCorrectorLabel = fs->make<TNamed>("jec_corrector_label", mCorrectorLabel);
   }

   if (mFirstJetPtCut) {
     mFirstJetThresholdParameter = fs->make<TParameter<double> >("cut_on_first_jet_treshold", mFirstJetThreshold);
   }

   mDeltaPhi = fs->make<TH1F>("deltaPhi", "deltaPhi", 40, M_PI / 2., M_PI);
}


GammaJetFilter::~GammaJetFilter()
{

  // do anything here that needs to be done at desctruction time
  // (e.g. close files, deallocate resources etc.)
}

void GammaJetFilter::createTrees(const std::string& rootName, TFileService& fs) {

  TFileDirectory dir = fs.mkdir(rootName);
  std::vector<TTree*>& trees = mJetTrees[rootName];

  trees.push_back(dir.make<TTree>("first_jet", "first jet tree"));
  trees.push_back(dir.make<TTree>("second_jet", "second jet tree"));

  trees.push_back(dir.make<TTree>("first_jet_raw", "first raw jet tree"));
  trees.push_back(dir.make<TTree>("second_jet_raw", "second raw jet tree"));

  trees.push_back(dir.make<TTree>("first_jet_gen", "first gen jet tree"));
  trees.push_back(dir.make<TTree>("second_jet_gen", "second gen jet tree"));

  // MET
  std::vector<TTree*>& met = mMETTrees[rootName];
  met.push_back(dir.make<TTree>("met", "met tree"));
  met.push_back(dir.make<TTree>("met_raw", "met raw tree"));
  met.push_back(dir.make<TTree>("met_gen", "met gen tree"));

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

  if (mIsMC) {
    edm::Handle<GenEventInfoProduct> eventInfos;
    iEvent.getByLabel("generator", eventInfos);

    if (eventInfos.isValid() && eventInfos->hasBinningValues()) {
      double genPt = eventInfos->binningValues()[0];
      if (genPt < mPtHatMin || genPt > mPtHatMax) {
        return false;
      }
    }
  }

  edm::Handle<double> pFlowRho;
  iEvent.getByLabel(edm::InputTag("kt6PFJetsAK5PFlow", "rho"), pFlowRho);

  edm::Handle<pat::PhotonCollection> photons;
  iEvent.getByLabel(mPhotonsIT, photons);

  pat::PhotonCollection photonsRef;

  pat::PhotonCollection::const_iterator it = photons->begin();
  for (; it != photons->end(); ++it) {
    if (isValidPhotonEB(*it, *pFlowRho) && fabs(it->eta()) <= 1.3) {
      photonsRef.push_back(*it);
    }
  }

  // Only one good photon per event
  if (photonsRef.size() > 1)
    return false;

  const pat::Photon& photon = photonsRef[0];

  bool eventHasJets = false;

  // Process jets

  edm::Handle<pat::JetCollection> jetsHandle;

  iEvent.getByLabel(mJetsAK5PFlowIT, jetsHandle);
  pat::JetCollection jets = *jetsHandle;
  if (mDoJEC) {
    correctJets(jets, iEvent, iSetup);
  }
  eventHasJets |= processJets(photon, jets, AK5, mJetTrees["AK5PFlow"]);

  /*iEvent.getByLabel(mJetsAK7PFlowIT, jets);
    eventHasJets |= processJets(photon, jets, AK7, mJetTrees["AK7PFlow"]);

    iEvent.getByLabel(mJetsAK5CaloIT, jets);
    eventHasJets |= processJets(photon, jets, AK5, mJetTrees["AK5Calo"]);

    iEvent.getByLabel(mJetsAK7CaloIT, jets);
    eventHasJets |= processJets(photon, jets, AK7, mJetTrees["AK7Calo"]);*/

  if (! eventHasJets)
    return false;

  // MET

  FOREACH(mJetCollections) {
    edm::Handle<pat::METCollection> mets;
    iEvent.getByLabel(std::string("patMETs" + ((*it == "AK5Calo") ? "" : *it)), mets);
    metsToTree(mets->at(0), mMETTrees[*it]);
  }

  // Rho
  FOREACH(mJetCollections) {
    edm::Handle<double> rhos;
    if (it->find("Calo") != std::string::npos)
      iEvent.getByLabel(edm::InputTag("kt6CaloJets", "rho"), rhos);
    else
      iEvent.getByLabel(edm::InputTag(std::string("kt6PFJets" + *it), "rho"), rhos);

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
  updateBranch(mAnalysisTree, &mEventsWeight, "event_weight");

  mAnalysisTree->Fill();

  photonToTree(photon);

  // Electrons
  edm::Handle<pat::ElectronCollection> electrons;
  iEvent.getByLabel("selectedPatElectronsAK5PFlow", electrons);
  electronsToTree(electrons, primaryVertex);

  // Muons
  edm::Handle<pat::MuonCollection> muons;
  iEvent.getByLabel("selectedPatMuonsAK5PFlow", muons);
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

bool GammaJetFilter::processJets(const pat::Photon& photon, const pat::JetCollection& jets, const JetAlgorithm algo, std::vector<TTree*>& trees) {
  if (jets.size() == 0) {
    return false;
  }

  pat::JetCollection selectedJets;

  pat::JetCollection::const_iterator it = jets.begin();
  for (; it != jets.end(); ++it) {

    if (! isValidJet(*it))
      continue;

    if (selectedJets.size() == 0) {
      // First jet selection

      const double deltaPhi = reco::deltaPhi(photon, *it);
      if (fabs(deltaPhi) < M_PI / 2.)
        continue; // Only back 2 back event are interesting

      // Jet are ordered by pt value.
      // Events are supposed to be balanced between Jet and Gamma
      // If the leading jet has less than 30% of the Photon pt,
      // dump the event as it's not interesting
      if (mFirstJetPtCut && (it->pt() < photon.pt() * mFirstJetThreshold))
        return false;

      selectedJets.push_back(*it);

    } else {
      // Second jet selection
      const double deltaR = reco::deltaR(photon, *it);
      const double deltaR_threshold = (algo == AK5) ? 0.5 : 0.7;

      if (deltaR > deltaR_threshold) {
        selectedJets.push_back(*it);
        break;
      }
    }

  }

  if (selectedJets.size() > 0) {
    const pat::Jet* secondJet = NULL;
    if (selectedJets.size() > 1)
      secondJet = &selectedJets[1];

    jetsToTree(selectedJets[0], secondJet, trees);

    mDeltaPhi->Fill(fabs(reco::deltaPhi(photon, selectedJets[0])));

    return true;
  }

  return false;
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
    isValid &= jet.getPFConstituents().size() > 1;
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

bool GammaJetFilter::isValidPhotonEB(const pat::Photon& photon, const double rho) {
  if (mIsMC && !photon.genPhoton())
    return false;

  bool isValid = ! photon.hasPixelSeed();
  isValid &= photon.hadronicOverEm() < 0.05;
  isValid &= photon.sigmaIetaIeta() < 0.011;
  isValid &= photon.trkSumPtHollowConeDR04() < (2.0 + 0.001 * photon.et() + 0.0167 * rho);
  isValid &= photon.ecalRecHitSumEtConeDR04() < (4.2 + 0.006 * photon.et() + 0.183 * rho);
  isValid &= photon.hcalTowerSumEtConeDR04() < (2.2 + 0.0025 * photon.et() + 0.062 * rho);

  return isValid;
}

bool GammaJetFilter::isValidPhotonEE(const pat::Photon& photon, const double rho) {
  if (mIsMC && !photon.genPhoton())
    return false;

  bool isValid = ! photon.hasPixelSeed();
  isValid &= photon.hadronicOverEm() < 0.05;
  isValid &= photon.sigmaIetaIeta() < 0.011;
  isValid &= photon.trkSumPtHollowConeDR04() < (2.0 + 0.001 * photon.et() + 0.032 * rho);
  isValid &= photon.ecalRecHitSumEtConeDR04() < (4.2 + 0.006 * photon.et() + 0.090 * rho);
  isValid &= photon.hcalTowerSumEtConeDR04() < (2.2 + 0.0025 * photon.et() + 0.180 * rho);

  return isValid;
}

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
   * Run,LS,UTCTime,Beam Status,E(GeV),Delivered(/ub),Recorded(/ub)
   * use 'lumiCalc2.py -i lumiSummary.json -o output.csv -b stable lumibyls' to generate file
   */

  // Skip header line
  char buffer[1024];
  fgets(buffer, 1024, iff);

  while (fscanf(iff, "%d:%d,%d:%d,%*[^,],%*[^,],%*f,%*f,%lf", &run, &fill, &lumiSection_left, &lumiSection_right, &lumiRecorded) > 0 ) {

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

void GammaJetFilter::photonToTree(const pat::Photon& photon) {
  std::vector<boost::shared_ptr<void> > addresses;

  particleToTree(&photon, mPhotonTree, addresses);
  mPhotonTree->Fill();

  particleToTree(photon.genPhoton(), mPhotonGenTree, addresses);
  mPhotonGenTree->Fill();
}

void GammaJetFilter::jetsToTree(const pat::Jet& firstJet, const pat::Jet* secondJet, const std::vector<TTree*>& trees) {
  jetToTree(&firstJet, trees[0], trees[4]);
  jetToTree(secondJet, trees[1], trees[5]);

  // Raw jets
  pat::Jet uncorrectJet = firstJet.correctedJet("Uncorrected");
  jetToTree(&uncorrectJet, trees[2], NULL);

  uncorrectJet = (secondJet) ? secondJet->correctedJet("Uncorrected") : pat::Jet();
  jetToTree((secondJet) ? &uncorrectJet : NULL, trees[3], NULL);

}

void GammaJetFilter::jetToTree(const pat::Jet* jet, TTree* tree, TTree* genTree) {
  std::vector<boost::shared_ptr<void> > addresses;
  particleToTree(jet, tree, addresses);

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

    updateBranch(tree, &tcHighEfficiency, "btag_tc_high_eff");
    updateBranch(tree, &tcHighPurity, "btag_tc_high_pur");
    updateBranch(tree, &ssvHighEfficiency, "btag_ssv_high_eff");
    updateBranch(tree, &ssvHighPurity, "btag_ssv_high_pur");
    updateBranch(tree, &jetProbability, "btag_jet_probability");
    updateBranch(tree, &jetBProbability, "btag_jet_b_probability");

    tree->Fill(); // This Fill() must be called inside the {} block, otherwise it'll crash. Don't move it!
  } else {
    tree->Fill();
  }

  if (genTree) {
    particleToTree((jet) ? jet->genJet() : NULL, genTree, addresses);
    genTree->Fill();
  }
}

void GammaJetFilter::metsToTree(const pat::MET& met, const std::vector<TTree*>& trees) {
  metToTree(&met, trees[0], trees[2]);

  // Uncorrected MET
  pat::MET rawMET(met);
  reco::LeafCandidate::PolarLorentzVector p4 = rawMET.polarP4();
  p4.SetPt(met.uncorrectedPt());
  p4.SetPhi(met.uncorrectedPhi());
  rawMET.setP4(p4);

  metToTree(&rawMET, trees[1], NULL);
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
  static int   isolated[30];
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

    bool isIsolated = (it->dr03TkSumPt() + it->dr03EcalRecHitSumEt() + it->dr03HcalTowerSumEt()) / it->et();

    id[i]       = elecID;
    isolated[i] = isIsolated;
    pt[i]       = electron.pt();
    px[i]       = electron.px();
    py[i]       = electron.py();
    pz[i]       = electron.pz();
    eta[i]      = electron.eta();
    phi[i]      = electron.phi();
    charge[i]   = electron.charge();
  }

  updateBranch(mElectronsTree, &n, "n", "I");
  updateBranchArray(mElectronsTree, id, "id", "n", "I");
  updateBranchArray(mElectronsTree, isolated, "isolated", "n", "I");
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
  static int   isolated[30];
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
    bool muonID = it->isGlobalMuon() && it->isTrackerMuon();
    muonID     &= it->pt() > 20.;
    muonID     &= fabs(it->eta()) < 2.1;
    muonID     &= it->muonID("GlobalMuonPromptTight");
    //FIXME: reco::Tracks need to be keept in PF2PAT.
    //It's not the case right now, so muon ID will be incorrect
    /*muonID     &= (it->innerTrack()->numberOfValidHits() > 10);
      muonID     &= (it->dB() < 0.02);
      muonID     &= it->innerTrack()->hitPattern().pixelLayersWithMeasurement() >= 1;*/
    muonID     &= it->numberOfMatches() > 1;
    muonID     &= fabs(pv.z() - it->vertex().z()) < 1.;

    bool isIsolated = (it->trackIso() + it->ecalIso() + it->hcalIso()) / (it->pt());

    id[i]       = muonID;
    isolated[i] = isIsolated;
    pt[i]       = muon.pt();
    px[i]       = muon.px();
    py[i]       = muon.py();
    pz[i]       = muon.pz();
    eta[i]      = muon.eta();
    phi[i]      = muon.phi();
    charge[i]   = muon.charge();
  }

  updateBranch(mMuonsTree, &n, "n", "I");
  updateBranchArray(mMuonsTree, id, "id", "n", "I");
  updateBranchArray(mMuonsTree, isolated, "isolated", "n", "I");
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
