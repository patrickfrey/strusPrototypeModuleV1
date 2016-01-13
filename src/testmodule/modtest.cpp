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
