// SAmple of how to use FreeFermions core engine to calculate
// < n_i n_j >

#include <cstdlib>
#include <unistd.h>
#include "Engine.h"
#include "GeometryLibrary.h"
#include "TypeToString.h"
#include "CreationOrDestructionOp.h"
#include "HilbertState.h"
#include "GeometryParameters.h"
#include "Tokenizer.h"
#include "LibraryOperator.h"
#include "Concurrency.h"

typedef double RealType;
typedef std::complex<double> ComplexType;
typedef RealType FieldType;
typedef PsimagLite::Concurrency ConcurrencyType;
typedef PsimagLite::Matrix<RealType> MatrixType;
typedef FreeFermions::GeometryParameters<RealType> GeometryParamsType;
typedef FreeFermions::GeometryLibrary<MatrixType,GeometryParamsType> GeometryLibraryType;
typedef FreeFermions::Engine<RealType,FieldType> EngineType;
typedef FreeFermions::CreationOrDestructionOp<EngineType> OperatorType;
typedef FreeFermions::HilbertState<OperatorType> HilbertStateType;
typedef OperatorType::FactoryType OpNormalFactoryType;
typedef FreeFermions::LibraryOperator<OperatorType> LibraryOperatorType;
typedef LibraryOperatorType::FactoryType OpLibFactoryType;

void usage(const PsimagLite::String& thisFile)
{
	std::cout<<thisFile<<": USAGE IS "<<thisFile<<" ";
	std::cout<<" -n sites -e electronsUp -g geometry,[leg,filename]\n";
}

int main(int argc,char* argv[])
{
	int opt = 0;
	PsimagLite::String file("");

	while ((opt = getopt(argc, argv, "f:")) != -1) {
		switch (opt) {
		case 'f':
			file=optarg;
			break;
		default: /* '?' */
			usage("setMyGeometry");
			throw std::runtime_error("Wrong usage\n");
		}
	}

	if (file=="") {
		throw std::runtime_error("Wrong usage\n");
	}

	GeometryParamsType geometryParams(file);
	size_t electronsUp = GeometryParamsType::readElectrons(file,geometryParams.sites);

	size_t dof = 1; // spinless

	GeometryLibraryType geometry(geometryParams);

	std::cerr<<geometry;
	ConcurrencyType concurrency(argc,argv);
	EngineType engine(geometry,dof,true);
	PsimagLite::Vector<size_t>::Type ne(dof,electronsUp); // n. of up (= n. of  down electrons)
	HilbertStateType gs(engine,ne);
	RealType sum = 0;
	for (size_t i=0;i<ne[0];i++) sum += engine.eigenvalue(i);
	std::cerr<<"Energy="<<dof*sum<<"\n";	

	size_t sigma = 0;
	size_t tot = geometryParams.sites; //(debug) ? 1 : n;
	for (size_t site = 0; site<tot ; site++) {
		OpLibFactoryType opLibFactory(engine);
		LibraryOperatorType& myOp = opLibFactory(LibraryOperatorType::N,site,sigma);
		for (size_t site2=0; site2<tot; site2++) {
			HilbertStateType phi = gs;
			myOp.applyTo(phi);
			LibraryOperatorType& myOp2 = opLibFactory(LibraryOperatorType::N,site2,sigma);
			myOp2.applyTo(phi);
			std::cout<<scalarProduct(gs,phi)<<" ";
		}
		std::cout<<"\n";
	}
}

