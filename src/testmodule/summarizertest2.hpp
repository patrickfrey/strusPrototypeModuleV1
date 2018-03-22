/*
 * Copyright (c) 2014 Patrick P. Frey
 * Copyright (c) 2015,2016 Andreas Baumann
 * Copyright (c) 2015,2016 Eurospider IT AG Zurich
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef SUMMARIZER_TEST2_HPP_INCLUDED
#define SUMMARIZER_TEST2_HPP_INCLUDED

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

const unsigned int TEST2_DEFAULT_N = 50;
const std::string TEST2_DEFAULT_RESULTNAME = "forward";

namespace test {

class SummarizerFunctionContextTest2 : public strus::SummarizerFunctionContextInterface
{

	public:
	
		SummarizerFunctionContextTest2(
			const strus::StorageClientInterface *storage,
			const std::string &type,
			const strus::Index N,
			const std::string &resultname,
			strus::ErrorBufferInterface* errorhnd )
			: m_N( N ), m_resultname( resultname ),
			m_errorhnd( errorhnd ) { 
				
				m_forwardIndex = storage->createForwardIterator( type );
				if( !m_forwardIndex ) {
					throw strus::runtime_error( _TXT( "error creating forward index iterator" ) );
				}
			}
				
		virtual ~SummarizerFunctionContextTest2( ) {
		}

		virtual void addSummarizationFeature( const std::string &name,
			strus::PostingIteratorInterface *itr,
			const std::vector<strus::SummarizationVariable> &,
			double /*weight*/,
			const strus::TermStatistics &);

		virtual void setVariableValue( const std::string& name, double value) {
			m_errorhnd->report( strus::ErrorCodeUnknownIdentifier,
						_TXT("no variables known for function '%s'"), "match");
		}

		virtual std::vector<strus::SummaryElement> getSummary( const strus::Index &docno );

		virtual std::string debugCall( const strus::Index& docno);
	
	private:

		strus::Index m_N;
		std::string m_resultname;
		strus::ErrorBufferInterface *m_errorhnd;
		std::vector<strus::PostingIteratorInterface*> m_itrs;
		strus::ForwardIteratorInterface *m_forwardIndex;
};

class SummarizerFunctionInstanceTest2 : public strus::SummarizerFunctionInstanceInterface
{
	private:

		strus::ErrorBufferInterface *m_errorhnd;
		std::string m_type;
		unsigned int m_N;
		std::string m_resultname;

	public:
	
		explicit SummarizerFunctionInstanceTest2( strus::ErrorBufferInterface *errorhnd_ )
			: m_errorhnd( errorhnd_ ), m_N( TEST2_DEFAULT_N ),
			m_resultname( TEST2_DEFAULT_RESULTNAME ) { }

		virtual ~SummarizerFunctionInstanceTest2( ) { }

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

class SummarizerFunctionTest2 : public strus::SummarizerFunctionInterface
{
	private:

		strus::ErrorBufferInterface *m_errorhnd;

	public:
	
		explicit SummarizerFunctionTest2( strus::ErrorBufferInterface *errorhnd_ )
			: m_errorhnd( errorhnd_ ) { }

		virtual ~SummarizerFunctionTest2( ) { }

		virtual strus::SummarizerFunctionInstanceInterface *createInstance( const strus::QueryProcessorInterface *query_processor ) const;

		virtual strus::FunctionDescription getDescription( ) const;
};

} // namespace test

#endif
