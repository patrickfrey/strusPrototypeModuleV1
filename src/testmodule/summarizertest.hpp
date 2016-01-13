#ifndef SUMMARIZER_TEST_HPP_INCLUDED
#define SUMMARIZER_TEST_HPP_INCLUDED

#include "strus/summarizerFunctionContextInterface.hpp"
#include "strus/summarizerFunctionInterface.hpp"
#include "strus/summarizerFunctionInstanceInterface.hpp"
#include "strus/queryProcessorInterface.hpp"
#include "strus/attributeReaderInterface.hpp"
#include "strus/postingIteratorInterface.hpp"
#include "strus/index.hpp"
#include "strus/termStatistics.hpp"

#include "strus/errorBufferInterface.hpp"

#include "strus/private/errorUtils.hpp"
#include "strus/private/internationalization.hpp"

#include <vector>

namespace test {

class SummarizerFunctionContextTest : public strus::SummarizerFunctionContextInterface
{

	public:
	
		SummarizerFunctionContextTest( strus::AttributeReaderInterface *attribreader, 
			const std::string& attribute, strus::ErrorBufferInterface* errorhnd )
			: m_attribreader( attribreader ), 
			m_attribute( attribreader->elementHandle( attribute.c_str( ) ) ),
			m_errorhnd( errorhnd ) { }
				
		virtual ~SummarizerFunctionContextTest( ) {
			delete m_attribreader;
		}

		virtual void addSummarizationFeature( const std::string &,
			strus::PostingIteratorInterface *,
			const std::vector<strus::SummarizationVariable> &,
			float /*weight*/,
			const strus::TermStatistics &) {
			m_errorhnd->report( _TXT( "no sumarization features expected in summarization function '%s'" ), "test" );
		}
		
		virtual std::vector<SummarizerFunctionContextInterface::SummaryElement> getSummary( const strus::Index &docno );
	
	private:

		strus::AttributeReaderInterface *m_attribreader;
		int m_attribute;
		strus::ErrorBufferInterface *m_errorhnd;
};

class SummarizerFunctionInstanceTest : public strus::SummarizerFunctionInstanceInterface
{
	private:

		strus::ErrorBufferInterface *m_errorhnd;
		std::string m_attribute;

	public:
	
		explicit SummarizerFunctionInstanceTest( strus::ErrorBufferInterface *errorhnd_ )
			: m_errorhnd( errorhnd_ ) { }

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
