

// Calculates <phi | phi>
// where 
// |phi> = c_{p up} exp(iHt) c_{i\sigma} nbar_{i \bar{sigma}} c^dagger_{j sigma'} n_{j \bar{sigma'}} |gs>

#include "IncludesTime.h"


FieldType calcSuperDensity(size_t site, HilbertVectorType gs,const EngineType& engine,const ObservableLibraryType& library)
{

	HilbertVectorType tmpV = engine.newState();
	
	for (size_t sigma = 0;sigma<2;sigma++) {
		HilbertVectorType phi = engine.newState();
		library.applyNiOneFlavor(phi,gs,site,1-sigma);
		
		FreeOperatorType myOp2 = engine.newSimpleOperator("creation",site,sigma);
		HilbertVectorType phi2 = engine.newState();
		myOp2.apply(phi2,phi);
		
		tmpV.add(phi2);
		phi.clear();

	}
	return scalarProduct(tmpV,tmpV);
}


int main(int argc,char *argv[])
{
	if (argc!=5) throw std::runtime_error("Needs 5 arguments\n");
	size_t n = 16; // 16 sites
	size_t dof = 2; // spin up and down
	bool isPeriodic = false; 
	psimag::Matrix<FieldType> t(n,n);
	for (size_t i=0;i<n;i++) {
		for (size_t j=0;j<n;j++) {
			if (i-j==1 || j-i==1) t(i,j) = 1.0;
		}
	}
	if (isPeriodic) t(0,n-1) = t(n-1,0) = 1.0;
	
	bool verbose = false;
	EngineType engine(t,dof,false);
	ObservableLibraryType library(engine);
	
	std::vector<size_t> ne(dof,8); // 8 up and 8 down
	HilbertVectorType gs = engine.newGroundState(ne);
	
	size_t site = atoi(argv[1]);
	//size_t site2 = atoi(argv[2]);
	size_t site3 = atoi(argv[2]);
	size_t sigma3 = 0;

	FieldType superdensity = calcSuperDensity(site,gs,engine,library);
	std::cout<<"superdensity="<<superdensity<<"\n";
	
	for (size_t it=0;it<size_t(atoi(argv[3]));it++) {
		RealType time = it * atof(argv[4]);
		EtoTheIhTimeType eih(time,engine);
		DiagonalOperatorType eihOp(eih);
				
		//HilbertVectorType savedVector = engine.newState(verbose);
		//FieldType savedValue = 0;
		FieldType sum = 0;
		for (size_t sigma = 0;sigma<2;sigma++) {
			HilbertVectorType phi = engine.newState();
			library.applyNiOneFlavor(phi,gs,site,1-sigma);
	
			FreeOperatorType myOp2 = engine.newSimpleOperator("creation",site,sigma);
			HilbertVectorType phi2 = engine.newState();
			myOp2.apply(phi2,phi);
			phi.clear();
			
			//for (size_t sigma2 = 0;sigma2 < 2;sigma2++) {
				//HilbertVectorType phi3 = engine.newState();
				//library.applyNiBarOneFlavor(phi3,phi2,site2,1-sigma2);
				
				//FreeOperatorType myOp4 = engine.newSimpleOperator("destruction",site2,sigma2);
				//HilbertVectorType phi4 = engine.newState(verbose);
				//myOp4.apply(phi4,phi3);
				//phi3.clear();
				
				if (verbose) std::cerr<<"Applying exp(iHt)\n";
				HilbertVectorType phi5 = engine.newState(verbose);
				eihOp.apply(phi5,phi2);
				//phi4.clear();
				
				if (verbose) std::cerr<<"Applying c_p\n";
				FreeOperatorType myOp6 = engine.newSimpleOperator("destruction",site3,sigma3);
				HilbertVectorType phi6 = engine.newState(verbose);
				myOp6.apply(phi6,phi5);
				phi5.clear();
				
				if (verbose) std::cerr<<"Adding "<<sigma<<" "<<it<<"\n";
				sum += scalarProduct(phi6,phi6);
				if (verbose) std::cerr<<"Done with scalar product\n";
				//if (sigma ==0 && sigma2 ==0) savedVector = phi6;
				//if (sigma ==1 && sigma2 ==1) {
				//	savedValue = scalarProduct(phi6,savedVector);
				//	savedVector.clear();
				//}
				//timeVector.add(phi6);
			//}
		}
		//sum += 2*real(savedValue);
		std::cout<<site3<<" "<<sum<<"\n";
	}
}