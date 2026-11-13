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
// This code represents what your code should look like before challenge 3. Note that this does NOT include solutions for C1 and C2
void EIC_France_Analysis_C3_Checkpoint(
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

    for (const auto& MCparticle : MCparticles) {
      const auto MCpart_lorentz = get_lorentz_particle(MCparticle);
      if (MCparticle.getPDG() == 443) { // J/Psi is PDG 443
	std::vector<edm4hep::MCParticle> decays; // An array we will store our decay products in
	for (const auto& daughter : MCparticle.getDaughters()) { // Loop over our J/Psi daughter particles
	  decays.push_back(daughter); // Add the daughter particles to our vector

	  const auto  daug_mom = convert_vector_double(MCparticle.getMomentum());
	  const float daug_eta = daug_mom.Eta();
	  const float daug_mag = std::sqrt(daug_mom.Mag2());  // Mag() isn't implemented for XYZVector
	  cout << daughter.getPDG() << "   " << daug_eta << endl;
	}
      }

    } // End MC particles loop
    
  }  // end event loop

  // Save output ===================================

  output->cd();
  output->Write();
  output->Close();
  return;

}
