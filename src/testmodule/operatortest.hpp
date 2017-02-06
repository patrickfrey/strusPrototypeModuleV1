/*
 * Copyright (c) 2014 Patrick P. Frey
 * Copyright (c) 2015,2016 Andreas Baumann
 * Copyright (c) 2015,2016 Eurospider IT AG Zurich
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
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

		virtual strus::Index length( ) const;
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
