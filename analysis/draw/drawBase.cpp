#include "drawBase.h"
#include "fitTools.h"

#include "TColor.h"
#include "TRegexp.h"
#include "TVirtualFitter.h"
#include <iostream>
#include <algorithm>

#include <boost/algorithm/string.hpp>

#include <sys/stat.h>

#define LIGHT_RED TColor::GetColor(0xcf, 0xa0, 0xa1)

drawBase::drawBase(const std::string& analysisType, const std::string& recoType, const std::string& jetAlgo, bool outputGraphs, const std::string& flags) {

  style_ = new TStyle("drawBaseStyle", "");
  style_->SetCanvasColor(0);
  style_->SetPadColor(0);
  style_->SetFrameFillColor(0);
  style_->SetStatColor(0);
  style_->SetOptStat(0);
  style_->SetTitleFillColor(0);
  style_->SetCanvasBorderMode(0);
  style_->SetPadBorderMode(0);
  style_->SetFrameBorderMode(0);
  style_->SetPadBottomMargin(0.12);
  style_->SetPadLeftMargin(0.12);

  // For the canvas:
  style_->SetCanvasBorderMode(0);
  style_->SetCanvasColor(kWhite);
  style_->SetCanvasDefH(600); //Height of canvas
  style_->SetCanvasDefW(600); //Width of canvas
  style_->SetCanvasDefX(0);   //POsition on screen
  style_->SetCanvasDefY(0);

  // For the Pad:
  style_->SetPadBorderMode(0);
  // style_->SetPadBorderSize(Width_t size = 1);
  style_->SetPadColor(kWhite);
  style_->SetPadGridX(false);
  style_->SetPadGridY(false);
  style_->SetGridColor(0);
  style_->SetGridStyle(3);
  style_->SetGridWidth(1);

  // For the frame:
  style_->SetFrameBorderMode(0);
  style_->SetFrameBorderSize(1);
  style_->SetFrameFillColor(0);
  style_->SetFrameFillStyle(0);
  style_->SetFrameLineColor(1);
  style_->SetFrameLineStyle(1);
  style_->SetFrameLineWidth(1);

//// For the histo:
//  // style_->SetHistFillColor(1);
//  // style_->SetHistFillStyle(0);
//  style_->SetHistLineColor(1);
//  style_->SetHistLineStyle(0);
//  style_->SetHistLineWidth(1);
//  // style_->SetLegoInnerR(Float_t rad = 0.5);
//  // style_->SetNumberContours(Int_t number = 20);

//  style_->SetEndErrorSize(2);
////  style_->SetErrorMarker(20);
//  style_->SetErrorX(0.);
//
//  style_->SetMarkerStyle(20);

////For the fit/function:
//  style_->SetOptFit(1);
//  style_->SetFitFormat("5.4g");
//  style_->SetFuncColor(2);
//  style_->SetFuncStyle(1);
//  style_->SetFuncWidth(1);

////For the date:
//  style_->SetOptDate(0);
//  // style_->SetDateX(Float_t x = 0.01);
//  // style_->SetDateY(Float_t y = 0.01);

//// For the statistics box:
//  style_->SetOptFile(0);
//  style_->SetOptStat(0); // To display the mean and RMS:   SetOptStat("mr");
//  style_->SetStatColor(kWhite);
//  style_->SetStatFont(42);
//  style_->SetStatFontSize(0.025);
//  style_->SetStatTextColor(1);
//  style_->SetStatFormat("6.4g");
//  style_->SetStatBorderSize(1);
//  style_->SetStatH(0.1);
//  style_->SetStatW(0.15);
//  // style_->SetStatStyle(Style_t style = 1001);
//  // style_->SetStatX(Float_t x = 0);
//  // style_->SetStatY(Float_t y = 0);

  // Margins:
  style_->SetPadTopMargin(0.05);
  style_->SetPadBottomMargin(0.15);//0.13);
  style_->SetPadLeftMargin(0.15);//0.16);
  style_->SetPadRightMargin(0.05);//0.02);

  // For the Global title:

  style_->SetOptTitle(0);
  style_->SetTitleFont(42);
  style_->SetTitleColor(1);
  style_->SetTitleTextColor(1);
  style_->SetTitleFillColor(10);
  style_->SetTitleFontSize(0.05);
  // style_->SetTitleH(0); // Set the height of the title box
  // style_->SetTitleW(0); // Set the width of the title box
  // style_->SetTitleX(0); // Set the position of the title box
  // style_->SetTitleY(0.985); // Set the position of the title box
  // style_->SetTitleStyle(Style_t style = 1001);
  // style_->SetTitleBorderSize(2);

  // For the axis titles:

  style_->SetTitleColor(1, "XYZ");
  style_->SetTitleFont(42, "XYZ");
  style_->SetTitleSize(0.05, "XYZ");
  // style_->SetTitleXSize(Float_t size = 0.02); // Another way to set the size?
  // style_->SetTitleYSize(Float_t size = 0.02);
  style_->SetTitleXOffset(1.15);//0.9);
  style_->SetTitleYOffset(1.4); // => 1.15 if exponents
  // style_->SetTitleOffset(1.1, "Y"); // Another way to set the Offset

  // For the axis labels:

  style_->SetLabelColor(1, "XYZ");
  style_->SetLabelFont(42, "XYZ");
  style_->SetLabelOffset(0.007, "XYZ");
  style_->SetLabelSize(0.045, "XYZ");

  // For the axis:

  style_->SetAxisColor(1, "XYZ");
  style_->SetStripDecimals(kTRUE);
  style_->SetTickLength(0.03, "XYZ");
  style_->SetNdivisions(510, "XYZ");
  style_->SetPadTickX(1);  // To get tick marks on the opposite side of the frame
  style_->SetPadTickY(1);

//// Change for log plots:
//  style_->SetOptLogx(0);
//  style_->SetOptLogy(0);
//  style_->SetOptLogz(0);

//// Postscript options:
//  style_->SetPaperSize(20.,20.);
//  // style_->SetLineScalePS(Float_t scale = 3);
//  // style_->SetLineStyleString(Int_t i, const char* text);
//  // style_->SetHeaderPS(const char* header);
//  // style_->SetTitlePS(const char* pstitle);

//  // style_->SetBarOffset(Float_t baroff = 0.5);
//  // style_->SetBarWidth(Float_t barwidth = 0.5);
//  // style_->SetPaintTextFormat(const char* format = "g");
//  // style_->SetPalette(Int_t ncolors = 0, Int_t* colors = 0);
//  // style_->SetTimeOffset(Double_t toffset);
//  // style_->SetHistMinimumZero(kTRUE);

//  // Additional settings for QCD-10-011
//  style_->SetLegendBorderSize(0);

  // Legend
  style_->SetLegendBorderSize(1);
  style_->SetLegendFillColor(kWhite);
  style_->SetLegendFont(42);

  style_->cd();

  outputGraphs_ = outputGraphs;
  analysisType_ = analysisType;
  recoType_ = recoType;
  jetAlgo_ = jetAlgo;

  lumi_ = 0.;
  flags_ = flags;

  //dataFiles_.file = 0;

  scaleFactor_ = 0.;
  rebin_ = 1;

  logx_ = false;

  xAxisMin_ = 9999.;
  xAxisMax_ = 9999.;

  yAxisMax_ = 9999.;
  yAxisMaxScale_ = 1.4;
  yAxisMaxScaleLog_ = 5.;

  markerSize_ = 1.6;
  getBinLabels_ = false;
  legendTitle_ = "";
  legendTextSize_ = 0.038;

  poissonAsymmErrors_ = true;

  pdf_aussi_ = false;
  noStack_ = false;

  isCMSArticle_ = false;

  additionalLabel_ = 0;

  lastHistos_mcHistoSum_ = 0;

  // this is needed to avoid the same-histogram problem:
  TH1F::AddDirectory(kFALSE);
}



drawBase::~drawBase() {

  if (dataFiles_.size() != 0) {
    for (unsigned i = 0; i < dataFiles_.size(); ++i) {
      delete dataFiles_[i].file;
      dataFiles_[i].file = 0;
    }
  }

  if (mcFiles_.size() != 0) {
    for (unsigned i = 0; i < mcFiles_.size(); ++i) {
      delete mcFiles_[i].file;
      mcFiles_[i].file = 0;
    }
  }


}


void drawBase::set_lumiNormalization(float givenLumi) {

  if (givenLumi == -1.) {
    scaleFactor_ = lumi_; //eventweights are set so that histos are number of expected evets @ 1 pb-1. lumi is now also in pb-1
  } else {
    scaleFactor_ = givenLumi; //this is set to normalize MC histograms to a given int luminosity (if plotted with no data)
    lumi_ = givenLumi; //givenlumi is in pb-1
  }
}

void drawBase::set_shapeNormalization() {

  scaleFactor_ = -1.;
  if (dataFiles_.size() == 0) {
    noStack_ = true;
  }

}

void drawBase::set_isCMSArticle(bool set) {
  isCMSArticle_ = set;
}




void drawBase::drawHisto_vs_pt(int nBinsPt, float* ptBins, const std::string& name, const std::string& axisName, const std::string& units, const std::string& instanceName, bool log_aussi, int legendQuadrant, std::string flags, const std::string& labelText) {

  std::vector<float> ptBins_vect;

  for (int iBin = 0; iBin < nBinsPt; ++iBin) {
    ptBins_vect.push_back(ptBins[iBin]);
  }

  //drawHisto_vs_pt(ptBins_vect, name, axisName, units, instanceName, log_aussi, legendQuadrant, flags, labelText);
}



void drawBase::drawHisto_vs_pt(std::vector<std::pair<float, float> > ptBins, const std::string& name, const std::string& axisName, const std::string& units, const std::string& instanceName, bool log_aussi, int legendQuadrant, const std::string& labelText) {


  bool isMPF = TString(name).Contains("mpf", TString::kIgnoreCase);
  // Ignore bin between 3500 - 7000
  //int number_of_plots = ptBins.size() - 1;
  int number_of_plots = ptBins.size();

  std::string ptPhotMean_name = "ptPhotMean";
  std::string ptJetGenMean_name = "ptJetGenMean";

/*  TH2D* h2_ptPhot_data = 0;
  TH2D* h2_ptPhot_mc = 0;
  TH2D* h2_ptJetGen_mc = 0;*/

// hp_ptPhot_data = (TProfile*)dataFiles_[0].file->Get(ptPhotMean_name.c_str());
// hp_ptPhot_mc = (TProfile*)mcFiles_[0].file->Get(ptPhotMean_name.c_str());
// hp_ptJetGen_mc = (TProfile*)mcFiles_[0].file->Get("ptJetGenMean");

/*  h2_ptPhot_data = (TH2D*)dataFiles_[0].file->Get(ptPhotMean_name.c_str());
  h2_ptPhot_mc = (mcFiles_.size() > 0) ? (TH2D*)mcFiles_[0].file->Get(ptPhotMean_name.c_str()) : 0;
  h2_ptJetGen_mc = (mcFiles_.size() > 0) ? (TH2D*)mcFiles_[0].file->Get(ptJetGenMean_name.c_str()) : 0;

  if (mcFiles_.size() > 1) {
    for (unsigned i = 1; i < mcFiles_.size(); ++i) {
      //TProfile* hp_ptPhot_mc2 = (TProfile*)mcFiles_[i].file->Get(ptPhotMean_name.c_str());
      //TProfile* hp_ptJetGen_mc2 = (TProfile*)mcFiles_[i].file->Get("ptJetGenMean");
      TH2D* h2_ptPhot_mc2 = (TH2D*)mcFiles_[i].file->Get(ptPhotMean_name.c_str());
      TH2D* h2_ptJetGen_mc2 = (TH2D*)mcFiles_[i].file->Get(ptJetGenMean_name.c_str());
      //hp_ptPhot_mc->Add( hp_ptPhot_mc2 );
      //hp_ptJetGen_mc->Add( hp_ptJetGen_mc2 );
      h2_ptPhot_mc->Add(h2_ptPhot_mc2);
      h2_ptJetGen_mc->Add(h2_ptJetGen_mc2);
    }
  }
*/

  TGraphErrors* gr_response_vs_pt = new TGraphErrors(0);
  gr_response_vs_pt->SetName("response_vs_pt");

  TGraphErrors* gr_responseMC_vs_pt = new TGraphErrors(0);
  gr_responseMC_vs_pt->SetName("responseMC_vs_pt");

  TGraphErrors* gr_responseGEN_vs_pt = new TGraphErrors(0);
  gr_responseGEN_vs_pt->SetName("responseGEN_vs_pt");

  TGraphErrors* gr_resolution_vs_pt = new TGraphErrors(0);
  gr_resolution_vs_pt->SetName("resolution_vs_pt");

  TGraphErrors* gr_resolutionMC_vs_pt = new TGraphErrors(0);
  gr_resolutionMC_vs_pt->SetName("resolutionMC_vs_pt");

  TGraphErrors* gr_resolutionGEN_vs_pt = new TGraphErrors(0);
  gr_resolutionGEN_vs_pt->SetName("resolutionGEN_vs_pt");

  TGraphErrors* gr_purity_vs_pt = new TGraphErrors(0);
  gr_purity_vs_pt->SetName("purity_vs_pt");

  std::string histoName = name;

  for (int iplot = 0; iplot < number_of_plots; ++iplot) {

    //if( flags!="" ) histoName = histoName + "_" + flags;
    //
    std::pair<float, float> currentBin = ptBins[iplot];
    float ptMean = (currentBin.first + currentBin.second) / 2.;

    TString ptRange = TString::Format("ptPhot_%d_%d", (int) currentBin.first, (int) currentBin.second);

    // Set shape normalization for comparing shapes, even if we are in
    // prescaled region
    double oldScaleFactor = scaleFactor_;
    scaleFactor_ = -1;

    // pt phot cut label
    TString labelPtPhot = TString::Format("%d < p_{T}^{#gamma} < %d GeV/c", (int) currentBin.first, (int) currentBin.second);
    drawHisto(std::string(name + "_" + ptRange), axisName, units, instanceName, log_aussi, legendQuadrant, labelPtPhot.Data(), true, false);

    scaleFactor_ = oldScaleFactor;

    // save vs pt info:


    bool hasData = (lastHistos_data_.size() > 0);
    bool hasMC = (lastHistos_mc_.size() > 0);

    //Float_t dataResponse = (noDATA) ? 0. : dataHistos[0]->GetMean();
    //Float_t dataResponseErr = (noDATA) ? 0. : dataHistos[0]->GetMeanError();
    //Float_t dataRMS = (noDATA) ? 0. : dataHistos[0]->GetRMS();
    //Float_t dataRMSErr = (noDATA) ? 0. : dataHistos[0]->GetMeanError();

    Float_t meanTruncFraction = 0.99;
    Float_t rmsTruncFraction = 0.99;

    Float_t dataResponse = 0.;
    Float_t dataResponseErr = 0.;
    Float_t dataRMS = 0.;
    Float_t dataRMSErr = 0.;

    if (hasData) {
      fitTools::getTruncatedMeanAndRMS(lastHistos_data_[0], dataResponse, dataResponseErr, dataRMS, dataRMSErr, meanTruncFraction, rmsTruncFraction);
    }

    Float_t dataResolution = (hasData) ? dataRMS / dataResponse : 0.;
    Float_t dataResolutionErr = (hasData) ? sqrt(dataRMSErr * dataRMSErr / (dataResponse * dataResponse) + dataResolution * dataResolution * dataResponseErr * dataResponseErr / (dataResponse * dataResponse * dataResponse * dataResponse)) : 0.;


    if (hasData) {

      //if (h1_thisPhotPt_data->GetEntries() > 4) {
        gr_response_vs_pt->SetPoint(iplot, ptMean, dataResponse);
        gr_response_vs_pt->SetPointError(iplot, 0., dataResponseErr);
      //}

      if (dataResolution > 0.0005) {
        gr_resolution_vs_pt->SetPoint(iplot, ptMean, dataResolution);
        gr_resolution_vs_pt->SetPointError(iplot, 0., dataResolutionErr);
      }

    }

    Float_t mcResponse = 0.;
    Float_t mcResponseErr = 0.;
    Float_t mcRMS = 0.;
    Float_t mcRMSErr = 0.;

    if (hasMC) {
      fitTools::getTruncatedMeanAndRMS(lastHistos_mcHistoSum_, mcResponse, mcResponseErr, mcRMS, mcRMSErr, meanTruncFraction, rmsTruncFraction);
    }

    Float_t mcResolution = (!hasMC) ? 0. : mcRMS / mcResponse;
    Float_t mcResolutionErr = (!hasMC) ? 0. : sqrt(mcRMSErr * mcRMSErr / (mcResponse * mcResponse) + mcResolution * mcResolution * mcResponseErr * mcResponseErr / (mcResponse * mcResponse * mcResponse * mcResponse));

    if (hasMC) {

      //gr_responseMC_vs_pt->SetPoint(iplot, ptMeanMC, mcResponse);
      //gr_responseMC_vs_pt->SetPointError(iplot, ptMeanErrMC, mcResponseErr);

      //gr_resolutionMC_vs_pt->SetPoint(iplot, ptMeanMC, mcResolution);
      //gr_resolutionMC_vs_pt->SetPointError(iplot, ptMeanErrMC, mcResolutionErr);

      gr_responseMC_vs_pt->SetPoint(iplot, ptMean, mcResponse);
      gr_responseMC_vs_pt->SetPointError(iplot, 0., mcResponseErr);

      gr_resolutionMC_vs_pt->SetPoint(iplot, ptMean, mcResolution);
      gr_resolutionMC_vs_pt->SetPointError(iplot, 0., mcResolutionErr);

      float purityNum = lastHistos_mc_[0]->Integral(0, lastHistos_mc_[0]->GetNbinsX() + 1);
      float purityNumErr = lastHistos_mc_[0]->Integral(0, lastHistos_mc_[0]->GetNbinsX() + 1) / ((float)lastHistos_mc_[0]->GetEntries());
      float purityDenom = 0.;
      float purityDenomErr = 0.;
      for (unsigned iHisto = 0; iHisto < lastHistos_mc_.size(); ++iHisto) {
        purityDenom +=  lastHistos_mc_[iHisto]->Integral(0, lastHistos_mc_[iHisto]->GetNbinsX() + 1);
        purityDenomErr += lastHistos_mc_[iHisto]->Integral(0, lastHistos_mc_[iHisto]->GetNbinsX() + 1) * lastHistos_mc_[iHisto]->Integral(0, lastHistos_mc_[iHisto]->GetNbinsX() + 1) / ((float)lastHistos_mc_[iHisto]->GetEntries() * lastHistos_mc_[iHisto]->GetEntries());
      }
      purityDenomErr = sqrt(purityDenomErr);
      float purity = purityNum / purityDenom;
      float purityErr = sqrt(purityNumErr * purityNumErr / (purityDenom * purityDenom) + purityNum * purityNum * purityDenomErr * purityDenomErr / (purityDenom * purityDenom * purityDenom * purityDenom));
      //gr_purity_vs_pt->SetPoint(iplot, ptMeanMC, purity);
      //gr_purity_vs_pt->SetPointError(iplot, ptMeanErrMC, purityErr);
      gr_purity_vs_pt->SetPoint(iplot, ptMean, purity);
      gr_purity_vs_pt->SetPointError(iplot, 0., purityErr);


      // Get gen informations. To do that, we need to transform
      // resp_balancing_eta* in resp_balancing_gen_eta*
      std::string responseGenName = std::string(name + "_" + ptRange);
      boost::replace_all(responseGenName, "eta", "gen_eta");
      if (isMPF) {
        // For MPF, we don't have raw_gen
        boost::replace_all(responseGenName, "_raw", "");
      }

      TH1* responseGEN = static_cast<TH1*>(mcGet(0, responseGenName));
      for (unsigned i = 1; i < mcFiles_.size(); ++i) {
        TH1* responseGEN2 = static_cast<TH1*>(mcGet(i, responseGenName));
        responseGEN->Add(responseGEN2);
      }

      responseGEN->Scale(scaleFactor_);
      responseGEN->SetLineWidth(3);

      Float_t genResponse = 0.;
      Float_t genResponseErr = 0.;
      Float_t genRMS = 0.;
      Float_t genRMSErr = 0.;

      fitTools::getTruncatedMeanAndRMS(responseGEN, genResponse, genResponseErr, genRMS, genRMSErr, meanTruncFraction, rmsTruncFraction);

      Float_t genResolution = genRMS / genResponse;
      Float_t genResolutionErr = sqrt(genRMSErr * genRMSErr / (genResponse * genResponse) + genResolution * genResolution * genResponseErr * genResponseErr / (genResponse * genResponse * genResponse * genResponse));

      //TH1D* h1_thisPtJetGen = (TH1D*)h2_ptJetGen_mc->ProjectionY("thisPtJetGen", iplot + 1, iplot + 1);

      //Float_t ptMeanGEN = h1_thisPtJetGen->GetMean();
      //Float_t ptMeanErrGEN = h1_thisPtJetGen->GetMeanError();

      //gr_responseGEN_vs_pt->SetPoint(iplot, ptMeanGEN, genResponse);
      //gr_responseGEN_vs_pt->SetPointError(iplot, ptMeanErrGEN, genResponseErr);

      //gr_resolutionGEN_vs_pt->SetPoint(iplot, ptMeanGEN, genResolution);
      //gr_resolutionGEN_vs_pt->SetPointError(iplot, ptMeanErrGEN, genResolutionErr);

      gr_responseGEN_vs_pt->SetPoint(iplot, ptMean, genResponse);
      gr_responseGEN_vs_pt->SetPointError(iplot, 0., genResponseErr);

      gr_resolutionGEN_vs_pt->SetPoint(iplot, ptMean, genResolution);
      gr_resolutionGEN_vs_pt->SetPointError(iplot, 0., genResolutionErr);

    }

  } // for pt bins


  std::string graphFileName = "PhotonJetGraphs_" + get_fullSuffix() + ".root";
  TFile* graphFile = TFile::Open(graphFileName.c_str(), "update");
  graphFile->cd();

  TString graphName = TString::Format("%s_data_vs_pt", name.c_str()); // something like resp_balancing_eta011_data_vs_pt
  gr_response_vs_pt->SetName(graphName);
  gr_response_vs_pt->Write();

  graphName = TString::Format("%s_mc_vs_pt", name.c_str());
  gr_responseMC_vs_pt->SetName(graphName);
  gr_responseMC_vs_pt->Write();

  graphName = TString::Format("%s_gen_vs_pt", name.c_str());
  gr_responseGEN_vs_pt->SetName(graphName);
  gr_responseGEN_vs_pt->Write();

  graphName = TString::Format("%s_purity_vs_pt", name.c_str());
  gr_purity_vs_pt->SetName(graphName);
  gr_purity_vs_pt->Write();

  std::string resolutionName = name;
  boost::replace_all(resolutionName, "resp", "resolution");
  
  graphName = TString::Format("%s_data_vs_pt", resolutionName.c_str()); // something like resolution_balancing_eta011_data_vs_pt
  gr_resolution_vs_pt->SetName(graphName);
  gr_resolution_vs_pt->Write();

  graphName = TString::Format("%s_mc_vs_pt", resolutionName.c_str()); 
  gr_resolutionMC_vs_pt->SetName(graphName);
  gr_resolutionMC_vs_pt->Write();

  graphName = TString::Format("%s_gen_vs_pt", resolutionName.c_str());
  gr_resolutionGEN_vs_pt->SetName(graphName);
  gr_resolutionGEN_vs_pt->Write();

  graphFile->Close();

  
//gStyle->SetPadTickX(1);
//gStyle->SetPadTickY(1);

  bool noDATA = (gr_response_vs_pt->GetN() == 0);
  bool noMC = (gr_responseMC_vs_pt->GetN() == 0);

  int canvasHeight = (noMC) ? 600 : 800;
  TCanvas* c1 = new TCanvas("c1", "c1", 600, canvasHeight);
  c1->cd();

  // Data / MC comparison
  TPad* pad_hi = new TPad("pad_hi", "", 0., 0.33, 0.99, 0.99);
  pad_hi->Draw();
  pad_hi->SetLogx();
  pad_hi->SetLeftMargin(0.12);
  pad_hi->SetBottomMargin(0.015);

  // Data / MC ratio
  TPad* pad_lo = new TPad("pad_lo", "", 0., 0., 0.99, 0.33);
  pad_lo->Draw();
  pad_lo->SetLogx();
  pad_lo->SetLeftMargin(0.12);
  pad_lo->SetTopMargin(1.);
  pad_lo->SetBottomMargin(0.3);

  float ptPhotMax = ptBins[ptBins.size() - 1].second;
  float ptPhotMin = ptBins[0].first;

  TGraphErrors* gr_resp_ratio = 0;
  Float_t scale_uncert = (recoType_ == "Calo") ? 0.1 : 0.1;

  TH2* h2_axes_lo_resp = NULL;

  TLine* line_one = new TLine(ptPhotMin, 1., ptPhotMax, 1.);
  TLine* line_plus_resp = new TLine(ptPhotMin, 1.05, ptPhotMax, 1.05);
  TLine* line_minus_resp = new TLine(ptPhotMin, 0.95, ptPhotMax, 0.95);

  if (!noDATA && !noMC) {  //ugly will have to fix (cloning the TCanvas?)

    pad_lo->cd();
    
    h2_axes_lo_resp = new TH2D("axes_lo_resp", "", 10, ptPhotMin, ptPhotMax, 10, 0.86, 1.14);

    h2_axes_lo_resp->SetXTitle("Photon p_{T} [GeV/c]");
    h2_axes_lo_resp->SetYTitle("Data / MC");
    h2_axes_lo_resp->GetXaxis()->SetTitleOffset(1.2);
    h2_axes_lo_resp->GetYaxis()->SetTitleOffset(0.55);
    h2_axes_lo_resp->GetXaxis()->SetTickLength(0.06);
    h2_axes_lo_resp->GetXaxis()->SetMoreLogLabels();
    h2_axes_lo_resp->GetXaxis()->SetNoExponent();
    //h2_axes_lo_resp->GetXaxis()->SetLabelSize(0.);
    h2_axes_lo_resp->GetXaxis()->SetLabelSize(0.085);
    h2_axes_lo_resp->GetYaxis()->SetLabelSize(0.07);
    h2_axes_lo_resp->GetXaxis()->SetTitleSize(0.09);
    h2_axes_lo_resp->GetYaxis()->SetTitleSize(0.08);
    h2_axes_lo_resp->GetYaxis()->SetNdivisions(7, kTRUE);
    h2_axes_lo_resp->Draw("");

    line_one->Draw("same");

    //line_plus_resp->SetLineColor(46);
    line_plus_resp->SetLineWidth(2);
    line_plus_resp->SetLineStyle(2);

    //line_minus_resp->SetLineColor(46);
    line_minus_resp->SetLineWidth(2);
    line_minus_resp->SetLineStyle(2);

    gr_resp_ratio = this->get_graphRatio(gr_response_vs_pt, gr_responseMC_vs_pt);
    gr_resp_ratio->SetName("response_ratio");
    gr_resp_ratio->SetMarkerStyle(20);
    gr_resp_ratio->SetMarkerSize(1.5);
    gr_resp_ratio->SetMarkerColor(kBlue - 6);

    TF1* ratioFit = new TF1("ratioFit", "[0]", ptPhotMin, ptPhotMax);
    ratioFit->SetParameter(0, 0.);
    ratioFit->SetLineColor(46);
    ratioFit->SetLineWidth(2);
    gr_resp_ratio->Fit(ratioFit, "RQ");
    //std::cout << "-> ChiSquare: " << constline->GetChisquare() << "   NDF: " << constline->GetNDF() << std::endl;

    double fitValue = ratioFit->GetParameter(0);
    double fitError = ratioFit->GetParError(0);

    //TBox* errors = new TBox(ptPhotMin, fitValue - fitError, ptPhotMax, fitValue + fitError);
    //errors->SetFillColor(kBlue - 10);
    //errors->SetFillStyle(1001);

    TH1D* errors = new TH1D("errors", "errors", 100, ptPhotMin, ptPhotMax);
    (TVirtualFitter::GetFitter())->GetConfidenceIntervals(errors);
    errors->SetStats(false);
    errors->SetFillColor(LIGHT_RED);
    errors->SetLineColor(46);

    TPaveText* fitlabel = new TPaveText(0.55, 0.77, 0.88, 0.83, "brNDC");
    fitlabel->SetTextSize(0.08);
    fitlabel->SetFillColor(0);
    TString fitLabelText = TString::Format("Fit: %.3f #pm %.3f", fitValue, fitError);
    fitlabel->AddText(fitLabelText);
    fitlabel->Draw("same");

    line_plus_resp->Draw("same");
    line_minus_resp->Draw("same");

    errors->Draw("e3 same");
    //ratioFit->Draw("same");

    gr_resp_ratio->Draw("P same");

    gPad->RedrawAxis();

    pad_hi->cd();

  } // if !nodata && !nomc

  TH2D* h2_axes = new TH2D("axes_again", "", 10, ptPhotMin, ptPhotMax, 10, 0.4, 1.25);
  //h2_axes->SetXTitle("Photon p_{T} [GeV/c]");
  h2_axes->SetYTitle("Jet p_{T} response");
  //h2_axes->SetYTitle("< p_{T}^{jet} / p_{T}^{#gamma} >");
  h2_axes->GetXaxis()->SetTitleOffset(1.1);
  h2_axes->GetYaxis()->SetTitleOffset(1.2);
  h2_axes->GetYaxis()->SetTitleSize(0.045);
  //h2_axes->GetXaxis()->SetMoreLogLabels();
  //h2_axes->GetXaxis()->SetNoExponent();
  if (! noMC) {
    h2_axes->GetXaxis()->SetLabelSize(0.);
  }

  h2_axes->Draw();

  Float_t labelTextSize = 0.035;
  TPaveText* label_algo = get_labelAlgo(2);

  TLegend* legend = new TLegend(0.55, 0.15, 0.92, 0.38, legendTitle_.c_str());
  legend->SetFillColor(kWhite);
  legend->SetFillStyle(0);
  legend->SetTextSize(legendTextSize_);
  if (! isMPF) {
    if (!noDATA) {
      legend->AddEntry(gr_response_vs_pt, "Data (#gamma+Jet)", "P");
    }
    if (!noMC) {
      legend->AddEntry(gr_responseMC_vs_pt, "Simulation (#gamma+Jet)", "P");
    }
  } else {
    if (!noDATA) {
      legend->AddEntry(gr_response_vs_pt, "Data (MPF)", "P");
    }
    if (!noMC) {
      legend->AddEntry(gr_responseMC_vs_pt, "Simulation (MPF)", "P");
    }
  }
  if (!noMC) {
    //legend->AddEntry(gr_responseGEN_vs_pt, "True Response", "P");
  }
  legend->Draw("same");

  Float_t cmsTextSize = 0.043;
  TPaveText* label_cms = get_labelCMS(1);
  label_cms->SetTextSize(cmsTextSize);

  //Float_t sqrtTextSize = 0.041;
  TPaveText* label_sqrt = get_labelSqrt(1);

  label_cms->Draw("same");
  label_sqrt->Draw("same");
  label_algo->Draw("same");


  if (!noMC) {
    /*
    gr_responseGEN_vs_pt->SetMarkerStyle(29);
    gr_responseGEN_vs_pt->SetMarkerColor(46);
    gr_responseGEN_vs_pt->SetMarkerSize(2.);
    gr_responseGEN_vs_pt->Draw("Psame");
    */

    gr_responseMC_vs_pt->SetMarkerStyle(24);
    gr_responseMC_vs_pt->SetMarkerSize(1.5);
    gr_responseMC_vs_pt->SetMarkerColor(kBlue - 6);
    gr_responseMC_vs_pt->SetLineColor(kBlue - 6);
    gr_responseMC_vs_pt->Draw("Psame");
  }

  if (!noDATA) {
    if (noMC) {
      gr_response_vs_pt->SetMarkerColor(TColor::GetColor(0, 0, 153));
      gr_response_vs_pt->SetLineColor(TColor::GetColor(0, 0, 153));
      gr_response_vs_pt->SetLineWidth(1.);
      gr_response_vs_pt->SetMarkerStyle(21);
      gr_response_vs_pt->SetMarkerSize(1);
    } else {
      gr_response_vs_pt->SetMarkerStyle(20);
      gr_response_vs_pt->SetMarkerSize(1.5);
      gr_response_vs_pt->SetMarkerColor(kBlue - 6);
    }

    gr_response_vs_pt->Draw("Psame");
  }

  gPad->RedrawAxis();

  std::string canvName = outputdir_ + "/" + name + "_vs_pt";

  if (noMC) {
    std::string canvName_eps = canvName + ".eps";
    c1->SaveAs(canvName_eps.c_str());
    //std::string canvName_png = canvName + ".png";
    //c1->SaveAs(canvName_png.c_str());
  }

  std::string canvName_fit_eps = canvName + "_FITLINE.eps";
  c1->SaveAs(canvName_fit_eps.c_str());

  // ----------------------------------------------------
  //             and now resolutions:
  // ----------------------------------------------------

  TH2* h2_axes_lo_reso = new TH2D("axes_lo_reso", "", 10, ptPhotMin, ptPhotMax, 10, (1. - 6.*scale_uncert), (1. + 6.*scale_uncert));

  if (!noDATA && !noMC) {

    pad_lo->cd();

    h2_axes_lo_reso->SetXTitle("Photon p_{T} [GeV/c]");
    h2_axes_lo_reso->SetYTitle("Data / MC");
    h2_axes_lo_reso->GetXaxis()->SetTitleOffset(1.2);
    h2_axes_lo_reso->GetYaxis()->SetTitleOffset(0.55);
    h2_axes_lo_reso->GetXaxis()->SetTickLength(0.06);
    h2_axes_lo_reso->GetXaxis()->SetMoreLogLabels();
    h2_axes_lo_reso->GetXaxis()->SetNoExponent();
    h2_axes_lo_reso->GetXaxis()->SetLabelSize(0.085);
    h2_axes_lo_reso->GetYaxis()->SetLabelSize(0.07);
    h2_axes_lo_reso->GetXaxis()->SetTitleSize(0.09);
    h2_axes_lo_reso->GetYaxis()->SetTitleSize(0.08);
    h2_axes_lo_reso->GetYaxis()->SetNdivisions(5, kTRUE);
    h2_axes_lo_reso->Draw("");

    line_one->Draw("same");

    TLine* line_plus_reso = new TLine(ptPhotMin, 1. + 2.*scale_uncert, ptPhotMax, 1. + 2.*scale_uncert);
    //line_plus_reso->SetLineColor(46);
    //line_plus_reso->SetLineWidth(2);
    line_plus_reso->SetLineStyle(2);
    line_plus_reso->Draw("same");

    TLine* line_minus_reso = new TLine(ptPhotMin, 1. - 2.*scale_uncert, ptPhotMax, 1. - 2.*scale_uncert);
    //line_minus_reso->SetLineColor(46);
    //line_minus_reso->SetLineWidth(2);
    line_minus_reso->SetLineStyle(2);
    line_minus_reso->Draw("same");

    TGraphErrors* gr_reso_ratio = this->get_graphRatio(gr_resolution_vs_pt, gr_resolutionMC_vs_pt);
    gr_reso_ratio->SetName("reso_ratio");
    gr_reso_ratio->SetMarkerStyle(20);
    gr_reso_ratio->SetMarkerSize(1.8);
    gr_reso_ratio->Draw("P");

    TF1* constline = new TF1("constline", "[0]", ptPhotMin, ptPhotMax);
    constline->SetParameter(0, 1.);
    //constline->SetLineColor(8);
    //constline->SetLineColor(38);
    constline->SetLineColor(46);
    //constline->SetLineStyle(3);
    constline->SetLineWidth(3);
    gr_reso_ratio->Fit(constline, "RQ");
    //std::cout << "-> ChiSquare: " << constline->GetChisquare() << "   NDF: " << constline->GetNDF() << std::endl;

    TPaveText* fitlabel = new TPaveText(0.55, 0.4, 0.88, 0.45, "brNDC");
    fitlabel->SetTextSize(0.08);
    fitlabel->SetFillColor(0);
    char fitLabelText[150];
    sprintf(fitLabelText, "FIT: %.3f #pm %.3f", constline->GetParameter(0), constline->GetParError(0));
    fitlabel->AddText(fitLabelText);
    fitlabel->Draw("same");
    //line_plus_resp->Draw("same");
    constline->Draw("same");
    gr_reso_ratio->Draw("P same");
    gPad->RedrawAxis();

    pad_hi->cd();

  } // if !nodata and !nomc

  TH2D* h2_axes2 = new TH2D("axes_again2", "", 10, ptPhotMin, ptPhotMax, 10, 0., 1.);
  //h2_axes2->SetXTitle("Photon p_{T} [GeV/c]");
  h2_axes2->SetYTitle("Resolution");
  //h2_axes2->GetXaxis()->SetTitleOffset(1.1);
  h2_axes2->GetYaxis()->SetTitleOffset(1.5);
  //h2_axes2->GetXaxis()->SetMoreLogLabels();
  //h2_axes2->GetXaxis()->SetNoExponent();

  if (! noMC) {
    h2_axes2->GetXaxis()->SetLabelSize(0.);
  }

  h2_axes2->Draw();

  TPaveText* label_cms2 = get_labelCMS(1);
  label_cms2->SetTextSize(cmsTextSize);
  //TPaveText* label_cms2 = new TPaveText(0.58, 0.83, 0.75, 0.87, "brNDC");
  //label_cms2->SetFillColor(kWhite);
  //label_cms2->SetTextSize(cmsTextSize);
  //label_cms2->SetTextFont(62);
  //label_cms2->AddText(label_CMS_text.c_str());

  TPaveText* label_sqrt2 = get_labelSqrt(1);
  //TPaveText* label_sqrt2 = new TPaveText(0.58, 0.78, 0.75, 0.82, "brNDC");
  //label_sqrt2->SetFillColor(kWhite);
  //label_sqrt2->SetTextSize(sqrtTextSize);
  //label_sqrt2->SetTextFont(42);
  //label_sqrt2->AddText(label_sqrt_text.c_str());

  TPaveText* label_algo2 = get_labelAlgo(2);
  //TPaveText* label_algo2 = new TPaveText(0.27, 0.82, 0.32, 0.86, "brNDC");
  //label_algo2->SetFillColor(kWhite);
  //label_algo2->SetTextSize(labelTextSize);
  //label_algo2->AddText(jetAlgoName.c_str());

  TLegend* legend2 = new TLegend(0.5, 0.5, 0.85, 0.73, legendTitle_.c_str());
  legend2->SetFillColor(kWhite);
  legend2->SetFillStyle(0);
  legend2->SetTextSize(labelTextSize);
  if (! isMPF) {
    if (!noDATA) {
      legend2->AddEntry(gr_resolution_vs_pt, "Data (#gamma+Jet)", "P");
    }
    if (!noMC) {
      legend2->AddEntry(gr_resolutionMC_vs_pt, "Simulation (#gamma+Jet)", "P");
    }
  } else {
    if (!noDATA) {
      legend2->AddEntry(gr_resolution_vs_pt, "Data (MPF)", "P");
    }
    if (!noMC) {
      legend2->AddEntry(gr_resolutionMC_vs_pt, "Simulation (MPF)", "P");
    }

  }
  if (! noMC) {
    legend2->AddEntry(gr_resolutionGEN_vs_pt, "True Resolution", "P");
  }

  legend2->Draw("same");

  if (!noMC) {
    gr_resolutionGEN_vs_pt->SetMarkerStyle(29);
    gr_resolutionGEN_vs_pt->SetMarkerColor(46);
    gr_resolutionGEN_vs_pt->SetMarkerSize(2.);
    gr_resolutionGEN_vs_pt->Draw("Psame");

    gr_resolutionMC_vs_pt->SetMarkerStyle(24);
    gr_resolutionMC_vs_pt->SetMarkerSize(1.8);
    gr_resolutionMC_vs_pt->SetMarkerColor(kBlack);
    gr_resolutionMC_vs_pt->SetLineColor(kBlack);
    gr_resolutionMC_vs_pt->Draw("Psame");
  }

  if (!noDATA) {
    if (noMC) {
      gr_resolution_vs_pt->SetMarkerColor(TColor::GetColor(0, 0, 153));
      gr_resolution_vs_pt->SetLineColor(TColor::GetColor(0, 0, 153));
      gr_resolution_vs_pt->SetLineWidth(1.);
      gr_resolution_vs_pt->SetMarkerStyle(21);
      gr_resolution_vs_pt->SetMarkerSize(1.);
    } else {
      gr_resolution_vs_pt->SetMarkerStyle(20);
      gr_resolution_vs_pt->SetMarkerSize(1.8);
      gr_resolution_vs_pt->SetMarkerColor(kBlack);
    }

    gr_resolution_vs_pt->Draw("Psame");
  }

  label_cms2->Draw("same");
  label_sqrt2->Draw("same");
  label_algo2->Draw("same");

  gPad->RedrawAxis();

  canvName = outputdir_ + "/" + resolutionName + "_vs_pt";

  if (outputGraphs_) {
    std::string canvName_eps = canvName + ".eps";
    c1->SaveAs(canvName_eps.c_str());
    //std::string canvName_png = canvName + ".png";
    //c1->SaveAs(canvName_png.c_str());
  }

  delete h2_axes;
  h2_axes = 0;
  delete h2_axes2;
  h2_axes2 = 0;
  delete h2_axes_lo_resp;
  h2_axes_lo_resp = 0;
  delete h2_axes_lo_reso;
  h2_axes_lo_reso = 0;
  delete c1;
  c1 = 0;

  //gStyle->SetPadTickX(0);
  //gStyle->SetPadTickY(0);*/

}

void drawBase::drawHisto(const std::string& name, const std::string& axisName, const std::string& units, const std::string& instanceName, bool log_aussi, int legendQuadrant, const std::string& labelText, bool add_jetAlgoText, bool drawRatio, double fitMin, double fitMax) {

  std::vector<TH1*> dataHistos;
  for (unsigned int iData = 0; iData < dataFiles_.size(); iData++) {
    dataHistos.push_back(static_cast<TH1*>(dataGet(iData, name)));
  }

  std::vector<TH1*> mcHistos;
  for (size_t i = 0; i < mcFiles_.size(); i++) {
    TH1* histo = static_cast<TH1*>(mcGet(i, name.c_str()));
    if (histo) {
      mcHistos.push_back(histo);
    }
  }

  if (mcHistos.size() == 0) {
    std::cout << "Histo " << name << " not found in MC. Drawing only datas" << std::endl;
  }

  std::vector<TH1*> mcHistos_superimp;
  TH1* mcHisto0_superimp = 0;
  if (mcFiles_superimp_.size() > 0 && mcFiles_superimp_[0].file != 0) {
    mcHisto0_superimp = (TH1*)mcFiles_superimp_[0].file->Get(name.c_str());
  }
  if (mcHisto0_superimp != 0) {
    mcHistos_superimp.push_back(mcHisto0_superimp);
  }

  drawHisto_fromHistos(dataHistos, mcHistos, mcHistos_superimp, name, axisName, units, instanceName, log_aussi, legendQuadrant, "", labelText, add_jetAlgoText, drawRatio, fitMin, fitMax);


} //drawhisto

void drawBase::drawHisto_fromTree(const std::string& treeName, const std::string& varName, const std::string& selection, int nBins, float xMin, float xMax, const std::string& name, const std::string& axisName, const std::string& units, const std::string& instanceName, bool log_aussi, int legendQuadrant, const std::string& flags, const std::string& labelText, bool add_jetAlgoText) {


  // need this = true here. love these root features.
  TH1F::AddDirectory(kTRUE);


  bool noDATA = false;
  bool noMC = false;


  std::string histoName = name;
  if (flags != "") {
    histoName = histoName + "_" + flags;
  }

  std::vector<TH1*> dataHistos;
  for (unsigned int iData = 0; iData < dataFiles_.size(); iData++) {
    TTree* tree = (TTree*)dataFiles_[iData].file->Get(treeName.c_str());
    if (tree == 0) {
      std::cout << "Didn't find tree '" << treeName << "' in data file: " << dataFiles_[iData].file->GetName() << std::endl;
      std::cout << "Skipping." << std::endl;
      continue;
    }
    char histoName[500];
    sprintf(histoName, "%s_%d", name.c_str(), iData);
    TH1* newHisto = new TH1D(histoName, "", nBins, xMin, xMax);
    tree->Project(histoName, varName.c_str(), selection.c_str());
    dataHistos.push_back(newHisto);
  }

  noDATA = (dataHistos.size() == 0);


  std::vector<TH1*> mcHistos;
  TTree* treeMC = (TTree*)mcFiles_[0].file->Get(treeName.c_str());
  if (treeMC == 0) {
    std::cout << "Didn't find tree '" << treeName << "' in MC file: " << mcFiles_[0].file->GetName() << std::endl;
    std::cout << "Skipping." << std::endl;
  } else {
    char histoNameMC[500];
    sprintf(histoNameMC, "%sMC_0", name.c_str());
    TH1* mcHisto0 = new TH1D(histoNameMC, "", nBins, xMin, xMax);
    treeMC->Project(histoNameMC, varName.c_str(), selection.c_str());
    mcHistos.push_back(mcHisto0);
  }

  noMC = (mcHistos.size() == 0);


  if (noDATA && noMC) {
    std::cout << "Didn't find histo '" << histoName << "'. Skipping." << std::endl;
    return;
  }


  if (mcFiles_.size() > 1) {
    for (unsigned int iMC = 1; iMC < mcFiles_.size(); iMC++) {
      TTree* tree = (TTree*)mcFiles_[iMC].file->Get(treeName.c_str());
      if (tree == 0) {
        std::cout << "Didn't find tree '" << treeName << "' in MC file: " << mcFiles_[iMC].file->GetName() << std::endl;
        std::cout << "Skipping." << std::endl;
        continue;
      }
      char histoNameMC[500];
      sprintf(histoNameMC, "%sMC_%d", name.c_str(), iMC);
      TH1* newHisto = new TH1D(histoNameMC, "", nBins, xMin, xMax);
      tree->Project(histoNameMC, varName.c_str(), selection.c_str());
      mcHistos.push_back(newHisto);
    } //for mc files
  } // if mcfiles > 1

  //TFile* file = TFile::Open("PROVA.root", "recreate");
  //for( unsigned i=0; i<mcHistos.size(); ++i ) mcHistos[i]->Write();
  //file->Close();
  //exit(13);

  // superimposed mc histos (for now only one):
  std::vector<TH1*> mcHistos_superimp;
  //TH1D* mcHisto0_superimp = 0;
  if (mcFiles_superimp_.size() > 0 && mcFiles_superimp_[0].file != 0) {
    TTree* tree = (TTree*)mcFiles_superimp_[0].file->Get(treeName.c_str());
    if (tree == 0) {
      std::cout << "Didn't find tree '" << treeName << "' in superimposed MC file: " << mcFiles_superimp_[0].file->GetName() << std::endl;
      std::cout << "Skipping." << std::endl;
    } else {
      char histoNameMC[500];
      sprintf(histoNameMC, "%sMCsuperimp_0", name.c_str());
      TH1* newHisto = new TH1D(histoNameMC, "", nBins, xMin, xMax);
      tree->Project(histoNameMC, varName.c_str(), selection.c_str());
      mcHistos_superimp.push_back(newHisto);
    }
  }


  // put it back to false:
  TH1F::AddDirectory(kTRUE);

  drawHisto_fromHistos(dataHistos, mcHistos, mcHistos_superimp, name, axisName, units, instanceName, log_aussi, legendQuadrant, flags, labelText, add_jetAlgoText);

} //drawhisto_fromTree




void drawBase::drawHisto_fromHistos(std::vector<TH1*> dataHistos, std::vector<TH1*> mcHistos, std::vector<TH1*> mcHistos_superimp, const std::string& name, const std::string& axisName, const std::string& units, const std::string& instanceName, bool log_aussi, int legendQuadrant, const std::string& flags, const std::string& labelText, bool add_jetAlgoText, bool drawRatio/* = true*/, double fitMin/* = 0*/, double fitMax/* = 8000*/) {

  bool noDATA = false;
  bool noMC = false;

  if (dataHistos.size() == 0) {
    noDATA = true;
  }

  if (mcHistos.size() == 0) {
    noMC = true;
  }


  // FIRST: SET BASIC AESTHETICS FOR DATA HISTO(S)
  int markerStyle_default = 20;
  int markerColor_default = 1;
  for (unsigned iData = 0; iData < dataHistos.size(); ++iData) {
    dataHistos[iData]->Rebin(rebin_);

    dataFiles_[iData].lineColor = kBlack;
    dataFiles_[iData].lineWidth = 1.;

    if (dataFiles_[iData].markerStyle != -1) {
      dataHistos[iData]->SetMarkerStyle(dataFiles_[iData].markerStyle);
    } else {
      if (noMC) {
        dataHistos[iData]->SetMarkerStyle(21);
        dataFiles_[iData].markerStyle = 21;
      } else {
        dataFiles_[iData].markerStyle = markerStyle_default;
        dataHistos[iData]->SetMarkerStyle(markerStyle_default++);  // make it change at every histo
      }
    }

    if (dataFiles_[iData].fillStyle != -1) {
      dataHistos[iData]->SetMarkerSize(0);
      dataHistos[iData]->SetFillStyle(dataFiles_[iData].fillStyle);
      dataHistos[iData]->SetFillColor(dataFiles_[iData].fillColor);

      dataFiles_[iData].markerSize = 0;

      if (dataFiles_[iData].fillStyle == 1001) {
        dataHistos[iData]->SetLineColor(kBlack);
        dataHistos[iData]->SetLineWidth(0);

        dataFiles_[iData].lineColor = kBlack;
        dataFiles_[iData].lineWidth = 0.;
      }
    }

    if (dataFiles_[iData].fillColor != -1) {
      dataHistos[iData]->SetMarkerColor(dataFiles_[iData].fillColor);
    } else {
      if (noMC) {
        dataHistos[iData]->SetMarkerColor(TColor::GetColor(0, 0, 153));
        dataHistos[iData]->SetLineColor(TColor::GetColor(0, 0, 153));
        dataHistos[iData]->SetLineWidth(1.);

        dataFiles_[iData].fillColor = TColor::GetColor(0, 0, 153);
        dataFiles_[iData].lineColor = TColor::GetColor(0, 0, 153);
        dataFiles_[iData].lineWidth = 1.;
      } else {
        dataFiles_[iData].fillColor = markerColor_default;
        dataHistos[iData]->SetMarkerColor(markerColor_default++);  // make it change at every histo
      }
    }

    //if (noMC) {
      dataHistos[iData]->SetMarkerSize(1.);
      dataFiles_[iData].markerSize = 1.;
    //}
  }

  // SECOND: SET BASIC AESTHETICS FOR MC HISTO(S) and CREATE MC HISTO SUM
  TH1D* mcHisto_sum = 0;
  float fillColor_default = 1;
  float fillStyle_default = 3004;
  if (!noMC) {
    if (mcFiles_[0].fillColor == -1) {
      mcHistos[0]->SetFillColor(fillColor_default++);  //so that it changes at every histo
    } else {
      mcHistos[0]->SetFillColor(mcFiles_[0].fillColor);
    }
    if (noStack_) {
      if (mcFiles_[0].fillStyle != 1001) {
        mcHistos[0]->SetLineColor(mcFiles_[0].fillColor);
        mcHistos[0]->SetLineWidth(2);
      }
    }
    if (mcFiles_[0].fillStyle == -1) {
      if (noStack_) { //default is solid fill (if stacked)
        mcHistos[0]->SetFillStyle(fillStyle_default++);  //so that it changes at every histo
      }
    } else {
      if (noStack_) { //default is solid fill (if stacked)
        mcHistos[0]->SetFillStyle(mcFiles_[0].fillStyle);
      }
    }
    if (mcFiles_[0].markerStyle != -1) {
      mcHistos[0]->SetMarkerStyle(mcFiles_[0].markerStyle);
      mcHistos[0]->SetMarkerColor(mcFiles_[0].fillColor);
      mcHistos[0]->SetMarkerSize(markerSize_);
      mcHistos[0]->SetLineWidth(0);
    }
    mcHistos[0]->Rebin(rebin_);
    mcHistos[0]->Scale(mcFiles_[0].weight);

    mcHisto_sum = new TH1D(*((TH1D*)mcHistos[0]->Clone()));


    if (mcHistos.size() > 1) {
      for (unsigned i = 1; i < mcHistos.size(); ++i) {
        mcHistos[i]->Rebin(rebin_);
        mcHistos[i]->Scale(mcFiles_[i].weight);
        mcHisto_sum->Add((TH1D*)(mcHistos[i]->Clone()));
        if (mcFiles_[i].fillColor == -1) {
          mcHistos[i]->SetFillColor(fillColor_default++);  //so that it changes at every histo
        } else {
          mcHistos[i]->SetFillColor(mcFiles_[i].fillColor);
        }
        if (noStack_) {
          if (mcFiles_[i].fillStyle != 1001) {
            mcHistos[i]->SetLineColor(mcFiles_[i].fillColor);
            mcHistos[i]->SetLineWidth(2);
          }
        }
        if (mcFiles_[i].fillStyle == -1) {
          if (noStack_) { //default is solid fill (if stacked)
            mcHistos[i]->SetFillStyle(fillStyle_default++);  //so that it changes at every histo
          }
        } else {
          if (noStack_) { //default is solid fill (if stacked)
            mcHistos[i]->SetFillStyle(mcFiles_[i].fillStyle);
          }
        }
        if (mcFiles_[i].markerStyle != -1) {
          mcHistos[i]->SetMarkerStyle(mcFiles_[i].markerStyle);
          mcHistos[i]->SetMarkerColor(mcFiles_[i].fillColor);
          mcHistos[i]->SetMarkerSize(markerSize_);
          mcHistos[i]->SetLineWidth(0);
        }
      } //for mc files
    } //if mc files size > 1
  } // if !nomc




  // superimposed MC histos (for now only one):
  if (mcHistos_superimp.size() > 0) {
    mcHistos_superimp[0]->SetLineColor(mcFiles_superimp_[0].lineColor);
    mcHistos_superimp[0]->SetLineWidth(2);
    mcHistos_superimp[0]->Rebin(rebin_);
    mcHistos_superimp[0]->Scale(mcFiles_superimp_[0].weight);
  }

  // normalize:
  if (scaleFactor_ > 0.) {

    if (!noMC) {
      mcHisto_sum->Scale(scaleFactor_);
      for (unsigned i = 0; i < mcHistos.size(); ++i) {
        mcHistos[i]->Scale(scaleFactor_);
      }
      for (unsigned i = 0; i < mcHistos_superimp.size(); ++i) {
        mcHistos_superimp[i]->Scale(scaleFactor_);
      }
    }

  } else { //scale factor < 0 --> normalize to shapes

    if (!noDATA && !noMC) {  //normalize mc to data shape
      // default: choose first data histo:
      Float_t dataIntegral = dataHistos[0]->Integral(0, dataHistos[0]->GetNbinsX() + 1);
      Float_t mcIntegral = mcHisto_sum->Integral(0, mcHisto_sum->GetNbinsX() + 1);
      mcHisto_sum->Scale(dataIntegral / mcIntegral);
      for (unsigned i = 0; i < mcHistos.size(); ++i) {
        mcHistos[i]->Scale(dataIntegral / mcIntegral);
      }
      //} else if( noDATA ) { //normalize each MC to its area
  } else { //normalize each histo to its area
    // first: MC
    if (!noMC) {
      Float_t mcIntegral_sum = mcHisto_sum->Integral(0, mcHisto_sum->GetNbinsX() + 1);
      //Float_t mcIntegral_sum = mcHisto_sum->GetEntries();
      mcHisto_sum->Scale(1. / mcIntegral_sum);
      for (unsigned i = 0; i < mcHistos.size(); ++i) {
        Float_t mcIntegral = mcHistos[i]->Integral(0, mcHistos[i]->GetNbinsX() + 1);
        //Float_t mcIntegral = mcHistos[i]->GetEntries();
        if (noStack_) {
          mcHistos[i]->Scale(1. / mcIntegral);
        } else {
          mcHistos[i]->Scale(1. / mcIntegral_sum);
        }
      }
    }
    // second: data
    for (unsigned i = 0; i < dataHistos.size(); ++i) {
      Float_t dataIntegral = dataHistos[i]->GetEntries();
      dataHistos[i]->Scale(1. / dataIntegral);
    }
    //} else if( !noDATA && noMC ) {
    //  // nothing to do here, as data does not have to be normalized
    //} else {
    //  std::cout << "DATA and MC files not properly initialized. Will not normalize." << std::endl;
  }
  } //if scalefactor


  // create stack:
  THStack* mcHisto_stack = new THStack();
  int nHistos = mcHistos.size();
  for (int i = 0; i < nHistos; ++i) {
    mcHisto_stack->Add((TH1D*)(mcHistos[nHistos - i - 1]->Clone()), "HISTO");
  }
  mcHisto_stack->SetName("stack");

  TH1* refHisto = (noDATA) ? mcHistos[0] : dataHistos[0];

  //Float_t yAxisMaxScale_ = (name=="phiJet" || name=="etaJet" || name=="ptSecondJetRel" || name=="phiPhot" || name=="etaPhot" ) ? 1.8 : 1.6;
  //if( name=="phiPhot" || name=="etaPhot" ) yAxisMaxScale_=2.;
  //if(name=="clusterMajPhotReco" || name=="clusterMinPhotReco") yAxisMaxScale_ = 2.;
  Int_t nBinsx = refHisto->GetNbinsX();
  Float_t xMin = refHisto->GetXaxis()->GetXmin();
  Float_t xMax = refHisto->GetXaxis()->GetXmax();
  Float_t yMax_data = (noDATA) ? 0. : dataHistos[0]->GetMaximum();
  //Float_t yMax_mc = (noMC) ? 0. : mcHisto_sum->GetMaximum();
  std::string nostack_str = (noStack_) ? "nostack" : "";
  Float_t yMax_mc = (noMC) ? 0. : mcHisto_stack->GetMaximum(nostack_str.c_str());
  //if( scaleFactor_<0. ) yMax_mc /= mcHisto_sum->Integral(0, mcHisto_sum->GetNbinsX()+1);
  if (scaleFactor_ < 0. && noDATA) {
    for (size_t i = 0; i < mcHistos.size(); ++i) {
      if (mcHistos[i]->GetMaximum() > yMax_mc) {
        yMax_mc = mcHistos[i]->GetMaximum();
      }
    }
  }
  Float_t yMax = (yMax_data > yMax_mc) ? yAxisMaxScale_ * yMax_data : yAxisMaxScale_ * yMax_mc;
  Float_t yMin = 0.;

  LegendBox lb = get_legendBox(legendQuadrant);
  int totalNfiles = mcFiles_.size() + dataFiles_.size();
  //    if( dataFile_.file!=0 ) totalNfiles++;

  if (totalNfiles > 5) {
    yMax *= 1.2;
  }


  TLegend* legend = new TLegend(lb.xMin, lb.yMin, lb.xMax, lb.yMax, legendTitle_.c_str());
  legend->SetFillColor(kWhite);
  legend->SetTextSize(legendTextSize_);
  for (unsigned i = 0; i < dataHistos.size(); ++i)
    if (dataFiles_[i].fillStyle != -1) {
      legend->AddEntry(dataHistos[i], (dataFiles_[i].legendName).c_str(), "F");
    } else {
      legend->AddEntry(dataHistos[i], (dataFiles_[i].legendName).c_str(), "P");
    }
  for (unsigned i = 0; i < mcHistos.size(); ++i)  {
    if (mcFiles_[i].markerStyle == -1) {
      legend->AddEntry(mcHistos[i], (mcFiles_[i].legendName).c_str(), "F");
    } else {
      legend->AddEntry(mcHistos[i], (mcFiles_[i].legendName).c_str(), "P");
    }
  }
  for (unsigned i = 0; i < mcHistos_superimp.size(); ++i) {
    legend->AddEntry(mcHistos_superimp[i], (mcFiles_superimp_[i].legendName).c_str(), "L");
  }

  bool noBinLabels = true;
  TH2D* h2_axes = new TH2D("axes", "", nBinsx, xMin, xMax, 10, yMin, yMax);
  if (xAxisMin_ != 9999.) {
    h2_axes->GetXaxis()->SetRangeUser(xAxisMin_, xMax);
  }
  if (xAxisMax_ != 9999.) {
    h2_axes->GetXaxis()->SetRangeUser(xMin, xAxisMax_);
  }
  if (yAxisMax_ != 9999.) {
    h2_axes->GetYaxis()->SetRangeUser(yMin, yAxisMax_);
  }
  if (getBinLabels_) {
    for (int iBinx = 1; iBinx < nBinsx + 1; ++iBinx) {
      if (std::string(refHisto->GetXaxis()->GetBinLabel(iBinx)) != "") {
        noBinLabels = false;
      }
      h2_axes->GetXaxis()->SetBinLabel(iBinx, refHisto->GetXaxis()->GetBinLabel(iBinx));
    }
  }


  // create data graph (poisson asymm errors):
  TGraphAsymmErrors* graph_data_poisson = new TGraphAsymmErrors(0);
  if (dataHistos.size() == 1 && poissonAsymmErrors_) {
    if (noBinLabels) {
      graph_data_poisson = fitTools::getGraphPoissonErrors(dataHistos[0]);
    } else {
      graph_data_poisson = fitTools::getGraphPoissonErrors(dataHistos[0], "binWidth");
    }

    graph_data_poisson->SetMarkerStyle(dataFiles_[0].markerStyle);
    graph_data_poisson->SetMarkerSize(dataFiles_[0].markerSize);

    graph_data_poisson->SetLineColor(dataFiles_[0].fillColor);
    graph_data_poisson->SetLineWidth(dataFiles_[0].lineWidth);

    if (dataFiles_[0].fillStyle == -1) {
      graph_data_poisson->SetMarkerColor(dataFiles_[0].fillColor);
    } else {
      graph_data_poisson->SetFillStyle(dataFiles_[0].fillStyle);
      graph_data_poisson->SetFillColor(dataFiles_[0].fillColor);
    }

    if (noMC) {
    }
  }




  // axis titles:

  std::string xAxis = axisName;
  if (units != "") {
    xAxis += " [" + units + "]";
  }

  std::string yAxis = instanceName;

  if (scaleFactor_ < 0. && (dataFiles_.size() == 0 || mcFiles_.size() == 0)) {
    yAxis = "Normalized to Unity";
  } else {
    char yAxis_char[150];
    bool equalBins = true;
    for (int ibin = 1; ibin < refHisto->GetNbinsX(); ++ibin)
      if (refHisto->GetBinWidth(ibin) != refHisto->GetBinWidth(ibin + 1)) {
        equalBins = false;
      }

    //if( units!="" && equalBins ) {
    if (equalBins && noBinLabels) {
      std::string units_text = (units != "") ? (" " + units) : "";
      if ((refHisto->GetBinWidth(1)) < 0.1) {
        sprintf(yAxis_char, "%s / (%.2f%s)", instanceName.c_str(), refHisto->GetBinWidth(1), units_text.c_str());
      } else if (((int)(10.*refHisto->GetBinWidth(1)) % 10) == 0) {
        sprintf(yAxis_char, "%s / (%.0f%s)", instanceName.c_str(), refHisto->GetBinWidth(1), units_text.c_str());
      } else {
        sprintf(yAxis_char, "%s / (%.1f%s)", instanceName.c_str(), refHisto->GetBinWidth(1), units_text.c_str());
      }
    } else {
      sprintf(yAxis_char, "%s", instanceName.c_str());
    }
    std::string yAxis_str_tmp(yAxis_char);
    yAxis = yAxis_str_tmp;
  }



  h2_axes->SetXTitle(xAxis.c_str());
  h2_axes->SetYTitle(yAxis.c_str());

  if (!noBinLabels) {
    h2_axes->GetXaxis()->SetLabelSize(0.07);
  }

  if (drawRatio) {
    h2_axes->GetXaxis()->SetTitleOffset(1.1);
    h2_axes->GetYaxis()->SetTitleOffset(1.2);
    h2_axes->GetYaxis()->SetTitleSize(0.045);
    h2_axes->GetXaxis()->SetLabelSize(0.);
  }

  TPaveText* label_cms = get_labelCMS(0);
  TPaveText* label_sqrt = get_labelSqrt(0);


  TPaveText* label_cuts = 0;

  //TPaveText* label_bonus = new TPaveText(0.63, lb.yMin-0.07, 0.84, lb.yMin-0.02,  "brNDC");
  TPaveText* label_bonus = new TPaveText(0.65, 0.45, 0.9, 0.55, "brNDC");
  label_bonus->SetFillColor(kWhite);
  label_bonus->SetTextSize(0.030);
  if (add_jetAlgoText) {
    std::string jetAlgoText = get_algoName();
    label_bonus->AddText(jetAlgoText.c_str());
  }
  label_bonus->AddText(labelText.c_str());

  TCanvas* c1 = new TCanvas("c1", "c1", (drawRatio) ? 600 : 800, 800);
  c1->cd();

  // Data / MC comparison
  TVirtualPad* pad_hi = NULL; 
  TPad* pad_lo = NULL;
  
  if (drawRatio) {
    pad_hi = new TPad("pad_hi", "", 0., 0.33, 0.99, 0.99);
    pad_hi->Draw();
    //pad_hi->SetLogx();
    pad_hi->SetLeftMargin(0.12);
    pad_hi->SetBottomMargin(0.015);

    // Data / MC ratio
    pad_lo = new TPad("pad_lo", "", 0., 0., 0.99, 0.33);
    pad_lo->Draw();
    //pad_lo->SetLogx();
    pad_lo->SetLeftMargin(0.12);
    pad_lo->SetTopMargin(1.);
    pad_lo->SetBottomMargin(0.3);

    pad_hi->cd();
  } else {
    pad_hi = gPad;
  }

  if (logx_) {
    pad_hi->SetLogx();

    h2_axes->GetXaxis()->SetMoreLogLabels();
    h2_axes->GetXaxis()->SetNoExponent();
  }

  h2_axes->Draw("");
  legend->Draw("same");
  if (!noMC) {
    if (!noStack_) {
      mcHisto_stack->Draw("histo same");
      for (unsigned i = 0; i < mcHistos_superimp.size(); ++i) {
        int backwardsIndex = mcHistos_superimp.size() - 1 - i; //backwards is prettier: bg on the back, signal in front
        mcHistos_superimp[backwardsIndex]->Draw("h same");
      }
    } else {
      for (unsigned i = 0; i < mcHistos.size(); ++i) {
        int backwardsIndex = mcHistos.size() - 1 - i; //backwards is prettier: bg on the back, signal in front
        if (mcFiles_[backwardsIndex].markerStyle != -1) {
          mcHistos[backwardsIndex]->Draw("p same");
        } else {
          mcHistos[backwardsIndex]->Draw("h same");
        }
      }
    }
  } // if !nomc
  for (unsigned i = 0; i < dataHistos.size(); ++i) {
    if (dataHistos.size() == 1 && poissonAsymmErrors_) {
      graph_data_poisson->Draw("P same");
      //xframe->Draw("same");
    } else {
      int backwardsIndex = dataHistos.size() - 1 - i; //backwards is prettier: bg on the back, signal in front
      if (dataFiles_[backwardsIndex].fillStyle != -1) {
        dataHistos[backwardsIndex]->Draw("h same");
      } else {
        dataHistos[backwardsIndex]->Draw("e same");
      }
    }
    //if( dataFiles_[i].fillStyle!=-1 )
    //  dataHistos[i]->Draw("h same");
    //else
    //  dataHistos[i]->Draw("E same");
  }

  pad_hi->RedrawAxis();
  label_cms->Draw("same");
  label_sqrt->Draw("same");
  if (label_cuts != 0) {
    label_cuts->Draw("same");
  }
  if (labelText != "") {
    label_bonus->Draw("same");
  }
  if (additionalLabel_ != 0) {
    additionalLabel_->Draw("same");
  }

  // Draw ratio
  if (drawRatio && !noDATA && !noMC)
    drawHistRatio(pad_lo, dataHistos[0], mcHisto_sum, xAxis.c_str(), fitMin, fitMax);

  if (outputdir_ == "") {
    this->set_outputdir();
  }
  std::string canvasName = outputdir_ + "/" + name;
  if (flags != "") {
    canvasName = canvasName + "_" + flags;
  }

  if (outputGraphs_) {
    std::cout << "Saved " << canvasName << std::endl;
    std::string canvasName_eps = canvasName + ".eps";
    c1->SaveAs(canvasName_eps.c_str());
    std::string canvasName_png = canvasName + ".png";
    c1->SaveAs(canvasName_png.c_str());
    std::string canvasName_pdf = canvasName + ".pdf";
    if (pdf_aussi_) {
      c1->SaveAs(canvasName_pdf.c_str());
    }
  }

  if (log_aussi) {
    pad_hi->cd();

    // look for minimum in histos:
    float yMin_log = yMax;
    // first: MC
    if (!noMC) {
      if (noStack_) {
        for (size_t iHisto = 0; iHisto < mcHistos.size(); ++iHisto)
          for (int iBin = 1; iBin < mcHistos[iHisto]->GetNbinsX() + 1; ++iBin)
            if (mcHistos[iHisto]->GetBinContent(iBin) > 0. && mcHistos[iHisto]->GetBinContent(iBin) < yMin_log) {
              yMin_log = mcHistos[iHisto]->GetBinContent(iBin);
            }
      } else {
        for (int iBin = 1; iBin < mcHisto_sum->GetNbinsX() + 1; ++iBin)
          if (mcHisto_sum->GetBinContent(iBin) > 0. && mcHisto_sum->GetBinContent(iBin) < yMin_log) {
            yMin_log = mcHisto_sum->GetBinContent(iBin);
          }
      }
    } // if nomc
    // second: data
    for (size_t iHisto = 0; iHisto < dataHistos.size(); ++iHisto)
      for (int iBin = 1; iBin < dataHistos[iHisto]->GetNbinsX() + 1; ++iBin)
        if (dataHistos[iHisto]->GetBinContent(iBin) > 0. && dataHistos[iHisto]->GetBinContent(iBin) < yMin_log) {
          yMin_log = dataHistos[iHisto]->GetBinContent(iBin);
        }

    TH2D* h2_axes_log = new TH2D("axes_log", "", nBinsx, xMin, xMax, 10, 0.1 * yMin_log, yAxisMaxScaleLog_ * yMax);

    if (drawRatio) {
      h2_axes_log->GetXaxis()->SetTitleOffset(1.1);
      h2_axes_log->GetYaxis()->SetTitleOffset(1.2);
      h2_axes_log->GetYaxis()->SetTitleSize(0.045);
      h2_axes_log->GetXaxis()->SetLabelSize(0.);
    }

    //TH2D* h2_axes_log = new TH2D("axes_log", "", nBinsx, xMin, xMax, 10, 0.1*yMin_log, 100.*yMax);
    if (xAxisMin_ != 9999.) {
      h2_axes_log->GetXaxis()->SetRangeUser(xAxisMin_, xMax);
    }
    if (xAxisMax_ != 9999.) {
      h2_axes_log->GetXaxis()->SetRangeUser(xMin, xAxisMax_);
    }
    if (yAxisMax_ != 9999.) {
      h2_axes_log->GetYaxis()->SetRangeUser(0.1 * yMin_log, yAxisMaxScaleLog_ * yAxisMax_);
    }
    h2_axes_log->SetXTitle(xAxis.c_str());
    h2_axes_log->SetYTitle(yAxis.c_str());
    //h2_axes_log->GetXaxis()->SetTitleOffset(1.1);
    //h2_axes_log->GetYaxis()->SetTitleOffset(1.5);
    if (!noBinLabels) {
      h2_axes_log->GetXaxis()->SetLabelSize(0.07);
    }
    if (getBinLabels_) {
      for (int iBinx = 1; iBinx < nBinsx + 1; ++iBinx) {
        if (std::string(refHisto->GetXaxis()->GetBinLabel(iBinx)) != "") {
          h2_axes_log->GetXaxis()->SetBinLabel(iBinx, refHisto->GetXaxis()->GetBinLabel(iBinx));
        }
      }
    }
    pad_hi->SetLogy();
    if (name == "ptPhot" && analysisType_ == "PhotonJet") {
      c1->SetLogx();
      h2_axes_log->GetXaxis()->SetNoExponent();
      h2_axes_log->GetXaxis()->SetMoreLogLabels();
    }
    h2_axes_log->Draw("");
    legend->Draw("same");
    if (!noMC) {
      if (!noStack_) {
        mcHisto_stack->Draw("histo same");
        for (unsigned i = 0; i < mcHistos_superimp.size(); ++i) {
          int backwardsIndex = mcHistos_superimp.size() - 1 - i; //backwards is prettier: bg on the back, signal in front
          mcHistos_superimp[backwardsIndex]->Draw("h same");
        }

      } else {
        for (unsigned i = 0; i < mcHistos.size(); ++i) {
          int backwardsIndex = mcHistos.size() - 1 - i; //backwards is prettier: bg on the back, signal in front
          if (mcFiles_[backwardsIndex].markerStyle != -1) {
            mcHistos[backwardsIndex]->Draw("p same");
          } else {
            mcHistos[backwardsIndex]->Draw("h same");
          }
        }
      }
    } // if !nomc
    for (unsigned i = 0; i < dataHistos.size(); ++i) {
      if (dataHistos.size() == 1 && poissonAsymmErrors_) {
        graph_data_poisson->Draw("Psame");
      } else {
        int backwardsIndex = dataHistos.size() - 1 - i; //backwards is prettier: bg on the back, signal in front
        if (dataFiles_[backwardsIndex].fillStyle != -1) {
          dataHistos[backwardsIndex]->Draw("h same");
        } else {
          dataHistos[backwardsIndex]->Draw("e same");
        }
      }
      //if( dataFiles_[i].fillStyle!=-1 )
      //  dataHistos[i]->Draw("h same");
      //else
      //  dataHistos[i]->Draw("E same");
    }
    gPad->RedrawAxis();
    label_cms->Draw("same");
    label_sqrt->Draw("same");
    if (label_cuts != 0) {
      label_cuts->Draw("same");
    }
    if (labelText != "") {
      label_bonus->Draw("same");
    }
    if (additionalLabel_ != 0) {
      additionalLabel_->Draw("same");
    }

    if (outputGraphs_) {
      std::string canvasName_log = canvasName + "_log";
      std::string canvasName_eps = canvasName_log + ".eps";
      c1->SaveAs(canvasName_eps.c_str());
      std::string canvasName_png = canvasName_log + ".png";
      c1->SaveAs(canvasName_png.c_str());
      std::string canvasName_pdf = canvasName_log + ".pdf";
      if (pdf_aussi_) {
        c1->SaveAs(canvasName_pdf.c_str());
      }
    }
    delete h2_axes_log;
  }



  if (lastHistos_mcHistoSum_ != 0) {
    delete lastHistos_mcHistoSum_;
  }

  if (! noMC) {
    lastHistos_mcHistoSum_ = new TH1D(*mcHisto_sum);
  }


  lastHistos_data_.clear();
  lastHistos_mc_.clear();
  lastHistos_mc_superimp_.clear();

  for (unsigned iHisto = 0; iHisto < dataHistos.size(); ++iHisto) {
    TH1* newHisto = static_cast<TH1*>(dataHistos[iHisto]->Clone());
    lastHistos_data_.push_back(newHisto);
  }

  for (unsigned iHisto = 0; iHisto < mcHistos.size(); ++iHisto) {
    TH1* newHisto = static_cast<TH1*>(mcHistos[iHisto]->Clone());
    lastHistos_mc_.push_back(newHisto);
  }

  /*for (unsigned iHisto = 0; iHisto < mcHistos_superimp.size(); ++iHisto) {
    TH1D* newHisto = new TH1D(*(mcHistos_superimp[iHisto]));
    lastHistos_mc_superimp_.push_back(newHisto);
    }*/

  delete c1;
  delete legend;
  delete h2_axes;
  delete mcHisto_stack;
  delete mcHisto_sum;


  } //drawHisto

  void drawBase::drawProfile(const std::string& yVar, const std::string& xVar, int legendQuadrant) {

    std::string name = yVar + "_vs_" + xVar;
    if (xVar == "pt" || xVar == "ptCorr") {
      name = name + "_barrel";  //ugly fix for now
    }

    TProfile* dataProfile = (TProfile*)dataFiles_[0].file->Get(name.c_str());
    TProfile* mcProfile = (TProfile*)mcFiles_[0].file->Get(name.c_str()); //default: take first mc file. MUST BE FIXED

    if (dataProfile == 0 || mcProfile == 0) {
      std::cout << "Didn't find profile '" << name << "'. Continuing." << std::endl;
      return;
    }

    Float_t profile_xMin = dataProfile->GetXaxis()->GetXmin();
    Float_t profile_xMax = dataProfile->GetXaxis()->GetXmax();

    mcProfile->SetFillColor(38);
    //mcProfile->SetFillColor(kRed-7);
    //mcProfile->SetLineWidth(2);
    //mcProfile->SetLineColor(kRed);

    dataProfile->SetMarkerStyle(20);

    //Float_t etamax__rounded = (etamax_>2.5) ? 3. : 2.5;
    Float_t etamax_rounded = 3.;
    Float_t xMin = (xVar == "eta") ? -etamax_rounded : profile_xMin;
    Float_t xMax = (xVar == "eta") ?  etamax_rounded : profile_xMax;
    //Float_t xMin = profile_xMin;
    //Float_t xMax = profile_xMax;

    if (xVar == "Nch") {
      xMax = 11.5;
    }

    Float_t dataMax = dataProfile->GetMaximum();
    Float_t mcMax = mcProfile->GetMaximum();
    Float_t plotMax = (dataMax > mcMax) ? dataMax : mcMax;

    std::string xAxisName = get_axisName(xVar);
    std::string yAxisName = get_axisName(yVar);

    Float_t yAxisMaxScale_ = 1.5;
    if (yVar == "pt" || yVar == "ptCorr" || yVar == "Rch" || yVar == "PTch" || yVar == "Nch" || yVar == "Ngamma" || yVar == "Nnh" || yVar == "Rnh" || yVar == "Rgamma" || yVar == "Rgammanh") {
      yAxisMaxScale_ = 1.8;
    }

    TH2D* h2_axes = new TH2D("axes", "", 10, xMin, xMax, 10, 0., yAxisMaxScale_ * plotMax);
    h2_axes->SetXTitle(xAxisName.c_str());
    h2_axes->SetYTitle(yAxisName.c_str());
    h2_axes->GetXaxis()->SetTitleOffset(1.1);
    h2_axes->GetYaxis()->SetTitleOffset(1.2);


    LegendBox lb = get_legendBox(legendQuadrant);

    TLegend* legend = new TLegend(lb.xMin, lb.yMin, lb.xMax, lb.yMax);
    legend->SetFillColor(kWhite);
    legend->SetTextSize(legendTextSize_);
    legend->AddEntry(dataProfile, "Data", "P");
    legend->AddEntry(mcProfile, "Simulation", "F");

    TPaveText* label_cms = get_labelCMS(2);
    //TPaveText* label_cms = new TPaveText(0.25, 0.83, 0.42, 0.87, "brNDC");
    //label_cms->SetFillColor(kWhite);
    //label_cms->SetTextSize(0.038);
    //label_cms->SetTextFont(62);
    //std::string label_CMS_text = this->get_CMSText();
    //label_cms->AddText(label_CMS_text.c_str());

    TPaveText* label_sqrt = get_labelSqrt(2);
    //TPaveText* label_sqrt = new TPaveText(0.25, 0.78, 0.42, 0.82, "brNDC");
    //label_sqrt->SetFillColor(kWhite);
    //label_sqrt->SetTextSize(0.038);
    //label_sqrt->SetTextFont(42);
    //std::string label_sqrt_text = this->get_sqrtText();
    //label_sqrt->AddText(label_sqrt_text.c_str());

    Float_t label_cuts_xMin = 0.4;
    Float_t label_cuts_yMin = 0.55;
    Float_t label_cuts_xMax = 0.6;
    Float_t label_cuts_yMax = 0.7;

    //if( yVar=="pt" || yVar=="ptCorr" || yVar=="Rch" || yVar=="Rgamma" || yVar=="ETch" ) {
    //  label_cuts_xMin = 0.4;
    //  label_cuts_yMin = 0.65;
    //  label_cuts_xMax = 0.6;
    //  label_cuts_yMax = 0.8;
    //}

    if (xVar == "pt" || xVar == "ptCorr") {
      label_cuts_xMin = 0.20;
      label_cuts_xMax = 0.35;
    }



    TPaveText* label_cuts = new TPaveText(label_cuts_xMin, label_cuts_yMin, label_cuts_xMax, label_cuts_yMax, "brNDC");
    label_cuts->SetFillColor(kWhite);
    label_cuts->SetTextSize(0.035);
    label_cuts->SetTextFont(42);
    label_cuts->AddText("Anti-k_{T} 0.5 PFJets");
    if (xVar != "eta") {
      char etaRange_ch[100];
      sprintf(etaRange_ch, "|#eta| < %.1f", etamax_);
      label_cuts->AddText(etaRange_ch);
    }
    if (yVar != "pt" && yVar != "ptCorr") {
      char labelText[70];
      sprintf(labelText, "p_{T}^{%s} > %d GeV/c", raw_corr_.c_str(), pt_thresh_);
      label_cuts->AddText(labelText);
    }


    TCanvas* c1 = new TCanvas("c1", "c1", 800, 800);
    c1->cd();
    //if(name=="ptJet") c1->SetLogy();
    if (xVar == "pt" || xVar == "ptCorr") {
      c1->SetLogx();
      h2_axes->GetXaxis()->SetMoreLogLabels();
      h2_axes->GetXaxis()->SetNoExponent();
    }
    h2_axes->Draw("");
    mcProfile->Draw("histo same");
    dataProfile->Draw("E same");
    gPad->RedrawAxis();
    legend->Draw("same");
    label_cms->Draw("same");
    label_sqrt->Draw("same");
    label_cuts->Draw("same");

    if (outputGraphs_) {
      std::string canvasName = outputdir_ + "/" + name;
      std::string canvasName_eps = canvasName + ".eps";
      c1->SaveAs(canvasName_eps.c_str());
      std::string canvasName_png = canvasName + ".png";
      c1->SaveAs(canvasName_png.c_str());
      std::string canvasName_pdf = canvasName + ".pdf";
      if (pdf_aussi_) {
        c1->SaveAs(canvasName_pdf.c_str());
      }
    }


    delete c1;
    delete legend;
    delete h2_axes;

  } //drawProfile



  std::string drawBase::get_axisName(std::string name) {

    std::string axisName = "";

    if (name == "ptJet" || name == "pt") {
      axisName = "Jet p_{T}^{raw} [GeV/c]";
    }
    if (name == "ptCorrJet" || name == "ptCorr") {
      axisName = "Corrected p_{T} [GeV/c]";
    }
    if (name == "etaJet" || name == "eta") {
      axisName = "Jet #eta";
    }
    if (name == "phiJet" || name == "phi") {
      axisName = "Jet #phi";
    }
    if (name == "nCandJet") {
      axisName = "N Candidates";
    }
    if (name == "RchJet" || name == "Rch") {
      axisName = "Jet R_{ch}";
    }
    if (name == "RnhJet" || name == "Rnh") {
      axisName = "Jet R_{nh}";
    }
    if (name == "RgammaJet" || name == "Rgamma") {
      axisName = "Jet R_{#gamma}";
    }
    if (name == "RgammanhJet" || name == "Rgammanh") {
      axisName = "Jet R_{#gamma+nh}";
    }
    if (name == "EchJet" || name == "Ech") {
      axisName = "Jet E_{ch} [GeV]";
    }
    if (name == "EnhJet" || name == "Enh") {
      axisName = "Jet E_{nh} [GeV]";
    }
    if (name == "EgammaJet" || name == "Egamma") {
      axisName = "Jet E_{#gamma} [GeV]";
    }
    if (name == "EgammanhJet" || name == "Egammanh") {
      axisName = "Energy of Photons + Neutral Hadrons in Jet";
    }
    if (name == "PTchJet" || name == "PTch") {
      axisName = "p_{T}^{ch} [GeV/c]";
    }
    if (name == "PTnhJet" || name == "PTnh") {
      axisName = "p_{T}^{nh} [GeV/c]";
    }
    if (name == "PTgammaJet" || name == "PTgamma") {
      axisName = "p_{T}^{#gamma} [GeV/c]";
    }
    if (name == "asymmJet") {
      axisName = "Asymmetry";
    }
    if (name == "deltaPhiJet" || name == "deltaPhi") {
      axisName = "Photon-Jet Azimuth Difference [rad]";
    }
    if (name == "deltaPhi2ndJet" || name == "deltaPhi_2ndJet") {
      axisName = "Photon-2nd Jet Azimuth Difference [rad]";
    }
    if (name == "massJet") {
      axisName = "Jet Invariant Mass [GeV/c^{2}]";
    }
    if (name == "MoEJet") {
      axisName = "Jet Invariant Mass / Energy";
    }
    if (name == "ptOverMJet") {
      axisName = "Jet p_{T} / Invariant Mass";
    }
    if (name == "pOverMJet") {
      axisName = "Jet Momentum / Invariant Mass";
    }
    if (name == "diJetMass" || name == "JetJetMass") {
      axisName = "DiJet Invariant Mass [GeV/c^{2}]";
    }
    if (name == "deltaRjj") {
      axisName = "Jet-Jet #Delta R";
    }
    if (name == "deltaRZZ") {
      axisName = "Z-Z #Delta R";
    }
    if (name == "ptZZ") {
      axisName = "ZZ p_{T} [GeV/c]";
    }
    if (name == "ZZInvMass") {
      axisName = "ZZ Invariant Mass [GeV/c^{2}]";
    }
    if (name == "ZZInvMass") {
      axisName = "ZZ Invariant Mass [GeV/c^{2}]";
    }
    if (name == "LeptLeptMass") {
      axisName = "DiLepton Invariant Mass [GeV/c^{2}]";
    }
    if (name == "EleEleMass") {
      axisName = "DiElectron Invariant Mass [GeV/c^{2}]";
    }
    if (name == "MuMuMass") {
      axisName = "DiMuon Invariant Mass [GeV/c ^{2}]";
    }
    if (name == "EphotAveJet") {
      axisName = "Average Photon Energy in Jet [GeV]";
    }
    if (name == "NchJet" || name == "Nch") {
      axisName = "Number of Charged Hadrons in Jet";
    }
    if (name == "NgammaJet" || name == "Ngamma") {
      axisName = "Number of Photons in Jet";
    }
    if (name == "NnhJet" || name == "Nnh") {
      axisName = "Number of Neutral Hadrons in Jet";
    }
    if (name == "NgammanhJet" || name == "Ngammanh") {
      axisName = "Number of Photons + Neutral Hadrons in Jet";
    }
    if (name == "hcalIsoPhotReco") {
      axisName = "H / E (#DeltaR < 0.4)";
    }
    if (name == "ecalIsoPhotReco") {
      axisName = "ECAL Isolation (GT,  #DeltaR < 0.4)";
    }
    if (name == "nTrkIsoPhotReco") {
      axisName = "Number of Tracks in #DeltaR < 0.35";
    }
    if (name == "ptTrkIsoPhotReco") {
      axisName = "#Sigma p_{T}^{tracks in #DeltaR < 0.35} / p_{T}^{#gamma}";
    }
    if (name == "clusterMajPhotReco") {
      axisName = "Photon Cluster Major Axis";
    }
    if (name == "clusterMinPhotReco") {
      axisName = "Photon Cluster Minor Axis";
    }
    if (name == "ptPhot") {
      axisName = "Photon p_{T} [GeV/c]";
    }
    if (name == "phiPhot") {
      axisName = "Photon #phi";
    }
    if (name == "etaPhot") {
      axisName = "Photon #eta";
    }
    if (name == "pt2ndJet") {
      axisName = "Second Jet p_{T}^{raw} [GeV/c]";
    }
    if (name == "ptSecondJetRel") {
      axisName = "p_{T}^{2ndJet} / p_{T}^{#gamma}";
    }
    if (name == "response" || name == "response_loose" || name == "response_clusterOK") {
      axisName = "p_{T}^{jet} / p_{T}^{#gamma}";
    }
    if (name == "responseMPF") {
      axisName = "MPF Response";
    }

    return axisName;

  }




  void drawBase::drawStack(const std::string& varY, const std::string& varX, const std::string& etaRegion, const std::string& RECO_GEN, bool isData) const {

    TFile* file;
    if (isData) {
      file = dataFiles_[0].file;
    } else {
      file = mcFiles_[0].file;
    }

    std::string histoName;

    std::string suffix;

    if (RECO_GEN == "RECO") {
      suffix = "_vs_" + varX;
    } else if (RECO_GEN == "GEN") {
      suffix = "Gen_vs_" + varX;
    }

    if (varX == "eta") {
      suffix = suffix + "_stack";
    }

    if (etaRegion != "") {
      suffix = suffix + "_" + etaRegion;
    }

    histoName = "Rch" + suffix;
    //  std::cout << "-> Trying to get " << histoName << std::endl << std::flush;
    TH1D* h1_Rch = (TH1D*)file->Get(histoName.c_str());
    histoName = "Rgamma" + suffix;
    //  std::cout << "-> Trying to get " << histoName << std::endl << std::flush;
    TH1D* h1_Rgamma = (TH1D*)file->Get(histoName.c_str());
    histoName = "Rnh" + suffix;
    //  std::cout << "-> Trying to get " << histoName << std::endl << std::flush;
    TH1D* h1_Rnh = (TH1D*)file->Get(histoName.c_str());
    histoName = "Rmu" + suffix;
    //  std::cout << "-> Trying to get " << histoName << std::endl << std::flush;
    TH1D* h1_Rmu = (TH1D*)file->Get(histoName.c_str());
    histoName = "Re" + suffix;
    //  std::cout << "-> Trying to get " << histoName << std::endl << std::flush;
    TH1D* h1_Re = (TH1D*)file->Get(histoName.c_str());
    histoName = "Rhfhad" + suffix;
    //  std::cout << "-> Trying to get " << histoName << std::endl;
    TH1D* h1_Rhfhad = (TH1D*)file->Get(histoName.c_str());
    //  std::cout << "-> Trying to get " << histoName << std::endl;
    histoName = "Rhfem" + suffix;
    TH1D* h1_Rhfem = (TH1D*)file->Get(histoName.c_str());


    int nBins_stack = fitTools::getNbins_stack(varX);
    Double_t Lower_stack[nBins_stack];
    fitTools::getBins_stack(nBins_stack, Lower_stack, varX);


    histoName = varY + "ch_vs_" + varX + "_bis";
    TH1D* h1_Rch_stack = new TH1D(histoName.c_str(), "", nBins_stack - 1, Lower_stack);
    h1_Rch_stack->SetFillColor(kRed);

    histoName = varY + "gamma_vs_" + varX + "_bis";
    TH1D* h1_Rgamma_stack = new TH1D(histoName.c_str(), "", nBins_stack - 1, Lower_stack);
    h1_Rgamma_stack->SetFillColor(kBlue);

    histoName = varY + "nh_vs_" + varX + "_bis";
    TH1D* h1_Rnh_stack = new TH1D(histoName.c_str(), "", nBins_stack - 1, Lower_stack);
    h1_Rnh_stack->SetFillColor(kGreen);

    histoName = varY + "mu_vs_" + varX + "_bis";
    TH1D* h1_Rmu_stack = new TH1D(histoName.c_str(), "", nBins_stack - 1, Lower_stack);
    h1_Rmu_stack->SetFillColor(kOrange);

    histoName = varY + "e_vs_" + varX + "_bis";
    TH1D* h1_Re_stack = new TH1D(histoName.c_str(), "", nBins_stack - 1, Lower_stack);
    h1_Re_stack->SetFillColor(kCyan);

    histoName = varY + "hfhad_vs_" + varX + "_bis";
    TH1D* h1_Rhfhad_stack = new TH1D(histoName.c_str(), "", nBins_stack - 1, Lower_stack);
    h1_Rhfhad_stack->SetFillColor(kMagenta - 9);

    histoName = varY + "hfhem_vs_" + varX + "_bis";
    TH1D* h1_Rhfem_stack = new TH1D(histoName.c_str(), "", nBins_stack - 1, Lower_stack);
    h1_Rhfem_stack->SetFillColor(kBlue - 5);


    for (int i = 1; i < (nBins_stack); ++i) {
      h1_Rch_stack->SetBinContent(i, h1_Rch->GetBinContent(i));
      h1_Rgamma_stack->SetBinContent(i, h1_Rgamma->GetBinContent(i));
      h1_Rnh_stack->SetBinContent(i, h1_Rnh->GetBinContent(i));
      h1_Rmu_stack->SetBinContent(i, h1_Rmu->GetBinContent(i));
      h1_Re_stack->SetBinContent(i, h1_Re->GetBinContent(i));
      h1_Rhfhad_stack->SetBinContent(i, h1_Rhfhad->GetBinContent(i));
      h1_Rhfem_stack->SetBinContent(i, h1_Rhfem->GetBinContent(i));
    }


    THStack* stack = new THStack("stack", "");
    stack->Add(h1_Rch_stack);
    stack->Add(h1_Rgamma_stack);
    stack->Add(h1_Re_stack);
    stack->Add(h1_Rnh_stack);
    stack->Add(h1_Rhfhad_stack);
    stack->Add(h1_Rhfem_stack);
    stack->Add(h1_Rmu_stack);
    stack->Draw(); //this draw is magically needed

    //THStack* stack = new THStack("stack", "");
    //stack->Add(h1_Rch_vs_eta, "HISTO");
    //stack->Add(h1_Rgamma_vs_eta, "HISTO");
    //stack->Add(h1_Re_vs_eta, "HISTO");
    //stack->Add(h1_Rnh_vs_eta, "HISTO");
    //if( RECO_GEN=="RECO" ) {
    //  stack->Add(h1_Rhfhad_vs_eta, "HISTO");
    //  stack->Add(h1_Rhfem_vs_eta, "HISTO");
    //}
    //stack->Add(h1_Rmu_vs_eta, "HISTO");
    //stack->Draw(); //this draw is magically needed

    std::string varX_name;
    if (varX == "eta") {
      varX_name = "#eta";
    } else if (varX == "pt") {
      if (RECO_GEN == "RECO") {
        varX_name = "Raw p_{T}";
      } else {
        varX_name = "p_{T}";
      }
    } else if (varX == "ptCorr") {
      varX_name = "Corrected p_{T}";
    } else if (varX == "phi") {
      varX_name = "#phi";
    }

    std::string xTitle = (RECO_GEN == "RECO") ? "PFJet " : "GenJet ";
    xTitle = xTitle + varX_name;

    std::string yTitle;
    if (varY == "R") {
      yTitle = "Mean Fraction of Jet Energy";
    } else if (varY == "N") {
      yTitle = "Particle Multiplicity";
    } else if (varY == "Pt") {
      if (RECO_GEN == "RECO") {
        yTitle = "<p_{T}^{RECO}> [GeV/c]";
      } else if (RECO_GEN == "GEN") {
        yTitle = "<p_{T}^{GEN}> [GeV/c]";
      }
    }
    stack->GetXaxis()->SetTitle(xTitle.c_str());
    stack->GetYaxis()->SetTitle(yTitle.c_str());

    TPaveText* label_cms = get_labelCMS(2);
    //TPaveText* label_cms = new TPaveText(0.25, 0.83, 0.42, 0.87, "brNDC");
    //label_cms->SetFillColor(kWhite);
    //label_cms->SetTextSize(0.038);
    //label_cms->SetTextFont(62);
    //std::string label_CMS_text = this->get_CMSText();
    //label_cms->AddText(label_CMS_text.c_str());


    TPaveText* label_sqrt = new TPaveText(0.25, 0.78, 0.42, 0.82, "brNDC");
    label_sqrt->SetFillColor(kWhite);
    label_sqrt->SetTextSize(0.038);
    label_sqrt->SetTextFont(42);
    std::string label_sqrt_text;
    if (isData) {
      label_sqrt_text = "#sqrt{s} = 8 TeV, DATA";
    } else {
      label_sqrt_text = "#sqrt{s} = 8 TeV, MC";
    }
    label_sqrt->AddText(label_sqrt_text.c_str());

    Float_t yMin_cuts = (varX != "eta" && varX != "pt" && varX != "ptCorr") ? 0.77 : 0.78;
    Float_t yMax_cuts = (varX != "eta" && varX != "pt" && varX != "ptCorr") ? 0.89 : 0.88;
    Float_t textSize_cuts = (varX != "eta" && varX != "pt" && varX != "ptCorr") ? 0.03 : 0.035;

    TPaveText* label_cuts = new TPaveText(0.6, yMin_cuts, 0.8, yMax_cuts,  "brNDC");
    label_cuts->SetFillColor(kWhite);
    label_cuts->SetTextSize(textSize_cuts);
    label_cuts->SetTextFont(42);
    label_cuts->AddText("Anti-k_{T} R=0.5");
    std::string apexText = (RECO_GEN == "RECO") ? raw_corr_ : "GEN";
    //std::string apexText = raw_corr_;
    if (varX != "eta") {
      std::string etaRange = get_etaRangeText(etaRegion);
      label_cuts->AddText(etaRange.c_str());
    }
    if ((varX != "pt") && (varX != "ptCorr")) {
      char label_pt_text[100];
      sprintf(label_pt_text, "p_{T}^{%s} > %d GeV/c", apexText.c_str(), pt_thresh_);
      label_cuts->AddText(label_pt_text);
    }

    Float_t xMin, xMax;
    if (varX == "eta") {
      xMin = -5.5;
      xMax = 5.5;
    } else {
      xMin = Lower_stack[0];
      xMax = Lower_stack[nBins_stack - 1];
    }


    TH2D* h2_axes = new TH2D("axes", "", 10, xMin, xMax, 10, 0., 1.2);
    h2_axes->SetXTitle(xTitle.c_str());
    h2_axes->SetYTitle(yTitle.c_str());
    h2_axes->GetXaxis()->SetTitleOffset(1.1);
    h2_axes->GetYaxis()->SetTitleOffset(1.2);
    if (varX == "pt" || varX == "ptCorr") {
      h2_axes->GetXaxis()->SetMoreLogLabels();
      h2_axes->GetXaxis()->SetNoExponent();
    }

    //if( varX != "eta" )
    //  gStyle->SetPadTickY(1);

    TCanvas* c1 = new TCanvas("c1", "c1", 800, 800);
    //c1->SetLeftMargin(1.2);
    //c1->SetBottomMargin(1.2);
    c1->cd();
    if (varX == "pt" || varX == "ptCorr") {
      c1->SetLogx();
    }
    h2_axes->Draw();
    stack->Draw("same");
    label_cms->Draw("same");
    label_sqrt->Draw("same");
    label_cuts->Draw("same");
    gPad->RedrawAxis();


    std::string canvasName;
    if (RECO_GEN == "RECO") {
      if (isData) {
        canvasName = outputdir_ + "/stack_" + varY + "_vs_" + varX;
      } else {
        canvasName = outputdir_ + "/stackMC_" + varY + "_vs_" + varX;
      }
    } else if (RECO_GEN == "GEN") {
      canvasName = outputdir_ + "/stackGEN_" + varY + "_vs_" + varX;
    }

    if (outputGraphs_) {
      std::string canvasName_eps = canvasName + ".eps";
      std::string canvasName_png = canvasName + ".png";
      std::string canvasName_pdf = canvasName + ".pdf";
      c1->SaveAs(canvasName_eps.c_str());
      c1->SaveAs(canvasName_png.c_str());
      if (pdf_aussi_) {
        c1->SaveAs(canvasName_pdf.c_str());
      }
    }

    delete c1;
    delete h2_axes;

    delete h1_Rch_stack;
    delete h1_Rgamma_stack;
    delete h1_Rnh_stack;
    delete h1_Re_stack;
    delete h1_Rmu_stack;
    delete h1_Rhfhad_stack;
    delete h1_Rhfem_stack;

    //gStyle->SetPadTickY(0);


  } //drawStack




  void drawBase::compareDifferentHistos(const std::vector< HistoAndName > histos, const std::string saveVarName, const std::string xAxisName, const std::string& units, const std::string& instanceName, bool stacked, int legendQuadrant) {

    for (unsigned iData = 0; iData < dataFiles_.size(); ++iData) {
      compareDifferentHistos_singleFile(dataFiles_[iData], histos, saveVarName, xAxisName, units, instanceName, stacked, legendQuadrant);
    }
    for (unsigned iMC = 0; iMC < mcFiles_.size(); ++iMC) {
      compareDifferentHistos_singleFile(mcFiles_[iMC], histos, saveVarName, xAxisName, units, instanceName, stacked, legendQuadrant);
    }

  }




  void drawBase::compareDifferentHistos_singleFile(InputFile infile, const std::vector< HistoAndName > histosandnames, const std::string saveVarName, const std::string xAxisName, const std::string& units, const std::string& instanceName, bool stacked, int legendQuadrant) {

    bool normalized = (lumi_ <= 0.);

    std::vector< TH1F* > histos;
    std::vector<std::string> legendNames;

    for (unsigned i = 0; i < histosandnames.size(); ++i) {
      TH1F* thisHisto = (TH1F*)infile.file->Get((histosandnames[i].histoName).c_str());
      if (thisHisto != 0) {
        histos.push_back(thisHisto);
      } else {
        std::cout << "Didn't find histo '" << histosandnames[i].histoName << "' in file '" << infile.file->GetName() << "'. Skipping." << std::endl;
      }

      legendNames.push_back(histosandnames[i].legendName);

    }

    if (histos.size() == 0) {
      std::cout << "Didn't find anything. Exiting." << std::endl;
      return;
    }


    std::vector< int > fillColors;
    fillColors.push_back(38);
    fillColors.push_back(46);
    fillColors.push_back(kGray + 3);
    fillColors.push_back(kRed + 3);
    fillColors.push_back(30);
    fillColors.push_back(40);
    fillColors.push_back(kOrange);

    LegendBox lb = get_legendBox(legendQuadrant, &legendNames);

    TLegend* legend = new TLegend(lb.xMin, lb.yMin, lb.xMax, lb.yMax, legendTitle_.c_str());
    legend->SetFillColor(0);
    legend->SetTextSize(legendTextSize_);


    float xMin, xMax, yMin, yMax;
    yMin = 0.;

    THStack* stack = new THStack();

    for (unsigned iHisto = 0; iHisto < histos.size(); ++iHisto) {

      histos[iHisto]->Rebin(rebin_);

      if (normalized) {
        Float_t integral = histos[iHisto]->Integral(0, histos[iHisto]->GetNbinsX() + 1);
        histos[iHisto]->Scale(1. / integral);
      } else {
        histos[iHisto]->Scale(scaleFactor_);
      }

      // 1. look for axis ranges:
      float this_xMin = histos[iHisto]->GetXaxis()->GetXmin();
      float this_xMax = histos[iHisto]->GetXaxis()->GetXmax();
      float this_yMax = histos[iHisto]->GetMaximum();

      if (iHisto == 0) {
        xMin = this_xMin;
        xMax = this_xMax;
        yMax = this_yMax;
      } else {
        if (this_xMin < xMin) {
          xMin = this_xMin;
        }
        if (this_xMax > xMax) {
          xMax = this_xMax;
        }
        if (this_yMax > yMax) {
          yMax = this_yMax;
        }
      }

      // 2. set fill colors and styles
      if (!stacked) {
        histos[iHisto]->SetFillStyle(3004 + iHisto);
      }
      int colorIndex = (iHisto < fillColors.size()) ? fillColors[iHisto] : iHisto;
      histos[iHisto]->SetFillColor(colorIndex);
      if (!stacked) {
        histos[iHisto]->SetLineColor(colorIndex);
      }
      histos[iHisto]->SetLineWidth(3);

      if (histosandnames[iHisto].markerStyle != -1) {
        histos[iHisto]->SetMarkerStyle(histosandnames[iHisto].markerStyle);
        histos[iHisto]->SetMarkerColor(colorIndex);
      }


      // 3. add to legend
      if (histos[iHisto]->GetMarkerStyle() == 1) {
        legend->AddEntry(histos[iHisto], (histosandnames[iHisto].legendName).c_str() , "F");
      } else {
        legend->AddEntry(histos[iHisto], (histosandnames[iHisto].legendName).c_str() , "P");
      }


      // add to stack (useful if stacked):
      stack->Add(histos[iHisto]);

    }

    if (stacked) {
      yMax = stack->GetMaximum();
    }

    yMax *= yAxisMaxScale_;
    if (histos.size() >= 4) {
      yMax *= 1.15;
    }

    TH2D* h2_axes = new TH2D("axes", "", 10, xMin, xMax, 10, yMin, yMax);
    std::string xAxisName_full(xAxisName);
    if (units != "") {
      xAxisName_full += " [" + units + "]";
    }
    h2_axes->SetXTitle(xAxisName_full.c_str());
    if (normalized) {
      h2_axes->SetYTitle("Normalized to Unity");
    } else {
      char yAxisName_char[150];
      if (units != "") {
        if (((int)(10.*histos[0]->GetBinWidth(1)) % 10) == 0) {
          sprintf(yAxisName_char, "%s / (%.0f %s)", instanceName.c_str(), histos[0]->GetBinWidth(1), units.c_str());
        } else {
          sprintf(yAxisName_char, "%s / (%.1f %s)", instanceName.c_str(), histos[0]->GetBinWidth(1), units.c_str());
        }
      } else {
        sprintf(yAxisName_char, "%s", instanceName.c_str());
      }
      h2_axes->SetYTitle(yAxisName_char);
    }

    TPaveText* label_CMS = get_labelCMS();
    TPaveText* label_sqrt = get_labelSqrt();

    //  TPaveText* label_CMStop = get_labelCMStop();

    std::vector<TPaveText*> rmsText;
    float yMin_text = 0.25;
    float yMax_text = 0.55;
    //float yStep = (yMax_text-yMin_text)/(float)histos.size();
    float yStep = std::min((float)0.05, (yMax_text - yMin_text) / (float)histos.size());
    for (unsigned iText = 0; iText < histos.size(); ++iText) {
      TPaveText* rms = new TPaveText(0.6, yMax_text - (iText + 1)*yStep, 0.8, yMax_text - iText * yStep, "brNDC");
      rms->SetFillColor(0);
      rms->SetTextSize(0.035);
      rms->SetTextColor(histos[iText]->GetFillColor());
      char rms_text[150];
      sprintf(rms_text, "RMS = %.2f #pm %.2f", histos[iText]->GetRMS(), histos[iText]->GetRMSError());
      rms->AddText(rms_text);
      rmsText.push_back(rms);
    }



    TCanvas* c1 = new TCanvas("c1", "", 600, 600);
    c1->cd();

    h2_axes->Draw();
    if (stacked) {
      stack->Draw("histo same");
    } else {
      std::vector<int> toBeDrawnLater;
      for (unsigned i = 0; i < histos.size(); ++i) {
        int thisHisto = histos.size() - i - 1;
        // skip ones for which markerstyle is defined (draw on top later):
        if (histos[thisHisto]->GetMarkerStyle() != 1) { //default
          toBeDrawnLater.push_back(thisHisto);
          continue;
        }

        // reverse order is prettier:
        if (normalized) {
          histos[thisHisto]->DrawNormalized("histo same");
        } else {
          histos[thisHisto]->Draw("histo same");
        }
      } //for histos

      for (unsigned i = 0; i < toBeDrawnLater.size(); ++i) {
        if (normalized) {
          histos[toBeDrawnLater[i]]->DrawNormalized("p same");
        } else {
          histos[toBeDrawnLater[i]]->Draw("p same");
        }
      } //for later histos

    } // if stacked
    legend->Draw("same");
    label_CMS->Draw("same");
    label_sqrt->Draw("same");
    gPad->RedrawAxis();

    if (outputGraphs_) {
      std::string canvasName = this->get_outputdir() + "/" + infile.datasetName + "_" + saveVarName;
      if (stacked) {
        canvasName += "_stack";
      }
      std::string canvasName_eps = canvasName + ".eps";
      std::string canvasName_png = canvasName + ".png";
      c1->SaveAs(canvasName_eps.c_str());
      c1->SaveAs(canvasName_png.c_str());
    }

    delete c1;
    c1 = 0;
    delete h2_axes;
    h2_axes = 0;
    delete legend;
    legend = 0;

  } //compareDifferentHistos




  void drawBase::drawObjects(const std::vector< TObject* > objects, const std::string& name,
      const std::string& xAxisName, float xMin, float xMax,
      const std::string& yAxisName, float yMin, float yMax,
      bool logx, bool logy) {


    TH2D* axes = new TH2D("axes", "", 10, xMin, xMax, 10, yMin, yMax);
    axes->SetXTitle(xAxisName.c_str());
    axes->SetYTitle(yAxisName.c_str());
    if (logx) {
      axes->GetXaxis()->SetMoreLogLabels();
      axes->GetXaxis()->SetNoExponent();
    }
    axes->GetXaxis()->SetTitleOffset(1.1);
    axes->GetYaxis()->SetTitleOffset(1.5);


    TLegend* legend = new TLegend(0.45, 0.15, 0.88, 0.4, "|#eta| < 1.3");
    legend->SetFillColor(kWhite);
    legend->SetTextSize(legendTextSize_);
    for (unsigned i = 0; i < objects.size(); ++i) {
      legend->AddEntry(objects[i], objects[i]->GetName(), "P");
    }

    TPaveText* label_cms = get_labelCMS();
    TPaveText* label_sqrt = get_labelSqrt();

    //Float_t labelTextSize = 0.035;
    std::string jetAlgoName = get_algoName();
    TPaveText* label_algo = get_labelAlgo(3);
    //TPaveText* label_algo = new TPaveText(0.27, 0.15, 0.32, 0.2, "brNDC");
    //label_algo->SetFillColor(kWhite);
    //label_algo->SetTextSize(labelTextSize);
    //label_algo->AddText(jetAlgoName.c_str());

    TCanvas* c1 = new TCanvas("c1", "c1", 600, 600);
    c1->SetLeftMargin(0.12);
    c1->SetBottomMargin(0.12);
    c1->cd();
    if (logx) {
      c1->SetLogx();
    }
    if (logy) {
      c1->SetLogy();
    }
    axes->Draw();
    for (unsigned i = 0; i < objects.size(); ++i) {
      if (i == 0) {
        objects[i]->Draw("pl same");
      } else {
        objects[i]->Draw("p same");
      }
    }
    legend->Draw("same");
    label_cms->Draw("same");
    label_sqrt->Draw("same");
    label_algo->Draw("same");

    if (outputGraphs_) {
      std::string name_eps = outputdir_ + "/" + name + ".eps";
      std::string name_png = outputdir_ + "/" + name + ".png";
      c1->SaveAs(name_eps.c_str());
      c1->SaveAs(name_png.c_str());
    }

    delete c1;
    c1 = 0;
    delete axes;
    axes = 0;
    delete legend;
    legend = 0;

  }




  void drawBase::add_dataFile(TFile* dataFile, const std::string& datasetName, const std::string& legendName, int markerColor, int markerStyle, int fillStyle) {

    InputFile thisFile;
    thisFile.file = dataFile;
    thisFile.datasetName = datasetName;
    thisFile.legendName = legendName;
    thisFile.weight = 1.;
    thisFile.fillColor = markerColor;
    thisFile.markerStyle = markerStyle;
    thisFile.fillStyle = fillStyle;

    if (dataFile == 0) {
      std::cout << "File: '" << dataFile->GetName() << " does not exist! Skipping." << std::endl;
      return;
    }
    dataFiles_.push_back(thisFile);
    std::cout << "-> Added DATA file '" << dataFile->GetName() << "'." << std::endl;

  }



  void drawBase::add_mcFile(TFile* mcFile, const std::string& datasetName, const std::string& legendName, int fillColor, int fillStyle, int markerStyle, int lineColor, int lineWidth) {

    this->add_mcFile(mcFile, 1., datasetName, legendName, fillColor, fillStyle, markerStyle, lineColor, lineWidth);

  }

  void drawBase::add_mcFile(TFile* mcFile, float weight, const std::string& datasetName, const std::string& legendName, int fillColor, int fillStyle, int markerStyle, int lineColor, int lineWidth) {

    InputFile thisfile;
    thisfile.file = mcFile;
    thisfile.datasetName = datasetName;
    thisfile.weight = weight;
    thisfile.legendName = legendName;
    thisfile.fillColor = fillColor;
    thisfile.fillStyle = fillStyle;
    thisfile.markerStyle = markerStyle;
    thisfile.lineColor = lineColor;
    thisfile.lineWidth = lineWidth;
    if (mcFile == 0) {
      std::cout << "File: '" << mcFile->GetName() << " does not exist! Skipping." << std::endl;
      return;
    }
    mcFiles_.push_back(thisfile);

    std::cout << "-> Added MC file '" << mcFile->GetName()  << "'." << std::endl;
  }



  void drawBase::add_mcFile_superimp(TFile* mcFile, const std::string& datasetName, const std::string& legendName, float multFactor, int lineColor, int lineWidth) {

    InputFile thisfile;
    thisfile.file = mcFile;
    thisfile.datasetName = datasetName;
    thisfile.weight = multFactor;
    thisfile.legendName = legendName;
    thisfile.fillColor = 0;
    thisfile.fillStyle = 0;
    thisfile.markerStyle = 0;
    thisfile.lineColor = lineColor;
    thisfile.lineWidth = lineWidth;
    if (mcFile == 0) {
      std::cout << "File: '" << mcFile->GetName() << " does not exist! Skipping." << std::endl;
      return;
    }
    mcFiles_superimp_.push_back(thisfile);

    std::cout << "-> Added (superimposed) MC file '" << mcFile->GetName()  << "'." << std::endl;
  }




  std::string drawBase::get_fullSuffix() const {

    std::string fullSuffix = get_outputSuffix();

    if (scaleFactor_ == 0.) {
      std::cout << "Scale factor has to be set before getting full suffix." << std::endl;
      std::cout << "You probably did:" << std::endl;
      std::cout << "   drawBase::set_outputdir()" << std::endl;
      std::cout << "   drawBase::set_normalization()" << std::endl;
      std::cout << "instead of: " << std::endl;
      std::cout << "   drawBase::set_normalization()" << std::endl;
      std::cout << "   drawBase::set_outputdir()" << std::endl;
      std::cout << "Please fix this!" << std::endl;
    } else {
      fullSuffix += "_";
      if (scaleFactor_ == -1.) {
        fullSuffix += "SHAPE";
      } else {
        fullSuffix += "LUMI";
      }
    }

    if (flags_ != "") {
      fullSuffix += ("_" + flags_);
    }

    return fullSuffix;

  }




  void drawBase::set_outputdir(const std::string& outputdir) {

    if (outputdir != "") {

      outputdir_ = outputdir;

    } else { //default outputdir

      outputdir_ = analysisType_ + "Plots_" + this->get_fullSuffix();

    }

    mkdir(outputdir_.c_str(), 0777);

  }



  void drawBase::set_mcMarkers(bool set) {

    int markerStyle = 20 + dataFiles_.size();
    for (unsigned i = 0; i < mcFiles_.size(); ++i) {
      if (set) {
        mcFiles_[i].markerStyle = markerStyle++;
      } else {
        mcFiles_[i].markerStyle = -1;
      }
    }

    if (mcFiles_.size() == 0) {
      std::cout << "-> MC Files not set! Set them before calling set_mcMarkers()!" << std::endl;
    }

  }



  std::string drawBase::get_etaRangeText(const std::string& etaRegion) const {

    char etaRange_ch[100];
    sprintf(etaRange_ch, "|#eta| < %.1f", etamax_);
    std::string etaRange(etaRange_ch);
    if (etaRegion == "barrel") {
      etaRange = "|#eta| < 1.4";
    }
    if (etaRegion == "endcap") {
      etaRange = "1.4 < |#eta| < 2.4";
    }
    if (etaRegion == "eta02") {
      etaRange = "|#eta| < 2";
    }
    if (etaRegion == "eta23") {
      etaRange = "2 < |#eta| < 3";
    }
    if (etaRegion == "eta163") {
      etaRange = "1.6 < |#eta| < 3";
    }
    if (etaRegion == "eta1425") {
      etaRange = "1.4 < |#eta| < 2.5";
    }
    if (etaRegion == "eta1430") {
      etaRange = "1.4 < |#eta| < 3.0";
    }
    if (etaRegion == "Rch050") {
      etaRange = "|#eta| < 2.5, R_{ch} < 50%";
    }
    if (etaRegion == "Rch5070") {
      etaRange = "|#eta| < 2.5, 50 < R_{ch} < 70%";
    }
    if (etaRegion == "Rch70100") {
      etaRange = "|#eta| < 2.5, R_{ch} > 70%";
    }
    if (etaRegion == "ptPhot_10_15") {
      etaRange = "10 < p_{T}^{#gamma} < 15 GeV/c";
    }
    if (etaRegion == "ptPhot_15_3500") {
      etaRange = "p_{T}^{#gamma} > 15 GeV/c";
    }

    return etaRange;

  }


  LegendBox drawBase::get_legendBox(int legendQuadrant, const std::vector<std::string>* legendNames) const {


    if (legendQuadrant < 0 || legendQuadrant > 5) {
      legendQuadrant = 1;
      std::cout << "Invalid legend quadrant '" << legendQuadrant << "'. Using 1." << std::endl;
    }

    int nNames_total = 0;
    if (legendTitle_ != "") {
      nNames_total += 1;
    }
    if (legendNames != 0) {
      nNames_total += legendNames->size();
    } else {
      nNames_total += dataFiles_.size();
      nNames_total += mcFiles_.size();
    }

    LegendBox lb;

    if (legendQuadrant == 1) {
      lb.xMin = 0.63;
      lb.yMax = 0.91;
      lb.yMin = lb.yMax - 0.07 * (float)nNames_total;
      lb.xMax = 0.92;
    } else if (legendQuadrant == 0) {
      lb.xMin = 0.5;
      lb.yMax = 0.88;
      lb.yMin = lb.yMax - 0.07 * (float)nNames_total;
      lb.xMax = 0.73;
    } else if (legendQuadrant == 2) {
      lb.xMin = 0.2;
      lb.yMax = 0.91;
      lb.yMin = lb.yMax - 0.07 * (float)nNames_total;
      lb.xMax = 0.49;
    } else if (legendQuadrant == 3) {
      lb.xMin = 0.18;
      lb.yMin = 0.15;
      lb.xMax = 0.41;
      lb.yMax = lb.yMin + 0.07 * (float)nNames_total;
    } else if (legendQuadrant == 4) {
      lb.xMin = 0.5;
      lb.yMin = 0.15;
      lb.xMax = 0.73;
      lb.yMax = lb.yMin + 0.07 * (float)nNames_total;
    } else if (legendQuadrant == 5) {
      lb.xMin = 0.4;
      lb.yMin = 0.15;
      lb.xMax = 0.6;
      lb.yMax = lb.yMin + 0.07 * (float)nNames_total;
    }

    bool widen = false;
    if (legendTitle_.size() > 13) {
      widen = true;
    }
    if (legendNames != 0) {
      for (unsigned i = 0; i < legendNames->size(); ++i)
        if (legendNames->at(i).length() > 12) {
          widen = true;
        }
    } else {
      for (unsigned i = 0; i < dataFiles_.size(); ++i)
        if (dataFiles_[i].legendName.length() > 13) {
          widen = true;
        }
      for (unsigned i = 0; i < mcFiles_.size(); ++i)
        if (mcFiles_[i].legendName.length() > 13) {
          widen = true;
        }
    }

    if (widen) {
      if (legendQuadrant == 1 || legendQuadrant == 4) {
        lb.xMin *= 0.9;
      }
      if (legendQuadrant == 2 || legendQuadrant == 3) {
        lb.xMax *= 1.1;
      }
    }

    return lb;

  }



  std::string drawBase::get_lumiText() const {

    float lumi4Text(lumi_);
    lumi4Text *= 1000000.; // in mub-1
    bool onlyOneDecimal = false;
    std::string units = "#mub ^{-1}";
    if (lumi4Text > 10.) {
      lumi4Text /= 1000.;
      units = "nb ^{-1}";
    }
    if (lumi4Text > 100.) {
      lumi4Text /= 1000.;
      units = "pb ^{-1}";
    }
    if (lumi4Text >= 1000.) {
      lumi4Text /= 1000.;
      units = "fb ^{-1}";
    }

    if (lumi4Text > 10.) {
      onlyOneDecimal = true;
    }

    if (dataFiles_.size() == 0) {
      if (((int)(lumi4Text * 10.) % 10) == 0) {
        onlyOneDecimal = true;
      }
    }

    char lumiText[200];
    if (onlyOneDecimal) {
      sprintf(lumiText, "%.0f %s", lumi4Text, units.c_str());
    } else {
      sprintf(lumiText, "%.1f %s", lumi4Text, units.c_str());
    }

    std::string lumiText_str(lumiText);

    return lumiText;

  }


  std::string drawBase::get_sqrtText() const {

    std::string lumiText = this->get_lumiText();

    char label_sqrt_text[150];
    if (isCMSArticle_) {
      sprintf(label_sqrt_text, "#sqrt{s} = 8 TeV");
    } else {
      if (lumi_ == 0.) {
        sprintf(label_sqrt_text, "#sqrt{s} = 8 TeV");
      } else {
        sprintf(label_sqrt_text, "#sqrt{s} = 8 TeV, L = %s", lumiText.c_str());
      }
    }

    std::string returnString(label_sqrt_text);

    return returnString;

  }


  std::string drawBase::get_algoType() const {

    if (recoType_ == "calo") {
      return jetAlgo_;
    } else if (recoType_ == "jpt" && jetAlgo_ == "akt5") {
      return "jptak5";
    } else if (recoType_ == "jpt" && jetAlgo_ == "akt7") {
      return "jptak7";
    } else {
      return ((std::string)recoType_ + jetAlgo_);
    }

  }



  std::string drawBase::get_algoName() const {

    std::string algoName;

    if (jetAlgo_ == "AK5") {
      algoName = "Anti-k_{T} 0.5 ";
    } else if (jetAlgo_ == "AK7") {
      algoName = "Anti-k_{T} 0.7 ";
    } else if (jetAlgo_ == "KT4") {
      algoName = "k_{T} 0.4 ";
    } else if (jetAlgo_ == "KT6") {
      algoName = "k_{T} 0.6 ";
    } else {
      std::cout << "Jet algo '" << jetAlgo_ << "' currently not supported. Exiting." << std::endl;
      exit(918);
    }

    if (recoType_ == "PFlow") {
      algoName += "PFJets";
    } else if (recoType_ == "Calo") {
      algoName += "CaloJets";
    } else if (recoType_ == "JPT") {
      algoName += "JPTJets";
    } else {
      std::cout << "Reco Type '" << recoType_ << "' currently not supported. Exiting." << std::endl;
      exit(919);
    }

    return algoName;

  }



  //void cmsPrel(bool wide = false) {
  TPaveText* drawBase::get_labelCMStop(bool wide) const {

    //TLatex *latex = new TLatex();
    //latex->SetNDC();
    TPaveText* label_cmstop = new TPaveText(0.15, 0.94, 0.96, 0.98, "brNDC");
    label_cmstop->SetTextSize(0.045);
    label_cmstop->SetFillColor(0);

    label_cmstop->SetTextAlign(31); // align right
    //latex->DrawLatex(wide ? 0.98 : 0.95, 0.96, "#sqrt{s} = 7 TeV");
    label_cmstop->AddText("#sqrt{s} = 8 TeV");
    std::string leftText;
    if (dataFiles_.size() == 0) {
      leftText = "CMS Simulation 2012";
    } else {
      if (isCMSArticle_) {
        leftText = "CMS 2012";
      } else {
        leftText = "CMS Preliminary 2012";
      }
    }

    if (lumi_ > 0.) {
      label_cmstop->SetTextAlign(11); // align left
      std::string lumiText = this->get_lumiText();
      if (dataFiles_.size() == 0) {
        lumiText = "L = " + lumiText;
      }
      //label_cmstop->DrawLatex(wide ? 0.06 : 0.15, 0.96, Form("%s, %s", leftText.c_str(), lumiText.c_str()));
      label_cmstop->AddText(Form("%s, %s", leftText.c_str(), lumiText.c_str()));
    } else {
      label_cmstop->SetTextAlign(11); // align left
      //label_cmstop->DrawLatex(wide ? 0.06 : 0.15, 0.96, Form("%s", leftText.c_str()));
      label_cmstop->AddText(Form("%s", leftText.c_str()));
    }

    return label_cmstop;

  } // cmsPrel



  TPaveText* drawBase::get_labelCMS(int legendQuadrant) const {

    if (legendQuadrant != 0 && legendQuadrant != 1 && legendQuadrant != 2 && legendQuadrant != 3) {
      std::cout << "WARNING! Legend quadrant '" << legendQuadrant << "' not yet implemented for CMS label. Using 2." << std::endl;
      legendQuadrant = 2;
    }

    float x1, y1, x2, y2;
    if (legendQuadrant == 1) {
      x1 = 0.63;
      y1 = 0.86;
      x2 = 0.8;
      y2 = 0.92;
    } else if (legendQuadrant == 2) {
      x1 = (isCMSArticle_) ? 0.22 : 0.25;
      y1 = 0.86;
      x2 = (isCMSArticle_) ? 0.39 : 0.42;
      y2 = 0.92;
    } else if (legendQuadrant == 3) {
      x1 = 0.25;
      y1 = 0.2;
      x2 = 0.42;
      y2 = 0.24;
    } else if (legendQuadrant == 0) {
      x1 = 0.17;
      y1 = 0.963;
      x2 = 0.65;
      y2 = 0.985;
    }


    TPaveText* cmslabel = new TPaveText(x1, y1, x2, y2, "brNDC");
    cmslabel->SetFillColor(kWhite);
    cmslabel->SetTextSize(0.038);
    if (legendQuadrant == 0) {
      //cmslabel->SetTextAlign(11);
    }
    cmslabel->SetTextFont(62);
    std::string label_CMS_text = this->get_CMSText();
    if (legendQuadrant != 0) {
      cmslabel->AddText(label_CMS_text.c_str());
    } else {
      std::string leftText;
      if (dataFiles_.size() == 0) {
        leftText = "CMS Simulation 2012";
      } else {
        if (isCMSArticle_) {
          leftText = "CMS 2012";
        } else {
          leftText = "CMS Preliminary 2012";
        }
      }
      if (lumi_ > 0.) {
        //cmslabel->SetTextAlign(11); // align left
        std::string lumiText = this->get_lumiText();
        cmslabel->AddText(Form("%s, %s", leftText.c_str(), lumiText.c_str()));
      } else {
        //cmslabel->SetTextAlign(11); // align left
        cmslabel->AddText(Form("%s", leftText.c_str()));
      }
    }

    return cmslabel;

  }




  TPaveText* drawBase::get_labelSqrt(int legendQuadrant) const {

    if (legendQuadrant != 0 && legendQuadrant != 1 && legendQuadrant != 2 && legendQuadrant != 3) {
      std::cout << "WARNING! Legend quadrant '" << legendQuadrant << "' not yet implemented for Sqrt label. Using 2." << std::endl;
      legendQuadrant = 2;
    }


    float x1, y1, x2, y2;
    if (legendQuadrant == 1) {
      x1 = 0.63;
      y1 = 0.82;
      x2 = 0.8;
      y2 = 0.86;
    } else if (legendQuadrant == 2) {
      x1 = (isCMSArticle_) ? 0.22 : 0.25;
      y1 = 0.82;
      x2 = (isCMSArticle_) ? 0.39 : 0.42;
      y2 = 0.86;
    } else if (legendQuadrant == 3) {
      x1 = 0.25;
      y1 = 0.16;
      x2 = 0.42;
      y2 = 0.2;
    } else if (legendQuadrant == 0) {
      x1 = 0.7;
      y1 = 0.953;
      x2 = 0.96;
      y2 = 0.975;
    }


    TPaveText* label_sqrt = new TPaveText(x1, y1, x2, y2, "brNDC");
    label_sqrt->SetFillColor(kWhite);
    label_sqrt->SetTextSize(0.038);
    label_sqrt->SetTextFont(42);
    std::string label_sqrt_text = this->get_sqrtText();
    if (legendQuadrant != 0) {
      label_sqrt->AddText(label_sqrt_text.c_str());
    } else {
      label_sqrt->SetTextAlign(31); // align right
      label_sqrt->AddText("#sqrt{s} = 8 TeV");
    }

    return label_sqrt;

  }


  TPaveText* drawBase::get_labelAlgo(int legendQuadrant) const {


    float x1, y1, x2, y2;
    if (legendQuadrant == 1) {
      x1 = 0.77;
      y1 = 0.88;
      x2 = 0.82;
      y2 = 0.92;
    } else if (legendQuadrant == 2) {
      x1 = 0.30;
      y1 = 0.86;
      x2 = 0.35;
      y2 = 0.92;
    } else if (legendQuadrant == 3) {
      x1 = 0.3;
      //y1 = 0.15;
      y1 = 0.18;
      x2 = 0.35;
      //y2 = 0.2;
      y2 = 0.21;
    } else if (legendQuadrant == 4) {
      x1 = 0.75;
      y1 = 0.18;
      x2 = 0.8;
      y2 = 0.21;
    } else {
      std::cout << "WARNING! Legend quadrant '" << legendQuadrant << "' not yet implemented for Algo label. Using 3." << std::endl;
      x1 = 0.27;
      y1 = 0.15;
      x2 = 0.32;
      y2 = 0.2;
    }

    Float_t labelTextSize = 0.035;
    std::string jetAlgoName = (recoType_ != "" && jetAlgo_ != "") ? get_algoName() : "";
    TPaveText* label_algo = new TPaveText(x1, y1, x2, y2, "brNDC");
    label_algo->SetFillColor(kWhite);
    label_algo->SetTextSize(labelTextSize);
    label_algo->AddText(jetAlgoName.c_str());
    //label_algo->SetTextAlign(11);

    return label_algo;

  }


  std::string drawBase::get_CMSText() const {

    std::string returnString;

    if (dataFiles_.size() == 0) {
      returnString = "CMS Simulation 2012";
    } else {
      if (isCMSArticle_) {
        std::string lumiText = this->get_lumiText();
        returnString = "CMS 2012, " + lumiText;
      } else {
        returnString = "CMS Preliminary 2012";
      }
    }

    return returnString;

  }


  std::string drawBase::get_outputSuffix() const {

    std::string suffix = "";

    if (dataFiles_.size() != 0) {

      for (unsigned iData = 0; iData < dataFiles_.size(); ++iData) {
        if (iData > 0) {
          suffix += "_";
        }
        suffix += dataFiles_[iData].datasetName;
      }

      if (mcFiles_.size() != 0) {
        suffix += "_vs_";
      }

    }

    if (mcFiles_.size() > 0) {
      suffix += mcFiles_[0].datasetName;
    }

    for (unsigned int i = 1; i < mcFiles_.size(); ++i) {
      suffix += "_plus_";
      suffix += mcFiles_[i].datasetName;
    }

    std::string algoName = jetAlgo_;
    if (recoType_ != "calo") {
      algoName = recoType_ + algoName;
    }
    if (recoType_ == "jpt" && jetAlgo_ == "akt5") {
      algoName = "jptak5";
    }
    if (recoType_ == "jpt" && jetAlgo_ == "akt7") {
      algoName = "jptak7";
    }
    if (algoName != "") {
      suffix += ("_" + algoName);
    }

    return suffix;

  }


  TGraphErrors* drawBase::get_graphRatio(TGraphErrors* gr_data, TGraphErrors* gr_MC) {

    TGraphErrors* gr_ratio = new TGraphErrors(0);

    for (int i = 0; i < gr_MC->GetN(); ++i) {

      Double_t datax, datay;
      gr_data->GetPoint(i, datax, datay);
      //Double_t dataxerr = gr_data->GetErrorX(i);
      Double_t datayerr = gr_data->GetErrorY(i);

      Double_t mcx, mcy;
      gr_MC->GetPoint(i, mcx, mcy);
      Double_t mcxerr = gr_MC->GetErrorX(i);
      Double_t mcyerr = gr_MC->GetErrorY(i);

      Double_t ratiox = mcx;
      Double_t ratioxerr = mcxerr;

      Double_t ratioy = datay / mcy;
      Double_t ratioyerr = sqrt(datayerr * datayerr / (mcy * mcy) + datay * datay * mcyerr * mcyerr / (mcy * mcy * mcy * mcy));


      gr_ratio->SetPoint(i, ratiox, ratioy);
      gr_ratio->SetPointError(i, ratioxerr, ratioyerr);

    } //for points

    return gr_ratio;

  }


  void drawBase::add_label(const std::string& text, float xmin, float ymin, float xmax, float ymax) {

    additionalLabel_ = new TPaveText(xmin, ymin, xmax, ymax, "brNDC");
    additionalLabel_->SetTextSize(0.035);
    additionalLabel_->SetFillColor(0);
    additionalLabel_->AddText(text.c_str());

  }

  void drawBase::delete_label() {

    delete additionalLabel_;
    additionalLabel_ = 0;

  }

  void drawBase::drawHistRatio(TPad* pad, TH1* data, TH1* mc, const std::string& xTitle, double fitMin/* = 0*/, double fitMax/* = 8000*/) {

    pad->cd();

    pad->SetGridy();

    TH1* data_clone = static_cast<TH1*>(data->Clone("data_cloned"));
    data_clone->Divide(mc);

    // Fit the ratio
    TF1* ratioFit = new TF1("ratioFit", "pol1", fitMin, fitMax);
    ratioFit->SetParameter(0, 1.);
    ratioFit->SetParameter(0, 2.);

    ratioFit->SetParLimits(0, -5, 5);
    ratioFit->SetParLimits(1, -5, 5);
    ratioFit->SetLineColor(46);
    ratioFit->SetLineWidth(1.5);
    data_clone->Fit(ratioFit, "QR");
    
    //TH1D* errors = new TH1D("errors", "errors", 100, fitMin, fitMax);
    //(TVirtualFitter::GetFitter())->GetConfidenceIntervals(errors);
    //errors->SetStats(false);
    //errors->SetLineColor(46);
    //errors->SetFillColor(LIGHT_RED);

    double fitValue = ratioFit->GetParameter(0);
    double fitError = ratioFit->GetParError(0);

    data_clone->SetMaximum(2);
    data_clone->SetMinimum(0);

    data_clone->SetXTitle(xTitle.c_str());
    data_clone->SetYTitle("Data / MC");

    data_clone->GetXaxis()->SetTitleOffset(1.10);
    data_clone->GetYaxis()->SetTitleOffset(0.55);
    data_clone->GetXaxis()->SetTickLength(0.06);
    data_clone->GetXaxis()->SetLabelSize(0.085);
    data_clone->GetYaxis()->SetLabelSize(0.07);
    data_clone->GetXaxis()->SetTitleSize(0.09);
    data_clone->GetYaxis()->SetTitleSize(0.08);
    data_clone->GetYaxis()->SetNdivisions(505, true);

    data_clone->Draw("e");
    //errors->Draw("e3 same");
    ratioFit->Draw("same");

    TPaveText* fitlabel = new TPaveText(0.45, 0.35, 0.78, 0.51, "brNDC");
    fitlabel->SetTextFont(42);
    fitlabel->SetTextSize(0.08);
    fitlabel->SetFillColor(0);
    TString fitLabelText = TString::Format("Fit: b = %.4f #pm %.4f", fitValue, fitError);
    fitlabel->AddText(fitLabelText);
    fitLabelText = TString::Format("Fit: a = %.4f #pm %.4f", ratioFit->GetParameter(1) , ratioFit->GetParError(1));
    fitlabel->AddText(fitLabelText);

    fitlabel->Draw("same");

    gPad->Update();
    gPad->RedrawAxis();
  }

  void drawBase::drawHisto_vs_vertex(std::vector<std::pair<int, int> > vertexBins, const std::string& name, const std::string& axisName, const std::string& units, const std::string& instanceName, bool log_aussi, int legendQuadrant, const std::string& labelText) {
    
  bool isMPF = TString(name).Contains("mpf", TString::kIgnoreCase);
  // Ignore bin between 3500 - 7000
  //int number_of_plots = ptBins.size() - 1;
  int number_of_plots = vertexBins.size();

  std::string ptPhotMean_name = "ptPhotMean";
  std::string ptJetGenMean_name = "ptJetGenMean";


  TGraphErrors* gr_response_vs_pt = new TGraphErrors(0);
  gr_response_vs_pt->SetName("response_vs_pt");

  TGraphErrors* gr_responseMC_vs_pt = new TGraphErrors(0);
  gr_responseMC_vs_pt->SetName("responseMC_vs_pt");

  //TGraphErrors* gr_responseGEN_vs_pt = new TGraphErrors(0);
  //gr_responseGEN_vs_pt->SetName("responseGEN_vs_pt");

  TGraphErrors* gr_resolution_vs_pt = new TGraphErrors(0);
  gr_resolution_vs_pt->SetName("resolution_vs_pt");

  TGraphErrors* gr_resolutionMC_vs_pt = new TGraphErrors(0);
  gr_resolutionMC_vs_pt->SetName("resolutionMC_vs_pt");

  //TGraphErrors* gr_resolutionGEN_vs_pt = new TGraphErrors(0);
  //gr_resolutionGEN_vs_pt->SetName("resolutionGEN_vs_pt");

  //TGraphErrors* gr_purity_vs_pt = new TGraphErrors(0);
  //gr_purity_vs_pt->SetName("purity_vs_pt");

  std::string histoName = name;

  for (int iplot = 0; iplot < number_of_plots; ++iplot) {

    //if( flags!="" ) histoName = histoName + "_" + flags;
    //
    std::pair<float, float> currentBin = vertexBins[iplot];
    float vertexMean = (currentBin.first + currentBin.second) / 2.;

    TString vertexRange = TString::Format("nvertex_%d_%d", (int) currentBin.first, (int) currentBin.second);

    // Set shape normalization for comparing shapes, even if we are in
    // prescaled region
    double oldScaleFactor = scaleFactor_;
    scaleFactor_ = -1;

    // pt phot cut label
    TString labelPtPhot = TString::Format("%d < NPV < %d", (int) currentBin.first, (int) currentBin.second);
    drawHisto(std::string(name + "_" + vertexRange), axisName, units, instanceName, log_aussi, legendQuadrant, labelPtPhot.Data(), true);

    scaleFactor_ = oldScaleFactor;

    // save vs pt info:


    bool hasData = (lastHistos_data_.size() > 0);
    bool hasMC = (lastHistos_mc_.size() > 0);

    //Float_t dataResponse = (noDATA) ? 0. : dataHistos[0]->GetMean();
    //Float_t dataResponseErr = (noDATA) ? 0. : dataHistos[0]->GetMeanError();
    //Float_t dataRMS = (noDATA) ? 0. : dataHistos[0]->GetRMS();
    //Float_t dataRMSErr = (noDATA) ? 0. : dataHistos[0]->GetMeanError();

    Float_t meanTruncFraction = 0.99;
    Float_t rmsTruncFraction = 0.99;

    Float_t dataResponse = 0.;
    Float_t dataResponseErr = 0.;
    Float_t dataRMS = 0.;
    Float_t dataRMSErr = 0.;

    if (hasData) {
      fitTools::getTruncatedMeanAndRMS(lastHistos_data_[0], dataResponse, dataResponseErr, dataRMS, dataRMSErr, meanTruncFraction, rmsTruncFraction);
    }

    Float_t dataResolution = (hasData) ? dataRMS / dataResponse : 0.;
    Float_t dataResolutionErr = (hasData) ? sqrt(dataRMSErr * dataRMSErr / (dataResponse * dataResponse) + dataResolution * dataResolution * dataResponseErr * dataResponseErr / (dataResponse * dataResponse * dataResponse * dataResponse)) : 0.;


    if (hasData) {

      //if (h1_thisPhotPt_data->GetEntries() > 4) {
        gr_response_vs_pt->SetPoint(iplot, vertexMean, dataResponse);
        gr_response_vs_pt->SetPointError(iplot, 0., dataResponseErr);
      //}

      if (dataResolution > 0.0005) {
        gr_resolution_vs_pt->SetPoint(iplot, vertexMean, dataResolution);
        gr_resolution_vs_pt->SetPointError(iplot, 0., dataResolutionErr);
      }

    }

    Float_t mcResponse = 0.;
    Float_t mcResponseErr = 0.;
    Float_t mcRMS = 0.;
    Float_t mcRMSErr = 0.;

    if (hasMC) {
      fitTools::getTruncatedMeanAndRMS(lastHistos_mcHistoSum_, mcResponse, mcResponseErr, mcRMS, mcRMSErr, meanTruncFraction, rmsTruncFraction);
    }

    Float_t mcResolution = (!hasMC) ? 0. : mcRMS / mcResponse;
    Float_t mcResolutionErr = (!hasMC) ? 0. : sqrt(mcRMSErr * mcRMSErr / (mcResponse * mcResponse) + mcResolution * mcResolution * mcResponseErr * mcResponseErr / (mcResponse * mcResponse * mcResponse * mcResponse));

    if (hasMC) {

      //gr_responseMC_vs_pt->SetPoint(iplot, ptMeanMC, mcResponse);
      //gr_responseMC_vs_pt->SetPointError(iplot, ptMeanErrMC, mcResponseErr);

      //gr_resolutionMC_vs_pt->SetPoint(iplot, ptMeanMC, mcResolution);
      //gr_resolutionMC_vs_pt->SetPointError(iplot, ptMeanErrMC, mcResolutionErr);

      gr_responseMC_vs_pt->SetPoint(iplot, vertexMean, mcResponse);
      gr_responseMC_vs_pt->SetPointError(iplot, 0., mcResponseErr);

      gr_resolutionMC_vs_pt->SetPoint(iplot, vertexMean, mcResolution);
      gr_resolutionMC_vs_pt->SetPointError(iplot, 0., mcResolutionErr);

      //float purityNum = lastHistos_mc_[0]->Integral(0, lastHistos_mc_[0]->GetNbinsX() + 1);
      //float purityNumErr = lastHistos_mc_[0]->Integral(0, lastHistos_mc_[0]->GetNbinsX() + 1) / ((float)lastHistos_mc_[0]->GetEntries());
      //float purityDenom = 0.;
      //float purityDenomErr = 0.;
      //for (unsigned iHisto = 0; iHisto < lastHistos_mc_.size(); ++iHisto) {
        //purityDenom +=  lastHistos_mc_[iHisto]->Integral(0, lastHistos_mc_[iHisto]->GetNbinsX() + 1);
        //purityDenomErr += lastHistos_mc_[iHisto]->Integral(0, lastHistos_mc_[iHisto]->GetNbinsX() + 1) * lastHistos_mc_[iHisto]->Integral(0, lastHistos_mc_[iHisto]->GetNbinsX() + 1) / ((float)lastHistos_mc_[iHisto]->GetEntries() * lastHistos_mc_[iHisto]->GetEntries());
      //}
      //purityDenomErr = sqrt(purityDenomErr);
      //float purity = purityNum / purityDenom;
      //float purityErr = sqrt(purityNumErr * purityNumErr / (purityDenom * purityDenom) + purityNum * purityNum * purityDenomErr * purityDenomErr / (purityDenom * purityDenom * purityDenom * purityDenom));
      ////gr_purity_vs_pt->SetPoint(iplot, ptMeanMC, purity);
      ////gr_purity_vs_pt->SetPointError(iplot, ptMeanErrMC, purityErr);
      //gr_purity_vs_pt->SetPoint(iplot, ptMean, purity);
      //gr_purity_vs_pt->SetPointError(iplot, 0., purityErr);


      //// Get gen informations. To do that, we need to transform
      //// resp_balancing_eta* in resp_balancing_gen_eta*
      //std::string responseGenName = std::string(name + "_" + ptRange);
      //boost::replace_all(responseGenName, "eta", "gen_eta");
      //if (isMPF) {
        //// For MPF, we don't have raw_gen
        //boost::replace_all(responseGenName, "_raw", "");
      //}

      //TH1* responseGEN = static_cast<TH1*>(mcGet(0, responseGenName));
      //for (unsigned i = 1; i < mcFiles_.size(); ++i) {
        //TH1* responseGEN2 = static_cast<TH1*>(mcGet(i, responseGenName));
        //responseGEN->Add(responseGEN2);
      //}

      //responseGEN->Scale(scaleFactor_);
      //responseGEN->SetLineWidth(3);

      //Float_t genResponse = 0.;
      //Float_t genResponseErr = 0.;
      //Float_t genRMS = 0.;
      //Float_t genRMSErr = 0.;

      //fitTools::getTruncatedMeanAndRMS(responseGEN, genResponse, genResponseErr, genRMS, genRMSErr, meanTruncFraction, rmsTruncFraction);

      //Float_t genResolution = genRMS / genResponse;
      //Float_t genResolutionErr = sqrt(genRMSErr * genRMSErr / (genResponse * genResponse) + genResolution * genResolution * genResponseErr * genResponseErr / (genResponse * genResponse * genResponse * genResponse));

      ////TH1D* h1_thisPtJetGen = (TH1D*)h2_ptJetGen_mc->ProjectionY("thisPtJetGen", iplot + 1, iplot + 1);

      ////Float_t ptMeanGEN = h1_thisPtJetGen->GetMean();
      ////Float_t ptMeanErrGEN = h1_thisPtJetGen->GetMeanError();

      ////gr_responseGEN_vs_pt->SetPoint(iplot, ptMeanGEN, genResponse);
      ////gr_responseGEN_vs_pt->SetPointError(iplot, ptMeanErrGEN, genResponseErr);

      ////gr_resolutionGEN_vs_pt->SetPoint(iplot, ptMeanGEN, genResolution);
      ////gr_resolutionGEN_vs_pt->SetPointError(iplot, ptMeanErrGEN, genResolutionErr);

      //gr_responseGEN_vs_pt->SetPoint(iplot, ptMean, genResponse);
      //gr_responseGEN_vs_pt->SetPointError(iplot, 0., genResponseErr);

      //gr_resolutionGEN_vs_pt->SetPoint(iplot, ptMean, genResolution);
      //gr_resolutionGEN_vs_pt->SetPointError(iplot, 0., genResolutionErr);

    }

  } // for pt bins


  std::string graphFileName = "PhotonJetGraphsVertices_" + get_fullSuffix() + ".root";
  TFile* graphFile = TFile::Open(graphFileName.c_str(), "update");
  graphFile->cd();

  TString graphName = TString::Format("%s_data_vs_npv", name.c_str()); // something like resp_balancing_eta011_data_vs_pt
  gr_response_vs_pt->SetName(graphName);
  gr_response_vs_pt->Write();

  graphName = TString::Format("%s_mc_vs_npv", name.c_str());
  gr_responseMC_vs_pt->SetName(graphName);
  gr_responseMC_vs_pt->Write();

  //graphName = TString::Format("%s_gen_vs_npv", name.c_str());
  //gr_responseGEN_vs_pt->SetName(graphName);
  //gr_responseGEN_vs_pt->Write();

  //graphName = TString::Format("%s_purity_vs_npv", name.c_str());
  //gr_purity_vs_pt->SetName(graphName);
  //gr_purity_vs_pt->Write();

  std::string resolutionName = name;
  boost::replace_all(resolutionName, "resp", "resolution");
  
  graphName = TString::Format("%s_data_vs_npv", resolutionName.c_str()); // something like resolution_balancing_eta011_data_vs_pt
  gr_resolution_vs_pt->SetName(graphName);
  gr_resolution_vs_pt->Write();

  graphName = TString::Format("%s_mc_vs_npv", resolutionName.c_str()); 
  gr_resolutionMC_vs_pt->SetName(graphName);
  gr_resolutionMC_vs_pt->Write();

  //graphName = TString::Format("%s_gen_vs_npv", resolutionName.c_str());
  //gr_resolutionGEN_vs_pt->SetName(graphName);
  //gr_resolutionGEN_vs_pt->Write();

  graphFile->Close();

  
//gStyle->SetPadTickX(1);
//gStyle->SetPadTickY(1);

  bool noDATA = (gr_response_vs_pt->GetN() == 0);
  bool noMC = (gr_responseMC_vs_pt->GetN() == 0);

  int canvasHeight = (noMC) ? 600 : 800;
  TCanvas* c1 = new TCanvas("c1", "c1", 600, canvasHeight);
  c1->cd();

  // Data / MC comparison
  TPad* pad_hi = new TPad("pad_hi", "", 0., 0.33, 0.99, 0.99);
  pad_hi->Draw();
  //pad_hi->SetLogx();
  pad_hi->SetLeftMargin(0.12);
  pad_hi->SetBottomMargin(0.015);

  // Data / MC ratio
  TPad* pad_lo = new TPad("pad_lo", "", 0., 0., 0.99, 0.33);
  pad_lo->Draw();
  //pad_lo->SetLogx();
  pad_lo->SetLeftMargin(0.12);
  pad_lo->SetTopMargin(1.);
  pad_lo->SetBottomMargin(0.3);

  float npvMax = vertexBins[vertexBins.size() - 1].second;
  float npvMin = vertexBins[0].first;

  TGraphErrors* gr_resp_ratio = 0;
  Float_t scale_uncert = (recoType_ == "Calo") ? 0.1 : 0.1;

  TH2* h2_axes_lo_resp = NULL;

  TLine* line_one = new TLine(npvMin, 1., npvMax, 1.);
  TLine* line_plus_resp = new TLine(npvMin, 1.05, npvMax, 1.05);
  TLine* line_minus_resp = new TLine(npvMin, 0.95, npvMax, 0.95);

  if (!noDATA && !noMC) {  //ugly will have to fix (cloning the TCanvas?)

    pad_lo->cd();
    
    h2_axes_lo_resp = new TH2D("axes_lo_resp", "", 10, npvMin, npvMax, 10, 0.86, 1.14);

    h2_axes_lo_resp->SetXTitle("Number of primary vertices");
    h2_axes_lo_resp->SetYTitle("Data / MC");
    h2_axes_lo_resp->GetXaxis()->SetTitleOffset(1.2);
    h2_axes_lo_resp->GetYaxis()->SetTitleOffset(0.55);
    h2_axes_lo_resp->GetXaxis()->SetTickLength(0.06);
    h2_axes_lo_resp->GetXaxis()->SetMoreLogLabels();
    h2_axes_lo_resp->GetXaxis()->SetNoExponent();
    //h2_axes_lo_resp->GetXaxis()->SetLabelSize(0.);
    h2_axes_lo_resp->GetXaxis()->SetLabelSize(0.085);
    h2_axes_lo_resp->GetYaxis()->SetLabelSize(0.07);
    h2_axes_lo_resp->GetXaxis()->SetTitleSize(0.09);
    h2_axes_lo_resp->GetYaxis()->SetTitleSize(0.08);
    h2_axes_lo_resp->GetYaxis()->SetNdivisions(7, kTRUE);
    h2_axes_lo_resp->Draw("");

    line_one->Draw("same");

    //line_plus_resp->SetLineColor(46);
    line_plus_resp->SetLineWidth(2);
    line_plus_resp->SetLineStyle(2);

    //line_minus_resp->SetLineColor(46);
    line_minus_resp->SetLineWidth(2);
    line_minus_resp->SetLineStyle(2);

    gr_resp_ratio = this->get_graphRatio(gr_response_vs_pt, gr_responseMC_vs_pt);
    gr_resp_ratio->SetName("response_ratio");
    gr_resp_ratio->SetMarkerStyle(20);
    gr_resp_ratio->SetMarkerSize(1.5);
    gr_resp_ratio->SetMarkerColor(kBlue - 6);

    TF1* ratioFit = new TF1("ratioFit", "[0] + [1]*x", npvMin, npvMax);
    ratioFit->SetParameter(0, 1.);
    ratioFit->SetParameter(1, 0.);

    ratioFit->SetLineColor(46);
    ratioFit->SetLineWidth(2);
    gr_resp_ratio->Fit(ratioFit, "RQ");
    //std::cout << "-> ChiSquare: " << constline->GetChisquare() << "   NDF: " << constline->GetNDF() << std::endl;

    double fitValue = ratioFit->GetParameter(0);
    double fitError = ratioFit->GetParError(0);

    //TBox* errors = new TBox(npvMin, fitValue - fitError, npvMax, fitValue + fitError);
    //errors->SetFillColor(kBlue - 10);
    //errors->SetFillStyle(1001);

    TPaveText* fitlabel = new TPaveText(0.45, 0.77, 0.90, 0.83, "brNDC");
    fitlabel->SetTextSize(0.08);
    fitlabel->SetFillColor(0);
    TString fitLabelText = TString::Format("Fit: %.3f #pm %.3f + (%.3f #pm %.3f)x", fitValue, fitError, ratioFit->GetParameter(1), ratioFit->GetParError(1));
    fitlabel->AddText(fitLabelText);
    fitlabel->Draw("same");

    TH1D* errors = new TH1D("errors", "errors", 100, npvMin, npvMax);
    (TVirtualFitter::GetFitter())->GetConfidenceIntervals(errors);
    errors->SetStats(false);
    errors->SetLineColor(46);
    errors->SetFillColor(LIGHT_RED);
    errors->Draw("e3 same");

    line_plus_resp->Draw("same");
    line_minus_resp->Draw("same");

    //ratioFit->Draw("same");

    gr_resp_ratio->Draw("P same");

    gPad->RedrawAxis();

    pad_hi->cd();

  } // if !nodata && !nomc

  TH2D* h2_axes = new TH2D("axes_again", "", 10, npvMin, npvMax, 10, 0.7, 1.20);
  //h2_axes->SetXTitle("Photon p_{T} [GeV/c]");
  h2_axes->SetYTitle("Jet p_{T} response");
  //h2_axes->SetYTitle("< p_{T}^{jet} / p_{T}^{#gamma} >");
  h2_axes->GetXaxis()->SetTitleOffset(1.1);
  h2_axes->GetYaxis()->SetTitleOffset(1.2);
  h2_axes->GetYaxis()->SetTitleSize(0.045);
  //h2_axes->GetXaxis()->SetMoreLogLabels();
  //h2_axes->GetXaxis()->SetNoExponent();
  if (! noMC) {
    h2_axes->GetXaxis()->SetLabelSize(0.);
  }

  h2_axes->Draw();

  Float_t labelTextSize = 0.035;
  TPaveText* label_algo = get_labelAlgo(2);

  TLegend* legend = new TLegend(0.55, 0.15, 0.92, 0.38, legendTitle_.c_str());
  legend->SetFillColor(kWhite);
  legend->SetFillStyle(0);
  legend->SetTextSize(legendTextSize_);
  if (! isMPF) {
    if (!noDATA) {
      legend->AddEntry(gr_response_vs_pt, "Data (#gamma+Jet)", "P");
    }
    if (!noMC) {
      legend->AddEntry(gr_responseMC_vs_pt, "Simulation (#gamma+Jet)", "P");
    }
  } else {
    if (!noDATA) {
      legend->AddEntry(gr_response_vs_pt, "Data (MPF)", "P");
    }
    if (!noMC) {
      legend->AddEntry(gr_responseMC_vs_pt, "Simulation (MPF)", "P");
    }
  }
  if (!noMC) {
    //legend->AddEntry(gr_responseGEN_vs_pt, "True Response", "P");
  }
  legend->Draw("same");

  Float_t cmsTextSize = 0.043;
  TPaveText* label_cms = get_labelCMS(1);
  label_cms->SetTextSize(cmsTextSize);

  //Float_t sqrtTextSize = 0.041;
  TPaveText* label_sqrt = get_labelSqrt(1);

  label_cms->Draw("same");
  label_sqrt->Draw("same");
  label_algo->Draw("same");


  if (!noMC) {
    /*
    gr_responseGEN_vs_pt->SetMarkerStyle(29);
    gr_responseGEN_vs_pt->SetMarkerColor(46);
    gr_responseGEN_vs_pt->SetMarkerSize(2.);
    gr_responseGEN_vs_pt->Draw("Psame");
    */

    gr_responseMC_vs_pt->SetMarkerStyle(24);
    gr_responseMC_vs_pt->SetMarkerSize(1.5);
    gr_responseMC_vs_pt->SetMarkerColor(kBlue - 6);
    gr_responseMC_vs_pt->SetLineColor(kBlue - 6);
    gr_responseMC_vs_pt->Draw("Psame");
  }

  if (!noDATA) {
    if (noMC) {
      gr_response_vs_pt->SetMarkerColor(TColor::GetColor(0, 0, 153));
      gr_response_vs_pt->SetLineColor(TColor::GetColor(0, 0, 153));
      gr_response_vs_pt->SetLineWidth(1.);
      gr_response_vs_pt->SetMarkerStyle(21);
      gr_response_vs_pt->SetMarkerSize(1);
    } else {
      gr_response_vs_pt->SetMarkerStyle(20);
      gr_response_vs_pt->SetMarkerSize(1.5);
      gr_response_vs_pt->SetMarkerColor(kBlue - 6);
    }

    gr_response_vs_pt->Draw("Psame");
  }

  gPad->RedrawAxis();

  std::string canvName = outputdir_ + "/" + name + "_vs_npv";

  if (noMC) {
    std::string canvName_eps = canvName + ".eps";
    c1->SaveAs(canvName_eps.c_str());
    //std::string canvName_png = canvName + ".png";
    //c1->SaveAs(canvName_png.c_str());
  }

  std::string canvName_fit_eps = canvName + "_FITLINE.eps";
  c1->SaveAs(canvName_fit_eps.c_str());

  // ----------------------------------------------------
  //             and now resolutions:
  // ----------------------------------------------------

  TH2* h2_axes_lo_reso = new TH2D("axes_lo_reso", "", 10, npvMin, npvMax, 10, (1. - 6.*scale_uncert), (1. + 6.*scale_uncert));

  if (!noDATA && !noMC) {

    pad_lo->cd();

    h2_axes_lo_reso->SetXTitle("Number of primary vertices");
    h2_axes_lo_reso->SetYTitle("Data / MC");
    h2_axes_lo_reso->GetXaxis()->SetTitleOffset(1.2);
    h2_axes_lo_reso->GetYaxis()->SetTitleOffset(0.55);
    h2_axes_lo_reso->GetXaxis()->SetTickLength(0.06);
    h2_axes_lo_reso->GetXaxis()->SetMoreLogLabels();
    h2_axes_lo_reso->GetXaxis()->SetNoExponent();
    h2_axes_lo_reso->GetXaxis()->SetLabelSize(0.085);
    h2_axes_lo_reso->GetYaxis()->SetLabelSize(0.07);
    h2_axes_lo_reso->GetXaxis()->SetTitleSize(0.09);
    h2_axes_lo_reso->GetYaxis()->SetTitleSize(0.08);
    h2_axes_lo_reso->GetYaxis()->SetNdivisions(5, kTRUE);
    h2_axes_lo_reso->Draw("");

    line_one->Draw("same");

    TLine* line_plus_reso = new TLine(npvMin, 1. + 2.*scale_uncert, npvMax, 1. + 2.*scale_uncert);
    //line_plus_reso->SetLineColor(46);
    //line_plus_reso->SetLineWidth(2);
    line_plus_reso->SetLineStyle(2);
    line_plus_reso->Draw("same");

    TLine* line_minus_reso = new TLine(npvMin, 1. - 2.*scale_uncert, npvMax, 1. - 2.*scale_uncert);
    //line_minus_reso->SetLineColor(46);
    //line_minus_reso->SetLineWidth(2);
    line_minus_reso->SetLineStyle(2);
    line_minus_reso->Draw("same");

    TGraphErrors* gr_reso_ratio = this->get_graphRatio(gr_resolution_vs_pt, gr_resolutionMC_vs_pt);
    gr_reso_ratio->SetName("reso_ratio");
    gr_reso_ratio->SetMarkerStyle(20);
    gr_reso_ratio->SetMarkerSize(1.8);
    gr_reso_ratio->Draw("P");

    TF1* constline = new TF1("constline", "[0] + [1]*x", npvMin, npvMax);
    constline->SetParameter(0, 1.);
    constline->SetParameter(1, 0.);
    //constline->SetLineColor(8);
    //constline->SetLineColor(38);
    constline->SetLineColor(46);
    //constline->SetLineStyle(3);
    constline->SetLineWidth(3);
    gr_reso_ratio->Fit(constline, "RQ");
    //std::cout << "-> ChiSquare: " << constline->GetChisquare() << "   NDF: " << constline->GetNDF() << std::endl;

    TH1D* errors = new TH1D("errors", "errors", 100, npvMin, npvMax);
    (TVirtualFitter::GetFitter())->GetConfidenceIntervals(errors);
    errors->SetStats(false);
    errors->SetFillColor(LIGHT_RED);
    errors->SetLineColor(46);

    TPaveText* fitlabel = new TPaveText(0.45, 0.80, 0.90, 0.85, "brNDC");
    fitlabel->SetTextSize(0.08);
    fitlabel->SetFillColor(0);
    char fitLabelText[150];
    sprintf(fitLabelText, "Fit: %.3f #pm %.3f + (%.3f #pm %.3f)x", constline->GetParameter(0), constline->GetParError(0), constline->GetParameter(1), constline->GetParError(1));
    fitlabel->AddText(fitLabelText);
    fitlabel->Draw("same");
    //line_plus_resp->Draw("same");
    //constline->Draw("same");

    errors->Draw("e3 same");

    gr_reso_ratio->Draw("P same");
    gPad->RedrawAxis();

    pad_hi->cd();

  } // if !nodata and !nomc

  TH2D* h2_axes2 = new TH2D("axes_again2", "", 10, npvMin, npvMax, 10, 0., 1.);
  //h2_axes2->SetXTitle("Photon p_{T} [GeV/c]");
  h2_axes2->SetYTitle("Resolution");
  //h2_axes2->GetXaxis()->SetTitleOffset(1.1);
  h2_axes2->GetYaxis()->SetTitleOffset(1.5);
  //h2_axes2->GetXaxis()->SetMoreLogLabels();
  //h2_axes2->GetXaxis()->SetNoExponent();

  if (! noMC) {
    h2_axes2->GetXaxis()->SetLabelSize(0.);
  }

  h2_axes2->Draw();

  TPaveText* label_cms2 = get_labelCMS(1);
  label_cms2->SetTextSize(cmsTextSize);
  //TPaveText* label_cms2 = new TPaveText(0.58, 0.83, 0.75, 0.87, "brNDC");
  //label_cms2->SetFillColor(kWhite);
  //label_cms2->SetTextSize(cmsTextSize);
  //label_cms2->SetTextFont(62);
  //label_cms2->AddText(label_CMS_text.c_str());

  TPaveText* label_sqrt2 = get_labelSqrt(1);
  //TPaveText* label_sqrt2 = new TPaveText(0.58, 0.78, 0.75, 0.82, "brNDC");
  //label_sqrt2->SetFillColor(kWhite);
  //label_sqrt2->SetTextSize(sqrtTextSize);
  //label_sqrt2->SetTextFont(42);
  //label_sqrt2->AddText(label_sqrt_text.c_str());

  TPaveText* label_algo2 = get_labelAlgo(2);
  //TPaveText* label_algo2 = new TPaveText(0.27, 0.82, 0.32, 0.86, "brNDC");
  //label_algo2->SetFillColor(kWhite);
  //label_algo2->SetTextSize(labelTextSize);
  //label_algo2->AddText(jetAlgoName.c_str());

  TLegend* legend2 = new TLegend(0.5, 0.5, 0.85, 0.73, legendTitle_.c_str());
  legend2->SetFillColor(kWhite);
  legend2->SetFillStyle(0);
  legend2->SetTextSize(labelTextSize);
  if (! isMPF) {
    if (!noDATA) {
      legend2->AddEntry(gr_resolution_vs_pt, "Data (#gamma+Jet)", "P");
    }
    if (!noMC) {
      legend2->AddEntry(gr_resolutionMC_vs_pt, "Simulation (#gamma+Jet)", "P");
    }
  } else {
    if (!noDATA) {
      legend2->AddEntry(gr_resolution_vs_pt, "Data (MPF)", "P");
    }
    if (!noMC) {
      legend2->AddEntry(gr_resolutionMC_vs_pt, "Simulation (MPF)", "P");
    }

  }
  //if (! noMC) {
    //legend2->AddEntry(gr_resolutionGEN_vs_pt, "True Resolution", "P");
  //}

  legend2->Draw("same");

  if (!noMC) {
    //gr_resolutionGEN_vs_pt->SetMarkerStyle(29);
    //gr_resolutionGEN_vs_pt->SetMarkerColor(46);
    //gr_resolutionGEN_vs_pt->SetMarkerSize(2.);
    //gr_resolutionGEN_vs_pt->Draw("Psame");

    gr_resolutionMC_vs_pt->SetMarkerStyle(24);
    gr_resolutionMC_vs_pt->SetMarkerSize(1.8);
    gr_resolutionMC_vs_pt->SetMarkerColor(kBlack);
    gr_resolutionMC_vs_pt->SetLineColor(kBlack);
    gr_resolutionMC_vs_pt->Draw("Psame");
  }

  if (!noDATA) {
    if (noMC) {
      gr_resolution_vs_pt->SetMarkerColor(TColor::GetColor(0, 0, 153));
      gr_resolution_vs_pt->SetLineColor(TColor::GetColor(0, 0, 153));
      gr_resolution_vs_pt->SetLineWidth(1.);
      gr_resolution_vs_pt->SetMarkerStyle(21);
      gr_resolution_vs_pt->SetMarkerSize(1.);
    } else {
      gr_resolution_vs_pt->SetMarkerStyle(20);
      gr_resolution_vs_pt->SetMarkerSize(1.8);
      gr_resolution_vs_pt->SetMarkerColor(kBlack);
    }

    gr_resolution_vs_pt->Draw("Psame");
  }

  label_cms2->Draw("same");
  label_sqrt2->Draw("same");
  label_algo2->Draw("same");

  gPad->RedrawAxis();

  canvName = outputdir_ + "/" + resolutionName + "_vs_npv";

  if (outputGraphs_) {
    std::string canvName_eps = canvName + ".eps";
    c1->SaveAs(canvName_eps.c_str());
    //std::string canvName_png = canvName + ".png";
    //c1->SaveAs(canvName_png.c_str());
  }

  delete h2_axes;
  h2_axes = 0;
  delete h2_axes2;
  h2_axes2 = 0;
  delete h2_axes_lo_resp;
  h2_axes_lo_resp = 0;
  delete h2_axes_lo_reso;
  h2_axes_lo_reso = 0;
  delete c1;
  c1 = 0;

  //gStyle->SetPadTickX(0);
  //gStyle->SetPadTickY(0);*/
  }
