#include "strus/storageModule.hpp"
#include "strus/private/dll_tags.hpp"
#include "operatortest.hpp"
#include "weightingtest.hpp"
#include "summarizertest.hpp"

static const strus::PostingIteratorJoinConstructor operators[] =
{
	{ 0, 0 }
};

static const strus::WeightingFunctionConstructor weightingFunctions[] =
{
	{ 0, 0 }
};

static const strus::SummarizerFunctionConstructor summarizers[] =
{
	{ 0, 0 }
};

extern "C" DLL_PUBLIC strus::StorageModule entryPoint;

strus::StorageModule entryPoint( operators, weightingFunctions, summarizers );
