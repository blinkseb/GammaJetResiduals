//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed Mar 28 15:43:50 2012 by ROOT version 5.32/00
// from TTree photon/photon tree
// found on file: output_mc.root
//////////////////////////////////////////////////////////

#pragma once

#include "BaseTree.h"

// Header file for the classes stored in the TTree if any.

// Fixed size dimensions of array or collections stored in the TTree if any.

class PhotonTree: public BaseTree {
  public :

    // Declaration of leaf types
    Bool_t          has_pixel_seed;
    Float_t         hadTowOverEm;
    Float_t         sigmaIetaIeta;
    Float_t         rho;
    Bool_t          hasMatchedPromptElectron;
    Float_t         chargedHadronsIsolation;
    Float_t         neutralHadronsIsolation;
    Float_t         photonIsolation; 

    // List of branches
    TBranch        *b_jet_area;

    virtual void     Init(TTree *tree);
};

void PhotonTree::Init(TTree *tree)
{
  // Set branch addresses and branch pointers
  if (! tree)
    return;

  BaseTree::Init(tree);

  fChain->SetBranchAddress("has_pixel_seed", &has_pixel_seed, NULL);
  fChain->SetBranchAddress("hadTowOverEm", &hadTowOverEm, NULL);
  fChain->SetBranchAddress("sigmaIetaIeta", &sigmaIetaIeta, NULL);
  fChain->SetBranchAddress("rho", &rho, NULL);
  fChain->SetBranchAddress("hasMatchedPromptElectron", &hasMatchedPromptElectron, NULL);
  fChain->SetBranchAddress("chargedHadronsIsolation", &chargedHadronsIsolation, NULL);
  fChain->SetBranchAddress("neutralHadronsIsolation", &neutralHadronsIsolation, NULL);
  fChain->SetBranchAddress("photonIsolation", &photonIsolation, NULL);
}
