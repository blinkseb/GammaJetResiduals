gammaJetFinalizer -i PhotonJet_2ndLevel_Photon_Run2011.root -d Photon_Run2011 --algo ak5 --type pf --mc-comp
gammaJetFinalizer --input-list mc_QCD.list -d QCD --algo ak5 --type pf --mc --mc-comp
gammaJetFinalizer --input-list mc_G.list -d G --algo ak5 --type pf --mc --mc-comp
