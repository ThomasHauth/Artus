
#pragma once

#include <algorithm>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/regex.hpp>

#include "CondFormats/JetMETObjects/interface/FactorizedJetCorrector.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"

#include "Kappa/DataFormats/interface/Kappa.h"

#define USE_JEC
#include "KappaTools/RootTools/JECTools.h"

#include "Artus/Core/interface/ProducerBase.h"
#include "Artus/Utility/interface/Utility.h"

/**
   \brief Producer for jet corrections (mainly JEC)
   
   Required config tags:
   - JetEnergyCorrectionParameters (files containing the correction parameters in the right order)
   - JetEnergyCorrectionUncertaintyParameters (default: empty)
   - JetEnergyCorrectionUncertaintyShift (default 0.0)
   
   Required packages (unfortunately, nobody knows a tag):
   git cms-addpkg CondFormats/JetMETObjects
   
   Documentation:
   https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookJetEnergyCorrections#JetEnCorFWLite
*/


template<class TTypes, class TJet>
class JetCorrectionsProducerBase: public ProducerBase<TTypes>
{

public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;
	
	JetCorrectionsProducerBase(std::vector<TJet>* event_type::*jets,
	                           std::vector<std::shared_ptr<TJet> > product_type::*correctedJets) :
		ProducerBase<TTypes>(),
		m_jetsMember(jets),
		m_correctedJetsMember(correctedJets)
	{
	}

	virtual void Init(setting_type const& settings) ARTUS_CPP11_OVERRIDE
	{
		ProducerBase<TTypes>::Init(settings);
		
		// load correction parameters
		LOG(DEBUG) << "\tLoading JetCorrectorParameters from files...";
		std::vector<JetCorrectorParameters> jecParameters;
		for (std::vector<std::string>::const_iterator jecParametersFile = settings.GetJetEnergyCorrectionParameters().begin();
		     jecParametersFile != settings.GetJetEnergyCorrectionParameters().end(); ++jecParametersFile)
		{
			jecParameters.push_back(JetCorrectorParameters(*jecParametersFile));
			LOG(DEBUG) << "\t\t" << *jecParametersFile;
		}
		factorizedJetCorrector = new FactorizedJetCorrector(jecParameters);
		
		// initialise uncertainty calculation
		LOG(DEBUG) << "\tLoading JetCorrectionUncertainty from files...";
		if (! settings.GetJetEnergyCorrectionUncertaintyParameters().empty())
		{
			jetCorrectionUncertainty = new JetCorrectionUncertainty(settings.GetJetEnergyCorrectionUncertaintyParameters());
			LOG(DEBUG) << "\t\t" << settings.GetJetEnergyCorrectionUncertaintyParameters();
		}
	}

	virtual void Produce(event_type const& event, product_type& product,
	                     setting_type const& settings) const ARTUS_CPP11_OVERRIDE
	{
		
		// create a copy of all jets in the event (first temporarily for the JEC)
		(product.*m_correctedJetsMember).clear();
		std::vector<TJet> correctJetsForJecTools((event.*m_jetsMember)->size());
		size_t jetIndex = 0;
		for (typename std::vector<TJet>::const_iterator jet = (event.*m_jetsMember)->begin();
			 jet != (event.*m_jetsMember)->end(); ++jet)
		{
			correctJetsForJecTools[jetIndex] = *jet;
			++jetIndex;
		}
		
		// apply jet energy corrections and uncertainty shift
		correctJets(&correctJetsForJecTools, factorizedJetCorrector, jetCorrectionUncertainty,
		            event.m_jetArea->median, event.m_vertexSummary->nVertices, -1,
		            settings.GetJetEnergyCorrectionUncertaintyShift());
		
		// create the shared pointers to store in the product
		(product.*m_correctedJetsMember).clear();
		(product.*m_correctedJetsMember).resize(correctJetsForJecTools.size());
		jetIndex = 0;
		for (typename std::vector<TJet>::const_iterator jet = correctJetsForJecTools.begin();
			 jet != correctJetsForJecTools.end(); ++jet)
		{
			(product.*m_correctedJetsMember)[jetIndex] = std::shared_ptr<TJet>(new TJet(*jet));
			++jetIndex;
		}
		
		// perform corrections on copied jets
		for (typename std::vector<std::shared_ptr<TJet> >::iterator jet = (product.*m_correctedJetsMember).begin();
			 jet != (product.*m_correctedJetsMember).end(); ++jet)
		{
			// No general correction available
		
			// perform possible analysis-specific corrections
			AdditionalCorrections(jet->get(), event, product, settings);
		}
		
		// sort vectors of corrected jets by pt
		std::sort((product.*m_correctedJetsMember).begin(), (product.*m_correctedJetsMember).end(),
		          [](std::shared_ptr<TJet> jet1, std::shared_ptr<TJet> jet2) -> bool
		          { return jet1->p4.Pt() > jet2->p4.Pt(); });
	}


protected:
	
	// Can be overwritten for analysis-specific use cases
	virtual void AdditionalCorrections(TJet* jet, event_type const& event,
	                                   product_type& product, setting_type const& settings) const
	{
	}


private:
	std::vector<TJet>* event_type::*m_jetsMember;
	std::vector<std::shared_ptr<TJet> > product_type::*m_correctedJetsMember;
	
	FactorizedJetCorrector* factorizedJetCorrector = 0;
	JetCorrectionUncertainty* jetCorrectionUncertainty = 0;
};



/**
   \brief Producer for Jet Energy Correction (JEC)
   
   Operates on the vector event.m_jets and product::m_correctedJets.
*/
template<class TTypes>
class JetCorrectionsProducer: public JetCorrectionsProducerBase<TTypes, KDataPFJet>
{
public:
	JetCorrectionsProducer() :
		JetCorrectionsProducerBase<TTypes, KDataPFJet>(&TTypes::event_type::m_jets,
		                                               &TTypes::product_type::m_correctedJets)
	{
	};
	
	virtual std::string GetProducerId() const ARTUS_CPP11_OVERRIDE {
		return "JetCorrectionsProducer";
	}
};



/**
   \brief Producer for Jet Energy Correction (JEC)
   
   Operates on the vector event.m_tjets and product::m_correctedTaggedJets.
*/
template<class TTypes>
class TaggedJetCorrectionsProducer: public JetCorrectionsProducerBase<TTypes, KDataPFTaggedJet>
{
public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;
	
	TaggedJetCorrectionsProducer() :
		JetCorrectionsProducerBase<TTypes, KDataPFTaggedJet>(&TTypes::event_type::m_tjets,
		                                                     &TTypes::product_type::m_correctedTaggedJets)
	{
	};
	
	virtual std::string GetProducerId() const ARTUS_CPP11_OVERRIDE {
		return "TaggedJetCorrectionsProducer";
	}
};


