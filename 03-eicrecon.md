---
title: "Reconstructing our Simulated Output"
teaching: 15
exercises: 15
---

:::::::::::::::::::::::::::::::::::::::::::::::::: questions

- What is EICrecon?
- What does EICrecon do?
- How do we process events through EICrecon?

::::::::::::::::::::::::::::::::::::::::::::::::::

:::::::::::::::::::::::::::::::::::::::::::::::::: objectives

- Process our simulated events through EICrecon

::::::::::::::::::::::::::::::::::::::::::::::::::

## Introduction

As mentioned at the end of the last page, our simulation output itself isn't really that useful for physics analysis. We need to reconstruct our events first. In doing so, we will reconstruct things like **tracks** and **clusters** from our raw detector hit information. We can then assign these to particles for example.

The ePIC software that does this step for us is called [EICrecon](https://github.com/eic/eicrecon). EICrecon is a [JANA](https://jeffersonlab.github.io/JANA2/#/) based reconstruction for the ePIC detector. We can process our simulation output through EICrecon to get reconstructed events that we can then analyse and interpret. As before, we'll see how we can run EICrecon and some common flags, then we'll process our output in full as we look at the sample output from a quick run.


## Running EICrecon

As with our simulation, `EICrecon` is installed within `eic-shell`. We can check the available options and flags via:

```bash
eicrecon -h
```

Again, when we run, we'll want to provide a few of these flags, namely:

- `-Ppodio:output_file`
  - Specify the file to output to, example usage
    - `-Ppodio:output_file=test_output.root`
- `-Pjana:nevents`
  - The number of events from our input file to run, example usage
    - `-Pjana:nevents=100` $\rightarrow$ run 100 events
- `-Pdd4hep:xml_files`
  - The detector configuration file that was used in the simulation and to base our reconstruction on. Example usage
    - `-Pdd4hep:xml_files=epic_craterlake.xml`
    - Note that by default, `EICrecon` will search for this file in `$DECTOR_PATH`, so we need to source our detector geometry before running `EICrecon` still

Our input file is specified as an argument without a flag. So as an example command to run our 10 event file from before, we would run:

```bash
eicrecon -Ppodio:output_file=Test_10_Events_Reco.root -Pjana:nevents=1000 -Pdd4hep:xml_files=epic_craterlake_9x130.xml Test_10_Events.edm4hep.root
```
again, once we run this, we'll get some configuration info dumped to screen and then it should run relatively quickly.

Let's look at the output

:::::::::::::::::::::::::::::::::::::::::::::::::: callout

### Time Saving (Again)

As before, due to how long the reconstruction will take, I recommend running a longer reconstruction in the background as we look at our small initial file.

```bash
eicrecon -Ppodio:output_file=1000_Events_Reco.root -Pjana:nevents=10 -Pdd4hep:xml_files=epic_craterlake_9x130.xml 1000_Events.edm4hep.root
```

::::::::::::::::::::::::::::::::::::::::::::::::::

### EICrecon Output File

:::::::::::::::::::::::::::::::::::::::::::::::::: keypoints

- Point 1

::::::::::::::::::::::::::::::::::::::::::::::::::
