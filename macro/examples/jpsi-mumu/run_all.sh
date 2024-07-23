#/bin/bash
spdroot.py -l -b -q "simu.C(100)"
spdroot.py -l -b -q reco.C
spdroot.py -l -b -q analyze_jpsi.C
