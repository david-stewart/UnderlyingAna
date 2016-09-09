//==============================================================================================================
//
//		2016.09.06	Li YI
//		use Unfold2D class to unfold undelrying vs leading jet pT
//
//
//==============================================================================================================

#include "Unfold2D.hh"

#include <TH1.h>
#include <TH2.h>
#include <TProfile.h>
#include <TFile.h>
#include <TString.h>
#include <TMath.h>
#include <TSystem.h>

#include <iostream>
#include <fstream>
#include <set>
#include <cmath>
#include <exception>
#include <cstdlib>      // std::rand, std::srand
#include <algorithm>    // std::random_shuffle

using namespace std;

int main( int argc, const char** argv ) {

	Unfold2D *uf2 = new Unfold2D("test");
	
	uf2->Init();

	uf2->TrainAndTest();

	return 0;
}
