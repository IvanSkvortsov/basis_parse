#include"SingleBasisSet.hpp"

int main(int argc, char** argv){

	SingleBasisSet<double> A;
	if(!A.import_gamessFormat(argv[1])) return 0;
	//A.printBasisSet();
	//A.exportBasisSetMolproFormat("3.basis");
	return 0;
}


