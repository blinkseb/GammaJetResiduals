#include "TFile.h"
#include "TROOT.h"
#include "TChain.h"
#include "TSystem.h"
#include "TTree.h"

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
#include <PhysicsTools/Utilities/interface/LumiReWeighting.h>

#include <SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h>

#include "tclap/CmdLine.h"

#include "gammaJetFinalizer.h"

#define RESET_COLOR "\033[m"
#define MAKE_RED "\033[31m"
#define MAKE_BLUE "\033[34m"

bool EXIT = false;

GammaJetFinalizer::GammaJetFinalizer() {
  mLumiReWeighter = NULL;
  mPUWeight = 1.;

  mSecondJetPtThreshold = 0.10;
  mDoMCComparison = false;
  mNoPUReweighting = false;
  mIsBatchJob = false;
}

GammaJetFinalizer::~GammaJetFinalizer() {

}

std::string GammaJetFinalizer::buildPostfix() {
  std::string algo = mJetAlgo == AK5 ? "AK5" : "AK7";
  std::string type = mJetType == PF ? "PFlow" : "Calo";

  return algo + type;
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

  // Initialization
  mExtrapBinning.initialize(mPtBinning, (mJetType == PF) ? "PFlow" : "Calo");

  std::cout << "Opening files ..." << std::endl;

  const std::string postFix = buildPostfix();

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

  secondJet.Init(&secondJetChain);
  secondRawJet.Init(&secondRawJetChain);
  secondGenJet.Init(&secondGenJetChain);

  MET.Init(&metChain);
  genMET.Init(&genMetChain);
  rawMET.Init(&rawMetChain);

  misc.Init(&miscChain);

  std::cout << "done." << std::endl;

  std::cout << std::endl << "##########" << std::endl;
  std::cout << "# " << MAKE_BLUE << "Running on " << MAKE_RED << ((mIsMC) ? "MC" : "DATA") << RESET_COLOR << std::endl;
  std::cout << "##########" << std::endl << std::endl;

  // Output file
  // Build output file name
  // PhotonJet_<dataset>_<postfix>.root
  std::string outputFile = (!mIsBatchJob)
    ? TString::Format("PhotonJet_%s_%s.root", mDatasetName.c_str(), postFix.c_str()).Data()
    : TString::Format("PhotonJet_%s_%s_part%02d.root", mDatasetName.c_str(), postFix.c_str(), mCurrentJob).Data();
  fwlite::TFileService fs(outputFile);

  /*TTree* photonTree = NULL;
    cloneTree(photon.fChain, photonTree);

    TTree* firstJetTree = NULL;
    cloneTree(firstJet.fChain, firstJetTree);

    TTree* secondJetTree = NULL;
    cloneTree(secondJet.fChain, secondJetTree);

    TTree* metTree = NULL;
    cloneTree(MET.fChain, metTree);

    TTree* muonsTree = NULL;
    cloneTree(muons.fChain, muonsTree);

    TTree* electronsTree = NULL;
    cloneTree(electrons.fChain, electronsTree);

    TTree* analysisTree = NULL;
    cloneTree(analysis.fChain, analysisTree);
    analysisTree->SetName("misc");*/

  std::cout << "Processing..." << std::endl;

  // Automatically call Sumw2 when creating an histogram
  TH1::SetDefaultSumw2(true);

  // Init some analysis variables
  TFileDirectory analysisDir = fs.mkdir("analysis");

  TH1F* h_nvertex = analysisDir.make<TH1F>("nvertex", "nvertex", 50, 0., 50.);
  TH1F* h_nvertex_reweighted = analysisDir.make<TH1F>("nvertex_reweighted", "nvertex_reweighted", 50, 0., 50.);

  TH1F* h_deltaPhi = analysisDir.make<TH1F>("deltaPhi", "deltaPhi", 60, M_PI / 2., M_PI);
  TH1F* h_deltaPhi_2ndJet = analysisDir.make<TH1F>("deltaPhi_2ndjet", "deltaPhi of 2nd jet", 60, M_PI / 2., M_PI);
  TH1F* h_ptPhoton = analysisDir.make<TH1F>("ptPhoton", "ptPhoton", 200, 5., 1000.);

  TH1F* h_deltaPhi_passedID = analysisDir.make<TH1F>("deltaPhi_passedID", "deltaPhi", 60, M_PI / 2., M_PI);
  TH1F* h_ptPhoton_passedID = analysisDir.make<TH1F>("ptPhoton_passedID", "ptPhoton", 200, 5., 1000.);
  TH1F* h_ptFirstJet_passedID = analysisDir.make<TH1F>("ptFirstJet_passedID", "ptFirstJet", 200, 5., 1000.);
  TH1F* h_ptSecondJet_passedID = analysisDir.make<TH1F>("ptSecondJet_passedID", "ptSecondJet", 60, 0., 100.);
  TH1F* h_MET_passedID = analysisDir.make<TH1F>("MET_passedID", "MET", 240, 0., 400.);

  // Balancing
  TFileDirectory balancingDir = analysisDir.mkdir("balancing");
  std::vector<std::vector<TH1F*> > responseBalancing = buildEtaVector<TH1F>(balancingDir, "resp_balancing", 150, 0., 2.);
  std::vector<std::vector<TH1F*> > responseBalancingRaw = buildEtaVector<TH1F>(balancingDir, "resp_balancing_raw", 150, 0., 2.);
  std::vector<std::vector<TH1F*> > responseBalancingGen;
  std::vector<std::vector<TH1F*> > responseBalancingRawGen;
  if (mIsMC) {
    responseBalancingGen = buildEtaVector<TH1F>(balancingDir, "resp_balancing_gen", 150, 0., 2.);
    responseBalancingRawGen = buildEtaVector<TH1F>(balancingDir, "resp_balancing_raw_gen", 150, 0., 2.);
  }

  std::vector<TH1F*> responseBalancingEta013 = buildVector<TH1F>(balancingDir, "resp_balancing", "eta013", 150, 0., 2.);
  std::vector<TH1F*> responseBalancingRawEta013 = buildVector<TH1F>(balancingDir, "resp_balancing_raw", "eta013", 150, 0., 2.);
  std::vector<TH1F*> responseBalancingGenEta013;
  std::vector<TH1F*> responseBalancingRawGenEta013;
  if (mIsMC) {
    responseBalancingGenEta013 = buildVector<TH1F>(balancingDir, "resp_balancing_gen", "eta013", 150, 0., 2.);
    responseBalancingRawGenEta013 = buildVector<TH1F>(balancingDir, "resp_balancing_raw_gen", "eta013", 150, 0., 2.);
  }

  // MPF
  TFileDirectory mpfDir = analysisDir.mkdir("mpf");
  std::vector<std::vector<TH1F*> > responseMPF = buildEtaVector<TH1F>(mpfDir, "resp_mpf", 150, 0., 2.);
  std::vector<std::vector<TH1F*> > responseMPFRaw = buildEtaVector<TH1F>(mpfDir, "resp_mpf_raw", 150, 0., 2.);
  std::vector<std::vector<TH1F*> > responseMPFGen;
  if (mIsMC) {
    responseMPFGen = buildEtaVector<TH1F>(mpfDir, "resp_mpf_gen", 150, 0., 2.);
  }

  std::vector<TH1F*> responseMPFEta013 = buildVector<TH1F>(mpfDir, "resp_mpf", "eta013", 150, 0., 2.);
  std::vector<TH1F*> responseMPFRawEta013 = buildVector<TH1F>(mpfDir, "resp_mpf_raw", "eta013", 150, 0., 2.);
  std::vector<TH1F*> responseMPFGenEta013;
  if (mIsMC) {
    responseMPFGenEta013 = buildVector<TH1F>(mpfDir, "resp_mpf_gen", "eta013", 150, 0., 2.);
  }

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
  std::vector<std::vector<TH1F*> > extrap_responseMPFEta013 = buildExtrapolationVector<TH1F>(extrapDir, "extrap_resp_mpf", "eta013", extrapolationBins, extrapolationMin, extrapolationMax);

  ExtrapolationVectors<TH1F>::type extrap_responseMPFGen;
  std::vector<std::vector<TH1F*> > extrap_responseMPFGenEta013;
  if (mIsMC) {
    extrap_responseMPFGen = buildExtrapolationEtaVector<TH1F>(extrapDir, "extrap_resp_mpf_gen", extrapolationBins, extrapolationMin, extrapolationMax);
    extrap_responseMPFGenEta013 = buildExtrapolationVector<TH1F>(extrapDir, "extrap_resp_mpf_gen", "eta013", extrapolationBins, extrapolationMin, extrapolationMax);
  }

  // Viola
  std::vector<TH1F*> ptFirstJetEta024 = buildVector<TH1F>(analysisDir, "ptFirstJet", "eta024", 500, 5., 1005.);

  uint64_t totalEvents = photonChain.GetEntries();
  uint64_t passedEvents = 0;

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

    if (mIsMC) {
      computePUWeight();
    }

    double eventWeight = (mIsMC) ? mPUWeight * analysis.event_weight : 1.;

    //TODO: On-the-fly JEC

    // Event selection
    // The photon is good from previous step
    // From previous step, we have fabs(deltaPhi(photon, firstJet)) > PI/2
    double deltaPhi = fabs(reco::deltaPhi(firstJet.phi, photon.phi));
    bool isBack2Back = (deltaPhi >= (M_PI - 1.));
    if (! isBack2Back) {
      continue;
    }

    bool secondJetOK = !secondJet.is_present || (secondJet.pt < 5. || secondJet.pt < mSecondJetPtThreshold * photon.pt);

    if (mDoMCComparison) {
      // Lowest unprescaled trigger for 2011 if at 135 GeV
      if (photon.pt < 155)
        continue;
    }

    h_nvertex->Fill(analysis.nvertex, analysis.event_weight);
    h_nvertex_reweighted->Fill(analysis.nvertex, eventWeight);

    double deltaPhi_2ndJet = fabs(reco::deltaPhi(secondJet.phi, photon.phi));
    h_deltaPhi->Fill(deltaPhi, eventWeight);
    h_deltaPhi_2ndJet->Fill(deltaPhi_2ndJet, eventWeight); 
    h_ptPhoton->Fill(photon.pt, eventWeight);

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

    if (secondJet.is_present) {
      do {
        int extrapBin = mExtrapBinning.getBin(photon.pt, secondJet.pt, ptBin);

        if (extrapBin < 0) {
          //std::cout << "No bin found for extrapolation: " << secondJet.pt / photon.pt << std::endl;
          break;
        }

        float r_RecoPhot = firstJet.pt / photon.pt;
        float r_RecoGen  = firstJet.pt / firstGenJet.pt;
        float r_RecoPhotRaw = firstRawJet.pt / photon.pt;
        float r_RecoGenRaw  = firstRawJet.pt / firstGenJet.pt;
        float r_GenPhot  = firstGenJet.pt / photon.pt;
        float r_GenGamma  = firstGenJet.pt / genPhoton.pt;
        float r_PhotGamma  = photon.pt / genPhoton.pt;

        // Special case

        if (fabs(firstJet.eta) < 1.3) {
          extrap_responseBalancingEta013[ptBin][extrapBin]->Fill(r_RecoPhot, eventWeight);
          extrap_responseBalancingRawEta013[ptBin][extrapBin]->Fill(r_RecoPhotRaw, eventWeight);
          extrap_responseMPFEta013[ptBin][extrapBin]->Fill(r_RecoPhot, eventWeight);

          if (mIsMC && ptBinGen >= 0 && etaBinGen >= 0) {
            extrap_responseBalancingGenEta013[ptBinGen][extrapBin]->Fill(r_RecoGen, eventWeight);
            extrap_responseBalancingRawGenEta013[ptBinGen][extrapBin]->Fill(r_RecoGenRaw, eventWeight);
            extrap_responseBalancingGenPhotEta013[ptBinGen][extrapBin]->Fill(r_GenPhot, eventWeight);
            extrap_responseBalancingGenGammaEta013[ptBinGen][extrapBin]->Fill(r_GenGamma, eventWeight);
            extrap_responseBalancingPhotGammaEta013[ptBinGen][extrapBin]->Fill(r_PhotGamma, eventWeight);
            extrap_responseMPFGenEta013[ptBinGen][extrapBin]->Fill(r_RecoPhot, eventWeight);
          }
        }

        if (etaBin < 0)
          break;

        extrap_responseBalancing[etaBin][ptBin][extrapBin]->Fill(r_RecoPhot, eventWeight);
        extrap_responseBalancingRaw[etaBin][ptBin][extrapBin]->Fill(r_RecoPhotRaw, eventWeight);
        extrap_responseMPF[etaBin][ptBin][extrapBin]->Fill(r_RecoPhot, eventWeight);

        if (mIsMC && ptBinGen >= 0 && etaBinGen >= 0) {
          extrap_responseBalancingGen[etaBinGen][ptBinGen][extrapBin]->Fill(r_RecoGen, eventWeight);
          extrap_responseBalancingRawGen[etaBinGen][ptBinGen][extrapBin]->Fill(r_RecoGenRaw, eventWeight);
          extrap_responseBalancingGenPhot[etaBinGen][ptBinGen][extrapBin]->Fill(r_GenPhot, eventWeight);
          extrap_responseBalancingGenGamma[etaBinGen][ptBinGen][extrapBin]->Fill(r_GenGamma, eventWeight);
          extrap_responseBalancingPhotGamma[etaBinGen][ptBinGen][extrapBin]->Fill(r_PhotGamma, eventWeight);
          extrap_responseMPFGen[etaBinGen][ptBinGen][extrapBin]->Fill(r_RecoPhot, eventWeight);
        }

      } while (false);
    }

    if (secondJetOK) {


      h_deltaPhi_passedID->Fill(deltaPhi, eventWeight);
      h_ptPhoton_passedID->Fill(photon.pt, eventWeight);
      h_ptFirstJet_passedID->Fill(firstJet.pt, eventWeight);
      h_ptSecondJet_passedID->Fill(secondJet.pt, eventWeight);
      h_MET_passedID->Fill(MET.et, eventWeight);

      // Special case
      if (fabs(firstJet.eta) < 1.3) {
        responseBalancingEta013[ptBin]->Fill(respBalancing, eventWeight);
        responseBalancingRawEta013[ptBin]->Fill(respBalancingRaw, eventWeight);

        responseMPFEta013[ptBin]->Fill(respMPF, eventWeight);
        responseMPFRawEta013[ptBin]->Fill(respMPFRaw, eventWeight);

        if (mIsMC && ptBinGen >= 0) {
          responseBalancingGenEta013[ptBinGen]->Fill(respBalancingGen, eventWeight);
          responseBalancingRawGenEta013[ptBinGen]->Fill(respBalancingRawGen, eventWeight);

          responseMPFGenEta013[ptBinGen]->Fill(respMPFGen, eventWeight);
        }
      }

      // Viola
      if (fabs(firstJet.eta) < 2.4 && (fabs(firstJet.eta) < 1.4442 || fabs(firstJet.eta) > 1.5560)){ 
        ptFirstJetEta024[ptBin]->Fill(firstJet.pt, eventWeight);
      }

      if (etaBin < 0) {
        //std::cout << "Jet eta " << firstJet.eta() << " is not covered by our eta binning. Dumping event." << std::endl;
        continue;
      }


      responseBalancing[etaBin][ptBin]->Fill(respBalancing, eventWeight);
      responseBalancingRaw[etaBin][ptBin]->Fill(respBalancingRaw, eventWeight);

      responseMPF[etaBin][ptBin]->Fill(respMPF, eventWeight);
      responseMPFRaw[etaBin][ptBin]->Fill(respMPFRaw, eventWeight);

      // Gen values
      if (mIsMC && ptBinGen >= 0 && etaBinGen >= 0) {
        responseBalancingGen[etaBinGen][ptBinGen]->Fill(respBalancingGen, eventWeight);
        responseBalancingRawGen[etaBinGen][ptBinGen]->Fill(respBalancingRawGen, eventWeight);

        responseMPFGen[etaBinGen][ptBinGen]->Fill(respMPFGen, eventWeight);
      }

      /*photonTree->Fill();
        firstJetTree->Fill();
        secondJetTree->Fill();
        metTree->Fill();
        electronsTree->Fill();
        muonsTree->Fill();
        analysisTree->Fill();*/

      passedEvents++;
    }
  }

  std::cout << "Selection efficiency: " << MAKE_RED << (double) passedEvents / (to - from) * 100 << "%" << RESET_COLOR << std::endl;
}

void GammaJetFinalizer::doSecondJetExtrapolation() {

  // TODO: Extrapolation

}

template<typename T>
std::vector<T*> GammaJetFinalizer::buildVector(TFileDirectory dir, const std::string& branchName, const std::string& etaName, int nBins, double xMin, double xMax) {

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
std::vector<std::vector<T*> > GammaJetFinalizer::buildEtaVector(TFileDirectory dir, const std::string& branchName, int nBins, double xMin, double xMax) {
  size_t etaBinningSize = mEtaBinning.size();
  std::vector<std::vector<T*> > etaBinning;

  for (size_t i = 0; i < etaBinningSize; i++) {
    const std::string etaName = mEtaBinning.getBinName(i);
    etaBinning.push_back(buildVector<T>(dir, branchName, etaName, nBins, xMin, xMax));
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

void GammaJetFinalizer::computePUWeight() {
  static std::string puPrefix = "/gridgroup/cms/brochet/public/pu";
  static std::string puData = TString::Format("%s/pu_truth_data_photon_2011_true_nopix_cleaned_50bins.root", puPrefix.c_str()).Data();
  static std::string puMC = TString::Format("%s/fall11_computed_mc_%s_pu_truth_50bins.root", puPrefix.c_str(), mDatasetName.c_str()).Data();

  if (mNoPUReweighting)
    return;

  if (! mLumiReWeighter) {
    if (! boost::filesystem::exists(puMC)) {
      std::cout << "Warning: " << MAKE_RED << "pileup histogram for MC was not found. No PU reweighting." << RESET_COLOR << std::endl;
      mNoPUReweighting = true;
      mPUWeight = 1.;
      return;
    } else {
      mLumiReWeighter = new edm::LumiReWeighting(puMC, puData, "pileup", "pileup");
    }
  }

  mPUWeight = mLumiReWeighter->weight(analysis.ntrue_interactions);
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

    cmd.parse(argc, argv);

    std::cout << "Initializing..." << std::endl;
    gSystem->Load("libFWCoreFWLite.so");
    AutoLibraryLoader::enable();
    std::cout << "done." << std::endl;

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
    if (totalJobsArg.isSet() && currentJobArg.isSet()) {
      finalizer.setBatchJob(currentJobArg.getValue(), totalJobsArg.getValue());
    }

    finalizer.runAnalysis();

  } catch (TCLAP::ArgException &e) {
    std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
    return 1;
  }


}
