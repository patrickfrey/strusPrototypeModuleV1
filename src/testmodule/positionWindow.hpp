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

#ifndef _STRUS_POSITION_WINDOW_HPP_INCLUDED
#define _STRUS_POSITION_WINDOW_HPP_INCLUDED
#include "strus/index.hpp"
#include "strus/postingIteratorInterface.hpp"
#include <set>

namespace strus {

// Sliding window to iterate on the sets of posting iterator elements 
// that are withing a defined proximity range:
class PositionWindow
{
public:
	// Element in the sliding position window implemented as set:
	struct Element
	{
		PostingIteratorInterface* itr;	// ... reference this element belongs to
		Index pos;			// ... position of this element

		Element()
			:itr(0),pos(0){}
		Element( PostingIteratorInterface* itr_, Index pos_)
			:itr(itr_),pos(pos_){}
		Element( const Element& o)
			:itr(o.itr),pos(o.pos){}

		bool operator < ( const Element& o) const
		{
			if (pos == o.pos)
			{
				return itr < o.itr;
			}
			return pos < o.pos;
		}
	};

	// Constructor that fills the sliding window implemented as set 
	// with the argument element start positions:
	PositionWindow( 
		const std::vector<PostingIteratorInterface*>& args,
		unsigned int range_,
		unsigned int cardinality_,
		Index firstpos_);

	// Check if there is any valid window:
	bool first();

	// Skip to the next window and return true, if there is one more:
	bool next();

	// Return the size of the current window:
	unsigned int size();

	// Return the starting position of the current window:
	unsigned int pos();

private:
	// Get the top element of the current window:
	std::set<Element>::iterator getWinTopElement();

private:
	std::vector<PostingIteratorInterface*> m_args;
	std::set<Element> m_set;	// ... set used for sliding window
	unsigned int m_setsize;		// ... current size of m_set
	unsigned int m_range;		// ... maximum proximity range
	unsigned int m_cardinality;	// ... number of elements for a candidate window
};

}//namespace
#endif

