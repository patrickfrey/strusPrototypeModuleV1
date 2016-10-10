/*
 * Copyright (c) 2014 Patrick P. Frey
 * Copyright (c) 2015,2016 Andreas Baumann
 * Copyright (c) 2015,2016 Eurospider IT AG Zurich
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "strus/storageModule.hpp"
#include "strus/base/dll_tags.hpp"
#include "operatortest.hpp"
#include "window_joinop.hpp"
#include "weightingtest.hpp"
#include "summarizertest.hpp"
#include "summarizertest2.hpp"
#include "weightingFormula.hpp"
#include "weightingMetadata2.hpp"

strus::PostingJoinOperatorInterface *createPostingIteratorTest_test( strus::ErrorBufferInterface *errorhnd )
{
	return new test::PostingJoinOperatorTest( errorhnd );
}

static const strus::PostingIteratorJoinConstructor operators[] =
{
	{ "test", createPostingIteratorTest_test },
	{ "minwin", strus::createWindowJoinOperator },
	{ 0, 0 }
};

strus::WeightingFunctionInterface *createWeightingFunction_test( strus::ErrorBufferInterface *errorhnd )
{
	return new test::WeightingFunctionTest( errorhnd );
}

strus::WeightingFunctionInterface *createWeightingFunction_formula( strus::ErrorBufferInterface *errorhnd )
{
	return new strus::WeightingFunctionFormula( errorhnd );
}

strus::WeightingFunctionInterface *createWeightingFunction_metadata2( strus::ErrorBufferInterface *errorhnd )
{
	return new strus::WeightingFunctionMetadata2( errorhnd );
}

static const strus::WeightingFunctionConstructor weightingFunctions[] =
{
	{ "test", createWeightingFunction_test },
	{ "formula_test", createWeightingFunction_formula },
	{ "metadata2", createWeightingFunction_metadata2 },
	{ 0, 0 }
};

strus::SummarizerFunctionInterface *createSummarizerFunction_test( strus::ErrorBufferInterface *errorhnd )
{
	return new test::SummarizerFunctionTest( errorhnd );
}

strus::SummarizerFunctionInterface *createSummarizerFunction_test2( strus::ErrorBufferInterface *errorhnd )
{
	return new test::SummarizerFunctionTest2( errorhnd );
}

static const strus::SummarizerFunctionConstructor summarizers[] =
{
	{ "test", createSummarizerFunction_test },
	{ "test2", createSummarizerFunction_test2 },
	{ 0, 0 }
};

extern "C" DLL_PUBLIC strus::StorageModule entryPoint;

strus::StorageModule entryPoint( operators, weightingFunctions, summarizers );
