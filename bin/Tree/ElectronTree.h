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

#include "LeptonTree.h"

// Header file for the classes stored in the TTree if any.

// Fixed size dimensions of array or collections stored in the TTree if any.

class ElectronTree: public LeptonTree {
  public :

    // Declaration of leaf types
    Float_t         isolation[30];   //[n]

    // List of branches
    TBranch        *b_isolation;   //!

    virtual void     Init(TTree *tree);
};

void ElectronTree::Init(TTree *tree)
{
  // Set branch addresses and branch pointers
  if (!tree)
    return;

  LeptonTree::Init(tree);

  fChain->SetBranchAddress("isolation", &isolation, &b_isolation);

  // Enable cache for better read performances
  fChain->SetCacheSize(10000000);
  fChain->AddBranchToCache("*");
}
