TTree* interative_analysis(TFile* f, bool step2 = false) {
  TTree* t = new TTree("t", "t");

  if (step2) {
    t->AddFriend((TTree*) f->Get("gammaJet/photon"));
    t->AddFriend((TTree*) f->Get("gammaJet/analysis"));
    t->AddFriend((TTree*) f->Get("gammaJet/muons"));
    t->AddFriend((TTree*) f->Get("gammaJet/electrons"));
    t->AddFriend((TTree*) f->Get("gammaJet/AK5PFlow/first_jet"));
    t->AddFriend((TTree*) f->Get("gammaJet/AK5PFlow/second_jet"));
    t->AddFriend((TTree*) f->Get("gammaJet/AK5PFlow/met"));
    t->AddFriend((TTree*) f->Get("gammaJet/AK5PFlow/misc"));
  } else {
    t->AddFriend((TTree*) f->Get("photon"));
    t->AddFriend((TTree*) f->Get("misc"), "analysis");
    t->AddFriend((TTree*) f->Get("muons"));
    t->AddFriend((TTree*) f->Get("electrons"));
    t->AddFriend((TTree*) f->Get("first_jet"));
    t->AddFriend((TTree*) f->Get("second_jet"));
    t->AddFriend((TTree*) f->Get("met"));
  }

  return t;
}
