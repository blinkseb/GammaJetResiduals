#include <TFile.h>
#include <TROOT.h>
#include <TChain.h>
#include <TSystem.h>
#include <TTree.h>
#include <TParameter.h>
#include <TH2D.h>

#include <fstream>
#include <sstream>

#include <signal.h>
#include <stdlib.h>
#include <stdio.h>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

#include <FWCore/FWLite/interface/AutoLibraryLoader.h>
#include <FWCore/Framework/interface/Event.h>

#include <DataFormats/Common/interface/Handle.h>
#include <DataFormats/FWLite/interface/Event.h>
#include <DataFormats/FWLite/interface/ChainEvent.h>
#include <DataFormats/Math/interface/deltaPhi.h>
#include <DataFormats/PatCandidates/interface/Jet.h>
#include <DataFormats/PatCandidates/interface/MET.h>
#include <DataFormats/PatCandidates/interface/Photon.h>

#include <PhysicsTools/FWLite/interface/TFileService.h>

#include <SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h>

#include "tclap/CmdLine.h"

#include "gammaJetFinalizer.h"
#include "PUReweighter.h"
#include "JECReader.h"

#include <boost/regex.hpp>

#define RESET_COLOR "\033[m"
#define MAKE_RED "\033[31m"
#define MAKE_BLUE "\033[34m"

#define ADD_TREES true

#define DELTAPHI_CUT (2.8)

#define TRIGGER_OK                    0
#define TRIGGER_NOT_FOUND            -1
#define TRIGGER_FOUND_BUT_PT_OUT     -2

bool EXIT = false;

GammaJetFinalizer::GammaJetFinalizer():
  mRandomGenerator(0) {
  mPUWeight = 1.;

  mDoMCComparison = false;
  mNoPUReweighting = false;
  mIsBatchJob = false;
  mUseExternalJECCorrecion = false;
}

GammaJetFinalizer::~GammaJetFinalizer() {

}

std::string GammaJetFinalizer::buildPostfix() {
  std::string algo = mJetAlgo == AK5 ? "AK5" : "AK7";
  std::string type = mJetType == PF ? "PFlow" : "Calo";

  std::string postfix = type + algo;

  if (mUseCHS)
    postfix += "chs";

  return postfix;
}

void GammaJetFinalizer::loadFiles(TChain& chain) {
  for (std::vector<std::string>::const_iterator it = mInputFiles.begin(); it != mInputFiles.end(); ++it) {
    chain.Add(it->c_str());
  }
}

void GammaJetFinalizer::cloneTree(TTree* from, TTree*& to) {
  to = from->CloneTree(0);
  from->CopyAddresses(to);
}

void GammaJetFinalizer::runAnalysis() {
  
  if (mIsMC) {
    mMCTriggers = new MCTriggers("triggers_mc.xml");
  } else {
    mTriggers = new Triggers("triggers.xml");
  }

  // Initialization
  mExtrapBinning.initialize(mPtBinning, (mJetType == PF) ? "PFlow" : "Calo");
  mNewExtrapBinning.initialize(mAlphaCut);

  if (mIsMC) {
    std::cout << "Parsing triggers_mc.xml ..." << std::endl;
    if (! mMCTriggers->parse()) {
      std::cerr << "Failed to parse triggers_mc.xml..." << std::endl;
      return;
    }
    std::cout << "done." << std::endl;
  } else {
    std::cout << "Parsing triggers.xml ..." << std::endl;
    if (! mTriggers->parse()) {
      std::cerr << "Failed to parse triggers.xml..." << std::endl;
      return;
    }
    std::cout << "done." << std::endl;
  }
  std::cout << "triggers mapping:" << std::endl;
  if (mIsMC)
    mMCTriggers->print();
  else
   mTriggers->print();

  std::cout << "Opening files ..." << std::endl;

  const std::string postFix = buildPostfix();

  // Set max TTree size
  TTree::SetMaxTreeSize(429496729600LL);

  TChain analysisChain("gammaJet/analysis");
  TChain photonChain("gammaJet/photon");
  TChain genPhotonChain("gammaJet/photon_gen");
  TChain muonsChain("gammaJet/muons");
  TChain electronsChain("gammaJet/electrons");

  TString treeName = TString::Format("gammaJet/%s/first_jet", postFix.c_str());
  TChain firstJetChain(treeName);
  treeName = TString::Format("gammaJet/%s/first_jet_raw", postFix.c_str());
  TChain firstRawJetChain(treeName);
  treeName = TString::Format("gammaJet/%s/first_jet_gen", postFix.c_str());
  TChain firstGenJetChain(treeName);

  treeName = TString::Format("gammaJet/%s/second_jet", postFix.c_str());
  TChain secondJetChain(treeName);
  treeName = TString::Format("gammaJet/%s/second_jet_raw", postFix.c_str());
  TChain secondRawJetChain(treeName);
  treeName = TString::Format("gammaJet/%s/second_jet_gen", postFix.c_str());
  TChain secondGenJetChain(treeName);

  treeName = TString::Format("gammaJet/%s/met", postFix.c_str());
  TChain metChain(treeName);
  treeName = TString::Format("gammaJet/%s/met_raw", postFix.c_str());
  TChain rawMetChain(treeName);
  treeName = TString::Format("gammaJet/%s/met_gen", postFix.c_str());
  TChain genMetChain(treeName);

  treeName = TString::Format("gammaJet/%s/misc", postFix.c_str());
  TChain miscChain(treeName);

  loadFiles(analysisChain);
  loadFiles(photonChain);
  loadFiles(genPhotonChain);
  loadFiles(muonsChain);
  loadFiles(electronsChain);

  loadFiles(firstJetChain);
  loadFiles(firstRawJetChain);
  loadFiles(firstGenJetChain);

  loadFiles(secondJetChain);
  loadFiles(secondGenJetChain);
  loadFiles(secondRawJetChain);

  loadFiles(metChain);
  loadFiles(genMetChain);
  loadFiles(rawMetChain);

  loadFiles(miscChain);

  analysis.Init(&analysisChain);
  photon.Init(&photonChain);
  genPhoton.Init(&genPhotonChain);
  muons.Init(&muonsChain);
  electrons.Init(&electronsChain);

  firstJet.Init(&firstJetChain);
  firstRawJet.Init(&firstRawJetChain);
  firstGenJet.Init(&firstGenJetChain);

#if !ADD_TREES
  firstJet.DisableUnrelatedBranches();
  firstRawJet.DisableUnrelatedBranches();
#endif

  secondJet.Init(&secondJetChain);
  secondRawJet.Init(&secondRawJetChain);
  secondGenJet.Init(&secondGenJetChain);

#if !ADD_TREES
  secondJet.DisableUnrelatedBranches();
  secondRawJet.DisableUnrelatedBranches();
#endif

  MET.Init(&metChain);
  genMET.Init(&genMetChain);
  rawMET.Init(&rawMetChain);

  misc.Init(&miscChain);

  std::cout << "done." << std::endl;

  std::cout << std::endl << "##########" << std::endl;
  std::cout << "# " << MAKE_BLUE << "Running on " << MAKE_RED << ((mIsMC) ? "MC" : "DATA") << RESET_COLOR << std::endl;
  if (mUseExternalJECCorrecion) {
    std::cout << "# " << MAKE_RED << "Using external JEC " << RESET_COLOR << std::endl;
  }
  std::cout << "##########" << std::endl << std::endl;

  // Output file
  // Build output file name
  // PhotonJet_<dataset>_<postfix>.root
  std::string outputFile = (!mIsBatchJob)
    ? TString::Format("PhotonJet_%s_%s.root", mDatasetName.c_str(), postFix.c_str()).Data()
    : TString::Format("PhotonJet_%s_%s_part%02d.root", mDatasetName.c_str(), postFix.c_str(), mCurrentJob).Data();
  fwlite::TFileService fs(outputFile);

#if ADD_TREES
  TTree* photonTree = NULL;
  cloneTree(photon.fChain, photonTree);

  TTree* genPhotonTree = NULL;
  cloneTree(genPhoton.fChain, genPhotonTree);

  TTree* firstJetTree = NULL;
  cloneTree(firstJet.fChain, firstJetTree);

  TTree* firstGenJetTree = NULL;
  cloneTree(firstGenJet.fChain, firstGenJetTree);

  TTree* firstRawJetTree = NULL;
  cloneTree(firstRawJet.fChain, firstRawJetTree);

  TTree* secondJetTree = NULL;
  cloneTree(secondJet.fChain, secondJetTree);

  TTree* secondGenJetTree = NULL;
  cloneTree(secondGenJet.fChain, secondGenJetTree);

  TTree* secondRawJetTree = NULL;
  cloneTree(secondRawJet.fChain, secondRawJetTree);

  TTree* metTree = NULL;
  cloneTree(MET.fChain, metTree);

  TTree* rawMetTree = NULL;
  cloneTree(rawMET.fChain, rawMetTree);

  TTree* genMetTree = NULL;
  cloneTree(genMET.fChain, genMetTree);

  TTree* muonsTree = NULL;
  cloneTree(muons.fChain, muonsTree);

  TTree* electronsTree = NULL;
  cloneTree(electrons.fChain, electronsTree);

  TTree* analysisTree = NULL;
  cloneTree(analysis.fChain, analysisTree);
  analysisTree->SetName("misc");
#endif

  FactorizedJetCorrector* jetCorrector = NULL;
  //void* jetCorrector = NULL;
  if (mUseExternalJECCorrecion) {

    std::string jecJetAlgo = "AK5";
    if (mJetType == PF)
      jecJetAlgo += "PF";
    else/* if (recoType == "calo")*/
      jecJetAlgo += "Calo";
    /*else if (recoType == "jpt")
      jecJetAlgo += "JPT";*/

    if (mJetType == PF && mUseCHS)
      jecJetAlgo += "chs";

    std::cout << "Using '" << jecJetAlgo << "' algorithm for external JEC" << std::endl;

    const std::string payloadsFile = "jec_payloads.xml";
    jetCorrector = makeFactorizedJetCorrectorFromXML(payloadsFile, jecJetAlgo, mIsMC);
  }

  std::cout << "Processing..." << std::endl;

  // Automatically call Sumw2 when creating an histogram
  TH1::SetDefaultSumw2(true);

  // Init some analysis variables
  TFileDirectory analysisDir = fs.mkdir("analysis");

  TH1F* h_nvertex = analysisDir.make<TH1F>("nvertex", "nvertex", 50, 0., 50.);
  TH1F* h_nvertex_reweighted = analysisDir.make<TH1F>("nvertex_reweighted", "nvertex_reweighted", 50, 0., 50.);

  TH1F* h_deltaPhi = analysisDir.make<TH1F>("deltaPhi", "deltaPhi", 60, M_PI / 2, M_PI);
  TH1F* h_deltaPhi_2ndJet = analysisDir.make<TH1F>("deltaPhi_2ndjet", "deltaPhi of 2nd jet", 60, M_PI / 2., M_PI);
  TH1F* h_ptPhoton = analysisDir.make<TH1F>("ptPhoton", "ptPhoton", 200, 5., 1000.);
  TH1F* h_ptFirstJet = analysisDir.make<TH1F>("ptFirstJet", "ptFirstJet", 200, 5., 1000.);
  TH1F* h_ptSecondJet = analysisDir.make<TH1F>("ptSecondJet", "ptSecondJet", 60, 0., 100.);
  TH1F* h_MET = analysisDir.make<TH1F>("MET", "MET", 150, 0., 300.);
  TH1F* h_alpha = analysisDir.make<TH1F>("alpha", "alpha", 100, 0., 2.);

  TH1F* h_rho = analysisDir.make<TH1F>("rho", "rho", 100, 0, 50);
  TH1F* h_hadTowOverEm = analysisDir.make<TH1F>("hadTowOverEm", "hadTowOverEm", 100, 0, 0.05);
  TH1F* h_sigmaIetaIeta = analysisDir.make<TH1F>("sigmaIetaIeta", "sigmaIetaIeta", 100, 0, 0.011);
  TH1F* h_chargedHadronsIsolation = analysisDir.make<TH1F>("chargedHadronsIsolation", "chargedHadronsIsolation", 100, 0, 0.7);
  TH1F* h_neutralHadronsIsolation = analysisDir.make<TH1F>("neutralHadronsIsolation", "neutralHadronsIsolation", 100, 0, 100);
  TH1F* h_photonIsolation = analysisDir.make<TH1F>("photonIsolation", "photonIsolation", 100, 0, 15);

  TH1F* h_deltaPhi_passedID = analysisDir.make<TH1F>("deltaPhi_passedID", "deltaPhi", 40, M_PI / 2, M_PI);
  TH1F* h_ptPhoton_passedID = analysisDir.make<TH1F>("ptPhoton_passedID", "ptPhoton", 200, 5., 1000.);
  TH1F* h_ptFirstJet_passedID = analysisDir.make<TH1F>("ptFirstJet_passedID", "ptFirstJet", 200, 5., 1000.);
  TH1F* h_ptSecondJet_passedID = analysisDir.make<TH1F>("ptSecondJet_passedID", "ptSecondJet", 60, 0., 100.);
  TH1F* h_MET_passedID = analysisDir.make<TH1F>("MET_passedID", "MET", 150, 0., 300.);
  TH1F* h_rawMET_passedID = analysisDir.make<TH1F>("rawMET_passedID", "raw MET", 150, 0., 300.);
  TH1F* h_alpha_passedID = analysisDir.make<TH1F>("alpha_passedID", "alpha", 100, 0., 2.);

  TH1F* h_rho_passedID = analysisDir.make<TH1F>("rho_passedID", "rho", 100, 0, 50);
  TH1F* h_hadTowOverEm_passedID = analysisDir.make<TH1F>("hadTowOverEm_passedID", "hadTowOverEm", 100, 0, 0.05);
  TH1F* h_sigmaIetaIeta_passedID = analysisDir.make<TH1F>("sigmaIetaIeta_passedID", "sigmaIetaIeta", 100, 0, 0.011);
  TH1F* h_chargedHadronsIsolation_passedID = analysisDir.make<TH1F>("chargedHadronsIsolation_passedID", "chargedHadronsIsolation", 100, 0, 0.7);
  TH1F* h_neutralHadronsIsolation_passedID = analysisDir.make<TH1F>("neutralHadronsIsolation_passedID", "neutralHadronsIsolation", 100, 0, 100);
  TH1F* h_photonIsolation_passedID = analysisDir.make<TH1F>("photonIsolation_passedID", "photonIsolation", 100, 0, 15);

  TH2D* h_METvsfirstJet = analysisDir.make<TH2D>("METvsfirstJet", "MET vs firstJet", 150, 0., 300., 150, 0., 500.);
  TH2D* h_firstJetvsSecondJet = analysisDir.make<TH2D>("firstJetvsSecondJet", "firstJet vs secondJet", 60, 5., 100., 60, 5., 100.);

  // Balancing
  TFileDirectory balancingDir = analysisDir.mkdir("balancing");
  std::vector<std::vector<TH1F*> > responseBalancing = buildEtaPtVector<TH1F>(balancingDir, "resp_balancing", 150, 0., 2.);
  std::vector<std::vector<TH1F*> > responseBalancingRaw = buildEtaPtVector<TH1F>(balancingDir, "resp_balancing_raw", 150, 0., 2.);
  std::vector<std::vector<TH1F*> > responseBalancingGen;
  std::vector<std::vector<TH1F*> > responseBalancingRawGen;
  if (mIsMC) {
    responseBalancingGen = buildEtaPtVector<TH1F>(balancingDir, "resp_balancing_gen", 150, 0., 2.);
    responseBalancingRawGen = buildEtaPtVector<TH1F>(balancingDir, "resp_balancing_raw_gen", 150, 0., 2.);
  }

  std::vector<TH1F*> responseBalancingEta013 = buildPtVector<TH1F>(balancingDir, "resp_balancing", "eta013", 150, 0., 2.);
  std::vector<TH1F*> responseBalancingRawEta013 = buildPtVector<TH1F>(balancingDir, "resp_balancing_raw", "eta013", 150, 0., 2.);
  std::vector<TH1F*> responseBalancingGenEta013;
  std::vector<TH1F*> responseBalancingRawGenEta013;
  if (mIsMC) {
    responseBalancingGenEta013 = buildPtVector<TH1F>(balancingDir, "resp_balancing_gen", "eta013", 150, 0., 2.);
    responseBalancingRawGenEta013 = buildPtVector<TH1F>(balancingDir, "resp_balancing_raw_gen", "eta013", 150, 0., 2.);
  }
  std::vector<TH1F*> responseBalancingEta024 = buildPtVector<TH1F>(balancingDir, "resp_balancing", "eta024", 150, 0., 2.);

  // MPF
  TFileDirectory mpfDir = analysisDir.mkdir("mpf");
  std::vector<std::vector<TH1F*> > responseMPF = buildEtaPtVector<TH1F>(mpfDir, "resp_mpf", 150, 0., 2.);
  std::vector<std::vector<TH1F*> > responseMPFRaw = buildEtaPtVector<TH1F>(mpfDir, "resp_mpf_raw", 150, 0., 2.);
  std::vector<std::vector<TH1F*> > responseMPFGen;
  if (mIsMC) {
    responseMPFGen = buildEtaPtVector<TH1F>(mpfDir, "resp_mpf_gen", 150, 0., 2.);
  }

  std::vector<TH1F*> responseMPFEta013 = buildPtVector<TH1F>(mpfDir, "resp_mpf", "eta013", 150, 0., 2.);
  std::vector<TH1F*> responseMPFRawEta013 = buildPtVector<TH1F>(mpfDir, "resp_mpf_raw", "eta013", 150, 0., 2.);
  std::vector<TH1F*> responseMPFGenEta013;
  if (mIsMC) {
    responseMPFGenEta013 = buildPtVector<TH1F>(mpfDir, "resp_mpf_gen", "eta013", 150, 0., 2.);
  }
  std::vector<TH1F*> responseMPFEta024 = buildPtVector<TH1F>(mpfDir, "resp_mpf", "eta024", 150, 0., 2.);

  // vs number of vertices
  TFileDirectory vertexDir = analysisDir.mkdir("vertex");
  std::vector<std::vector<TH1F*>> vertex_responseBalancing = buildEtaVertexVector<TH1F>(vertexDir, "resp_balancing", 150, 0., 2.);
  std::vector<std::vector<TH1F*>> vertex_responseBalancingRaw = buildEtaVertexVector<TH1F>(vertexDir, "resp_balancing_raw", 150, 0., 2.);
  std::vector<TH1F*> vertex_responseBalancingEta013 = buildVertexVector<TH1F>(vertexDir, "resp_balancing", "eta013", 150, 0., 2.);
  std::vector<TH1F*> vertex_responseBalancingRawEta013 = buildVertexVector<TH1F>(vertexDir, "resp_balancing_raw", "eta013", 150, 0., 2.);

  std::vector<std::vector<TH1F*>> vertex_responseMPF = buildEtaVertexVector<TH1F>(vertexDir, "resp_mpf", 150, 0., 2.);
  std::vector<std::vector<TH1F*>> vertex_responseMPFRaw = buildEtaVertexVector<TH1F>(vertexDir, "resp_mpf_raw", 150, 0., 2.);
  std::vector<TH1F*> vertex_responseMPFEta013 = buildVertexVector<TH1F>(vertexDir, "resp_mpf", "eta013", 150, 0., 2.);
  std::vector<TH1F*> vertex_responseMPFRawEta013 = buildVertexVector<TH1F>(vertexDir, "resp_mpf_raw", "eta013", 150, 0., 2.);

  // Extrapolation
  int extrapolationBins = 50;
  double extrapolationMin = 0.;
  double extrapolationMax = 2.;
  TFileDirectory extrapDir = analysisDir.mkdir("extrapolation");
  ExtrapolationVectors<TH1F>::type extrap_responseBalancing = buildExtrapolationEtaVector<TH1F>(extrapDir, "extrap_resp_balancing", extrapolationBins, extrapolationMin, extrapolationMax);
  ExtrapolationVectors<TH1F>::type extrap_responseBalancingRaw = buildExtrapolationEtaVector<TH1F>(extrapDir, "extrap_resp_balancing_raw", extrapolationBins, extrapolationMin, extrapolationMax);
  std::vector<std::vector<TH1F*> > extrap_responseBalancingEta013 = buildExtrapolationVector<TH1F>(extrapDir, "extrap_resp_balancing", "eta013", extrapolationBins, extrapolationMin, extrapolationMax);
  std::vector<std::vector<TH1F*> > extrap_responseBalancingRawEta013 = buildExtrapolationVector<TH1F>(extrapDir, "extrap_resp_balancing_raw", "eta013", extrapolationBins, extrapolationMin, extrapolationMax);

  ExtrapolationVectors<TH1F>::type extrap_responseBalancingGen;
  ExtrapolationVectors<TH1F>::type extrap_responseBalancingRawGen;
  ExtrapolationVectors<TH1F>::type extrap_responseBalancingGenPhot;
  ExtrapolationVectors<TH1F>::type extrap_responseBalancingGenGamma;
  ExtrapolationVectors<TH1F>::type extrap_responseBalancingPhotGamma;

  std::vector<std::vector<TH1F*> > extrap_responseBalancingGenEta013;
  std::vector<std::vector<TH1F*> > extrap_responseBalancingRawGenEta013;
  std::vector<std::vector<TH1F*> > extrap_responseBalancingGenPhotEta013;
  std::vector<std::vector<TH1F*> > extrap_responseBalancingGenGammaEta013;
  std::vector<std::vector<TH1F*> > extrap_responseBalancingPhotGammaEta013;

  if (mIsMC) {
    extrap_responseBalancingGen = buildExtrapolationEtaVector<TH1F>(extrapDir, "extrap_resp_balancing_gen", extrapolationBins, extrapolationMin, extrapolationMax);
    extrap_responseBalancingRawGen = buildExtrapolationEtaVector<TH1F>(extrapDir, "extrap_resp_balancing_raw_gen", extrapolationBins, extrapolationMin, extrapolationMax);
    extrap_responseBalancingGenPhot = buildExtrapolationEtaVector<TH1F>(extrapDir, "extrap_resp_balancing_gen_phot", extrapolationBins, extrapolationMin, extrapolationMax);
    extrap_responseBalancingGenGamma = buildExtrapolationEtaVector<TH1F>(extrapDir, "extrap_resp_balancing_gen_gamma", extrapolationBins, extrapolationMin, extrapolationMax);
    extrap_responseBalancingPhotGamma = buildExtrapolationEtaVector<TH1F>(extrapDir, "extrap_resp_balancing_phot_gamma", extrapolationBins, extrapolationMin, extrapolationMax);

    extrap_responseBalancingGenEta013 = buildExtrapolationVector<TH1F>(extrapDir, "extrap_resp_balancing_gen", "eta013", extrapolationBins, extrapolationMin, extrapolationMax);
    extrap_responseBalancingRawGenEta013 = buildExtrapolationVector<TH1F>(extrapDir, "extrap_resp_balancing_raw_gen", "eta013", extrapolationBins, extrapolationMin, extrapolationMax);
    extrap_responseBalancingGenPhotEta013 = buildExtrapolationVector<TH1F>(extrapDir, "extrap_resp_balancing_gen_phot", "eta013", extrapolationBins, extrapolationMin, extrapolationMax);
    extrap_responseBalancingGenGammaEta013 = buildExtrapolationVector<TH1F>(extrapDir, "extrap_resp_balancing_gen_gamma", "eta013", extrapolationBins, extrapolationMin, extrapolationMax);
    extrap_responseBalancingPhotGammaEta013 = buildExtrapolationVector<TH1F>(extrapDir, "extrap_resp_balancing_phot_gamma", "eta013", extrapolationBins, extrapolationMin, extrapolationMax);
  }
  ExtrapolationVectors<TH1F>::type extrap_responseMPF = buildExtrapolationEtaVector<TH1F>(extrapDir, "extrap_resp_mpf", extrapolationBins, extrapolationMin, extrapolationMax);
  ExtrapolationVectors<TH1F>::type extrap_responseMPFRaw = buildExtrapolationEtaVector<TH1F>(extrapDir, "extrap_resp_mpf_raw", extrapolationBins, extrapolationMin, extrapolationMax);
  std::vector<std::vector<TH1F*> > extrap_responseMPFEta013 = buildExtrapolationVector<TH1F>(extrapDir, "extrap_resp_mpf", "eta013", extrapolationBins, extrapolationMin, extrapolationMax);
  std::vector<std::vector<TH1F*> > extrap_responseMPFRawEta013 = buildExtrapolationVector<TH1F>(extrapDir, "extrap_resp_mpf_raw", "eta013", extrapolationBins, extrapolationMin, extrapolationMax);

  ExtrapolationVectors<TH1F>::type extrap_responseMPFGen;
  std::vector<std::vector<TH1F*> > extrap_responseMPFGenEta013;
  if (mIsMC) {
    extrap_responseMPFGen = buildExtrapolationEtaVector<TH1F>(extrapDir, "extrap_resp_mpf_gen", extrapolationBins, extrapolationMin, extrapolationMax);
    extrap_responseMPFGenEta013 = buildExtrapolationVector<TH1F>(extrapDir, "extrap_resp_mpf_gen", "eta013", extrapolationBins, extrapolationMin, extrapolationMax);
  }
  
  // New extrapolation
  TFileDirectory newExtrapDir = analysisDir.mkdir("new_extrapolation");
  std::vector<std::shared_ptr<GaussianProfile>> new_extrap_responseBalancing = buildNewExtrapolationEtaVector(newExtrapDir, "extrap_resp_balancing", extrapolationBins, extrapolationMin, extrapolationMax);
  std::vector<std::shared_ptr<GaussianProfile>> new_extrap_responseBalancingRaw = buildNewExtrapolationEtaVector(newExtrapDir, "extrap_resp_balancing_raw", extrapolationBins, extrapolationMin, extrapolationMax);  
  std::shared_ptr<GaussianProfile> new_extrap_responseBalancingEta013 = buildNewExtrapolationVector(newExtrapDir, "extrap_resp_balancing", "eta013", extrapolationBins, extrapolationMin, extrapolationMax);
  std::shared_ptr<GaussianProfile> new_extrap_responseBalancingRawEta013 = buildNewExtrapolationVector(newExtrapDir, "extrap_resp_balancing_raw", "eta013", extrapolationBins, extrapolationMin, extrapolationMax);

  std::vector<std::shared_ptr<GaussianProfile>> new_extrap_responseMPF = buildNewExtrapolationEtaVector(newExtrapDir, "extrap_resp_mpf", extrapolationBins, extrapolationMin, extrapolationMax);
  std::vector<std::shared_ptr<GaussianProfile>> new_extrap_responseMPFRaw = buildNewExtrapolationEtaVector(newExtrapDir, "extrap_resp_mpf_raw", extrapolationBins, extrapolationMin, extrapolationMax);
  std::shared_ptr<GaussianProfile> new_extrap_responseMPFEta013 = buildNewExtrapolationVector(newExtrapDir, "extrap_resp_mpf", "eta013", extrapolationBins, extrapolationMin, extrapolationMax);
  std::shared_ptr<GaussianProfile> new_extrap_responseMPFRawEta013 = buildNewExtrapolationVector(newExtrapDir, "extrap_resp_mpf_raw", "eta013", extrapolationBins, extrapolationMin, extrapolationMax);

  // Viola
  std::vector<TH1F*> ptFirstJetEta024 = buildPtVector<TH1F>(analysisDir, "ptFirstJet", "eta024", 500, 5., 1005.);

  // Luminosity
  if (! mIsMC) {
    // For data, there's only one file, so open it in order to read the luminosity
    TFile* f = TFile::Open(mInputFiles[0].c_str());
    analysisDir.make<TParameter<double>>("luminosity", static_cast<TParameter<double>*>(f->Get("gammaJet/total_luminosity"))->GetVal());
    f->Close();
    delete f;
  }

  // Store alpha cut
  analysisDir.make<TParameter<double>>("alpha_cut", mAlphaCut);

  uint64_t totalEvents = photonChain.GetEntries();
  uint64_t passedEvents = 0;
  uint64_t passedEventsFromTriggers = 0;
  uint64_t rejectedEventsFromTriggers = 0;
  uint64_t rejectedEventsTriggerNotFound = 0;
  uint64_t rejectedEventsPtOut = 0;

  uint64_t passedPhotonJetCut = 0;
  uint64_t passedDeltaPhiCut = 0;
  uint64_t passedPixelSeedVetoCut = 0;
  uint64_t passedMuonsCut = 0;
  uint64_t passedElectronsCut = 0;
  uint64_t passedAlphaCut = 0;

  uint64_t from = 0;
  uint64_t to = totalEvents;

  if (mIsBatchJob) {
    // Compute new from / to index
    uint64_t eventsPerJob = totalEvents / mTotalJobs;
    from = mCurrentJob * eventsPerJob;
    to = (mCurrentJob == (mTotalJobs - 1)) ? totalEvents : (mCurrentJob + 1) * eventsPerJob;

    std::cout << "Batch mode: running from " << from << " (included) to " << to << " (excluded)" << std::endl;
  }

  for (uint64_t i = from; i < to; i++) {

    if ((i - from) % 50000 == 0) {
      std::cout << "Processing event #" << (i - from + 1) << " of " << (to - from) << " (" << (float) (i - from) / (to - from) * 100 << "%)" << std::endl;
    }

    if (EXIT) {
      break;
    }

    analysis.GetEntry(i);
    photon.GetEntry(i);
    genPhoton.GetEntry(i);
    muons.GetEntry(i);
    electrons.GetEntry(i);

    firstJet.GetEntry(i);
    firstRawJet.GetEntry(i);
    firstGenJet.GetEntry(i);

    secondJet.GetEntry(i);
    secondRawJet.GetEntry(i);
    secondGenJet.GetEntry(i);

    MET.GetEntry(i);
    genMET.GetEntry(i);
    rawMET.GetEntry(i);

    misc.GetEntry(i);

    if (! photon.is_present || ! firstJet.is_present)
      continue;

    passedPhotonJetCut++;

    /*
    {
      // DEBUG
      double deltaPhi = fabs(reco::deltaPhi(photon.phi, firstJet.phi));
      std::cout << firstJet.pt << " " << firstJet.phi << " " << photon.pt << " " << photon.phi << " " << deltaPhi << std::endl;
    }
    */

    if (jetCorrector) {
      // jetCorrector isn't null. Correct raw jet with jetCorrector and rebuild the corrected jet
      jetCorrector->setJetEta(firstRawJet.eta);
      jetCorrector->setJetPt(firstRawJet.pt);
      jetCorrector->setRho(misc.rho);
      jetCorrector->setJetA(firstRawJet.jet_area);
      jetCorrector->setNPV(analysis.nvertex);

      double correction = jetCorrector->getCorrection();
      firstJet.pt = firstRawJet.pt * correction;

      jetCorrector->setJetEta(secondRawJet.eta);
      jetCorrector->setJetPt(secondRawJet.pt);
      jetCorrector->setRho(misc.rho);
      jetCorrector->setJetA(secondRawJet.jet_area);
      jetCorrector->setNPV(analysis.nvertex);

      correction = jetCorrector->getCorrection();
      secondJet.pt = secondRawJet.pt * correction;
    }

    int checkTriggerResult = 0;
    std::string passedTrigger;
    float triggerWeight = 1.;
    if ((checkTriggerResult = checkTrigger(passedTrigger, triggerWeight)) != TRIGGER_OK) {
      switch (checkTriggerResult) {
        case TRIGGER_NOT_FOUND:
          if (mVerbose) {
            std::cout << MAKE_RED << "[Run #" << analysis.run << "] Event does not pass required trigger. List of passed triggers: " << RESET_COLOR << std::endl;
            size_t size = analysis.trigger_names->size();
            for (size_t i = 0; i < size; i++) {
              if (analysis.trigger_results->at(i)) {
                std::cout << "\t" << analysis.trigger_names->at(i) << std::endl;
              }
            }
          }
          rejectedEventsTriggerNotFound++;
          break;
        case TRIGGER_FOUND_BUT_PT_OUT:
          /*bool contains250 = false;
          size_t size = analysis.trigger_names->size();
          for (size_t i = 0; i < size; i++) {
            if (analysis.trigger_results->at(i) && TString(analysis.trigger_names->at(i)).Contains("HLT_Photon250")) {
              contains250 = true;
              break;
            }
          }*/
          //if (contains250) {
          if (mVerbose) {
            std::cout << MAKE_RED << "[Run #" << analysis.run << ", pT: " << photon.pt << "] Event does pass required trigger, but pT is out of range. List of passed triggers: " << RESET_COLOR << std::endl;
            size_t size = analysis.trigger_names->size();
            for (size_t i = 0; i < size; i++) {
              if (analysis.trigger_results->at(i)) {
                std::cout << "\t" << analysis.trigger_names->at(i) <<  std::endl;
              }
            }
          }
          rejectedEventsPtOut++;
          break;
      }

      rejectedEventsFromTriggers++;
      continue;
    }
    passedEventsFromTriggers++;

    //if (analysis.nvertex >= 21)
    //  continue;
    
    if (mIsMC) {
      passedTrigger = cleanTriggerName(passedTrigger);
      computePUWeight(passedTrigger);
      triggerWeight = 1.;
    } else {
      triggerWeight = 1. / triggerWeight;
    }

    double generatorWeight = (mIsMC) ? analysis.generator_weight : 1.;
    if (generatorWeight == 0.)
      generatorWeight = 1.;
    
    double eventWeight = (mIsMC) ? mPUWeight * analysis.event_weight * generatorWeight : triggerWeight;
#if ADD_TREES
    double oldAnalysisWeight = analysis.event_weight;
    analysis.event_weight = eventWeight;
#endif

#if ADD_TREES
    if (mUncutTrees) {
      photonTree->Fill();
      genPhotonTree->Fill();
      firstJetTree->Fill();
      firstGenJetTree->Fill();
      firstRawJetTree->Fill();
      secondJetTree->Fill();
      secondGenJetTree->Fill();
      secondRawJetTree->Fill();
      metTree->Fill();
      rawMetTree->Fill();
      genMetTree->Fill();
      electronsTree->Fill();
      muonsTree->Fill();
      analysisTree->Fill();
    }
#endif


    // Event selection
    // The photon is good from previous step
    // From previous step, we have fabs(deltaPhi(photon, firstJet)) > PI/2
    double deltaPhi = fabs(reco::deltaPhi(photon.phi, firstJet.phi));

    bool isBack2Back = (deltaPhi >= DELTAPHI_CUT);
    if (! isBack2Back) {
      continue;
    }

    passedDeltaPhiCut++;

    // Pixel seed veto
    if (photon.has_pixel_seed)
      continue;

    passedPixelSeedVetoCut++;

    // No muons
    if (muons.n != 0)
      continue;

    passedMuonsCut++;

    // Electron veto. No electron close to the photon
    bool keepEvent = true;
    for (int j = 0; j < electrons.n; j++) {
      double deltaR = fabs(reco::deltaR(photon.eta, photon.phi, electrons.eta[j], electrons.phi[j]));
      if (deltaR < 0.13) {
        keepEvent = false;
        break;
      }
    }

    if (! keepEvent)
      continue;

    passedElectronsCut++;

    /*
    if (firstJet.pt < 12)
      continue;
    */

    //bool secondJetOK = !secondJet.is_present || (secondJet.pt < mAlphaCut * photon.pt);
    bool secondJetOK = !secondJet.is_present || (secondJet.pt < 10 || secondJet.pt < mAlphaCut * photon.pt);

    if (mDoMCComparison) {
      // Lowest unprescaled trigger for 2012 if at 150 GeV
      if (photon.pt < 165)
        continue;
    }

    if (secondJetOK)
      passedAlphaCut++;

#if ADD_TREES
    h_nvertex->Fill(analysis.nvertex, oldAnalysisWeight);
#else
    h_nvertex->Fill(analysis.nvertex, analysis.event_weight);
#endif

    h_nvertex_reweighted->Fill(analysis.nvertex, eventWeight);

    double deltaPhi_2ndJet = fabs(reco::deltaPhi(secondJet.phi, photon.phi));
    h_deltaPhi->Fill(deltaPhi, eventWeight);
    h_deltaPhi_2ndJet->Fill(deltaPhi_2ndJet, eventWeight); 
    h_ptPhoton->Fill(photon.pt, eventWeight);
    h_ptFirstJet->Fill(firstJet.pt, eventWeight);
    h_ptSecondJet->Fill(secondJet.pt, eventWeight);
    h_MET->Fill(MET.pt, eventWeight);
    h_alpha->Fill(secondJet.pt / photon.pt, eventWeight);

    h_rho->Fill(photon.rho, eventWeight);
    h_hadTowOverEm->Fill(photon.hadTowOverEm, eventWeight);
    h_sigmaIetaIeta->Fill(photon.sigmaIetaIeta, eventWeight);
    h_chargedHadronsIsolation->Fill(photon.chargedHadronsIsolation, eventWeight);
    h_neutralHadronsIsolation->Fill(photon.neutralHadronsIsolation, eventWeight);
    h_photonIsolation->Fill(photon.photonIsolation, eventWeight);

    // Dump to Tree
    /*photonToTree(photon);
      firstJetToTree(firstJet);
      if (secondJet.is_present) {
      secondJetToTree(*secondJet);
      }*/

    // Compute values
    // MPF
    float deltaPhi_Photon_MET = reco::deltaPhi(photon.phi, MET.phi);
    respMPF = 1. + MET.et * photon.pt * cos(deltaPhi_Photon_MET) / (photon.pt * photon.pt);

    float deltaPhi_Photon_MET_gen = reco::deltaPhi(genPhoton.phi, genMET.phi);
    float respMPFGen = 1. + genMET.et * genPhoton.pt * cos(deltaPhi_Photon_MET_gen) / (genPhoton.pt * genPhoton.pt);

    float deltaPhi_Photon_MET_raw = reco::deltaPhi(photon.phi, rawMET.phi);
    float respMPFRaw = 1. + rawMET.et * photon.pt * cos(deltaPhi_Photon_MET_raw) / (photon.pt * photon.pt);

    // Balancing
    respBalancing = firstJet.pt / photon.pt;
    respBalancingGen = firstJet.pt / firstGenJet.pt;
    respBalancingRaw = firstRawJet.pt / photon.pt;
    respBalancingRawGen = firstRawJet.pt / firstGenJet.pt;

    // For DATA/MC comparison
    respGenPhoton = firstGenJet.pt / photon.pt;
    respGenGamma = firstGenJet.pt / genPhoton.pt;
    respPhotonGamma = photon.pt / genPhoton.pt;

    int ptBin = mPtBinning.getPtBin(photon.pt);
    if (ptBin < 0) {
      //std::cout << "Photon pt " << photon.pt() << " is not covered by our pt binning. Dumping event." << std::endl;
      continue;
    }
    int ptBinGen = mPtBinning.getPtBin(genPhoton.pt);

    int etaBin = mEtaBinning.getBin(firstJet.eta);
    int etaBinGen = mEtaBinning.getBin(firstGenJet.eta);

    int vertexBin = mVertexBinning.getVertexBin(analysis.nvertex);

    if (secondJet.is_present) {
      do {
        int extrapBin = mExtrapBinning.getBin(photon.pt, secondJet.pt, ptBin);
        int rawExtrapBin = extrapBin; // mExtrapBinning.getBin(photon.pt, secondRawJet.pt, ptBin); // We don't want that

        float r_RecoPhot = firstJet.pt / photon.pt;
        float r_RecoGen  = firstJet.pt / firstGenJet.pt;
        float r_GenPhot  = firstGenJet.pt / photon.pt;
        float r_GenGamma  = firstGenJet.pt / genPhoton.pt;
        float r_PhotGamma  = photon.pt / genPhoton.pt;

        do {
          if (extrapBin < 0) {
            //std::cout << "No bin found for extrapolation: " << secondJet.pt / photon.pt << std::endl;
            break;
          }

          // Special case

          if (fabs(firstJet.eta) < 1.3) {
            extrap_responseBalancingEta013[ptBin][extrapBin]->Fill(r_RecoPhot, eventWeight);
            extrap_responseMPFEta013[ptBin][extrapBin]->Fill(respMPF, eventWeight);

            if (mIsMC && ptBinGen >= 0 && etaBinGen >= 0) {
              extrap_responseBalancingGenEta013[ptBinGen][extrapBin]->Fill(r_RecoGen, eventWeight);
              extrap_responseBalancingGenPhotEta013[ptBinGen][extrapBin]->Fill(r_GenPhot, eventWeight);
              extrap_responseBalancingGenGammaEta013[ptBinGen][extrapBin]->Fill(r_GenGamma, eventWeight);
              extrap_responseBalancingPhotGammaEta013[ptBinGen][extrapBin]->Fill(r_PhotGamma, eventWeight);
              extrap_responseMPFGenEta013[ptBinGen][extrapBin]->Fill(respMPFGen, eventWeight);
            }
          }

          if (etaBin < 0)
            break;

          extrap_responseBalancing[etaBin][ptBin][extrapBin]->Fill(r_RecoPhot, eventWeight);
          extrap_responseMPF[etaBin][ptBin][extrapBin]->Fill(respMPF, eventWeight);

          if (mIsMC && ptBinGen >= 0 && etaBinGen >= 0) {
            extrap_responseBalancingGen[etaBinGen][ptBinGen][extrapBin]->Fill(r_RecoGen, eventWeight);
            extrap_responseBalancingGenPhot[etaBinGen][ptBinGen][extrapBin]->Fill(r_GenPhot, eventWeight);
            extrap_responseBalancingGenGamma[etaBinGen][ptBinGen][extrapBin]->Fill(r_GenGamma, eventWeight);
            extrap_responseBalancingPhotGamma[etaBinGen][ptBinGen][extrapBin]->Fill(r_PhotGamma, eventWeight);
            extrap_responseMPFGen[etaBinGen][ptBinGen][extrapBin]->Fill(respMPFGen, eventWeight);
          }
        } while (false);

        do {

          if (rawExtrapBin < 0) {
            //std::cout << "No bin found for extrapolation: " << secondJet.pt / photon.pt << std::endl;
            break;
          }

          float r_RecoPhotRaw = firstRawJet.pt / photon.pt;
          float r_RecoGenRaw  = firstRawJet.pt / firstGenJet.pt;

          // Special case

          if (fabs(firstJet.eta) < 1.3) {
            extrap_responseBalancingRawEta013[ptBin][rawExtrapBin]->Fill(r_RecoPhotRaw, eventWeight);
            extrap_responseMPFRawEta013[ptBin][rawExtrapBin]->Fill(respMPFRaw, eventWeight);

            if (mIsMC && ptBinGen >= 0 && etaBinGen >= 0) {
              extrap_responseBalancingRawGenEta013[ptBinGen][rawExtrapBin]->Fill(r_RecoGenRaw, eventWeight);
            }
          }

          if (etaBin < 0)
            break;

          extrap_responseBalancingRaw[etaBin][ptBin][rawExtrapBin]->Fill(r_RecoPhotRaw, eventWeight);
          extrap_responseMPFRaw[etaBin][ptBin][rawExtrapBin]->Fill(respMPFRaw, eventWeight);

          if (mIsMC && ptBinGen >= 0 && etaBinGen >= 0) {
            extrap_responseBalancingRawGen[etaBinGen][ptBinGen][rawExtrapBin]->Fill(r_RecoGenRaw, eventWeight);
          }
        } while (false);

      } while (false);

      // New extrapolation
      do {

        // Cut on photon pt. The first two bins are too low stats for beeing usefull
        if (photon.pt < 165)
          break;

        float r_RecoPhot = firstJet.pt / photon.pt;
        float r_RecoPhotRaw = firstRawJet.pt / photon.pt;
        float alpha = secondJet.pt / photon.pt;
        float raw_alpha = alpha; // secondRawJet.pt / photon.pt; // We don't want that

        // Special case
        if (fabs(firstJet.eta) < 1.3) {
          new_extrap_responseBalancingEta013->fill(alpha, r_RecoPhot, eventWeight);
          new_extrap_responseBalancingRawEta013->fill(raw_alpha, r_RecoPhotRaw, eventWeight);
          new_extrap_responseMPFEta013->fill(alpha, respMPF, eventWeight);
          new_extrap_responseMPFRawEta013->fill(raw_alpha, respMPFRaw, eventWeight);
        }

        if (etaBin < 0)
          break;

        new_extrap_responseBalancing[etaBin]->fill(alpha, r_RecoPhot, eventWeight);
        new_extrap_responseBalancingRaw[etaBin]->fill(raw_alpha, r_RecoPhotRaw, eventWeight);
        new_extrap_responseMPF[etaBin]->fill(alpha, respMPF, eventWeight);
        new_extrap_responseMPFRaw[etaBin]->fill(raw_alpha, respMPFRaw, eventWeight);


      } while (false);
    }

    if (secondJetOK) {

      do {
        h_deltaPhi_passedID->Fill(deltaPhi, eventWeight);
        h_ptPhoton_passedID->Fill(photon.pt, eventWeight);
        h_ptFirstJet_passedID->Fill(firstJet.pt, eventWeight);
        h_ptSecondJet_passedID->Fill(secondJet.pt, eventWeight);
        h_MET_passedID->Fill(MET.et, eventWeight);
        h_rawMET_passedID->Fill(rawMET.et, eventWeight);
        h_alpha_passedID->Fill(secondJet.pt / photon.pt, eventWeight);

        h_METvsfirstJet->Fill(MET.et, firstJet.pt, eventWeight);
        h_firstJetvsSecondJet->Fill(firstJet.pt, secondJet.pt, eventWeight);

        h_rho_passedID->Fill(photon.rho, eventWeight);
        h_hadTowOverEm_passedID->Fill(photon.hadTowOverEm, eventWeight);
        h_sigmaIetaIeta_passedID->Fill(photon.sigmaIetaIeta, eventWeight);
        h_chargedHadronsIsolation_passedID->Fill(photon.chargedHadronsIsolation, eventWeight);
        h_neutralHadronsIsolation_passedID->Fill(photon.neutralHadronsIsolation, eventWeight);
        h_photonIsolation_passedID->Fill(photon.photonIsolation, eventWeight);

        // Special case
        if (fabs(firstJet.eta) < 1.3) {
          responseBalancingEta013[ptBin]->Fill(respBalancing, eventWeight);
          responseBalancingRawEta013[ptBin]->Fill(respBalancingRaw, eventWeight);

          responseMPFEta013[ptBin]->Fill(respMPF, eventWeight);
          responseMPFRawEta013[ptBin]->Fill(respMPFRaw, eventWeight);

          if (vertexBin >= 0) {
            vertex_responseBalancingEta013[vertexBin]->Fill(respBalancing, eventWeight);
            vertex_responseBalancingRawEta013[vertexBin]->Fill(respBalancingRaw, eventWeight);

            vertex_responseMPFEta013[vertexBin]->Fill(respMPF, eventWeight);
            vertex_responseMPFRawEta013[vertexBin]->Fill(respMPF, eventWeight);
          }

          if (mIsMC && ptBinGen >= 0) {
            responseBalancingGenEta013[ptBinGen]->Fill(respBalancingGen, eventWeight);
            responseBalancingRawGenEta013[ptBinGen]->Fill(respBalancingRawGen, eventWeight);

            responseMPFGenEta013[ptBinGen]->Fill(respMPFGen, eventWeight);
          }
        }

        if (fabs(firstJet.eta) < 2.4 && (fabs(firstJet.eta) < 1.4442 || fabs(firstJet.eta) > 1.5560)){ 
          // Viola
          ptFirstJetEta024[ptBin]->Fill(firstJet.pt, eventWeight);

          responseBalancingEta024[ptBin]->Fill(respBalancing, eventWeight);
          responseMPFEta024[ptBin]->Fill(respMPF, eventWeight);
        }

        if (etaBin < 0) {
          //std::cout << "Jet eta " << firstJet.eta() << " is not covered by our eta binning. Dumping event." << std::endl;
          break;
        }


        responseBalancing[etaBin][ptBin]->Fill(respBalancing, eventWeight);
        responseBalancingRaw[etaBin][ptBin]->Fill(respBalancingRaw, eventWeight);

        responseMPF[etaBin][ptBin]->Fill(respMPF, eventWeight);
        responseMPFRaw[etaBin][ptBin]->Fill(respMPFRaw, eventWeight);

        if (vertexBin >= 0) {
          vertex_responseBalancing[etaBin][vertexBin]->Fill(respBalancing, eventWeight);
          vertex_responseBalancingRaw[etaBin][vertexBin]->Fill(respBalancingRaw, eventWeight);

          vertex_responseMPF[etaBin][vertexBin]->Fill(respMPF, eventWeight);
          vertex_responseMPFRaw[etaBin][vertexBin]->Fill(respMPF, eventWeight);
        }

        // Gen values
        if (mIsMC && ptBinGen >= 0 && etaBinGen >= 0) {
          responseBalancingGen[etaBinGen][ptBinGen]->Fill(respBalancingGen, eventWeight);
          responseBalancingRawGen[etaBinGen][ptBinGen]->Fill(respBalancingRawGen, eventWeight);

          responseMPFGen[etaBinGen][ptBinGen]->Fill(respMPFGen, eventWeight);
        }
      } while (false);

#if ADD_TREES
      if (! mUncutTrees) {
        photonTree->Fill();
        genPhotonTree->Fill();
        firstJetTree->Fill();
        firstGenJetTree->Fill();
        firstRawJetTree->Fill();
        secondJetTree->Fill();
        secondGenJetTree->Fill();
        secondRawJetTree->Fill();
        metTree->Fill();
        rawMetTree->Fill();
        genMetTree->Fill();
        electronsTree->Fill();
        muonsTree->Fill();
        analysisTree->Fill();
      }
#endif

      passedEvents++;
    }
  }

  std::cout << "Selection efficiency: " << MAKE_RED << (double) passedEvents / (to - from) * 100 << "%" << RESET_COLOR << std::endl;
  std::cout << "Efficiency for photon/jet cut: " << MAKE_RED << (double) passedPhotonJetCut / (to - from) * 100 << "%" << RESET_COLOR << std::endl;
  std::cout << "Selection efficiency for trigger selection: " << MAKE_RED << (double) passedEventsFromTriggers / (to - from) * 100 << "%" << RESET_COLOR << std::endl;
  std::cout << "Efficiency for Δφ cut: " << MAKE_RED << (double) passedDeltaPhiCut / (to - from) * 100 << "%" << RESET_COLOR << std::endl;
  std::cout << "Efficiency for pixel seed veto cut: " << MAKE_RED << (double) passedPixelSeedVetoCut / (to - from) * 100 << "%" << RESET_COLOR << std::endl;
  std::cout << "Efficiency for muons cut: " << MAKE_RED << (double) passedMuonsCut / (to - from) * 100 << "%" << RESET_COLOR << std::endl;
  std::cout << "Efficiency for electrons cut: " << MAKE_RED << (double) passedElectronsCut / (to - from) * 100 << "%" << RESET_COLOR << std::endl;
  std::cout << "Efficiency for α cut: " << MAKE_RED << (double) passedAlphaCut / (to - from) * 100 << "%" << RESET_COLOR << std::endl;

  std::cout << std::endl;
  std::cout << "Rejected events because trigger was not found: " << MAKE_RED << (double) rejectedEventsTriggerNotFound / (rejectedEventsFromTriggers) * 100 << "%" << RESET_COLOR << std::endl;
  std::cout << "Rejected events because trigger was found but pT was out of range: " << MAKE_RED << (double) rejectedEventsPtOut / (rejectedEventsFromTriggers) * 100 << "%" << RESET_COLOR << std::endl;
}

void GammaJetFinalizer::doSecondJetExtrapolation() {

  // TODO: Extrapolation

}

template<typename T>
std::vector<T*> GammaJetFinalizer::buildPtVector(TFileDirectory dir, const std::string& branchName, const std::string& etaName, int nBins, double xMin, double xMax) {

  std::vector<T*> vector;
  size_t ptBinningSize = mPtBinning.size();
  for (size_t j = 0; j < ptBinningSize; j++) {

    const std::pair<float, float> bin = mPtBinning.getBinValue(j);
    std::stringstream ss;
    ss << branchName << "_" << etaName << "_ptPhot_" << (int) bin.first << "_" << (int) bin.second;

    T* object = dir.make<T>(ss.str().c_str(), ss.str().c_str(), nBins, xMin, xMax);
    vector.push_back(object);
  }

  return vector;
}

template<typename T>
std::vector<std::vector<T*> > GammaJetFinalizer::buildEtaPtVector(TFileDirectory dir, const std::string& branchName, int nBins, double xMin, double xMax) {
  size_t etaBinningSize = mEtaBinning.size();
  std::vector<std::vector<T*> > etaBinning;

  for (size_t i = 0; i < etaBinningSize; i++) {
    const std::string etaName = mEtaBinning.getBinName(i);
    etaBinning.push_back(buildPtVector<T>(dir, branchName, etaName, nBins, xMin, xMax));
  }

  return etaBinning;
}

template<typename T>
std::vector<T*> GammaJetFinalizer::buildVertexVector(TFileDirectory dir, const std::string& branchName, const std::string& etaName, int nBins, double xMin, double xMax) {

  std::vector<T*> vector;
  size_t vertexBinningSize = mVertexBinning.size();
  for (size_t j = 0; j < vertexBinningSize; j++) {

    const std::pair<int, int> bin = mVertexBinning.getBinValue(j);
    std::stringstream ss;
    ss << branchName << "_" << etaName << "_nvertex_" << bin.first << "_" << bin.second;

    T* object = dir.make<T>(ss.str().c_str(), ss.str().c_str(), nBins, xMin, xMax);
    vector.push_back(object);
  }

  return vector;
}

template<typename T>
std::vector<std::vector<T*> > GammaJetFinalizer::buildEtaVertexVector(TFileDirectory dir, const std::string& branchName, int nBins, double xMin, double xMax) {
  size_t etaBinningSize = mEtaBinning.size();
  std::vector<std::vector<T*> > etaBinning;

  for (size_t i = 0; i < etaBinningSize; i++) {
    const std::string etaName = mEtaBinning.getBinName(i);
    etaBinning.push_back(buildVertexVector<T>(dir, branchName, etaName, nBins, xMin, xMax));
  }

  return etaBinning;
}

template<typename T>
std::vector<std::vector<T*> > GammaJetFinalizer::buildExtrapolationVector(TFileDirectory dir, const std::string& branchName, const std::string& etaName, int nBins, double xMin, double xMax) {

  std::vector<std::vector<T*> > vector;
  size_t ptBinningSize = mPtBinning.size();
  for (size_t j = 0; j < ptBinningSize; j++) {

    const std::pair<float, float> bin = mPtBinning.getBinValue(j);
    std::stringstream ss;
    ss << branchName << "_" << etaName;

    TString subDirectoryName = TString::Format("extrap_ptPhot_%d_%d", (int) bin.first, (int) bin.second);
    TFileDirectory subDir = dir.mkdir(subDirectoryName.Data());

    std::vector<T*> subvector;
    size_t extrapBinningSize = mExtrapBinning.size();
    for (size_t p = 0; p < extrapBinningSize; p++) {
      TString name = TString::Format("%s_%d", ss.str().c_str(), (int) p);

      T* object = subDir.make<T>(name, name, nBins, xMin, xMax);
      subvector.push_back(object);
    }

    vector.push_back(subvector);
  }

  return vector;
}

template<typename T>
std::vector<std::vector<std::vector<T*> > > GammaJetFinalizer::buildExtrapolationEtaVector(TFileDirectory dir, const std::string& branchName, int nBins, double xMin, double xMax) {

  size_t etaBinningSize = mEtaBinning.size();
  std::vector<std::vector<std::vector<T*> > > etaBinning;

  for (size_t i = 0; i < etaBinningSize; i++) {
    const std::string etaName = mEtaBinning.getBinName(i);

    std::vector<std::vector<T*> > vector = buildExtrapolationVector<T>(dir, branchName, etaName, nBins, xMin, xMax);
    etaBinning.push_back(vector);
  }

  return etaBinning;
}

std::shared_ptr<GaussianProfile> GammaJetFinalizer::buildNewExtrapolationVector(TFileDirectory dir, const std::string& branchName, const std::string& etaName, int nBins, double xMin, double xMax) {

  std::stringstream ss;
  ss << branchName << "_" << etaName;

  std::shared_ptr<GaussianProfile> object(new GaussianProfile(ss.str(), mNewExtrapBinning.size(), 0, mNewExtrapBinning.size() * mNewExtrapBinning.getBinWidth(), nBins, xMin, xMax));
  object->setPrefix("alpha");
  object->initialize(dir);

  return object;
}

std::vector<std::shared_ptr<GaussianProfile>> GammaJetFinalizer::buildNewExtrapolationEtaVector(TFileDirectory dir, const std::string& branchName, int nBins, double xMin, double xMax) {

  size_t etaBinningSize = mEtaBinning.size();
  std::vector<std::shared_ptr<GaussianProfile>> etaBinning;

  for (size_t i = 0; i < etaBinningSize; i++) {
    const std::string etaName = mEtaBinning.getBinName(i);
    

    std::shared_ptr<GaussianProfile> object = buildNewExtrapolationVector(dir, branchName, etaName, nBins, xMin, xMax);
    etaBinning.push_back(object);
  }

  return etaBinning;
}

std::string GammaJetFinalizer::cleanTriggerName(const std::string& trigger) {
  static boost::regex r(R"(_\.\*|\.\*)");
  return boost::regex_replace(trigger, r, "");
}

void GammaJetFinalizer::computePUWeight(const std::string& passedTrigger) {
  static std::string cmsswBase = getenv("CMSSW_BASE");
  static std::string puPrefix = TString::Format("%s/src/JetMETCorrections/GammaJetFilter/analysis/PUReweighting", cmsswBase.c_str()).Data();
  static std::string puMC = TString::Format("%s/summer12_computed_mc_%s_pu_truth_75bins.root", puPrefix.c_str(), mDatasetName.c_str()).Data();
  std::string puData = TString::Format("%s/pu_truth_data_photon_2012_true_%s_75bins.root", puPrefix.c_str(), passedTrigger.c_str()).Data();
  //std::string puData = TString::Format("%s/pu_truth_data_photon_2012_true_75bins.root", puPrefix.c_str()).Data();

  if (mNoPUReweighting)
    return;

  if (! mLumiReweighting.count(passedTrigger)) {

    /*if (! boost::filesystem::exists(puMC)) {
      std::cout << "Warning: " << MAKE_RED << "pileup histogram for MC was not found. No PU reweighting." << RESET_COLOR << std::endl;
      std::cout << "File missing: " << puMC << std::endl;
      mNoPUReweighting = true;
      mPUWeight = 1.;
      return;
    } else {*/
      std::cout << MAKE_BLUE << "Create PU reweighting profile for " << passedTrigger << RESET_COLOR << std::endl;
      mLumiReweighting[passedTrigger] = boost::shared_ptr<PUReweighter>(new PUReweighter(puData/*, puMC*/));
    /*}*/

  }

  mPUWeight = mLumiReweighting[passedTrigger]->weight(analysis.ntrue_interactions);
}

void GammaJetFinalizer::checkInputFiles() {
  for (std::vector<std::string>::iterator it = mInputFiles.begin(); it != mInputFiles.end();) {
    TFile* f = TFile::Open(it->c_str());
    if (! f) {
      std::cerr << "Error: can't open '" << it->c_str() << "'. Removed from input files." << std::endl;
      it = mInputFiles.erase(it);
      continue;
    }

    TTree* analysis = static_cast<TTree*>(f->Get("gammaJet/analysis"));
    if (! analysis || analysis->GetEntry(0) == 0) {
      std::cerr << "Error: Trees inside '" << it->c_str() << "' were empty. Removed from input files." << std::endl;
      it = mInputFiles.erase(it);

      f->Close();
      delete f;

      continue;
    }

    f->Close();
    delete f;

    ++it;
  }
}

int GammaJetFinalizer::checkTrigger(std::string& passedTrigger, float& weight) {

  if (! mIsMC) {
    const PathVector& mandatoryTriggers = mTriggers->getTriggers(analysis.run);

    // Method 2:
    // - With the photon p_t, find the trigger it should pass
    // - Then, look on trigger list if it pass it or not (only for data)

    //if (! mIsMC) {

    const PathData* mandatoryTrigger = nullptr;
    for (auto& path: mandatoryTriggers) {
      if (path.second.range.in(photon.pt)) {
        mandatoryTrigger = &path;
      }
    }

    if (!mandatoryTrigger)
      return TRIGGER_NOT_FOUND;

    weight = mandatoryTrigger->second.weight;

    // This photon must pass mandatoryTrigger.first
    size_t size = analysis.trigger_names->size();
    for (int i = size - 1; i >= 0; i--) {
      bool passed = analysis.trigger_results->at(i);
      if (! passed)
        continue;

      if (boost::regex_match(analysis.trigger_names->at(i), mandatoryTrigger->first)) {
        passedTrigger = mandatoryTrigger->first.str();
        return TRIGGER_OK;
      }
    }
  } else {
    const std::map<Range<float>, std::vector<MCTrigger>>& triggers = mMCTriggers->getTriggers();

    const std::vector<MCTrigger>* mandatoryTrigger = nullptr;
    for (auto& path: triggers) {
      if (path.first.in(photon.pt)) {
        mandatoryTrigger = &path.second;
      }
    }

    if (!mandatoryTrigger)
      return TRIGGER_NOT_FOUND;

    weight = 1;

    if (mandatoryTrigger->size() > 1) {
      double random = mRandomGenerator.Rndm();

      double weight_low = 0;
      double weight_high = 0;
      for (int32_t i = 0; i < (int32_t) mandatoryTrigger->size(); i++) {
        if (i - 1 >= 0)
          weight_low += (*mandatoryTrigger)[i - 1].weight;
        weight_high += (*mandatoryTrigger)[i].weight;

        if (random > weight_low && random <= weight_high) {
          passedTrigger = (*mandatoryTrigger)[i].name.str();
          return TRIGGER_OK;
        }
      }

      throw new std::exception(); // This should NEVER happened
    }

    passedTrigger = mandatoryTrigger->at(0).name.str();
    return TRIGGER_OK;
  }

  //} else {

    //// Method 1
    //size_t size = analysis.trigger_names->size();
    //for (int i = size - 1; i >= 0; i--) {
      //bool passed = analysis.trigger_results->at(i);
      //if (! passed)
        //continue;

      //for (const PathData& mandatoryTrigger: mandatoryTriggers) {
        //if (boost::regex_match(analysis.trigger_names->at(i), mandatoryTrigger.first)) {
          //passedTrigger = mandatoryTrigger.first.str();
          //weight = mandatoryTrigger.second.weight;
          //return TRIGGER_OK;
        //}
      //}
    //}
  //}

  return TRIGGER_NOT_FOUND;
}

std::vector<std::string> readInputFiles(const std::string& list) {
  std::ifstream f(list.c_str());
  std::string line;
  std::vector<std::string> files;
  while (std::getline(f, line)) {
    boost::algorithm::trim(line);
    if (line.length() == 0 || line[0] == '#')
      continue;

    files.push_back(line);
  }

  if (files.size() == 0) {
    throw new TCLAP::ArgException("No input files found in " + list);
  }

  return files;
}

void handleCtrlC(int s){
  EXIT = true;
}

int main(int argc, char** argv) {
  struct sigaction sigIntHandler;

  sigIntHandler.sa_handler = handleCtrlC;
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;

  sigaction(SIGINT, &sigIntHandler, NULL);

  try {
    TCLAP::CmdLine cmd("Step 3 of Gamma+Jet analysis", ' ', "0.1");

    TCLAP::ValueArg<std::string> datasetArg("d", "dataset", "Dataset name", true, "", "string", cmd);

    TCLAP::MultiArg<std::string> inputArg("i", "in", "Input file", true, "string");
    TCLAP::ValueArg<std::string> inputListArg("", "input-list", "Text file containing input files", true, "input.list", "string");
    cmd.xorAdd(inputArg, inputListArg);

    std::vector<std::string> jetTypes;
    jetTypes.push_back("pf");
    jetTypes.push_back("calo");
    TCLAP::ValuesConstraint<std::string> allowedJetTypes(jetTypes);

    TCLAP::ValueArg<std::string> typeArg("", "type", "jet type", true, "pf", &allowedJetTypes, cmd);

    std::vector<std::string> algoTypes;
    algoTypes.push_back("ak5");
    algoTypes.push_back("ak7");
    TCLAP::ValuesConstraint<std::string> allowedAlgoTypes(algoTypes);

    TCLAP::ValueArg<std::string> algoArg("", "algo", "jet algo", true, "ak5", &allowedAlgoTypes, cmd);

    TCLAP::SwitchArg mcArg("", "mc", "MC?", cmd);

    TCLAP::ValueArg<int> totalJobsArg("", "num-jobs", "number of jobs planned", false, -1, "int", cmd);
    TCLAP::ValueArg<int> currentJobArg("", "job", "current job id", false, -1, "int", cmd);

    TCLAP::SwitchArg mcComparisonArg("", "mc-comp", "Cut photon pt to avoid trigger prescale issues", cmd);
    TCLAP::SwitchArg externalJECArg("", "jec", "Use external JEC", cmd);

    TCLAP::ValueArg<float> alphaCutArg("", "alpha", "P_t^{second jet} / p_t^{photon} cut (default: 0.2)", false, 0.2, "float", cmd);

    TCLAP::SwitchArg chsArg("", "chs", "Use CHS branches", cmd);
    TCLAP::SwitchArg verboseArg("v", "verbose", "Enable verbose mode", cmd);
    TCLAP::SwitchArg uncutTreesArg("", "uncut-trees", "Fill trees before second jet cut", cmd);

    cmd.parse(argc, argv);

    //std::cout << "Initializing..." << std::endl;
    //gSystem->Load("libFWCoreFWLite.so");
    //AutoLibraryLoader::enable();
    //std::cout << "done." << std::endl;

    std::vector<std::string> files;
    if (inputArg.isSet()) {
      files = inputArg.getValue();
    } else {
      files = readInputFiles(inputListArg.getValue());
    }

    GammaJetFinalizer finalizer;
    finalizer.setInputFiles(files);
    finalizer.setDatasetName(datasetArg.getValue());
    finalizer.setJetAlgo(typeArg.getValue(), algoArg.getValue());
    finalizer.setMC(mcArg.getValue());
    finalizer.setMCComparison(mcComparisonArg.getValue());
    finalizer.setUseExternalJEC(externalJECArg.getValue());
    finalizer.setAlphaCut(alphaCutArg.getValue());
    finalizer.setCHS(chsArg.getValue());
    finalizer.setVerbose(verboseArg.getValue());
    finalizer.setUncutTrees(uncutTreesArg.getValue());
    if (totalJobsArg.isSet() && currentJobArg.isSet()) {
      finalizer.setBatchJob(currentJobArg.getValue(), totalJobsArg.getValue());
    }

    finalizer.runAnalysis();

  } catch (TCLAP::ArgException &e) {
    std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
    return 1;
  }
}
