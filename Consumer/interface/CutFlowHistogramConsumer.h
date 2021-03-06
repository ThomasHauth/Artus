
#pragma once

//#include <boost/scoped_ptr.hpp>

#include <TH1.h>
#include "TROOT.h"

#include "Artus/Core/interface/Cpp11Support.h"
#include "Artus/Utility/interface/RootFileHelper.h"

#include "Artus/Consumer/interface/CutFlowConsumerBase.h"


template < class TTypes >
class CutFlowHistogramConsumer: public CutFlowConsumerBase< TTypes > {
public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;
	
	typedef typename std::function<float(event_type const&, product_type const&)> weight_extractor_lambda;

	virtual std::string GetConsumerId() const
		ARTUS_CPP11_OVERRIDE
	{
		return "cutflow_histogram";
	}
	
	CutFlowHistogramConsumer() :
		CutFlowConsumerBase< TTypes >(),
		m_addWeightedCutFlow(false),
		m_histogramsInitialised(false)
	{
	}

	virtual void Init(Pipeline<TTypes> * pipeline) ARTUS_CPP11_OVERRIDE
	{
		CutFlowConsumerBase<TTypes>::Init(pipeline);
		
		// default weight = 1.0
		// overwrite this in analysis-specific code
		// and set m_addWeightedCutFlow to true
		weightExtractor = [](event_type const&, product_type const&) { return 1.0; };
	}

	virtual void ProcessEvent(event_type const& event,
	                          product_type const& product,
	                          FilterResult & filterResult)
	{
		CutFlowConsumerBase<TTypes>::ProcessEvent(event, product, filterResult);
		
		// initialise histograms in first event
		if(! m_histogramsInitialised) {
			m_histogramsInitialised = InitialiseHistograms(filterResult);
		}

		float weight = weightExtractor(event, product);
		int bin = 0;
		
		// fill first bin of histograms with all events
		m_cutFlowUnweightedHist->Fill(float(bin));
	
		if(m_addWeightedCutFlow) {
			m_cutFlowWeightedHist->Fill(float(bin), weight);
		}
		
		// fill bins of histograms corresponding to passed filters
		FilterResult::FilterDecisions const& filterDecisions = filterResult.GetFilterDecisions();
		for(FilterResult::FilterDecisions::const_iterator filterDecision = filterDecisions.begin();
		    filterDecision != filterDecisions.end(); filterDecision++)
		{
			++bin;
			if (filterDecision->second == FilterResult::Decision::Passed) {
				m_cutFlowUnweightedHist->Fill(float(bin));
			
				if(m_addWeightedCutFlow) {
					m_cutFlowWeightedHist->Fill(float(bin), weight);
				}
			}
		}
	}

	virtual void Finish() ARTUS_CPP11_OVERRIDE {
		CutFlowConsumerBase<TTypes>::Finish();
		
		// save histograms
		RootFileHelper::SafeCd(this->GetPipelineSettings().GetRootOutFile(),
		                       this->GetPipelineSettings().GetRootFileFolder());
		
		m_cutFlowUnweightedHist->Write(m_cutFlowUnweightedHist->GetName());
	
		if(m_addWeightedCutFlow) {
			m_cutFlowWeightedHist->Write(m_cutFlowWeightedHist->GetName());
		}
	}

protected:
	TH1F* m_cutFlowUnweightedHist = 0;
	TH1F* m_cutFlowWeightedHist = 0;
	weight_extractor_lambda weightExtractor;
	bool m_addWeightedCutFlow;

private:
	bool m_histogramsInitialised;
	
	// initialise histograms; to be called in first event
	bool InitialiseHistograms(FilterResult & filterResult) {

		// filters
		std::vector<std::string> filterNames = filterResult.GetFilterNames();
		int nFilters = filterNames.size();
	
		// histograms
		RootFileHelper::SafeCd(this->GetPipelineSettings().GetRootOutFile(),
		                       this->GetPipelineSettings().GetRootFileFolder());
		
		std::string cutFlowHistTitle("Cut Flow for Pipeline \"" + this->GetPipelineSettings().GetName() + "\"");
		
		m_cutFlowUnweightedHist = new TH1F("cutFlowUnweighted",
		                                   cutFlowHistTitle.c_str(),
		                                   nFilters+1, 0.0, nFilters+1.0);
	
		if(m_addWeightedCutFlow) {
			m_cutFlowWeightedHist = new TH1F("cutFlowWeighted",
			                                 cutFlowHistTitle.c_str(),
			                                 nFilters+1, 0.0, nFilters+1.0);
		}
	
		// names for bins
		int bin = 1;
	
		m_cutFlowUnweightedHist->GetXaxis()->SetBinLabel(bin, "without filters");
	
		if(m_addWeightedCutFlow) {
			m_cutFlowWeightedHist->GetXaxis()->SetBinLabel(bin, "without filters");
		}
	
		for(std::vector<std::string>::const_iterator filterName = filterNames.begin();
		    filterName != filterNames.end(); ++filterName)
		{
			++bin;
			m_cutFlowUnweightedHist->GetXaxis()->SetBinLabel(bin, filterName->c_str());
		
			if(m_addWeightedCutFlow) {
				m_cutFlowWeightedHist->GetXaxis()->SetBinLabel(bin, filterName->c_str());
			}
		}
	
		return true;
	}

};
