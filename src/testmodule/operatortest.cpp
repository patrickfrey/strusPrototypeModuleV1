/*
 * Copyright (c) 2014 Patrick P. Frey
 * Copyright (c) 2015,2016 Andreas Baumann
 * Copyright (c) 2015,2016 Eurospider IT AG Zurich
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
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

strus::Index TestPostingIterator::length() const
{
	// TODO: length
	return 1;
}

strus::PostingIteratorInterface *PostingJoinOperatorTest::createResultIterator(
			const std::vector<strus::Reference<strus::PostingIteratorInterface> > &itrs,
			int range,
			unsigned int cardinality ) const
{
	if( range <= 0 || (unsigned int)range < itrs.size( ) ) {
		m_errorhnd->report( *strus::ErrorCode( strus::StrusComponentCore, strus::ErrorOperationBuildData, strus::ErrorCauseValueOutOfRange),
					_TXT( "range of 'test' is out of range" ) );
		return 0;
	}
	
	if( cardinality > itrs.size( ) ) {
		m_errorhnd->report( *strus::ErrorCode( strus::StrusComponentCore, strus::ErrorOperationBuildData, strus::ErrorCauseValueOutOfRange),
					_TXT( "ardinality of'test' is out of range" ) );
		return 0;
	}
	
	if( itrs.size( ) == 0 ) {
		m_errorhnd->report( *strus::ErrorCode( strus::StrusComponentCore, strus::ErrorOperationBuildData, strus::ErrorCauseIncompleteDefinition),
					_TXT( "too few arguments for 'test'" ) );
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
		strus::PostingJoinOperatorInterface::Description descr( "test", _TXT( "Demonstrating how to implement a 'test' posting join operator" ) );
		return descr;
	}
	CATCH_ERROR_ARG1_MAP_RETURN( _TXT( "error creating posting join operator description for '%s': %s" ), "test", *m_errorhnd,
		strus::PostingJoinOperatorInterface::Description( ) );
}

} // namespace test
