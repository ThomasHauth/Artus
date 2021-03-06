/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include "Artus/Core/interface/Cpp11Support.h"

#include "TraxTypes.h"

/*
 * IMPORTANT NOTE:
 * Not nummericaly stable. Don't use in real world applications
 * This is just to illustrate an easy consumer.
 */

class MeanPtConsumer: public TraxConsumerBase {
public:
	MeanPtConsumer() :
			m_itemCount(0), m_mean(0.0f) {

	}

	virtual std::string GetConsumerId() const ARTUS_CPP11_OVERRIDE {
		return "mean_pt";
	}

	virtual void ProcessFilteredEvent(TraxEvent const& event,
			TraxProduct const& product ) ARTUS_CPP11_OVERRIDE {

		m_mean = ((m_mean * m_itemCount) + event.m_floatPtSim)
				/ (m_itemCount + 1);

		m_itemCount++;

	}

	virtual void Finish() ARTUS_CPP11_OVERRIDE {
		// todo: print pipeline name here
		LOG(INFO) << "Track mean Pt is " << m_mean << " [GeV].";
	}

private:
	long long m_itemCount;
	double m_mean;
};
