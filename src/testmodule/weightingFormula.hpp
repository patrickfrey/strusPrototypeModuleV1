/*
 * Copyright (c) 2014 Patrick P. Frey
 * Copyright (c) 2015,2016 Andreas Baumann
 * Copyright (c) 2015,2016 Eurospider IT AG Zurich
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
 
///\file weightingFormula.hpp
///\brief Implementation of a weighting function defined as formula in a string
#ifndef _STRUS_WEIGHTING_FORMULA_HPP_INCLUDED
#define _STRUS_WEIGHTING_FORMULA_HPP_INCLUDED
#include "formulaInterpreter.hpp"
#include "strus/weightingFunctionInterface.hpp"
#include "strus/weightingFunctionInstanceInterface.hpp"
#include "strus/weightingFunctionContextInterface.hpp"
#include "strus/metaDataReaderInterface.hpp"
#include "strus/storageClientInterface.hpp"
#include "strus/index.hpp"
#include "strus/postingIteratorInterface.hpp"
#include "private/utils.hpp"
#include "private/internationalization.hpp"
#include <vector>
#include <sstream>
#include <iostream>
#include <iomanip>

namespace strus
{
/// \brief Forward declaration
class ErrorBufferInterface;

class FunctionMap
	:public FormulaInterpreter::FunctionMap
{
public:
	FunctionMap();

public:
	static FormulaInterpreter::IteratorSpec dimMap( void* ctx, const char* type);
	static double variableMap_metadata( void* ctx, int typeidx, unsigned int idx);
	static double variableMap_param( void* ctx, int typeidx, unsigned int idx);
	static double variableMap_df( void* ctx, int typeidx, unsigned int idx);
	static double variableMap_ff( void* ctx, int typeidx, unsigned int idx);
	static double variableMap_weight( void* ctx, int typeidx, unsigned int idx);
	static double variableMap_match( void* ctx, int typeidx, unsigned int idx);
	static double unaryFunction_minus( double arg);
	static double unaryFunction_log( double arg);
	static double unaryFunction_log10( double arg);
	static double unaryFunction_sgn( double arg);
	static double unaryFunction_sqrt( double arg);
	static double binaryFunction_minus( double arg1, double arg2);
	static double binaryFunction_plus( double arg1, double arg2);
	static double binaryFunction_mul( double arg1, double arg2);
	static double binaryFunction_div( double arg1, double arg2);
	static double binaryFunction_pow( double arg1, double arg2);
	static double binaryFunction_min( double arg1, double arg2);
	static double binaryFunction_max( double arg1, double arg2);
	static std::pair<unsigned int, unsigned int> weightingFunction_minwin( void* ctx, int typeidx, int range, int cardinality);
	static double weightingFunction_minwinsize2( void* ctx, int typeidx, int range, int cardinality);
	static double weightingFunction_minwinpos2( void* ctx, int typeidx, int range, int cardinality);
};

/// \class WeightingFunctionContextFormula
/// \brief Weighting function based on a formula defined as string
class WeightingFunctionContextFormula
	:public WeightingFunctionContextInterface
{
public:
	WeightingFunctionContextFormula(
		const StorageClientInterface* storage,
		MetaDataReaderInterface* metadata_,
		const FormulaInterpreter::FunctionMap& functionMap,
		const std::string& formula,
		const std::vector<double>& paramar,
		ErrorBufferInterface* errorhnd_);

	class Feature
	{
	public:
		Feature( const Feature& o)
			:m_itr(o.m_itr),m_weight(o.m_weight),m_df(o.m_df){}
		Feature( PostingIteratorInterface* itr_, double weight_, const TermStatistics& stats_)
			:m_itr(itr_),m_weight(weight_),m_df(stats_.documentFrequency()>=0?stats_.documentFrequency():std::numeric_limits<double>::quiet_NaN()){}

		double df() const
		{
			if (std::isnan(m_df))
			{
				m_df = m_itr->documentFrequency();
			}
			return m_df;
		}
		double ff() const
		{
			return m_match?m_itr->frequency():0.0;
		}
		double weight() const
		{
			return m_match?m_weight:0.0;
		}
		double match() const
		{
			return m_match?1.0:0.0;
		}
		
		PostingIteratorInterface* itr() const
		{
			return m_itr;
		}

		void skipDoc( Index docno)
		{
			m_match = (docno == m_itr->skipDoc( docno));
		}
		

	private:
		PostingIteratorInterface* m_itr;
		double m_weight;
		mutable double m_df;
		bool m_match;
	};

	virtual void addWeightingFeature(
			const std::string& name_,
			PostingIteratorInterface* itr_,
			double weight_,
			const TermStatistics& stats_);

	virtual void setVariableValue( const std::string& name, double value);

	virtual double call( const Index& docno);

	virtual std::string debugCall( const strus::Index& docno);

private:
	friend class FunctionMap;
	typedef std::vector<Feature> FeatureVector;
	std::vector<double> m_paramar;
	std::vector<FeatureVector> m_featar;
	std::map<std::string,std::size_t> m_sets;
	MetaDataReaderInterface* m_metadata;
	FormulaInterpreter m_interpreter;
	ErrorBufferInterface* m_errorhnd;				///< buffer for error messages
};


/// \class WeightingFunctionInstanceFormula
/// \brief Weighting function instance based on a formula define as string
class WeightingFunctionInstanceFormula
	:public WeightingFunctionInstanceInterface
{
public:
	explicit WeightingFunctionInstanceFormula( ErrorBufferInterface* errorhnd_)
		:m_errorhnd(errorhnd_){}

	virtual ~WeightingFunctionInstanceFormula(){}

	virtual void addStringParameter( const std::string& name, const std::string& value);
	virtual void addNumericParameter( const std::string& name, const NumericVariant& value);

	virtual std::vector<std::string> getVariables() const
	{
		return std::vector<std::string>();
	}

	virtual WeightingFunctionContextInterface* createFunctionContext(
			const StorageClientInterface* storage_,
			MetaDataReaderInterface* metadata,
			const GlobalStatistics& stats) const;

	virtual std::string tostring() const;

private:
	FunctionMap m_functionmap;
	std::string m_formula;
	std::vector<double> m_paramar;
	ErrorBufferInterface* m_errorhnd;				///< buffer for error messages
};


/// \class WeightingFunctionFormula
/// \brief Weighting function based on a formula defined as string
class WeightingFunctionFormula
	:public WeightingFunctionInterface
{
public:
	explicit WeightingFunctionFormula( ErrorBufferInterface* errorhnd_)
		:m_errorhnd(errorhnd_){}

	virtual ~WeightingFunctionFormula(){}

	virtual WeightingFunctionInstanceInterface* createInstance(const QueryProcessorInterface* processor) const;

	virtual FunctionDescription getDescription() const;

private:
	ErrorBufferInterface* m_errorhnd;				///< buffer for error messages
};

}//namespace
#endif

