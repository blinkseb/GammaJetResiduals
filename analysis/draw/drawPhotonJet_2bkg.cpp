#include <stdlib.h>

#include "TParameter.h"
#include "TError.h"
#include "drawBase.h"
#include "fitTools.h"

#include "etaBinning.h"
#include "ptBinning.h"


bool useMCassoc_ = false;
bool ONEVTX = false;
bool OUTPUT_GRAPHS = true;

int main(int argc, char* argv[]) {

  if (argc != 7 && argc != 8) {
    std::cout << "USAGE: ./drawPhotonJet [data_dataset] [mc_SIGNAL_dataset] [mc_BG_dataset] [recoType] [jetAlgo] [norm ('LUMI' or 'SHAPE')] [flags=\"\"]" << std::endl;
    exit(23);
  }

  std::string data_dataset(argv[1]);
  std::string mc_photonjet(argv[2]);
  std::string mc_QCD(argv[3]);
  std::string recoType(argv[4]);
  std::string jetAlgo(argv[5]);
  std::string norm(argv[6]);
  if (norm != "LUMI" && norm != "SHAPE") {
    std::cout << "'" << norm << "' normalization not implemented yet." << std::endl;
    std::cout << "Only 'LUMI' and 'SHAPE' currently supported." << std::endl;
    std::cout << "Exiting." << std::endl;
    exit(9811);
  }
  std::string flags = "";
  if (argc == 8) {
    std::string flags_str(argv[7]);
    flags = flags_str;
  }


  std::string algoType;
  if (recoType == "calo") {
    algoType = jetAlgo;
  } else {
    algoType = recoType + jetAlgo;
  }
  if (recoType == "jpt" && jetAlgo == "akt5") {
    algoType = "jptak5";
  }

  jetAlgo = (jetAlgo == "ak5") ? "AK5" : "AK7";
  recoType = (recoType == "pf") ? "PFlow" : "Calo";
  std::string postFix = jetAlgo + recoType;


  //Float_t etamax = 3.;
  //bool sameEvents = false; //until njets histos have no overflows... or maybe use GetEntries instead of integral?

  drawBase* db = new drawBase("PhotonJet", recoType, jetAlgo, OUTPUT_GRAPHS);
  db->set_pdf_aussi((bool)false);
  db->set_flags(flags);
  db->set_isCMSArticle((bool)true);

  std::cout << "flags set." << std::endl;

  TString dataFileName;
  if (flags.length() > 0) {
    dataFileName = TString::Format("PhotonJet_%s_%s_%s.root", data_dataset.c_str(), postFix.c_str(), flags.c_str());
  } else {
    dataFileName = TString::Format("PhotonJet_%s_%s.root", data_dataset.c_str(), postFix.c_str());
  }

  TFile* dataFile = TFile::Open(dataFileName);
  std::cout << "Opened data file '" << dataFileName << "'." << std::endl;

  db->add_dataFile(dataFile, data_dataset);

  TString mc1FileName;
  if (flags.length() > 0) {
    mc1FileName = TString::Format("PhotonJet_%s_%s_%s.root", mc_photonjet.c_str(), postFix.c_str(), flags.c_str());
  } else {
    mc1FileName = TString::Format("PhotonJet_%s_%s.root", mc_photonjet.c_str(), postFix.c_str());
  }
  TFile* mcPhotonJetFile = TFile::Open(mc1FileName);
  std::cout << "Opened mc file '" << mc1FileName << "'." << std::endl;

  db->add_mcFile(mcPhotonJetFile, mc_photonjet, "#gamma+jet MC", 46);

  TString mc2FileName;
  if (flags.length() > 0) {
    mc2FileName = TString::Format("PhotonJet_%s_%s_%s.root", mc_QCD.c_str(), postFix.c_str(), flags.c_str());
  } else {
    mc2FileName = TString::Format("PhotonJet_%s_%s.root", mc_QCD.c_str(), postFix.c_str());
  }
  TFile* mcQCDFile = TFile::Open(mc2FileName);
  std::cout << "Opened mc file '" << mc2FileName << "'." << std::endl;

  if (mc_QCD != mc_photonjet) {
    db->add_mcFile(mcQCDFile, mc_QCD, "QCD MC", 38);
  }

  // MC should already be normalized to a lumi of 1 pb-1
  // Read luminosity
  TParameter<double>* lumi = static_cast<TParameter<double>*>(dataFile->Get("analysis/luminosity"));
  db->set_lumi(lumi->GetVal() * 1e-6);
  if (norm == "LUMI") {
    db->set_lumiNormalization();
  } else {
    db->set_shapeNormalization();
  }

  db->setFolder("analysis");
  db->set_outputdir();

  bool log = true;
  gErrorIgnoreLevel = kWarning;

  // Data / MC comparison
  db->drawHisto("ptPhoton_passedID", "Photon Transverse Momentum", "GeV", "Events", log);
  db->drawHisto("ptFirstJet_passedID", "Jet Transverse Momentum", "GeV", "Events", log);
  db->drawHisto("ptSecondJet_passedID", "2nd Jet Transverse Momentum", "GeV", "Events", log);

  //db->drawHisto("etaPhot", "#eta", "", "", log);

  db->drawHisto("nvertex", "Number of Reconstructed Vertices", "", "Events", log);
  db->drawHisto("nvertex_reweighted", "Number of Reconstructed Vertices (after reweighting)", "", "Events", log);
  db->drawHisto("deltaPhi_passedID", "#Delta #varphi", "", "Events", log);

  db->drawHisto("MET_passedID", "Missing E_{T}", "GeV", "Events", log);

  PtBinning ptBinning;
  std::vector<std::pair<float, float> > ptBins = ptBinning.getBinning();

  EtaBinning etaBinning;
  size_t etaBinningSize = etaBinning.size();

  db->set_rebin(5);

  // Balancing
  db->setFolder("analysis/balancing");
  for (size_t i = 0; i < etaBinningSize; i++) {
    db->set_legendTitle(etaBinning.getBinTitle(i));
    
    TString responseName = TString::Format("resp_balancing_%s", etaBinning.getBinName(i).c_str());
    db->drawHisto_vs_pt(ptBins, responseName.Data(), "Balancing Response", "", "Events", log);

    // Raw jets
    responseName = TString::Format("resp_balancing_raw_%s", etaBinning.getBinName(i).c_str());
    db->drawHisto_vs_pt(ptBins, responseName.Data(), "Balancing Response (raw jets)", "", "Events", log);

  }
  // Special case eta < 1.3

  db->set_legendTitle("|#eta| < 1.3");
  db->drawHisto_vs_pt(ptBins, "resp_balancing_eta013", "Balancing Response", "", "Events", log);
  db->drawHisto_vs_pt(ptBins, "resp_balancing_raw_eta013", "Balancing Response (raw jets)", "", "Events", log);

  // MPF
  db->setFolder("analysis/mpf");
  for (size_t i = 0; i < etaBinningSize; i++) {
    db->set_legendTitle(etaBinning.getBinTitle(i));
    
    TString responseName = TString::Format("resp_mpf_%s", etaBinning.getBinName(i).c_str());
    db->drawHisto_vs_pt(ptBins, responseName.Data(), "MPF Response", "", "Events", log);

    // Raw jets
    responseName = TString::Format("resp_mpf_raw_%s", etaBinning.getBinName(i).c_str());
    db->drawHisto_vs_pt(ptBins, responseName.Data(), "MPF Response (raw ME_{T})", "", "Events", log);

  }
  // Special case eta < 1.3

  db->set_legendTitle("|#eta| < 1.3");
  db->drawHisto_vs_pt(ptBins, "resp_mpf_eta013", "MPF Response", "", "Events", log);
  db->drawHisto_vs_pt(ptBins, "resp_mpf_raw_eta013", "MPF Response (raw ME_{T})", "", "Events", log);

  delete db;
  db = NULL;

  return 0;

}


