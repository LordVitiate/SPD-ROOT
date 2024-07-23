#
# A basic image for FairSoft/FairRoot that will be installed under
# the /opt/ directory.

# For detailed explanation of each step see comments and dockerfile online
# reference.

# For FairSoft customization (well, some sort of), see the
# spd-fairsopft-${FAIRS_V}.conf file in the current dir.

#
# Base image
# Possible variants: cern/cc7-base, debian:stable, etc. (will be downloaded
# automatically from DockerHub). If you'll change the base image, take
# attention to the software installation section below as well (yum/apt-get
# install).
FROM fedora:27

#
# crank@qcrypt.org, renat.dusaev@cern.ch
MAINTAINER Renat Dusaev

#
# FairSoft/FairRoot versions variables
# The following versions are deprecated a little, up to the moment. One will
# probably wish to bump versions soon.
#   - FairSoft release version:
ENV FAIRS_V nov15p7
#   - FairRoot release version:
ENV FAIRR_V v-17.03a

#
# Dependencies for deb-family distros (Debian, Ubuntu, etc.):
#RUN apt-get update \
# && apt-get install -y cmake cmake-data g++ gcc gfortran \
#  debianutils build-essential make patch sed \
#  libx11-dev libxft-dev libxext-dev libxpm-dev libxmu-dev \
#  libglu1-mesa-dev libgl1-mesa-dev \
#  libncurses5-dev curl libcurl4-openssl-dev bzip2 libbz2-dev gzip unzip tar \
#  subversion git xutils-dev flex bison lsb-release python-dev \
#  libc6-dev-i386 libxml2-dev wget libssl-dev libkrb5-dev \
#  automake autoconf libtool \
# && apt-get clean

#
# Dependencies for RHEL-family distros (Fedora, CentOS, SLC) + extras.
RUN yum -y install cmake gcc gcc-c++ gcc-gfortran make patch \
  libX11-devel libXft-devel libXpm-devel libXext-devel \
  libXmu-devel mesa-libGLU-devel mesa-libGL-devel ncurses-devel \
  curl-devel bzip2 bzip2-devel unzip \
  expat-devel subversion git flex bison imake redhat-lsb-core python-devel \
  libxml2-devel wget openssl-devel krb5-devel \
  automake autoconf libtool which \
  \
  sudo doxygen xorg-x11-apps glx-utils xset mesa-dri-drivers \
 && yum clean all

# ^^^ extras:
# Install sudo app to perform superuser's operation from within a
# userspace. Install glxgears/xset apps for testing/benchmarking purposes, as
# a side effect makin yum to install a couple of important X11 libs (libXcursor,
# libXxf86misc, etc).

#
# The 'collector' is a default user in system, responsible for operations
# in userspace like configuring and building the software components.
RUN useradd --comment 'Source maintaining user.' \
    --create-home --home-dir /var/src \
    --user-group \
    --shell /bin/bash \
    collector \
 && echo "collector ALL=(ALL) NOPASSWD: ALL" > /etc/sudoers.d/collector \
 && chmod 0440 /etc/sudoers.d/collector

#
# Create the landing directories
RUN mkdir -p /opt/FairSoft.install/${FAIRS_V} \
 && mkdir -p /opt/FairRoot.install/${FAIRR_V} \
 && chown collector:collector /opt/FairSoft.install/${FAIRS_V}

#
# Deploy the FairSoft release version, cleaning up the unnecessary sources code
# data. Make a symlink in /opt referencing active version of FairSoft, and set
# the necessary SIMPATH environment variable.
# NOTE: the FairSoft's ./configure.sh does perform actual installation
# procedure (thus, being NOT a configuration routine)!

COPY spd-fairsoft-${FAIRS_V}.conf /var/src/

RUN cd /tmp \
 && chown collector:collector -R /var/src/ \
 && su collector -c "wget https://github.com/FairRootGroup/FairSoft/archive/${FAIRS_V}.tar.gz" \
 && su collector -c "tar xf ${FAIRS_V}.tar.gz -C /var/src" \
 && rm -f /tmp/${FAIRS_V}.tar.gz \
 && cd /var/src/FairSoft-${FAIRS_V} \
 && su collector -c "./configure.sh /var/src/spd-fairsoft-${FAIRS_V}.conf" \
 && cd /opt \
 && rm -rf /var/src/FairSoft-${FAIRS_V} \
 && ln -s FairSoft.install/${FAIRS_V} FairSoft \
 && chown root:root -R /opt/FairSoft.install/${FAIRS_V} \
 && rm /var/src/spd-fairsoft-${FAIRS_V}.conf

# The SIMPATH environment variable is necessary to FairRoot to install.
ENV SIMPATH /opt/FairSoft.install/${FAIRS_V}

#
# Deploy the FairRoot
# NOTE: for ancient distros (like CC7, the
# /opt/FairSoft.install/${FAIRS_V}/bin/cmake has to be placed instead of cmake.
# We intentionally do not modifying the PATH variable explicitly since it is
# considered as a poor style.
RUN cd /tmp \
 && su collector -c "wget https://github.com/FairRootGroup/FairRoot/archive/${FAIRR_V}.tar.gz" \
 && su collector -c "tar xf ${FAIRR_V}.tar.gz -C /var/src" \
 && rm ${FAIRR_V}.tar.gz \
 && su collector -c "mkdir /var/src/FairRoot-${FAIRR_V}/build" \
 && cd /var/src/FairRoot-${FAIRR_V}/build \
 && su collector -c "cmake -DCMAKE_INSTALL_PREFIX=/opt/FairRoot.install/${FAIRR_V} -DUSE_DIFFERENT_COMPILER=TRUE .." \
 && make \
 && make install \
 && cd ../.. \
 && rm -rf /var/src/FairRoot-${FAIRR_V} \
 && cd /opt \
 && ln -s FairRoot.install/${FAIRR_V} FairRoot \
 && chown root:root -R FairRoot.install/${FAIRR_V}

#
# Set defaults for ready image: default user, its working directory, and shared
# volume for placing the volatile data (e.g. source code of an active project):
USER collector
WORKDIR /var/src

# The /var/src volume is a home directory for 'collector' user and shall be
# considered as a default working catalogue for user sources. The Geant4 data
# dir is also exposed as a volume for integration purposes.
VOLUME ["/var/src" "/opt/FairSoft.install/nov15p7/share/Geant4-10.1.2"]

