/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include "Artus/Core/interface/Cpp11Support.h"
#include <typeinfo>
#include <string>

template<class TTypes>
class TriggerObjectsProducer : public ProducerBase<TTypes> {
public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;

	virtual std::string GetProducerId() const {
		return "TriggerObjectsProducer";
	}

	virtual void Produce(event_type const& event, product_type& product,
	                     setting_type const& settings) const ARTUS_CPP11_OVERRIDE
	{
		std::cout << "Event"  << std::endl;

		for (std::vector<std::string>::const_iterator hltPath = settings.GetHltPaths().begin(); hltPath != settings.GetHltPaths().end(); ++hltPath)
		{
			std::string hltName = product.m_hltInfo->getHLTName(*hltPath);
			std::cout << "Trigger " << *hltPath << " " << hltName << " " << product.m_hltInfo->getPrescale(hltName) << std::endl;
			std::cout << "Trigger " << *hltPath << " " << hltName << "pos " << product.m_hltInfo->getHLTPosition(hltName) << std::endl;
			if (event.m_eventMetadata->hltFired(hltName, event.m_lumiMetadata)){
				std::cout << "Trigger fired: " << hltName << std::endl;
			}
		}
// 		for (std::vector<KDataLV>::const_iterator trgobj = event.m_triggerObjects->trgObjects.begin(); trgobj != event.m_triggerObjects->trgObjects.end(); trgobj++)
// 		{
// 			std::cout << trgobj->p4.Pt() << std::endl;
// 		}
//

		std::cout << "toIdxFilter size " << event.m_triggerObjects->toIdxFilter.size() << std::endl;
		std::cout << "toFilter size " << event.m_triggerInfos->toFilter.size() << std::endl;
		std::cout << "nFiltersPerHLT size " << event.m_triggerInfos->nFiltersPerHLT.size() << std::endl;
		std::cout << "nFiltersPerHLT " << event.m_triggerInfos->nFiltersPerHLT << std::endl;
		for (std::vector<std::vector<int> >::size_type i = 0; i != event.m_triggerObjects->toIdxFilter.size(); i++)
		{
			std::cout << "Filter: " << event.m_triggerInfos->toFilter[i] << std::endl;
			if (event.m_triggerObjects->toIdxFilter[i].size() == 0)
				continue;
			std::cout << "Filter: " << event.m_triggerInfos->toFilter[i] << " " << event.m_triggerInfos->toFilter[i].size() << std::endl;
			for (std::vector<int>::const_iterator toidxf = event.m_triggerObjects->toIdxFilter[i].begin(); toidxf != event.m_triggerObjects->toIdxFilter[i].end(); toidxf++){
				std::cout << event.m_triggerObjects->trgObjects[(*toidxf)].p4.Pt() << std::endl;
			}
		}
	}

};
