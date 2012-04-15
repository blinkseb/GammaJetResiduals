#include "drawExtrap.h"
#include "fitTools.h"

#include "TGraphErrors.h"
#include "TH1F.h"

#define OUTPUT_GRAPHS false

drawExtrap::drawExtrap(const std::string& analysisType, const std::string& recoType, const std::string& jetAlgo, bool outputGraphs, const std::string& flags) : drawBase(analysisType, recoType, jetAlgo, outputGraphs, flags) {

  FIT_RMS_ = "FIT";
  NOQ_ = false;
  INTPERC_ = 95.;
  FIXM_ = false;
  EXCLUDE_FIRST_POINT_ = false;

  mExtrapBinning.initialize(mPtBinning, (recoType == "pf") ? "PFlow" : "Calo");
}



void drawExtrap::drawResponseExtrap(const std::string& etaRegion, bool rawJets) {

  int recoPhot_color = kRed;
  int recoGen_color = kBlue;
  int genPhot_color = kGreen + 2;
  //int genPhot_color = kGreen+3;

  std::string etaRegion_str;
  if (etaRegion == "eta013") etaRegion_str = "|#eta| < 1.3";
  else if (etaRegion == "eta011") etaRegion_str = "|#eta| < 1.1";
  else if (etaRegion == "eta009") etaRegion_str = "|#eta| < 0.9";
  else if (etaRegion == "eta132") etaRegion_str = "1.3 < |#eta| < 2";
  else if (etaRegion == "eta1524") etaRegion_str = "1.5 < |#eta| < 2.4";
  else if (etaRegion == "eta23") etaRegion_str = "2 < |#eta| < 3";
  else if (etaRegion == "eta243") etaRegion_str = "2.4 < |#eta| < 3";
  else if (etaRegion == "eta35") etaRegion_str = "3 < |#eta| < 5";
  else etaRegion_str = "[unknown eta region]";

  std::vector<std::pair<float, float> > ptPhot_binning = mPtBinning.getBinning();

  //Float_t ptPhotRecoDATA[NBINS_PT-1];
  //Float_t ptPhotReco_errDATA[NBINS_PT-1];

  //Float_t ptPhotReco[NBINS_PT-1];
  //Float_t ptPhotReco_err[NBINS_PT-1];

  //Float_t extrapReso_RecoRelDATA[NBINS_PT-1];
  //Float_t extrapReso_err_RecoRelDATA[NBINS_PT-1];
  //Float_t extrapReso_RecoRel[NBINS_PT-1];
  //Float_t extrapReso_err_RecoRel[NBINS_PT-1];
  //Float_t trueReso_RecoRel[NBINS_PT-1];
  //Float_t trueReso_err_RecoRel[NBINS_PT-1];
  //Float_t intrReso_RecoRel[NBINS_PT-1];
  //Float_t intrReso_err_RecoRel[NBINS_PT-1];
  //Float_t extrapResp_RecoRelDATA[NBINS_PT-1];
  //Float_t extrapResp_err_RecoRelDATA[NBINS_PT-1];
  //Float_t extrapResp_RecoRel[NBINS_PT-1];
  //Float_t extrapResp_err_RecoRel[NBINS_PT-1];
  //Float_t trueResp_RecoRel[NBINS_PT-1];
  //Float_t trueResp_err_RecoRel[NBINS_PT-1];
  //Float_t intrResp_RecoRel[NBINS_PT-1];
  //Float_t intrResp_err_RecoRel[NBINS_PT-1];
  //Float_t imbalanceResp_RecoRel[NBINS_PT-1];
  //Float_t imbalanceResp_err_RecoRel[NBINS_PT-1];
  //Float_t pullResp_RecoRel[NBINS_PT-1];
  //Float_t pullResp_err_RecoRel[NBINS_PT-1];
  //Float_t pullReso_RecoRel[NBINS_PT-1];
  //Float_t pullReso_err_RecoRel[NBINS_PT-1];

  TGraphErrors* gr_DATAResp_vs_pt = new TGraphErrors(0);
  gr_DATAResp_vs_pt->SetName("gr_DATAResp_vs_pt");
  gr_DATAResp_vs_pt->SetMarkerStyle(25);
  gr_DATAResp_vs_pt->SetMarkerColor(kBlack);
  gr_DATAResp_vs_pt->SetMarkerSize(1.5);

  TGraphErrors* gr_DATARespMPF_vs_pt = NULL;
  TGraphErrors* gr_extrapRespMPF_vs_pt = NULL;

  /*TGraphErrors* gr_DATARespMPF_TypeICor_vs_pt = NULL;
  TGraphErrors* gr_extrapRespMPF_TypeICor_vs_pt = NULL;
  TGraphErrors* gr_DATARespMPF_TypeIpIICor_vs_pt = NULL;
  TGraphErrors* gr_extrapRespMPF_TypeIpIICor_vs_pt = NULL;*/

  //if (! corrected) {
    gr_DATARespMPF_vs_pt = new TGraphErrors(0);
    gr_DATARespMPF_vs_pt->SetName("gr_DATARespMPF_vs_pt");
    gr_DATARespMPF_vs_pt->SetMarkerStyle(25);
    gr_DATARespMPF_vs_pt->SetMarkerColor(kBlack);
    gr_DATARespMPF_vs_pt->SetMarkerSize(1.5);

    gr_extrapRespMPF_vs_pt = new TGraphErrors(0);
    gr_extrapRespMPF_vs_pt->SetName("gr_extrapRespMPF_vs_pt");
    gr_extrapRespMPF_vs_pt->SetMarkerStyle(25);
    gr_extrapRespMPF_vs_pt->SetMarkerColor(kBlack);
    gr_extrapRespMPF_vs_pt->SetMarkerSize(1.5);
  /*} else {
    gr_DATARespMPF_TypeICor_vs_pt = new TGraphErrors(0);
    gr_DATARespMPF_TypeICor_vs_pt->SetName("gr_DATARespMPF_TypeICor_vs_pt");
    gr_DATARespMPF_TypeICor_vs_pt->SetMarkerStyle(25);
    gr_DATARespMPF_TypeICor_vs_pt->SetMarkerColor(kBlack);
    gr_DATARespMPF_TypeICor_vs_pt->SetMarkerSize(1.5);

    gr_extrapRespMPF_TypeICor_vs_pt = new TGraphErrors(0);
    gr_extrapRespMPF_TypeICor_vs_pt->SetName("gr_extrapRespMPF_TypeICor_vs_pt");
    gr_extrapRespMPF_TypeICor_vs_pt->SetMarkerStyle(25);
    gr_extrapRespMPF_TypeICor_vs_pt->SetMarkerColor(kBlack);
    gr_extrapRespMPF_TypeICor_vs_pt->SetMarkerSize(1.5);

    gr_DATARespMPF_TypeIpIICor_vs_pt = new TGraphErrors(0);
    gr_DATARespMPF_TypeIpIICor_vs_pt->SetName("gr_DATARespMPF_TypeIpIICor_vs_pt");
    gr_DATARespMPF_TypeIpIICor_vs_pt->SetMarkerStyle(25);
    gr_DATARespMPF_TypeIpIICor_vs_pt->SetMarkerColor(kBlack);
    gr_DATARespMPF_TypeIpIICor_vs_pt->SetMarkerSize(1.5);

    gr_extrapRespMPF_TypeIpIICor_vs_pt = new TGraphErrors(0);
    gr_extrapRespMPF_TypeIpIICor_vs_pt->SetName("gr_extrapRespMPF_TypeIpIICor_vs_pt");
    gr_extrapRespMPF_TypeIpIICor_vs_pt->SetMarkerStyle(25);
    gr_extrapRespMPF_TypeIpIICor_vs_pt->SetMarkerColor(kBlack);
    gr_extrapRespMPF_TypeIpIICor_vs_pt->SetMarkerSize(1.5);
  }*/

  TGraphErrors* gr_extrapResp_vs_pt = new TGraphErrors(0);
  gr_extrapResp_vs_pt->SetName("gr_extrapResp_vs_pt");
  gr_extrapResp_vs_pt->SetMarkerStyle(25);
  gr_extrapResp_vs_pt->SetMarkerColor(kBlack);
  gr_extrapResp_vs_pt->SetMarkerSize(1.5);

  TGraphErrors* gr_intrResp_vs_pt = new TGraphErrors(0);
  gr_intrResp_vs_pt->SetName("gr_intrResp_vs_pt");
  gr_intrResp_vs_pt->SetMarkerStyle(29);
  gr_intrResp_vs_pt->SetMarkerColor(kBlue);
  gr_intrResp_vs_pt->SetMarkerSize(1.5);

  TGraphErrors* gr_qGenPartResp_vs_pt = new TGraphErrors(0);
  gr_qGenPartResp_vs_pt->SetName("gr_qGenPartResp_vs_pt");
  gr_qGenPartResp_vs_pt->SetMarkerStyle(23);
  gr_qGenPartResp_vs_pt->SetMarkerColor(kGray + 2);
  gr_qGenPartResp_vs_pt->SetMarkerSize(1.5);

  TGraphErrors* gr_qGenGammaResp_vs_pt = new TGraphErrors(0);
  gr_qGenGammaResp_vs_pt->SetName("gr_qGenGammaResp_vs_pt");
  gr_qGenGammaResp_vs_pt->SetMarkerStyle(23);
  gr_qGenGammaResp_vs_pt->SetMarkerColor(kGray + 2);
  gr_qGenGammaResp_vs_pt->SetMarkerSize(1.5);

  TGraphErrors* gr_qPartGammaResp_vs_pt = new TGraphErrors(0);
  gr_qPartGammaResp_vs_pt->SetName("gr_qPartGammaResp_vs_pt");
  //gr_qPartGammaResp_vs_pt->SetMarkerStyle(23);
  //gr_qPartGammaResp_vs_pt->SetMarkerColor(kGray+2);
  //gr_qPartGammaResp_vs_pt->SetMarkerSize(1.5);

  TGraphErrors* gr_qPhotGammaResp_vs_pt = new TGraphErrors(0);
  gr_qPhotGammaResp_vs_pt->SetName("gr_qPhotGammaResp_vs_pt");
  //gr_qPhotGammaResp_vs_pt->SetMarkerStyle(23);
  //gr_qPhotGammaResp_vs_pt->SetMarkerColor(kGray+2);
  //gr_qPhotGammaResp_vs_pt->SetMarkerSize(1.5);

  TGraphErrors* gr_qResp_vs_pt = new TGraphErrors(0);
  gr_qResp_vs_pt->SetName("gr_qResp_vs_pt");
  //gr_qResp_vs_pt->SetMarkerStyle(20);
  //gr_qResp_vs_pt->SetMarkerColor(kBlack);
  //gr_qResp_vs_pt->SetMarkerSize(1.5);
  //

  TGraphErrors* gr_mcRatioResp_vs_pt = new TGraphErrors(0);
  gr_mcRatioResp_vs_pt->SetName("gr_MCRatioResp_vs_pt");
  gr_mcRatioResp_vs_pt->SetMarkerStyle(25);
  gr_mcRatioResp_vs_pt->SetMarkerColor(kBlack);
  gr_mcRatioResp_vs_pt->SetMarkerSize(1.5);



  TGraphErrors* gr_DATAReso_vs_pt = new TGraphErrors(0);
  gr_DATAReso_vs_pt->SetName("gr_DATAReso_vs_pt");
  gr_DATAReso_vs_pt->SetMarkerStyle(25);
  gr_DATAReso_vs_pt->SetMarkerColor(kBlack);
  gr_DATAReso_vs_pt->SetMarkerSize(1.5);

  TGraphErrors* gr_extrapReso_vs_pt = new TGraphErrors(0);
  gr_extrapReso_vs_pt->SetName("gr_extrapReso_vs_pt");
  gr_extrapReso_vs_pt->SetMarkerStyle(25);
  gr_extrapReso_vs_pt->SetMarkerColor(kBlack);
  gr_extrapReso_vs_pt->SetMarkerSize(1.5);

  TGraphErrors* gr_reso_subtr_vs_pt = new TGraphErrors(0);
  gr_reso_subtr_vs_pt->SetName("gr_reso_subtr_vs_pt");
  gr_reso_subtr_vs_pt->SetMarkerStyle(25);
  gr_reso_subtr_vs_pt->SetMarkerColor(kBlack);
  gr_reso_subtr_vs_pt->SetMarkerSize(1.5);

  TGraphErrors* gr_DATAReso_subtr_vs_pt = new TGraphErrors(0);
  gr_DATAReso_subtr_vs_pt->SetName("gr_DATAReso_subtr_vs_pt");
  gr_DATAReso_subtr_vs_pt->SetMarkerStyle(25);
  gr_DATAReso_subtr_vs_pt->SetMarkerColor(kBlack);
  gr_DATAReso_subtr_vs_pt->SetMarkerSize(1.5);

  TGraphErrors* gr_reso_ratio_vs_pt = new TGraphErrors(0);
  gr_reso_ratio_vs_pt->SetName("gr_reso_ratio_vs_pt");
  gr_reso_ratio_vs_pt->SetMarkerStyle(25);
  gr_reso_ratio_vs_pt->SetMarkerColor(kBlack);
  gr_reso_ratio_vs_pt->SetMarkerSize(1.5);

  TGraphErrors* gr_intrReso_vs_pt = new TGraphErrors(0);
  gr_intrReso_vs_pt->SetName("gr_intrReso_vs_pt");
  gr_intrReso_vs_pt->SetMarkerStyle(29);
  gr_intrReso_vs_pt->SetMarkerColor(kBlue);
  gr_intrReso_vs_pt->SetMarkerSize(1.5);

  TGraphErrors* gr_qGenPartReso_vs_pt = new TGraphErrors(0);
  gr_qGenPartReso_vs_pt->SetName("gr_qGenPartReso_vs_pt");
  //gr_qGenPartReso_vs_pt->SetMarkerStyle(23);
  //gr_qGenPartReso_vs_pt->SetMarkerColor(kGray+2);
  //gr_qGenPartReso_vs_pt->SetMarkerSize(1.5);

  TGraphErrors* gr_qGenGammaReso_vs_pt = new TGraphErrors(0);
  gr_qGenGammaReso_vs_pt->SetName("gr_qGenGammaReso_vs_pt");
  //gr_qGenGammaReso_vs_pt->SetMarkerStyle(23);
  //gr_qGenGammaReso_vs_pt->SetMarkerColor(kGray+2);
  //gr_qGenGammaReso_vs_pt->SetMarkerSize(1.5);

  TGraphErrors* gr_qPartGammaReso_vs_pt = new TGraphErrors(0);
  gr_qPartGammaReso_vs_pt->SetName("gr_qPartGammaReso_vs_pt");
  //gr_qPartGammaReso_vs_pt->SetMarkerStyle(23);
  //gr_qPartGammaReso_vs_pt->SetMarkerColor(kGray+2);
  //gr_qPartGammaReso_vs_pt->SetMarkerSize(1.5);

  TGraphErrors* gr_qPhotGammaReso_vs_pt = new TGraphErrors(0);
  gr_qPhotGammaReso_vs_pt->SetName("gr_qPhotGammaReso_vs_pt");
  //gr_qPhotGammaReso_vs_pt->SetMarkerStyle(23);
  //gr_qPhotGammaReso_vs_pt->SetMarkerColor(kGray+2);
  //gr_qPhotGammaReso_vs_pt->SetMarkerSize(1.5);

  TGraphErrors* gr_qReso_vs_pt = new TGraphErrors(0);
  gr_qReso_vs_pt->SetName("gr_qReso_vs_pt");
  //gr_qReso_vs_pt->SetMarkerStyle(29);
  //gr_qReso_vs_pt->SetMarkerColor(kBlue);
  //gr_qReso_vs_pt->SetMarkerSize(1.5);

  TGraphErrors* gr_DATAResoMPF_vs_pt = NULL;
  TGraphErrors* gr_extrapResoMPF_vs_pt = NULL;

  /*TGraphErrors* gr_DATAResoMPF_TypeICor_vs_pt = NULL;
  TGraphErrors* gr_extrapResoMPF_TypeICor_vs_pt = NULL;
  TGraphErrors* gr_DATAResoMPF_TypeIpIICor_vs_pt = NULL;
  TGraphErrors* gr_extrapResoMPF_TypeIpIICor_vs_pt = NULL;*/

  //if (! corrected) {
    gr_DATAResoMPF_vs_pt = new TGraphErrors(0);
    gr_DATAResoMPF_vs_pt->SetName("gr_DATAResoMPF_vs_pt");
    gr_DATAResoMPF_vs_pt->SetMarkerStyle(25);
    gr_DATAResoMPF_vs_pt->SetMarkerColor(kBlack);
    gr_DATAResoMPF_vs_pt->SetMarkerSize(1.5);

    gr_extrapResoMPF_vs_pt = new TGraphErrors(0);
    gr_extrapResoMPF_vs_pt->SetName("gr_extrapResoMPF_vs_pt");
    gr_extrapResoMPF_vs_pt->SetMarkerStyle(25);
    gr_extrapResoMPF_vs_pt->SetMarkerColor(kBlack);
    gr_extrapResoMPF_vs_pt->SetMarkerSize(1.5);
  /*} else {
    gr_DATAResoMPF_TypeICor_vs_pt = new TGraphErrors(0);
    gr_DATAResoMPF_TypeICor_vs_pt->SetName("gr_DATAResoMPF_TypeICor_vs_pt");
    gr_DATAResoMPF_TypeICor_vs_pt->SetMarkerStyle(25);
    gr_DATAResoMPF_TypeICor_vs_pt->SetMarkerColor(kBlack);
    gr_DATAResoMPF_TypeICor_vs_pt->SetMarkerSize(1.5);

    gr_extrapResoMPF_TypeICor_vs_pt = new TGraphErrors(0);
    gr_extrapResoMPF_TypeICor_vs_pt->SetName("gr_extrapResoMPF_TypeICor_vs_pt");
    gr_extrapResoMPF_TypeICor_vs_pt->SetMarkerStyle(25);
    gr_extrapResoMPF_TypeICor_vs_pt->SetMarkerColor(kBlack);
    gr_extrapResoMPF_TypeICor_vs_pt->SetMarkerSize(1.5);

    gr_DATAResoMPF_TypeIpIICor_vs_pt = new TGraphErrors(0);
    gr_DATAResoMPF_TypeIpIICor_vs_pt->SetName("gr_DATAResoMPF_TypeIpIICor_vs_pt");
    gr_DATAResoMPF_TypeIpIICor_vs_pt->SetMarkerStyle(25);
    gr_DATAResoMPF_TypeIpIICor_vs_pt->SetMarkerColor(kBlack);
    gr_DATAResoMPF_TypeIpIICor_vs_pt->SetMarkerSize(1.5);

    gr_extrapResoMPF_TypeIpIICor_vs_pt = new TGraphErrors(0);
    gr_extrapResoMPF_TypeIpIICor_vs_pt->SetName("gr_extrapResoMPF_TypeIpIICor_vs_pt");
    gr_extrapResoMPF_TypeIpIICor_vs_pt->SetMarkerStyle(25);
    gr_extrapResoMPF_TypeIpIICor_vs_pt->SetMarkerColor(kBlack);
    gr_extrapResoMPF_TypeIpIICor_vs_pt->SetMarkerSize(1.5);
  }*/

  TGraphErrors* gr_mcRatioReso_vs_pt = new TGraphErrors(0);
  gr_mcRatioReso_vs_pt->SetName("gr_MCRatioReso_vs_pt");
  gr_mcRatioReso_vs_pt->SetMarkerStyle(25);
  gr_mcRatioReso_vs_pt->SetMarkerColor(kBlack);
  gr_mcRatioReso_vs_pt->SetMarkerSize(1.5);


  /*std::string ptPhotReco_vs_pt_name = "ptPhotMean_no2ndJet";
  if (etaRegion != "") ptPhotReco_vs_pt_name += "_" + etaRegion;
  TH2D* h2_ptPhotReco_vs_pt = (TH2D*)(get_mcFile(0))->Get(ptPhotReco_vs_pt_name.c_str());
  TH2D* h2_ptPhotReco_vs_ptDATA = (get_dataFile(0) == 0) ? 0 : (TH2D*)(get_dataFile(0))->Get(ptPhotReco_vs_pt_name.c_str());*/

  std::string suffix = get_fullSuffix();
  //std::string suffix = "Photon_Run2011_vs_G_plus_QCD_pfakt5_LUMI_2ndJet10"; // B A D !
  std::string graphFileName = "PhotonJetExtrapGraphs_" + suffix + "_" + etaRegion + ((rawJets) ? "RAW" : "") + "_" + FIT_RMS_;

  if (NOQ_) graphFileName += "_NOQ";
  if (FIXM_) graphFileName += "_FIXM";
  if (EXCLUDE_FIRST_POINT_) graphFileName += "_NOFIRSTP";

  graphFileName += ".root";

  TFile* graphFile = new TFile(graphFileName.c_str(), "recreate");
  graphFile->cd();

  for (uint32_t iPtBin = 0; iPtBin < (ptPhot_binning.size() - 2); //-3 instead of -1 (extrap reaches up to ~2 less bins in pt wrt balancing)
       ++iPtBin) {

    std::pair<float, float> currentBin = mPtBinning.getBinValue(iPtBin);
    float ptMin = currentBin.first;
    float ptMax = currentBin.second;

    std::string rawPostfix = (rawJets) ? "_raw" : "";

    char projName[100];
    sprintf(projName, "projection_%d", iPtBin);

    //TH1D* h1_proj = h2_ptPhotReco_vs_pt->ProjectionY(projName, iPtBin + 1, iPtBin + 1);
    //float ptPhotReco_thisBin = h1_proj->GetMean();
    //float ptPhotReco_err_thisBin = (h1_proj->GetEntries() > 1.) ? h1_proj->GetRMS() / sqrt(h1_proj->GetEntries()) : h1_proj->GetRMS();
    float ptPhotReco_thisBin =  (currentBin.first + currentBin.second) / 2.;
    float ptPhotReco_err_thisBin = 0;

    /*TH1D* h1_projDATA = h2_ptPhotReco_vs_ptDATA->ProjectionY(projName, iPtBin + 1, iPtBin + 1);
    float ptPhotReco_thisBinDATA = h1_projDATA->GetMean();
    float ptPhotReco_err_thisBinDATA = (h1_projDATA->GetEntries() > 1.) ? h1_projDATA->GetRMS() / sqrt(h1_projDATA->GetEntries()) : h1_projDATA->GetRMS();

    Double_t ptMin = ptPhot_binning[iPtBin];
    Double_t ptMax = ptPhot_binning[iPtBin + 1];*/

    int nPoints = mExtrapBinning.size();

    float x[nPoints];
    float x_err[nPoints];
    getXPoints(iPtBin, x, x_err);

    /*Float_t xDATA[nPoints];
    Float_t x_errDATA[nPoints];
    getXPoints(get_dataFile(0), xHistoName, nPoints, xDATA, x_errDATA);*/

    Float_t y_resp_DATA[nPoints];
    Float_t y_resp_err_DATA[nPoints];

    Float_t y_resp_recoPhot[nPoints];
    Float_t y_resp_recoPhot_err[nPoints];

    Float_t y_resp_MPFDATA[nPoints];
    Float_t y_resp_err_MPFDATA[nPoints];

    Float_t y_resp_MPF[nPoints];
    Float_t y_resp_MPF_err[nPoints];

    // Corrected MPF
    /*Float_t y_resp_MPFDATA_TypeICor[nPoints];
    Float_t y_resp_MPFDATA_TypeICor_err[nPoints];

    Float_t y_resp_MPF_TypeICor[nPoints];
    Float_t y_resp_MPF_TypeICor_err[nPoints];

    Float_t y_resp_MPFDATA_TypeIpIICor[nPoints];
    Float_t y_resp_MPFDATA_TypeIpIICor_err[nPoints];

    Float_t y_resp_MPF_TypeIpIICor[nPoints];
    Float_t y_resp_MPF_TypeIpIICor_err[nPoints];*/

    Float_t y_resp_genPhot[nPoints];
    Float_t y_resp_genPhot_err[nPoints];

    Float_t y_resp_genMPF[nPoints];
    Float_t y_resp_genMPF_err[nPoints];

    Float_t y_resp_genRecoPhot[nPoints];
    Float_t y_resp_genRecoPhot_err[nPoints];

    Float_t y_resp_recoGenMet[nPoints];
    Float_t y_resp_recoGenMet_err[nPoints];

    /*Float_t y_resp_recoGenCos[nPoints];
    Float_t y_resp_recoGenCos_err[nPoints];

    Float_t y_resp_genPart[nPoints];
    Float_t y_resp_genPart_err[nPoints];*/

    Float_t y_resp_genGamma[nPoints];
    Float_t y_resp_genGamma_err[nPoints];

    /*Float_t y_resp_partGamma[nPoints];
    Float_t y_resp_partGamma_err[nPoints];*/

    Float_t y_resp_photGamma[nPoints];
    Float_t y_resp_photGamma_err[nPoints];

    Float_t y_resp_recoGen[nPoints];
    Float_t y_resp_recoGen_err[nPoints];

    Float_t y_reso_DATA[nPoints];
    Float_t y_reso_err_DATA[nPoints];

    Float_t y_reso_recoPhot[nPoints];
    Float_t y_reso_recoPhot_err[nPoints];

    Float_t y_reso_MPFDATA[nPoints];
    Float_t y_reso_err_MPFDATA[nPoints];

    Float_t y_reso_MPF[nPoints];
    Float_t y_reso_MPF_err[nPoints];

    /*Float_t y_reso_MPFDATA_TypeICor[nPoints];
    Float_t y_reso_MPFDATA_TypeICor_err[nPoints];

    Float_t y_reso_MPF_TypeICor[nPoints];
    Float_t y_reso_MPF_TypeICor_err[nPoints];

    Float_t y_reso_MPFDATA_TypeIpIICor[nPoints];
    Float_t y_reso_MPFDATA_TypeIpIICor_err[nPoints];

    Float_t y_reso_MPF_TypeIpIICor[nPoints];
    Float_t y_reso_MPF_TypeIpIICor_err[nPoints];*/

    Float_t y_reso_genPhot[nPoints];
    Float_t y_reso_genPhot_err[nPoints];

    Float_t y_reso_genMPF[nPoints];
    Float_t y_reso_genMPF_err[nPoints];

    /*Float_t y_reso_genRecoPhot[nPoints];
    Float_t y_reso_genRecoPhot_err[nPoints];*/

    /*Float_t y_reso_recoGenMet[nPoints];
    Float_t y_reso_recoGenMet_err[nPoints];*/

    /*Float_t y_reso_recoGenCos[nPoints];
    Float_t y_reso_recoGenCos_err[nPoints];*/

    Float_t y_reso_recoGen[nPoints];
    Float_t y_reso_recoGen_err[nPoints];

    /*Float_t y_reso_genPart[nPoints];
    Float_t y_reso_genPart_err[nPoints];*/

    Float_t y_reso_genGamma[nPoints];
    Float_t y_reso_genGamma_err[nPoints];

    /*Float_t y_reso_partGamma[nPoints];
    Float_t y_reso_partGamma_err[nPoints];*/

    Float_t y_reso_photGamma[nPoints];
    Float_t y_reso_photGamma_err[nPoints];

    TString yHistoName = TString::Format("analysis/extrapolation/extrap_ptPhot_%d_%d/extrap_resp_balancing%s_%s", (int) currentBin.first, (int) currentBin.second, rawPostfix.c_str(), etaRegion.c_str());
    getYPoints(get_dataFile(0), yHistoName, nPoints, y_resp_DATA, y_resp_err_DATA,  y_reso_DATA, y_reso_err_DATA);
    getYPoints(get_mcFile(0), yHistoName, nPoints, y_resp_recoPhot, y_resp_recoPhot_err,  y_reso_recoPhot, y_reso_recoPhot_err);

    yHistoName = TString::Format("analysis/extrapolation/extrap_ptPhot_%d_%d/extrap_resp_mpf%s_%s", (int) currentBin.first, (int) currentBin.second, rawPostfix.c_str(), etaRegion.c_str());
    getYPoints(get_dataFile(0), yHistoName, nPoints, y_resp_MPFDATA, y_resp_err_MPFDATA,  y_reso_MPFDATA, y_reso_err_MPFDATA);
    getYPoints(get_mcFile(0), yHistoName, nPoints, y_resp_MPF, y_resp_MPF_err,  y_reso_MPF, y_reso_MPF_err);

    yHistoName = TString::Format("analysis/extrapolation/extrap_ptPhot_%d_%d/extrap_resp_balancing_gen_phot_%s", (int) currentBin.first, (int) currentBin.second, etaRegion.c_str());
    getYPoints(get_mcFile(0), yHistoName, nPoints, y_resp_genPhot, y_resp_genPhot_err,  y_reso_genPhot, y_reso_genPhot_err);

    yHistoName = TString::Format("analysis/extrapolation/extrap_ptPhot_%d_%d/extrap_resp_mpf%s_gen_%s", (int) currentBin.first, (int) currentBin.second, rawPostfix.c_str(), etaRegion.c_str());
    getYPoints(get_mcFile(0), yHistoName, nPoints, y_resp_genMPF, y_resp_genMPF_err,  y_reso_genMPF, y_reso_genMPF_err);

    yHistoName = TString::Format("analysis/extrapolation/extrap_ptPhot_%d_%d/extrap_resp_balancing%s_gen_%s", (int) currentBin.first, (int) currentBin.second, rawPostfix.c_str(), etaRegion.c_str());
    getYPoints(get_mcFile(0), yHistoName, nPoints, y_resp_recoGen, y_resp_recoGen_err,  y_reso_recoGen, y_reso_recoGen_err);

    yHistoName = TString::Format("analysis/extrapolation/extrap_ptPhot_%d_%d/extrap_resp_balancing_gen_gamma_%s", (int) currentBin.first, (int) currentBin.second, etaRegion.c_str());
    getYPoints(get_mcFile(0), yHistoName, nPoints, y_resp_genGamma, y_resp_genGamma_err,  y_reso_genGamma, y_reso_genGamma_err);

    yHistoName = TString::Format("analysis/extrapolation/extrap_ptPhot_%d_%d/extrap_resp_balancing_phot_gamma_%s", (int) currentBin.first, (int) currentBin.second, etaRegion.c_str());
    getYPoints(get_mcFile(0), yHistoName, nPoints, y_resp_photGamma, y_resp_photGamma_err,  y_reso_photGamma, y_reso_photGamma_err);

    // NPV Study
    /*const int npvMax = 31;

    // True Response / Resolution
    std::vector<Float_t> y_true_resp_npv;
    std::vector<Float_t> y_true_resp_npv_err;
    std::vector<Float_t> y_true_reso_npv;
    std::vector<Float_t> y_true_reso_npv_err;

    std::vector<Float_t> y_resp_npv_DATA;
    std::vector<Float_t> y_resp_npv_DATA_err;
    std::vector<Float_t> y_reso_npv_DATA;
    std::vector<Float_t> y_reso_npv_DATA_err;

    std::vector<Float_t> y_resp_npv_BAL;
    std::vector<Float_t> y_resp_npv_BAL_err;
    std::vector<Float_t> y_reso_npv_BAL;
    std::vector<Float_t> y_reso_npv_BAL_err;

    std::vector<Float_t> y_resp_npv_MPFDATA;
    std::vector<Float_t> y_resp_npv_MPFDATA_err;
    std::vector<Float_t> y_reso_npv_MPFDATA;
    std::vector<Float_t> y_reso_npv_MPFDATA_err;

    std::vector<Float_t> y_resp_npv_MPF;
    std::vector<Float_t> y_resp_npv_MPF_err;
    std::vector<Float_t> y_reso_npv_MPF;
    std::vector<Float_t> y_reso_npv_MPF_err;

    std::vector<Float_t> y_resp_npv_MPF_TypeICor_DATA;
    std::vector<Float_t> y_resp_npv_MPF_TypeICor_DATA_err;
    std::vector<Float_t> y_reso_npv_MPF_TypeICor_DATA;
    std::vector<Float_t> y_reso_npv_MPF_TypeICor_DATA_err;

    std::vector<Float_t> y_resp_npv_MPF_TypeICor_;
    std::vector<Float_t> y_resp_npv_MPF_TypeICor__err;
    std::vector<Float_t> y_reso_npv_MPF_TypeICor_;
    std::vector<Float_t> y_reso_npv_MPF_TypeICor__err;

    std::vector<Float_t> y_resp_npv_MPF_TypeIpIICor_DATA;
    std::vector<Float_t> y_resp_npv_MPF_TypeIpIICor_DATA_err;
    std::vector<Float_t> y_reso_npv_MPF_TypeIpIICor_DATA;
    std::vector<Float_t> y_reso_npv_MPF_TypeIpIICor_DATA_err;

    std::vector<Float_t> y_resp_npv_MPF_TypeIpIICor_;
    std::vector<Float_t> y_resp_npv_MPF_TypeIpIICor__err;
    std::vector<Float_t> y_reso_npv_MPF_TypeIpIICor_;
    std::vector<Float_t> y_reso_npv_MPF_TypeIpIICor__err;*/

    /*if (corrected)
      L2L3_text = "_L2L3";
    if (etaRegion != "")
      sprintf(yHistoName, "npv_ptPhot_%d_%d/response%s_%s_%d", (int)ptMin, (int)ptMax, L2L3_text.c_str(), etaRegion.c_str(), iPtBin);
    else
      sprintf(yHistoName, "npv_ptPhot_%d_%d/response%s_%d", (int)ptMin, (int)ptMax, L2L3_text.c_str(), iPtBin);
    getYPointsVector(get_dataFile(0), yHistoName, npvMax, y_resp_npv_DATA, y_resp_npv_DATA_err, y_reso_npv_DATA, y_reso_npv_DATA_err);
    getYPointsVector(get_mcFile(0), yHistoName, npvMax, y_resp_npv_BAL, y_resp_npv_BAL_err, y_reso_npv_BAL, y_reso_npv_BAL_err);

    if (! corrected) {
      if (etaRegion != "")
        sprintf(yHistoName, "npv_ptPhot_%d_%d/response%s_mpf_%s_%d", (int)ptMin, (int)ptMax, L2L3_text.c_str(), etaRegion.c_str(), iPtBin);
      else
        sprintf(yHistoName, "npv_ptPhot_%d_%d/response%s_mpf_%d", (int)ptMin, (int)ptMax, L2L3_text.c_str(), iPtBin);
      getYPointsVector(get_dataFile(0), yHistoName, npvMax, y_resp_npv_MPFDATA, y_resp_npv_MPFDATA_err, y_reso_npv_MPFDATA, y_reso_npv_MPFDATA_err);
      getYPointsVector(get_mcFile(0), yHistoName, npvMax, y_resp_npv_MPF, y_resp_npv_MPF_err, y_reso_npv_MPF, y_reso_npv_MPF_err);
    } else {
      if (etaRegion != "")
        sprintf(yHistoName, "npv_ptPhot_%d_%d/response_L2L3_mpf_TypeICor_%s_%d", (int)ptMin, (int)ptMax, etaRegion.c_str(), iPtBin);
      else
        sprintf(yHistoName, "npv_ptPhot_%d_%d/response_L2L3_mpf_TypeICor_%d", (int)ptMin, (int)ptMax, iPtBin);
      getYPointsVector(get_dataFile(0), yHistoName, npvMax, y_resp_npv_MPF_TypeICor_DATA, y_resp_npv_MPF_TypeICor_DATA_err, y_reso_npv_MPF_TypeICor_DATA, y_reso_npv_MPF_TypeICor_DATA_err);
      getYPointsVector(get_mcFile(0), yHistoName, npvMax, y_resp_npv_MPF_TypeICor_, y_resp_npv_MPF_TypeICor__err, y_reso_npv_MPF_TypeICor_, y_reso_npv_MPF_TypeICor__err);

      if (etaRegion != "")
        sprintf(yHistoName, "npv_ptPhot_%d_%d/response_L2L3_mpf_TypeIpIICor_%s_%d", (int)ptMin, (int)ptMax, etaRegion.c_str(), iPtBin);
      else
        sprintf(yHistoName, "npv_ptPhot_%d_%d/response_L2L3_mpf_TypeIpIICor_%d", (int)ptMin, (int)ptMax, iPtBin);
      getYPointsVector(get_dataFile(0), yHistoName, npvMax, y_resp_npv_MPF_TypeIpIICor_DATA, y_resp_npv_MPF_TypeIpIICor_DATA_err, y_reso_npv_MPF_TypeIpIICor_DATA, y_reso_npv_MPF_TypeIpIICor_DATA_err);
      getYPointsVector(get_mcFile(0), yHistoName, npvMax, y_resp_npv_MPF_TypeIpIICor_, y_resp_npv_MPF_TypeIpIICor__err, y_reso_npv_MPF_TypeIpIICor_, y_reso_npv_MPF_TypeIpIICor__err);

    }

    if (etaRegion != "")
      sprintf(yHistoName, "npv_ptPhot_%d_%d/true_response%s_%s_%d", (int)ptMin, (int)ptMax, L2L3_text.c_str(), etaRegion.c_str(), iPtBin);
    else
      sprintf(yHistoName, "npv_ptPhot_%d_%d/true_response%s_%s_%d", (int)ptMin, (int)ptMax, L2L3_text.c_str(), etaRegion.c_str(), iPtBin);
    getYPointsVector(get_mcFile(0), yHistoName, npvMax, y_true_resp_npv, y_true_resp_npv_err, y_true_reso_npv, y_true_reso_npv_err);*/

    //draw response histograms:

    TGraphErrors* gr_resp_DATA = new TGraphErrors(nPoints, x, y_resp_DATA, x_err, y_resp_err_DATA);
    gr_resp_DATA->SetMarkerStyle(20);
    gr_resp_DATA->SetMarkerColor(recoPhot_color);

    TGraphErrors* gr_resp_recoPhot = new TGraphErrors(nPoints, x, y_resp_recoPhot, x_err, y_resp_recoPhot_err);
    gr_resp_recoPhot->SetMarkerStyle(24);
    gr_resp_recoPhot->SetMarkerColor(recoPhot_color);

    TGraphErrors* gr_resp_MPFDATA = new TGraphErrors(nPoints, x, y_resp_MPFDATA, x_err, y_resp_err_MPFDATA);
    gr_resp_MPFDATA->SetMarkerStyle(20);
    gr_resp_MPFDATA->SetMarkerColor(recoPhot_color);

    TGraphErrors* gr_resp_MPF = new TGraphErrors(nPoints, x, y_resp_MPF, x_err, y_resp_MPF_err);
    gr_resp_MPF->SetMarkerStyle(24);
    gr_resp_MPF->SetMarkerColor(recoPhot_color);

    // Type I + II Corrected MPF
    /*TGraphErrors* gr_resp_MPFDATA_TypeICor = new TGraphErrors(nPoints, xDATA, y_resp_MPFDATA_TypeICor, x_errDATA, y_resp_MPFDATA_TypeICor_err);
    gr_resp_MPFDATA_TypeICor->SetMarkerStyle(20);
    gr_resp_MPFDATA_TypeICor->SetMarkerColor(recoPhot_color);

    TGraphErrors* gr_resp_MPF_TypeICor = new TGraphErrors(nPoints, x, y_resp_MPF_TypeICor, x_err, y_resp_MPF_TypeICor_err);
    gr_resp_MPF_TypeICor->SetMarkerStyle(24);
    gr_resp_MPF_TypeICor->SetMarkerColor(recoPhot_color);

    TGraphErrors* gr_resp_MPFDATA_TypeIpIICor = new TGraphErrors(nPoints, xDATA, y_resp_MPFDATA_TypeIpIICor, x_errDATA, y_resp_MPFDATA_TypeIpIICor_err);
    gr_resp_MPFDATA_TypeIpIICor->SetMarkerStyle(20);
    gr_resp_MPFDATA_TypeIpIICor->SetMarkerColor(recoPhot_color);

    TGraphErrors* gr_resp_MPF_TypeIpIICor = new TGraphErrors(nPoints, x, y_resp_MPF_TypeIpIICor, x_err, y_resp_MPF_TypeIpIICor_err);
    gr_resp_MPF_TypeIpIICor->SetMarkerStyle(24);
    gr_resp_MPF_TypeIpIICor->SetMarkerColor(recoPhot_color);*/

    TGraphErrors* gr_resp_genPhot = new TGraphErrors(nPoints, x, y_resp_genPhot, x_err, y_resp_genPhot_err);
    gr_resp_genPhot->SetMarkerStyle(22);
    //gr_resp_genPhot->SetMarkerColor(kGreen+3);
    gr_resp_genPhot->SetMarkerColor(genPhot_color);

    TGraphErrors* gr_resp_metRecoGen = new TGraphErrors(nPoints, x, y_resp_recoGenMet, x_err, y_resp_recoGenMet_err);
    gr_resp_metRecoGen->SetMarkerStyle(21);
    //gr_resp_genPhot->SetMarkerColor(kGreen+3);
    gr_resp_metRecoGen->SetMarkerColor(genPhot_color);

    /*TGraphErrors* gr_resp_cosRecoGen = new TGraphErrors(nPoints, x, y_resp_recoGenCos, x_err, y_resp_recoGenCos_err);
    gr_resp_cosRecoGen->SetMarkerStyle(21);
    gr_resp_cosRecoGen->SetMarkerColor(kOrange);
    //gr_resp_metRecoGen->SetMarkerColor(genPhot_color);*/

    TGraphErrors* gr_resp_genMPF = new TGraphErrors(nPoints, x, y_resp_genMPF, x_err, y_resp_genMPF_err);
    gr_resp_genMPF->SetMarkerStyle(22);
    //gr_resp_genPhot->SetMarkerColor(kGreen+3);
    gr_resp_genMPF->SetMarkerColor(genPhot_color);

    TGraphErrors* gr_resp_recoGen = new TGraphErrors(nPoints, x, y_resp_recoGen, x_err, y_resp_recoGen_err);
    gr_resp_recoGen->SetMarkerStyle(21);
    gr_resp_recoGen->SetMarkerColor(recoGen_color);

    TGraphErrors* gr_resp_photGenReco = new TGraphErrors(nPoints, x, y_resp_genRecoPhot, x_err, y_resp_genRecoPhot_err);
    gr_resp_photGenReco->SetMarkerStyle(21);
    gr_resp_photGenReco->SetMarkerColor(recoGen_color);

    /*TGraphErrors* gr_resp_genPart = new TGraphErrors(nPoints, x, y_resp_genPart, x_err, y_resp_genPart_err);
    gr_resp_genPart->SetMarkerStyle(21);
    gr_resp_genPart->SetMarkerColor(kGreen);*/

    TGraphErrors* gr_resp_genGamma = new TGraphErrors(nPoints, x, y_resp_genGamma, x_err, y_resp_genGamma_err);
    gr_resp_genGamma->SetMarkerStyle(21);
    gr_resp_genGamma->SetMarkerColor(kGreen);

    /*TGraphErrors* gr_resp_partGamma = new TGraphErrors(nPoints, x, y_resp_partGamma, x_err, y_resp_partGamma_err);
    gr_resp_partGamma->SetMarkerStyle(21);
    gr_resp_partGamma->SetMarkerColor(kYellow);*/

    TGraphErrors* gr_resp_photGamma = new TGraphErrors(nPoints, x, y_resp_photGamma, x_err, y_resp_photGamma_err);
    gr_resp_photGamma->SetMarkerStyle(21);
    gr_resp_photGamma->SetMarkerColor(kGray);

    /*if (recoGen != "RecoRelRaw") {
      // NPV Study
      Float_t npv_x[npvMax];
      Float_t npv_x_err[npvMax] = {0};
      for (int i = 0; i < npvMax; i++)
        npv_x[i] = i;

      // True Response
      TGraphErrors* gr_true_resp_vs_npv = new TGraphErrors(y_true_resp_npv.size(), npv_x, (Float_t *) &y_true_resp_npv[0], npv_x_err, (Float_t *) &y_true_resp_npv_err[0]);
      gr_true_resp_vs_npv->SetName("gr_true_resp_vs_npv");
      gr_true_resp_vs_npv->RemovePoint(0); // nothing interesting at NPV = 0

      // Balancing
      TGraphErrors* gr_resp_bal_data_vs_npv = new TGraphErrors(y_resp_npv_DATA.size(), npv_x, (Float_t *) &y_resp_npv_DATA[0], npv_x_err, (Float_t *) &y_resp_npv_DATA_err[0]);
      gr_resp_bal_data_vs_npv->SetName("gr_resp_bal_data_vs_npv");
      gr_resp_bal_data_vs_npv->RemovePoint(0); // nothing interesting at NPV = 0
      gr_resp_bal_data_vs_npv->SetMarkerStyle(20);
      gr_resp_bal_data_vs_npv->SetMarkerColor(recoPhot_color);

      TGraphErrors* gr_resp_bal_vs_npv = new TGraphErrors(y_resp_npv_BAL.size(), npv_x, (Float_t*) &y_resp_npv_BAL[0], npv_x_err, (Float_t *) &y_resp_npv_BAL_err[0]);
      gr_resp_bal_vs_npv->SetName("gr_resp_bal_vs_npv");
      gr_resp_bal_vs_npv->RemovePoint(0); // nothing interesting at NPV = 0
      gr_resp_bal_vs_npv->SetMarkerStyle(24);
      gr_resp_bal_vs_npv->SetMarkerColor(recoPhot_color);

      // MPF
      TGraphErrors* gr_resp_mpf_data_vs_npv = NULL;
      TGraphErrors* gr_resp_mpf_TypeICor_data_vs_npv = NULL;
      TGraphErrors* gr_resp_mpf_TypeIpIICor_data_vs_npv = NULL;

      TGraphErrors* gr_resp_mpf_vs_npv = NULL;
      TGraphErrors* gr_resp_mpf_TypeICor_vs_npv = NULL;
      TGraphErrors* gr_resp_mpf_TypeIpIICor_vs_npv = NULL;

      if (! corrected) {
        gr_resp_mpf_data_vs_npv = new TGraphErrors(y_resp_npv_MPFDATA.size(), npv_x, (Float_t *) &y_resp_npv_MPFDATA[0], npv_x_err, (Float_t *) &y_resp_npv_MPFDATA_err[0]);
        gr_resp_mpf_data_vs_npv->SetName("gr_resp_mpf_data_vs_npv");
        gr_resp_mpf_data_vs_npv->RemovePoint(0); // nothing interesting at NPV = 0
        gr_resp_mpf_data_vs_npv->SetMarkerStyle(20);
        gr_resp_mpf_data_vs_npv->SetMarkerColor(recoPhot_color);

        gr_resp_mpf_vs_npv = new TGraphErrors(y_resp_npv_MPF.size(), npv_x, (Float_t*) &y_resp_npv_MPF[0], npv_x_err, (Float_t *) &y_resp_npv_MPF_err[0]);
        gr_resp_mpf_vs_npv->SetName("gr_resp_mpf_vs_npv");
        gr_resp_mpf_vs_npv->RemovePoint(0); // nothing interesting at NPV = 0
        gr_resp_mpf_vs_npv->SetMarkerStyle(24);
        gr_resp_mpf_vs_npv->SetMarkerColor(recoPhot_color);
      } else {
        gr_resp_mpf_TypeICor_data_vs_npv = new TGraphErrors(y_resp_npv_MPF_TypeICor_DATA.size(), npv_x, (Float_t *) &y_resp_npv_MPF_TypeICor_DATA[0], npv_x_err, (Float_t *) &y_resp_npv_MPF_TypeICor_DATA_err[0]);
        gr_resp_mpf_TypeICor_data_vs_npv->SetName("gr_resp_mpf_TypeICor_data_vs_npv");
        gr_resp_mpf_TypeICor_data_vs_npv->RemovePoint(0); // nothing interesting at NPV = 0
        gr_resp_mpf_TypeICor_data_vs_npv->SetMarkerStyle(20);
        gr_resp_mpf_TypeICor_data_vs_npv->SetMarkerColor(recoPhot_color);

        gr_resp_mpf_TypeICor_vs_npv = new TGraphErrors(y_resp_npv_MPF_TypeICor_.size(), npv_x, (Float_t*) &y_resp_npv_MPF_TypeICor_[0], npv_x_err, (Float_t *) &y_resp_npv_MPF_TypeICor__err[0]);
        gr_resp_mpf_TypeICor_vs_npv->SetName("gr_resp_mpf_TypeICor_vs_npv");
        gr_resp_mpf_TypeICor_vs_npv->RemovePoint(0); // nothing interesting at NPV = 0
        gr_resp_mpf_TypeICor_vs_npv->SetMarkerStyle(24);
        gr_resp_mpf_TypeICor_vs_npv->SetMarkerColor(recoPhot_color);

        gr_resp_mpf_TypeIpIICor_data_vs_npv = new TGraphErrors(y_resp_npv_MPF_TypeIpIICor_DATA.size(), npv_x, (Float_t *) &y_resp_npv_MPF_TypeIpIICor_DATA[0], npv_x_err, (Float_t *) &y_resp_npv_MPF_TypeIpIICor_DATA_err[0]);
        gr_resp_mpf_TypeIpIICor_data_vs_npv->SetName("gr_resp_mpf_TypeIpIICor_data_vs_npv");
        gr_resp_mpf_TypeIpIICor_data_vs_npv->RemovePoint(0); // nothing interesting at NPV = 0
        gr_resp_mpf_TypeIpIICor_data_vs_npv->SetMarkerStyle(20);
        gr_resp_mpf_TypeIpIICor_data_vs_npv->SetMarkerColor(recoPhot_color);

        gr_resp_mpf_TypeIpIICor_vs_npv = new TGraphErrors(y_resp_npv_MPF_TypeIpIICor_.size(), npv_x, (Float_t*) &y_resp_npv_MPF_TypeIpIICor_[0], npv_x_err, (Float_t *) &y_resp_npv_MPF_TypeIpIICor__err[0]);
        gr_resp_mpf_TypeIpIICor_vs_npv->SetName("gr_resp_mpf_TypeIpIICor_vs_npv");
        gr_resp_mpf_TypeIpIICor_vs_npv->RemovePoint(0); // nothing interesting at NPV = 0
        gr_resp_mpf_TypeIpIICor_vs_npv->SetMarkerStyle(24);
        gr_resp_mpf_TypeIpIICor_vs_npv->SetMarkerColor(recoPhot_color);
      }

      // Fits
      
         const std::string fct = "[0] + [1]*x";
      // Balancing
      TF1 *fit_resp_bal_data_vs_npv = new TF1("fit1", fct.c_str(), 0, y_resp_npv_DATA.size());
      fit_resp_bal_data_vs_npv->SetLineColor(recoPhot_color);
      fit_resp_bal_data_vs_npv->SetParameter(0, 1);
      fit_resp_bal_data_vs_npv->SetParameter(1, 0);
      gr_resp_bal_data_vs_npv->Fit(fit_resp_bal_data_vs_npv, "QR");

      TF1 *fit_resp_bal_vs_npv = new TF1("fit2", fct.c_str(), 0, y_resp_npv_BAL.size());
      fit_resp_bal_vs_npv->SetLineColor(recoPhot_color);
      fit_resp_bal_vs_npv->SetParameter(0, 1);
      fit_resp_bal_vs_npv->SetParameter(1, 0);
      gr_resp_bal_vs_npv->Fit(fit_resp_bal_vs_npv, "QR");

      // MPF
      TF1 *fit_resp_mpf_data_vs_npv = new TF1("fit3", fct.c_str(), 0, y_resp_npv_DATA.size());
      fit_resp_mpf_data_vs_npv->SetLineColor(recoPhot_color);
      fit_resp_mpf_data_vs_npv->SetParameter(0, 1);
      fit_resp_mpf_data_vs_npv->SetParameter(1, 0);
      gr_resp_mpf_data_vs_npv->Fit(fit_resp_mpf_data_vs_npv, "QR");

      TF1 *fit_resp_mpf_vs_npv = new TF1("fit4", fct.c_str(), 0, y_resp_npv_BAL.size());
      fit_resp_mpf_vs_npv->SetLineColor(recoPhot_color);
      fit_resp_mpf_vs_npv->SetParameter(0, 1);
      fit_resp_mpf_vs_npv->SetParameter(1, 0);
      gr_resp_mpf_vs_npv->Fit(fit_resp_mpf_vs_npv, "QR");
      

      TString dirName = TString::Format("npv_ptPhot_%d_%d", (int) ptMin, (int) ptMax);

      graphFile->mkdir(dirName);
      graphFile->cd(dirName);

      gr_true_resp_vs_npv->Write();

      gr_resp_bal_data_vs_npv->Write();
      gr_resp_bal_vs_npv->Write();

      if (! corrected) {
        gr_resp_mpf_data_vs_npv->Write();
        gr_resp_mpf_vs_npv->Write();

        delete gr_resp_mpf_data_vs_npv;
        delete gr_resp_mpf_vs_npv;
      } else {
        gr_resp_mpf_TypeICor_data_vs_npv->Write();
        gr_resp_mpf_TypeICor_vs_npv->Write();

        delete gr_resp_mpf_TypeICor_data_vs_npv;
        delete gr_resp_mpf_TypeICor_vs_npv;

        gr_resp_mpf_TypeIpIICor_data_vs_npv->Write();
        gr_resp_mpf_TypeIpIICor_vs_npv->Write();

        delete gr_resp_mpf_TypeIpIICor_data_vs_npv;
        delete gr_resp_mpf_TypeIpIICor_vs_npv;
      }

      delete gr_true_resp_vs_npv;

      delete gr_resp_bal_data_vs_npv;
      delete gr_resp_bal_vs_npv;

      graphFile->cd();
    }*/

    Float_t lastX = x[nPoints - 1];
    Float_t xMax_fit = lastX + 2. / 100.;
    //Float_t xMax_fit_green = x[2]+2./100.;

    Float_t xMax_axis;
    if (lastX <= 12. / 100.)
      xMax_axis = 15. / 100.;
    else if (lastX <= 20. / 100.)
      xMax_axis = 25. / 100.;
    else if (lastX <= 25. / 100.)
      xMax_axis = 30. / 100.;
    else if (lastX <= 35. / 100.)
      xMax_axis = 40. / 100.;


    std::string xTitle = "p_{T}^{2ndJet} / p_{T}^{#gamma}";

    std::string fitFunct_name;
    ////  if( ptMin <=150. )
    fitFunct_name = "[0] - x*x*[1]";
    ////  else
    ////    fitFunct_name = "[0] - x*[1]";


    TF1* fit_resp_genPhot = new TF1("fit_resp_genPhot", fitFunct_name.c_str());
    fit_resp_genPhot->SetRange(0., xMax_fit);
    fit_resp_genPhot->SetLineWidth(0.5);
    //fit_resp_genPhot->SetLineColor(kGreen+3);
    fit_resp_genPhot->SetLineColor(genPhot_color);
    gr_resp_genPhot->Fit(fit_resp_genPhot, "RQ");

    TF1* fit_resp_recoGen = new TF1("fit_resp_recoGen", "[0]");
    fit_resp_recoGen->SetRange(0., xMax_fit);
    fit_resp_recoGen->SetLineWidth(0.5);
    fit_resp_recoGen->SetLineColor(recoGen_color);
    gr_resp_recoGen->Fit(fit_resp_recoGen, "RQ");

    TF1* fit_resp_genRecoPhot = new TF1("fit_resp_genRecoPhot", "[0]");
    fit_resp_genRecoPhot->SetRange(0., xMax_fit);
    fit_resp_genRecoPhot->SetLineWidth(0.5);
    fit_resp_genRecoPhot->SetLineColor(recoGen_color);
    gr_resp_photGenReco->Fit(fit_resp_genRecoPhot, "RQ");

    TF1* fit_resp_recoGenMet = new TF1("fit_resp_recoGenMet", "[0]");
    fit_resp_recoGenMet->SetRange(0., xMax_fit);
    fit_resp_recoGenMet->SetLineWidth(0.5);
    fit_resp_recoGenMet->SetLineColor(kGreen);
    gr_resp_metRecoGen->Fit(fit_resp_recoGenMet, "RQ");

    /*TF1* fit_resp_recoGenCos = new TF1("fit_resp_recoGenCos", "[0] - x*x*[1]");
    fit_resp_recoGenCos->SetRange(0., xMax_fit);
    fit_resp_recoGenCos->SetLineWidth(0.5);
    fit_resp_recoGenCos->SetLineColor(kOrange);
    if (!corrected && recoGen != "RecoRelRaw")
      gr_resp_cosRecoGen->Fit(fit_resp_recoGenCos, "RQ");*/

    fitFunct_name = "[0]*[1] - x*x*[2]";

    TF1* fit_resp_genMPF = new TF1("fit_resp_genMPF", "[0]");
    fit_resp_genMPF->SetRange(0., xMax_fit);
    fit_resp_genMPF->SetLineWidth(0.5);
    fit_resp_genMPF->SetLineColor(kGreen);
    gr_resp_genMPF->Fit(fit_resp_genMPF, "RQ");

    /*TF1* fit_resp_genPart = new TF1("fit_resp_genPart", "[0]+[1]*x");
    fit_resp_genPart->SetRange(0., xMax_fit);
    fit_resp_genPart->SetLineWidth(0.5);
    fit_resp_genPart->SetLineColor(kGreen);
    gr_resp_genPart->Fit(fit_resp_genPart, "RQ");*/

    /*TF1* fit_resp_partGamma = new TF1("fit_resp_partGamma", "[0]+[1]*x");
    fit_resp_partGamma->SetRange(0., xMax_fit);
    fit_resp_partGamma->SetLineWidth(0.5);
    fit_resp_partGamma->SetLineColor(kYellow);
    gr_resp_partGamma->Fit(fit_resp_partGamma, "RQ");*/

    TF1* fit_resp_photGamma = new TF1("fit_resp_photGamma", "[0]");
    fit_resp_photGamma->SetRange(0., xMax_fit);
    fit_resp_photGamma->SetLineWidth(0.5);
    fit_resp_photGamma->SetLineColor(kGray);
    gr_resp_photGamma->Fit(fit_resp_photGamma, "RQ");

    std::string total_resp_str = "fit_resp_recoGen*fit_resp_genPhot";
    TF1* total_resp = new TF1("total_resp", total_resp_str.c_str());
    total_resp->SetRange(0., xMax_fit);
    total_resp->SetLineColor(kGray + 2);

    float q_resp = fit_resp_genPhot->GetParameter(0);

    fitFunct_name = "[0]*[1] - x*x*[2]";
    TF1* fit_respParabola = new TF1("fit_respParabola", fitFunct_name.c_str());
    fit_respParabola->SetRange(0., xMax_fit);
    if (NOQ_) {  //to evaluate syst
      float delta_q_resp = fabs((1. - q_resp) / 2.);
      if (q_resp > 1.) fit_respParabola->FixParameter(1, 1. + delta_q_resp);
      else            fit_respParabola->FixParameter(1, 1. - delta_q_resp);
    } else {
      fit_respParabola->FixParameter(1, fit_resp_genPhot->GetParameter(0));
    }
    // if( FIXM_ ) {
    //   fit_respParabola->FixParameter(2, fit_resp_genPhot->GetParameter(1));
    // } else {
    fit_respParabola->SetParameter(2, fit_resp_genPhot->GetParameter(1));
    // }
    fit_respParabola->SetLineColor(2);
    fit_respParabola->SetLineColor(recoPhot_color);
    fit_respParabola->SetLineStyle(2);
    fit_respParabola->SetLineWidth(1.);
    gr_resp_recoPhot->Fit(fit_respParabola, "RQ");

    const std::string lineFunction = "[0] + [1]*x";
    TF1* fit_respParabolaMPF = new TF1("fit_respParabolaMPF", lineFunction.c_str());
    fit_respParabolaMPF->SetRange(0., xMax_fit);
    fit_respParabolaMPF->SetParameter(0, fit_resp_genMPF->GetParameter(0));
    fit_respParabolaMPF->SetParameter(1, 0);
    fit_respParabolaMPF->SetLineColor(2);
    fit_respParabolaMPF->SetLineColor(recoPhot_color);
    fit_respParabolaMPF->SetLineStyle(2);
    fit_respParabolaMPF->SetLineWidth(1.);
    gr_resp_MPF->Fit(fit_respParabolaMPF, "RQ");

    TF1* fit_respParabola_DATA = new TF1("fit_respParabola_DATA", fitFunct_name.c_str());
    fit_respParabola_DATA->SetRange(0., xMax_fit);
    if (NOQ_) {  //to evaluate syst
      float delta_q_resp = fabs((1. - q_resp) / 2.);
      if (q_resp > 1.) fit_respParabola_DATA->FixParameter(1, 1. + delta_q_resp);
      else            fit_respParabola_DATA->FixParameter(1, 1. - delta_q_resp);
    } else {
      fit_respParabola_DATA->FixParameter(1, fit_resp_genPhot->GetParameter(0));
    }
    if (FIXM_)
      fit_respParabola_DATA->FixParameter(2, fit_respParabola->GetParameter(2));
    fit_respParabola_DATA->SetLineColor(recoPhot_color);
    fit_respParabola_DATA->SetLineWidth(1.);
    gr_resp_DATA->Fit(fit_respParabola_DATA, "RQ");


    TF1* fit_respParabola_MPFDATA = new TF1("fit_respParabola_MPFDATA", lineFunction.c_str());
    fit_respParabola_MPFDATA->SetRange(0., xMax_fit);
    fit_respParabola_MPFDATA->SetParameter(0, fit_resp_genMPF->GetParameter(0));
    fit_respParabola_MPFDATA->SetParameter(1, 0);
    fit_respParabola_MPFDATA->SetLineColor(recoPhot_color);
    fit_respParabola_MPFDATA->SetLineWidth(1.);
    gr_resp_MPFDATA->Fit(fit_respParabola_MPFDATA, "RQ");

    /* Type I+II corrected MPF */
    /*TF1* fit_resp_MPF_TypeICor = NULL, *fit_resp_MPF_TypeIpIICor = NULL;
      TF1* fit_resp_MPFDATA_TypeICor = NULL, *fit_resp_MPFDATA_TypeIpIICor = NULL;
      if (corrected) {
      fit_resp_MPF_TypeICor = new TF1("fit_resp_MPF_TypeICor", lineFunction.c_str(), 0, xMax_fit);
      fit_resp_MPF_TypeICor->SetParameter(0, 1);
      fit_resp_MPF_TypeICor->SetParameter(1, 0);
      fit_resp_MPF_TypeICor->SetLineColor(recoPhot_color);
      fit_resp_MPF_TypeICor->SetLineWidth(1.);
      gr_resp_MPF_TypeICor->Fit(fit_resp_MPF_TypeICor, "QR");

      fit_resp_MPFDATA_TypeICor = new TF1("fit_resp_MPFDATA_TypeICor", lineFunction.c_str(), 0, xMax_fit);
      fit_resp_MPFDATA_TypeICor->SetParameter(0, 1);
      fit_resp_MPFDATA_TypeICor->SetParameter(1, 0);
      fit_resp_MPFDATA_TypeICor->SetLineColor(recoPhot_color);
      fit_resp_MPFDATA_TypeICor->SetLineWidth(1.);
      gr_resp_MPFDATA_TypeICor->Fit(fit_resp_MPFDATA_TypeICor, "QR");

      fit_resp_MPF_TypeIpIICor = new TF1("fit_resp_MPF_TypeIpIICor", lineFunction.c_str(), 0, xMax_fit);
      fit_resp_MPF_TypeIpIICor->SetParameter(0, 1);
      fit_resp_MPF_TypeIpIICor->SetParameter(1, 0);
      fit_resp_MPF_TypeIpIICor->SetLineColor(recoPhot_color);
      fit_resp_MPF_TypeIpIICor->SetLineWidth(1.);
      gr_resp_MPF_TypeIpIICor->Fit(fit_resp_MPF_TypeIpIICor, "QR");

      fit_resp_MPFDATA_TypeIpIICor = new TF1("fit_resp_MPFDATA_TypeIpIICor", lineFunction.c_str(), 0, xMax_fit);
      fit_resp_MPFDATA_TypeIpIICor->SetParameter(0, 1);
      fit_resp_MPFDATA_TypeIpIICor->SetParameter(1, 0);
      fit_resp_MPFDATA_TypeIpIICor->SetLineColor(recoPhot_color);
      fit_resp_MPFDATA_TypeIpIICor->SetLineWidth(1.);
      gr_resp_MPFDATA_TypeIpIICor->Fit(fit_resp_MPFDATA_TypeIpIICor, "QR");
      }*/


    /*std::string total_mpf_str = "1+(fit_resp_recoGenCos*fit_resp_genRecoPhot*fit_resp_recoGenMet*(fit_resp_genMPF - 1))";
      TF1* total_mpf = new TF1("total_mpf", total_mpf_str.c_str());
      total_resp->SetRange(0., xMax_fit);
      total_resp->SetLineColor(kGray);*/

    // set response graph points:

    gr_DATAResp_vs_pt->SetPoint(iPtBin, ptPhotReco_thisBin, fit_respParabola_DATA->GetParameter(0));
    gr_DATAResp_vs_pt->SetPointError(iPtBin, ptPhotReco_err_thisBin, fit_respParabola_DATA->GetParError(0));

    gr_extrapResp_vs_pt->SetPoint(iPtBin, ptPhotReco_thisBin, fit_respParabola->GetParameter(0));
    gr_extrapResp_vs_pt->SetPointError(iPtBin, ptPhotReco_err_thisBin, fit_respParabola->GetParError(0));

    gr_DATARespMPF_vs_pt->SetPoint(iPtBin, ptPhotReco_thisBin, fit_respParabola_MPFDATA->GetParameter(0));
    gr_DATARespMPF_vs_pt->SetPointError(iPtBin, ptPhotReco_err_thisBin, fit_respParabola_MPFDATA->GetParError(0));

    gr_extrapRespMPF_vs_pt->SetPoint(iPtBin, ptPhotReco_thisBin, fit_respParabolaMPF->GetParameter(0));
    gr_extrapRespMPF_vs_pt->SetPointError(iPtBin, ptPhotReco_err_thisBin, fit_respParabolaMPF->GetParError(0));
    /* else if (recoGen != "RecoRelRaw") {
       gr_DATARespMPF_TypeICor_vs_pt->SetPoint(iPtBin, ptPhotReco_thisBinDATA, fit_resp_MPFDATA_TypeICor->GetParameter(0));
       gr_DATARespMPF_TypeICor_vs_pt->SetPointError(iPtBin, ptPhotReco_err_thisBinDATA, fit_resp_MPFDATA_TypeICor->GetParError(0));
       gr_extrapRespMPF_TypeICor_vs_pt->SetPoint(iPtBin, ptPhotReco_thisBin, fit_resp_MPF_TypeICor->GetParameter(0));
       gr_extrapRespMPF_TypeICor_vs_pt->SetPointError(iPtBin, ptPhotReco_err_thisBin, fit_resp_MPF_TypeICor->GetParError(0));

       gr_DATARespMPF_TypeIpIICor_vs_pt->SetPoint(iPtBin, ptPhotReco_thisBinDATA, fit_resp_MPFDATA_TypeIpIICor->GetParameter(0));
       gr_DATARespMPF_TypeIpIICor_vs_pt->SetPointError(iPtBin, ptPhotReco_err_thisBinDATA, fit_resp_MPFDATA_TypeIpIICor->GetParError(0));
       gr_extrapRespMPF_TypeIpIICor_vs_pt->SetPoint(iPtBin, ptPhotReco_thisBin, fit_resp_MPF_TypeIpIICor->GetParameter(0));
       gr_extrapRespMPF_TypeIpIICor_vs_pt->SetPointError(iPtBin, ptPhotReco_err_thisBin, fit_resp_MPF_TypeIpIICor->GetParError(0));
       }*/

    gr_intrResp_vs_pt->SetPoint(iPtBin, ptPhotReco_thisBin, fit_resp_recoGen->GetParameter(0));
    gr_intrResp_vs_pt->SetPointError(iPtBin, ptPhotReco_err_thisBin, fit_resp_recoGen->GetParError(0));

    gr_qResp_vs_pt->SetPoint(iPtBin, ptPhotReco_thisBin, (fit_resp_genPhot->GetParameter(0) - 1.));
    gr_qResp_vs_pt->SetPointError(iPtBin, ptPhotReco_err_thisBin, (fit_resp_genPhot->GetParError(0)));

    /*gr_qGenPartResp_vs_pt->SetPoint(iPtBin, ptPhotReco_thisBin, (fit_resp_genPart->GetParameter(0) - 1.));
      gr_qGenPartResp_vs_pt->SetPointError(iPtBin, ptPhotReco_err_thisBin, (fit_resp_genPart->GetParError(0)));

      gr_qPartGammaResp_vs_pt->SetPoint(iPtBin, ptPhotReco_thisBin, (fit_resp_partGamma->GetParameter(0) - 1.));
      gr_qPartGammaResp_vs_pt->SetPointError(iPtBin, ptPhotReco_err_thisBin, (fit_resp_partGamma->GetParError(0)));*/

    gr_qPhotGammaResp_vs_pt->SetPoint(iPtBin, ptPhotReco_thisBin, (fit_resp_photGamma->GetParameter(0) - 1.));
    gr_qPhotGammaResp_vs_pt->SetPointError(iPtBin, ptPhotReco_err_thisBin, (fit_resp_photGamma->GetParError(0)));


    //Float_t y0_recoGen = fit_resp_recoGen->GetParameter(0);
    //Float_t y0_genPhot = fit_resp_genPhot->GetParameter(0);
    //Float_t y0_err_recoGen = fit_resp_recoGen->GetParError(0);
    //Float_t y0_err_genPhot = fit_resp_genPhot->GetParError(0);
    //
    //gr_qGenPartResp_vs_pt->SetPoint( iPtBin, ptPhotReco_thisBin, y0_genPhot*y0_recoGen );
    //float trueResp_err = y0_err_genPhot*y0_err_genPhot*y0_recoGen*y0_recoGen;
    //trueResp_err += y0_genPhot*y0_genPhot*y0_err_recoGen*y0_err_recoGen;
    //trueResp_err = sqrt( trueResp_err );
    //gr_qGenPartResp_vs_pt->SetPointError( iPtBin, ptPhotReco_err_thisBin, trueResp_err );


    Float_t yMin_axis;
    if (this->get_recoType() == "calo") {
      yMin_axis = (currentBin.first < 80.) ? 0.3 : 0.5;
      if (currentBin.first < 20.) yMin_axis = 0.2;
    } else {
      yMin_axis = (currentBin.first < 80.) ? 0.7 : 0.7;
      if (currentBin.first < 20.) yMin_axis = 0.6;
    }
    if (etaRegion == "eta1524" || etaRegion == "eta243") yMin_axis -= 0.1;


    float yMax_resp = (! rawJets) ? 1.3 : 1.2;

    TH2D* h2_axes_resp = new TH2D("axes_resp", "", 10, 0., xMax_axis, 10, yMin_axis, yMax_resp);
    h2_axes_resp->SetXTitle(xTitle.c_str());
    h2_axes_resp->SetYTitle((rawJets) ? "Response (with raw jets)" : "Response");
    //h2_axes_resp->GetXaxis()->SetTitleOffset(1.1);
    //h2_axes_resp->GetYaxis()->SetTitleOffset(1.5);

    //LegendBox legbox = this->get_legendBox(1);
    LegendBox legbox;
    legbox.xMin = 0.6;
    legbox.yMin = 0.67;
    legbox.xMax = 0.92;
    legbox.yMax = 0.92;

    TLegend* legend_resp;
    if (etaRegion_str != "")
      legend_resp = new TLegend(legbox.xMin, legbox.yMin, legbox.xMax, legbox.yMax, etaRegion_str.c_str());
    else
      legend_resp = new TLegend(legbox.xMin, legbox.yMin, legbox.xMax, legbox.yMax);
    legend_resp->SetTextSize(0.035);
    legend_resp->SetTextFont(42);
    //legend_resp->SetFillStyle(0);
    legend_resp->SetFillColor(kWhite);
    legend_resp->AddEntry(gr_resp_recoGen, "MC Intrinsic", "P");
    legend_resp->AddEntry(gr_resp_genPhot, "MC Imbalance", "P");
    legend_resp->AddEntry(total_resp, "MC Intr #oplus Imb", "L");

    legend_resp->AddEntry(gr_resp_DATA, "DATA ( #gamma + jet)", "P");
    legend_resp->AddEntry(gr_resp_recoPhot, "MC (#gamma + jet)", "P");


    char labeltext[50];
    sprintf(labeltext, "%d < p_{T}^{#gamma} < %d GeV", (int)ptMin, (int)ptMax);
    //sprintf(labeltext, "%d < p_{T}^{#gamma} < %d GeV/c", (int)ptMin, (int)ptMax);
    TPaveText* label_resp = new TPaveText(0.18, 0.15, 0.4, 0.18, "brNDC");
    label_resp->SetFillColor(kWhite);
    label_resp->SetTextSize(0.035);
    label_resp->AddText(labeltext);

    TPaveText* label_algo = this->get_labelAlgo(4);

    TPaveText* label_cms = this->get_labelCMS(0);
    TPaveText* label_sqrt = this->get_labelSqrt(0);

    //TLatex* label_CMStop = this->get_labelCMStop();

    float markerSize = 1.4;
    gr_resp_recoGen->SetMarkerSize(markerSize);
    gr_resp_genPhot->SetMarkerSize(markerSize);
    gr_resp_recoPhot->SetMarkerSize(markerSize);
    gr_resp_DATA->SetMarkerSize(markerSize);
    gr_resp_MPFDATA->SetMarkerSize(markerSize);
    gr_resp_MPF->SetMarkerSize(markerSize);

    TCanvas* c1_resp = new TCanvas("c1_resp", "c1_resp", 600, 600);
    //c1_resp->SetLeftMargin(0.12);
    //c1_resp->SetBottomMargin(0.12);
    c1_resp->cd();
    h2_axes_resp->Draw();
    total_resp->Draw("same");
    gr_resp_recoGen->Draw("Psame");
    gr_resp_genPhot->Draw("Psame");
    legend_resp->Draw("same");
    label_resp->Draw("same");
    label_cms->Draw("same");
    label_sqrt->Draw("same");
    //label_CMStop->Draw("same");
    label_algo->Draw("same");
    gr_resp_recoPhot->Draw("Psame");
    gr_resp_DATA->Draw("Psame");

    rawPostfix = (rawJets) ? "RAW" : "";

    char canvasName_resp_eps[500];
    char canvasName_resp_png[500];

    sprintf(canvasName_resp_eps, "%s/response%s_%s_ptPhot_%d_%d.eps", get_outputdir().c_str(), rawPostfix.c_str(), etaRegion.c_str(), (int)ptMin, (int)ptMax);
    sprintf(canvasName_resp_png, "%s/response%s_%s_ptPhot_%d_%d.png", get_outputdir().c_str(), rawPostfix.c_str(), etaRegion.c_str(), (int)ptMin, (int)ptMax);

    if (OUTPUT_GRAPHS || ptMin == 150) {
      c1_resp->SaveAs(canvasName_resp_eps);
      c1_resp->SaveAs(canvasName_resp_png);
    }

    delete legend_resp;

    // MPF Extrap
    c1_resp->Clear();

    c1_resp->cd();
    h2_axes_resp->Draw();

    if (etaRegion_str != "")
      legend_resp = new TLegend(legbox.xMin, legbox.yMin, legbox.xMax, legbox.yMax, etaRegion_str.c_str());
    else
      legend_resp = new TLegend(legbox.xMin, legbox.yMin, legbox.xMax, legbox.yMax);
    legend_resp->SetTextSize(0.035);
    legend_resp->SetTextFont(42);
    //legend_resp->SetFillStyle(0);
    legend_resp->SetFillColor(kWhite);
    legend_resp->AddEntry(gr_resp_genMPF, "MPF Gen", "P");
    legend_resp->AddEntry(gr_resp_MPFDATA, "DATA (MPF #gamma + jet)", "P");
    legend_resp->AddEntry(gr_resp_MPF, "MC (MPF #gamma + jet)", "P");

    //    total_mpf->Draw("same");
    gr_resp_MPF->Draw("Psame");
    gr_resp_genMPF->Draw("Psame");
    //    gr_resp_photGenReco->Draw("Psame");
    //    gr_resp_metRecoGen->Draw("Psame");
    //    gr_resp_cosRecoGen->Draw("Psame");

    legend_resp->Draw("same");
    label_resp->Draw("same");
    label_cms->Draw("same");
    label_sqrt->Draw("same");
    //label_CMStop->Draw("same");
    label_algo->Draw("same");
    gr_resp_MPFDATA->Draw("Psame");

    // Fit parameters
    TPaveText* label_fit = new TPaveText(0.45, 0.40, 0.4, 0.34, "blNDC");
    label_fit->SetFillColor(kWhite);
    label_fit->SetTextSize(0.030);

    TString line1 = TString::Format("MC: (%.3lf #pm %.3lf)  + (%.3lf #pm %.3lf)x", fit_respParabolaMPF->GetParameter(0), fit_respParabolaMPF->GetParError(0), fit_respParabolaMPF->GetParameter(1), fit_respParabolaMPF->GetParError(1));
    TString line2 = TString::Format("Data: (%.3lf #pm %.3lf)  + (%.3lf #pm %.3lf)x", fit_respParabola_MPFDATA->GetParameter(0), fit_respParabola_MPFDATA->GetParError(0), fit_respParabola_MPFDATA->GetParameter(1), fit_respParabola_MPFDATA->GetParError(1));

    label_fit->SetTextAlign(11);
    label_fit->AddText(line1);
    label_fit->AddText(line2);

    label_fit->Draw("same");

    std::string MPF = "MPF";

    sprintf(canvasName_resp_eps, "%s/response%s%s_%s_ptPhot_%d_%d.eps", get_outputdir().c_str(), MPF.c_str(), rawPostfix.c_str(), etaRegion.c_str(), (int)ptMin, (int)ptMax);
    sprintf(canvasName_resp_png, "%s/response%s%s_%s_ptPhot_%d_%d.png", get_outputdir().c_str(), MPF.c_str(), rawPostfix.c_str(), etaRegion.c_str(), (int)ptMin, (int)ptMax);

    if (OUTPUT_GRAPHS || ptMin == 150) {
      c1_resp->SaveAs(canvasName_resp_eps);
      c1_resp->SaveAs(canvasName_resp_png);
    }

    /* else if (recoGen != "RecoRelRaw") {
    // MPF Extrap Type I
    c1_resp->Clear();

    c1_resp->cd();
    h2_axes_resp->Draw();

    if (etaRegion_str != "")
    legend_resp = new TLegend(legbox.xMin, legbox.yMin, legbox.xMax, legbox.yMax, etaRegion_str.c_str());
    else
    legend_resp = new TLegend(legbox.xMin, legbox.yMin, legbox.xMax, legbox.yMax);
    legend_resp->SetTextSize(0.035);
    legend_resp->SetTextFont(42);
    //legend_resp->SetFillStyle(0);
    legend_resp->SetFillColor(kWhite);
    legend_resp->AddEntry(gr_resp_genMPF, "MPF Gen", "P");
    legend_resp->AddEntry(gr_resp_MPFDATA, "DATA (MPF #gamma + jet)", "P");
    legend_resp->AddEntry(gr_resp_MPF, "MC (MPF #gamma + jet)", "P");

    //    total_mpf->Draw("same");
    //    gr_resp_photGenReco->Draw("Psame");
    //    gr_resp_metRecoGen->Draw("Psame");
    //    gr_resp_cosRecoGen->Draw("Psame");

    legend_resp->Draw("same");
    label_resp->Draw("same");
    label_cms->Draw("same");
    label_sqrt->Draw("same");
    //label_CMStop->Draw("same");
    label_algo->Draw("same");
    gr_resp_genMPF->Draw("Psame");
    gr_resp_MPF_TypeICor->Draw("Psame");
    gr_resp_MPFDATA_TypeICor->Draw("Psame");

    // Fit parameters
    TPaveText* label_fit = new TPaveText(0.45, 0.40, 0.4, 0.34, "blNDC");
    label_fit->SetFillColor(kWhite);
    label_fit->SetTextSize(0.030);

    TString line1 = TString::Format("MC: (%.3lf #pm %.3lf)  + (%.3lf #pm %.3lf)x", fit_resp_MPF_TypeICor->GetParameter(0), fit_resp_MPF_TypeICor->GetParError(0), fit_resp_MPF_TypeICor->GetParameter(1), fit_resp_MPF_TypeICor->GetParError(1));
    TString line2 = TString::Format("Data: (%.3lf #pm %.3lf)  + (%.3lf #pm %.3lf)x", fit_resp_MPFDATA_TypeICor->GetParameter(0), fit_resp_MPFDATA_TypeICor->GetParError(0), fit_resp_MPFDATA_TypeICor->GetParameter(1), fit_resp_MPFDATA_TypeICor->GetParError(1));

    label_fit->SetTextAlign(11);
    label_fit->AddText(line1);
    label_fit->AddText(line2);

    label_fit->Draw("same");

    std::string MPF = "MPF";

    if (etaRegion != "") {
    sprintf(canvasName_resp_eps, "%s/responseMPF_TypeICor_%s_ptPhot_%d_%d_%s.eps", get_outputdir().c_str(), etaRegion.c_str(), (int)ptMin, (int)ptMax, recoGen.c_str());
    sprintf(canvasName_resp_png, "%s/responseMPF_TypeICor_%s_ptPhot_%d_%d_%s.png", get_outputdir().c_str(), etaRegion.c_str(), (int)ptMin, (int)ptMax, recoGen.c_str());
    } else {
    sprintf(canvasName_resp_eps, "%s/responseMPF_TypeICor_ptPhot_%d_%d_%s.eps", get_outputdir().c_str(), (int)ptMin, (int)ptMax, recoGen.c_str());
    sprintf(canvasName_resp_png, "%s/responseMPF_TypeICor_ptPhot_%d_%d_%s.png", get_outputdir().c_str(), (int)ptMin, (int)ptMax, recoGen.c_str());
    }

    if (OUTPUT_GRAPHS || ptMin == 150) {
    c1_resp->SaveAs(canvasName_resp_eps);
    c1_resp->SaveAs(canvasName_resp_png);
    }

    // MPF Extrap Type I+II
    c1_resp->Clear();

    c1_resp->cd();
    h2_axes_resp->Draw();

    if (etaRegion_str != "")
    legend_resp = new TLegend(legbox.xMin, legbox.yMin, legbox.xMax, legbox.yMax, etaRegion_str.c_str());
    else
      legend_resp = new TLegend(legbox.xMin, legbox.yMin, legbox.xMax, legbox.yMax);
    legend_resp->SetTextSize(0.035);
    legend_resp->SetTextFont(42);
    //legend_resp->SetFillStyle(0);
    legend_resp->SetFillColor(kWhite);
    legend_resp->AddEntry(gr_resp_genMPF, "MPF Gen", "P");
    legend_resp->AddEntry(gr_resp_MPFDATA, "DATA (MPF #gamma + jet)", "P");
    legend_resp->AddEntry(gr_resp_MPF, "MC (MPF #gamma + jet)", "P");

    //    total_mpf->Draw("same");
    //    gr_resp_photGenReco->Draw("Psame");
    //    gr_resp_metRecoGen->Draw("Psame");
    //    gr_resp_cosRecoGen->Draw("Psame");

    legend_resp->Draw("same");
    label_resp->Draw("same");
    label_cms->Draw("same");
    label_sqrt->Draw("same");
    //label_CMStop->Draw("same");
    label_algo->Draw("same");
    gr_resp_genMPF->Draw("Psame");
    gr_resp_MPF_TypeIpIICor->Draw("Psame");
    gr_resp_MPFDATA_TypeIpIICor->Draw("Psame");

    // Fit parameters
    delete label_fit;
    label_fit = new TPaveText(0.45, 0.40, 0.4, 0.34, "blNDC");
    label_fit->SetFillColor(kWhite);
    label_fit->SetTextSize(0.030);

    line1 = TString::Format("MC: (%.3lf #pm %.3lf)  + (%.3lf #pm %.3lf)x", fit_resp_MPF_TypeIpIICor->GetParameter(0), fit_resp_MPF_TypeIpIICor->GetParError(0), fit_resp_MPF_TypeIpIICor->GetParameter(1), fit_resp_MPF_TypeIpIICor->GetParError(1));
    line2 = TString::Format("Data: (%.3lf #pm %.3lf)  + (%.3lf #pm %.3lf)x", fit_resp_MPFDATA_TypeIpIICor->GetParameter(0), fit_resp_MPFDATA_TypeIpIICor->GetParError(0), fit_resp_MPFDATA_TypeIpIICor->GetParameter(1), fit_resp_MPFDATA_TypeIpIICor->GetParError(1));

    label_fit->SetTextAlign(11);
    label_fit->AddText(line1);
    label_fit->AddText(line2);

    label_fit->Draw("same");

    if (etaRegion != "") {
      sprintf(canvasName_resp_eps, "%s/responseMPF_TypeIpIICor_%s_ptPhot_%d_%d_%s.eps", get_outputdir().c_str(), etaRegion.c_str(), (int)ptMin, (int)ptMax, recoGen.c_str());
      sprintf(canvasName_resp_png, "%s/responseMPF_TypeIpIICor_%s_ptPhot_%d_%d_%s.png", get_outputdir().c_str(), etaRegion.c_str(), (int)ptMin, (int)ptMax, recoGen.c_str());
    } else {
      sprintf(canvasName_resp_eps, "%s/responseMPF_TypeIpIICor_ptPhot_%d_%d_%s.eps", get_outputdir().c_str(), (int)ptMin, (int)ptMax, recoGen.c_str());
      sprintf(canvasName_resp_png, "%s/responseMPF_TypeIpIICor_ptPhot_%d_%d_%s.png", get_outputdir().c_str(), (int)ptMin, (int)ptMax, recoGen.c_str());
    }

    if (OUTPUT_GRAPHS || ptMin == 150) {
      c1_resp->SaveAs(canvasName_resp_eps);
      c1_resp->SaveAs(canvasName_resp_png);
    }
  }*/

  // TEST //
  // Compute Data/MC ratio and do the extrapolation AFTER

  TGraphErrors* gr_mcRatio = this->get_graphRatio(gr_resp_DATA, gr_resp_recoPhot);
  gr_mcRatio->SetName("gr_mcRatioResp");
  gr_mcRatio->SetMarkerStyle(20);
  gr_mcRatio->SetMarkerColor(recoPhot_color);
  gr_mcRatio->SetMarkerSize(markerSize);

  TF1* fit_mcRatio_Parabola = new TF1("fit_mcRatio_Parabola", fitFunct_name.c_str());
  fit_respParabola_DATA->SetRange(0., xMax_fit);
  /*if( NOQ_ ) { //to evaluate syst
    float delta_q_resp = fabs( (1.-q_resp)/2. );
    if( q_resp>1. ) fit_respParabola_DATA->FixParameter(1, 1.+delta_q_resp);
    else            fit_respParabola_DATA->FixParameter(1, 1.-delta_q_resp);
    } else {*/
  fit_mcRatio_Parabola->FixParameter(1, 1);
  //}
  if (FIXM_)
    fit_mcRatio_Parabola->FixParameter(2, 1);
  fit_mcRatio_Parabola->SetLineColor(recoPhot_color);
  fit_mcRatio_Parabola->SetLineWidth(1.);
  gr_mcRatio->Fit(fit_mcRatio_Parabola, "RQ");

  gr_mcRatioResp_vs_pt->SetPoint(iPtBin, ptPhotReco_thisBin, fit_mcRatio_Parabola->GetParameter(0));

  c1_resp->Clear();
  c1_resp->cd();
  h2_axes_resp->SetYTitle("DATA/MC ratio");
  h2_axes_resp->Draw();

  if (etaRegion_str != "")
    legend_resp = new TLegend(legbox.xMin, legbox.yMin, legbox.xMax, legbox.yMax, etaRegion_str.c_str());
  else
    legend_resp = new TLegend(legbox.xMin, legbox.yMin, legbox.xMax, legbox.yMax);
  legend_resp->SetTextSize(0.035);
  legend_resp->SetTextFont(42);
  //legend_resp->SetFillStyle(0);
  legend_resp->SetFillColor(kWhite);
  legend_resp->AddEntry(gr_mcRatio, "DATA/MC ratio", "P");

  legend_resp->Draw("same");
  label_resp->Draw("same");
  label_cms->Draw("same");
  label_sqrt->Draw("same");
  label_algo->Draw("same");

  gr_mcRatio->Draw("Psame");

  if (OUTPUT_GRAPHS || ptMin == 150) {
    sprintf(canvasName_resp_eps, "%s/dataMC_ratio_%s_%s_ptPhot_%d_%d.pdf", get_outputdir().c_str(), rawPostfix.c_str(), etaRegion.c_str(), (int)ptMin, (int)ptMax);
    c1_resp->SaveAs(canvasName_resp_eps);
  }

  delete gr_mcRatio;
  delete fit_mcRatio_Parabola;
  delete legend_resp;

  delete c1_resp;
  delete h2_axes_resp;

  //draw resolution histograms:

  TGraphErrors* gr_reso_DATA = new TGraphErrors(nPoints, x, y_reso_DATA, x_err, y_reso_err_DATA);
  gr_reso_DATA->SetMarkerStyle(20);
  gr_reso_DATA->SetMarkerColor(recoPhot_color);
  // take out points with reso=0:
  for (int iPointDATA = 0; iPointDATA < gr_reso_DATA->GetN(); ++iPointDATA) {
    Double_t x, y;
    gr_reso_DATA->GetPoint(iPointDATA, x, y);
    Double_t yerr = gr_reso_DATA->GetErrorY(iPointDATA);
    if (y < 0.00000001 || yerr == 0.00000000001) gr_reso_DATA->RemovePoint(iPointDATA);
  }
  gr_reso_DATA->SetLineColor(recoPhot_color);
  gr_reso_DATA->SetLineWidth(1.);
  gr_reso_DATA->ResetAttLine();


  TGraphErrors* gr_reso_recoPhot = new TGraphErrors(nPoints, x, y_reso_recoPhot, x_err, y_reso_recoPhot_err);
  gr_reso_recoPhot->SetMarkerStyle(24);
  gr_reso_recoPhot->SetMarkerColor(recoPhot_color);
  gr_reso_recoPhot->SetLineColor(recoPhot_color);
  gr_reso_recoPhot->SetLineStyle(2);
  gr_reso_recoPhot->SetLineWidth(1.);

  TGraphErrors* gr_reso_genPhot = new TGraphErrors(nPoints, x, y_reso_genPhot, x_err, y_reso_genPhot_err);
  gr_reso_genPhot->SetMarkerStyle(22);
  //gr_reso_genPhot->SetMarkerColor(kGreen+3);
  gr_reso_genPhot->SetMarkerColor(genPhot_color);

  TGraphErrors* gr_reso_recoGen = new TGraphErrors(nPoints, x, y_reso_recoGen, x_err, y_reso_recoGen_err);
  gr_reso_recoGen->SetMarkerStyle(21);
  gr_reso_recoGen->SetMarkerColor(recoGen_color);

  /*TGraphErrors* gr_reso_genPart = new TGraphErrors(nPoints, x, y_reso_genPart, x_err, y_reso_genPart_err);
    gr_reso_genPart->SetMarkerStyle(21);
    gr_reso_genPart->SetMarkerColor(kGreen);*/

  TGraphErrors* gr_reso_genGamma = new TGraphErrors(nPoints, x, y_reso_genGamma, x_err, y_reso_genGamma_err);
  gr_reso_genGamma->SetMarkerStyle(21);
  gr_reso_genGamma->SetMarkerColor(30);

  /*TGraphErrors* gr_reso_partGamma = new TGraphErrors(nPoints, x, y_reso_partGamma, x_err, y_reso_partGamma_err);
    gr_reso_partGamma->SetMarkerStyle(21);
    gr_reso_partGamma->SetMarkerColor(kYellow);*/

  TGraphErrors* gr_reso_photGamma = new TGraphErrors(nPoints, x, y_reso_photGamma, x_err, y_reso_photGamma_err);
  gr_reso_photGamma->SetMarkerStyle(21);
  gr_reso_photGamma->SetMarkerColor(kGray);

  TGraphErrors* gr_reso_MPFDATA = new TGraphErrors(nPoints, x, y_reso_MPFDATA, x_err, y_reso_err_MPFDATA);
  gr_reso_MPFDATA->SetMarkerStyle(20);
  gr_reso_MPFDATA->SetMarkerColor(recoPhot_color);

  TGraphErrors* gr_reso_MPF = new TGraphErrors(nPoints, x, y_reso_MPF, x_err, y_reso_MPF_err);
  gr_reso_MPF->SetMarkerStyle(24);
  gr_reso_MPF->SetMarkerColor(recoPhot_color);

  // Type I + II Corrected MPF
  /*TGraphErrors* gr_reso_MPFDATA_TypeICor = new TGraphErrors(nPoints, xDATA, y_reso_MPFDATA_TypeICor, x_errDATA, y_reso_MPFDATA_TypeICor_err);
    gr_reso_MPFDATA_TypeICor->SetMarkerStyle(20);
    gr_reso_MPFDATA_TypeICor->SetMarkerColor(recoPhot_color);

    TGraphErrors* gr_reso_MPF_TypeICor = new TGraphErrors(nPoints, x, y_reso_MPF_TypeICor, x_err, y_reso_MPF_TypeICor_err);
    gr_reso_MPF_TypeICor->SetMarkerStyle(24);
    gr_reso_MPF_TypeICor->SetMarkerColor(recoPhot_color);

    TGraphErrors* gr_reso_MPFDATA_TypeIpIICor = new TGraphErrors(nPoints, xDATA, y_reso_MPFDATA_TypeIpIICor, x_errDATA, y_reso_MPFDATA_TypeIpIICor_err);
    gr_reso_MPFDATA_TypeIpIICor->SetMarkerStyle(20);
    gr_reso_MPFDATA_TypeIpIICor->SetMarkerColor(recoPhot_color);

    TGraphErrors* gr_reso_MPF_TypeIpIICor = new TGraphErrors(nPoints, x, y_reso_MPF_TypeIpIICor, x_err, y_reso_MPF_TypeIpIICor_err);
    gr_reso_MPF_TypeIpIICor->SetMarkerStyle(24);
    gr_reso_MPF_TypeIpIICor->SetMarkerColor(recoPhot_color);*/

  TGraphErrors* gr_reso_genMPF = new TGraphErrors(nPoints, x, y_reso_genMPF, x_err, y_reso_genMPF_err);
  gr_reso_genMPF->SetMarkerStyle(22);
  //gr_reso_genPhot->SetMarkerColor(kGreen+3);
  gr_reso_genMPF->SetMarkerColor(genPhot_color);

  Double_t x1, x2, y1, y2;

  TF1* fit_reso_genPhot = new TF1("fit_reso_genPhot", "[0] + x*[1]");
  fit_reso_genPhot->SetRange(0., xMax_fit);
  fit_reso_genPhot->SetLineWidth(0.5);
  //fit_reso_genPhot->SetLineColor(kGreen+3);
  fit_reso_genPhot->SetLineColor(genPhot_color);
  gr_reso_genPhot->Fit(fit_reso_genPhot, "RQ");

  /*TF1* fit_reso_genPart = new TF1("fit_reso_genPart", "[0] + x*[1]");
    fit_reso_genPart->SetRange(0., xMax_fit);
    fit_reso_genPart->SetLineWidth(0.5);
    fit_reso_genPart->SetLineColor(kGreen);
    gr_reso_genPart->Fit(fit_reso_genPart, "RQ");*/

  TF1* fit_reso_genGamma = new TF1("fit_reso_genGamma", "[0] + x*[1]");
  fit_reso_genGamma->SetRange(0., xMax_fit);
  fit_reso_genGamma->SetLineWidth(0.5);
  fit_reso_genGamma->SetLineColor(kGreen);
  gr_reso_genGamma->Fit(fit_reso_genGamma, "RQ");

  /*TF1* fit_reso_partGamma = new TF1("fit_reso_partGamma", "[0] + x*[1]");
    fit_reso_partGamma->SetRange(0., xMax_fit);
    fit_reso_partGamma->SetLineWidth(0.5);
    fit_reso_partGamma->SetLineColor(kYellow);
    gr_reso_partGamma->Fit(fit_reso_partGamma, "RQ");*/

  TF1* fit_reso_photGamma = new TF1("fit_reso_photGamma", "[0]");
  fit_reso_photGamma->SetRange(0., xMax_fit);
  fit_reso_photGamma->SetLineWidth(0.5);
  fit_reso_photGamma->SetLineColor(kGray);
  gr_reso_photGamma->Fit(fit_reso_photGamma, "RQ");


  TF1* fit_reso_recoGen = new TF1("fit_reso_recoGen", "[0]");
  fit_reso_recoGen->SetRange(0., xMax_fit);
  gr_reso_recoGen->GetPoint(1, x1, y1);
  gr_reso_recoGen->GetPoint(2, x2, y2);
  fit_reso_recoGen->SetParameter(0, y1);
  fit_reso_recoGen->SetLineWidth(0.5);
  fit_reso_recoGen->SetLineColor(recoGen_color);
  gr_reso_recoGen->Fit(fit_reso_recoGen, "RQ");

  std::string sum_str;
  sum_str = "sqrt( fit_reso_recoGen*fit_reso_recoGen + fit_reso_genPhot*fit_reso_genPhot )";

  TF1* sum = new TF1("sum", sum_str.c_str());
  sum->SetRange(0., xMax_fit);
  sum->SetLineColor(kGray + 2);

  Double_t x1_reco, y1_reco;
  gr_reso_recoPhot->GetPoint(1, x1_reco, y1_reco);
  Double_t x2_reco, y2_reco;
  gr_reso_recoPhot->GetPoint(2, x2_reco, y2_reco);

  // genPhot: y = mx + q
  // recoGen: y = c
  Float_t c = fit_reso_recoGen->GetParameter(0);
  Float_t q = fit_reso_genPhot->GetParameter(0);
  Float_t qerr = fit_reso_genPhot->GetParError(0);
  Float_t m = fit_reso_genPhot->GetParameter(1);
  // [0] = c; [1] = q; [2] = m
  TF1* fit_extrapToZero_sqrt = new TF1("fit_extrapToZero_sqrt", "sqrt([0]*[0] + [1]*[1] + 2.*[1]*[2]*x + [2]*[2]*x*x)");
  fit_extrapToZero_sqrt->SetRange(0., xMax_fit);
  fit_extrapToZero_sqrt->SetParameter(0, c);
  fit_extrapToZero_sqrt->SetParLimits(0, 0.01, 0.3);
  if (NOQ_)
    fit_extrapToZero_sqrt->FixParameter(1, 0.5 * q); //to evaluate syst
  else
    fit_extrapToZero_sqrt->FixParameter(1, q); //fixed
  if (FIXM_) {
    fit_extrapToZero_sqrt->FixParameter(2, m);
  } else {
    fit_extrapToZero_sqrt->SetParameter(2, m);
    //fit_extrapToZero_sqrt->SetParLimits(2, 0., 0.05);
    fit_extrapToZero_sqrt->SetParLimits(2, 0., 0.05 * 100.);
  }
  fit_extrapToZero_sqrt->SetLineStyle(2);
  fit_extrapToZero_sqrt->SetLineColor(recoPhot_color);
  fit_extrapToZero_sqrt->SetLineWidth(1.);

  TF1* fit_extrapToZero_sqrt_DATA = new TF1(*fit_extrapToZero_sqrt);
  fit_extrapToZero_sqrt_DATA->SetName("fit_extrapToZero_sqrt_DATA");
  if (FIXM_)
    fit_extrapToZero_sqrt_DATA->FixParameter(2, m);
  //fit_extrapToZero_sqrt_DATA->FixParameter( 2, fit_extrapToZero_sqrt->GetParameter(2) );
  fit_extrapToZero_sqrt_DATA->SetLineStyle(1);
  fit_extrapToZero_sqrt_DATA->SetLineWidth(1.);

  TF1* fit_extrapToZero_line = new TF1("fit_extrapToZero_line", "[0]+[1]*x");
  fit_extrapToZero_line->SetRange(0., xMax_fit);
  fit_extrapToZero_line->SetParameter(0, y1_reco);
  fit_extrapToZero_line->SetParameter(1, (y2_reco - y1_reco) / (x2_reco - x1_reco));
  fit_extrapToZero_line->SetParLimits(0, 0., 0.4);
  fit_extrapToZero_line->SetLineColor(recoPhot_color);
  fit_extrapToZero_line->SetLineWidth(0.5);

  gr_reso_recoPhot->Fit(fit_extrapToZero_sqrt, "RQ");

  if (EXCLUDE_FIRST_POINT_) {
    gr_reso_DATA->RemovePoint(0);
  }
  gr_reso_DATA->Fit(fit_extrapToZero_sqrt_DATA, "RQ");

  gr_DATAReso_vs_pt->SetPoint(iPtBin, ptPhotReco_thisBin,  fit_extrapToZero_sqrt_DATA->GetParameter(0));
  gr_DATAReso_vs_pt->SetPointError(iPtBin, ptPhotReco_err_thisBin, fit_extrapToZero_sqrt_DATA->GetParError(0));

  gr_extrapReso_vs_pt->SetPoint(iPtBin, ptPhotReco_thisBin, fit_extrapToZero_sqrt->GetParameter(0));
  gr_extrapReso_vs_pt->SetPointError(iPtBin, ptPhotReco_err_thisBin, fit_extrapToZero_sqrt->GetParError(0));
  //extrapReso = sqrt(fit_extrapToZero_sqrt->GetParameter(0));
  //extrapReso_err = ( 1. / (2.*sqrt(extrapReso)) * fit_extrapToZero_sqrt->GetParError(0)); //error propag

  gr_intrReso_vs_pt->SetPoint(iPtBin, ptPhotReco_thisBin, fit_reso_recoGen->GetParameter(0));
  gr_intrReso_vs_pt->SetPointError(iPtBin, ptPhotReco_err_thisBin, fit_reso_recoGen->GetParError(0));

  /*gr_qGenPartReso_vs_pt->SetPoint(iPtBin, ptPhotReco_thisBin, fit_reso_genPart->GetParameter(0));
    gr_qGenPartReso_vs_pt->SetPointError(iPtBin, ptPhotReco_err_thisBin, fit_reso_genPart->GetParError(0));*/

  gr_qGenGammaReso_vs_pt->SetPoint(iPtBin, ptPhotReco_thisBin, fit_reso_genGamma->GetParameter(0));
  gr_qGenGammaReso_vs_pt->SetPointError(iPtBin, ptPhotReco_err_thisBin, fit_reso_genGamma->GetParError(0));

  /*gr_qPartGammaReso_vs_pt->SetPoint(iPtBin, ptPhotReco_thisBin, fit_reso_partGamma->GetParameter(0));
    gr_qPartGammaReso_vs_pt->SetPointError(iPtBin, ptPhotReco_err_thisBin, fit_reso_partGamma->GetParError(0));*/

  gr_qPhotGammaReso_vs_pt->SetPoint(iPtBin, ptPhotReco_thisBin, fit_reso_photGamma->GetParameter(0));
  gr_qPhotGammaReso_vs_pt->SetPointError(iPtBin, ptPhotReco_err_thisBin, fit_reso_photGamma->GetParError(0));

  gr_qReso_vs_pt->SetPoint(iPtBin, ptPhotReco_thisBin, q);
  gr_qReso_vs_pt->SetPointError(iPtBin, ptPhotReco_err_thisBin, qerr);

  // "ratio" method:
  TGraphErrors* gr_reso_subtr = new TGraphErrors(0);
  gr_reso_subtr->SetName("reso_subtr");
  gr_reso_subtr->SetMarkerStyle(21);
  gr_reso_subtr->SetMarkerColor(kGray);
  for (int iP = 0; iP < gr_reso_recoPhot->GetN(); ++iP) {
    Double_t xP, yP;
    gr_reso_recoPhot->GetPoint(iP, xP, yP);
    Double_t yP_err = gr_reso_recoPhot->GetErrorY(iP);
    Double_t xMC, yMC;
    gr_reso_genPhot->GetPoint(iP, xMC, yMC);
    Double_t yMC_err = gr_reso_genPhot->GetErrorY(iP);
    //float reso_subtr = sqrt( yP*yP - fit_reso_genPhot->Eval(xP)*fit_reso_genPhot->Eval(xP) );
    float reso_subtr = sqrt(yP * yP - yMC * yMC);
    float reso_subtr_err = sqrt(yP_err * yP_err + yMC_err * yMC_err);
    gr_reso_subtr->SetPoint(iP, xP, reso_subtr);
    gr_reso_subtr->SetPointError(iP, 0., reso_subtr_err);
  }
  TF1* fit_reso_subtr = new TF1("fit_reso_subtr", "[0]");
  fit_reso_subtr->SetRange(0., xMax_fit);
  fit_reso_subtr->SetLineWidth(0.5);
  fit_reso_subtr->SetLineColor(kGray);
  gr_reso_subtr->Fit(fit_reso_subtr, "RQ");

  gr_reso_subtr_vs_pt->SetPoint(iPtBin, ptPhotReco_thisBin, fit_reso_subtr->GetParameter(0));
  gr_reso_subtr_vs_pt->SetPointError(iPtBin, ptPhotReco_err_thisBin, fit_reso_subtr->GetParError(0));

  TGraphErrors* gr_reso_DATA_subtr = new TGraphErrors(0);
  gr_reso_DATA_subtr->SetName("reso_DATA_subtr");
  gr_reso_DATA_subtr->SetMarkerStyle(21);
  gr_reso_DATA_subtr->SetMarkerColor(kGray);
  for (int iP = 0; iP < gr_reso_DATA->GetN(); ++iP) {
    Double_t xP, yP;
    gr_reso_DATA->GetPoint(iP, xP, yP);
    Double_t yP_err = gr_reso_DATA->GetErrorY(iP);
    Double_t xMC, yMC;
    gr_reso_genPhot->GetPoint(iP, xMC, yMC);
    Double_t yMC_err = gr_reso_genPhot->GetErrorY(iP);
    //float reso_subtr = sqrt( yP*yP - fit_reso_genPhot->Eval(xP)*fit_reso_genPhot->Eval(xP) );
    float reso_subtr = sqrt(yP * yP - yMC * yMC);
    float reso_subtr_err = sqrt(yP_err * yP_err + yMC_err * yMC_err);
    gr_reso_DATA_subtr->SetPoint(iP, xP, reso_subtr);
    gr_reso_DATA_subtr->SetPointError(iP, 0., reso_subtr_err);
  }
  TF1* fit_reso_DATA_subtr = new TF1("fit_reso_DATA_subtr", "[0]");
  fit_reso_DATA_subtr->SetRange(0., xMax_fit);
  fit_reso_DATA_subtr->SetLineWidth(0.5);
  fit_reso_DATA_subtr->SetLineColor(kGray);
  gr_reso_DATA_subtr->Fit(fit_reso_DATA_subtr, "RQ");

  gr_DATAReso_subtr_vs_pt->SetPoint(iPtBin, ptPhotReco_thisBin, fit_reso_DATA_subtr->GetParameter(0));
  gr_DATAReso_subtr_vs_pt->SetPointError(iPtBin, ptPhotReco_err_thisBin, fit_reso_DATA_subtr->GetParError(0));

  TGraphErrors* gr_reso_ratio = fitTools::get_graphRatio(gr_reso_DATA_subtr, gr_reso_subtr);
  TF1* fit_reso_ratio = new TF1("fit_reso_ratio", "[0]");
  fit_reso_ratio->SetRange(0., xMax_fit);
  fit_reso_ratio->SetLineWidth(0.5);
  fit_reso_ratio->SetLineColor(kGray);
  gr_reso_ratio->Fit(fit_reso_ratio, "RQ");

  gr_reso_ratio_vs_pt->SetPoint(iPtBin, ptPhotReco_thisBin, fit_reso_ratio->GetParameter(0));
  gr_reso_ratio_vs_pt->SetPointError(iPtBin, ptPhotReco_err_thisBin, fit_reso_ratio->GetParError(0));

  // MPF
  TF1* fit_reso_genMPF = new TF1("fit_reso_genMPF", "[0] + [1]*x", 0, xMax_fit);
  fit_reso_genMPF->SetLineColor(kGreen);
  fit_reso_genMPF->SetLineWidth(1.);
  gr_reso_genMPF->Fit(fit_reso_genMPF, "QR");

  TF1* fit_reso_MPFDATA = NULL;
  TF1* fit_reso_MPF = NULL;

  /*TF1* fit_reso_MPFDATA_TypeICor = NULL, *fit_reso_MPFDATA_TypeIpIICor = NULL;
    TF1* fit_reso_MPF_TypeICor = NULL, *fit_reso_MPF_TypeIpIICor = NULL;*/

  //if (! corrected) {
  fit_reso_MPF = new TF1("fit_reso_MPF", "[0] + [1]*x", 0, xMax_fit);
  fit_reso_MPFDATA = new TF1("fit_reso_MPFDATA", "[0] + [1]*x", 0, xMax_fit);

  fit_reso_MPFDATA->SetLineColor(recoPhot_color);
  fit_reso_MPFDATA->SetLineWidth(1.);
  fit_reso_MPF->SetLineColor(recoPhot_color);
  fit_reso_MPF->SetLineWidth(1.);

  fit_reso_MPFDATA->SetParameter(0, fit_reso_genMPF->GetParameter(0));
  fit_reso_MPF->SetParameter(0, fit_reso_genMPF->GetParameter(0));
  fit_reso_MPFDATA->SetParameter(1, 0);
  fit_reso_MPF->SetParameter(1, 0);

  gr_reso_MPF->Fit(fit_reso_MPF, "QR");
  gr_reso_MPFDATA->Fit(fit_reso_MPFDATA, "QR");

  gr_DATAResoMPF_vs_pt->SetPoint(iPtBin, ptPhotReco_thisBin, fit_reso_MPFDATA->GetParameter(0));
  gr_DATAResoMPF_vs_pt->SetPointError(iPtBin, ptPhotReco_err_thisBin, fit_reso_MPFDATA->GetParError(0));

  gr_extrapResoMPF_vs_pt->SetPoint(iPtBin, ptPhotReco_thisBin, fit_reso_MPF->GetParameter(0));
  gr_extrapResoMPF_vs_pt->SetPointError(iPtBin, ptPhotReco_err_thisBin, fit_reso_MPF->GetParError(0));
  /*} else {
  // Type I MPF
  fit_reso_MPF_TypeICor = new TF1("fit_reso_MPF_TypeICor", "[0] + [1]*x", 0, xMax_fit);
  fit_reso_MPFDATA_TypeICor = new TF1("fit_reso_MPFDATA_TypeICor", "[0] + [1]*x", 0, xMax_fit);

  fit_reso_MPFDATA_TypeICor->SetLineColor(recoPhot_color);
  fit_reso_MPFDATA_TypeICor->SetLineWidth(1.);
  fit_reso_MPF_TypeICor->SetLineColor(recoPhot_color);
  fit_reso_MPF_TypeICor->SetLineWidth(1.);

  fit_reso_MPFDATA_TypeICor->SetParameter(0, fit_reso_genMPF->GetParameter(0));
  fit_reso_MPF_TypeICor->SetParameter(0, fit_reso_genMPF->GetParameter(0));
  fit_reso_MPFDATA_TypeICor->SetParameter(1, 0);
  fit_reso_MPF_TypeICor->SetParameter(1, 0);

  gr_reso_MPF_TypeICor->Fit(fit_reso_MPF_TypeICor, "QR");
  gr_reso_MPFDATA_TypeICor->Fit(fit_reso_MPFDATA_TypeICor, "QR");

  gr_DATAResoMPF_TypeICor_vs_pt->SetPoint(iPtBin, ptPhotReco_thisBinDATA, fit_reso_MPFDATA_TypeICor->GetParameter(0));
  gr_DATAResoMPF_TypeICor_vs_pt->SetPointError(iPtBin, ptPhotReco_err_thisBinDATA, fit_reso_MPFDATA_TypeICor->GetParError(0));

  gr_extrapResoMPF_TypeICor_vs_pt->SetPoint(iPtBin, ptPhotReco_thisBin, fit_reso_MPF_TypeICor->GetParameter(0));
  gr_extrapResoMPF_TypeICor_vs_pt->SetPointError(iPtBin, ptPhotReco_err_thisBin, fit_reso_MPF_TypeICor->GetParError(0));

  // Type II MPF
  fit_reso_MPF_TypeIpIICor = new TF1("fit_reso_MPF_TypeIpIICor", "[0] + [1]*x", 0, xMax_fit);
  fit_reso_MPFDATA_TypeIpIICor = new TF1("fit_reso_MPFDATA_TypeIpIICor", "[0] + [1]*x", 0, xMax_fit);

  fit_reso_MPFDATA_TypeIpIICor->SetLineColor(recoPhot_color);
  fit_reso_MPFDATA_TypeIpIICor->SetLineWidth(1.);
  fit_reso_MPF_TypeIpIICor->SetLineColor(recoPhot_color);
  fit_reso_MPF_TypeIpIICor->SetLineWidth(1.);

  fit_reso_MPFDATA_TypeIpIICor->SetParameter(0, fit_reso_genMPF->GetParameter(0));
  fit_reso_MPF_TypeIpIICor->SetParameter(0, fit_reso_genMPF->GetParameter(0));
  fit_reso_MPFDATA_TypeIpIICor->SetParameter(1, 0);
  fit_reso_MPF_TypeIpIICor->SetParameter(1, 0);

  gr_reso_MPF_TypeIpIICor->Fit(fit_reso_MPF_TypeIpIICor, "QR");
  gr_reso_MPFDATA_TypeIpIICor->Fit(fit_reso_MPFDATA_TypeIpIICor, "QR");

  gr_DATAResoMPF_TypeIpIICor_vs_pt->SetPoint(iPtBin, ptPhotReco_thisBinDATA, fit_reso_MPFDATA_TypeIpIICor->GetParameter(0));
  gr_DATAResoMPF_TypeIpIICor_vs_pt->SetPointError(iPtBin, ptPhotReco_err_thisBinDATA, fit_reso_MPFDATA_TypeIpIICor->GetParError(0));

  gr_extrapResoMPF_TypeIpIICor_vs_pt->SetPoint(iPtBin, ptPhotReco_thisBin, fit_reso_MPF_TypeIpIICor->GetParameter(0));
  gr_extrapResoMPF_TypeIpIICor_vs_pt->SetPointError(iPtBin, ptPhotReco_err_thisBin, fit_reso_MPF_TypeIpIICor->GetParError(0));
  }*/

  Float_t ymax;
  if (currentBin.first < 40.) ymax = (get_recoType() == "pf") ? 0.7 : 0.9;
  else if (currentBin.first < 80.) ymax = (get_recoType() == "pf") ? 0.6 : 0.8;
  else ymax = (get_recoType() == "pf") ? 0.4 : 0.6;

  TH2D* h2_axes_reso = new TH2D("axes_reso", "", 10, 0., xMax_axis, 10, 0., ymax);
  h2_axes_reso->SetXTitle(xTitle.c_str());
  if (! rawJets)
    h2_axes_reso->SetYTitle("Jet p_{T} Resolution");
  else
    h2_axes_reso->SetYTitle("Raw Jet p_{T} Resolution");


  Float_t minLegend = 0.2;
  TLegend* legend_reso;
  if (etaRegion_str != "")
    legend_reso = new TLegend(minLegend, 0.6, 0.55, 0.85, etaRegion_str.c_str());
  else
    legend_reso = new TLegend(minLegend, 0.6, 0.55, 0.85);
  legend_reso->SetTextSize(0.04);
  //legend_reso->SetFillStyle(0);
  legend_reso->SetFillColor(kWhite);
  legend_reso->AddEntry(gr_reso_recoGen, "MC Intrinsic", "P");
  legend_reso->AddEntry(gr_reso_genPhot, "MC Imbalance", "P");
  legend_reso->AddEntry(sum, "MC Intr #oplus Imb", "L");
  legend_reso->AddEntry(gr_reso_recoPhot, "MC (#gamma + jet)", "PL");
  legend_reso->AddEntry(gr_reso_DATA, "DATA (#gamma + jet)", "L");

  TPaveText* label_reso = new TPaveText(0.25, 0.85, 0.47, 0.9, "brNDC");
  label_reso->SetFillColor(kWhite);
  label_reso->SetTextSize(0.035);
  label_reso->AddText(labeltext);

  gr_reso_recoGen->SetMarkerSize(markerSize);
  gr_reso_genPhot->SetMarkerSize(markerSize);
  gr_reso_recoPhot->SetMarkerSize(markerSize);
  gr_reso_DATA->SetMarkerSize(markerSize);

  TCanvas* c1_reso = new TCanvas("c1_reso", "c1_reso", 600, 600);
  //c1_reso->SetLeftMargin(0.12);
  //c1_reso->SetBottomMargin(0.12);
  c1_reso->cd();
  h2_axes_reso->Draw();
  sum->Draw("same");
  gr_reso_recoGen->Draw("Psame");
  gr_reso_genPhot->Draw("Psame");
  legend_reso->Draw("same");
  //legend_resp->Draw("same");
  //label_reso->Draw("same");
  //label_resp->Draw("same");
  label_reso->Draw("same");
  label_cms->Draw("same");
  label_sqrt->Draw("same");
  //label_CMStop->Draw("same");
  label_algo->Draw("same");
  // save one propaganda plot with no data to explain method:
  if (ptMin == 47.) {
    char canvasName_reso_eps_NODATA[500];
    char canvasName_reso_png_NODATA[500];
    sprintf(canvasName_reso_eps_NODATA, "%s/resolution%s_%s_ptPhot_%d_%d_NODATANOMC.eps", get_outputdir().c_str(), rawPostfix.c_str(), etaRegion.c_str(), (int)ptMin, (int)ptMax);
    sprintf(canvasName_reso_png_NODATA, "%s/resolution%s_%s_ptPhot_%d_%d_NODATANOMC.png", get_outputdir().c_str(), rawPostfix.c_str(), etaRegion.c_str(), (int)ptMin, (int)ptMax);

    if (OUTPUT_GRAPHS) {
      c1_reso->SaveAs(canvasName_reso_eps_NODATA);
      c1_reso->SaveAs(canvasName_reso_png_NODATA);
    }
  }
  gr_reso_recoPhot->Draw("Psame");
  if (ptMin == 47.) {
    char canvasName_reso_eps_NODATA[500];
    char canvasName_reso_png_NODATA[500];
    sprintf(canvasName_reso_eps_NODATA, "%s/resolution%s_%s_ptPhot_%d_%d_NODATA.eps", get_outputdir().c_str(), rawPostfix.c_str(), etaRegion.c_str(), (int)ptMin, (int)ptMax);
    sprintf(canvasName_reso_png_NODATA, "%s/resolution%s_%s_ptPhot_%d_%d_NODATA.png", get_outputdir().c_str(), rawPostfix.c_str(), etaRegion.c_str(), (int)ptMin, (int)ptMax);

    if (OUTPUT_GRAPHS) {
      c1_reso->SaveAs(canvasName_reso_eps_NODATA);
      c1_reso->SaveAs(canvasName_reso_png_NODATA);
    }
  }
  gr_reso_DATA->Draw("Psame");


  char canvasName_reso[500];
  sprintf(canvasName_reso, "%s/resolution%s_%s_ptPhot_%d_%d", get_outputdir().c_str(), rawPostfix.c_str(), etaRegion.c_str(), (int)ptMin, (int)ptMax);
  std::string canvasName_reso_str(canvasName_reso);
  std::string canvasName_reso_eps = canvasName_reso_str + ".eps";
  std::string canvasName_reso_png = canvasName_reso_str + ".png";
  if (OUTPUT_GRAPHS || ptMin == 150) {
    c1_reso->SaveAs(canvasName_reso_eps.c_str());
    c1_reso->SaveAs(canvasName_reso_png.c_str());
  }

  //gr_reso_genPart->Draw("p same");
  //gr_reso_partGamma->Draw("p same");
  //gr_reso_genGamma->Draw("p same");
  //gr_reso_photGamma->Draw("p same");
  gr_reso_DATA_subtr->Draw("p same");

  std::string canvasName_reso_all = canvasName_reso_str + "_ALL";
  std::string canvasName_reso_all_eps = canvasName_reso_all + ".eps";
  std::string canvasName_reso_all_png = canvasName_reso_all + ".png";
  if (OUTPUT_GRAPHS) {
    c1_reso->SaveAs(canvasName_reso_all_eps.c_str());
    c1_reso->SaveAs(canvasName_reso_all_png.c_str());
  }

  delete legend_reso;
  delete c1_reso;

  // MPF Resolution
  if (etaRegion_str != "")
    legend_reso = new TLegend(minLegend, 0.6, 0.55, 0.85, etaRegion_str.c_str());
  else
    legend_reso = new TLegend(minLegend, 0.6, 0.55, 0.85);
  legend_reso->SetTextSize(0.04);
  //legend_reso->SetFillStyle(0);
  legend_reso->SetFillColor(kWhite);
  legend_reso->AddEntry(gr_reso_genMPF, "MPF Gen", "P");
  legend_reso->AddEntry(gr_reso_MPF, "MC (#gamma + jet MPF)", "PL");
  legend_reso->AddEntry(gr_reso_MPFDATA, "DATA (#gamma + jet MPF)", "L");

  delete label_reso;
  label_reso = new TPaveText(0.25, 0.85, 0.47, 0.9, "brNDC");
  label_reso->SetFillColor(kWhite);
  label_reso->SetTextSize(0.035);
  label_reso->AddText(labeltext);

  gr_reso_MPF->SetMarkerSize(markerSize);
  gr_reso_MPFDATA->SetMarkerSize(markerSize);
  gr_reso_genMPF->SetMarkerSize(markerSize);

  c1_reso = new TCanvas("c1_reso", "c1_reso", 600, 600);
  c1_reso->cd();
  h2_axes_reso->Draw();
  legend_reso->Draw("same");
  label_reso->Draw("same");
  label_cms->Draw("same");
  label_sqrt->Draw("same");
  label_algo->Draw("same");

  gr_reso_genMPF->Draw("Psame");
  gr_reso_MPF->Draw("Psame");
  gr_reso_MPFDATA->Draw("Psame");

  sprintf(canvasName_reso, "%s/resolutionMPF%s_%s_ptPhot_%d_%d", get_outputdir().c_str(), rawPostfix.c_str(), etaRegion.c_str(), (int)ptMin, (int)ptMax);
  
  canvasName_reso_str = canvasName_reso;
  canvasName_reso_eps = canvasName_reso_str + ".eps";
  canvasName_reso_png = canvasName_reso_str + ".png";
  if (OUTPUT_GRAPHS || ptMin == 150) {
    c1_reso->SaveAs(canvasName_reso_eps.c_str());
    c1_reso->SaveAs(canvasName_reso_png.c_str());
  }

  delete legend_reso;
  delete c1_reso;
  /* else if (recoGen != "RecoRelRaw") {
  // MPF Type I Resolution
  if (etaRegion_str != "")
  legend_reso = new TLegend(minLegend, 0.6, 0.55, 0.85, etaRegion_str.c_str());
  else
  legend_reso = new TLegend(minLegend, 0.6, 0.55, 0.85);
  legend_reso->SetTextSize(0.04);
  //legend_reso->SetFillStyle(0);
  legend_reso->SetFillColor(kWhite);
  legend_reso->AddEntry(gr_reso_genMPF, "MPF Gen", "P");
  legend_reso->AddEntry(gr_reso_MPF_TypeICor, "MC (#gamma + jet MPF Type I corr.)", "PL");
  legend_reso->AddEntry(gr_reso_MPFDATA_TypeICor, "DATA (#gamma + jet MPF Type I corr.)", "L");

  TPaveText* label_reso = new TPaveText(0.25, 0.85, 0.47, 0.9, "brNDC");
  label_reso->SetFillColor(kWhite);
  label_reso->SetTextSize(0.035);
  label_reso->AddText(labeltext);

  gr_reso_MPF->SetMarkerSize(markerSize);
  gr_reso_MPFDATA->SetMarkerSize(markerSize);
  gr_reso_genMPF->SetMarkerSize(markerSize);

  c1_reso = new TCanvas("c1_reso", "c1_reso", 600, 600);
  c1_reso->cd();
  h2_axes_reso->Draw();
  legend_reso->Draw("same");
  label_reso->Draw("same");
  label_cms->Draw("same");
  label_sqrt->Draw("same");
  label_algo->Draw("same");

  gr_reso_genMPF->Draw("Psame");
  gr_reso_MPF_TypeICor->Draw("Psame");
  gr_reso_MPFDATA_TypeICor->Draw("Psame");

  char canvasName_reso[500];
  if (etaRegion != "") {
  sprintf(canvasName_reso, "%s/resolution_%s_MPF_TypeICor_%s_ptPhot_%d_%d_%s", get_outputdir().c_str(), L2L3_text.c_str(), etaRegion.c_str(), (int)ptMin, (int)ptMax, recoGen.c_str());
  } else {
  sprintf(canvasName_reso, "%s/resolution_%s_MPF_TypeICor_ptPhot_%d_%d_%s", get_outputdir().c_str(), L2L3_text.c_str(), (int)ptMin, (int)ptMax, recoGen.c_str());
  }
  std::string canvasName_reso_str(canvasName_reso);
  std::string canvasName_reso_eps = canvasName_reso_str + ".eps";
  std::string canvasName_reso_png = canvasName_reso_str + ".png";
  if (OUTPUT_GRAPHS || ptMin == 150) {
  c1_reso->SaveAs(canvasName_reso_eps.c_str());
  c1_reso->SaveAs(canvasName_reso_png.c_str());
  }

  delete legend_reso;
  delete c1_reso;
  }*/


  // Compute Data/MC ratio and do the extrapolation AFTER

  TGraphErrors* gr_mcRatioReso = this->get_graphRatio(gr_reso_DATA, gr_reso_recoPhot);
  gr_mcRatioReso->SetName("gr_mcRatioReso");
  gr_mcRatioReso->SetMarkerStyle(20);
  gr_mcRatioReso->SetMarkerColor(recoPhot_color);
  gr_mcRatioReso->SetMarkerSize(markerSize);

  // genPhot: y = mx + q
  // recoGen: y = c
  // [0] = c; [1] = q; [2] = m
  /*TF1* fit_mcRatioReso = new TF1("fit_mcRatioReso", "sqrt([0]*[0] + [1]*[1] + 2.*[1]*[2]*x + [2]*[2]*x*x)");
    fit_mcRatioReso->SetRange(0., xMax_fit);
    fit_mcRatioReso->SetParameter(0, 1);
  //if( NOQ_ )
  //  fit_mcRatioReso->FixParameter(1, 0.5*q); //to evaluate syst
  //else
  fit_mcRatioReso->FixParameter(1, 1); //fixed
  if( FIXM_ ) {
  fit_mcRatioReso->FixParameter(2, 1);
  } else {
  fit_mcRatioReso->SetParameter(2, 1);
  //fit_extrapToZero_sqrt->SetParLimits(2, 0., 0.05);
  fit_mcRatioReso->SetParLimits(2, 0., 0.05*100.);
  }*/
  TF1* fit_mcRatioReso = new TF1("fit_mcRatioReso", "[0] + [1]*x");
  fit_mcRatioReso->SetParameter(0, 1);
  fit_mcRatioReso->SetParameter(1, 0);
  fit_mcRatioReso->SetLineStyle(2);
  fit_mcRatioReso->SetLineColor(recoPhot_color);
  fit_mcRatioReso->SetLineWidth(1.);

  gr_mcRatioReso->Fit(fit_mcRatioReso, "RQ");

  gr_mcRatioReso_vs_pt->SetPoint(iPtBin, ptPhotReco_thisBin, fit_mcRatioReso->GetParameter(0));

  c1_reso = new TCanvas("c1_reso", "c1_reso", 600, 600);
  c1_reso->Clear();
  c1_reso->cd();
  h2_axes_resp = new TH2D("axes_resp", "", 10, 0., xMax_axis, 10, yMin_axis, yMax_resp);
  h2_axes_resp->SetYTitle("DATA/MC ratio");
  h2_axes_resp->Draw();

  if (etaRegion_str != "")
    legend_reso = new TLegend(legbox.xMin, legbox.yMin, legbox.xMax, legbox.yMax, etaRegion_str.c_str());
  else
    legend_reso = new TLegend(legbox.xMin, legbox.yMin, legbox.xMax, legbox.yMax);
  legend_reso->SetTextSize(0.035);
  legend_reso->SetTextFont(42);
  //legend_resp->SetFillStyle(0);
  legend_reso->SetFillColor(kWhite);
  legend_reso->AddEntry(gr_mcRatioReso, "DATA/MC ratio", "P");

  legend_reso->Draw("same");
  label_resp->Draw("same");
  label_cms->Draw("same");
  label_sqrt->Draw("same");
  label_algo->Draw("same");

  gr_mcRatioReso->Draw("Psame");

  if (OUTPUT_GRAPHS || ptMin == 150) {
    char output_name[500];
    sprintf(output_name, "%s/reso_dataMC_ratio_%s_%s_ptPhot_%d_%d.pdf", get_outputdir().c_str(), rawPostfix.c_str(), etaRegion.c_str(), (int)ptMin, (int)ptMax);
    std::cout << output_name << std::endl;
    c1_reso->SaveAs(output_name);
  }

  delete gr_mcRatioReso;
  delete fit_mcRatioReso;
  delete legend_reso;

  delete c1_reso;
  delete h2_axes_reso;
  delete h2_axes_resp;


  delete label_reso;
  delete label_resp;

  delete gr_reso_recoPhot;
  delete gr_reso_recoGen;

  //gr_pullResp_vs_pt->SetPoint( iPtBin, 100.*(extrapResp_RecoRel[iPtBin] - intrResp_RecoRel[iPtBin])/intrResp_RecoRel[iPtBin];
  //pullResp_err_RecoRel[iPtBin] = 100.*sqrt( extrapResp_err_RecoRel[iPtBin]*extrapResp_err_RecoRel[iPtBin] + intrResp_RecoRel[iPtBin]*intrResp_RecoRel[iPtBin] );

  //pullReso_RecoRel[iPtBin] = 100.*(extrapReso_RecoRel[iPtBin] - intrReso_RecoRel[iPtBin])/intrReso_RecoRel[iPtBin];
  //pullReso_err_RecoRel[iPtBin] = 100.*sqrt( extrapReso_err_RecoRel[iPtBin]*extrapReso_err_RecoRel[iPtBin] + intrReso_RecoRel[iPtBin]*intrReso_RecoRel[iPtBin] );

  //imbalance_RecoRel[iPtBin] *= 100.; //in percent
  //imbalance_err_RecoRel[iPtBin] *= 100.;

  } //for iPtBin

  gr_DATAResp_vs_pt->Write();
  gr_extrapResp_vs_pt->Write();
  //if (! corrected) {
  gr_DATARespMPF_vs_pt->Write();
  gr_extrapRespMPF_vs_pt->Write();
  /*} else {
    gr_DATARespMPF_TypeICor_vs_pt->Write();
    gr_extrapRespMPF_TypeICor_vs_pt->Write();
    gr_DATARespMPF_TypeIpIICor_vs_pt->Write();
    gr_extrapRespMPF_TypeIpIICor_vs_pt->Write();
    }*/
  gr_intrResp_vs_pt->Write();
  gr_qResp_vs_pt->Write();
  //gr_qGenPartResp_vs_pt->Write();
  gr_qGenGammaResp_vs_pt->Write();
  //gr_qPartGammaResp_vs_pt->Write();
  gr_qPhotGammaResp_vs_pt->Write();
  gr_mcRatioResp_vs_pt->Write();

  gr_DATAReso_vs_pt->Write();
  gr_extrapReso_vs_pt->Write();
  gr_reso_subtr_vs_pt->Write();
  gr_DATAReso_subtr_vs_pt->Write();
  gr_reso_ratio_vs_pt->Write();
  gr_intrReso_vs_pt->Write();
  gr_qReso_vs_pt->Write();
  //gr_qGenPartReso_vs_pt->Write();
  gr_qGenGammaReso_vs_pt->Write();
  //gr_qPartGammaReso_vs_pt->Write();
  gr_qPhotGammaReso_vs_pt->Write();
  //if (! corrected) {
  gr_DATAResoMPF_vs_pt->Write();
  gr_extrapResoMPF_vs_pt->Write();
  /*} else {
    gr_DATAResoMPF_TypeICor_vs_pt->Write();
    gr_extrapResoMPF_TypeICor_vs_pt->Write();
    gr_DATAResoMPF_TypeIpIICor_vs_pt->Write();
    gr_extrapResoMPF_TypeIpIICor_vs_pt->Write();
    }*/
  gr_mcRatioReso_vs_pt->Write();

  graphFile->Close();


} //drawExtrap

void drawExtrap::getXPoints(int ptBin, Float_t* x, Float_t* x_err) const {


  /*for (int i = 0; i < nPoints; ++i) {
    char fullName[100];
    sprintf(fullName, "%s_%d", xHistoName,  i);
    TH1D* h1_pt2ndJetMean = (TH1D*)file->Get(fullName);
    x[i] = h1_pt2ndJetMean->GetMean();
    x_err[i] =  h1_pt2ndJetMean->GetRMS() / sqrt((Float_t)h1_pt2ndJetMean->GetEntries());
    }*/
  std::pair<float, float> bin = mExtrapBinning.getBinValue(ptBin);

  float minPt = bin.first;
  float maxPt = 0;
  float stepPt = bin.second - bin.first;
  size_t numPoints = mExtrapBinning.size();

  for (size_t i = 0; i < numPoints; i++) {

    maxPt = minPt + stepPt;
    x[i] = (minPt + maxPt) / 2.;
    x_err[i] = 0.;

    minPt += stepPt;
  }

} //getxpoints

void drawExtrap::getYPointsVector(TFile *file, const std::string& yHistoName, Int_t nPoints, std::vector<Float_t>& y_resp, std::vector<Float_t>& y_resp_err, std::vector<Float_t>& y_reso, std::vector<Float_t>& y_reso_err) const {

  int min = -1, max = 0;
  for (int i = 0; i < nPoints; i++) {
    TString fullName = TString::Format("%s_%d", yHistoName.c_str(), i);
    TH1* h1_r = static_cast<TH1*>(file->Get(fullName));

    if (! h1_r) {
      std::cout << "Didn't find " << yHistoName << " in file " << file->GetName() << std::endl;
      return;
    }

    if (h1_r->GetMean() > 0.4) {
      if (min < 0)
        min = i;

      max = i;
    } else if (min > 0)
      break;
  }

  for (int i = 0; i < nPoints; ++i) {
    if (i < min)
      continue;
    if (i > max)
      return;

    TString fullName = TString::Format("%s_%d", yHistoName.c_str(), i);
    TH1* h1_r = static_cast<TH1*>(file->Get(fullName));

    if (! h1_r) {
      std::cout << "Didn't find " << yHistoName << " in file " << file->GetName() << std::endl;
      return;
    }

    Float_t mean70, mean70_err, rms70, rms70_err;
    fitTools::getTruncatedMeanAndRMS(h1_r, mean70, mean70_err, rms70, rms70_err, 0.70, 0.70);

    Float_t mean95, mean95_err, rms95, rms95_err;
    fitTools::getTruncatedMeanAndRMS(h1_r, mean95, mean95_err, rms95, rms95_err, 0.95, 0.95);

    Float_t mean99, mean99_err, rms99, rms99_err;
    fitTools::getTruncatedMeanAndRMS(h1_r, mean99, mean99_err, rms99, rms99_err, 0.99, 0.99);

    //Float_t mean, mean_err, rms, rms_err;
    //fitTools::getProjectionMeanAndRMS(h1_r, mean, mean_err, rms, rms_err, 1., 0.9);

    TF1* gaussian = new TF1("gaussian", "gaus");
    gaussian->SetLineColor(kRed);
    TH1* newhisto = NULL;
    fitTools::fitProjection_sameArea(h1_r, gaussian, &newhisto, (Float_t)(INTPERC_ / 100.), "RQ");
    //fitTools::fitProjection(h1_r, gaussian, 2., "RQ");

    Float_t mu = gaussian->GetParameter(1);
    Float_t sigma = gaussian->GetParameter(2);
    //TF1* gaussian_chi = new TF1("gaussian_chi", "gaus");
    //fitTools::fitProjection(h1_r, gaussian_chi, 1.5, "RQN");
    Float_t mu_err = gaussian->GetParError(1);
    //Float_t sigma_err = gaussian->GetParError(2);
    //Float_t sigma_err = (LUMI>0) ? sigma/sqrt((Float_t)LUMI*h1_r->Integral()*(Float_t)INT_PERC_/100.) : h1_r->GetRMSError();
    Float_t sigma_err = h1_r->GetRMSError();

    if (FIT_RMS_ == "FIT") {

      y_resp.push_back(mu);
      y_resp_err.push_back(mu_err);

      y_reso.push_back(sigma / mu);
      y_reso_err.push_back(sqrt(sigma_err * sigma_err / (mu * mu) + sigma * sigma * mu_err * mu_err / (mu * mu * mu * mu)));

    } else if (FIT_RMS_ == "RMS99") {

      y_resp.push_back(mean99);
      y_resp_err.push_back(mean99_err);

      y_reso.push_back(rms99 / mean99);
      y_reso_err.push_back(sqrt(rms99_err * rms99_err / (mean99 * mean99) + rms99 * rms99 * mean99_err * mean99_err / (mean99 * mean99 * mean99 * mean99)));

    } else {

      std::cout << "WARNING!! FIT_RMS type '" << FIT_RMS_ << "' currently not supported. Exiting." << std::endl;
      exit(66);

    }

  } //for
}

void drawExtrap::getYPoints(TFile * file, const char* yHistoName, Int_t nPoints, Float_t* y_resp, Float_t* y_resp_err,  Float_t* y_reso, Float_t* y_reso_err) const {

  for (int i = 0; i < nPoints; ++i) {

    TString fullName = TString::Format("%s_%d", yHistoName, i);
    TH1* h1_r = (TH1*) file->Get(fullName);

    if (! h1_r) {
      std::cout << "Didn't find " << fullName << " in file " << file->GetName() << std::endl;
      return;
    }

    //this ugly fix saves from empty relative pt binning plots
    if (h1_r->GetEntries() == 0) {
      y_resp[i] = -1.;
      y_resp_err[i] = 0.;
      y_reso[i] = -1.;
      y_reso_err[i] = 0.;
      continue;
    }

    float mean = h1_r->GetMean();
    float rms = h1_r->GetRMS();
    float mean_err = h1_r->GetMeanError();
    float rms_err = h1_r->GetRMSError();

    float rmsFactor = -1;

    if (FIT_RMS_ == "FIT") {

      TF1* gaussian = new TF1("gaussian", "gaus");
      gaussian->SetLineColor(kRed);
      TH1* newhisto = NULL;
      fitTools::fitProjection_sameArea(h1_r, gaussian, &newhisto, (Float_t)(INTPERC_ / 100.), "RQ");

      float mu = gaussian->GetParameter(1);
      float sigma = gaussian->GetParameter(2);
      float mu_err = gaussian->GetParError(1);
      float sigma_err = h1_r->GetRMSError();

      y_resp[i] = mu;
      y_resp_err[i] = mu_err;

      y_reso[i] = sigma / mu;
      y_reso_err[i] = sqrt(sigma_err * sigma_err / (mu * mu) + sigma * sigma * mu_err * mu_err / (mu * mu * mu * mu));

      continue;

    } else if (FIT_RMS_ == "RMS70") {
      rmsFactor = 0.70;
    } else if (FIT_RMS_ == "RMS95") {
      rmsFactor = 0.95;
    } else if (FIT_RMS_ == "RMS99") {
      rmsFactor = 0.99;
    } else {
      std::cout << "WARNING!! FIT_RMS type '" << FIT_RMS_ << "' currently not supported. Exiting." << std::endl;
      exit(66);
    }

    if (rmsFactor > 0) {
      fitTools::getTruncatedMeanAndRMS(h1_r, mean, mean_err, rms, rms_err, rmsFactor, rmsFactor);
    }

    y_resp[i] = mean;
    y_resp_err[i] = mean_err;

    y_reso[i] = rms / mean;
    y_reso_err[i] = sqrt(rms_err * rms_err / (mean * mean) + rms * rms * mean_err * mean_err / (mean * mean * mean * mean));
  } //for

} //getYPoints

