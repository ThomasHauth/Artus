
#pragma once

#include "Artus/Consumer/interface/CutFlowHistogramConsumer.h"
#include "Artus/Utility/interface/SafeMap.h"


/**
   \brief CutFlow consumer
   Config tags:
   - EventWeight, e.g. "eventWeight"
   
   Writes out cutflow histograms, one non-weighted and one weighted.
   The weight is taken from product.m_weights[settings.GetEventWeight()].
   If you wish a custom weight, derive from this (or its upper) class and
   fill the memember weightExtractor according to you requirements.
*/
template<class TTypes>
class KappaCutFlowHistogramConsumer: public CutFlowHistogramConsumer<TTypes> {

public:
	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	
	virtual void Init(Pipeline<TTypes> * pipeline) ARTUS_CPP11_OVERRIDE
	{
		CutFlowHistogramConsumer<TTypes>::Init(pipeline);
	
		this->weightExtractor = [pipeline](event_type const& event, product_type const& product) -> float {
			return SafeMap::GetWithDefault(product.m_weights, std::string(pipeline->GetSettings().GetEventWeight()), 1.0);
		};
	
		this->m_addWeightedCutFlow = true;
	}
};
