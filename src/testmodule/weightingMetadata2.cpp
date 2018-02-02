/*
 * Copyright (c) 2014 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "weightingMetadata2.hpp"
#include "strus/metaDataReaderInterface.hpp"
#include "strus/errorBufferInterface.hpp"
#include "strus/numericVariant.hpp"
#include "private/internationalization.hpp"
#include "private/errorUtils.hpp"
#include "private/utils.hpp"
#include <iostream>
#include <iomanip>

using namespace strus;

WeightingFunctionContextMetadata2::WeightingFunctionContextMetadata2(
		MetaDataReaderInterface* metadata_,
		const std::string& elementName1_,
		const std::string& elementName2_,
		double weight1_,
		double weight2_,
		ErrorBufferInterface* errorhnd_)
	:m_metadata(metadata_)
	,m_elementHandle1(metadata_->elementHandle(elementName1_))
	,m_elementHandle2(metadata_->elementHandle(elementName2_))
	,m_weight1(weight1_)
	,m_weight2(weight2_)
	,m_errorhnd(errorhnd_)
{}

void WeightingFunctionContextMetadata2::addWeightingFeature(
		const std::string&,
		PostingIteratorInterface*,
		double,
		const TermStatistics&)
{
	m_errorhnd->report( *strus::ErrorCode( strus::StrusComponentCore, strus::ErrorOperationBuildData, strus::ErrorCauseNotImplemented),
				_TXT("passing feature parameter to weighting function '%s' that has no feature parameters"), "metadata2");
}

void WeightingFunctionContextMetadata2::setVariableValue( const std::string& name, double value)
{
	m_errorhnd->report( *strus::ErrorCode( strus::StrusComponentCore, strus::ErrorOperationBuildData, strus::ErrorCauseNotImplemented),
				_TXT("no variables known for function '%s'"), "metadata2");
}

double WeightingFunctionContextMetadata2::call( const Index& docno)
{
	m_metadata->skipDoc( docno);
	return (double)( m_weight1 * (double)m_metadata->getValue( m_elementHandle1) +
		m_weight2 * (double)m_metadata->getValue( m_elementHandle2) );
}

std::string WeightingFunctionContextMetadata2::debugCall( const strus::Index& docno)
{
	return "n/a";
}

static NumericVariant parameterValue( const std::string& name, const std::string& value)
{
	NumericVariant rt;
	if (!rt.initFromString(value.c_str())) throw strus::runtime_error(_TXT("numeric value expected as parameter '%s' (%s)"), name.c_str(), value.c_str());
	return rt;
}

void WeightingFunctionInstanceMetadata2::addStringParameter( const std::string& name, const std::string& value)
{
	try
	{
		if (utils::caseInsensitiveEquals( name, "name1"))
		{
			m_elementName1 = value;
		}
		else if (utils::caseInsensitiveEquals( name, "name2"))
		{
			m_elementName2 = value;
		}
		else
		{
			addNumericParameter( name, parameterValue( name, value));
		}
	}
	CATCH_ERROR_ARG1_MAP( _TXT("error adding string parameter to weighting function '%s': %s"), "metadata2", *m_errorhnd);
}

void WeightingFunctionInstanceMetadata2::addNumericParameter( const std::string& name, const NumericVariant& value)
{
	try
	{
		if (utils::caseInsensitiveEquals( name, "weight1"))
		{
			m_weight1 = (double)value;
		}
		else if (utils::caseInsensitiveEquals( name, "weight2"))
		{
			m_weight2 = (double)value;
		}
		else if (utils::caseInsensitiveEquals( name, "name1") || utils::caseInsensitiveEquals( name, "name2"))
		{
			throw strus::runtime_error( _TXT("illegal numeric type for '%s' weighting function parameter '%s'"), "metadata2", name.c_str());
		}
		else
		{
			m_errorhnd->report( *strus::ErrorCode( strus::StrusComponentCore, strus::ErrorOperationBuildData, strus::ErrorCauseNotImplemented),
						_TXT("unknown '%s' weighting function parameter '%s'"), "metadata2", name.c_str());
		}
	}
	CATCH_ERROR_ARG1_MAP( _TXT("error adding numeric parameter to weighting function '%s': %s"), "metadata2", *m_errorhnd);
}

WeightingFunctionContextInterface* WeightingFunctionInstanceMetadata2::createFunctionContext(
		const StorageClientInterface*,
		MetaDataReaderInterface* metadata_,
		const GlobalStatistics&) const
{
	try
	{
		if (m_elementName1.empty())
		{
			m_errorhnd->report( *strus::ErrorCode( strus::StrusComponentCore, strus::ErrorOperationBuildData, strus::ErrorCauseIncompleteRequest),
						_TXT("undefined '%s' weighting function parameter '%s'"), "metadata2", "name1");
		}
		if (m_elementName2.empty())
		{
			m_errorhnd->report( *strus::ErrorCode( strus::StrusComponentCore, strus::ErrorOperationBuildData, strus::ErrorCauseIncompleteRequest),
						_TXT("undefined '%s' weighting function parameter '%s'"), "metadata2", "name2");
		}
		return new WeightingFunctionContextMetadata2( metadata_, m_elementName1, m_elementName2, m_weight1, m_weight2, m_errorhnd);
	}
	CATCH_ERROR_ARG1_MAP_RETURN( _TXT("error creating context of weighting function '%s': %s"), "metadata2", *m_errorhnd, 0);
}

std::string WeightingFunctionInstanceMetadata2::tostring() const
{
	try
	{
		std::ostringstream rt;
		rt << std::setw(2) << std::setprecision(5)
			<< "name1=" << m_elementName1 << ", weight1=" << m_weight1 << ", "
			<< "name2=" << m_elementName2 << ", weight2=" << m_weight2;
		return rt.str();
	}
	CATCH_ERROR_ARG1_MAP_RETURN( _TXT("error mapping weighting function '%s' to string: %s"), "metadata2", *m_errorhnd, std::string());
}


WeightingFunctionInstanceInterface* WeightingFunctionMetadata2::createInstance(
		const QueryProcessorInterface*) const
{
	try
	{
		return new WeightingFunctionInstanceMetadata2( m_errorhnd);
	}
	CATCH_ERROR_ARG1_MAP_RETURN( _TXT("error creating instance of weighting function '%s': %s"), "metadata2", *m_errorhnd, 0);
}


FunctionDescription WeightingFunctionMetadata2::getDescription() const
{
	try
	{
		typedef FunctionDescription::Parameter P;
		FunctionDescription rt( _TXT("Calculate the weight of a document as value of a meta data element."));
		rt( P::Metadata, "name1", _TXT( "name of the first meta data element to use as weight"), "");
		rt( P::Metadata, "name2", _TXT( "name of the second meta data element to use as weight"), "");
		rt( P::Numeric, "weight1", _TXT( "weight of the first meta data element to use as weight"), "");
		rt( P::Numeric, "weight2", _TXT( "weight of the second meta data element to use as weight"), "");
		return rt;
	}
	CATCH_ERROR_ARG1_MAP_RETURN( _TXT("error creating weighting function description for '%s': %s"), "metadata2", *m_errorhnd, FunctionDescription());
}

