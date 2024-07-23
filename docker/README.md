NOTE: this is a draft document. Scenario may be broken! 19/02/018

# Docker Image for SPDRoot

The [dockerfile](https://docs.docker.com/engine/reference/builder/) document
is a special scenario format allowing one to build customized Docker images
for particular needs. The resulting image usually consists of fully-deployed
self-consistant software bundle clamping specific runtime environment for
smooth development, testing and production performance.

The copule of options are available to get the FairSoft/FairRoot container
image for SPD: retrieving the image from DockerHub, or building own image
in-place.

## Retrieving the image from DockerHub

Note, that fetching an image from DockerHub will require ~3Gb of external
traffic. Run:

    $ docker pull crankone/fairroot:17.03a

Omit the next section after successfull complete.

## Build own image

For the SPDRoot we have composed an image keeping the FairSoft/FairRoot
software environment with minimal contribution of collateral packages from
base layer (Fedora:27 <100mb).

To build the image, run:

    $ sudo docker build -t fairroot:latest .

in the current dir. Once the image will be built (may take a while since
FairSoft unconditionally builds heavy packages like Boost or ROOT), you may
get into the containerized environment with:

    $ sudo docker -it fairroot:latest /bin/bash

# Work with the Containers

Further working process will most probably imply frequent updates of SPDRoot
code base, so it is reasonable to keep the SPDRoot source
code on the host system, in the local dir. You may use your cloned SPDRoot git
repo (say `/home/me/projects/spdroot`) as is, using your usual tools for code
management and editing by launching the container with following snippet:

    $ sudo docker run -v /home/me/projects/spdroot:/var/src \
                      -it fairroot:latest /bin/bash

This command will "mount" your local dir `/home/me/projects/spdroot` to the
container's `/var/src` (default place for active source code project).

For X11 applications one may additionally supply the `-e DISPLAY=unix$DISPLAY`
argument to `docker run` command.

It is not necessary to get into the shell to run applications in the container.
For convinience we ship the small shell script forwarding the documents into
the container environment:

- For interactive root session:
    $ sudo ./do.run root
- To run the script from `macro/` dir:
    $ sudo ./do.run root -x macro/SimuTor.C

If you're not complain too much about the security, one may safely add the
user into "docker" group to omit the `sudo` command.
