/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include "Artus/Core/interface/FactoryBase.h"

#include "TestTypes.h"

#include "TestGlobalProducer.h"
#include "TestLocalProducer.h"
#include "TestConsumer.h"
#include "Pipeline_t.h"

class TestFactory: public FactoryBase<TestTypes> {
public:

	virtual ~TestFactory() {
	}

	virtual TestProducerBase * createProducer(std::string const& id)
			ARTUS_CPP11_OVERRIDE
			{
		if (TestLocalProducer().GetProducerId() == id) {
			return new TestLocalProducer();
		} else if (TestLocalProducerFromGlobal().GetProducerId() == id) {
			return new TestLocalProducerFromGlobal();
		} else if (TestGlobalProducer().GetProducerId() == id) {
			return new TestGlobalProducer();
		} else if (TestGlobalProducer2().GetProducerId() == id) {
			return new TestGlobalProducer2();
		} else {
			LOG(FATAL) << "Producer with id " << id << " not found!";
		}

		return ARTUS_CPP11_NULLPTR;
	}

	virtual ConsumerBase<TestTypes> * createConsumer(std::string const& id)
			ARTUS_CPP11_OVERRIDE
			{
		if (TestConsumer().GetConsumerId() == id) {
			return new TestConsumer();
		} else if (TestConsumerLocalProduct().GetConsumerId() == id) {
			return new TestConsumerLocalProduct();
		} else {
			LOG(FATAL) << "Consumer with id " << id << " not found!";
		}

		return ARTUS_CPP11_NULLPTR;
	}

	virtual FilterBase<TestTypes> * createFilter(std::string const& id)
			ARTUS_CPP11_OVERRIDE
			{
		if (TestFilter().GetFilterId() == id) {
			return new TestFilter();
		} else if (TestFilter2().GetFilterId() == id) {
			return new TestFilter2();
		} else if (TestFilter3().GetFilterId() == id) {
			return new TestFilter3();
		} else if (TestGlobalFilter().GetFilterId() == id) {
			return new TestGlobalFilter();
		} else if (TestGlobalFilter2().GetFilterId() == id) {
			return new TestGlobalFilter2();
		} else {
			LOG(FATAL) << "Filter with id " << id << " not found!";
		}

		return ARTUS_CPP11_NULLPTR;
	}

};

