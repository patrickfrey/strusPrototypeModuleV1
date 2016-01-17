/*
---------------------------------------------------------------------
    Demo and test modules for the search web service
    using the C++ library strus which implements basic operations
    to build a search engine for structured search on unstructured data.

    Copyright (C) 2013,2014 Patrick Frey
    Copyright (C) 2015,2016 Andreas Baumann <mail@andreasbaumann.cc>
    Copyright (C) 2015,2016 Eurospider IT AG Zurich

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

--------------------------------------------------------------------

	The latest version of strus can be found at 'http://github.com/patrickfrey/strus'
	For documentation see 'http://patrickfrey.github.com/strus'

--------------------------------------------------------------------
*/

#ifndef OPERATOR_TEST_HPP_INCLUDED
#define OPERATOR_TEST_HPP_INCLUDED

#include "strus/postingJoinOperatorInterface.hpp"
#include "strus/postingIteratorInterface.hpp"

#include "strus/errorBufferInterface.hpp"

namespace test {

class TestPostingIterator : public strus::PostingIteratorInterface
{
	public:
	
		TestPostingIterator( const std::vector<strus::Reference< strus::PostingIteratorInterface> > &args, strus::ErrorBufferInterface* errorhnd );
	
		virtual ~TestPostingIterator( ) { }

		virtual strus::Index skipDoc( const strus::Index &docno );
		
		virtual strus::Index skipDocCandidate( const strus::Index &docno );
		
		virtual strus::Index skipPos( const strus::Index &firstPos );

		virtual const char *featureid( ) const;
 
		virtual strus::Index documentFrequency( ) const;
	
		virtual unsigned int frequency( );
		
		virtual strus::Index docno( ) const;
		
		virtual strus::Index posno( ) const;
};

class PostingJoinOperatorTest : public strus::PostingJoinOperatorInterface
{
	private:

		strus::ErrorBufferInterface *m_errorhnd;

	public:

		explicit PostingJoinOperatorTest( strus::ErrorBufferInterface *errorhnd_ )
			: m_errorhnd( errorhnd_ ) { }
	
		virtual ~PostingJoinOperatorTest( ) { }

		virtual strus::PostingIteratorInterface *createResultIterator(
			const std::vector<strus::Reference<strus::PostingIteratorInterface> > &itrs,
			int range,
			unsigned int cardinality ) const;

		virtual strus::PostingJoinOperatorInterface::Description getDescription( ) const;

};

} // namespace test

#endif
