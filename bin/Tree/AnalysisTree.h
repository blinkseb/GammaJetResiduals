//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed Mar 28 15:59:52 2012 by ROOT version 5.32/00
// from TTree analysis/analysis tree
// found on file: output_mc.root
//////////////////////////////////////////////////////////

#pragma once

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.

// Fixed size dimensions of array or collections stored in the TTree if any.

class AnalysisTree {
  public :
    TTree          *fChain;   //!pointer to the analyzed TTree or TChain

    // Declaration of leaf types
    Float_t         ntrue_interactions;
    UInt_t          nvertex;
    Int_t           pu_nvertex;
    Float_t         event_weight;
    Float_t         generator_weight;

    // List of branches
    TBranch        *b_ntrue_interactions;   //!
    TBranch        *b_nvertex;   //!
    TBranch        *b_pu_nvertex;   //!
    TBranch        *b_event_weight;   //!

    AnalysisTree();
    virtual ~AnalysisTree();
    virtual Int_t    GetEntry(Long64_t entry);

    virtual void     Init(TTree *tree);
};


AnalysisTree::AnalysisTree() : fChain(0) 
{
}

AnalysisTree::~AnalysisTree()
{
  if (!fChain)
    return;

  delete fChain->GetCurrentFile();
}

Int_t AnalysisTree::GetEntry(Long64_t entry)
{
  // Read contents of entry.
  if (!fChain)
    return 0;

  return fChain->GetEntry(entry);
}


void AnalysisTree::Init(TTree *tree)
{
  // Set branch addresses and branch pointers
  if (!tree)
    return;

  fChain = tree;
  fChain->SetMakeClass(1);

  fChain->SetBranchAddress("ntrue_interactions", &ntrue_interactions, &b_ntrue_interactions);
  fChain->SetBranchAddress("nvertex", &nvertex, &b_nvertex);
  fChain->SetBranchAddress("pu_nvertex", &pu_nvertex, &b_nvertex);
  fChain->SetBranchAddress("event_weight", &event_weight, &b_event_weight);
  fChain->SetBranchAddress("generator_weight", &generator_weight, NULL);
}
