/*
 * Copyright (c) 2014 Patrick P. Frey
 * Copyright (c) 2015,2016 Andreas Baumann
 * Copyright (c) 2015,2016 Eurospider IT AG Zurich
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

/// \brief Implementation of helper functions shared by iterators
#include "postingIteratorHelpers.hpp"
#include "strus/errorBufferInterface.hpp"
#include "strus/private/internationalization.hpp"
#include "strus/private/errorUtils.hpp"
#include "strus/private/bitOperations.hpp"
#include <sstream>
#include <iostream>

using namespace strus;

Index strus::getFirstAllMatchDocno(
		std::vector<Reference< PostingIteratorInterface> >& args,
		Index docno,
		bool allowEmpty)
{
	if (args.empty()) return 0;
	
	Index docno_iter = docno;
	std::vector<Reference< PostingIteratorInterface> >::iterator
		ae = args.end();
	for (;;)
	{
		std::vector<Reference< PostingIteratorInterface> >::iterator
			ai = args.begin();

		docno_iter = (*ai)->skipDocCandidate( docno_iter);
		if (docno_iter == 0)
		{
			return 0;
		}
		for (++ai; ai != ae; ++ai)
		{
			Index docno_next = (*ai)->skipDocCandidate( docno_iter);
			if (docno_next == 0)
			{
				return 0;
			}
			if (docno_next != docno_iter)
			{
				docno_iter = docno_next;
				break;
			}
		}
		if (ai == ae)
		{
			if (!allowEmpty)
			{
				ai = args.begin();
				for (; ai != ae; ++ai)
				{
					if (docno_iter != (*ai)->skipDoc( docno_iter))
					{
						++docno_iter;
						continue;
					}
				}
			}
			return docno_iter;
		}
	}
}

Index strus::getFirstAllMatchDocnoSubset(
		std::vector<Reference< PostingIteratorInterface> >& args,
		Index docno,
		bool allowEmpty,
		std::size_t cardinality)
{
	if (args.empty()) return 0;
	if (args.size() > 64) throw std::runtime_error("number of arguments for getFirstAllMatchDocnoSubset out of range");

	Index docno_iter = docno;
	for (;;)
	{
		std::vector<Reference< PostingIteratorInterface> >::iterator
			ai = args.begin(), ae = args.end();

		std::size_t nof_matches = 0;
		Index match_docno = 0;
		uint64_t candidate_set = 0;

		for (unsigned int aidx = 0; ai != ae; ++ai,++aidx)
		{
			Index docno_next = (*ai)->skipDocCandidate( docno_iter);
			if (docno_next)
			{
				if (match_docno)
				{
					if (match_docno == docno_next)
					{
						candidate_set |= (uint64_t)1<<(aidx);
						++nof_matches;
					}
					else if (match_docno > docno_next)
					{
						candidate_set = (uint64_t)1<<(aidx);
						match_docno = docno_next;
						nof_matches = 1;
					}
				}
				else
				{
					candidate_set = (uint64_t)1<<(aidx);
					match_docno = docno_next;
					nof_matches = 1;
				}
			}
		}
		if (nof_matches >= cardinality)
		{
			if (!allowEmpty)
			{
				unsigned int aidx = BitOperations::bitScanForward( candidate_set);
				while (aidx)
				{
					if (match_docno != (*ai)->skipDoc( match_docno))
					{
						--nof_matches;
						if (nof_matches < cardinality) break;
					}
					candidate_set -= (uint64_t)1<<(aidx-1);
					aidx = BitOperations::bitScanForward( candidate_set);
				}
				if (nof_matches < cardinality)
				{
					docno_iter = match_docno+1;
					continue;
				}
			}
			return match_docno;
		}
		else if (match_docno)
		{
			docno_iter = match_docno+1;
		}
		else
		{
			break;
		}
	}
	return 0;
}

void strus::encodeInteger( std::string& buf, int val)
{
	std::ostringstream num;
	num << val;
	buf.append( num.str());
}

