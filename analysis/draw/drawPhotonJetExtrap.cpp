#include <stdlib.h>
#include "drawExtrap.h"
#include "fitTools.h"

#include "tclap/CmdLine.h"

bool useMCassoc_ = false;
bool NOQ = false;
bool FIXM = false;
bool EXCLUDE_FIRST_POINT = false;
bool OUTPUT_GRAPHS = true;

int main(int argc, char* argv[]) {

  try {
    TCLAP::CmdLine cmd("Perform Gamma+Jet extrapolation", ' ', "0.1");

    std::vector<std::string> jetTypes;
    jetTypes.push_back("pf");
    jetTypes.push_back("calo");
    TCLAP::ValuesConstraint<std::string> allowedJetTypes(jetTypes);

    TCLAP::ValueArg<std::string> typeArg("", "type", "jet type", false, "pf", &allowedJetTypes, cmd);

    std::vector<std::string> algoTypes;
    algoTypes.push_back("ak5");
    algoTypes.push_back("ak7");
    TCLAP::ValuesConstraint<std::string> allowedAlgoTypes(algoTypes);

    TCLAP::ValueArg<std::string> algoArg("", "algo", "jet algo", false, "ak5", &allowedAlgoTypes, cmd);

    std::vector<std::string> resoTypes;
    resoTypes.push_back("FIT");
    resoTypes.push_back("RMS");
    resoTypes.push_back("RMS70");
    resoTypes.push_back("RMS95");
    resoTypes.push_back("RMS99");
    TCLAP::ValuesConstraint<std::string> allowedResoTypes(resoTypes);

    TCLAP::ValueArg<std::string> resoArg("", "reso-algo", "algo for resolution calculation", false, "RMS99", &allowedResoTypes, cmd);

    TCLAP::UnlabeledValueArg<std::string> dataArg("data_dataset", "data dataset name", true, "Photon_Run2011", "string", cmd);
    TCLAP::UnlabeledValueArg<std::string> mc1Arg("mc1_dataset", "first MC dataset name", true, "G", "string", cmd);
    TCLAP::UnlabeledValueArg<std::string> mc2Arg("mc2_dataset", "second MC dataset name", true, "QCD", "string", cmd);

    cmd.parse(argc, argv);

    std::string data_dataset = dataArg.getValue();
    std::string mc_dataset = mc1Arg.getValue();
    std::string mc2_dataset = mc2Arg.getValue();
    std::string FIT_RMS = resoArg.getValue();
    std::string flags = "";

    /*std::string flags = "";
      if (argc == 8) {
      std::string flags_str(argv[7]);
      flags = flags_str;
      std::cout << "flags set." << std::endl;
      }*/

    /*bool useGenJets = false;
      if (argc == 9) {
      std::string flags_str(argv[8]);
      if (flags_str == "GENJETS") useGenJets = true;
      }*/


    std::string jetAlgo = (algoArg.getValue() == "ak5") ? "AK5" : "AK7";
    std::string recoType = (typeArg.getValue() == "pf") ? "PFlow" : "Calo";
    std::string postFix = jetAlgo + recoType;

    std::string algoType;
    if (recoType == "Calo")
      algoType = jetAlgo;
    else
      algoType = recoType + jetAlgo;

    if (recoType == "JPT" && jetAlgo == "AK5")
      algoType = "jptak5";

    //std::string mcFlags = (useGenJets) ? "GENJETS_" + flags : flags;

    drawExtrap* db = new drawExtrap("PhotonJet", recoType, jetAlgo, true);
    db->set_pdf_aussi(false);
    db->set_isCMSArticle(false);

    db->set_FITRMS(resoArg.getValue());

    std::string NOQtext = (NOQ) ? "_NOQ" : "";
    std::string FIXMtext = (FIXM) ? "_FIXM" : "";
    std::string NOFIRSTPtext = (EXCLUDE_FIRST_POINT) ? "_NOFIRSTP" : "";

    char outputdir_char[200];
    /*if (mcFlags != "") {
      sprintf(outputdir_char, "PhotonJetExtrapPlots_%s_vs_%s_%s_%s_%s%s%s%s", data_dataset.c_str(), mc_dataset.c_str(), algoType.c_str(), mcFlags.c_str(), FIT_RMS.c_str(), FIXMtext.c_str(), NOFIRSTPtext.c_str(), NOQtext.c_str());
      } else*/
    {
      sprintf(outputdir_char, "PhotonJetExtrapPlots_%s_vs_%s_%s_%s%s%s%s", data_dataset.c_str(), mc_dataset.c_str(), algoType.c_str(), FIT_RMS.c_str(), FIXMtext.c_str(), NOFIRSTPtext.c_str(), NOQtext.c_str());
    }
    std::string outputdir_str(outputdir_char);

    //std::vector< float > ptPhot_binning = fitTools::getPtPhot_binning();

    db->set_outputdir(outputdir_str);

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
      mc1FileName = TString::Format("PhotonJet_%s_%s_%s.root", mc_dataset.c_str(), postFix.c_str(), flags.c_str());
    } else {
      mc1FileName = TString::Format("PhotonJet_%s_%s.root", mc_dataset.c_str(), postFix.c_str());
    }
    TFile* mcPhotonJetFile = TFile::Open(mc1FileName);
    std::cout << "Opened mc file '" << mc1FileName << "'." << std::endl;

    db->add_mcFile(mcPhotonJetFile, mc_dataset, "#gamma+jet MC", 46);

    TString mc2FileName;
    if (flags.length() > 0) {
      mc2FileName = TString::Format("PhotonJet_%s_%s_%s.root", mc2_dataset.c_str(), postFix.c_str(), flags.c_str());
    } else {
      mc2FileName = TString::Format("PhotonJet_%s_%s.root", mc2_dataset.c_str(), postFix.c_str());
    }
    TFile* mcQCDFile = TFile::Open(mc2FileName);
    std::cout << "Opened mc file '" << mc2FileName << "'." << std::endl;

    if (mc_dataset != mc2_dataset) {
      db->add_mcFile(mcQCDFile, mc2_dataset, "QCD MC", 38);
    } 

    db->set_lumiNormalization(5.09868 * 1e3);

    db->set_NOQ(NOQ);
    db->set_FIXM(FIXM);
    db->set_EXCLUDEFIRSTPOINT(EXCLUDE_FIRST_POINT);

    db->drawResponseExtrap("eta011", false);
    db->drawResponseExtrap("eta011", true);

    db->drawResponseExtrap("eta013", false);
    db->drawResponseExtrap("eta013", true);

    db->drawResponseExtrap("eta1524", false);
    db->drawResponseExtrap("eta1524", true);

    db->drawResponseExtrap("eta243", false);
    db->drawResponseExtrap("eta243", true);

    //db->drawResponseExtrap("eta35");

    delete db;
  } catch (TCLAP::ArgException &e) {
    std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
    return 1;
  }


  return 0;
}


