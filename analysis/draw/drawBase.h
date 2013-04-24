//************************************************************************
//
//      DrawBase is the Base Class from which all DrawX.C inherit
//
//      Draws Data-MC comparisons
//
//************************************************************************

#pragma once

#include "TCanvas.h"
#include "TPad.h"
#include "TH1F.h"
#include "THStack.h"
#include "TProfile.h"
#include "TH2D.h"
#include "TF1.h"
#include "TGraphErrors.h"
#include "TFile.h"
#include "TTree.h"
#include "TStyle.h"
#include "TLegend.h"
#include "TPaveText.h"
#include "TDirectory.h"
//#include "TLatex.h"


struct InputFile {
  TFile* file;
  std::string datasetName;
  float weight;
  std::string legendName;
  int fillColor;
  int fillStyle;
  int markerStyle;
  int markerSize;
  int lineColor;
  int lineWidth;
};


class HistoAndName {
public:
  HistoAndName(int markerstyle = -1) {
    markerStyle = markerstyle;
  };
  std::string histoName;
  std::string legendName;
  int markerStyle;
};


struct LegendBox {

  float xMin;
  float xMax;
  float yMin;
  float yMax;

};


class drawBase {

public:

  drawBase(const std::string& analysisType, const std::string& recoType, const std::string& jetAlgo, bool outputGraphs, const std::string& flags = "");
  virtual ~drawBase();

  void set_shapeNormalization();
  void set_lumiNormalization(float givenLumi = -1.);
  void set_sameEventNormalization();
  void set_sameInstanceNormalization();

  //void drawHisto( const std::string& name, const std::string& etaRegion, const std::string& flags, const std::string& axisName="", const std::string& units="", int legendQuadrant=1, bool log_aussi=false);
  void drawHisto_vs_pt(int nBinsPt, float* ptBins, const std::string& name, const std::string& axisName, const std::string& units = "", const std::string& instanceName = "Entries", bool log_aussi = false, int legendQuadrant = 1, std::string flags = "", const std::string& labelText = "");
  void drawHisto_vs_vertex(std::vector<std::pair<int, int> > vertexBins, const std::string& name, const std::string& axisName, const std::string& units = "", const std::string& instanceName = "Entries", bool log_aussi = false, int legendQuadrant = 1, const std::string& labelText = "");
  void drawHisto_vs_pt(std::vector<std::pair<float, float> > ptBins, const std::string& name, const std::string& axisName, const std::string& units = "", const std::string& instanceName = "Entries", bool log_aussi = false, int legendQuadrant = 1, const std::string& labelText = "");
  void drawHisto(const std::string& name, const std::string& axisName, const std::string& units = "", const std::string& instanceName = "Entries", bool log_aussi = false, int legendQuadrant = 1, const std::string& labelText = "", bool add_jetAlgoText = false, bool drawRatio = true, double fitMin = 0, double fitMax = 8000);
  void drawHisto_fromHistos(std::vector<TH1*> dataHistos, std::vector<TH1*> mcHistos, std::vector<TH1*> mcHistos_superimp, const std::string& name, const std::string& axisName, const std::string& units = "", const std::string& instanceName = "Entries", bool log_aussi = false, int legendQuadrant = 1, const std::string& flags = "", const std::string& labelText = "", bool add_jetAlgoText = false, bool drawRatio = true, double fitMin = 0, double fitMax = 8000);
  void drawHisto_fromTree(const std::string& treeName, const std::string& varName, const std::string& selection, int nBins, float xMin, float xMax, const std::string& name, const std::string& axisName, const std::string& units = "", const std::string& instanceName = "Entries", bool log_aussi = false, int legendQuadrant = 1, const std::string& flags = "", const std::string& labelText = "", bool add_jetAlgoText = false);
  void drawProfile(const std::string& yVar, const std::string& xVar, int legendQuadrant = 1);
  void drawStack(const std::string& varY, const std::string& varX, const std::string& RECO_GEN, bool isData) const {
    this->drawStack(varY, varX, "", RECO_GEN, isData);
  };
  void drawStack(const std::string& varY, const std::string& varX, const std::string& etaRegion, const std::string& RECO_GEN, bool isData) const;
  void compareDifferentHistos(const std::vector< HistoAndName > histosandnames, const std::string saveVarName, const std::string xAxisName, const std::string& units = "", const std::string& instanceName = "Entries", bool stacked = false, int legendQuadrant = 1);
  void compareDifferentHistos_singleFile(InputFile file, const std::vector< HistoAndName > histosandnames, const std::string saveVarName, const std::string xAxisName, const std::string& units = "", const std::string& instanceName = "Entries", bool stacked = false, int legendQuadrant = 1);
  void drawObjects(const std::vector< TObject* > objects, const std::string& name,
                   const std::string& xAxisName, float xMin, float xMax,
                   const std::string& yAxisName, float yMin, float yMax,
                   bool logx = false, bool logy = false);

  void set_analysisType(const std::string analysisType) {
    analysisType_ = analysisType;
  };
  void add_dataFile(TFile* dataFile, const std::string& datasetName, const std::string& legendName = "Data", int markerColor = -1, int markerStyle = -1, int fillStyle = -1);
  void add_mcFile(TFile* mcFile, const std::string& datasetName, const std::string& legendName, int fillColor = -1, int fillStyle = -1, int markerStyle = -1, int lineColor = -1, int lineWidth = -1);
  void add_mcFile_superimp(TFile* mcFile, const std::string& datasetName, const std::string& legendName, float multFactor = 1., int lineColor = -1, int lineWidth = -1);
  // in the following function weight must be cross_section(in pb) / Nevents:
  void add_mcFile(TFile* mcFile, float weight, const std::string& datasetName, const std::string& legendName, int fillColor = -1, int fillStyle = -1, int markerStyle = -1, int lineColor = -1, int lineWidth = -1);
  void set_lumi(float lumi) {
    lumi_ = lumi;
  };
  void set_outputdir(const std::string& outputdir = "");   //if "" is passed, default outputdir is set
  void set_flags(const std::string& flags) {
    flags_ = flags;
  };
  void set_pt_thresh(Int_t pt_thresh) {
    pt_thresh_ = pt_thresh;
  };
  void set_etamax(Float_t etamax) {
    etamax_ = etamax;
  };
  void set_raw_corr(const std::string& raw_corr) {
    raw_corr_ = raw_corr;
  };
  void set_pdf_aussi(bool pdf_aussi) {
    pdf_aussi_ = pdf_aussi;
  };
  void set_logx(bool logx = true) {
    logx_ = logx;
  };
  void set_scaleFactor(float scaleFactor) {
    scaleFactor_ = scaleFactor;
  };
  void set_xAxisMin(float xAxisMin = 9999.) {
    xAxisMin_ = xAxisMin;
  };
  void set_xAxisMax(float xAxisMax = 9999.) {
    xAxisMax_ = xAxisMax;
  };
  void set_yAxisMax(float yAxisMax = 9999.) {
    yAxisMax_ = yAxisMax;
  };
  void set_yAxisMaxScale(float yAxisMaxScale = 1.4) {
    yAxisMaxScale_ = yAxisMaxScale;
  };
  void set_yAxisMaxScaleLog(float yAxisMaxScale) {
    yAxisMaxScaleLog_ = yAxisMaxScale;
  };
  void set_noStack(bool set = true) {
    noStack_ = set;
  };
  void set_isCMSArticle(bool set = true);
  void set_rebin(int rebin) {
    rebin_ = rebin;
  };
  void set_mcMarkers(bool set = true);
  void set_markerSize(float markerSize) {
    markerSize_ = markerSize;
  };
  void set_getBinLabels(bool getBinL = true) {
    getBinLabels_ = getBinL;
  };
  void set_legendTitle(const std::string& title) {
    legendTitle_ = title;
  };
  void add_label(const std::string& text, float xmin, float ymin, float xmax, float ymax);
  void delete_label();

  LegendBox get_legendBox(int legendQuadrant = 1, const std::vector<std::string>* legendNames = 0) const;
  TPaveText* get_labelCMS(int legendQuadrant = 0) const;
  TPaveText* get_labelCMStop(bool wide = false) const;
  TPaveText* get_labelSqrt(int legendQuadrant = 0) const;
  TPaveText* get_labelAlgo(int legendQuadrant = 3) const;
  std::string get_CMSText() const;
  std::string get_analysisType() const {
    return analysisType_;
  };
  std::string get_recoType() const {
    return recoType_;
  };
  std::string get_flags() const {
    return flags_;
  };
  std::string get_legendTitle() const {
    return legendTitle_;
  };
  TFile* get_dataFile(int i) const {
    return dataFiles_[i].file;
  };
  TFile* get_mcFile(int i) const {
    return mcFiles_[i].file;
  };
  std::string get_outputdir() const {
    return outputdir_;
  };
  Int_t get_pt_thresh() const {
    return pt_thresh_;
  };
  Float_t get_etamax() const {
    return etamax_;
  };
  std::string get_raw_corr() const {
    return raw_corr_;
  };
  bool get_pdf_aussi() const {
    return pdf_aussi_;
  };

  void setFolder(const std::string& to) {
    folder_ = to;
  }

  TObject *dataGet(size_t index, const std::string& name) {
    dataFiles_[index].file->cd(folder_.c_str());
    return gDirectory->Get(name.c_str());
  }

  TObject *mcGet(size_t index, const std::string& name) {
    mcFiles_[index].file->cd(folder_.c_str());
    return gDirectory->Get(name.c_str());
  }

  std::string get_etaRangeText(const std::string& etaRegion) const;
  std::string get_sqrtText() const;
  std::string get_lumiText() const;
  std::string get_algoName() const;
  std::string get_algoType() const;
  std::string get_axisName(std::string name);
  std::string get_outputSuffix() const;
  std::string get_fullSuffix() const;

  TGraphErrors* get_graphRatio(TGraphErrors* gr_data, TGraphErrors* gr_MC);

  void drawHistRatio(TPad* pad, TH1* data, TH1* mc, const std::string& xTitle, double fitMin = 0, double fitMax = 8000);

  void setOutputGraphs(bool output) {
    outputGraphs_ = output;
  }

private:


  TStyle* style_;

  std::string analysisType_;
  std::string recoType_;
  std::string jetAlgo_;
  bool outputGraphs_;

  std::string folder_;
  std::string flags_;

  std::vector< InputFile > dataFiles_;
  std::vector< InputFile > mcFiles_;
  std::vector< InputFile > mcFiles_superimp_;

  TH1* lastHistos_mcHistoSum_;
  std::vector< TH1* > lastHistos_data_;
  std::vector< TH1* > lastHistos_mc_;
  std::vector< TH1* > lastHistos_mc_superimp_;

  Float_t scaleFactor_;

  Float_t xAxisMin_;
  Float_t xAxisMax_;
  Float_t yAxisMax_;
  Float_t yAxisMaxScale_;
  Float_t yAxisMaxScaleLog_;

  Float_t markerSize_;
  Float_t lumi_;

  Int_t rebin_;

  std::string outputdir_;
  Int_t pt_thresh_;
  Float_t etamax_;
  std::string raw_corr_;
  bool pdf_aussi_;
  bool logx_;
  bool getBinLabels_;
  std::string legendTitle_;
  float legendTextSize_;

  bool poissonAsymmErrors_;

  TPaveText* additionalLabel_;

  bool noStack_;

  bool isCMSArticle_;

};
