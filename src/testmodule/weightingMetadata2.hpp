/*
 * Copyright (c) 2014 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_WEIGHTING_METADATA2_HPP_INCLUDED
#define _STRUS_WEIGHTING_METADATA2_HPP_INCLUDED
#include "strus/weightingFunctionInterface.hpp"
#include "strus/weightingFunctionInstanceInterface.hpp"
#include "strus/weightingFunctionContextInterface.hpp"
#include "strus/index.hpp"
#include "strus/numericVariant.hpp"
#include "strus/postingIteratorInterface.hpp"
#include "private/internationalization.hpp"
#include "private/utils.hpp"
#include <limits>
#include <vector>

namespace strus
{

/// \brief Forward declaration
class WeightingFunctionMetadata2;
/// \brief Forward declaration
class ErrorBufferInterface;


/// \class WeightingFunctionContextMetadata2
/// \brief Weighting function FunctionContext for the metadata weighting function
class WeightingFunctionContextMetadata2
	:public WeightingFunctionContextInterface
{
public:
	WeightingFunctionContextMetadata2(
			MetaDataReaderInterface* metadata_,
			const std::string& elementName1_,
			const std::string& elementName2_,
			double weight1_,
			double weight2_,
			ErrorBufferInterface* errorhnd_);

	virtual void addWeightingFeature(
			const std::string&,
			PostingIteratorInterface*,
			double/*weight*/,
			const TermStatistics&);

	virtual double call( const Index& docno);

	virtual std::string debugCall( const strus::Index& docno);

private:
	MetaDataReaderInterface* m_metadata;
	Index m_elementHandle1;
	Index m_elementHandle2;
	double m_weight1;
	double m_weight2;
	ErrorBufferInterface* m_errorhnd;				///< buffer for error messages
};

/// \class WeightingFunctionInstanceMetadata2
/// \brief Weighting function instance for a weighting that returns a metadata element for every matching document
class WeightingFunctionInstanceMetadata2
	:public WeightingFunctionInstanceInterface
{
public:
	explicit WeightingFunctionInstanceMetadata2( ErrorBufferInterface* errorhnd_)
		:m_weight1(1.0),m_weight2(1.0),m_errorhnd(errorhnd_){}

	virtual ~WeightingFunctionInstanceMetadata2(){}

	virtual void addStringParameter( const std::string& name, const std::string& value);

	virtual void addNumericParameter( const std::string& name, const NumericVariant& value);

	virtual WeightingFunctionContextInterface* createFunctionContext(
			const StorageClientInterface*,
			MetaDataReaderInterface* metadata_,
			const GlobalStatistics&) const;

	virtual std::string tostring() const;

private:
	double m_weight1;
	double m_weight2;
	std::string m_elementName1;
	std::string m_elementName2;
	ErrorBufferInterface* m_errorhnd;				///< buffer for error messages
};


/// \class WeightingFunctionMetadata2
/// \brief Weighting function that simply returns the value of a meta data element multiplied by a weight
class WeightingFunctionMetadata2
	:public WeightingFunctionInterface
{
public:
	explicit WeightingFunctionMetadata2( ErrorBufferInterface* errorhnd_)
		:m_errorhnd(errorhnd_){}
	virtual ~WeightingFunctionMetadata2(){}

	virtual WeightingFunctionInstanceInterface* createInstance(
			const QueryProcessorInterface* processor) const;

	virtual FunctionDescription getDescription() const;

private:
	ErrorBufferInterface* m_errorhnd;				///< buffer for error messages
};

}//namespace
#endif

