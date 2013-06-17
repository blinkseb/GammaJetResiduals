//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed Mar 28 16:00:10 2012 by ROOT version 5.32/00
// from TTree muons/muons tree
// found on file: output_mc.root
//////////////////////////////////////////////////////////

#pragma once

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.

// Fixed size dimensions of array or collections stored in the TTree if any.

class LeptonTree {
  public :
    TTree          *fChain;   //!pointer to the analyzed TTree or TChain
    Int_t           fCurrent; //!current Tree number in a TChain

    // Declaration of leaf types
    Int_t           n;
    Int_t           id[30];   //[n]
    Float_t         pt[30];   //[n]
    Float_t         px[30];   //[n]
    Float_t         py[30];   //[n]
    Float_t         pz[30];   //[n]
    Float_t         eta[30];   //[n]
    Float_t         phi[30];   //[n]
    Int_t           charge[30];   //[n]

    // List of branches
    TBranch        *b_n;   //!
    TBranch        *b_id;   //!
    TBranch        *b_pt;   //!
    TBranch        *b_px;   //!
    TBranch        *b_py;   //!
    TBranch        *b_pz;   //!
    TBranch        *b_eta;   //!
    TBranch        *b_phi;   //!
    TBranch        *b_charge;   //!

    LeptonTree();
    virtual ~LeptonTree();
    virtual Int_t    GetEntry(Long64_t entry);

    virtual void     Init(TTree *tree);
    virtual void     InitCache();
};

LeptonTree::LeptonTree() : fChain(0) 
{
}

LeptonTree::~LeptonTree()
{
  if (!fChain)
    return;

  delete fChain->GetCurrentFile();
}

Int_t LeptonTree::GetEntry(Long64_t entry)
{
  // Read contents of entry.
  if (!fChain)
    return 0;

  return fChain->GetEntry(entry);
}

void LeptonTree::Init(TTree *tree)
{
  // Set branch addresses and branch pointers
  if (!tree)
    return;

  fChain = tree;
  fChain->SetMakeClass(1);

  fChain->SetBranchAddress("n", &n, &b_n);
  fChain->SetBranchAddress("id", &id, &b_id);
  fChain->SetBranchAddress("pt", &pt, &b_pt);
  fChain->SetBranchAddress("px", &px, &b_px);
  fChain->SetBranchAddress("py", &py, &b_py);
  fChain->SetBranchAddress("pz", &pz, &b_pz);
  fChain->SetBranchAddress("eta", &eta, &b_eta);
  fChain->SetBranchAddress("phi", &phi, &b_phi);
  fChain->SetBranchAddress("charge", &charge, &b_charge);
}

void LeptonTree::InitCache() {
  //fChain->SetCacheSize(-1);
  //fChain->AddBranchToCache("*");
}
