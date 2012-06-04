import FWCore.ParameterSet.Config as cms

## import skeleton process
from PhysicsTools.PatAlgos.patTemplate_cfg import *
from PhysicsTools.PatAlgos.tools.coreTools import *
from PhysicsTools.PatAlgos.tools.cmsswVersionTools import *

process = cms.Process("Test")

process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.GlobalTag.globaltag = 'GR_R_44_V13::All'
#process.GlobalTag.globaltag = 'START42_V13::All'
# process.GlobalTag.globaltag = 'GR_H_V22::All'

#from CondCore.DBCommon.CondDBSetup_cfi import *
#process.jec = cms.ESSource("PoolDBESSource",CondDBSetup,
#                           connect = cms.string("frontier://FrontierPrep/CMS_COND_PHYSICSTOOLS"),
#                           toGet =  cms.VPSet(
#                            cms.PSet(record = cms.string("JetCorrectionsRecord"),
#                                     tag = cms.string("JetCorrectorParametersCollection_Jec11_V9_AK5Calo"),
#                                    label= cms.untracked.string("AK5Calo")),
#                            cms.PSet(record = cms.string("JetCorrectionsRecord"),
#                                    tag = cms.string("JetCorrectorParametersCollection_Jec11_V9_AK5PF"),
#                                    label=cms.untracked.string("AK5PF")),
#                            cms.PSet(record = cms.string("JetCorrectionsRecord"),
#                                    tag = cms.string("JetCorrectorParametersCollection_Jec11_V9_AK5PFchs"),
#                                    label=cms.untracked.string("AK5PFchs")),
#                            cms.PSet(record = cms.string("JetCorrectionsRecord"),
#                                     tag = cms.string("JetCorrectorParametersCollection_Jec11_V9_AK5JPT"),
#                                     label=cms.untracked.string("AK5JPT")),
#                              )
#                        )
#es_prefer_jec = cms.ESPrefer("PoolDBESSource", "jec")

process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(1))
process.source = cms.Source("EmptySource")
process.readAK5PF    = cms.EDAnalyzer('JetCorrectorDBReader',  
        # below is the communication to the database 
        payloadName    = cms.untracked.string('AK5PF'),
        # this is used ONLY for the name of the printed txt files. You can use any name that you like, 
        # but it is recommended to use the GT name that you retrieved the files from.
        globalTag      = cms.untracked.string('Jec11_V12'),
        printScreen    = cms.untracked.bool(False),
        createTextFile = cms.untracked.bool(True)
  )
process.readAK5Calo = process.readAK5PF.clone(payloadName = 'AK5Calo')
process.readAK5JPT = process.readAK5PF.clone(payloadName = 'AK5JPT')
process.readAK5PFchs = process.readAK5PF.clone(payloadName = 'AK5PFchs')
process.p = cms.Path(process.readAK5PF * process.readAK5Calo * process.readAK5JPT * process.readAK5PFchs)
