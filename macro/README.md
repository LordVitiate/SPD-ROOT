This file describes the most important files in this directory.

# `rootlogon.C` and `rootlogoff.C`.

These files are needed to load necessary dynamic libraries and headers before running your script or properly exiting the ROOT session. The `spdroot.py` script is a wrapper that runs `rootlogon.C` first, then your scripts, and finally `rootlogoff.C`. You can copy them to your working directory and use `root' instead.

# Simulation

Historically, the reference simulation file is called `SimQslPy8.C`. This is the only simulation file that is kept up to date. You can find a Pythia6 usage example in `SimuQslPy6.C`, but the detector description there is outdated.

# Reconstruction

Historically, the reference reconstruction file is `analysis/RecoEventFull.C`. Similar to `SimuQslPy8.C`, this is the only file that is kept up to date.

# Performance tests

The `performance-tests` folder contains maintained simulation files and reference numbers for detector performance.

# Other Files

The other files can be useful for various purposes, such as visualizing detectors, checking radiation length, etc., but they are mostly not maintained. You can use them as a starting point for your studies.