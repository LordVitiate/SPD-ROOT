#!/usr/bin/env python

import sys
import os

var = os.environ['VMCWORKDIR']

rootlogon = var+"/macro/rootlogon.C"
rootlogoff = var+"/macro/rootlogoff.C"

if len(sys.argv) > 1:
    os.system('root ' + ' '.join(sys.argv[1:-1])  + ' ' + rootlogon + ' "' + sys.argv[-1].replace('"', r'\"') + '" ' + rootlogoff)
