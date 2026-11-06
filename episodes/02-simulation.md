---
title: "Simulating ePIC Events"
teaching: 15 # teaching time in minutes
exercises: 15 # exercise time in minutes
---

:::::::::::::::::::::::::::::::::::::: questions 

- How can we use the simulation of the ePIC detector?
- What geometries are available?

::::::::::::::::::::::::::::::::::::::::::::::::

::::::::::::::::::::::::::::::::::::: objectives

- Process our event generator sample through the ePIC detector simulation
  
::::::::::::::::::::::::::::::::::::::::::::::::

## Introduction

The ePIC simulation is pre-installed within `eic-shell`. The ePIC geometry is defined in the DD4HEP (Detector Description for High Energy Physics) framework. More info can be found in 
the tutorials on [DD4HEP to define ePIC detectors ](https://eic.github.io/tutorial-geometry-development-using-dd4hep/) and [Using npsim and Geant4](https://eic.github.io/tutorial-simulations-using-npsim-and-geant4/) 

Crucially, DD4HEP acts as a wrapper around the familiar [Geant4](https://geant4.web.cern.ch/) Monte-Carlo simulation toolkit. 

### Introduction - Setup

To run a simulation, we need to enter `eic-shell` if we're not already there:

```bash
./eic-shell
```

Once inside, we need to load in a geometry. We can choose different options of we want, but we will use `epic-main`:


```bash
source /opt/detector/epic-main/bin/thisepic.sh
```

::::::::::::::::::::::::::::::::::::: callout
### Other Geometries

You can do

```bash
ls -lrth /opt/detector/
```
to view the other versions of the detector geometry available in `eic-shell`. These are tagged by month typically. They should have a corresponding `thisepic.sh` script you could source instead in their `/bin/` directory.

::::::::::::::::::::::::::::::::::::::::::::::::


Before we run anything, we'll make a directory to run in to keep things neat:

```bash
mkdir EIC_France_Simulations && cd EIC_France_Simulations
```

Copy your generated event file from earlier into this directory too. We're now ready to simulate! 

# Running Simulations

We will run our simulation using the `npsim` command. 

::::::::::::::::::::::::::::::::::::: callout
### npsim vs ddsim

In other tutorials, you may see reference to `ddsim` as well as `npsim`? Is this the same thing?

**Not quite**. They are very similar, but `npsim` effectively runs with a set of extra flags. Crucially, these flags **enable the simulation of optical photons**. As such, `npsim` should be used whenever optical photons are important for your output - e.g. in calorimetry or Cherenkov detectors. For rapid simulations of tracking, `ddsim` may be appropriate. Generally for full detector event simulations, use `npsim`.

::::::::::::::::::::::::::::::::::::::::::::::::

We can check the available flags and options with 

```bash
npsim -h
```

there's lots of options! We'll narrow down to a few:

- Flag 1
  - Info
- Flag 2
  - Info
- Flag 3
  - Info
- Flag 4
  - Info
- Flag 5
  - Info
 
::::::::::::::::::::::::::::::::::::: callout
### The Afterburner

We processed our simulation output "as is". In doing so, we've ignored the impact of beam effects on our initial and final state particles. We could add these effects in using the [afterburner](https://github.com/eic/afterburner/). This piece of software adds in a large combination of effects to our MC file, including the beam crossing angle, beam divergences and crab cavity impacts. The overall effect is to slightly alter components of the beam and final state particle momentum. For realistic physics simulations, we should generally apply the afterburner. Doing so is simple, within `eic-shell` we can simply run:

```bash
abconv -p <<preset>> <<Input_File>> -o <<Output_File>> 
```
where `<<preset>>` is the beam configuration preset to use and `<<Output_File>>` is the output file to write to (do not include a file suffix). Other options and flags are available, as usual, try `abconv -h`.

Note that the afterburner will try to automatically detect the beam configuration if the `-p` option is not used. Available presets can be seen from the output of `abconv -h`, but some are not printed by default. See [Lines 2110-2166](https://github.com/eic/afterburner/blob/740f6356225fc81c9baee5c76d6dd19976c99417/cpp/afterburner/EicConfigurator.cc#L2110) for the fill list of configurations available.

::::::::::::::::::::::::::::::::::::::::::::::::

::::::::::::::::::::::::::::::::::::: keypoints 

- Point 1

::::::::::::::::::::::::::::::::::::::::::::::::
