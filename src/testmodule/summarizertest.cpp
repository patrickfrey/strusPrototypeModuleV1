/*
---------------------------------------------------------------------
    Demo and test modules for the search web service
    using the C++ library strus which implements basic operations
    to build a search engine for structured search on unstructured data.

    Copyright (C) 2013,2014 Patrick Frey
    Copyright (C) 2015,2016 Andreas Baumann
    Copyright (C) 2015,2016 Eurospider IT AG Zurich

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public
    License as published by the Free Software Foundation; either
    version 3 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

--------------------------------------------------------------------

	The latest version of strus can be found at 'http://github.com/patrickfrey/strus'
	For documentation see 'http://patrickfrey.github.com/strus'

--------------------------------------------------------------------
*/

#include "summarizertest.hpp"

#include "strus/attributeReaderInterface.hpp"
#include "strus/storageClientInterface.hpp"
#include "strus/arithmeticVariant.hpp"

#include "strus/private/errorUtils.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#include <string>
#include <sstream>
#include <queue>

#include <iostream>
#include <iomanip>

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

		// show a first "bonus" attribute (just for demonstrating the concept)
		if( m_attribute > 0 ) {
			m_attribreader->skipDoc( docno );
			std::string attr = m_attribreader->getValue( m_attribute );
			if( !attr.empty( ) ) { 
				elems.push_back( SummaryElement( attr ) );
			} else {
				elems.push_back( SummaryElement( "..." ) );
			}
		}
		
		// we also add a metadata feature (just for demonstrating the concept)
		if( m_metadata > 0 ) {
			m_metadatareader->skipDoc( docno );
			strus::ArithmeticVariant value = m_metadatareader->getValue( m_metadata );
			if( value.defined( ) ) {
				elems.push_back( SummaryElement( value.tostring( ).c_str( ) ) );
			} else {
				elems.push_back( SummaryElement( "n/a" ) );
			}
		}
		
		strus::Index first_pos;
		if( m_start_first_match ) {
			first_pos = 99999999;
			for( std::vector<strus::PostingIteratorInterface *>::const_iterator itr = m_itrs.begin( ); itr != m_itrs.end( ); itr++ ) {				
				if( (*itr)->skipDoc( docno ) == docno ) {
					strus::Index idxPos = (*itr)->skipPos( 0 );
					if( idxPos != 0 ) {
						first_pos = std::min( first_pos, idxPos );
					}
				}
			}
		} else {
			first_pos = 0;
			for( std::vector<strus::PostingIteratorInterface *>::const_iterator itr = m_itrs.begin( ); itr != m_itrs.end( ); itr++ ) {				
				(*itr)->skipPos( first_pos );
			}
		}

		// remember all feature positions for the top N positions
		std::priority_queue< strus::Index, std::vector<strus::Index>, std::greater<strus::Index> > positions;
		for( std::vector<strus::PostingIteratorInterface *>::const_iterator itr = m_itrs.begin( ); itr != m_itrs.end( ); itr++ ) {
			if( (*itr)->skipDoc( docno ) == docno ) {
				strus::Index idxPos = first_pos;
				while( idxPos != 0 && idxPos <= first_pos + m_N ) {
					positions.push( idxPos );
					idxPos = (*itr)->skipPos( idxPos + 1 );
				}
			}
		}
		
		// first position (for debugging)
		{
			std::stringstream ss;
			ss << boost::format( "first_pos: %1%" ) % first_pos;
			elems.push_back( ss.str( ) );
		}

		// skip in forward index to the right document
		m_forwardIndex->skipDoc( docno );

		// iterate through forward index up to position N, if we have found
		// a feature we must "highlight", we do so.
		strus::Index nextMarkPos = -1;
		if( !positions.empty( ) ) {
			nextMarkPos = positions.top( );
			positions.pop( );
		}
		for( strus::Index pos = first_pos; pos <= first_pos + m_N; pos++ ) {
			m_forwardIndex->skipPos( pos );
			std::string w = m_forwardIndex->fetch( );
			if( pos == nextMarkPos ) {
				std::stringstream ss;				
				ss << boost::format( m_mark ) % w;
				elems.push_back( ss.str( ) );
				while( !positions.empty( ) && nextMarkPos == pos ) {
					nextMarkPos = positions.top( );
					positions.pop( );
				}
			} else {
				elems.push_back( w );
			}
		}
	
		return elems;
	}
	CATCH_ERROR_ARG1_MAP_RETURN( _TXT( "error fetching '%s' summary: %s" ), "test", *m_errorhnd, std::vector<strus::SummarizerFunctionContextInterface::SummaryElement>( ) );
}

std::string SummarizerFunctionInstanceTest::tostring() const
{
	try
	{
		std::ostringstream rt;
		rt << "attribute='" << m_attribute << "', type='" << m_type << "', N=" << m_N;
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
		} else if( boost::algorithm::iequals( name, "metadata" ) ) {
			m_metadata = value;
		} else if( boost::algorithm::iequals( name, "type" ) ) {
			m_type = value;
		} else if( boost::algorithm::iequals( name, "mark" ) ) {
			m_mark = value;
		} else {
			m_errorhnd->report( _TXT( "unknown '%s' string summarization function parameter '%s'" ), "test", name.c_str( ) );
		}
	}
	CATCH_ERROR_ARG1_MAP( _TXT( "error adding string parameter to '%s' summarizer: %s"), "test", *m_errorhnd );
}

void SummarizerFunctionInstanceTest::addNumericParameter( const std::string& name, const strus::ArithmeticVariant& value )
{
	if( boost::algorithm::iequals( name, "N" ) ) {
		m_N = (unsigned int)value;
	} else if( boost::algorithm::iequals( name, "attribute" ) ) {		
		m_errorhnd->report( _TXT( "no numeric value expected for parameter '%s' in summarization function '%s'"), name.c_str( ), "test" );
	} else {
		m_errorhnd->report( _TXT( "unknown '%s' numeric summarization function parameter '%s'" ), "test", name.c_str( ) );
	}
}

void SummarizerFunctionInstanceTest::addBooleanParameter( const std::string& name, const bool& value)
{
	if( boost::algorithm::iequals( name, "start_first_match" ) ) {
		m_start_first_match = value;
	} else {
		m_errorhnd->report( _TXT("unknown '%s' boolean summarization function parameter '%s'"), "test", name.c_str());
	}
}

strus::SummarizerFunctionContextInterface *SummarizerFunctionInstanceTest::createFunctionContext( 
	const strus::StorageClientInterface* storage,
	strus::MetaDataReaderInterface* metadata,
	const strus::GlobalStatistics& stats ) const
{
	try {
		strus::AttributeReaderInterface *attributeReader = storage->createAttributeReader( );
		if( !attributeReader ) {
			m_errorhnd->explain( _TXT( "error creating context of 'test' summarizer: %s" ) );
			return 0;
		}
		return new SummarizerFunctionContextTest( storage, attributeReader, metadata, m_attribute, m_metadata, m_type, m_N, m_start_first_match, m_mark, m_errorhnd );
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
		descr( strus::SummarizerFunctionInterface::Description::Param::Metadata, "metadata", _TXT( "a metadata parameter" ) );
		descr( strus::SummarizerFunctionInterface::Description::Param::Feature, "match", _TXT( "defines the query features to respect for summarizing"));
		descr( strus::SummarizerFunctionInterface::Description::Param::Numeric, "N", _TXT( "maximal size of the abstract" ) );
		descr( strus::SummarizerFunctionInterface::Description::Param::String, "mark", _TXT( "how to mark a hit in boost format syntax with one parameter %1%" ) );
		descr( strus::SummarizerFunctionInterface::Description::Param::Boolean, "start_first_match", _TXT( "start with abstracting at the first match( default: first position of document)" ) );
		return descr;
	}
	CATCH_ERROR_ARG1_MAP_RETURN( _TXT( "error creating summarizer function description for '%s': %s" ), "test", *m_errorhnd,
		strus::SummarizerFunctionInterface::Description( ) );
}

} // namespace test
