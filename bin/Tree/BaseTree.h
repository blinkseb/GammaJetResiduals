//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed Mar 28 15:43:50 2012 by ROOT version 5.32/00
// from TTree photon/photon tree
// found on file: output_mc.root
//////////////////////////////////////////////////////////

#pragma once

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.

// Fixed size dimensions of array or collections stored in the TTree if any.

class BaseTree {
  public :
    TTree          *fChain;   //!pointer to the analyzed TTree or TChain

    // Declaration of leaf types
    Int_t           is_present;
    Float_t         et;
    Float_t         pt;
    Float_t         eta;
    Float_t         phi;
    Float_t         px;
    Float_t         py;
    Float_t         pz;
    Float_t         e;

    // List of branches
    TBranch        *b_is_present;   //!
    TBranch        *b_et;   //!
    TBranch        *b_pt;   //!
    TBranch        *b_eta;   //!
    TBranch        *b_phi;   //!
    TBranch        *b_px;   //!
    TBranch        *b_py;   //!
    TBranch        *b_pz;   //!

    BaseTree();
    virtual ~BaseTree();
    virtual Int_t    GetEntry(Long64_t entry);
    virtual void     Init(TTree *tree);
    virtual void     InitCache();
};

typedef BaseTree GenTree;

BaseTree::BaseTree() : fChain(0)
{
}

BaseTree::~BaseTree()
{
  if (!fChain)
    return;

  delete fChain->GetCurrentFile();
}

Int_t BaseTree::GetEntry(Long64_t entry)
{
  // Read contents of entry.
  if (!fChain)
    return 0;

  return fChain->GetEntry(entry);
}

void BaseTree::Init(TTree *tree)
{
  // Set branch addresses and branch pointers
  if (! tree)
    return;

  fChain = tree;
  fChain->SetMakeClass(1);

  fChain->SetBranchAddress("is_present", &is_present, &b_is_present);
  fChain->SetBranchAddress("et", &et, &b_et);
  fChain->SetBranchAddress("pt", &pt, &b_pt);
  fChain->SetBranchAddress("eta", &eta, &b_eta);
  fChain->SetBranchAddress("phi", &phi, &b_phi);
  fChain->SetBranchAddress("px", &px, &b_px);
  fChain->SetBranchAddress("py", &py, &b_py);
  fChain->SetBranchAddress("pz", &pz, &b_pz);
  fChain->SetBranchAddress("e", &e, NULL);
  
  InitCache();
}

void BaseTree::InitCache() {
  //fChain->SetCacheSize(-1);
  //fChain->AddBranchToCache("*");
}
