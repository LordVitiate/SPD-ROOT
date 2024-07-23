# Photon reconstruction
# The recommended number of events to simulate is 20000
spdroot.py -b -q "SimuEcal.C(1000)"
spdroot.py -b -q "RecoEventFull.C()" 
spdroot.py -b -q "CheckReconstruction.C()" 

# Pi0 reconstruction
spdroot.py -b -q "SimuEcal.C(1000, 111, \\\"runPi0.root\\\",\\\"paramsPi0.root\\\")" 
spdroot.py -b -q "RecoEventFull.C(\\\"runPi0.root\\\", \\\"recoPi0.root\\\", \\\"paramsPi0.root\\\")" 
spdroot.py -b -q "CheckReconstructionPi0.C()" 



