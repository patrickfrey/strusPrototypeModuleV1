#include "weightingtest.hpp"

#include "strus/private/errorUtils.hpp"

#include <boost/algorithm/string.hpp>
#include <string>
#include <sstream>

namespace test {

void WeightingFunctionContextTest::addWeightingFeature( const std::string& name,
	strus::PostingIteratorInterface* itr,
	float weight,
	const strus::TermStatistics& stats )
{
}

float WeightingFunctionContextTest::call( const strus::Index &docno )
{
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
	// we have no string parameters
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
	const strus::StorageClientInterface* storage_,
	strus::MetaDataReaderInterface* metadata,
	const strus::GlobalStatistics& stats ) const
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
		strus::WeightingFunctionInterface::Description descr( _TXT( "Demonstrating how to implement a weighting schema \"test\"" ) );
		descr( strus::WeightingFunctionInterface::Description::Param::Feature, "param", _TXT( "a dummy parameter" ) );
		return descr;
	}
	CATCH_ERROR_ARG1_MAP_RETURN( _TXT( "error creating weighting function description for '%s': %s" ), "test", *m_errorhnd,
		strus::WeightingFunctionInterface::Description( ) );
}

} // namespace test
