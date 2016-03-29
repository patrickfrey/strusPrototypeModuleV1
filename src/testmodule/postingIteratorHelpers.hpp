/*
 * Copyright (c) 2014 Patrick P. Frey
 * Copyright (c) 2015,2016 Andreas Baumann
 * Copyright (c) 2015,2016 Eurospider IT AG Zurich
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Helper functions shared by iterators

#ifndef _STRUS_ITERATOR_HELPERS_HPP_INCLUDED
#define _STRUS_ITERATOR_HELPERS_HPP_INCLUDED
#include "strus/index.hpp"
#include "strus/reference.hpp"
#include "strus/postingIteratorInterface.hpp"
#include <vector>
#include <string>

namespace strus
{

Index getFirstAllMatchDocno(
		std::vector<Reference< PostingIteratorInterface> >& args,
		Index docno,
		bool allowEmpty);

Index getFirstAllMatchDocnoSubset(
		std::vector<Reference< PostingIteratorInterface> >& args,
		Index docno,
		bool allowEmpty,
		std::size_t cardinality);

void encodeInteger( std::string& buf, int val);

}//namespace
#endif

