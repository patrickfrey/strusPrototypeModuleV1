/*
 * Copyright (c) 2014 Patrick P. Frey
 * Copyright (c) 2015,2016 Andreas Baumann
 * Copyright (c) 2015,2016 Eurospider IT AG Zurich
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "summarizertest2.hpp"

#include "strus/attributeReaderInterface.hpp"
#include "strus/storageClientInterface.hpp"
#include "strus/numericVariant.hpp"

#include "strus/private/errorUtils.hpp"

#include "strus/constants.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#include <string>
#include <sstream>
#include <queue>
#include <set>

#include <iomanip>

#undef LOWLEVEL_DEBUG

#ifdef LOWLEVEL_DEBUG
#include <iostream>
#endif

namespace test {

void SummarizerFunctionContextTest2::addSummarizationFeature( const std::string &name,
			strus::PostingIteratorInterface *itr,
			const std::vector<strus::SummarizationVariable> &,
			double /*weight*/,
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

std::vector<strus::SummaryElement> SummarizerFunctionContextTest2::getSummary( const strus::Index &docno )
{
	try {
		std::vector<strus::SummaryElement> elems;

		// iterate every hit, scan for all occurrences (where the
		// query expression matches the feature type in the forward
		// index) and accumulate a set of arrays with unique features
		strus::Index first_pos;
		first_pos = 99999999;

		for( std::vector<strus::PostingIteratorInterface *>::const_iterator itr = m_itrs.begin( ); itr != m_itrs.end( ); itr++ ) {				
			if( (*itr)->skipDoc( docno ) == docno ) {
				strus::Index idxPos = (*itr)->skipPos( 0 );
				if( idxPos != 0 ) {
					first_pos = std::min( first_pos, idxPos );
				}
			}
		}

		// remember all feature positions for the top N positions
		std::priority_queue< strus::Index, std::vector<strus::Index>, std::greater<strus::Index> > positions;
		for( std::vector<strus::PostingIteratorInterface *>::const_iterator itr = m_itrs.begin( ); itr != m_itrs.end( ); itr++ ) {
			if( (*itr)->skipDoc( docno ) == docno ) {
				strus::Index idxPos = first_pos;
				while( idxPos != 0 && idxPos <= first_pos + m_N ) {
					positions.push( idxPos );
#ifdef LOWLEVEL_DEBUG
					std::cout << "DEBUG: position seen " << idxPos << std::endl;
#endif
					idxPos = (*itr)->skipPos( idxPos + 1 );
				}
			}
		}
		
		// skip in forward index to the right document
		m_forwardIndex->skipDoc( docno );
		
		std::set<std::string> candidates;

		// iterate through forward index up to position N, if we have found
		// a feature we must "highlight", we do so.
		strus::Index lastPos = -1;
		do {
			strus::Index pos = positions.top( );
			positions.pop( );
			while( !positions.empty( ) && pos == lastPos ) {
				pos = positions.top( );
				positions.pop( );
			}
			if( positions.empty( ) ) {
				continue;
			}
			lastPos = pos;
			if( m_forwardIndex->skipPos( pos ) == pos ) {
				std::string w = m_forwardIndex->fetch( );
#ifdef LOWLEVEL_DEBUG
				std::cout << "DEBUG: forward position skip " << pos << " " << w << std::endl;
#endif
				if( (unsigned int)pos >= strus::Constants::storage_max_position_info( ) ) {
#ifdef LOWLEVEL_DEBUG
					std::cout << "DEBUG: ignoring illegal token on max_position " << pos << " " << w << std::endl;
#endif
				} else {
					if( candidates.find( w ) == candidates.end( ) ) {
						elems.push_back( strus::SummaryElement( m_resultname, w ) );
						candidates.insert( w );				
					}
				}
			}
		} while( !positions.empty( ) );
			
		return elems;
	}
	CATCH_ERROR_ARG1_MAP_RETURN( _TXT( "error fetching '%s' summary: %s" ), "test", *m_errorhnd, std::vector<strus::SummaryElement>( ) );
}

std::string SummarizerFunctionContextTest2::debugCall( const strus::Index& docno)
{
	return "n/a";
}

std::string SummarizerFunctionInstanceTest2::tostring() const
{
	try
	{
		std::ostringstream rt;
		rt <<  "'type='" << m_type << "', 'N'='" << m_N << "'";
		return rt.str();
	}
	CATCH_ERROR_ARG1_MAP_RETURN( _TXT( "error mapping '%s' summarizer function to string: %s" ), "test2", *m_errorhnd, std::string( ) );
}


void SummarizerFunctionInstanceTest2::addStringParameter( const std::string& name, const std::string& value )
{
	try {
		if( boost::algorithm::iequals( name, "match" ) ) {
			m_errorhnd->report( _TXT( "parameter '%s' for summarization function '%s' expected to be defined as feature and not as string or numeric value" ), name.c_str( ), "test" );
		}
		if( boost::algorithm::iequals( name, "type" ) ) {
			m_type = value;
		} else {
			m_errorhnd->report( _TXT( "unknown '%s' string summarization function parameter '%s'" ), "test2", name.c_str( ) );
		}
	}
	CATCH_ERROR_ARG1_MAP( _TXT( "error adding string parameter to '%s' summarizer: %s"), "test2", *m_errorhnd );
}

void SummarizerFunctionInstanceTest2::addNumericParameter( const std::string& name, const strus::NumericVariant& value )
{
	if( boost::algorithm::iequals( name, "type" ) ) {		
		m_errorhnd->report( _TXT( "no numeric value expected for parameter '%s' in summarization function '%s'"), name.c_str( ), "test2" );
	} else if( boost::algorithm::iequals( name, "N" ) ) {
		m_N = (unsigned int)value;
	} else {
		m_errorhnd->report( _TXT( "unknown '%s' numeric summarization function parameter '%s'" ), "test2", name.c_str( ) );
	}
}

void SummarizerFunctionInstanceTest2::defineResultName(const std::string& resultname, const std::string& itemname )
{
	try
	{
		if (boost::algorithm::iequals( itemname, "resultname"))
		{
			m_resultname = resultname;
		}
		else
		{
			throw strus::runtime_error( _TXT("unknown item name '%s"), itemname.c_str());
		}
	}
	CATCH_ERROR_ARG1_MAP( _TXT("error defining result name of '%s' summarizer: %s"), "test2", *m_errorhnd);
}

strus::SummarizerFunctionContextInterface* SummarizerFunctionInstanceTest2::createFunctionContext(
			const strus::StorageClientInterface* storage_,
			strus::MetaDataReaderInterface* metadata,
			const strus::GlobalStatistics& stats ) const
{
	try {
		return new SummarizerFunctionContextTest2( storage_, m_type, m_N, m_resultname, m_errorhnd );
	}
	CATCH_ERROR_ARG1_MAP_RETURN( _TXT("error creating context of '%s' summarizer: %s"), "test2", *m_errorhnd, 0);
}

strus::SummarizerFunctionInstanceInterface *SummarizerFunctionTest2::createInstance( const strus::QueryProcessorInterface *query_processor ) const
{
	try {
		return new SummarizerFunctionInstanceTest2( m_errorhnd );
	}
	CATCH_ERROR_ARG1_MAP_RETURN( _TXT( "error creating instance of '%s' summarizer: %s" ), "test2", *m_errorhnd, 0 );
}

strus::FunctionDescription SummarizerFunctionTest2::getDescription( ) const
{
	try {
		strus::FunctionDescription descr( _TXT( "Demonstrating how to implement a summarizer 'test2'" ) );
		descr( strus::FunctionDescription::Parameter::String, "type", _TXT( "defines the forward index type to extract for every hit"));
		return descr;
	}
	CATCH_ERROR_ARG1_MAP_RETURN( _TXT( "error creating summarizer function description for '%s': %s" ), "test2", *m_errorhnd,
		strus::FunctionDescription( ) );
}

} // namespace test
