---
title: "Reconstructing our Simulated Output"
teaching: 15 # teaching time in minutes
exercises: 15 # exercise time in minutes
---

:::::::::::::::::::::::::::::::::::::: questions 

- What is EICrecon?
- What does EICrecon do?
- How do we process events through EICrecon?

::::::::::::::::::::::::::::::::::::::::::::::::

::::::::::::::::::::::::::::::::::::: objectives

- Process our simulated events through EICrecon
  
::::::::::::::::::::::::::::::::::::::::::::::::

## Introduction

As mentioned at the end of the last page, our simulation output itself isn't really that useful for physics analysis. We need to reconstruct our events first. In doing so, we will reconstruct things like **tracks** and **clusters** from our raw detector hit information. We can then assign these to particles for example. 

The ePIC software that does this step for us is called [EICrecon](https://github.com/eic/eicrecon). EICrecon is a [JANA](https://jeffersonlab.github.io/JANA2/#/) based reconstruction for the ePIC detector. We can process our simulation output through EICrecon to get reconstructed events that we can then analyse and interpret. As before, we'll see how we can run EICrecon and some common flags, then we'll process our output in full as we look at the sample output from a quick run.

::::::::::::::::::::::::::::::::::::::::::::::::

## Running EICrecon

::::::::::::::::::::::::::::::::::::: callout

### Callout

::::::::::::::::::::::::::::::::::::: keypoints 

- Point 1

::::::::::::::::::::::::::::::::::::::::::::::::
