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

#include "positionWindow.hpp"
#include "strus/postingJoinOperatorInterface.hpp"
#include "strus/postingIteratorInterface.hpp"
#include "strus/errorBufferInterface.hpp"

using namespace strus;

// Constructor that fills the sliding window implemented as set 
// with the argument element start positions:
PositionWindow::PositionWindow(
		const std::vector<PostingIteratorInterface*>& args,
		unsigned int range_,
		unsigned int cardinality_,
		Index firstpos_)
	:m_args(args)
	,m_setsize(0)
	,m_range(range_)
	,m_cardinality(cardinality_>0?cardinality_:args.size())
{
	std::vector<PostingIteratorInterface*>::iterator
		ai = m_args.begin(), ae = m_args.end();
	for (; ai != ae; ++ai)
	{
		PostingIteratorInterface* itr = *ai;
		Index pos = itr->skipPos( firstpos_);
		if (pos)
		{
			m_set.insert( Element( itr, pos));
			++m_setsize;
		}
	}
}

// Get the top element of the current window:
std::set<PositionWindow::Element>::iterator PositionWindow::getWinTopElement()
{
	unsigned int ii=0;
	std::set<Element>::iterator
		si = m_set.begin(), se = m_set.end();
	for (; ii<m_cardinality && si != se; ++ii,++si){}
	return si;
}

// Check if there is any valid window:
bool PositionWindow::first()
{
	// Return, if there is valid window left:
	return m_setsize >= m_cardinality;
}

// Skip to the next window and return true, if there is one more:
bool PositionWindow::next()
{
	// Calculate how many positions we can skip with the 
	// first element without loosing any window of a size
	// within the defined proximity range:
	PostingIteratorInterface* itr = m_set.begin()->itr;
	std::set<Element>::iterator top = getWinTopElement();
	Index posdiff = top->pos - m_set.begin()->pos;
	Index skipsize = posdiff > (Index)m_range ? (posdiff - (Index)m_range) : 1;
	// Change the position of the first element in the 
	// sliding window by the calculated size:
	Index newpos = itr->skipPos( m_set.begin()->pos + skipsize);
	if (newpos)
	{
		m_set.insert( Element( itr, newpos));
	}
	else
	{
		--m_setsize;
	}
	m_set.erase( m_set.begin());
	// Return, if there is valid window left:
	return m_setsize >= m_cardinality;
}

// Return the size of the current window:
unsigned int PositionWindow::size()
{
	std::set<Element>::iterator si = getWinTopElement();
	return si->pos - m_set.begin()->pos;
}

// Return the starting position of the current window:
unsigned int PositionWindow::pos()
{
	return (m_setsize >= m_cardinality)?m_set.begin()->pos:0;
}

