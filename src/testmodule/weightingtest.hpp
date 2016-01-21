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

#ifndef WEIGHTING_TEST_HPP_INCLUDED
#define WEIGHTING_TEST_HPP_INCLUDED

#include "strus/weightingFunctionInterface.hpp"
#include "strus/weightingFunctionInstanceInterface.hpp"
#include "strus/weightingFunctionContextInterface.hpp"
#include "strus/metaDataReaderInterface.hpp"
#include "strus/storageClientInterface.hpp"
#include "strus/postingIteratorInterface.hpp"
#include "strus/index.hpp"
#include "strus/termStatistics.hpp"

#include "strus/errorBufferInterface.hpp"

const float DEFAULT_PARAM_VALUE = 1.0;

namespace test {

class WeightingFunctionContextTest : public strus::WeightingFunctionContextInterface
{
	public:
	
		WeightingFunctionContextTest( const strus::StorageClientInterface *storage,
			strus::MetaDataReaderInterface *metadata_,
			double param,
			strus::ErrorBufferInterface *errorhnd )
			: m_storage( storage ), m_metadata( metadata_ ), m_param( param ), m_errorhnd( errorhnd ) { }
			
		virtual ~WeightingFunctionContextTest( ) { }

		virtual void addWeightingFeature( const std::string &name,
			strus::PostingIteratorInterface *itr,
			float weight,
			const strus::TermStatistics &stats );

		virtual float call( const strus::Index &docno );
	
	private:

		const strus::StorageClientInterface *m_storage;
		strus::MetaDataReaderInterface *m_metadata;
		double m_param;
		strus::ErrorBufferInterface *m_errorhnd;
		
};

class WeightingFunctionInstanceTest : public strus::WeightingFunctionInstanceInterface
{
	private:

		strus::ErrorBufferInterface *m_errorhnd;
		double m_param;

	public:
	
		explicit WeightingFunctionInstanceTest( strus::ErrorBufferInterface *errorhnd_ )
			: m_errorhnd( errorhnd_ ), m_param( DEFAULT_PARAM_VALUE ) { }

		virtual ~WeightingFunctionInstanceTest( ) { }

		virtual void addStringParameter( const std::string& name, const std::string& value );
		virtual void addNumericParameter( const std::string& name, const strus::ArithmeticVariant& value );

		virtual strus::WeightingFunctionContextInterface* createFunctionContext(
			const strus::StorageClientInterface* storage_,
			strus::MetaDataReaderInterface* metadata,
			const strus::GlobalStatistics& stats ) const;

		virtual std::string tostring( ) const;
};

class WeightingFunctionTest : public strus::WeightingFunctionInterface
{
	private:

		strus::ErrorBufferInterface *m_errorhnd;

	public:
	
		explicit WeightingFunctionTest( strus::ErrorBufferInterface *errorhnd_ )
			: m_errorhnd( errorhnd_ ) { }

		virtual ~WeightingFunctionTest( ) { }

		virtual strus::WeightingFunctionInstanceInterface* createInstance( ) const;

		virtual strus::WeightingFunctionInterface::Description getDescription( ) const;
};

} // namespace test

#endif
