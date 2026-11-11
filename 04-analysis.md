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
- Identify $J\psi$ particles and their decay products in the MCParticles collection
- Connect some reconstructed tracks and calorimeter clusters, identify potential electrons/positrons
- Try to find some reconstructed $J/\psi$'s

::::::::::::::::::::::::::::::::::::::::::::::::

## Introduction - EDM4EIC and PODIO

So far, we've looked at the files we've generated using ROOT. However, you might have noticed at one point that our file suffix included `edm4eic`, and indeed, when opening the file in ROOT you probably saw some reference to this. This is because the EIC software stack uses the [EDM4eic](https://github.com/eic/EDM4eic) (**E**vent **D**ata **M**odel for **EIC**).

This data model is defined in a single YAML file - [edm4eic.yaml](https://github.com/eic/EDM4eic/blob/main/edm4eic.yaml). There are further details (including a diagram of the model) in a [different tutorial](https://eic.github.io/tutorial-track-cluster-matching/02-podio.html).

[PODIO](https://github.com/AIDASoft/podio) is the toolkit which we utilise to generate and interface with this data model. PODIO - which stands for **P**lain-**O**ld-**D**ata **I**nput/**O**utput - reads a YAML data moel file and generates all of the C++ and Python code needed to work with it.

For the remainder of this tutorial, we're going to use PODIO to analyse our data. We will do so using C++ and ROOT, but note that you can also use Python and ROOT with only a few minor changes. 

This is acutally a slightly reworked segment of a broader tutorial from Derek Anderson on [Track-Cluster Matching](https://eic.github.io/tutorial-track-cluster-matching/index.html). For the python equivalent versions of the steps below, you could consult this tutorial.

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

:::::::::::::::::::::::::::::::::::::::::::::::::: callout

### Function and File Naming

If your file is **not** called `EIC_France_Analysis.cxx`, make sure you change the name of the void function.

Equally, the input and output filenames are **hardcoded** in this script. Modify the input to match your input filename and name the output as you like.

::::::::::::::::::::::::::::::::::::::::::::::::::

As is, this won't really do anything yet except make a new file, `EIC_France_Analysis_Out.root`, which should be blank. **But**, we can check it runs via:

```bash
root -b -q EIC_France_Analysis.cxx
```

if your paths are OK, this should run without issue (and do nothing except make a new empty file of course).

:::::::::::::::::::::::::::::::::::::::::::::::::: callout

### Execute in `eic-shell`

Unless you have `ed4hep` and `podio` installed locally, run this within `eic-shell`.

::::::::::::::::::::::::::::::::::::::::::::::::::

### Adding a Getter

A script that doesn't do anything isn't particularly useful (hey it could be worse, it could just crash). So let's make it a bit more useful. 

We'll start by adding a `getter` to get some info from our file.

```C++
 auto& MCparticles = frame.get<edm4hep::MCParticleCollection>("MCParticles");
```

add this any point after the frame is defined, within the event loop. We can add other collections in the same way as we want/need.

### Adding some helpers

We'll also add some helper functions that will be used a little later on:

```C++
// Convert edm4hep::Vector3f into a ROOT::Math::XZYVector
//   --> Useful for some vector calculations
auto convert_vector_float = [](const edm4hep::Vector3f& edm_vec) {
  return ROOT::Math::XYZVector(edm_vec.x, edm_vec.y, edm_vec.z);
};

// Convert edm4hep::Vector3d into a ROOT::Math::XZYVector
//   --> edm4hep stores momentum values as doubles, not floats
auto convert_vector_double = [](const edm4hep::Vector3d& edm_vec) {
  return ROOT::Math::XYZVector(edm_vec.x, edm_vec.y, edm_vec.z);
};

// Get 4-vector for a particle
//   --> Will need to compare rec vs. sim momentum/eta 
auto get_lorentz_particle = [](const edm4hep::MCParticle& particle) {
  return ROOT::Math::PxPyPzM4D(
    particle.getMomentum().x,
    particle.getMomentum().y,
    particle.getMomentum().z,
    particle.getMass()
  );
};

// Compare 2 object IDs
//   --> Needed for bookkeeping
auto compare_object_ids = [](const podio::ObjectID& lhs, const podio::ObjectID& rhs) {
  if (lhs.collectionID == rhs.collectionID) {
    return (lhs.index < rhs.index);
  } else {
    return (lhs.collectionID < rhs.collectionID);
  }
};
```

Add these after the getter we added above.

## MCParticles

Now that we've added a getter and our helper functions, we can actually try and get some info and do something with it. Our getter got the `MCParticles` collection for our frame (i.e. our event). These are our *actual* event output, our **truth** information. We can loop over all particles in this collection and create 4-vectors for the MCParticles in our event:

```C++
for (const auto& MCparticle : MCparticles) {
  const auto MCpart_lorentz = get_lorentz_particle(MCparticle);
  cout << MCparticle.getPDG() << "   " << MCpart_lorentz.Px() << endl;
}
```

Add this after all of our functions we've just added (still within the event loop). Now re-run the macro, we should now get some output.

### Histograms

We probably *don't* just want to print lots of information to screen though. More useful would be to make some plots of some sort, so let's do that. Let's fill some histograms with our information. To begin with, let's define a histogram:

```C++
TH1D* h1_MCPart_Px = new TH1D("h1_MCPart_Px", "P_{xMC};P_{xMC}/(GeV/c);Entries", 200, -1, 1);
```

Add this definition *before* the event loop begins. We've define a 1D histogram to fill with our MC particle $P_{x}$ values here. So let's then fill it with this information:

```C++
  h1_MCPart_Px->Fill(MCpart_lorentz.Px());
```

put this after the cout statement before (or replace the statement with it).

::::::::::::::::::::::::::::::::::::: challenge
### Challenge: Add some extra histograms

Add and fill the following histograms to the code:

1. Py, Pz, P and E
2. $\eta$
3. $\theta$
4. $\theta$ for **electrons** only

**Hint** - Think carefully about the number of bins and the binning range of these new histograms
:::::::::::::::::::::::::::::::::

### Beam Particles

Our MCParticles collection contains everything we fed into our simulation from our generated events. This *includes* the incoming electron and ion beam. We can easily identify these particles by using the `GeneratorStatus` value of our particles.

Beam particles will have a **generator status of 4.**

Our final state particles have a **generator status of 1**. Within our loop over MCParticles we can use:

```C++
cout << MCparticle.getGeneratorStatus() << endl;
```

to print the status value.


::::::::::::::::::::::::::::::::::::: challenge
### Challenge: Replot our histogramss

For the histograms we created and filled before, fill them only for **all non-beam particles** instead.

Make a separate set of histograms for **all beam particles** and fill them with the incoming electron and ion beam information.

:::::::::::::::::::::::::::::::::

### Identifying $J/\psi$ Particles **and** Daughters

So far, much of this probably looks like things we could probably do straightforwardly in ROOT without dealing with PODIO. We'll now look at a very quick demonstration of how we can use PODIO very nicely to pick out something of interest to us. We will use PODIO to quickly identify the decay products of $J/\psi$ particles:

```C++
  if (particle.getPDG() == 443) { // J/Psi is PDG 443
  std::vector<edm4hep::MCParticle> decays; // An array we will store our decay products in
  for (const auto& daughter : particle.getDaughters()) { // Loop over our J/Psi daughter particles
    decays.push_back(daughter); // Add the daughter particles to our vector

    const auto  daug_mom = convert_vector_double(particle.getMomentum());
    const float daug_eta = ele_mom.Eta();
    const float daug_mag = std::sqrt(ele_mom.Mag2());  // Mag() isn't implemented for XYZVector
    cout << daughter.getPDG() << "   " << daug_eta << endl;
  }
```

add this block after looping over particles in a frame. Now when we run our code, we should print out the PDG code and $\eta$ value for all of our produced $J/\psi$ daughters.

::::::::::::::::::::::::::::::::::::: challenge
### Challenge: Find some leptonic decays

Now that we have the block of code to pick out the daughters of the produced $J/Psi$, let's take a closer look. Add in:

- A histogram of the momentum, $P$, of all daughter particles
- A 2D histogram of $\eta$ for the decay products vs $P_{J/\psi}$ - the momentum of the initial $J/\psi$
- Identify only events where the $J/\psi$ decayed to an $e^{+}e^{-}$ pair
  - Create an alternative version of the histograms above that are filled *only* for $e^{+}e^{-}$ pairs
  - Create a 2D histogram of $P_{e^{+}}$ vs $P_{e^{-}}$ - the momentum of the positron versus the electron in the $J/\psi$ decay pair
  - Create a 2D histogram of $\eta_{e^{+}}$ vs $\eta_{e^{-}}$ - the $\eta$ of the positron versus the electron in the $J/\psi$ decay pair

:::::::::::::::::::::::::::::::::

:::::::::::::::::::::::::::::::::::::::::::::::::: callout

### `cout` lines

Remember to remove or comment out any `cout` lines if they're no longer useful to prevent a lot of printout to screen.

::::::::::::::::::::::::::::::::::::::::::::::::::

## Reconstructed Particles

After doing our simulation, we probably don't want to just know our MC truth information (indeed, we already knew this before simulating anything). We probably also want to look at our **reconstructed** information. Which of our events were reconstructed? What did we see in the event?

We will begin by adding some getters to get some **track** and calorimeter **cluster** information in the central detector. We'll then match up tracks and clusters before trying to identify electrons (or positrons) from them. First, the getters:

```C++
auto& tracks       = frame.get<edm4eic::TrackCollection>("CentralCKFTracks");
auto& clusters_neg = frame.get<edm4eic::ClusterCollection>("EcalEndcapNClusters");
auto& clusters_cen = frame.get<edm4eic::ClusterCollection>("EcalBarrelClusters");
auto& clusters_pos = frame.get<edm4eic::ClusterCollection>("EcalEndcapPClusters");
auto& associations = frame.get<edm4eic::MCRecoTrackParticleAssociationCollection>("CentralCKFTrackAssociations");
```

Add these in the same place as the existing getter for MCParticles. Now, two other helper functions to add:

```C++
// Get cluster 3-momentum using its position and energy
//   --> Assuming interaction vetex is at origin!
auto get_momentum = [](const ROOT::Math::XYZVector& position, const double energy) {
  return energy * position.Unit();
};

// Get distance in eta-phi space between 2 3-vectors
//   --> For matching tracks and clusters
auto get_distance = [](const ROOT::Math::XYZVector& vec_1, const ROOT::Math::XYZVector& vec_2) {
  return std::hypot(vec_1.Eta() - vec_2.Eta(), vec_1.Phi() - vec_2.Phi());
};
```
Add these alongside the helper functions that we started with. Now, we're going to loop over our tracks and try to see which (if any of them) match up to clusters in our central barrel EM calorimeter. We'll also initialise a vector to fill with any candidates we end up with:

```C++
std::vector<edm4eic::Track> rec_electrons; // An array we'll fill with our candidate elctrons/positrons
std::set<podio::ObjectID, decltype(compare_object_ids)> used_clusters;

// Begin our loop over tracks
for (const auto& track : tracks) {
  const auto  trk_mom = convert_vector_float(track.getMomentum()); // Take the track and turn it into a vector
  const float trk_eta = trk_mom.Eta();
  const float trk_mag = std::sqrt(trk_mom.Mag2());  // Mag() isn't implemented for XYZVectors

  float best_distance = 999.0;
  std::optional<edm4eic::Cluster> best_match;

  for (const auto& cluster : clusters_cen) {
    if (used_clusters.contains(cluster.getObjectID())) { // If we've already matched this cluster, skip it
      continue;
    }

    const auto  clust_pos = convert_vector_float(cluster.getPosition());
    const auto  clust_mom = get_momentum(clust_pos, cluster.getEnergy());
    const float distance = get_distance(trk_mom, clust_mom);
    if ((distance < match_cut) && (distance < best_distance)) {
      best_distance = distance;
      best_match    = cluster;
      used_clusters.insert(cluster.getObjectID());
    }
  } // End loop over central barrel EM calorimeter

  // Compare track vs. matched cluster, identify e- or e+ candidates ------------
  if (best_match.has_value()) { // Check something was actually matched
    cout << trk_mag << endl; // Print the track magnitude - could fill a hist in here :)
  }
  else{
    continue;
  }

  const float match_ene = best_match.value().getEnergy();
  const float trk_ep    = match_ene / trk_mag; // Calculate an E/p ratio for the track. For electrons, this should be close to 1
  if (trk_mag > 0.0) {
    cout << trk_ep << endl; // Print the E/p value out - Could fill a hist in here :)
  }
  else{
    continue;
  }

  if ((trk_ep > ep_min) && (trk_ep < ep_max)) {
    rec_electrons.push_back(track); // Add the e+ or e- candidate to our vector array
  }

} // End of loop over tracks
```

Add this block within our event loop (after the frame has been acquired and the getters set etc).

::::::::::::::::::::::::::::::::::::: challenge
### Challenge: Add the endcaps and some plots

Our block of code above only checks the barrel electromagnetic calorimeter. We added getters earlier for the endcap electromagnetic calorimeters too. Add routines that check the positive and negative end cap calorimeters for track/cluster matches.

Add some histograms and plot for all matched track/clusters in your events that look like electrons/positrons in the central detector:

- 1D histograms of:
  - The E/p ratio calculated
  - $\eta$
  - $P$
- 2D histograms of:
  - The E/p ratio vs $\eta$ 
  - $P$ vs $\eta$

:::::::::::::::::::::::::::::::::

### $J/\psi$ Candidates

Now that we've identified some potential tracks that *look* like electrons and positrons, we can try and combine them to see if any look like they could come from the decay of a $J/\psi$.

We'll do something similar to our clusters where we retain an array of clusters we've checked, and also define vector to store candidates in:

```C++
// Get 4-vector for a track with a certain mass
auto get_lorentz_track = [](const edm4eic::Track& track, const float mass) {
  return ROOT::Math::PxPyPzM4D(
    track.getMomentum().x,
    track.getMomentum().y,
    track.getMomentum().z,
    mass
    );
};
std::set<podio::ObjectID, decltype(compare_object_ids)> used_electrons;
std::vector<std::pair<edm4eic::Track, edm4eic::Track>> jpsi_candidates;

for (const auto& electron_1 : rec_electrons) {
  for (const auto& electron_2 : rec_electrons) {
    if (electron_1.getObjectID() == electron_2.getObjectID()) { // Don't try to combine things with themselves
      continue;
    }
    if (used_electrons.contains(electron_1.getObjectID())) { // If we've looked at our first electron, move on
      continue;
    }
    if (used_electrons.contains(electron_2.getObjectID())) { // If we've looked at our second electron, move on
      continue;
    }

    const auto lorentz_1 = get_lorentz_track(electron_1, 0.000511);
    const auto lorentz_2 = get_lorentz_track(electron_1, 0.000511);

    const ROOT::Math::PxPyPzE4D total_mom( // Sum up momentum components
      lorentz_1.Px() + lorentz_2.Px(),
      lorentz_1.Py() + lorentz_2.Py(),
      lorentz_1.Pz() + lorentz_2.Pz(),
      lorentz_1.E() + lorentz_2.E()
    );
    const float inv_mass = total_mom.M(); // Calculate the invariant mass

    if ((inv_mass > mass_min) && (inv_mass < mass_max)) {
      jpsi_candidates.push_back(
        {electron_1, electron_2} // If invariant mass of pair is within window, add candidate electron tracks to our candidate vector
      );
    used_electrons.insert(electron_1.getObjectID());
    used_electrons.insert(electron_2.getObjectID());
    }
  } // End loop over 2nd electron
} // End loop over 1st electron
```

::::::::::::::::::::::::::::::::::::: challenge
### Challenge: Plot invariant mass

For events where a $J/\psi$ candidate is found, fill a histogram with the calculated invariant mass from the $e^{+}e^{-}$ pair.

Create and fill a 2D histogram of $\eta$ vs $P$ for the candidate reconstructed $J/\psi$'s found with this routine.

:::::::::::::::::::::::::::::::::

::::::::::::::::::::::::::::::::::::: keypoints 

- Point 1

::::::::::::::::::::::::::::::::::::::::::::::::
