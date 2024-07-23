#/bin/bash
spdroot.py -l -b -q "simu.C(1000)"
spdroot.py -l -b -q reco.C
spdroot.py -l -b -q analyze_chic.C
