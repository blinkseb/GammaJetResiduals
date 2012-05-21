import FWCore.ParameterSet.Config as cms
import os

process = cms.Process("GAMMAJET")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.threshold = cms.untracked.string('INFO')
process.MessageLogger.cerr.FwkReport.reportEvery = 1000

process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_cff')
process.GlobalTag.globaltag = cms.string("START44_V13::All")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

from FWCore.ParameterSet.VarParsing import VarParsing
maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )
readFiles = cms.untracked.vstring(
    #"/store/user/sbrochet/G_Pt-30to50_TuneZ2_7TeV_pythia6/JetMet_PF2PAT_Fall11/5105f901d1c8d136e9eaa25f22cecd3a/patTuple_PF2PAT_MC_59_1_3LQ.root"
    #"file:patTuple_PF2PAT_MC.root"
    "/store/user/sbrochet/G_Pt-120to170_TuneZ2star_8TeV_pythia6/JetMet_PF2PAT_01apr_Summer12/7dd90f19ebb89f404e8c497cae6b6c9f/patTuple_PF2PAT_MC_17_1_Zvq.root"
    )
process.source = cms.Source ("PoolSource", fileNames = readFiles)

from FWCore.ParameterSet.VarParsing import VarParsing
options = VarParsing()

options.register ('datasetName',
				  '',
				  VarParsing.multiplicity.singleton,
				  VarParsing.varType.string,
				  "The dataset currently processed. A folder named 'datasetName' must exists")

options.parseArguments()

# Extract number of generated events, cross section, and pt hat
if os.path.exists(options.datasetName): 
  file = os.path.join(options.datasetName, "xsection");
  f = open(file, "r");
  (generatedEvents, crossSection, ptHatMax, ptHatMin) = f.read().split()
  f.close()
  binnedSample = True
else:
  binnedSample = False
  generatedEvents = crossSection = ptHatMax = ptHatMin = 1

process.gammaJet = cms.EDFilter('GammaJetFilter',
    isMC = cms.untracked.bool(True),
    photons = cms.untracked.InputTag("selectedPatPhotons"),
    firstJetPtCut = cms.untracked.bool(False),

    binnedSample = cms.untracked.bool(binnedSample),

    # JEC
    doJetCorrection = cms.untracked.bool(False),
    correctJecFromRaw = cms.untracked.bool(False),
    #correctorLabel = cms.untracked.string("ak5PFL1FastL2L3")
    correctorLabel = cms.untracked.string("ak5PFResidual")
    )

if binnedSample:
  process.gammaJet.crossSection = cms.untracked.double(crossSection)
  process.gammaJet.generatedEvents = cms.untracked.uint64(int(generatedEvents))
  process.gammaJet.ptHatMin = cms.untracked.double(ptHatMin)
  process.gammaJet.ptHatMax = cms.untracked.double(ptHatMax)


process.p = cms.Path(process.gammaJet)

#process.out = cms.OutputModule("PoolOutputModule",
#    fileName = cms.untracked.string("delete_me.root"),
#    SelectEvents = cms.untracked.PSet(
#      SelectEvents = cms.vstring('p')
#      )
#    )

#process.out.outputCommands = cms.untracked.vstring('keep *',
#    'drop *_selectedPatJets*_*_*',
#    'drop *_selectedPatPhotons*_*_*',
#    'keep *_selectedPatJets*_genJets_*',
#    'keep *_selectedPatJets*_caloTowers_*',
#    # Drop CHS
#    'drop *_*chs*_*_*'
#)

process.TFileService = cms.Service("TFileService",
    fileName = cms.string("output_mc.root")
    )

#process.out.fileName = 'patTuple_cleaned.root'
process.options = cms.untracked.PSet(wantSummary = cms.untracked.bool(True))

#process.outpath = cms.EndPath(process.out)
