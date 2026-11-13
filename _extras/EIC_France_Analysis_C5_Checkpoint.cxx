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
// This code represents what your script should look like before challenge 5. Note that solutions for C1-4 are NOT included.
void EIC_France_Analysis_C5_Checkpoint(
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

    auto& MCparticles = frame.get<edm4hep::MCParticleCollection>("MCParticles");
    auto& tracks       = frame.get<edm4eic::TrackCollection>("CentralCKFTracks");
    auto& clusters_neg = frame.get<edm4eic::ClusterCollection>("EcalEndcapNClusters");
    auto& clusters_cen = frame.get<edm4eic::ClusterCollection>("EcalBarrelClusters");
    auto& clusters_pos = frame.get<edm4eic::ClusterCollection>("EcalEndcapPClusters");
    auto& associations = frame.get<edm4eic::MCRecoTrackParticleAssociationCollection>("CentralCKFTrackAssociations");
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

    for (const auto& MCparticle : MCparticles) {
      const auto MCpart_lorentz = get_lorentz_particle(MCparticle);
      if (MCparticle.getPDG() == 443) { // J/Psi is PDG 443
	
	const auto MCJPsi_mom = MCpart_lorentz;
	std::vector<edm4hep::MCParticle> decays; // An array we will store our decay products in
	for (const auto& daughter : MCparticle.getDaughters()) { // Loop over our J/Psi daughter particles
	  decays.push_back(daughter); // Add the daughter particles to our vector
	  
	  const auto  daug_mom = convert_vector_double(MCparticle.getMomentum());
	  const float daug_eta = daug_mom.Eta();
	  const float daug_mag = std::sqrt(daug_mom.Mag2());  // Mag() isn't implemented for XYZVector

	} // End loop over daughters
      } // end of J/Psi loop
    } // End MC particles loop
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

    // Check all of our reconstructed electron like objects and try to pair them together
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
	const auto lorentz_2 = get_lorentz_track(electron_2, 0.000511);

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

  }  // end event loop

  // Save output ===================================

  output->cd();
  output->Write();
  output->Close();
  return;

}
