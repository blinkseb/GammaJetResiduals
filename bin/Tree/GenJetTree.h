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

#include <TClonesArray.h>
#include <TLorentzVector.h>

// Header file for the classes stored in the TTree if any.

// Fixed size dimensions of array or collections stored in the TTree if any.

class GenJetTree: public BaseTree {
  public :

    // Declaration of leaf types
    TClonesArray*    neutrinos;
    TClonesArray*    neutrinos_pdg_id;
    int              parton_pdg_id;
    TLorentzVector*  parton_p4;
    int              parton_flavour;

    virtual void     Init(TTree *tree);
    GenJetTree();
};

GenJetTree::GenJetTree() {

  neutrinos = NULL;
  neutrinos_pdg_id = NULL;
  parton_p4 = NULL;

}

void GenJetTree::Init(TTree *tree)
{
  // Set branch addresses and branch pointers
  if (! tree)
    return;

  BaseTree::Init(tree);

  if (fChain->GetBranch("neutrinos")) {
    neutrinos = new TClonesArray("TLorentzVector", 3);
    fChain->SetBranchAddress("neutrinos", &neutrinos, NULL);
  }

  if (fChain->GetBranch("neutrinos_pdg_id")) {
    neutrinos_pdg_id = new TClonesArray("TParameter<int>", 3);
    fChain->SetBranchAddress("neutrinos_pdg_id", &neutrinos_pdg_id, NULL);
  }
  
  if (fChain->GetBranch("parton_p4")) {
    parton_p4 = new TLorentzVector();
    fChain->SetBranchAddress("parton_p4", &parton_p4, NULL);
  }

  fChain->SetBranchAddress("parton_pdg_id", &parton_pdg_id, NULL);
  fChain->SetBranchAddress("parton_flavour", &parton_flavour, NULL);
  
  // Enable cache for better read performances
  fChain->SetCacheSize(10000000);
  fChain->AddBranchToCache("*");
}
