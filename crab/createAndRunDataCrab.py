#! /usr/bin/env python

import os, datetime, pwd

from optparse import OptionParser
parser = OptionParser()
parser.add_option("", "--run", action="store_true", dest="run", default=False, help="run crab")
(options, args) = parser.parse_args()

global_json = "Cert_190456-208686_8TeV_PromptReco_Collisions12_JSON.txt"

datasets = [

    ["/Photon/Run2012A-13Jul2012-v1/AOD", "Photon_Run2012A-13Jul2012", "Cert_190456-196531_8TeV_13Jul2012ReReco_Collisions12_JSON_v2.txt", "FT_53_V6_AN3", [190456, 193621]],
    ["/Photon/Run2012A-recover-06Aug2012-v1/AOD", "Photon_Run2012A-recover-06Aug2012", "Cert_190782-190949_8TeV_06Aug2012ReReco_Collisions12_JSON.txt", "FT_53_V6C_AN3", [190782, 190949]],
    ["/SinglePhoton/Run2012B-13Jul2012-v1/AOD", "SinglePhoton_Run2012B-13Jul2012", "Cert_190456-196531_8TeV_13Jul2012ReReco_Collisions12_JSON_v2.txt", "FT_53_V6_AN3", [193833, 196531]],
    ["/SinglePhoton/Run2012C-24Aug2012-v1/AOD", "SinglePhoton_Run2012C-24Aug2012", "Cert_198022-198523_8TeV_24Aug2012ReReco_Collisions12_JSON.txt", "FT_53_V10_AN3", [198022, 198913]],
    ["/SinglePhoton/Run2012C-PromptReco-v2/AOD", "SinglePhoton_Run2012C-PromptReco", "", "GR_P_V41_AN3", [198934, 203746]],
    ["/SinglePhoton/Run2012C-EcalRecover_11Dec2012-v1/AOD", "SinglePhoton_Run2012C-EcalRecover_11Dec2012", "Cert_201191-201191_8TeV_11Dec2012ReReco-recover_Collisions12_JSON.txt", "FT_P_V42C_AN3", [201191, 201191]],
    ["/SinglePhoton/Run2012D-PromptReco-v1/AOD", "SinglePhoton_Run2012D-PromptReco", "", "GR_P_V42_AN3", [203768, 208686]],

    ]

# Get email address
email = "%s@ipnl.in2p3.fr" % (pwd.getpwuid(os.getuid()).pw_name)

d = datetime.datetime.now().strftime("%d%b%y")

version = 1

print("Creating configs for crab. Today is %s, you are %s and it's version %d" % (d, email, version))
print("")

for dataset in datasets:
  dataset_path = dataset[0]
  dataset_name = dataset[1]
  dataset_json = dataset[2]
  if len(dataset_json) == 0:
    dataset_json = global_json

  dataset_globaltag = dataset[3]

  runselection = ""
  if len(dataset) > 4 and len(dataset[4]) == 2:
    runselection = "runselection = %d-%d" % (dataset[4][0], dataset[4][1])

  publish_name = "%s_%s-v%d" % (dataset_name, d, version)
  ui_working_dir = ("crab_data_%s") % (dataset_name)
  output_file = "crab_data_%s.cfg" % (dataset_name)

  print("Creating config file for '%s'" % (dataset_path))
  print("\tName: %s" % dataset_name)
  print("\tJSON: %s" % dataset_json)
  print("\tGlobal tag: %s" % dataset_globaltag)
  print("\tRun selection: %s" % runselection)
  print("\tPublishing name: %s" % publish_name)
  print("")

  os.system("sed -e \"s#@datasetname@#%s#\" -e \"s#@uiworkingdir@#%s#g\" -e \"s#@lumi_mask@#%s#g\" -e \"s#@runselection@#%s#g\" -e \"s#@publish_data_name@#%s#g\" -e \"s#@email@#%s#g\" -e \"s#@globaltag@#%s#g\" crab_data.cfg.template.ipnl > %s" % (dataset_path, ui_working_dir, dataset_json, runselection, publish_name, email, dataset_globaltag, output_file))

  cmd = "crab -create -submit -cfg %s" % (output_file)
  if options.run:
    os.system(cmd)
