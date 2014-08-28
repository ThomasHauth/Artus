
#pragma once

//#include <boost/scoped_ptr.hpp>

#include <TH1.h>
#include "TROOT.h"

#include "Artus/Core/interface/Cpp11Support.h"
#include "Artus/Utility/interface/RootFileHelper.h"


template < class TTypes >
class TriggerResultsHistogramConsumer: public ConsumerBase< TTypes > {
public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;
	
	virtual std::string GetConsumerId() const
		ARTUS_CPP11_OVERRIDE
	{
		return "TriggerResultsHistogramConsumer";
	}

	virtual void Init(Pipeline<TTypes> * pipeline) ARTUS_CPP11_OVERRIDE
	{
		CutFlowConsumerBase<TTypes>::Init(pipeline);

		RootFileHelper::SafeCd(this->GetPipelineSettings().GetRootOutFile(),
		                       this->GetPipelineSettings().GetRootFileFolder());

		std::vector<std::string> triggerNames = this->GetPipelineSettings().GetHltPaths();
	
		for (std::string hltPath : triggerNames) {
			m_triggerResultHists.push_back(new TH1F(hltPath.c_str(), hltPath.c_str(),
		                                   triggerNames.size()+1, 0.0, triggerNames.size()+1.0));
		}


	}

	virtual void ProcessEvent(event_type const& event,
	                          product_type const& product,
	                          FilterResult & filterResult)
	{
	//	CutFlowConsumerBase<TTypes>::ProcessEvent(event, product, filterResult);
	//	
	//	// fill first bin of histograms with all events
	//	m_cutFlowUnweightedHist->Fill(float(bin));
	//
	//	// fill bins of histograms corresponding to passed filters
	//	FilterResult::FilterDecisions const& filterDecisions = filterResult.GetFilterDecisions();
	//	for(FilterResult::FilterDecisions::const_iterator filterDecision = filterDecisions.begin();
	//	    filterDecision != filterDecisions.end(); filterDecision++)
	//	{
	//		++bin;
	//		if (filterDecision->second == FilterResult::Decision::Passed) {
	//			m_cutFlowUnweightedHist->Fill(float(bin));
	//		
	//			if(m_addWeightedCutFlow) {
	//				m_cutFlowWeightedHist->Fill(float(bin), weight);
	//			}
	//		}
	//	}
	}

	virtual void Finish() ARTUS_CPP11_OVERRIDE {
		CutFlowConsumerBase<TTypes>::Finish();
		
		// save histograms
		RootFileHelper::SafeCd(this->GetPipelineSettings().GetRootOutFile(),
		                       this->GetPipelineSettings().GetRootFileFolder());
		
		for (TH1F hist : m_triggerResultHists) {
		hist->Write(hist->GetName());
		}
	
	}

protected:
	std::vector<TH1F*> m_triggerResultHists;

};
