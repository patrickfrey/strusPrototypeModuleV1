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

#include "operatortest.hpp"

#include "strus/private/errorUtils.hpp"

namespace test {

TestPostingIterator::TestPostingIterator( const std::vector<strus::Reference< strus::PostingIteratorInterface> > &args, strus::ErrorBufferInterface* errorhnd )
{
}

strus::Index TestPostingIterator::skipDoc( const strus::Index &docno )
{
	return 0;
}
		
strus::Index TestPostingIterator::skipDocCandidate( const strus::Index &docno )
{
	return 0;
}
		
strus::Index TestPostingIterator::skipPos( const strus::Index &firstPos )
{
	return 0;
}

const char *TestPostingIterator::featureid( ) const
{
	return 0;
}
 
strus::Index TestPostingIterator::documentFrequency( ) const
{
	return 0;
}
	
unsigned int TestPostingIterator::frequency( )
{
	return 0;
}
		
strus::Index TestPostingIterator::docno( ) const
{
	return 0;
}
		
strus::Index TestPostingIterator::posno( ) const
{
	return 0;
}

strus::PostingIteratorInterface *PostingJoinOperatorTest::createResultIterator(
			const std::vector<strus::Reference<strus::PostingIteratorInterface> > &itrs,
			int range,
			unsigned int cardinality ) const
{
	if( range != 0 ) {
		m_errorhnd->report( _TXT( "no range argument expected for 'test'" ) );
		return 0;
	}
	
	if( cardinality != 0 ) {
		m_errorhnd->report( _TXT( "no cardinality argument expected for 'test'" ) );
		return 0;
	}
	
	if( itrs.size( ) == 0 ) {
		m_errorhnd->report( _TXT( "too few arguments for 'test'" ) );
		return 0;
	}

	try {
		return new TestPostingIterator( itrs, m_errorhnd );
	}
	CATCH_ERROR_ARG1_MAP_RETURN( _TXT( "error creating instance of '%s' posting join iterator: %s" ), "test", *m_errorhnd, 0 );
}

strus::PostingJoinOperatorInterface::Description PostingJoinOperatorTest::getDescription( ) const
{
	try {
		strus::PostingJoinOperatorInterface::Description descr( _TXT( "Demonstrating how to implement a 'test' posting join operator" ) );
		return descr;
	}
	CATCH_ERROR_ARG1_MAP_RETURN( _TXT( "error creating posting join operator description for '%s': %s" ), "test", *m_errorhnd,
		strus::PostingJoinOperatorInterface::Description( ) );
}

} // namespace test
