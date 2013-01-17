

// SAmple of how to use FreeFermions core engine to calculate
// <c^\dagger_i c_j >
#include <cstdlib>
#include "Engine.h"
#include "GeometryLibrary.h"
#include "ConcurrencySerial.h"
#include "TypeToString.h"
#include "CreationOrDestructionOp.h"
#include "HilbertState.h"
#include "EtoTheBetaH.h"
#include "DiagonalOperator.h"
#include "Tokenizer.h"
#include "LibraryOperator.h"
#include "Combinations.h"
#include "GeometryParameters.h"
#include "DriverHelper.h"

typedef double RealType;
typedef RealType FieldType;
typedef PsimagLite::ConcurrencySerial<RealType> ConcurrencyType;
typedef PsimagLite::Matrix<RealType> MatrixType;
typedef FreeFermions::GeometryParameters<RealType> GeometryParamsType;
typedef FreeFermions::GeometryLibrary<MatrixType,GeometryParamsType> GeometryLibraryType;
typedef FreeFermions::Engine<RealType,FieldType,ConcurrencyType> EngineType;
typedef FreeFermions::CreationOrDestructionOp<EngineType> OperatorType;
typedef FreeFermions::EToTheBetaH<EngineType> EtoTheBetaHType;
typedef FreeFermions::DiagonalOperator<EtoTheBetaHType> DiagonalOperatorType;
typedef FreeFermions::HilbertState<OperatorType,DiagonalOperatorType> HilbertStateType;
typedef DiagonalOperatorType::FactoryType OpDiagonalFactoryType;
typedef OperatorType::FactoryType OpNormalFactoryType;
typedef FreeFermions::LibraryOperator<OperatorType> LibraryOperatorType;
typedef LibraryOperatorType::FactoryType OpLibFactoryType;
typedef FreeFermions::DriverHelper<GeometryLibraryType> DriverHelperType;

RealType fermi(const RealType& x)
{
	return 1.0/(1.0+exp(x));
}

int main(int argc,char *argv[])
{
	int opt;
	size_t n =0;
	RealType beta = 0;
	RealType mu=0;
	std::vector<RealType> v;
	std::vector<std::string> str;
	GeometryParamsType geometryParams;

	while ((opt = getopt(argc, argv, "n:p:m:b:g:")) != -1) {
		switch (opt) {
			case 'n':
				n = atoi(optarg);
				geometryParams.sites = n;
				break;
			case 'p':
				DriverHelperType::readPotential(v,optarg);
				break;
			case 'm':
				mu = atof(optarg);
				break;
			case 'b':
				beta = atof(optarg);
				break;
			case 'g':
				str.clear();
				PsimagLite::tokenizer(optarg,str,",");
				DriverHelperType::setMyGeometry(geometryParams,str);
				break;
			default: /* '?' */
				DriverHelperType::usage(argv[0],"-n sites -g geometry,[leg,filename]");
				throw std::runtime_error("Wrong usage\n");
		}
	}

	if (n==0) {
		DriverHelperType::usage(argv[0],"-n sites -g geometry,[leg,filename]");
		throw std::runtime_error("Wrong usage\n");
	}

	size_t dof = 1; // spinless
	GeometryLibraryType geometry(geometryParams);

	geometry.addPotential(v);
	geometry.addPotential(mu);

	std::cerr<<v;

	ConcurrencyType concurrency(argc,argv);
	EngineType engine(geometry,concurrency,dof,true);

	std::cout<<geometry;
	std::cout<<"#beta="<<beta<<" mu="<<mu<<"\n";

	for (size_t i=0;i<n;i++) {
		for (size_t j=0;j<n;j++) {
			RealType value = 0;
			for (size_t k=0;k<n;k++)
				value += std::conj(engine.eigenvector(i,k))*engine.eigenvector(j,k)*
						fermi(beta*(engine.eigenvalue(k)-mu));
			std::cout<<value<<" ";
		}
		std::cout<<"\n";
	}

}
