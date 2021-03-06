

// SAmple of how to use FreeFermions core engine to calculate
// <c^\dagger_i c_j >
#include <cstdlib>
#include "Engine.h"
#include "GeometryLibrary.h"
#include "ConcurrencySerial.h"
#include "TypeToString.h"
#include "CreationOrDestructionOp.h"
#include "HilbertState.h"
#include "GeometryParameters.h"
#include "EtoTheIhTime.h"
#include "DiagonalOperator.h"

typedef double RealType;
typedef std::complex<double> ComplexType;
typedef ComplexType FieldType;
typedef PsimagLite::ConcurrencySerial<RealType> ConcurrencyType;
typedef PsimagLite::Matrix<RealType> MatrixType;
typedef FreeFermions::GeometryParameters<RealType> GeometryParamsType;
typedef FreeFermions::GeometryLibrary<MatrixType,GeometryParamsType> GeometryLibraryType;
typedef FreeFermions::Engine<RealType,FieldType,ConcurrencyType> EngineType;
typedef FreeFermions::CreationOrDestructionOp<EngineType> OperatorType;
typedef FreeFermions::EToTheIhTime<EngineType> EtoTheIhTimeType;
typedef FreeFermions::DiagonalOperator<EtoTheIhTimeType> DiagonalOperatorType;
typedef FreeFermions::HilbertState<OperatorType,DiagonalOperatorType> HilbertStateType;
typedef DiagonalOperatorType::FactoryType OpDiagonalFactoryType;
typedef OperatorType::FactoryType OpNormalFactoryType;


enum {SPIN_UP,SPIN_DOWN};

// <phi | H |phi>
FieldType phiHPhi(OpNormalFactoryType& opNormalFactory,const HilbertStateType& gs,const PsimagLite::Vector<SizeType>::Type& sites,DiagonalOperatorType& h,const PsimagLite::Vector<ComplexType>::Type& weights)
{
	FieldType sum = 0;
	for (SizeType i = 0;i<sites.size();i++) {
		 HilbertStateType backvector = gs;
		 OperatorType& cdaggerI = opNormalFactory(OperatorType::CREATION,sites[i],SPIN_UP);
		 cdaggerI.applyTo(backvector);
		 h.applyTo(backvector);
		 for (SizeType j = 0;j<sites.size();j++) {
			 HilbertStateType tmp = gs;
			 OperatorType& cdaggerJ = opNormalFactory(OperatorType::CREATION,sites[j],SPIN_UP);
			 cdaggerJ.applyTo(tmp);
			 sum += scalarProduct(backvector,tmp)*weights[i]*weights[j];
		 }
	}
	return sum;

}

// <phi| n_p | phi>
FieldType phiNpPhi(OpNormalFactoryType& opNormalFactory,const HilbertStateType& gs,SizeType siteP,const PsimagLite::Vector<SizeType>::Type& sites,SizeType sigma,DiagonalOperatorType& eihOp,const PsimagLite::Vector<ComplexType>::Type& weights)
{
	FieldType sum = 0;
	OperatorType& cdaggerP = opNormalFactory(OperatorType::CREATION,siteP,sigma);
	OperatorType& cP = opNormalFactory(OperatorType::DESTRUCTION,siteP,sigma);
	for (SizeType i = 0;i<sites.size();i++) {
		HilbertStateType backvector = gs;
		OperatorType& cdaggerI = opNormalFactory(OperatorType::CREATION,sites[i],SPIN_UP);
		cdaggerI.applyTo(backvector);
		//eihOp.applyTo(backvector);

		cP.applyTo(backvector);
		cdaggerP.applyTo(backvector);

		for (SizeType j = 0;j<sites.size();j++) {
				HilbertStateType tmp = gs; 
				OperatorType& cdaggerJ= opNormalFactory(OperatorType::CREATION,sites[j],SPIN_UP);
				cdaggerJ.applyTo(tmp);
				//eihOp.applyTo(tmp);

				sum += scalarProduct(backvector,tmp)*weights[i]*weights[j];
		}
	}
	return sum;
}

// <phi| | phi>
FieldType phiPhi(OpNormalFactoryType& opNormalFactory,const HilbertStateType& gs,const PsimagLite::Vector<SizeType>::Type& sites,SizeType sigma,DiagonalOperatorType& eihOp,const PsimagLite::Vector<ComplexType>::Type& weights)
{
	FieldType sum = 0;
	for (SizeType i = 0;i<sites.size();i++) {
		HilbertStateType backvector = gs;
		OperatorType& cdaggerI = opNormalFactory(OperatorType::CREATION,sites[i],SPIN_UP);
		cdaggerI.applyTo(backvector);
		//eihOp.applyTo(backvector);
		for (SizeType j = 0;j<sites.size();j++) {
				HilbertStateType tmp = gs; 
				OperatorType& cdaggerJ = opNormalFactory(OperatorType::CREATION,sites[j],SPIN_UP);
				cdaggerJ.applyTo(tmp);
				//eihOp.applyTo(tmp);
				sum += scalarProduct(backvector,tmp)*weights[i]*weights[j];
		}
	}
	return sum;
}

int main(int argc,char* argv[])
{
	int argce = 5;
	SizeType whatGeometry = GeometryLibraryType::CHAIN; // FEAS; //CHAIN; // KTWONIFFOUR;
	PsimagLite::String s = "Needs " + ttos(argce) + " argument(s)\n";
	if (argc<argce) throw std::runtime_error(s.c_str());
	SizeType n = atoi(argv[1]); // n. of  sites
	SizeType dof = 1; // spinless
	GeometryParamsType geometryParams;
	geometryParams.sites = n;
	geometryParams.type =whatGeometry;
	if (whatGeometry==GeometryLibraryType::LADDER || whatGeometry==GeometryLibraryType::FEAS)
		geometryParams.leg = 2;
	if (whatGeometry==GeometryLibraryType::FEAS || whatGeometry==GeometryLibraryType::KTWONIFFOUR)
		geometryParams.filename = argv[3];
	RealType timeMax =atof(argv[3]);
	RealType timeStep=atof(argv[4]);

	GeometryLibraryType geometry(geometryParams);
	//GeometryLibraryType geometry(n,GeometryLibraryType::CHAIN);
	//geometry.setGeometry(t,GeometryLibraryType::OPTION_PERIODIC);
	

//  	typename PsimagLite::Vector<RealType>::Type w;
// 	PsimagLite::IoSimple::In io(argv[3]);
// 	try {
// 		io.read(w,"PotentialT");
// 	} catch (std::exception& e) {
// 		std::cerr<<"No PotentialT in file "<<argv[3]<<"\n";
// 	}
// 	io.rewind();
// 	typename PsimagLite::Vector<RealType>::Type v;
// 	io.read(v,"potentialV");
// 	for (SizeType i=0;i<v.size();i++) v[i] += w[i];
// 
//  	geometry.addPotential(v);
	std::cerr<<geometry;
	ConcurrencyType concurrency(argc,argv);
	EngineType engine(geometry,concurrency,dof,true);
	PsimagLite::Vector<SizeType>::Type ne(dof,atoi(argv[2])); // n. of up (= n. of  down electrons)
	HilbertStateType gs(engine,ne);
	RealType sum = 0;
	for (SizeType i=0;i<ne[0];i++) sum += engine.eigenvalue(i);
	std::cerr<<"Energy="<<dof*sum<<"\n";	
	
	OpNormalFactoryType opNormalFactory(engine);
	
	//MatrixType cicj(n,n);
	//SizeType norb = (whatGeometry == GeometryLibraryType::FEAS) ? 2 : 1;
	PsimagLite::Vector<SizeType>::Type sites(n-2);
	PsimagLite::Vector<ComplexType>::Type weights(n-2);
	for (SizeType i=1;i<15;i++) {
		sites[i-1]=i;
		RealType tmp123 = (i-n/2)*(i-n/2)/8.;
		weights[i-1] = exp(-tmp123);
		std::cout<<"WEIGHT["<<i<<"]="<<weights[i-1]<<" "<<tmp123<<" <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n";
	}

//	std::cout<<"site\tvalue\tnumerator\tdenominator\n";
	std::cout<<"time ";
	for (SizeType site = 0; site<n ; site++) std::cout<<site<<" ";
	std::cout<<"\n";
	for (RealType time=0;time<timeMax;time+=timeStep)  {
		FieldType total = 0;
		OpDiagonalFactoryType opDiagonalFactory(engine);
		EtoTheIhTimeType eih(time,engine,0);
		DiagonalOperatorType& eihOp = opDiagonalFactory(eih);
		FieldType denominator = phiPhi(opNormalFactory,gs,sites,SPIN_UP,eihOp,weights);
		std::cout<<time<<" ";
		for (SizeType site = 0; site<n ; site++) {
			FieldType numerator = phiNpPhi(opNormalFactory,gs,site,sites,SPIN_UP,eihOp,weights);
			FieldType value = numerator/denominator;
			//std::cout<<site<<" "<<value<<" "<<numerator<<" "<<denominator<<"\n";
			RealType valueReal = PsimagLite::real(value)+0.5;
			assert(fabs(PsimagLite::imag(value))<1e-6);
			std::cout<<valueReal<<" ";
			total += value;
		}

		RealType totalReal = PsimagLite::real(total);
		std::cout<<totalReal<<" ";
		EtoTheIhTimeType hh(2000.0,engine,0);
		DiagonalOperatorType& h = opDiagonalFactory(hh);
		std::cout<<phiHPhi(opNormalFactory,gs,sites,h,weights)<<"\n";
	}
}

