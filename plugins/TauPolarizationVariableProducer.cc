#include "FWCore/Framework/interface/global/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "DataFormats/Common/interface/View.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"
#include "DataFormats/JetReco/interface/GenJet.h"
#include "DataFormats/JetReco/interface/GenJetCollection.h"
#include "PhysicsTools/JetMCUtils/interface/JetMCTag.h"
#include "DataFormats/TauReco/interface/PFTau.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/PatCandidates/interface/Tau.h"

#include <vector>
#include <iostream>

class TauPolarizationVariableProducer : public edm::global::EDProducer<> 
{
public:
  TauPolarizationVariableProducer(const edm::ParameterSet& params) 
    : src_(consumes<pat::TauCollection>(params.getParameter<edm::InputTag>("src")))
  {
    produces<pat::TauCollection>(); 
  }
  
  ~TauPolarizationVariableProducer() override {}

  void produce(edm::StreamID id, edm::Event& evt, const edm::EventSetup& es) const override 
  {
    edm::Handle<pat::TauCollection> recoTaus;
    evt.getByToken(src_, recoTaus);

    auto newtau = std::make_unique<pat::TauCollection>();

    for (const auto & recoTau : *recoTaus) {

      double pt_neutral = 0;
      double pt_charged = 0;
      
      for(int ii = 0; ii < (int)recoTau.signalCands().size(); ii++){
	Int_t pdg = abs(recoTau.signalCands()[ii]->pdgId());
	Float_t candpt = recoTau.signalCands()[ii]->pt();
	
	if(pdg == 11 || pdg == 22 || pdg == 130) pt_neutral += candpt;
	if(pdg == 211){
	  if(candpt > pt_charged){
	    pt_charged = candpt;
	  }
	}
      }
      
      std::cout << "charged pt =" << pt_charged << ", neutral pt = " << pt_neutral << std::endl;
      
      pat::Tau _tau = recoTau;

      _tau.addUserFloat("ptsumcharged", pt_charged); 

      newtau->push_back(_tau);
   
    }

    evt.put(std::move(newtau));

  }

  static void fillDescriptions(edm::ConfigurationDescriptions & descriptions) {
    edm::ParameterSetDescription desc;
    desc.add<edm::InputTag>("src")->setComment("collection of visible gen taus (as reco::GenJetCollection)");
    descriptions.add("TauPolarizationVariableProducer", desc);
  }


private:
  const edm::EDGetTokenT<pat::TauCollection> src_;
};

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(TauPolarizationVariableProducer);

