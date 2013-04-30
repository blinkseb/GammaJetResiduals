#include <stdlib.h>

#include "TParameter.h"
#include "TError.h"
#include "drawBase.h"
#include "fitTools.h"

#include <TGraphErrors.h>
#include <TMultiGraph.h>
#include <TLatex.h>

#include <TVirtualFitter.h>
#include <TColor.h>

#include <sys/stat.h>

#include "etaBinning.h"
#include "ptBinning.h"

#include "prettyRootStyle.h"

bool useMCassoc_ = false;
bool ONEVTX = false;
bool OUTPUT_GRAPHS = true;

#define LIGHT_RED TColor::GetColor(0xcf, 0xa0, 0xa1)
#define LIGHT_BLUE TColor::GetColor(0xb2, 0xb2, 0xcf)
#define LIGHT_GRAY TColor::GetColor(0xcd, 0xcd, 0xcd)
#define LIGHT_MARRON TColor::GetColor(0xcf, 0xb9, 0xba)

void drawGraphs(TGraphErrors* data, TGraphErrors* mc, const std::string& method, const std::string& xTitle, const std::string& yTitle, const std::string& legendTitle, double lumi, const std::string& outputName, int dataMarkerStyle = 20, int dataMarkerColor = kBlack, int mcMarkerStyle = 29, int mcMarkerColor = kBlue) {

  data->SetMarkerSize(1.5);
  data->SetMarkerStyle(dataMarkerStyle);
  data->SetMarkerColor(dataMarkerColor);
  data->SetLineColor(dataMarkerColor);

  mc->SetMarkerSize(1.5);
  mc->SetMarkerStyle(mcMarkerStyle);
  mc->SetMarkerColor(mcMarkerColor);
  mc->SetLineColor(mcMarkerColor);

  // Fit
  TF1* data_fct = nullptr;
  TF1* mc_fct = nullptr;

  TH1D* errors_data = new TH1D("errors_data", "errors", 100, 0, 1);
  errors_data->SetStats(false);
  errors_data->SetFillColor(LIGHT_GRAY);
  //errors_data->SetLineColor(kRed);
  errors_data->SetFillStyle(1001);

  TH1D* errors_mc = (TH1D*) errors_data->Clone("errors_mc");
  errors_mc->SetFillColor(LIGHT_BLUE);

  if (method == "Balancing") {
    data_fct = new TF1("data_fct", "[0] - x*x*[1]", 0, 1);
    data_fct->SetLineColor(dataMarkerColor);
    data_fct->SetLineWidth(1);
    data_fct->SetLineStyle(2);

    data->Fit(data_fct, "RQN");
    (TVirtualFitter::GetFitter())->GetConfidenceIntervals(errors_data, 0.68);

    mc_fct = new TF1("mc_fct", "[0] - x*x*[1]", 0, 1);
    mc_fct->SetLineColor(mcMarkerColor);
    mc_fct->SetLineWidth(1);
    mc_fct->SetLineStyle(2);

    mc->Fit(mc_fct, "RQN");
    (TVirtualFitter::GetFitter())->GetConfidenceIntervals(errors_mc, 0.68);
  } else {
    data_fct = new TF1("data_fct", "[0] + x*[1]", 0, 1);
    data_fct->SetLineColor(dataMarkerColor);
    data_fct->SetLineWidth(1);
    data_fct->SetLineStyle(2);

    data->Fit(data_fct, "RQN");
    (TVirtualFitter::GetFitter())->GetConfidenceIntervals(errors_data, 0.68);

    mc_fct = new TF1("mc_fct", "[0] + x*[1]", 0, 1);
    mc_fct->SetLineColor(mcMarkerColor);
    mc_fct->SetLineWidth(1);
    mc_fct->SetLineStyle(2);

    mc->Fit(mc_fct, "RQN");
    (TVirtualFitter::GetFitter())->GetConfidenceIntervals(errors_mc, 0.68);

  }

  data_fct->SetRange(0, 1);
  mc_fct->SetRange(0, 1);

  TMultiGraph* mg = new TMultiGraph();
  mg->Add(data);
  mg->Add(mc);

  TString title = TString::Format(";%s;%s", xTitle.c_str(), yTitle.c_str());
  mg->SetTitle(title);

  TCanvas* canvas = new TCanvas("canvas", "", 800, 800);

  mg->Draw("ap");

  errors_data->Draw("e3 same");
  errors_mc->Draw("e3 same");

  data_fct->Draw("same");
  mc_fct->Draw("same");

  mg->Draw("ap same");


  TLegend* legend = new TLegend(0.18, 0.18, 0.55, 0.35);
  legend->SetTextFont(42);
  legend->SetFillColor(kWhite);
  legend->SetFillStyle(0);
  legend->SetTextSize(0.035);
  legend->SetBorderSize(1);

  TString legendTitleWithPtCut = TString::Format("%s, p_{T}^{#gamma} #geq 170 GeV", legendTitle.c_str());

  legend->SetHeader(legendTitleWithPtCut);
  legend->AddEntry(data, TString::Format("%s (data)", method.c_str()), "p");
  legend->AddEntry(mc, TString::Format("%s (MC)", method.c_str()), "p");
  legend->Draw();

  TLatex tl;
  tl.SetNDC();
  tl.SetTextSize(0.035);
  tl.SetTextFont(42);

  // Luminosity
  TString sLumi = TString::Format("L = %.02f fb^{-1}", lumi);
  tl.DrawLatex(0.18, 0.96, sLumi);

  // Energy
  tl.DrawLatex(0.80, 0.96, "#sqrt{s} = 8 TeV");

  canvas->Print(outputName.c_str());

  delete canvas;
  delete mg;

  delete errors_data;
  delete errors_mc;
}

void drawCombinedGraphs(TGraphErrors* balancingData, TGraphErrors* balancingMC, TGraphErrors* mpfData, TGraphErrors* mpfMC, const std::string& xTitle, const std::string& yTitle, const std::string& legendTitle, double lumi, const std::string& outputName) {

  balancingData->SetMarkerSize(1.5);
  balancingData->SetMarkerStyle(20);
  balancingData->SetMarkerColor(kBlack);
  balancingData->SetLineColor(kBlack);

  mpfData->SetMarkerSize(1.5);
  mpfData->SetMarkerStyle(20);
  mpfData->SetMarkerColor(kRed);
  mpfData->SetLineColor(kRed);

  balancingMC->SetMarkerSize(1.5);
  balancingMC->SetMarkerStyle(29);
  balancingMC->SetMarkerColor(kBlue);
  balancingMC->SetLineColor(kBlue);

  mpfMC->SetMarkerSize(1.5);
  mpfMC->SetMarkerStyle(29);
  mpfMC->SetMarkerColor(46);
  mpfMC->SetLineColor(46);
  
  TH1D* errors_bal_data = new TH1D("errors_bal_data", "errors", 100, 0, 1);
  errors_bal_data->SetStats(false);
  errors_bal_data->SetFillColor(LIGHT_GRAY);
  errors_bal_data->SetFillStyle(1001);

  TH1D* errors_bal_mc = (TH1D*) errors_bal_data->Clone("errors_bal_mc");
  errors_bal_mc->SetFillColor(LIGHT_BLUE);

  TH1D* errors_mpf_data = new TH1D("errors_mpf_data", "errors", 100, 0, 1);
  errors_mpf_data->SetStats(false);
  errors_mpf_data->SetFillColor(LIGHT_RED);
  errors_mpf_data->SetFillStyle(1001);

  TH1D* errors_mpf_mc = (TH1D*) errors_bal_data->Clone("errors_mpf_mc");
  errors_mpf_mc->SetFillColor(LIGHT_MARRON);

  TF1* balancingData_fct = new TF1("balancingData_fct", "[0] - x*x*[1]", 0, 1);
  balancingData_fct->SetLineColor(kBlack);
  balancingData_fct->SetLineWidth(1);
  balancingData_fct->SetLineStyle(2);

  balancingData->Fit(balancingData_fct, "QRN");
  (TVirtualFitter::GetFitter())->GetConfidenceIntervals(errors_bal_data, 0.68);

  TF1* balancingMC_fct = new TF1("mc_fct", "[0] - x*x*[1]", 0, 1);
  balancingMC_fct->SetLineColor(kBlue);
  balancingMC_fct->SetLineWidth(1);
  balancingMC_fct->SetLineStyle(2);

  balancingMC->Fit(balancingMC_fct, "QRN");
  (TVirtualFitter::GetFitter())->GetConfidenceIntervals(errors_bal_mc, 0.68);

  TF1* mpfData_fct = new TF1("mpfData_fct", "[0] + x*[1]", 0, 1);
  mpfData_fct->SetLineColor(kRed);
  mpfData_fct->SetLineWidth(1);
  mpfData_fct->SetLineStyle(2);

  mpfData->Fit(mpfData_fct, "QRN");
  (TVirtualFitter::GetFitter())->GetConfidenceIntervals(errors_mpf_data, 0.68);

  TF1* mpfMC_fct = new TF1("mc_fct", "[0] + x*[1]", 0, 1);
  mpfMC_fct->SetLineColor(46);
  mpfMC_fct->SetLineWidth(1);
  mpfMC_fct->SetLineStyle(2);

  mpfMC->Fit(mpfMC_fct, "QRN");
  (TVirtualFitter::GetFitter())->GetConfidenceIntervals(errors_mpf_mc, 0.68);

  TString balancing_ratio_legend = TString::Format("#color[4]{#scale[1]{r_{bal} = %.03f #pm %.03f}}", balancingData_fct->GetParameter(0) / balancingMC_fct->GetParameter(0), sqrt(pow(balancingData_fct->GetParError(0), 2) + pow(balancingMC_fct->GetParError(0), 2)));

  TString mpf_ratio_legend = TString::Format("#color[2]{#scale[1]{r_{MPF} = %.03f #pm %.03f}}", mpfData_fct->GetParameter(0) / mpfMC_fct->GetParameter(0), sqrt(pow(mpfData_fct->GetParError(0), 2) + pow(mpfMC_fct->GetParError(0), 2)));

  TMultiGraph* mg = new TMultiGraph();
  mg->Add(balancingData);
  mg->Add(balancingMC);
  mg->Add(mpfData);
  mg->Add(mpfMC);

  TString title = TString::Format(";%s;%s", xTitle.c_str(), yTitle.c_str());
  mg->SetTitle(title);

  TCanvas* canvas = new TCanvas("canvas", "", 800, 800);

  mg->Draw("ap");

  balancingData_fct->SetRange(0, 1);
  balancingMC_fct->SetRange(0, 1);
  mpfData_fct->SetRange(0, 1);
  mpfMC_fct->SetRange(0, 1);

  errors_bal_data->Draw("e3 same");
  errors_bal_mc->Draw("e3 same");

  errors_mpf_data->Draw("e3 same");
  errors_mpf_mc->Draw("e3 same");

  mg->Draw("ap same");

  balancingData_fct->Draw("same");
  balancingMC_fct->Draw("same");
  mpfData_fct->Draw("same");
  mpfMC_fct->Draw("same");

  TLegend* legend = new TLegend(0.18, 0.18, 0.55, 0.45);
  legend->SetTextFont(42);
  legend->SetFillColor(kWhite);
  legend->SetFillStyle(0);
  legend->SetTextSize(0.035);
  legend->SetBorderSize(1);

  TString legendTitleWithPtCut = TString::Format("%s, p_{T}^{#gamma} #geq 170 GeV", legendTitle.c_str());

  legend->SetHeader(legendTitleWithPtCut);
  legend->AddEntry(balancingData, "Balancing (data)", "p");
  legend->AddEntry(balancingMC, "Balancing (MC)", "p");
  legend->AddEntry(mpfData, "MPF (data)", "p");
  legend->AddEntry(mpfMC, "MPF (MC)", "p");
  legend->Draw();

  TLatex tl;
  tl.SetNDC();
  tl.SetTextSize(0.035);
  tl.SetTextFont(42);

  // Luminosity
  TString sLumi = TString::Format("L = %.02f fb^{-1}", lumi);
  tl.DrawLatex(0.18, 0.96, sLumi);

  // Energy
  tl.DrawLatex(0.80, 0.96, "#sqrt{s} = 8 TeV");

  // Ratios
  tl.DrawLatex(0.18, 0.515, balancing_ratio_legend);
  tl.DrawLatex(0.18, 0.47, mpf_ratio_legend);

  canvas->Print((outputName + ".pdf").c_str());

  delete legend;
  delete canvas;

  delete errors_bal_data;
  delete errors_bal_mc;

  delete errors_mpf_data;
  delete errors_mpf_mc;

  // Do now data / MC plots

  TH1D* errors_bal = new TH1D("errors_bal", "errors", 100, 0, 1);
  errors_bal->SetStats(false);
  errors_bal->SetFillColor(LIGHT_BLUE);
  errors_bal->SetFillStyle(1001);

  TH1D* errors_mpf = (TH1D*) errors_bal->Clone("errors_mpf");
  errors_mpf->SetFillColor(LIGHT_RED);

  TGraphErrors* balancing_ratio = fitTools::get_graphRatio(balancingData, balancingMC);
  balancing_ratio->SetMarkerSize(1.5);
  balancing_ratio->SetMarkerColor(kBlue);
  balancing_ratio->SetLineColor(kBlue);
  balancing_ratio->SetMarkerStyle(20);

  TGraphErrors* mpf_ratio = fitTools::get_graphRatio(mpfData, mpfMC);
  mpf_ratio->SetMarkerSize(1.5);
  mpf_ratio->SetMarkerColor(kRed);
  mpf_ratio->SetLineColor(kRed);
  mpf_ratio->SetMarkerStyle(20);

  TF1* balancingRatio_fct = new TF1("balancingRatio_fct", "[0] + x*[1]", 0, 1);
  balancingRatio_fct->SetLineColor(kBlue);
  balancingRatio_fct->SetLineWidth(1);
  balancingRatio_fct->SetLineStyle(2);

  balancing_ratio->Fit(balancingRatio_fct, "QRN");
  (TVirtualFitter::GetFitter())->GetConfidenceIntervals(errors_bal, 0.68);
  balancing_ratio_legend = TString::Format("#color[4]{#splitline{#scale[1.2]{r = %.03f #pm %.03f}}{#scale[0.8]{#chi^{2} / NDF: %.02f / %d}}}", balancingRatio_fct->GetParameter(0), balancingRatio_fct->GetParError(0), balancingRatio_fct->GetChisquare(), balancingRatio_fct->GetNDF());

  TF1* mpfRatio_fct = new TF1("mpfRatio_fct", "[0] + x*[1]", 0, 1);
  mpfRatio_fct->SetLineColor(kRed);
  mpfRatio_fct->SetLineWidth(1);
  mpfRatio_fct->SetLineStyle(2);

  mpf_ratio->Fit(mpfRatio_fct, "QRN");
  (TVirtualFitter::GetFitter())->GetConfidenceIntervals(errors_mpf, 0.68);
  mpf_ratio_legend = TString::Format("#color[2]{#splitline{#scale[1.2]{r = %.03f #pm %.03f}}{#scale[0.8]{#chi^{2} / NDF: %.02f / %d}}}", mpfRatio_fct->GetParameter(0), mpfRatio_fct->GetParError(0), mpfRatio_fct->GetChisquare(), mpfRatio_fct->GetNDF());

  TMultiGraph* mg2 = new TMultiGraph();
  mg2->Add(balancing_ratio);
  mg2->Add(mpf_ratio);

  title = TString::Format(";%s;Data / MC ratio", xTitle.c_str());
  mg2->SetTitle(title);

  canvas = new TCanvas("canvas", "", 800, 800);

  mg2->Draw("ap");

  balancingRatio_fct->SetRange(0, 1);
  mpfRatio_fct->SetRange(0, 1);

  errors_bal->Draw("e3 same");
  errors_mpf->Draw("e3 same");

  mg2->Draw("ap same");

  balancingRatio_fct->Draw("same");
  mpfRatio_fct->Draw("same");

  legend = new TLegend(0.18, 0.18, 0.50, 0.35);
  legend->SetTextFont(42);
  legend->SetFillColor(kWhite);
  legend->SetFillStyle(0);
  legend->SetTextSize(0.035);
  legend->SetBorderSize(1);

  legend->SetHeader(legendTitleWithPtCut);
  legend->AddEntry(balancing_ratio, "Balancing", "p");
  legend->AddEntry(mpf_ratio, "MPF", "p");
  legend->Draw();

  // Luminosity
  tl.DrawLatex(0.18, 0.96, sLumi);

  // Energy
  tl.DrawLatex(0.80, 0.96, "#sqrt{s} = 8 TeV");

  // Fit
  tl.DrawLatex(0.18, 0.47, balancing_ratio_legend);
  tl.DrawLatex(0.18, 0.38, mpf_ratio_legend);

  canvas->Print((outputName + "_ratio.pdf").c_str());

  delete legend;
  delete canvas;

  
  delete mg;
  delete mg2;

  delete errors_bal;
  delete errors_mpf;
}

int main(int argc, char* argv[]) {

  if (argc != 7 && argc != 8) {
    std::cout << "USAGE: ./drawPhotonJet [data_dataset] [mc_SIGNAL_dataset] [mc_BG_dataset] [recoType] [jetAlgo]" << std::endl;
    exit(23);
  }

  setPrettyStyle();

  std::string data_dataset(argv[1]);
  std::string mc_photonjet(argv[2]);
  std::string mc_QCD(argv[3]);
  std::string recoType(argv[4]);
  std::string jetAlgo(argv[5]);

  std::string algoType;
  if (recoType == "calo") {
    algoType = jetAlgo;
  } else {
    algoType = recoType + jetAlgo;
  }
  if (recoType == "jpt" && jetAlgo == "akt5") {
    algoType = "jptak5";
  }

  std::string flags = "";

  jetAlgo = (jetAlgo == "ak5") ? "AK5" : "AK7";
  recoType = (recoType == "pf") ? "PFlow" : "Calo";
  std::string postFix = recoType + jetAlgo;

  postFix += "chs";

  TString dataFileName;
  if (flags.length() > 0) {
    dataFileName = TString::Format("PhotonJet_%s_%s_%s.root", data_dataset.c_str(), postFix.c_str(), flags.c_str());
  } else {
    dataFileName = TString::Format("PhotonJet_%s_%s.root", data_dataset.c_str(), postFix.c_str());
  }

  TFile* dataFile = TFile::Open(dataFileName);
  std::cout << "Opened data file '" << dataFileName << "'." << std::endl;

  //db->add_dataFile(dataFile, data_dataset);

  TString mc1FileName;
  if (flags.length() > 0) {
    mc1FileName = TString::Format("PhotonJet_%s_%s_%s.root", mc_photonjet.c_str(), postFix.c_str(), flags.c_str());
  } else {
    mc1FileName = TString::Format("PhotonJet_%s_%s.root", mc_photonjet.c_str(), postFix.c_str());
  }

  TFile* mcPhotonJetFile = TFile::Open(mc1FileName);
  std::cout << "Opened mc file '" << mc1FileName << "'." << std::endl;

  if (mcPhotonJetFile) {
    //db->add_mcFile(mcPhotonJetFile, mc_photonjet, "#gamma+jet MC", 46);
  }

  if (mc_QCD != "") {
    TString mc2FileName;
    if (flags.length() > 0) {
      mc2FileName = TString::Format("PhotonJet_%s_%s_%s.root", mc_QCD.c_str(), postFix.c_str(), flags.c_str());
    } else {
      mc2FileName = TString::Format("PhotonJet_%s_%s.root", mc_QCD.c_str(), postFix.c_str());
    }
    TFile* mcQCDFile = TFile::Open(mc2FileName);
    std::cout << "Opened mc file '" << mc2FileName << "'." << std::endl;

    if (mcQCDFile && mc_QCD != mc_photonjet) {
      //db->add_mcFile(mcQCDFile, mc_QCD, "QCD MC", 38);
    }
  }

  // Create output directory
  mkdir("plots", 0755);

  TString directoryName = TString::Format("plots/%s_vs_%s_%s", data_dataset.c_str(), mc_photonjet.c_str(), postFix.c_str());
  mkdir(directoryName, 0755);

  directoryName = TString::Format("%s/extrapolation", directoryName.Data());
  mkdir(directoryName, 0755);

  TParameter<double>* pLumi = static_cast<TParameter<double>*>(dataFile->Get("analysis/luminosity"));
  double lumi = pLumi->GetVal() * 1e-9;

  //bool log = true;
  gErrorIgnoreLevel = kWarning;

  EtaBinning etaBinning;
  size_t etaBinningSize = etaBinning.size();

  TString rootFolder = "analysis/new_extrapolation";
  for (size_t i = 0; i < etaBinningSize; i++) {

    const std::string& etaName = etaBinning.getBinName(i);
    std::cout << "Processing " << etaName << std::endl;
    
    TString responseName = TString::Format("%s/extrap_resp_balancing_%s_graph", rootFolder.Data(), etaName.c_str());
    TString outputName = TString::Format("%s/extrap_resp_balancing_%s.pdf", directoryName.Data(), etaName.c_str());
    TGraphErrors* data = (TGraphErrors*) dataFile->Get(responseName);
    TGraphErrors* mc = (TGraphErrors*) mcPhotonJetFile->Get(responseName);

    drawGraphs(data, mc, "Balancing", "p_{t}^{2^{nd} jet} / p_{t}^{#gamma}", "Jet response", etaBinning.getBinTitle(i), lumi, outputName.Data());

    // Raw jets
    responseName = TString::Format("%s/extrap_resp_balancing_raw_%s_graph", rootFolder.Data(), etaName.c_str());
    outputName = TString::Format("%s/extrap_resp_balancing_raw_%s.pdf", directoryName.Data(), etaName.c_str());
    data = (TGraphErrors*) dataFile->Get(responseName);
    mc = (TGraphErrors*) mcPhotonJetFile->Get(responseName);

    drawGraphs(data, mc, "Balancing", "p_{t}^{2^{nd} jet} / p_{t}^{#gamma}", "Jet response (raw jets)", etaBinning.getBinTitle(i), lumi, outputName.Data());

    responseName = TString::Format("%s/extrap_resp_mpf_%s_graph", rootFolder.Data(), etaName.c_str());
    outputName = TString::Format("%s/extrap_resp_mpf_%s.pdf", directoryName.Data(), etaName.c_str());
    data = (TGraphErrors*) dataFile->Get(responseName);
    mc = (TGraphErrors*) mcPhotonJetFile->Get(responseName);

    drawGraphs(data, mc, "MPF", "p_{t}^{2^{nd} jet} / p_{t}^{#gamma}", "Jet response", etaBinning.getBinTitle(i), lumi, outputName.Data());

    // Raw jets
    responseName = TString::Format("%s/extrap_resp_mpf_raw_%s_graph", rootFolder.Data(), etaName.c_str());
    outputName = TString::Format("%s/extrap_resp_mpf_raw_%s.pdf", directoryName.Data(), etaName.c_str());
    data = (TGraphErrors*) dataFile->Get(responseName);
    mc = (TGraphErrors*) mcPhotonJetFile->Get(responseName);

    drawGraphs(data, mc, "MPF", "p_{t}^{2^{nd} jet} / p_{t}^{#gamma}", "Jet response (raw #slashed{E_{t}})", etaBinning.getBinTitle(i), lumi, outputName.Data());
  }
  // Special case eta < 1.3
  {
    const std::string& etaName = "eta013";
    std::cout << "Processing " << etaName << std::endl;
    
    TString responseName = TString::Format("%s/extrap_resp_balancing_%s_graph", rootFolder.Data(), etaName.c_str());
    TString outputName = TString::Format("%s/extrap_resp_balancing_%s.pdf", directoryName.Data(), etaName.c_str());
    TGraphErrors* data = (TGraphErrors*) dataFile->Get(responseName);
    TGraphErrors* mc = (TGraphErrors*) mcPhotonJetFile->Get(responseName);

    drawGraphs(data, mc, "Balancing", "p_{t}^{2^{nd} jet} / p_{t}^{#gamma}", "Jet response", "#eta #leq 1.3", lumi, outputName.Data());

    // Raw jets
    responseName = TString::Format("%s/extrap_resp_balancing_raw_%s_graph", rootFolder.Data(), etaName.c_str());
    outputName = TString::Format("%s/extrap_resp_balancing_raw_%s.pdf", directoryName.Data(), etaName.c_str());
    data = (TGraphErrors*) dataFile->Get(responseName);
    mc = (TGraphErrors*) mcPhotonJetFile->Get(responseName);

    drawGraphs(data, mc, "Balancing", "p_{t}^{2^{nd} jet} / p_{t}^{#gamma}", "Jet response (raw jets)", "#eta #leq 1.3", lumi, outputName.Data());

    responseName = TString::Format("%s/extrap_resp_mpf_%s_graph", rootFolder.Data(), etaName.c_str());
    outputName = TString::Format("%s/extrap_resp_mpf_%s.pdf", directoryName.Data(), etaName.c_str());
    data = (TGraphErrors*) dataFile->Get(responseName);
    mc = (TGraphErrors*) mcPhotonJetFile->Get(responseName);

    drawGraphs(data, mc, "MPF", "p_{t}^{2^{nd} jet} / p_{t}^{#gamma}", "Jet response", "#eta #leq 1.3", lumi, outputName.Data());

    // Raw jets
    responseName = TString::Format("%s/extrap_resp_mpf_raw_%s_graph", rootFolder.Data(), etaName.c_str());
    outputName = TString::Format("%s/extrap_resp_mpf_raw_%s.pdf", directoryName.Data(), etaName.c_str());
    data = (TGraphErrors*) dataFile->Get(responseName);
    mc = (TGraphErrors*) mcPhotonJetFile->Get(responseName);

    drawGraphs(data, mc, "MPF", "p_{t}^{2^{nd} jet} / p_{t}^{#gamma}", "Jet response (raw #slashed{E_{t}})", "#eta #leq 1.3", lumi, outputName.Data());
  }

  //db->set_legendTitle("|#eta| < 1.3");
  //db->drawHisto_vs_pt(ptBins, "resp_mpf_eta013", "MPF Response", "", "Events", log);
  //db->drawHisto_vs_pt(ptBins, "resp_mpf_raw_eta013", "MPF Response (raw ME_{T})", "", "Events", log);
  for (size_t i = 0; i < etaBinningSize; i++) {

    const std::string& etaName = etaBinning.getBinName(i);
    std::cout << "Processing " << etaName << std::endl;
    
    TString responseName = TString::Format("%s/extrap_resp_balancing_%s_graph", rootFolder.Data(), etaName.c_str());
    TString outputName = TString::Format("%s/extrap_resp_combined_%s", directoryName.Data(), etaName.c_str());
    TGraphErrors* balancingData = (TGraphErrors*) dataFile->Get(responseName);
    TGraphErrors* balancingMC = (TGraphErrors*) mcPhotonJetFile->Get(responseName);

    responseName = TString::Format("%s/extrap_resp_mpf_%s_graph", rootFolder.Data(), etaName.c_str());
    TGraphErrors* mpfData = (TGraphErrors*) dataFile->Get(responseName);
    TGraphErrors* mpfMC = (TGraphErrors*) mcPhotonJetFile->Get(responseName);

    drawCombinedGraphs(balancingData, balancingMC, mpfData, mpfMC, "p_{t}^{2^{nd} jet} / p_{t}^{#gamma}", "Jet response", etaBinning.getBinTitle(i), lumi, outputName.Data());

    responseName = TString::Format("%s/extrap_resp_balancing_raw_%s_graph", rootFolder.Data(), etaName.c_str());
    outputName = TString::Format("%s/extrap_resp_combined_raw_%s", directoryName.Data(), etaName.c_str());
    balancingData = (TGraphErrors*) dataFile->Get(responseName);
    balancingMC = (TGraphErrors*) mcPhotonJetFile->Get(responseName);

    responseName = TString::Format("%s/extrap_resp_mpf_raw_%s_graph", rootFolder.Data(), etaName.c_str());
    mpfData = (TGraphErrors*) dataFile->Get(responseName);
    mpfMC = (TGraphErrors*) mcPhotonJetFile->Get(responseName);

    drawCombinedGraphs(balancingData, balancingMC, mpfData, mpfMC, "p_{t}^{2^{nd} jet} / p_{t}^{#gamma}", "Jet response (raw objects)", etaBinning.getBinTitle(i), lumi, outputName.Data());

  }

  // Eta < 1.3
  {
    const std::string& etaName = "eta013";
    std::cout << "Processing " << etaName << std::endl;
    
    TString responseName = TString::Format("%s/extrap_resp_balancing_%s_graph", rootFolder.Data(), etaName.c_str());
    TString outputName = TString::Format("%s/extrap_resp_combined_%s", directoryName.Data(), etaName.c_str());
    TGraphErrors* balancingData = (TGraphErrors*) dataFile->Get(responseName);
    TGraphErrors* balancingMC = (TGraphErrors*) mcPhotonJetFile->Get(responseName);

    responseName = TString::Format("%s/extrap_resp_mpf_%s_graph", rootFolder.Data(), etaName.c_str());
    TGraphErrors* mpfData = (TGraphErrors*) dataFile->Get(responseName);
    TGraphErrors* mpfMC = (TGraphErrors*) mcPhotonJetFile->Get(responseName);

    drawCombinedGraphs(balancingData, balancingMC, mpfData, mpfMC, "p_{t}^{2^{nd} jet} / p_{t}^{#gamma}", "Jet response", "#eta #leq 1.3", lumi, outputName.Data());

    responseName = TString::Format("%s/extrap_resp_balancing_raw_%s_graph", rootFolder.Data(), etaName.c_str());
    outputName = TString::Format("%s/extrap_resp_combined_raw_%s", directoryName.Data(), etaName.c_str());
    balancingData = (TGraphErrors*) dataFile->Get(responseName);
    balancingMC = (TGraphErrors*) mcPhotonJetFile->Get(responseName);

    responseName = TString::Format("%s/extrap_resp_mpf_raw_%s_graph", rootFolder.Data(), etaName.c_str());
    mpfData = (TGraphErrors*) dataFile->Get(responseName);
    mpfMC = (TGraphErrors*) mcPhotonJetFile->Get(responseName);

    drawCombinedGraphs(balancingData, balancingMC, mpfData, mpfMC, "p_{t}^{2^{nd} jet} / p_{t}^{#gamma}", "Jet response (raw objects)", "#eta #leq 1.3", lumi, outputName.Data());
  }

  return 0;
}


