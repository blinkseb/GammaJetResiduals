#! /bin/env python

import argparse, os, tempfile, shutil
from subprocess import call

triggers = [
    "HLT_Photon20_*"
    "HLT_Photon30_*",
    "HLT_Photon50_*",
    "HLT_Photon75_*",
    "HLT_Photon90_*",
    "HLT_Photon135_*",
    "HLT_Photon150_*",
    "HLT_Photon160_*",
    "HLT_Photon250_*",
    "HLT_Photon300_*"
    ]

parser = argparse.ArgumentParser(description='Generate PU profile for each prescaled triggers')
parser.add_argument('crab_json', nargs=1)
parser.add_argument('lumi_json', nargs=1)
args = parser.parse_args()

crab_json = args.crab_json[0]
lumi_json = args.lumi_json[0]

if not os.path.exists(crab_json):
  print("Error: %s not found" % crab_json)
  exit(1)

if not os.path.exists(lumi_json):
  print("Error: %s not found" % lumi_json)
  exit(1)

tempFolder = tempfile.mkdtemp(dir="/scratch")
#print(tempFolder)

for trigger in triggers:
  print("Generate PU profile for '%s'" % trigger)

  tempFilename = trigger.rstrip("_*")
  outputCSV = os.path.join(tempFolder, tempFilename + ".csv")
  outputJSON = os.path.join(tempFolder, tempFilename + "_JSON.txt")
  outputROOT = "pu_truth_data_photon_2012_true_%s_75bins.root" % tempFilename

  print("\tRunning pixelLumiCalc...")
  call(["pixelLumiCalc.py", "lumibyls", "-i", crab_json, "--hltpath", trigger, "-o", outputCSV])
  print("\tRunning pileupReCalc_HLTpaths.py...")
  call(["pileupReCalc_HLTpaths.py", "-i", outputCSV, "--inputLumiJSON", lumi_json, "-o", outputJSON])
  print("\tRunning pileupCalc...")
  call(["pileupCalc.py", "-i", crab_json, "--inputLumiJSON", outputJSON, "--calcMode", "true", "--minBiasXsec", "69400", "--maxPileupBin", "75", "--numPileupBins", "75", "--pileupHistName", "pileup", outputROOT, "--verbose"])
  print

shutil.rmtree(tempFolder)
