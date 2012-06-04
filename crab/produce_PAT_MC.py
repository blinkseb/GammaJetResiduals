## import skeleton process
from PhysicsTools.PatAlgos.patTemplate_cfg import *

# load the PAT config
process.load("PhysicsTools.PatAlgos.patSequences_cff")

# Is this Data or MC ?
runOnMC = True

from PhysicsTools.SelectorUtils.pvSelector_cfi import pvSelector

process.goodOfflinePrimaryVertices = cms.EDFilter("PrimaryVertexObjectFilter",
    filterParams = pvSelector.clone( minNdof = cms.double(4.0), maxZ = cms.double(24.0) ),
    src = cms.InputTag('offlinePrimaryVertices')
    )

# Configure PAT to use PF2PAT instead of AOD sources
# this function will modify the PAT sequences.
from PhysicsTools.PatAlgos.tools.pfTools import *
from PhysicsTools.PatAlgos.tools.metTools import *

def usePF2PATForAnalysis(jetAlgo, postfix, useTypeIMET, usePFNoPU):

  # Jet corrections
  # No L2L3 Residual on purpose
  if usePFNoPU:
    jetCorrections = ("%sPFchs" % algo, ['L1FastJet', 'L2Relative', 'L3Absolute'])
    postfix += "chs"
  else:
    jetCorrections = ("%sPF" % algo, ['L1FastJet', 'L2Relative', 'L3Absolute'])

  p = postfix
  usePF2PAT(process, runPF2PAT=True, jetAlgo=jetAlgo, runOnMC=runOnMC, postfix=p, jetCorrections=jetCorrections, typeIMetCorrections = useTypeIMET)
  getattr(process, "pfPileUp" + p).Enable = cms.bool(usePFNoPU)
  if usePFNoPU:
    getattr(process, "pfPileUp" + p).Vertices = 'goodOfflinePrimaryVertices'
    getattr(process, "pfPileUp" + p).checkClosestZVertex = cms.bool(False)

  getattr(process, "pfJets" + p).doAreaFastjet = cms.bool(True)
  getattr(process, "pfJets" + p).doRhoFastjet = False

  # top projections in PF2PAT:
  getattr(process,"pfNoPileUp" + p).enable = usePFNoPU
  getattr(process,"pfNoMuon" + p).enable = True
  getattr(process,"pfNoElectron" + p).enable = True
  getattr(process,"pfNoTau" + p).enable = False
  getattr(process,"pfNoJet" + p).enable = True

  # verbose flags for the PF2PAT modules
  getattr(process,"pfNoMuon" + p).verbose = False

  # enable delta beta correction for muon selection in PF2PAT?
  getattr(process,"pfIsolatedMuons" + p).doDeltaBetaCorrection = False

  # Keep only jets with pt > 2 Gev
  getattr(process, "selectedPatJets" + p).cut = "pt > 2";

  # Remove uneeded objects
  names = ["Taus"]
  if jetAlgo != "AK5":
    names += ["Electrons", "Muons"]
  removeSpecificPATObjects(process, names = names, outputModules = ['out'], postfix = p) 

  adaptPVs(process, pvCollection = cms.InputTag("goodOfflinePrimaryVertices"), postfix = p)
  return getattr(process, "patPF2PATSequence" + p)

# Do we correct MET with Type1?
correctMETWithT1 = True

print "##########################"
print "PF jets with PF2PAT"
print "Using Type I met" if correctMETWithT1 else "NOT using Type I met"
print "##########################"

postfixes = {'AK5PFlow': 'AK5', 'AK7PFlow': 'AK7'}

path = cms.Sequence()
for p, algo in postfixes.items():

  path += usePF2PATForAnalysis(jetAlgo=algo, postfix=p, usePFNoPU=False, useTypeIMET=correctMETWithT1)
  #path += usePF2PATForAnalysis(jetAlgo=algo, postfix=p, usePFNoPU=True, useTypeIMET=correctMETWithT1)


# Clean patDefaultSequence from MC matching before cloning MET, Jets, etc.
if not runOnMC:
  # Remove MC Matching
  removeMCMatching(process, names = ["All"])

print "##########################"
print "Calo jets"
print "##########################"

# No L2L3 Residual on purpose
jetCorrections = ('AK5Calo', ['L1Offset', 'L2Relative', 'L3Absolute'])

addJetCollection(process, cms.InputTag('ak7CaloJets'),
    'AK7',
    'Calo',
    doJTA            = True,
    doBTagging       = True,
    jetCorrLabel     = jetCorrections,
    doType1MET       = correctMETWithT1,
    doL1Cleaning     = False,                 
    doL1Counters     = False,
    genJetCollection = cms.InputTag("ak7GenJets"),
    doJetID          = True,
    jetIdLabel       = "ak7"
    )

switchJetCollection(process, cms.InputTag('ak5CaloJets'),
    doJTA            = True,
    doBTagging       = True,
    jetCorrLabel     = jetCorrections,
    doType1MET       = correctMETWithT1,
    genJetCollection = cms.InputTag("ak5GenJets"),
    doJetID          = True,
    jetIdLabel       = "ak5"
    )

process.selectedPatJets.cut = "pt > 2"
process.selectedPatJetsAK7Calo.cut = "pt > 2"

if not runOnMC:
  # Remove MC Matching
  removeMCMatching(process, names = ["All"])

removeSpecificPATObjects(process, names = ['Electrons', 'Muons', 'Taus'])

# Add default pat sequence to our path
# This brings to life TcMET, Calo jets and Photons
path *= process.patDefaultSequence

# Filtering

# require physics declared
process.load('HLTrigger.special.hltPhysicsDeclared_cfi')
process.hltPhysicsDeclared.L1GtReadoutRecordTag = 'gtDigis'

# require scraping filter
process.scrapingVeto = cms.EDFilter("FilterOutScraping",
    applyfilter = cms.untracked.bool(True),
    debugOn = cms.untracked.bool(False),
    numtrack = cms.untracked.uint32(10),
    thresh = cms.untracked.double(0.2)
    )

# HB + HE noise filtering
process.load('CommonTools/RecoAlgos/HBHENoiseFilter_cfi')
process.HBHENoiseFilter.minIsolatedNoiseSumE        = 999999.
process.HBHENoiseFilter.minNumIsolatedNoiseChannels = 999999
process.HBHENoiseFilter.minIsolatedNoiseSumEt       = 999999

# Count events
process.nEventsTotal    = cms.EDProducer("EventCountProducer")
process.nEventsFiltered = cms.EDProducer("EventCountProducer")

# Let it run
process.p = cms.Path(
    process.nEventsTotal *
    process.scrapingVeto *
    process.goodOfflinePrimaryVertices *
    process.HBHENoiseFilter *
    path *
    process.nEventsFiltered
    )

# Add PF2PAT output to the created file
from PhysicsTools.PatAlgos.patEventContent_cff import patEventContentNoCleaning
process.load("CommonTools.ParticleFlow.PF2PAT_EventContent_cff")
process.out.outputCommands = cms.untracked.vstring('drop *',
    'keep *_photonCore_*_*',
    'keep double_kt6*Jets*_rho_*',
    'keep *_goodOfflinePrimaryVertices*_*_*',
    'keep recoPFCandidates_particleFlow_*_*',
    # Content of *patEventContentNoCleaning
    'keep *_selectedPatPhotons*_*_*', 'keep *_selectedPatElectrons*_*_*', 'keep *_selectedPatMuons*_*_*', 'keep *_selectedPatTaus*_*_*', 'keep *_selectedPatJets*_*_*', 'drop *_selectedPatJets_pfCandidates_*', 'drop *_*PF_caloTowers_*', 'drop *_*JPT_pfCandidates_*', 'drop *_*Calo_pfCandidates_*', 'keep *_patMETs*_*_*', 'keep *_selectedPatPFParticles*_*_*', 'keep *_selectedPatTrackCands*_*_*',
    'drop *_*PFlow_caloTowers_*',
    'keep *_addPileupInfo_*_*',
    'keep *recoTracks_generalTracks_*_*',
    'keep *_generator_*_*',
    # For Photon ID
    'keep *_reducedEcalRecHitsEB_*_*',
    'keep *_hybridSuperClusters_hybridBarrelBasicClusters_*'
    )

# switch on PAT trigger
# from PhysicsTools.PatAlgos.tools.trigTools import switchOnTrigger
# switchOnTrigger( process )

#  In addition you usually want to change the following
#  parameters:
## ------------------------------------------------------
#
process.GlobalTag.globaltag = cms.string("START44_V13::All") ##  (according to https://twiki.cern.ch/twiki/bin/view/CMS/SWGuideFrontierConditions)
#                                         ##
process.source.fileNames =  cms.untracked.vstring('file:input_mc.root')  ##  (e.g. 'file:AOD.root')
#                                         ##
process.maxEvents.input = 500
#                                         ##
#   process.out.outputCommands = [ ... ]  ##  (e.g. taken from PhysicsTools/PatAlgos/python/patEventContent_cff.py)
#                                         ##
process.out.fileName = 'patTuple_PF2PAT_MC.root'
#                                         ##
#   process.options.wantSummary = False   ##  (to suppress the long output at the end of the job)
