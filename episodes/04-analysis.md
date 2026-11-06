---
title: "Analysing Reconstructed Output"
teaching: 45 # teaching time in minutes
exercises: 45 # exercise time in minutes
---

:::::::::::::::::::::::::::::::::::::: questions 

- How can we analyse our reconstructed output?
- How can we identify a $J/\psi$?
- How do we know our reconstructed particles have been selected correctly?

::::::::::::::::::::::::::::::::::::::::::::::::

::::::::::::::::::::::::::::::::::::: objectives

- Create a script to open our reconstructed output
- Identify our beam particles from the MCParticles collection
- Identify $J\psi$ particles from their decay products in the MCParticles collection
- Connect some of our MCParticles to ReconstructedParticles with associations

::::::::::::::::::::::::::::::::::::::::::::::::

## Introduction - EDM4EIC and PODIO

So far, we've looked at the files we've generated using ROOT. However, you might have noticed at one point that our file suffix included `edm4eic`, and indeed, when opening the file in ROOT you probably saw some reference to this. This is because the EIC software stack uses the [EDM4eic](https://github.com/eic/EDM4eic) (**E**vent **D**ata **M**odel for **EIC**).

This data model is defined in a single YAML file - [edm4eic.yaml](https://github.com/eic/EDM4eic/blob/main/edm4eic.yaml). There are further details (including a diagram of the model) in a [different tutorial](https://eic.github.io/tutorial-track-cluster-matching/02-podio.html).

[PODIO](https://github.com/AIDASoft/podio) is the toolkit which we utilise to generate and interface with this data model. PODIO - which stands for **P**lain-**O**ld-**D**ata **I**nput/**O**utput - reads a YAML data moel file and generates all of the C++ and Python code needed to work with it.

For the remainder of this tutorial, we're going to use PODIO to analyse our data. We will do so using C++ and ROOT, but note that you can also use Python and ROOT with only a few minor changes.

### Script Skeleton

To begin with, open a new file, call it whatever you like, but for my example, I'm opening `EIC_France_Analysis.cxx`. Copy in the code below:

```C++
#include <edm4eic/ClusterCollection.h>
#include <edm4eic/MCRecoTrackParticleAssociationCollection.h>
#include <edm4eic/TrackCollection.h>
#include <edm4hep/MCParticleCollection.h>
#include <edm4hep/Vector3f.h>
#include <podio/Frame.h>
#include <podio/ObjectID.h>
#include <podio/ROOTReader.h>
#include <Math/Vector3D.h>
#include <Math/Vector4D.h>
#include <TFile.h>
#include <TH1.h>
#include <cmath>
#include <optional>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>
#include <utility>

TH1::SetDefaultSumw2(kTRUE);
```

This block just define various files to include, next, paste in:

```C++
void EIC_France_Analysis(
  const std::string& in_file   = "1000_Events_Reco.root",
  const std::string& out_file  = "EIC_France_Analysis_Out.root",
  const float        match_cut = 0.2,
  const float        ep_min    = 0.8,
  const float        ep_max    = 1.2,
  const float        mass_min  = 2.5,
  const float        mass_max  = 3.5
) {

  // Open reader and output file ==============================================

  podio::ROOTReader reader = podio::ROOTReader();
  reader.openFile(in_file);

  TFile* output = new TFile(out_file.data(), "recreate");

  // Event loop ===============================================================

  while (auto entry = reader.readNextEntry(podio::Category::Event)) {

    auto frame = podio::Frame(std::move(entry));
  }  // end event loop

  // Calculate efficiencies and save output ===================================

  output->cd();
  output->Write();
  output->Close();
  return;

}
```

:::::::::::::::::::::::::::::::::::::::::::::::::: warning

### Function and File Naming

If your file is **not** called `EIC_France_Analysis.cxx`, make sure you change the name of the void function.

Equally, the input and output filenames are **hardcoded** in this script. Modify the input to match your input filename and name the output as you like.

::::::::::::::::::::::::::::::::::::::::::::::::::

As is, this won't really do anything yet. **But**, we can check it runs via:

```bash
root -b -q EIC_France_Analysis.cxx
```

if your paths are OK, this should run without issue (and do nothing of course),

:::::::::::::::::::::::::::::::::::::::::::::::::: warning

### Execute in `eic-shell`

Unless you have `ed4hep` and `podio` installed locally, run this within `eic-shell`.

::::::::::::::::::::::::::::::::::::::::::::::::::

### Adding a Getter

A script that doesn't do anything isn't particularly useful (hey it could be worse, it could just crash). So let's make it a bit more useful. 

We'll start by adding a `getter` to get some info from our file.

## MCParticles

### Identifying $J/\psi$ Particles

## Associations

::::::::::::::::::::::::::::::::::::: keypoints 

- Point 1

::::::::::::::::::::::::::::::::::::::::::::::::
