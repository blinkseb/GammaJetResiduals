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

#include "BaseTree.h"

// Header file for the classes stored in the TTree if any.

// Fixed size dimensions of array or collections stored in the TTree if any.

class GenJetTree: public BaseTree {
  public :

    // Declaration of leaf types
    int             parton_pdg_id;
    int             parton_flavour;

    // List of branches

    GenJetTree();

    virtual void     Init(TTree *tree);
};

GenJetTree::GenJetTree():
  BaseTree::BaseTree()
{
}

void GenJetTree::Init(TTree *tree)
{
  // Set branch addresses and branch pointers
  if (! tree)
    return;

  BaseTree::Init(tree);

  fChain->SetBranchAddress("parton_pdg_id", &parton_pdg_id, NULL);
  fChain->SetBranchAddress("parton_flavour", &parton_flavour, NULL);
}
