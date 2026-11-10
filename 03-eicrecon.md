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

Our output file now contains quite a few more collections. Again, many of these connect directly to specific detectors. Now though, these collections relate to reconstructed quantities such as calorimeter clusters or particle tracks. For example, `B0ECalClusters`, is a collection of information tied to clusters in the B0 Electromagnetic Calorimeter. We also have hit level information in some collections.

There are two new sets of collections we need to take closer look at though. We now have collections with `Associations` involved somewhere in the name. These collections exist to help us map between `reconstructed` info and `simulated` (i.e. MC truth) information. For simulated data, we can **know** which MC truth particle caused a particular track or cluster and map between the two. This is useful for understanding what has actually happened in our event and we will explore it further in the next part of our tutorial. However, note that for **real** experimental data, we **cannot** map between the truth and the reconstruction in this way. So we need to be careful when using this power, particularly if we try to make selections of data using this information.

The other new type of collection is `Reconstructed` particles or jets. These collections group together reconstructed detector information (tracks, clusters, PID etc) and present to us a set of candidates for each of our events. For example `ReconstructedParticles` is a collection of particles that `EICrecon` has constructed based upon the detector reconstruction in the central detector. Looking at this collection, we see info we could use to construct 4-vectors for particles in our event.

We can take a quick look at this information directly from the file with `TTree->Draw()` style commands. Open the file with:

```bash
root Test_10_Events_Reco.root
```

:::::::::::::::::::::::::::::::::::::::::::::::::: callout

### Opening Output Files

Remember to open and inspect the resulting ROOT output files in a local terminal environment **outside of `eic-shell`**. Interactive ROOT graphics and TBrowser windows typically require a local display environment to render properly.

::::::::::::::::::::::::::::::::::::::::::::::::::

then we can use:

```C++
events->Draw("ReconstructedParticles.QUANTITY")
```

to draw a quantity we want (e.g. try putting `energy` in place of `QUANTITY` . By default, ROOT will automatically draw this to a 1D histogram with 100 bins. These 100 bins will span the full range of available data. This is often **not** that useful. We can instead define a histogram ourselves to draw our selection to:

```C++
events->Draw("ReconstructedParticles.QUANTITY>>h1(NBins, xLow, xHigh)")
```

will define a new histogram, `h1` with `NBins` between `xLow` and `xHigh`. Most importantly, we can actually define and impose some selection cuts on the events we draw too. This does **not** have to be on the quantity we're drawing. For example, we could draw our quantity only for charged particles with:

```C++
events->Draw("ReconstructedParticles.QUANTITY>>h1(NBins, xLow, xHigh)", "ReconstructedParticles.charge!=0")
```

Finally, we could also even provide some draw options:

```C++
events->Draw("ReconstructedParticles.QUANTITY>>h1(NBins, xLow, xHigh)", "ReconstructedParticles.charge!=0", "HISTERR")
```

where `HISTERR` will draw our histogram as a histogram with error bars.

::::::::::::::::::::::::::::::::::::: challenge
### Challenge: Quick TTree->Draw() Analysis

From your file, draw a histogram of $P_{z}$ for **Recosntructed Particles** that have an energy **under 20 GeV** and are **negatively** charged.

Draw this histogram with error bars. Configure your number of bins and bin width appropriately (with 10 events this won't really matter, for 1000, it should start to matter a bit more).

:::::::::::::::::::::::::::::::::

:::::::::::::::::::::::::::::::::::::::::::::::::: keypoints

- EICrecon is the reconstruction software for ePIC and EIC simulations
- Our output is a PODIO/ROOT file
- We can quickly analyse and inspect the contents with `TTree->Draw()`
::::::::::::::::::::::::::::::::::::::::::::::::::
