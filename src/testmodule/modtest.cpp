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

#include "strus/storageModule.hpp"
#include "strus/private/dll_tags.hpp"
#include "operatortest.hpp"
#include "weightingtest.hpp"
#include "summarizertest.hpp"

static const strus::PostingIteratorJoinConstructor operators[] =
{
	{ 0, 0 }
};

strus::WeightingFunctionInterface *createWeightingFunction_test( strus::ErrorBufferInterface *errorhnd )
{
	return new test::WeightingFunctionTest( errorhnd );
}

static const strus::WeightingFunctionConstructor weightingFunctions[] =
{
	{ "test", createWeightingFunction_test },
	{ 0, 0 }
};

strus::SummarizerFunctionInterface *createSummarizerFunction_test( strus::ErrorBufferInterface *errorhnd )
{
	return new test::SummarizerFunctionTest( errorhnd );
}

static const strus::SummarizerFunctionConstructor summarizers[] =
{
	{ "test", createSummarizerFunction_test },
	{ 0, 0 }
};

extern "C" DLL_PUBLIC strus::StorageModule entryPoint;

strus::StorageModule entryPoint( operators, weightingFunctions, summarizers );
