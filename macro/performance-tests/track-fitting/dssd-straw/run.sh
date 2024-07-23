#!/bin/sh
spdroot.py -q -b "simu.C(10000)"
spdroot.py -q -b "reco.C()"
spdroot.py -q -b "ana.C()"
