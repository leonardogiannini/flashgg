import FWCore.ParameterSet.Config as cms
import FWCore.Utilities.FileUtils as FileUtils
import FWCore.ParameterSet.VarParsing as VarParsing

process = cms.Process("FLASHggMicroAOD")

# setup 'analysis'  options
#options = VarParsing.VarParsing ('analysis')
#
## setup any defaults you want
#options.outputFile = './output.root'
#options.inputFiles= './input.root'
#options.maxEvents = -1 # -1 means all events
#
## get and parse the command line arguments
#options.parseArguments()

process.load("FWCore.MessageService.MessageLogger_cfi")

process.load("Configuration.StandardSequences.GeometryDB_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")
#process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff") # gives deprecated message in 80X but still runs
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
from Configuration.AlCa.GlobalTag import GlobalTag

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )
process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32( 1000 )

import os
process.GlobalTag = GlobalTag(process.GlobalTag, '', '')
process.source = cms.Source("PoolSource",
                             fileNames = cms.untracked.vstring(
                             #"root://cms-xrd-global.cern.ch//store/mc/RunIIFall17MiniAODv2/ttHJetToGG_M125_13TeV_amcatnloFXFX_madspin_pythia8/MINIAODSIM/PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/10000/EC74FFEA-C842-E811-A99B-0CC47A7C3420.root"
                             "root://cms-xrd-global.cern.ch//store/mc/RunIIAutumn18MiniAOD/ttHJetToGG_M125_13TeV_amcatnloFXFX_madspin_pythia8/MINIAODSIM/102X_upgrade2018_realistic_v15-v1/260000/27891EA5-F05F-5648-AD45-1D47EC8BE543.root"
                             #"root://cms-xrd-global.cern.ch/" + options.inputFiles
                             )
                             #fileNames = cms.untracked.vstring("file:myMicroAODOutputFile.root")
)

process.RandomNumberGeneratorService = cms.Service("RandomNumberGeneratorService")
process.RandomNumberGeneratorService.flashggRandomizedPhotons = cms.PSet(
          initialSeed = cms.untracked.uint32(16253245)
        )

process.MessageLogger.cerr.threshold = 'ERROR' # can't get suppressWarning to work: disable all warnings for now
# process.MessageLogger.suppressWarning.extend(['SimpleMemoryCheck','MemoryCheck']) # this would have been better...

# Uncomment the following if you notice you have a memory leak
# This is a lightweight tool to digg further
#process.SimpleMemoryCheck = cms.Service("SimpleMemoryCheck",
#                                        ignoreTotal = cms.untracked.int32(1),
#                                        monitorPssAndPrivate = cms.untracked.bool(True)
#                                       )

process.load("flashgg/MicroAOD/flashggMicroAODSequence_cff")

# NEEDED FOR ANYTHING PRIOR TO reMiniAOD
#process.weightsCount.pileupInfo = "addPileupInfo"

from flashgg.MicroAOD.flashggMicroAODOutputCommands_cff import microAODDefaultOutputCommand
process.out = cms.OutputModule("PoolOutputModule", fileName = cms.untracked.string("testmicroaod.root"),
                               outputCommands = microAODDefaultOutputCommand
                               )

process.p = cms.Path(process.flashggMicroAODSequence)
process.e = cms.EndPath(process.out)

from flashgg.MicroAOD.MicroAODCustomize import customize
customize(process)

if "DY" in customize.datasetName or "SingleElectron" in customize.datasetName or "DoubleEG" in customize.datasetName or "EGamma" in customize.datasetName:
    customize.customizeHLT(process)

process.source = cms.Source("PoolSource", fileNames=cms.untracked.vstring("root://cms-xrd-global.cern.ch/" + customize.fileNames[0]))
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(10000) )
