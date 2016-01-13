#include "weightingtest.hpp"

#include "strus/private/errorUtils.hpp"
#include "strus/private/internationalization.hpp"

#include <boost/algorithm/string.hpp>
#include <string>
#include <sstream>

namespace test {

void WeightingFunctionContextTest::addWeightingFeature( const std::string& name,
	strus::PostingIteratorInterface* itr,
	float weight,
	const strus::TermStatistics& stats )
{
	try {
		if( boost::algorithm::iequals( name, "match" ) ) {
			// TODO: do something with the feature
		} else {
			throw strus::runtime_error( _TXT( "unknown '%s' weighting function feature parameter '%s'" ), "test", name.c_str( ) );
		}
	}
	CATCH_ERROR_ARG1_MAP( _TXT( "error adding weighting feature to '%s' weighting: %s"), "test", *m_errorhnd );
}

float WeightingFunctionContextTest::call( const strus::Index &docno )
{
	// TODO: the weight is always a constant no mather whether the document
	// actually contains the feature or not
	return m_param;
}

std::string WeightingFunctionInstanceTest::tostring() const
{
	try {
		std::ostringstream ss;
		ss << "param=" << m_param;
		return ss.str( );
	}
	CATCH_ERROR_ARG1_MAP_RETURN( _TXT( "error mapping '%s' weighting function to string: %s" ), "test", *m_errorhnd, std::string( ) );
}

void WeightingFunctionInstanceTest::addStringParameter( const std::string& name, const std::string& value )
{
	try {
		if( boost::algorithm::iequals( name, "match" ) ) {
			m_errorhnd->report( _TXT( "parameter '%s' for weighting function '%s' expected to be defined as feature and not as string or numeric value" ), name.c_str( ), "test" );
		}
		// we have no string parameters
		m_errorhnd->report( _TXT("unknown '%s' string weighting function parameter '%s'"), "test", name.c_str( ) );
	}
	CATCH_ERROR_ARG1_MAP( _TXT( "error adding string parameter to '%s' weighting: %s"), "test", *m_errorhnd );
}

void WeightingFunctionInstanceTest::addNumericParameter( const std::string& name, const strus::ArithmeticVariant& value )
{
	if( boost::algorithm::iequals( name, "param" ) ) {
		m_param = (double)value;
	} else {
		m_errorhnd->report( _TXT( "unknown '%s' numeric weighting function parameter '%s'" ), "test", name.c_str( ) );
	}
}

strus::WeightingFunctionContextInterface *WeightingFunctionInstanceTest::createFunctionContext( 
	const strus::StorageClientInterface *storage_,
	strus::MetaDataReaderInterface *metadata,
	const strus::GlobalStatistics &stats ) const
{
	try {
		return new WeightingFunctionContextTest( storage_, metadata, m_param, m_errorhnd );
	}
	CATCH_ERROR_ARG1_MAP_RETURN( _TXT( "error creating context of '%s' weighting function: %s" ), "test", *m_errorhnd, 0 );
}

strus::WeightingFunctionInstanceInterface* WeightingFunctionTest::createInstance( ) const
{
	try {
		return new WeightingFunctionInstanceTest( m_errorhnd );
	}
	CATCH_ERROR_ARG1_MAP_RETURN( _TXT( "error creating instance of '%s' function: %s" ), "test", *m_errorhnd, 0 );
}

strus::WeightingFunctionInterface::Description WeightingFunctionTest::getDescription( ) const
{
	try {
		strus::WeightingFunctionInterface::Description descr( _TXT( "Demonstrating how to implement a weighting schema 'test'" ) );
		descr( strus::WeightingFunctionInterface::Description::Param::Numeric, "param", _TXT( "a dummy parameter" ) );
		descr( strus::WeightingFunctionInterface::Description::Param::Feature, "match", _TXT( "defines the query features to weight"));
		return descr;
	}
	CATCH_ERROR_ARG1_MAP_RETURN( _TXT( "error creating weighting function description for '%s': %s" ), "test", *m_errorhnd,
		strus::WeightingFunctionInterface::Description( ) );
}

} // namespace test
