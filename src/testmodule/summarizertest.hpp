/*
 * Copyright (c) 2014 Patrick P. Frey
 * Copyright (c) 2015,2016 Andreas Baumann
 * Copyright (c) 2015,2016 Eurospider IT AG Zurich
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef SUMMARIZER_TEST_HPP_INCLUDED
#define SUMMARIZER_TEST_HPP_INCLUDED

#include "strus/summarizerFunctionContextInterface.hpp"
#include "strus/summarizerFunctionInterface.hpp"
#include "strus/summarizerFunctionInstanceInterface.hpp"
#include "strus/queryProcessorInterface.hpp"
#include "strus/storageClientInterface.hpp"
#include "strus/attributeReaderInterface.hpp"
#include "strus/postingIteratorInterface.hpp"
#include "strus/forwardIteratorInterface.hpp"
#include "strus/index.hpp"
#include "strus/termStatistics.hpp"
#include "strus/errorCodes.hpp"
#include "strus/errorBufferInterface.hpp"

#include "strus/private/errorUtils.hpp"
#include "strus/private/internationalization.hpp"

#include "strus/metaDataReaderInterface.hpp"
#include "strus/attributeReaderInterface.hpp"

#include <vector>

const unsigned int TEST_DEFAULT_N = 50;
const std::string TEST_DEFAULT_MARK = "<b>%1%</b>";
const bool TEST_DEFAULT_START_FIRST_MATCH = false;
const unsigned int TEST_DEFAULT_NOF_SENTENCES = 3;
const bool TEST_DEFAULT_ADD_DOTS = false;
const std::string TEST_DEFAULT_RESULTNAME = "forward";

namespace test {

class SummarizerFunctionContextTest : public strus::SummarizerFunctionContextInterface
{

	public:
	
		SummarizerFunctionContextTest(
			const strus::StorageClientInterface *storage,
			strus::AttributeReaderInterface *attribreader, 
			strus::MetaDataReaderInterface *metadatareader,
			const std::string &attribute,
			const std::string &metadata,
			const std::vector<std::string> &types,
			const std::string &sentence,
			const unsigned int N,
			const bool add_dots,
			const unsigned int nof_sentences,
			const bool start_first_match,
			const std::string &mark,
			const std::string &resultname,
			strus::ErrorBufferInterface* errorhnd )
			: m_attribreader( attribreader ), 
			m_metadatareader( metadatareader ),
			m_attribute( 0 ),
			m_metadata( 0 ),
			m_types( types ),
			m_sentence( sentence ),
			m_N( N ),
			m_add_dots( add_dots ),
			m_nofSentences( nof_sentences),
			m_start_first_match( start_first_match ),
			m_mark( mark ),
			m_resultname( resultname ),
			m_errorhnd( errorhnd ),
			m_itrs( ),
			m_forwardIndex( ),
			m_sentenceIndex( 0 ) {				
				if( !attribute.empty( ) ) {
					attribreader->elementHandle( attribute.c_str( ) );
				}
				
				if( !metadata.empty( ) ) {
					metadatareader->elementHandle( metadata.c_str( ) );
				}				

				for( std::vector<std::string>::const_iterator it = m_types.begin( ); it != m_types.end( ); it++ ) {
					strus::ForwardIteratorInterface *fit = storage->createForwardIterator( *it );
					if( !fit ) {
						throw strus::runtime_error( _TXT( "error creating forward index iterator" ) );
					}
					m_forwardIndex.push_back( fit );
				}
				
				if( m_sentence != "" ) {
					m_sentenceIndex = storage->createForwardIterator( m_sentence );
					if( !m_sentenceIndex ) {
						throw strus::runtime_error( _TXT( "error creating forward index iterator for sentence markers" ) );
					}
				}
			}
				
		virtual ~SummarizerFunctionContextTest( ) {
			delete m_attribreader;
			for( std::vector<strus::ForwardIteratorInterface*>::iterator it = m_forwardIndex.begin( ); it != m_forwardIndex.end( ); it++ ) {
				delete *it;
			}
			delete m_sentenceIndex;
		}

		virtual void addSummarizationFeature( const std::string &name,
			strus::PostingIteratorInterface *itr,
			const std::vector<strus::SummarizationVariable> &,
			double /*weight*/,
			const strus::TermStatistics &);

		virtual void setVariableValue( const std::string& name, double value) {
			m_errorhnd->report( *strus::ErrorCode( strus::StrusComponentCore, strus::ErrorOperationBuildData, strus::ErrorCauseNotImplemented),
						_TXT("no variables known for function '%s'"), "match");
		}
		
		virtual std::vector<strus::SummaryElement> getSummary( const strus::Index &docno );

		virtual std::string debugCall( const strus::Index& docno);
	
	private:

		strus::AttributeReaderInterface *m_attribreader;
		strus::MetaDataReaderInterface *m_metadatareader;
		int m_attribute;
		int m_metadata;
		std::vector<std::string> m_types;
		std::string m_sentence;
		strus::Index m_N;
		bool m_add_dots;
		unsigned int m_nofSentences;
		bool m_start_first_match;
		std::string m_mark;
		std::string m_resultname;
		strus::ErrorBufferInterface *m_errorhnd;
		std::vector<strus::PostingIteratorInterface*> m_itrs;
		std::vector<strus::ForwardIteratorInterface*> m_forwardIndex;
		strus::ForwardIteratorInterface *m_sentenceIndex;
};

class SummarizerFunctionInstanceTest : public strus::SummarizerFunctionInstanceInterface
{
	private:

		strus::ErrorBufferInterface *m_errorhnd;
		std::string m_attribute;
		std::string m_metadata;
		std::vector<std::string> m_types;
		std::string m_sentence;
		unsigned int m_N;
		bool m_add_dots;
		unsigned int m_nofSentences;
		bool m_start_first_match;
		std::string m_mark;
		std::string m_resultname;

	public:
	
		explicit SummarizerFunctionInstanceTest( strus::ErrorBufferInterface *errorhnd_ )
			: m_errorhnd( errorhnd_ ), m_N( TEST_DEFAULT_N ), 
			m_add_dots( TEST_DEFAULT_ADD_DOTS ), m_nofSentences( TEST_DEFAULT_NOF_SENTENCES ),
			m_start_first_match( TEST_DEFAULT_START_FIRST_MATCH ), m_mark( TEST_DEFAULT_MARK ),
			m_resultname( TEST_DEFAULT_RESULTNAME ) { }

		virtual ~SummarizerFunctionInstanceTest( ) { }

		virtual void addStringParameter( const std::string& name, const std::string& value );
		virtual void addNumericParameter( const std::string& name, const strus::NumericVariant& value );
		virtual void defineResultName( const std::string& resultname, const std::string& itemname);

		virtual std::vector<std::string> getVariables() const
		{
			return std::vector<std::string>();
		}

		virtual strus::SummarizerFunctionContextInterface* createFunctionContext(
			const strus::StorageClientInterface* storage_,
			strus::MetaDataReaderInterface* metadata,
			const strus::GlobalStatistics& stats ) const;

		virtual std::string tostring( ) const;
};

class SummarizerFunctionTest : public strus::SummarizerFunctionInterface
{
	private:

		strus::ErrorBufferInterface *m_errorhnd;

	public:
	
		explicit SummarizerFunctionTest( strus::ErrorBufferInterface *errorhnd_ )
			: m_errorhnd( errorhnd_ ) { }

		virtual ~SummarizerFunctionTest( ) { }

		virtual strus::SummarizerFunctionInstanceInterface *createInstance( const strus::QueryProcessorInterface *query_processor ) const;

		virtual strus::FunctionDescription getDescription( ) const;
};

} // namespace test

#endif
