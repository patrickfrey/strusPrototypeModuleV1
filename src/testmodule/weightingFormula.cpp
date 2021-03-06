/*
 * Copyright (c) 2014 Patrick P. Frey
 * Copyright (c) 2015,2016 Andreas Baumann
 * Copyright (c) 2015,2016 Eurospider IT AG Zurich
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "weightingFormula.hpp"
#include "strus/errorBufferInterface.hpp"
#include "private/internationalization.hpp"
#include "private/errorUtils.hpp"
#include "private/utils.hpp"
#include "strus/constants.hpp"
#include "positionWindow.hpp"
#include <cmath>
#include <ctime>
#include <algorithm>
#include <boost/math/special_functions/sign.hpp>

using namespace strus;


FunctionMap::FunctionMap()
	:FormulaInterpreter::FunctionMap( &dimMap)
{
	defineVariableMap( "df", &variableMap_df);
	defineVariableMap( "ff", &variableMap_ff);
	defineVariableMap( "weight", &variableMap_weight);
	defineUnaryFunction( "log10", &unaryFunction_log10);
	defineUnaryFunction( "log", &unaryFunction_log);
	defineUnaryFunction( "-", &unaryFunction_minus);
	defineUnaryFunction( "sgn", &unaryFunction_sgn);
	defineUnaryFunction( "sqrt", &unaryFunction_sqrt);
	defineBinaryFunction( "-", &binaryFunction_minus);
	defineBinaryFunction( "+", &binaryFunction_plus);
	defineBinaryFunction( "*", &binaryFunction_mul);
	defineBinaryFunction( "/", &binaryFunction_div);
	defineBinaryFunction( "pow", &binaryFunction_pow);
	defineBinaryFunction( "min", &binaryFunction_min);
	defineBinaryFunction( "max", &binaryFunction_max);
	defineWeightingFunction( "minwinsize", &weightingFunction_minwinsize2);
	defineWeightingFunction( "minwinpos", &weightingFunction_minwinpos2);
}

FormulaInterpreter::IteratorSpec FunctionMap::dimMap( void* ctx, const char* type)
{
	WeightingFunctionContextFormula* THIS = (WeightingFunctionContextFormula*)ctx;
	std::map<std::string,std::size_t>::const_iterator ti = THIS->m_sets.find( type);
	if (ti == THIS->m_sets.end()) return FormulaInterpreter::IteratorSpec();
	return FormulaInterpreter::IteratorSpec( ti->second, THIS->m_featar[ ti->second].size());
}

double FunctionMap::variableMap_metadata( void* ctx, int typeidx, unsigned int idx)
{
	WeightingFunctionContextFormula* THIS = (WeightingFunctionContextFormula*)ctx;
	NumericVariant val = THIS->m_metadata->getValue( idx);
	if (val.defined())
	{
		return (double)val;
	}
	else
	{
		return std::numeric_limits<double>::quiet_NaN();
	}
}

double FunctionMap::variableMap_param( void* ctx, int typeidx, unsigned int idx)
{
	WeightingFunctionContextFormula* THIS = (WeightingFunctionContextFormula*)ctx;
	if (idx >= THIS->m_paramar.size()) return std::numeric_limits<double>::quiet_NaN();
	return THIS->m_paramar[idx];
}

double FunctionMap::variableMap_df( void* ctx, int typeidx, unsigned int idx)
{
	WeightingFunctionContextFormula* THIS = (WeightingFunctionContextFormula*)ctx;
	if (typeidx < 0) return std::numeric_limits<double>::quiet_NaN();
	if (idx >= THIS->m_featar[ typeidx].size()) return 0;
	return THIS->m_featar[ typeidx][idx].df();
}

double FunctionMap::variableMap_ff( void* ctx, int typeidx, unsigned int idx)
{
	WeightingFunctionContextFormula* THIS = (WeightingFunctionContextFormula*)ctx;
	if (typeidx < 0) return std::numeric_limits<double>::quiet_NaN();
	if (idx >= THIS->m_featar[ typeidx].size()) return 0;
	return THIS->m_featar[ typeidx][idx].ff();
}

double FunctionMap::variableMap_weight( void* ctx, int typeidx, unsigned int idx)
{
	WeightingFunctionContextFormula* THIS = (WeightingFunctionContextFormula*)ctx;
	if (typeidx < 0) return std::numeric_limits<double>::quiet_NaN();
	if (idx >= THIS->m_featar[ typeidx].size()) return std::numeric_limits<double>::quiet_NaN();
	return THIS->m_featar[ typeidx][idx].weight();
}

double FunctionMap::variableMap_match( void* ctx, int typeidx, unsigned int idx)
{
	WeightingFunctionContextFormula* THIS = (WeightingFunctionContextFormula*)ctx;
	if (typeidx < 0) return std::numeric_limits<double>::quiet_NaN();
	if (idx >= THIS->m_featar[ typeidx].size()) return std::numeric_limits<double>::quiet_NaN();
	return THIS->m_featar[ typeidx][idx].match();
}

double FunctionMap::unaryFunction_minus( double arg)
{
	return -arg;
}

double FunctionMap::unaryFunction_log10( double arg)
{
	return std::log10( arg);
}

double FunctionMap::unaryFunction_log( double arg)
{
	return std::log( arg);
}

double FunctionMap::unaryFunction_sgn( double arg )
{
	return (double)boost::math::sign( arg );
}

double FunctionMap::unaryFunction_sqrt( double arg )
{
	return sqrt( arg );
}

double FunctionMap::binaryFunction_minus( double arg1, double arg2)
{
	return arg1 - arg2;
}

double FunctionMap::binaryFunction_plus( double arg1, double arg2)
{
	return arg1 + arg2;
}

double FunctionMap::binaryFunction_mul( double arg1, double arg2)
{
	return arg1 * arg2;
}

double FunctionMap::binaryFunction_div( double arg1, double arg2)
{
	return arg1 / arg2;
}

double FunctionMap::binaryFunction_pow( double arg1, double arg2)
{
	return pow( arg1, arg2 );
}

double FunctionMap::binaryFunction_min( double arg1, double arg2)
{
	return std::min( arg1, arg2 );
}

double FunctionMap::binaryFunction_max( double arg1, double arg2)
{
	return std::max( arg1, arg2 );
}

std::pair<unsigned int, unsigned int> FunctionMap::weightingFunction_minwin( void* ctx, int typeidx, int range, int cardinality)
{
	WeightingFunctionContextFormula* THIS = (WeightingFunctionContextFormula*)ctx;	
	if (typeidx < 0) return std::make_pair( 999999, 999999 );
	std::vector<PostingIteratorInterface*> itrs;	
	for( std::vector<WeightingFunctionContextFormula::Feature>::const_iterator fitr = THIS->m_featar[typeidx].begin( ); fitr != THIS->m_featar[typeidx].end( ); fitr++ ) {
		WeightingFunctionContextFormula::Feature feat = *fitr;
		PostingIteratorInterface *itr = feat.itr( );
		itrs.push_back( itr );
	}
	
	// special case one query word, length of window is 1, position is the
	// first occurence of the one query word
	if( itrs.size( ) == 1 ) {
		std::vector<PostingIteratorInterface*>::iterator itr = itrs.begin( );
		Index min_pos = (*itr)->skipPos( 0 );
		return std::make_pair( min_pos, 1 );
	}
	unsigned int min_pos = 999999;
	unsigned int min_size = 999999;
	PositionWindow win( itrs, range, cardinality, 0 );
	if( !win.first( ) ) return std::make_pair( min_pos, min_size ); // no windows exists
	do {
		unsigned int winSize = win.size( );
		if( winSize < min_size ) {
			min_size = winSize;
			min_pos = win.pos( );
		}
	} while( win.next( ) );
	return std::make_pair( min_pos, min_size );
}

double FunctionMap::weightingFunction_minwinpos2( void* ctx, int typeidx, int range, int cardinality)
{
	return weightingFunction_minwin( ctx, typeidx, range, cardinality ).first;
}

double FunctionMap::weightingFunction_minwinsize2( void* ctx, int typeidx, int range, int cardinality)
{
	return weightingFunction_minwin( ctx, typeidx, range, cardinality ).second;
}


WeightingFunctionContextFormula::WeightingFunctionContextFormula(
		const StorageClientInterface* storage,
		MetaDataReaderInterface* metadata_,
		const FormulaInterpreter::FunctionMap& functionMap,
		const std::string& formula,
		const std::vector<double>& paramar,
		ErrorBufferInterface* errorhnd_)
	:m_paramar(paramar)
	,m_featar()
	,m_sets()
	,m_metadata(metadata_)
	,m_interpreter( functionMap, formula)
	,m_errorhnd(errorhnd_)
{}

void WeightingFunctionContextFormula::addWeightingFeature(
		const std::string& name_,
		PostingIteratorInterface* itr_,
		double weight_,
		const TermStatistics& stats_)
{
	try
	{
		std::size_t idx;
		std::map<std::string,std::size_t>::const_iterator ti = m_sets.find( name_);
		if (ti == m_sets.end())
		{
			m_sets[ name_] = idx = m_featar.size();
			m_featar.push_back( FeatureVector());
		}
		else
		{
			idx = ti->second;
		}
		m_featar[ idx].push_back( Feature( itr_, weight_, stats_));
	}
	CATCH_ERROR_ARG1_MAP( _TXT("error adding feature to weighting function '%s': %s"), "formula", *m_errorhnd);
}

void WeightingFunctionContextFormula::setVariableValue( const std::string& name, double value)
{
	m_errorhnd->report( strus::ErrorCodeNotImplemented,
				_TXT("no variables known for function '%s'"), "formula");
}

double WeightingFunctionContextFormula::call( const Index& docno)
{
	std::vector<FeatureVector>::iterator vi = m_featar.begin(), ve = m_featar.end();
	for (; vi != ve; ++vi)
	{
		FeatureVector::iterator fi = vi->begin(), fe = vi->end();
		for (; fi != fe; ++fi)
		{
			fi->skipDoc( docno);
		}
	}
	m_metadata->skipDoc( docno );
	try
	{
		return m_interpreter.run( this);
	}
	CATCH_ERROR_ARG1_MAP_RETURN( _TXT("error calling weighting function '%s': %s"), "formula", *m_errorhnd, 0.0);
}

std::string WeightingFunctionContextFormula::debugCall( const strus::Index& docno)
{
	return "n/a";
}

WeightingFunctionContextInterface* WeightingFunctionInstanceFormula::createFunctionContext(
		const StorageClientInterface* storage_,
		MetaDataReaderInterface* metadata,
		const GlobalStatistics& stats) const
{
	try
	{
		FormulaInterpreter::FunctionMap funcmap( m_functionmap);
		std::vector<double> paramar( m_paramar);
		Index ii=0,nn = metadata->nofElements();
		for (;ii<nn;++ii)
		{
			const char* name = metadata->getName( ii);
			funcmap.defineVariableMap( name, FormulaInterpreter::VariableMap( &FunctionMap::variableMap_metadata, ii));
		}
		GlobalCounter nofdocs = stats.nofDocumentsInserted()>=0?stats.nofDocumentsInserted():(GlobalCounter)storage_->nofDocumentsInserted();
		funcmap.defineVariableMap( "nofdocs", FormulaInterpreter::VariableMap( &FunctionMap::variableMap_param, m_paramar.size()));
		paramar.push_back( (double)nofdocs);

		if (m_formula.empty())
		{
			throw strus::runtime_error(_TXT("no weighting formula defined with string parameter 'formula'"));
		}
		return new WeightingFunctionContextFormula( storage_, metadata, funcmap, m_formula, paramar, m_errorhnd);
	}
	CATCH_ERROR_ARG1_MAP_RETURN( _TXT("error creating context of weighting function '%s': %s"), "formula", *m_errorhnd, 0);
}

void WeightingFunctionInstanceFormula::addStringParameter( const std::string& name, const std::string& value)
{
	try
	{
		if (utils::caseInsensitiveEquals( name, "formula"))
		{
			m_formula = value;
			if (value.empty()) throw strus::runtime_error( _TXT("empty value passed as '%s' weighting function parameter '%s'"), "formula", name.c_str());
		}
		else
		{
			throw strus::runtime_error( _TXT("unknown '%s' weighting function parameter '%s'"), "formula", name.c_str());
		}
	}
	CATCH_ERROR_ARG1_MAP( _TXT("error adding string parameter to weighting function '%s': %s"), "formula", *m_errorhnd);
}

void WeightingFunctionInstanceFormula::addNumericParameter( const std::string& name, const NumericVariant& value)
{
	try
	{
		m_functionmap.defineVariableMap( name, FormulaInterpreter::VariableMap( &FunctionMap::variableMap_param, m_paramar.size()));
		m_paramar.push_back( (double)value);
	}
	CATCH_ERROR_ARG1_MAP( _TXT("error adding numeric parameter to weighting function '%s': %s"), "formula", *m_errorhnd);
}

std::string WeightingFunctionInstanceFormula::tostring() const
{
	try
	{
		return m_formula + "\n--\n" + m_functionmap.tostring();
	}
	CATCH_ERROR_ARG1_MAP_RETURN( _TXT("error mapping weighting function '%s' to string: %s"), "formula", *m_errorhnd, std::string());
}


WeightingFunctionInstanceInterface* WeightingFunctionFormula::createInstance(const QueryProcessorInterface* processor) const
{
	try
	{
		return new WeightingFunctionInstanceFormula( m_errorhnd);
	}
	CATCH_ERROR_ARG1_MAP_RETURN( _TXT("error creating instance of weighting function '%s': %s"), "formula", *m_errorhnd, 0);
}

FunctionDescription WeightingFunctionFormula::getDescription() const
{
	try
	{
		FunctionDescription rt( _TXT("Calculate the weight of a document with a formula (EIT version)"));
		rt( FunctionDescription::Parameter::Feature, "match", _TXT( "defines the query features referenced in the formula to weight"), "");
		rt( FunctionDescription::Parameter::String, "formula", _TXT( "defines an expression to evaluate. You can use the operators '*','/','+','-' and the functions 'log'. Mixing operators of different precedence is only allowed using brackets '(' and ')'. The variables 'weight','ff' and 'df' can be used besides all variables specified as parameters or as meta data elements"), "");
		rt( FunctionDescription::Parameter::Numeric, "[a-z]+", _TXT( "defines a variable to be used in the formula expression"), "");
		return rt;
	}
	CATCH_ERROR_ARG1_MAP_RETURN( _TXT("error creating weighting function description for '%s': %s"), "formula", *m_errorhnd, strus::FunctionDescription());
}

