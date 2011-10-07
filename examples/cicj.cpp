

// SAmple of how to use FreeFermions core engine to calculate
// <c^\dagger_i c_j >
#include <cstdlib>
#include "Engine.h"
#include "GeometryLibrary.h"
#include "ConcurrencySerial.h"
#include "TypeToString.h"
#include "CreationOrDestructionOp.h"
#include "HilbertState.h"

typedef double RealType;
typedef std::complex<double> ComplexType;
typedef RealType FieldType;
typedef PsimagLite::ConcurrencySerial<RealType> ConcurrencyType;
typedef PsimagLite::Matrix<RealType> MatrixType;
typedef FreeFermions::Engine<RealType,FieldType,ConcurrencyType> EngineType;
typedef FreeFermions::CreationOrDestructionOp<EngineType> OperatorType;
typedef FreeFermions::HilbertState<OperatorType> HilbertStateType;
typedef FreeFermions::GeometryLibrary<MatrixType> GeometryLibraryType;
typedef OperatorType::FactoryType OpNormalFactoryType;


int main(int argc,char* argv[])
{
	int argce = 3;
	size_t whatGeometry = GeometryLibraryType::CHAIN;
	std::string s = "Needs " + ttos(argce) + " argument(s)\n";
	if (argc!=argce) throw std::runtime_error(s.c_str());
	size_t n = atoi(argv[1]); // n. of  sites
	size_t dof = 1; // spinless
	MatrixType t(n,n);
	//std::vector<MatrixType> t;
	GeometryLibraryType geometry(n,whatGeometry);
	//GeometryLibraryType geometry(n,GeometryLibraryType::CHAIN);
	//geometry.setGeometry(t,GeometryLibraryType::OPTION_PERIODIC);
	
	switch (whatGeometry) {
// 		case GeometryLibraryType::FEAS:
// 			geometry.setGeometry(t,"feasHoppings.inp",2); //,GeometryLibraryType::OPTION_PERIODIC);
// 			break;
		case GeometryLibraryType::CHAIN:
			geometry.setGeometry(t);
			break;
		case GeometryLibraryType::LADDER:
			geometry.setGeometry(t,2);
			break;
	}
	
	std::cerr<<t;
	ConcurrencyType concurrency(argc,argv);
	EngineType engine(t,concurrency,dof,true);
	std::vector<size_t> ne(dof,atoi(argv[2])); // n. of up (= n. of  down electrons)
	HilbertStateType gs(engine,ne);
	RealType sum = 0;
	for (size_t i=0;i<ne[0];i++) sum += engine.eigenvalue(i);
	std::cerr<<"Energy="<<dof*sum<<"\n";	
	size_t sigma = 0;
	//MatrixType cicj(n,n);
	size_t norb = (whatGeometry == GeometryLibraryType::FEAS) ? 2 : 1;
	for (size_t orbital=0; orbital<norb; orbital++) {
		for (size_t site = 0; site<n ; site++) {
			OpNormalFactoryType opNormalFactory(engine);
			OperatorType& myOp = opNormalFactory(OperatorType::DESTRUCTION,site+orbital*n,sigma);
			for (size_t site2=0; site2<n; site2++) {
				HilbertStateType phi = gs;
				myOp.applyTo(phi);
				OperatorType& myOp2 = opNormalFactory(OperatorType::CREATION,site2+orbital*n,sigma);
				myOp2.applyTo(phi);
				std::cout<<scalarProduct(gs,phi)<<" ";
				//cicj(site,site2) += scalarProduct(gs,phi);
			}
			std::cout<<"\n";
		}
		std::cout<<"-------------------------------------------\n";
	}
}

