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

#include "strus/errorBufferInterface.hpp"

#include "strus/private/errorUtils.hpp"
#include "strus/private/internationalization.hpp"

#include "strus/metaDataReaderInterface.hpp"
#include "strus/attributeReaderInterface.hpp"

#include <vector>

const unsigned int TEST_DEFAULT_N = 50;
const std::string TEST_DEFAULT_MARK = "<b>%1%</b>";
const bool TEST_DEFAULT_START_FIRST_MATCH = false;

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
			const unsigned int N,
			const bool start_first_match,
			const std::string mark,
			strus::ErrorBufferInterface* errorhnd )
			: m_attribreader( attribreader ), 
			m_metadatareader( metadatareader ),
			m_attribute( 0 ),
			m_metadata( 0 ),
			m_types( types ),
			m_N( N ),
			m_start_first_match( start_first_match ),
			m_mark( mark ),
			m_errorhnd( errorhnd ) { 
				
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
			}
				
		virtual ~SummarizerFunctionContextTest( ) {
			delete m_attribreader;
			for( std::vector<strus::ForwardIteratorInterface*>::iterator it = m_forwardIndex.begin( ); it != m_forwardIndex.end( ); it++ ) {
				delete *it;
			}
		}

		virtual void addSummarizationFeature( const std::string &name,
			strus::PostingIteratorInterface *itr,
			const std::vector<strus::SummarizationVariable> &,
			double /*weight*/,
			const strus::TermStatistics &);
		
		virtual std::vector<strus::SummaryElement> getSummary( const strus::Index &docno );
	
	private:

		strus::AttributeReaderInterface *m_attribreader;
		strus::MetaDataReaderInterface *m_metadatareader;
		int m_attribute;
		int m_metadata;
		std::vector<std::string> m_types;
		strus::Index m_N;
		bool m_start_first_match;
		std::string m_mark;
		strus::ErrorBufferInterface *m_errorhnd;
		std::vector<strus::PostingIteratorInterface*> m_itrs;
		std::vector<strus::ForwardIteratorInterface*> m_forwardIndex;
};

class SummarizerFunctionInstanceTest : public strus::SummarizerFunctionInstanceInterface
{
	private:

		strus::ErrorBufferInterface *m_errorhnd;
		std::string m_attribute;
		std::string m_metadata;
		std::vector<std::string> m_types;
		unsigned int m_N;
		bool m_start_first_match;
		std::string m_mark;

	public:
	
		explicit SummarizerFunctionInstanceTest( strus::ErrorBufferInterface *errorhnd_ )
			: m_errorhnd( errorhnd_ ), m_N( TEST_DEFAULT_N ), m_start_first_match( TEST_DEFAULT_START_FIRST_MATCH ), m_mark( TEST_DEFAULT_MARK ) { }

		virtual ~SummarizerFunctionInstanceTest( ) { }

		virtual void addStringParameter( const std::string& name, const std::string& value );
		virtual void addNumericParameter( const std::string& name, const strus::NumericVariant& value );

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
