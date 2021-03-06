// BEGIN LICENSE BLOCK
/*
Copyright  2009 , UT-Battelle, LLC
All rights reserved

[FreeFermions, Version 2.0.0]
[by G.A., Oak Ridge National Laboratory]

UT Battelle Open Source Software License 11242008

OPEN SOURCE LICENSE

Subject to the conditions of this License, each
contributor to this software hereby grants, free of
charge, to any person obtaining a copy of this software
and associated documentation files (the "Software"), a
perpetual, worldwide, non-exclusive, no-charge,
royalty-free, irrevocable copyright license to use, copy,
modify, merge, publish, distribute, and/or sublicense
copies of the Software.

1. Redistributions of Software must retain the above
copyright and license notices, this list of conditions,
and the following disclaimer.  Changes or modifications
to, or derivative works of, the Software should be noted
with comments and the contributor and organization's
name.

2. Neither the names of UT-Battelle, LLC or the
Department of Energy nor the names of the Software
contributors may be used to endorse or promote products
derived from this software without specific prior written
permission of UT-Battelle.

3. The software and the end-user documentation included
with the redistribution, with or without modification,
must include the following acknowledgment:

"This product includes software produced by UT-Battelle,
LLC under Contract No. DE-AC05-00OR22725  with the
Department of Energy."

*********************************************************
DISCLAIMER

THE SOFTWARE IS SUPPLIED BY THE COPYRIGHT HOLDERS AND
CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
COPYRIGHT OWNER, CONTRIBUTORS, UNITED STATES GOVERNMENT,
OR THE UNITED STATES DEPARTMENT OF ENERGY BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
DAMAGE.

NEITHER THE UNITED STATES GOVERNMENT, NOR THE UNITED
STATES DEPARTMENT OF ENERGY, NOR THE COPYRIGHT OWNER, NOR
ANY OF THEIR EMPLOYEES, REPRESENTS THAT THE USE OF ANY
INFORMATION, DATA, APPARATUS, PRODUCT, OR PROCESS
DISCLOSED WOULD NOT INFRINGE PRIVATELY OWNED RIGHTS.

*********************************************************


*/
// END LICENSE BLOCK
/** \ingroup DMRG */
/*@{*/

/*! \file OperatorFactory.h
 *
 *
 *
 */
#ifndef OPERATOR_FACTORY_H
#define OPERATOR_FACTORY_H
#include "Concurrency.h"

namespace FreeFermions {

template<typename OpType>
class OperatorFactory {
	typedef typename OpType::EngineType EngineType;
	typedef OperatorFactory<OpType> ThisType;
	typedef typename PsimagLite::Vector<OpType*>::Type GarbageType;

public:

	OperatorFactory(const EngineType& engine)
	    : engine_(&engine),
	      garbage_(PsimagLite::Concurrency::npthreads)
	{}

	~OperatorFactory()
	{
		for (SizeType i=0;i<garbage_.size();i++)
			for (SizeType j=0;j<garbage_[i].size(); ++j)
				delete garbage_[i][j];
	}

	OpType& operator()(SizeType x,
	                   SizeType site,
	                   SizeType sigma,
	                   SizeType threadId = 0)
	{
		OpType* op = new OpType(*engine_,x,site,sigma);
		garbage_[threadId].push_back(op);
		return *op;
	}

	template<typename SomeOtherType>
	OpType& operator()(SomeOtherType& x, SizeType threadId = 0)
	{
		OpType* op = new OpType(x);
		garbage_[threadId].push_back(op);
		return *op;
	}

	OpType& operator()(const OpType* op, SizeType threadId = 0)
	{
		OpType* op2 = new OpType(op);
		garbage_[threadId].push_back(op2);
		return *op2;
	}

private:

	const EngineType* engine_;
	typename PsimagLite::Vector<GarbageType>::Type garbage_;

}; // OperatorFactory
} // namespace Dmrg

/*@}*/
#endif // OPERATOR_FACTORY_H

