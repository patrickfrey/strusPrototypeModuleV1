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

#include <vector>

const unsigned int TEST_DEFAULT_N = 50;
const std::string TEST_DEFAULT_MARK = "<b>%1%</b>";

namespace test {

class SummarizerFunctionContextTest : public strus::SummarizerFunctionContextInterface
{

	public:
	
		SummarizerFunctionContextTest(
			const strus::StorageClientInterface *storage,
			strus::AttributeReaderInterface *attribreader, 
			const std::string &attribute,
			const std::string &type,
			const unsigned int N,
			const std::string mark,
			strus::ErrorBufferInterface* errorhnd )
			: m_attribreader( attribreader ), 
			m_attribute( attribreader->elementHandle( attribute.c_str( ) ) ),
			m_N( N ),
			m_mark( mark ),
			m_errorhnd( errorhnd ) { 
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
		
		virtual std::vector<SummarizerFunctionContextInterface::SummaryElement> getSummary( const strus::Index &docno );
	
	private:

		strus::AttributeReaderInterface *m_attribreader;
		int m_attribute;
		strus::Index m_N;
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
		std::string m_type;
		unsigned int m_N;
		std::string m_mark;

	public:
	
		explicit SummarizerFunctionInstanceTest( strus::ErrorBufferInterface *errorhnd_ )
			: m_errorhnd( errorhnd_ ), m_N( TEST_DEFAULT_N ), m_mark( TEST_DEFAULT_MARK ) { }

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
