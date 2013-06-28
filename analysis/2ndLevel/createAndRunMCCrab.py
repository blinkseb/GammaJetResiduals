#! /usr/bin/env python

import os, datetime, pwd, re

from optparse import OptionParser
parser = OptionParser()
parser.add_option("", "--run", action="store_true", dest="run", default=False, help="run crab")
(options, args) = parser.parse_args()

datasets = [
    # Format
    #["dataset publish name", "dataset name", "Low pt hat", "High pt hat", "Number of processed events", "Cross section"]
    # Gamma + jets
    ["/G_Pt-30to50_TuneZ2star_8TeV_pythia6/sbrochet-G_Pt-30to50_START53_V7A_22Feb13-v1-31346b79deb97ac1b786d692cd650a21/USER", "G_Pt-30to50", 30, 50, 1993325, 19931.62],
    ["/G_Pt-50to80_TuneZ2star_8TeV_pythia6/sbrochet-G_Pt-50to80_START53_V7A_22Feb13-v1-31346b79deb97ac1b786d692cd650a21/USER", "G_Pt-50to80", 50, 80, 1995062, 3322.309],
    ["/G_Pt-80to120_TuneZ2star_8TeV_pythia6/sbrochet-G_Pt-80to120_START53_V7A_22Feb13-v1-31346b79deb97ac1b786d692cd650a21/USER", "G_Pt-80to120", 80, 120, 1992627, 558.2865],
    ["/G_Pt-120to170_TuneZ2star_8TeV_pythia6/sbrochet-G_Pt-120to170_START53_V7A_22Feb13-v1-31346b79deb97ac1b786d692cd650a21/USER", "G_Pt-120to170", 120, 170, 2000043, 108.0068],
    ["/G_Pt-170to300_TuneZ2star_8TeV_pythia6/sbrochet-G_Pt-170to300_START53_V7A_22Feb13-v1-31346b79deb97ac1b786d692cd650a21/USER", "G_Pt-170to300", 170, 300, 2000069, 30.12207],
    ["/G_Pt-300to470_TuneZ2star_8TeV_pythia6/sbrochet-G_Pt-300to470_START53_V7A_22Feb13-v1-31346b79deb97ac1b786d692cd650a21/USER", "G_Pt-300to470", 300, 470, 2000130, 2.138632],
    ["/G_Pt-470to800_TuneZ2star_8TeV_pythia6/sbrochet-G_Pt-470to800_START53_V7A_22Feb13-v1-31346b79deb97ac1b786d692cd650a21/USER", "G_Pt-470to800", 470, 800, 1975231, 0.2119244],
    ["/G_Pt-800to1400_TuneZ2star_8TeV_pythia6/sbrochet-G_Pt-800to1400_START53_V7A_22Feb13-v1-31346b79deb97ac1b786d692cd650a21/USER", "G_Pt-800to1400", 800, 1400, 1973504, 0.007077847],
    ["/G_Pt-1400to1800_TuneZ2star_8TeV_pythia6/sbrochet-G_Pt-1400to1800_START53_V7A_22Feb13-v1-31346b79deb97ac1b786d692cd650a21/USER", "G_Pt-1400to1800", 1400, 1800, 1984890, 4.510327E-5],
    ["/G_Pt-1800_TuneZ2star_8TeV_pythia6/sbrochet-G_Pt-1800_START53_V7A_22Feb13-v1-31346b79deb97ac1b786d692cd650a21/USER", "G_Pt-1800", 1800, 8000, 1939122, 1.867141E-6],

    # QCD
    #["/QCD_Pt-15to3000_TuneZ2star_Flat_8TeV_pythia6/sbrochet-QCD_Pt-15-3000_START53_V7A_22Feb13-v1-31346b79deb97ac1b786d692cd650a21/USER", "QCD_Pt-15-3000", -1, -1, 9991674, 2.99815997E10],
    ["/QCD_Pt_30_80_EMEnriched_TuneZ2star_8TeV_pythia6/sbrochet-QCD_Pt-30to80_START53_V7A_24Apr13-v1-31346b79deb97ac1b786d692cd650a21/USER", "QCD_Pt-30to80", 30, 80, 31660888, 7.433E7],
    ["/QCD_Pt_80_170_EMEnriched_TuneZ2star_8TeV_pythia6/sbrochet-QCD_Pt-80to170_START53_V7A_24Apr13-v1-31346b79deb97ac1b786d692cd650a21/USER", "QCD_Pt-80to170", 30, 170, 31070763, 1191000.0],
    ["/QCD_Pt_170_250_EMEnriched_TuneZ2star_8TeV_pythia6/sbrochet-QCD_Pt-170to250_START53_V7A_24Apr13-v1-31346b79deb97ac1b786d692cd650a21/USER", "QCD_Pt-170to250", 170, 250, 31053066, 30990.0],
    ["/QCD_Pt_250_350_EMEnriched_TuneZ2star_8TeV_pythia6/sbrochet-QCD_Pt-250to350_START53_V7A_06May13-v1-60ae21afeccdf6fcfeaa8da24f363022/USER", "QCD_Pt-250to350", 250, 350, 33091322, 4250.0],
    ["/QCD_Pt_350_EMEnriched_TuneZ2star_8TeV_pythia6/sbrochet-QCD_Pt-350_START53_V7A_06May13-v1-60ae21afeccdf6fcfeaa8da24f363022/USER", "QCD_Pt-350", 350, 8000, 33060562, 810.0],
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

  pt_hat_low = dataset[2]
  pt_hat_high = dataset[3]

  events = dataset[4]
  xsection = dataset[5]

  ui_working_dir = ("crab_MC_%s") % (dataset_name)
  output_file = "crab_MC_%s.cfg" % (dataset_name)
  output_dir = "JetMET/MC/%s/%s" % (d, dataset_name)

  print("Creating config file for '%s'" % (dataset_path))
  print("\tName: %s" % dataset_name)
  print("\tOutput directory: %s" % output_dir)
  print("")

  os.system("sed -e \"s#@datasetname@#%s#\" -e \"s#@uiworkingdir@#%s#g\" -e \"s#@outputdir@#%s#g\" -e \"s#@email@#%s#g\" -e \"s#@processed_events@#%d#g\" -e \"s#@cross_section@#%.10e#g\" -e \"s#@low_pt_hat@#%f#g\" -e \"s#@high_pt_hat@#%f#g\" crab_MC.cfg.template.ipnl > %s" % (dataset_path, ui_working_dir, output_dir, email, events, xsection, pt_hat_low, pt_hat_high, output_file))

  cmd = "crab -create -submit -cfg %s" % (output_file)
  if options.run:
    os.system(cmd)
