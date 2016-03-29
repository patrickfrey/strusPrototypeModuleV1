/*
 * Copyright (c) 2014 Patrick P. Frey
 * Copyright (c) 2015,2016 Andreas Baumann
 * Copyright (c) 2015,2016 Eurospider IT AG Zurich
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef _STRUS_POSTING_JOIN_OPERATOR_WINDOW_HPP_INCLUDED
#define _STRUS_POSTING_JOIN_OPERATOR_WINDOW_HPP_INCLUDED

namespace strus {

// Forward declarations:
class PostingJoinOperatorInterface;
class ErrorBufferInterface;

// Window iterator constructor:
PostingJoinOperatorInterface* createWindowJoinOperator(
	ErrorBufferInterface* errorhnd);

}//namespace
#endif

