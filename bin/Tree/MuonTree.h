//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed Mar 28 16:00:10 2012 by ROOT version 5.32/00
// from TTree muons/muons tree
// found on file: output_mc.root
//////////////////////////////////////////////////////////

#pragma once

#include <TChain.h>
#include <TFile.h>
#include "LeptonTree.h"

// Header file for the classes stored in the TTree if any.

// Fixed size dimensions of array or collections stored in the TTree if any.

class MuonTree: public LeptonTree {
  public :

    // Declaration of leaf types
    Float_t         relative_isolation[30];   //[n]
    Float_t         delta_beta_relative_isolation[30];   //[n]

    virtual void     Init(TTree *tree);
};

void MuonTree::Init(TTree *tree)
{
  // Set branch addresses and branch pointers
  if (!tree)
    return;

  LeptonTree::Init(tree);

  fChain->SetBranchAddress("relative_isolation", &relative_isolation, NULL);
  fChain->SetBranchAddress("delta_beta_relative_isolation", &relative_isolation, NULL);

  LeptonTree::InitCache();
}
