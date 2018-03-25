/*
 * Copyright (c) 2014 Patrick P. Frey
 * Copyright (c) 2015,2016 Andreas Baumann
 * Copyright (c) 2015,2016 Eurospider IT AG Zurich
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "summarizertest.hpp"

#include "strus/attributeReaderInterface.hpp"
#include "strus/storageClientInterface.hpp"
#include "strus/numericVariant.hpp"

#include "strus/private/errorUtils.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#include <string>
#include <sstream>
#include <queue>

#include <iostream>
#include <iomanip>

#undef STRUS_LOWLEVEL_DEBUG

namespace test {

void SummarizerFunctionContextTest::addSummarizationFeature( const std::string &name,
			strus::PostingIteratorInterface *itr,
			const std::vector<strus::SummarizationVariable> &,
			double /*weight*/,
			const strus::TermStatistics &)
{
	try {
		if( boost::algorithm::iequals( name, "match" ) ) {
			m_itrs.push_back( itr );
		} else {
			m_errorhnd->report( strus::ErrorCodeNotImplemented,
						_TXT( "unknown '%s' summarization feature '%s'" ), "test", name.c_str( ) );
		}
	}
	CATCH_ERROR_ARG1_MAP( _TXT( "error adding summarization feature to '%s' summarizer: %s" ), "test", *m_errorhnd );
}

struct Candidate {
	strus::Index start;
	strus::Index end;
	unsigned int hits;
	
	bool operator <( const Candidate &other ) const
    {
		if( hits < other.hits ) return false;
		if( hits == other.hits ) {
			if( start > other.start ) return false;
			return true;
		}
		return true;
    }
};

struct FinalSorter {
	inline bool operator( )( const Candidate &c1, const Candidate &c2 )
	{
		if( c1.start < c2.start ) return true;
		return false;
	}
};

std::vector<strus::SummaryElement> SummarizerFunctionContextTest::getSummary( const strus::Index &docno )
{
	try {
		std::vector<strus::SummaryElement> elems;

		// show a first "bonus" attribute (just for demonstrating the concept)
		if( m_attribute > 0 ) {
			m_attribreader->skipDoc( docno );
			std::string attr = m_attribreader->getValue( m_attribute );
			if( !attr.empty( ) ) { 
				elems.push_back( strus::SummaryElement( "attribute", attr ) );
			} else {
				elems.push_back( strus::SummaryElement( "attribute", "n/a" ) );
			}
		}
		
		// we also add a metadata feature (just for demonstrating the concept)
		if( m_metadata > 0 ) {
			m_metadatareader->skipDoc( docno );
			strus::NumericVariant value = m_metadatareader->getValue( m_metadata );
			if( value.defined( ) ) {
				elems.push_back( strus::SummaryElement( "metadata", value.tostring( ).c_str( ) ) );
			} else {
				elems.push_back( strus::SummaryElement( "metadata", "n/a" ) );
			}
		}

		// more sophisticated, find best sentences if we have a sentence marker
		if( m_sentenceIndex > 0 ) {
			// position all iterators of all matching features to the
			// document and first position
			for( std::vector<strus::PostingIteratorInterface *>::const_iterator itr = m_itrs.begin( ); itr != m_itrs.end( ); itr++ ) {				
				if( (*itr)->skipDoc( docno ) == docno ) {
					(*itr)->skipPos( 0 );
				}
			}

			// skip in forward index to the right document (all feature
			// types to be added to summary and the sentence marker iterator)
			for( std::vector<strus::ForwardIteratorInterface*>::iterator it = m_forwardIndex.begin( ); it != m_forwardIndex.end( ); it++ ) {
				(*it)->skipDoc( docno );
			}
			m_sentenceIndex->skipDoc( docno );
			
			// iterate the whole forward index for sentences, count
			// matching terms per candidate sentence			
			std::vector<Candidate> candidates;
			strus::Index startSentence = m_sentenceIndex->skipPos( 0 );
			strus::Index endSentence = m_sentenceIndex->skipPos( 2 );
			while( endSentence > startSentence ) {
#ifdef STRUS_LOWLEVEL_DEBUG
				std::cout << "SENTENCE " << docno << ", [" << startSentence << ", " << endSentence << ")" << std::endl;
#endif
				Candidate candidate;
				candidate.start = startSentence;
				candidate.end = endSentence;
				candidate.hits = 0;
				for( std::vector<strus::PostingIteratorInterface *>::const_iterator itr = m_itrs.begin( ); itr != m_itrs.end( ); itr++ ) {	
					if( (*itr)->skipDoc( docno ) == docno ) {
						strus::Index pos = (*itr)->skipPos( startSentence );					
#ifdef STRUS_LOWLEVEL_DEBUG
					std::cout << "MATCH " << docno << " " << (*itr)->featureid( ) << " " << pos << std::endl;
#endif
						if( pos > 0 && pos < endSentence && pos >= startSentence ) {
							candidate.hits++;
						}
					}
				}
				
				if( candidate.hits > 0 ) {
					candidates.push_back( candidate );
				}
				
				startSentence = endSentence;
				endSentence = m_sentenceIndex->skipPos( endSentence + 1 );
			}			
			
			// sort candidates per nofMatches, startPos (preferring
			// more hits in a sentence over less hits, preferring
			// sentences in the beginning over sentences at the end)
			std::sort( candidates.begin( ), candidates.end( ) );
			unsigned int i = 1;
			std::vector<Candidate> finalCandidates;
			for( std::vector<Candidate>::const_iterator it = candidates.begin( ); it != candidates.end( ) && i <= m_nofSentences; it++, i++ ) {
				finalCandidates.push_back( *it );
			}
			
			// sort final candidates by start position (keeping order in
			// document intact)
			std::sort( finalCandidates.begin( ), finalCandidates.end( ), FinalSorter( ) );
			
			// iterate all candidates and keep at most N sentences
			// per summary
			for( std::vector<Candidate>::const_iterator it = finalCandidates.begin( ); it != finalCandidates.end( ); it++ ) {
#ifdef STRUS_LOWLEVEL_DEBUG
				std::cout << "CANDIDATE " << docno << ", [" << it->start << ", " << it->end << ") " << it->hits << std::endl;
#endif

				// remember all feature positions for the top N positions in the sentence
				std::priority_queue< strus::Index, std::vector<strus::Index>, std::greater<strus::Index> > positions;
				for( std::vector<strus::PostingIteratorInterface *>::const_iterator itr = m_itrs.begin( ); itr != m_itrs.end( ); itr++ ) {
					if( (*itr)->skipDoc( docno ) == docno ) {
						strus::Index idxPos = (*itr)->skipPos( it->start );
						while( idxPos != 0 && idxPos <= it->end && idxPos - it->start < m_N ) {
							positions.push( idxPos );
#ifdef STRUS_LOWLEVEL_DEBUG
							std::cout << "CANDIDATE_MATCHES " << docno << ", [" << it->start << ", " << it->end << ") " << idxPos << std::endl;
#endif						
							idxPos = (*itr)->skipPos( idxPos + 1 );
						}
					}
				}

				strus::Index nextMarkPos = -1;
				if( !positions.empty( ) ) {
					nextMarkPos = positions.top( );
					positions.pop( );
				}
				for( strus::Index pos = it->start; pos < it->end && pos - it->start < m_N; pos++ ) {
					for( std::vector<strus::ForwardIteratorInterface*>::iterator fit = m_forwardIndex.begin( ); fit != m_forwardIndex.end( ); fit++ ) {
						if( (*fit)->skipPos( pos ) == pos ) {
							std::string w = (*fit)->fetch( );
#ifdef STRUS_LOWLEVEL_DEBUG
				std::cout << "WORD " << docno << " " << w << " " << pos << " " << nextMarkPos << " " << std::endl;
#endif
							if( pos == nextMarkPos ) {
								std::stringstream ss;				
								ss << boost::format( m_mark ) % w;
								elems.push_back( strus::SummaryElement( m_resultname, ss.str( ) ) );
								while( !positions.empty( ) && nextMarkPos == pos ) {
									nextMarkPos = positions.top( );
									positions.pop( );
								}
							} else {
								elems.push_back( strus::SummaryElement( m_resultname, w ) );
							}
						}
					}
				}
				
				if( m_add_dots ) {
					elems.push_back( strus::SummaryElement( m_resultname, "..." ) );
				}
			}

		// simple algorithm, match N terms after first match or from
		// the beginning of the document
		} else {
			// position all iterators of all matching features to the
			// document and first position
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
						idxPos = (*itr)->skipPos( idxPos + 1 );
					}
				}
			}
			
			// skip in forward index to the right document
			for( std::vector<strus::ForwardIteratorInterface*>::iterator it = m_forwardIndex.begin( ); it != m_forwardIndex.end( ); it++ ) {
				(*it)->skipDoc( docno );
			}

			// iterate through forward index up to position N, if we have found
			// a feature we must "highlight", we do so.
			strus::Index nextMarkPos = -1;
			if( !positions.empty( ) ) {
				nextMarkPos = positions.top( );
				positions.pop( );
			}
			strus::Index first_forward_pos;
			if( m_start_first_match ) {
				first_forward_pos = first_pos;
			} else {
				first_forward_pos = 1;
			}		
			for( strus::Index pos = first_forward_pos; pos <= first_forward_pos + m_N; pos++ ) {
				for( std::vector<strus::ForwardIteratorInterface*>::iterator it = m_forwardIndex.begin( ); it != m_forwardIndex.end( ); it++ ) {
					if( (*it)->skipPos( pos ) == pos ) {
						std::string w = (*it)->fetch( );
						if( pos == nextMarkPos ) {
							std::stringstream ss;				
							ss << boost::format( m_mark ) % w;
							elems.push_back( strus::SummaryElement( m_resultname, ss.str( ) ) );
							while( !positions.empty( ) && nextMarkPos == pos ) {
								nextMarkPos = positions.top( );
								positions.pop( );
							}
						} else {
							elems.push_back( strus::SummaryElement( m_resultname, w ) );
						}
					}
				}
			}
			if( m_add_dots ) {
				elems.push_back( strus::SummaryElement( m_resultname, "..." ) );
			}
		}
	
		return elems;
	}
	CATCH_ERROR_ARG1_MAP_RETURN( _TXT( "error fetching '%s' summary: %s" ), "test", *m_errorhnd, std::vector<strus::SummaryElement>( ) );
}

std::string SummarizerFunctionContextTest::debugCall( const strus::Index& docno)
{
	return "n/a";
}

std::string SummarizerFunctionInstanceTest::tostring() const
{
	try
	{
		std::ostringstream rt;
		rt << "attribute='" << m_attribute << "', types=(";
		bool first = true;
		for( std::vector<std::string>::const_iterator it = m_types.begin( ); it != m_types.end( ); it++ ) {
			if( !first ) rt << ", ";
			first = false;
			rt << *it;
		}
		rt << "), N=" << m_N;
		rt << ", nof_sentences=" << m_nofSentences;
		rt << ", start_first_match=" << m_start_first_match;
		rt << ", add_dots=" << m_add_dots;
		return rt.str();
	}
	CATCH_ERROR_ARG1_MAP_RETURN( _TXT( "error mapping '%s' summarizer function to string: %s" ), "test", *m_errorhnd, std::string( ) );
}


void SummarizerFunctionInstanceTest::addStringParameter( const std::string& name, const std::string& value )
{
	try {
		if( boost::algorithm::iequals( name, "match" ) ) {
			m_errorhnd->report( strus::ErrorCodeInvalidArgument,
						_TXT( "parameter '%s' for summarization function '%s' expected to be defined as feature and not as string or numeric value" ), name.c_str( ), "test" );
		}
		if( boost::algorithm::iequals( name, "attribute" ) ) {
			m_attribute = value;
		} else if( boost::algorithm::iequals( name, "metadata" ) ) {
			m_metadata = value;
		} else if( boost::algorithm::iequals( name, "type" ) ) {
			m_types.push_back( value );
		} else if( boost::algorithm::iequals( name, "sentence" ) ) {
			m_sentence = value;
		} else if( boost::algorithm::iequals( name, "mark" ) ) {
			m_mark = value;
		} else {
			m_errorhnd->report( strus::ErrorCodeNotImplemented,
						_TXT( "unknown '%s' string summarization function parameter '%s'" ), "test", name.c_str( ) );
		}
	}
	CATCH_ERROR_ARG1_MAP( _TXT( "error adding string parameter to '%s' summarizer: %s"), "test", *m_errorhnd );
}

void SummarizerFunctionInstanceTest::addNumericParameter( const std::string& name, const strus::NumericVariant& value )
{
	if( boost::algorithm::iequals( name, "N" ) ) {
		m_N = (int)value.toint( );
	} else if( boost::algorithm::iequals( name, "nof_sentences" ) ) {
		m_nofSentences = (int)value.toint( );
	} else if( boost::algorithm::iequals( name, "start_first_match" ) ) {
		m_start_first_match = ( value.touint( ) > 0 );
	} else if( boost::algorithm::iequals( name, "add_dots" ) ) {
		m_add_dots = ( value.touint( ) > 0 );
	} else if( boost::algorithm::iequals( name, "attribute" ) ) {		
		m_errorhnd->report( strus::ErrorCodeInvalidArgument,
					_TXT( "no numeric value expected for parameter '%s' in summarization function '%s'"), name.c_str( ), "test" );
	} else {
		m_errorhnd->report( strus::ErrorCodeNotImplemented,
					_TXT( "unknown '%s' numeric summarization function parameter '%s'" ), "test", name.c_str( ) );
	}
}

void SummarizerFunctionInstanceTest::defineResultName(const std::string& resultname, const std::string& itemname )
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
	CATCH_ERROR_ARG1_MAP( _TXT("error defining result name of '%s' summarizer: %s"), "test", *m_errorhnd);
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
		return new SummarizerFunctionContextTest( storage, attributeReader, metadata, m_attribute, m_metadata, m_types, m_sentence, m_N, m_add_dots, m_nofSentences, m_start_first_match, m_mark, m_resultname, m_errorhnd );
	}
	CATCH_ERROR_ARG1_MAP_RETURN( _TXT("error creating context of '%s' summarizer: %s"), "test", *m_errorhnd, 0);
}

strus::SummarizerFunctionInstanceInterface *SummarizerFunctionTest::createInstance( const strus::QueryProcessorInterface *query_processor ) const
{
	try {
		return new SummarizerFunctionInstanceTest( m_errorhnd );
	}
	CATCH_ERROR_ARG1_MAP_RETURN( _TXT( "error creating instance of '%s' summarizer: %s" ), "test", *m_errorhnd, 0 );
}

strus::FunctionDescription SummarizerFunctionTest::getDescription( ) const
{
	try {
		strus::FunctionDescription descr( _TXT( "Demonstrating how to implement a summarizer 'test'" ) );
		descr( strus::FunctionDescription::Parameter::Attribute, "attribute", _TXT( "an attribute parameter" ) );
		descr( strus::FunctionDescription::Parameter::Metadata, "metadata", _TXT( "a metadata parameter" ) );
		descr( strus::FunctionDescription::Parameter::Feature, "match", _TXT( "defines the query features to respect for summarizing"));
		descr( strus::FunctionDescription::Parameter::Numeric, "N", _TXT( "maximal size of the abstract" ) );
		descr( strus::FunctionDescription::Parameter::String, "mark", _TXT( "how to mark a hit in boost format syntax with one parameter %1%" ) );
		descr( strus::FunctionDescription::Parameter::String, "type", _TXT( "feature type in forward index to be used to produce summaries (can appear multiple times)" ) );
		descr( strus::FunctionDescription::Parameter::String, "sentence", _TXT( "feature type in forward index to be used to segment the document (e. g. sentences)" ) );
		descr( strus::FunctionDescription::Parameter::Numeric, "nof_sentences", _TXT( "maximal size of the abstract in sentences" ) );
		descr( strus::FunctionDescription::Parameter::Numeric, "start_first_match", _TXT( "start with abstracting at the first match = 1 ( default: first position of document = 0)" ) );
		descr( strus::FunctionDescription::Parameter::Numeric, "add_dots", _TXT( "add ... when ommitting text (1 = true, 0 = false, default: 0)" ) );
		return descr;
	}
	CATCH_ERROR_ARG1_MAP_RETURN( _TXT( "error creating summarizer function description for '%s': %s" ), "test", *m_errorhnd,
		strus::FunctionDescription( ) );
}

} // namespace test
