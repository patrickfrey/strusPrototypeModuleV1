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
			const std::string &type,
			const unsigned int N,
			const bool start_first_match,
			const std::string mark,
			strus::ErrorBufferInterface* errorhnd )
			: m_attribreader( attribreader ), 
			m_metadatareader( metadatareader ),
			m_attribute( 0 ),
			m_metadata( 0 ),
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
				
				m_forwardIndex = storage->createForwardIterator( type );
				if( !m_forwardIndex ) {
					throw strus::runtime_error( _TXT( "error creating forward index iterator" ) );
				}
			}
				
		virtual ~SummarizerFunctionContextTest( ) {
			delete m_attribreader;
			delete m_forwardIndex;
		}

		virtual void addSummarizationFeature( const std::string &name,
			strus::PostingIteratorInterface *itr,
			const std::vector<strus::SummarizationVariable> &,
			float /*weight*/,
			const strus::TermStatistics &);
		
		virtual std::vector<strus::SummaryElement> getSummary( const strus::Index &docno );
	
	private:

		strus::AttributeReaderInterface *m_attribreader;
		strus::MetaDataReaderInterface *m_metadatareader;
		int m_attribute;
		int m_metadata;
		strus::Index m_N;
		bool m_start_first_match;
		std::string m_mark;
		strus::ErrorBufferInterface *m_errorhnd;
		std::vector<strus::PostingIteratorInterface*> m_itrs;
		strus::ForwardIteratorInterface *m_forwardIndex;
};

class SummarizerFunctionInstanceTest : public strus::SummarizerFunctionInstanceInterface
{
	private:

		strus::ErrorBufferInterface *m_errorhnd;
		std::string m_attribute;
		std::string m_metadata;
		std::string m_type;
		unsigned int m_N;
		bool m_start_first_match;
		std::string m_mark;

	public:
	
		explicit SummarizerFunctionInstanceTest( strus::ErrorBufferInterface *errorhnd_ )
			: m_errorhnd( errorhnd_ ), m_N( TEST_DEFAULT_N ), m_start_first_match( TEST_DEFAULT_START_FIRST_MATCH ), m_mark( TEST_DEFAULT_MARK ) { }

		virtual ~SummarizerFunctionInstanceTest( ) { }

		virtual void addStringParameter( const std::string& name, const std::string& value );
		virtual void addNumericParameter( const std::string& name, const strus::ArithmeticVariant& value );

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

		virtual strus::SummarizerFunctionInterface::Description getDescription( ) const;
};

} // namespace test

#endif
