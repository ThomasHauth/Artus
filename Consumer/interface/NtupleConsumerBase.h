/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include <boost/algorithm/string/join.hpp>

#include <TNtuple.h>

#include "Artus/Core/interface/ConsumerBase.h"
#include "Artus/Utility/interface/RootFileHelper.h"
#include "Artus/Utility/interface/Collections.h"
#include "Artus/Utility/interface/DefaultValues.h"

/*
Base class for all Consumers which output some form of Ntuple
*/

template<class TTypes>
class NtupleConsumerBase: public ConsumerBase<TTypes> {
public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;

	NtupleConsumerBase() : ConsumerBase<TTypes>() {
	}

	virtual ~NtupleConsumerBase() {
	}

	virtual void Init(Pipeline<TTypes> * pset) ARTUS_CPP11_OVERRIDE {
		ConsumerBase<TTypes>::Init(pset);

		m_quantitiesVector = pset->GetSettings().GetQuantities();
		m_quantities = boost::algorithm::join(m_quantitiesVector, ":");

		RootFileHelper::SafeCd(this->GetPipelineSettings().GetRootOutFile(),
		                       this->GetPipelineSettings().GetRootFileFolder());
		
		m_ntuple = new TNtuple("ntuple",
							   ("Ntuple for Pipeline \"" + this->GetPipelineSettings().GetName() + "\"").c_str(),
							   m_quantities.c_str());
	}

	virtual std::string GetConsumerId() const ARTUS_CPP11_OVERRIDE
	{
		return "ntuple";
	}

	virtual void ProcessFilteredEvent(event_type const& event,
			product_type const& product ) ARTUS_CPP11_OVERRIDE {
		ConsumerBase<TTypes>::ProcessFilteredEvent(event, product);

		// preallocated vector
		std::vector<float> array (m_quantitiesVector.size()) ;

		//iterate over string vector and fill the array for each quantity
		size_t arrayI = 0;
		for (stringvector::iterator it = m_quantitiesVector.begin(); it != m_quantitiesVector.end(); ++it) {
			array[ arrayI ] = returnvalue(*it, event, product);
			arrayI ++;
		}

		// add the array to the ntuple
		m_ntuple->Fill(&array[0]);
	}


	virtual void Finish() ARTUS_CPP11_OVERRIDE
	{
		RootFileHelper::SafeCd(this->GetPipelineSettings().GetRootOutFile(),
		                       this->GetPipelineSettings().GetRootFileFolder());
		
		m_ntuple->Write(m_ntuple->GetName());
	}


protected:
	TNtuple* m_ntuple;
	stringvector m_quantitiesVector;
	std::string m_quantities;


private:
	virtual float returnvalue(std::string string, event_type const& event,
			product_type const& product ) { return DefaultValues::UndefinedFloat; };
};


