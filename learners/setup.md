---
title: Setup
---

::::::::::::::::::::::::::::::::::::::::::::: callout

If you are using the [BNL SDCC](bnl-sdcc-quickstart.md) or the [JLab Farm](jlab-farm-quickstart.md),
you may find the quick start info useful if you are new to these systems.

:::::::::::::::::::::::::::::::::::::::::::::

In advance of the training session, please ensure that

1. You have a GitHub account ([sign up here](https://github.com/signup))
2. You have singularity/apptainer or docker (on Mac) installed and working
   - Provided by default on BNL SDCC and the JLab iFarm, check by trying the command `singularity` and verify that you get an info dump on the usage of the command
   - On your local system:
     - systems that use cvmfs or download images can install **singularity or apptainer** (see the [apptainer quick-start guide](https://apptainer.org/docs/user/main/quick_start.html))
     - systems that download images can install **docker** (see the [Docker website](https://www.docker.com/))
   - You should at a minimum be able to run either of the following commands and open a shell:
     - `singularity run docker://alpine`
     - `docker run --rm -it alpine`
3. Download `eic-shell`:
   - Due to time and bandwidth limitations, this should be downloaded to your desired system before the live tutorial
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

If you have completed the numbered steps above, then you are ready for the tutorial. Additional
information and training links are below:

- Basic familiarity with Unix shell and Git
  - Software Carpentry [Unix Shell](https://swcarpentry.github.io/shell-novice/) training (recommended)
  - Software Carpentry [Basic Git](https://swcarpentry.github.io/git-novice/) training (recommended)
- Basic familiarity with singularity and docker:
  - Software Carpentry [Incubator Singularity](https://carpentries-incubator.github.io/singularity-introduction/) training (optional)
  - HEP Software Foundation [Docker](https://hsf-training.github.io/hsf-training-docker/index.html) training (optional)
