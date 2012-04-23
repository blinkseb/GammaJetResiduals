//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed Mar 28 15:43:50 2012 by ROOT version 5.32/00
// from TTree photon/photon tree
// found on file: output_mc.root
//////////////////////////////////////////////////////////

#ifndef BaseTree_h
#define BaseTree_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

#include "BaseTree.h"

// Header file for the classes stored in the TTree if any.

// Fixed size dimensions of array or collections stored in the TTree if any.

class JetTree: public BaseTree {
  public :

    // Declaration of leaf types
    Float_t         jet_area;
    Float_t         btag_tc_high_eff;
    Float_t         btag_tc_high_pur;
    Float_t         btag_ssv_high_eff;
    Float_t         btag_ssv_high_pur;
    Float_t         btag_jet_probability;
    Float_t         btag_jet_b_probability;

    // List of branches
    TBranch        *b_jet_area;

    JetTree();

    virtual void     Init(TTree *tree);
};

#endif

JetTree::JetTree():
  BaseTree::BaseTree()
{
}

void JetTree::Init(TTree *tree)
{
  // Set branch addresses and branch pointers
  if (! tree)
    return;

  BaseTree::Init(tree);

  fChain->SetBranchAddress("jet_area", &jet_area, &b_jet_area);
  fChain->SetBranchAddress("btag_tc_high_eff", &btag_tc_high_eff, NULL);
  fChain->SetBranchAddress("btag_tc_high_pur", &btag_tc_high_pur, NULL);
  fChain->SetBranchAddress("btag_ssv_high_eff", &btag_tc_high_eff, NULL);
  fChain->SetBranchAddress("btag_ssv_high_pur", &btag_tc_high_pur, NULL);
  fChain->SetBranchAddress("btag_jet_probability", &btag_jet_probability, NULL);
  fChain->SetBranchAddress("btag_jet_b_probability", &btag_jet_b_probability, NULL);
}
