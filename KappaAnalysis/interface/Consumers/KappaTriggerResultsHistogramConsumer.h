
#pragma once

//#include <boost/scoped_ptr.hpp>

#include <TH1.h>
#include "TROOT.h"

#include "Artus/Core/interface/Cpp11Support.h"
#include "Artus/Utility/interface/RootFileHelper.h"

#include "Artus/Consumer/interface/LambdaNtupleConsumer.h"


template < class TTypes >
class KappaTriggerResultsHistogramConsumer: public ConsumerBase< TTypes > {
public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;
	
	virtual std::string GetConsumerId() const
		ARTUS_CPP11_OVERRIDE
	{
		return "KappaTriggerResultsHistogramConsumer";
	}

	virtual void Init(Pipeline<TTypes> * pipeline) ARTUS_CPP11_OVERRIDE
	{
		ConsumerBase<TTypes>::Init( pipeline );
		m_triggerPaths = this->GetPipelineSettings().GetHltPaths();
	
		RootFileHelper::SafeCd(this->GetPipelineSettings().GetRootOutFile(),
		                       this->GetPipelineSettings().GetRootFileFolder());

		for (std::string hltPath : m_triggerPaths) {
			m_triggerResultHists.push_back(new TH1F(hltPath.c_str(), hltPath.c_str(), 100., 0., 1000.));
		}


	}

	virtual void ProcessFilteredEvent(event_type const& event,
	                          product_type const& product)
	{
		for(std::vector<std::string>::size_type i = 0; i != m_triggerPaths.size(); i++) {
			if (event.m_eventMetadata->hltFired(product.m_hltInfo->getHLTName(m_triggerPaths[i]), event.m_lumiMetadata)){
				m_triggerResultHists[i]->Fill((LambdaNtupleConsumer<TTypes>::Quantities[this->GetPipelineSettings().GetTriggerEfficiencyQuantity()])(event, product));
			}
		}
	}

	virtual void Finish() ARTUS_CPP11_OVERRIDE {
		
		// save histograms
		RootFileHelper::SafeCd(this->GetPipelineSettings().GetRootOutFile(),
		                       this->GetPipelineSettings().GetRootFileFolder());
		
		for (TH1F* hist : m_triggerResultHists) {
			hist->Write(hist->GetName());
		}
	
	}

protected:
	std::vector<std::string> m_triggerPaths;
	std::vector<TH1F*> m_triggerResultHists;

};
