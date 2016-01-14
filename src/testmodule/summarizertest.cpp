#include "summarizertest.hpp"

#include "strus/attributeReaderInterface.hpp"
#include "strus/storageClientInterface.hpp"

#include "strus/private/errorUtils.hpp"

#include <boost/algorithm/string.hpp>
#include <string>
#include <sstream>
#include <iostream>

namespace test {

void SummarizerFunctionContextTest::addSummarizationFeature( const std::string &name,
			strus::PostingIteratorInterface *itr,
			const std::vector<strus::SummarizationVariable> &,
			float /*weight*/,
			const strus::TermStatistics &)
{
	try {
		if( boost::algorithm::iequals( name, "match" ) ) {
			m_itrs.push_back( itr );
		} else {
			m_errorhnd->report( _TXT( "unknown '%s' summarization feature '%s'" ), "test", name.c_str( ) );
		}
	}
	CATCH_ERROR_ARG1_MAP( _TXT( "error adding summarization feature to '%s' summarizer: %s" ), "test", *m_errorhnd );
}

std::vector<strus::SummarizerFunctionContextInterface::SummaryElement> SummarizerFunctionContextTest::getSummary( const strus::Index &docno )
{
	try {
		std::vector<SummaryElement> elems;

		m_attribreader->skipDoc( docno );
		std::string attr = m_attribreader->getValue( m_attribute );
		if( !attr.empty( ) ) { 
			elems.push_back( SummaryElement( attr ) );
		} else {
			elems.push_back( SummaryElement( "..." ) );
		}

		m_forwardIndex->skipDoc( docno );
		
		for( strus::Index pos = 0; pos < 100; pos++ ) {
			m_forwardIndex->skipPos( pos );
			elems.push_back( m_forwardIndex->fetch( ) );
		}

		//~ for( std::vector<strus::PostingIteratorInterface *>::const_iterator itr = m_itrs.begin( ); itr != m_itrs.end( ); itr++ ) {
			//~ if( (*itr)->skipDoc( docno ) == docno ) {
				//~ strus::Index pos = (*itr)->skipPos( 0 );
				//~ std::stringstream ss;
				//~ ss << "(" << docno << "," << pos << ")";
				//~ std::cout << ss.str( ) << std::endl;
				//~ elems.push_back( SummaryElement( ss.str( ) ) );
				//~ for( strus::Index pos = (*itr)->skipPos( 0 ); pos != 0; pos = (*itr)->skipPos( pos + 1 ) ) {
				//~ }
			//~ }
		//~ }
	
		return elems;
	}
	CATCH_ERROR_ARG1_MAP_RETURN( _TXT( "error fetching '%s' summary: %s" ), "test", *m_errorhnd, std::vector<strus::SummarizerFunctionContextInterface::SummaryElement>( ) );
}

	//~ try
	//~ {
		//~ std::vector<PostingIteratorInterface*>::const_iterator
			//~ ii = m_itrs.begin(), ie = m_itrs.end();
	
		//~ for (; ii != ie; ++ii)
		//~ {
			//~ if ((*ii)->skipDoc( docno) == docno)
			//~ {
				//~ unsigned int kk=0;
				//~ Index pos = (*ii)->skipPos( 0);
				//~ for (; pos && kk<m_maxNofMatches; ++kk,pos = (*ii)->skipPos( pos+1))
				//~ {
				//~ }
			//~ }
		//~ }
		//~ return rt;

std::string SummarizerFunctionInstanceTest::tostring() const
{
	try
	{
		std::ostringstream rt;
		rt << "attribute='" << m_attribute << "'";
		return rt.str();
	}
	CATCH_ERROR_ARG1_MAP_RETURN( _TXT( "error mapping '%s' summarizer function to string: %s" ), "test", *m_errorhnd, std::string( ) );
}


void SummarizerFunctionInstanceTest::addStringParameter( const std::string& name, const std::string& value )
{
	try {
		if( boost::algorithm::iequals( name, "match" ) ) {
			m_errorhnd->report( _TXT( "parameter '%s' for summarization function '%s' expected to be defined as feature and not as string or numeric value" ), name.c_str( ), "test" );
		}
		if( boost::algorithm::iequals( name, "attribute" ) ) {
			m_attribute = value;
		} else if( boost::algorithm::iequals( name, "type" ) ) {
			m_type = value;
		} else {
			m_errorhnd->report( _TXT( "unknown '%s' string summarization function parameter '%s'" ), "test", name.c_str( ) );
		}
	}
	CATCH_ERROR_ARG1_MAP( _TXT( "error adding string parameter to '%s' summarizer: %s"), "test", *m_errorhnd );
}

void SummarizerFunctionInstanceTest::addNumericParameter( const std::string& name, const strus::ArithmeticVariant& value )
{
	if( boost::algorithm::iequals( name, "attribute" ) ) {		
		m_errorhnd->report( _TXT( "no numeric value expected for parameter '%s' in summarization function '%s'"), name.c_str( ), "test" );
	} else {
		m_errorhnd->report( _TXT( "unknown '%s' numeric summarization function parameter '%s'" ), "test", name.c_str( ) );
	}
}

strus::SummarizerFunctionContextInterface *SummarizerFunctionInstanceTest::createFunctionContext( 
	const strus::StorageClientInterface* storage,
	strus::MetaDataReaderInterface* metadata,
	const strus::GlobalStatistics& stats ) const
{
	try {
		strus::AttributeReaderInterface *reader = storage->createAttributeReader( );
		if( !reader ) {
			m_errorhnd->explain( _TXT( "error creating context of 'test' summarizer: %s" ) );
			return 0;
		}
		return new SummarizerFunctionContextTest( storage, reader, m_attribute, m_type, m_errorhnd );
	}
	CATCH_ERROR_ARG1_MAP_RETURN( _TXT("error creating context of '%s' summarizer: %s"), "attribute", *m_errorhnd, 0);
}

strus::SummarizerFunctionInstanceInterface *SummarizerFunctionTest::createInstance( const strus::QueryProcessorInterface *query_processor ) const
{
	try {
		return new SummarizerFunctionInstanceTest( m_errorhnd );
	}
	CATCH_ERROR_ARG1_MAP_RETURN( _TXT( "error creating instance of '%s' summarizer: %s" ), "test", *m_errorhnd, 0 );
}

strus::SummarizerFunctionInterface::Description SummarizerFunctionTest::getDescription( ) const
{
	try {
		strus::SummarizerFunctionInterface::Description descr( _TXT( "Demonstrating how to implement a summarizer 'test'" ) );
		descr( strus::SummarizerFunctionInterface::Description::Param::Attribute, "attribute", _TXT( "an attribute parameter" ) );
		descr( strus::SummarizerFunctionInterface::Description::Param::Feature, "match", _TXT( "defines the query features to respect for summarizing"));
		return descr;
	}
	CATCH_ERROR_ARG1_MAP_RETURN( _TXT( "error creating summarizer function description for '%s': %s" ), "test", *m_errorhnd,
		strus::SummarizerFunctionInterface::Description( ) );
}

} // namespace test
