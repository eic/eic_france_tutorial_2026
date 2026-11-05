---
title: Setup
---

In advance of the training session, please ensure that

1. You have singularity/apptainer or docker (on Mac) installed and working
   - On your local system:
     - systems that use cvmfs or download images can install **singularity or apptainer** (see the [apptainer quick-start guide](https://apptainer.org/docs/user/main/quick_start.html))
     - systems that download images can install **docker** (see the [Docker website](https://www.docker.com/))
   - You should at a minimum be able to run either of the following commands and open a shell:
     - `singularity run docker://alpine`
     - `docker run --rm -it alpine`
2. Download `eic-shell`:
   - Due to time and bandwidth limitations, this **should be downloaded to your desired system before the live tutorial**
   - From the system you use with singularity/apptainer or docker, make a folder called `eic`
   - From inside this directory:
     - Install by running the command:

       ```bash
       curl --location https://get.epic-eic.org | bash
       ```

     - **or** you can save the file at <https://get.epic-eic.org> as `install.sh` and run this script by hand:

       ```bash
       wget --output-document install.sh https://get.epic-eic.org
       bash install.sh
       ```
   - Depending upon how your filesystem is setup, you *may* need to specify a different temporary directory
     - By default, a directory in `/tmp` is used
     - Use the `-t` prefix to set a new temp directory
        - `./install.sh -t /home/...`
      - Setup requires ~20 GB of free space
           
::::::::::::::::::::::::::::::::::::: callout
### ROOT

It is **strongly preferred** (but not required) that you can run ROOT locally on your system. ROOT is installed within eic-shell, so you can process macros in eic-shell. However, viewing plots and files interactively in this manner is not straightforward. A local install to view interactive windows is generally very helpful. 

See the [CERN ROOT](https://root.cern/install/) webpage for information on installing ROOT in a variety of ways on your system if it is not already installed.

:::::::::::::::::::::::::::::::::::::

If you have completed the numbered steps above, then you are ready for the tutorial. Additional
information and training links are below:

- Basic familiarity with Unix shell and Git
  - Software Carpentry [Unix Shell](https://swcarpentry.github.io/shell-novice/) training (recommended)
- Basic familiarity with singularity and docker:
  - Software Carpentry [Incubator Singularity](https://carpentries-incubator.github.io/singularity-introduction/) training (optional)
  - HEP Software Foundation [Docker](https://hsf-training.github.io/hsf-training-docker/index.html) training (optional)

::::::::::::::::::::::::::::::::::::::::::::: callout
### JLab/BNL Systems

If you are using the [BNL SDCC](bnl-sdcc-quickstart.md) or the [JLab Farm](jlab-farm-quickstart.md),
you may find the quick start info useful if you are new to these systems. 

Singularity is provided by default on BNL SDCC and the JLab iFarm, check by trying the command `singularity` and verify that you get an info dump on the usage of the command. You can therefore skip step 1 of the instructions and just begin from "2. Download `eic-shell`:"...

:::::::::::::::::::::::::::::::::::::::::::::
