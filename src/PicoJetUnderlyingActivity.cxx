//====================================================================================================
//  2016.09.19 David Stewart
//  taging the lines for a uswer input file
//====================================================================================================
//
//
//====================================================================================================
//
//	2015.09.22	Li Yi
//	modified from Kolja Kauder's Aj analysis to study underlying event activity dependence on jet
//	energy in pp 200 GeV
//
//====================================================================================================


/** 
  @author Kolja Kauder
  @version Revision 0.2
  @brief Aj analysis and embedding prep in p+p.
  @details Perform Aj analysis in a given TStarPicoJetTree chain. Can also save events with 10 GeV jets for embedding.
  @date Mar 04, 2015
  */

#include "UnderlyingAna.hh"
// #include "AjParameters.hh"

#include "inout.h"

#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TProfile.h>
#include <TFile.h>

#include <TLorentzVector.h>
#include <TClonesArray.h>
#include <TString.h>
#include <TChain.h>
#include <TBranch.h>
#include <TMath.h>
#include <TRandom.h>
#include <TSystem.h>

#include "TClonesArray.h"

#include <utility>	// std::pair, std::make_pair
#include <iostream>
#include <fstream>
#include <set>
#include <cmath>
#include <exception>
#include <cstdlib>      // std::rand, std::srand
#include <cstdio>       // rename
#include <algorithm>    // std::random_shuffle

#include "TStarJetPicoReader.h"
#include "TStarJetPicoEvent.h"
#include "TStarJetPicoEventHeader.h"
#include "TStarJetPicoEventCuts.h"

#include "TStarJetPicoPrimaryTrack.h"
#include "TStarJetPicoTrackCuts.h"
#include "TStarJetPicoTowerCuts.h"

#include "TStarJetPicoTriggerInfo.h"


using namespace std;
using namespace fastjet;

typedef std::pair<float, float> EtaPhiPair;

/** 
  - Set up input tree
  - Set up output histos and tree
  - Initialize UnderlyingAna object
  - Loop through events
  \arg argv: 
  - [1] : output file
  - [2] : trigger name
  - [3] : Input file pattern. Let TChain handle the globbing, i.e. use for example
  <BR><tt>% PicoJetUnderlyingActivity '~putschke/Data/ppHT/&lowast;.root'</tt>
  <BR>For cases like 
  <BR><tt>% PicoJetUnderlyingActivity ~putschke/Data/&lowast;/&lowast;.root</tt>
  <BR>change this macro
  - [4] : tower uncertainty switch ( -1/0/1 )
  - [5] : efficiency correction uncertainty switch ( -1/0/1 )
  <BR> Note that we use a vector for argv and shift the index down by one.
  */

float CorrectBemcVzEta(float geoEta, float PrimVertexZ, float radius = 224){	
    // bemc r_int  = 224 cm
    // eta in trigger info for bemc is just geometry eta. 
    // Need to shift according to the vertex position of each collisions
	float corrEta = 0;
	float T1;
        T1=2*atan(exp(-geoEta));
        Double_t zNew;
        if(geoEta!=0){zNew=radius/tan(T1);} //radius in cm 
        if(geoEta==0){zNew=0;}
        double zNom=zNew-PrimVertexZ;
        double THETA=atan2(radius,zNom);
        corrEta=-log(tan(THETA/2));

	return corrEta;

}

bool readinbadrunlist(std::set<int> & badrun, TextFileInputs & user)
{
               // TString csvfile="./include/pp200Y12_badrun.list", 
               // TextFileInputs &user) {
	
	// open infile
    TString csvfile = user.badrun_list.Data();
	std::string line;
	std::ifstream inFile (csvfile );
	
	std::cout<<"Loading bad run id from "<< csvfile.Data()<<std::endl;;
	        
	if ( !inFile.good() ) {
	  std::cout<<"Can't open "<<csvfile.Data()<<std::endl;
	  return false;
	}
	
	bool print_badrun_msg = true;
	while (std::getline (inFile, line) ){
	  if ( line.size()==0 ) continue; // skip empty lines
	  if ( line[0] == '#' ) continue; // skip comments
	
	  std::istringstream ss( line );
	  while( ss ){
	    std::string entry;
	    std::getline( ss, entry, ',' );
	    int ientry = atoi(entry.c_str());
	    if (ientry) {
	      badrun.insert( ientry );
		  if (user.noprint_badrunid) {
             if (print_badrun_msg) {
                std::cout<<"Adding badruns. Skipping badrun msgs." << endl;
                cout<<"  (To print badrun values, change 'noprint_badrunid' in user.i to '0'.)"<<endl;
                print_badrun_msg = false;
             }   
          } else {
              std::cout<<"Added bad runid "<<ientry<<std::endl;
          }
	    }
	  }
	}
	return true;
}



//-----------------------------
//  Reader for Event Cuts
//-----------------------------
TStarJetPicoReader SetupReader ( TChain* chain, TextFileInputs &user) {
    //TString TriggerName, double RefMultCut){ //TextFileInputs& user ){
    //TChain* chain, TString TriggerString, const double RefMultCut ){
	//TStarJetPicoDefinitions::SetDebugLevel(10); // 10 for more  output, 0 for less output

	TStarJetPicoReader reader;
	reader.SetInputChain (chain);

	// Event and track selection
	// -------------------------
	TStarJetPicoEventCuts* evCuts = reader.GetEventCuts();
    
    // Note, the trigger selection logic is in TStarJetPicoEventcuts.cxx
    // main selections here are ppMB, ppJP0, ppJP1, ppJP2
	evCuts->SetTriggerSelection( user.TriggerName ); //All, MB, HT, pp, ppHT, ppJP

	// Additional cuts 
	evCuts->SetVertexZCut (user.VzCut); /*19SEP var*/
	evCuts->SetRefMultCut ( user.RefMultCut ); /*19SEP var*/
	evCuts->SetVertexZDiffCut( user.VzDiffCut ); /*19SEP var*/

	evCuts->SetMaxEventPtCut ( user.MaxEventPtCut ); /*19SEP var*/
	evCuts->SetMaxEventEtCut ( user.MaxEventEtCut ); /*19SEP var*/

	evCuts->SetPVRankingCut ( 0 );		
    // Vertex ranking > 0. Use SetPVRankingCutOff() to turn off vertex ranking cut.  default is OFF

	std::cout << "Exclude event with track > " << evCuts->GetMaxEventPtCut() << std::endl;
	std::cout << "Exclude event with tower > " << evCuts->GetMaxEventEtCut() << std::endl;

	// Tracks cuts
	TStarJetPicoTrackCuts* trackCuts = reader.GetTrackCuts();
	trackCuts->SetDCACut( user.DcaCut ); /*19SEP var*/
	trackCuts->SetMinNFitPointsCut( user.NMinFit ); /*19SEP var*/
	trackCuts->SetFitOverMaxPointsCut( user.FitOverMaxPointsCut ); /*19SEP var*/
	trackCuts->SetMaxPtCut ( user.MaxTrackPt ); /*19SEP var*/

	std::cout << "Using these track cuts:" << std::endl;
	std::cout << " dca : " << trackCuts->GetDCACut(  ) << std::endl; /*19SEP var*/
	std::cout << " nfit : " <<   trackCuts->GetMinNFitPointsCut( ) << std::endl; /*19SEP var*/
	std::cout << " nfitratio : " <<   trackCuts->GetFitOverMaxPointsCut( ) << std::endl; /*19SEP var*/
	std::cout << " maxpt : " << trackCuts->GetMaxPtCut (  ) << std::endl; /*19SEP var*/

	// Towers
	TStarJetPicoTowerCuts* towerCuts = reader.GetTowerCuts();
	towerCuts->SetMaxEtCut(user.MaxEtCut); /*19SEP var*/
	towerCuts->AddBadTowers("./include/pp200Y12_badtower.list");		
    // #LY CHECK where is the bad tower list /*19SEP var*/

	// Tower energy correction (subtract associated charged particle deposit energy). 
    // By default, it is MIP correction (comment out the following 3 lines)
	reader.SetApplyFractionHadronicCorrection(kTRUE); /*19SEP var*/
	reader.SetFractionHadronicCorrection(0.9999);     /*19SEP var*/
	reader.SetRejectTowerElectrons( kFALSE );         /*19SEP var*/


	std::cout << "Using these tower cuts:" << std::endl;
	std::cout << "  GetMaxEtCut = " << towerCuts->GetMaxEtCut() << std::endl; /*19SEP var*/
	std::cout << "  Gety8PythiaCut = " << towerCuts->Gety8PythiaCut() << std::endl; /*19SEP var*/

	// V0s: Turn off
	reader.SetProcessV0s(false);

	return reader;

}


int main ( int argc, const char** argv ) {

	// use the input file, defaulted to user.i, but also can be specified by the command line.
    // can also specify the output directory name via the command line.

	/* const char *defaults[] = {"user.i",""}; */
	TString temp_ifile = "user.i";
	TString temp_odir  = "";

	cout << "argc " << argc << endl;
	if (argc > 1) temp_ifile = argv[1];
    if (argc > 2) temp_odir  = argv[2];

    TextFileInputs user  = read_user_input_textfile(temp_ifile.Data(), temp_odir);


    //------------------------------------------------------------------------------
    // If there are command line arguments, use them to overide the arguments in the 
    // the input file. This is to keep the code use compatible with previous uses.
    //
    // Note that the final condition of the user arguements are output to the 
    // ~/odir/user.final file.
	if (temp_odir != "") {
		// here, the command line passed an alternate output directory name.
          
        // move the outpute directory to the new output directory name
        /* int result = rename(user.odir, temp_odir); */
        /* if ( result != 0) { */
        /*     cout << "Failed to move output directory to " << temp_odir << endl << flush; */
        /*     exit(1); */
        /* } */

        // print the warning input messages
		TString temp = Form("%s/%s",temp_odir.Data(),"input.i_mod");
        ofstream mod(temp.Data());
        mod << "// Note: this program was called with the following command line arguments:" << endl;
        for (int i = 1; i < argc; i++){ mod << "//       " << argv[i]  << endl; }
        mod << "//      The keyword values input in " << temp_ifile  << " have been modified\n"
            << "//      by the values of the above parameters as follow:\n";

		user.odir = temp_odir;
		mod << "// odir -> " << user.odir << endl;

        //update the trigger name
        if (argc > 3) {
            user.TriggerName = argv[3];
            mod << "// TriggerName -> " << user.TriggerName << endl;
        }

        //update the input files
        if (argc > 4) {
            user.InputFiles = argv[4];
            mod << "// InputFiles -> " << user.InputFiles << endl;
        }

        //update the IntTowScale
        if (argc > 5){
            user.IntTowScale = atoi(argv[5]);
            mod <<"// IntTowScale -> " << user.IntTowScale << endl;
        }

		//update mEffUn
		if (argc>6) {
			user.mEffUn = atoi(argv[6]);
            mod <<"// mEffUn -> " << user.mEffUn << endl;
		}


		// update the options from the new odir name
        TString options = user.odir;

        // Get the jet resolution R from the odir name
        user.JetRes_R = 0.6;
        if      (options.Contains("R0.2")) { user.JetRes_R = 0.2; }
        else if (options.Contains("R0.4")) { user.JetRes_R = 0.4; }
        mod << "// JetRes_R -> " << user.JetRes_R << endl;

        // set track matching from the options
        user.TrackPileUpCut = 2;
        if      (options.Contains("NoTofMatch"))     { user.TrackPileUpCut = 0; }
        else if (options.Contains("BemcOrTofMatch")) { user.TrackPileUpCut = 1; }
        else if (options.Contains("BemcMatch"))      { user.TrackPileUpCut = 3; }
        mod << "// TrackPileUpCut -> " << user.TrackPileUpCut << endl;

        // set the jet algorithm
		user.JetAlgorithm = "antiket";
		if (options.Contains("kT") && (!options.Contains("AntikT"))){
			user.JetAlgorithm = "kt";
		}
        mod << "// JetAlgorithm -> " << user.JetAlgorithm << endl;

        // set underlying charge code
        user.UnderlyingChargeCode = -1;
        if      (options.Contains("TransCharged")) { user.UnderlyingChargeCode = 1; }
        else if (options.Contains("TransNeutral")) { user.UnderlyingChargeCode = 0; }
        mod << "// UnderlyingChargeCode -> " << user.UnderlyingChargeCode << endl;

        // set the TofMatchJetTrigger
        if (options.Contains("MatchTrig") && user.TriggerName.Contains("ppJP")){
            user.UlaToMatchTrig = true; 
        } else {
            user.UlaToMatchTrig = false;
        }
        mod << "// UlaToMatchTrig -> " << user.UlaToMatchTrig;

        //set the Jet Charge Code
        user.JetChargeCode = 2;
        if      (options.Contains("ChargeJet"))  { user.JetChargeCode = 1; }
        else if (options.Contains("NeutralJet")) { user.JetChargeCode = 0; }
        else if (options.Contains("FullJet"))    { user.JetChargeCode = 2; }
        mod << "// JetChargeCode -> " << user.JetChargeCode << endl;

		//set if the code use for auau
		user.isAuAu = false;
		if (options.Contains("AuAu") || options.Contains("auau")) { user.isAuAu = true; }
		mod << "// isAuAu -> " << user.isAuAu << endl;

        //set the dijetangle
        if (options.Contains("Dijet")) { user.DiJetAngle = 1; }
        else { user.DiJetAngle = 0; }
        mod << "// DiJetAngle -> " << user.DiJetAngle << endl;

        //set tranverse phi size
        if (options.Contains("TranPhi30")) { user.TransversePhiSize = 30.; }
        else                               { user.TransversePhiSize = 60.; }
    }
	
		
	//  Alternate the output directory name for the following logic:
	//  output file name can include(optional): 
	// 	"R0.6" (default) OR "R0.4" OR "R0.2";
	// 	"FullJet"(default) OR "ChargeJet" OR "NeutralJet";
	// 	"TransCharged" (default) particle only OR "TransNeutral" particle only;
	// 	"AntikT" (default) OR "kT"
	// 	"MatchTrig" together with "ppJP2" will match leading jet with the trigger jet phi, eta
	// 	"Monojet" (default) OR "Dijet"
	//	"TranPhi60" (default) OR "TranPhi30"
	

	/* // Set up some convenient default */
	/* // ------------------------------ */
	/* const char *defaults[] = {"user.i", "PicoJetUnderlyingActivity", */
        /* "./out.root", */
        /* //"/home/hep/caines/ly247/Scratch/pp200Y12_jetunderlying/FullJet_TransCharged_MatchTrig_ppJP2.root", */
        /* "ppJP2", */
        /* "/home/hep/caines/ly247/Scratch/pp12JP2Pico_151018.root", */
        /* "0", "0" }; */

    // {Code name, to be discard but needed since argv will use command name as
    // the [0], output file name, triggername, intput file list, for variable
    // IntTowScale to scale tower as systematics study, which effiencey file to
    // use }
	//


	// Throw arguments in a vector
	// ---------------------------
	/* vector<string> arguments(argv + 1, argv + argc); */

	// Load and set up tree
	// --------------------
	TString ChainName   = user.ChainName;
	TString OutFileName = user.OutFileName; //arguments.at(1);

	cout << " ################################################### " << endl;
	cout << " Triggering with R=" << user.JetRes_R << endl;
	cout << " ################################################### " << endl;
	cout << " TriggerName: "<< user.TriggerName << endl;

	/* int TrigFlagId = user.TrigFlagId; //0; */
    if (!user.TrigFlagId){
		if      (user.TriggerName=="ppJP2") user.TrigFlagId = 1236;
        else if (user.TriggerName=="ppJP1") user.TrigFlagId = 1228;
        else if (user.TriggerName=="ppJP0") user.TrigFlagId = 1220;
        else {

            cout << "Error: TrigFlagId was either not given, or could not be determined\n"
                 << "       from the TriggerName.\n" << endl;
        }
    }

	//open the output files
	cout << "Chain data: "<< user.InputFiles << " for "<< user.ChainName << endl;
	TChain* chain = new TChain( user.ChainName.Data() );
	chain->Add( user.InputFiles.Data() ); 

	// for systematics
	// ---------------
	if (!user.fTowScale) { user.fTowScale = 1.0 + 0.02*user.IntTowScale; }

	// user.mEffUn (note that this renames the output file
	if (!(user.mEffUn == 0 || user.mEffUn == 1 || user.mEffUn == -1)) {
		cout << "mEffUn = " << user.mEffUn << " not supported." << endl;
		return -1;
	}
    if (!user.isAuAu) {
		TString newbase = Form("Eff%i_",user.mEffUn);
        if (!user.OutFileName.Contains(newbase)){
            user.OutFileName.ReplaceAll ( gSystem->BaseName(user.OutFileName), 
                                      newbase + gSystem->BaseName(OutFileName));
        }
    }

	// user.IntTowScale: this renames the output file
	if (!(user.IntTowScale == 0 || user.IntTowScale == 1 || user.IntTowScale == -1)) {
		cout << "IntTowScale = " << user.IntTowScale << " not supported." << endl;
		return -1;
	}
    if (!user.isAuAu) {
		TString newbase = Form("Tow%i_",user.IntTowScale);
        if (!user.OutFileName.Contains(newbase)){
            user.OutFileName.ReplaceAll ( gSystem->BaseName(user.OutFileName), 
                                      newbase + gSystem->BaseName(OutFileName));
        }
    }
	if ( user.isAuAu && user.IntTowScale ){
		cerr << "IntTowScale = " << user.IntTowScale << " not supported in AuAu." <<endl;
		return -1;    
	}

	// setup the picojet reader
	cout << "SetupReader for pico" << endl; /*19SEP var*/

    // #ly note: Events & Tracks & Towers cuts are set here
	TStarJetPicoReader reader = SetupReader( chain, user );  //user.TriggerName, user.RefMultCut );
    reader.SetTrackPileUpCut(user.TrackPileUpCut);
	TStarJetPicoDefinitions::SetDebugLevel(user.TStarJet_DebugLevel);

	// Initialize analysis class
	// -------------------------
	//cout<<"initialize analysis"<<endl;		

	//TString OutFileName = "test.root"; 
	//float R = 0.6;	
	/* string jetalgorithm = "antikt"; */		
	/* if(OutFileName.Contains ("kT") && (!(OutFileName.Contains ("AntikT")))) { */
	/* 	jetalgorithm = "kt"; */
	/* } */

    TString ofil = Form("%s/%s",user.odir.Data(), user.OutFileName.Data());
	UnderlyingAna *ula = new UnderlyingAna( 
            user.JetRes_R,
			user.MaxTrackRap,
			user.CstMinPt,			// pt min for const.
            user.dPhiCut,
			ofil, //user.OutFileName,
			(string) user.JetAlgorithm
	);  

	cout << " ################################################### " << endl;
	cout << " jetchargecode = " << user.JetChargeCode <<endl; 
	cout << " underlyingchargecode = " << user.UnderlyingChargeCode <<endl; 
	cout << " ################################################### " << endl;

	// initial ttree & histograms in ula
	ula->Init();
    ula->SetToMatchJetTrigger(user.UlaToMatchTrig);

    // whether to apply neutral energy fraction in jet cut
    if (user.JetChargeCode == 2) ula->SetNeutralJetFracCut(true);
	else ula->SetNeutralJetFracCut(false);

	ula->SetJetCharge(user.JetChargeCode);			// Jet charge
	// underlying event charge: 0 for netural, 1 for charged, 2 for all
	ula->SetUnderlyingParticleCharge(user.UnderlyingChargeCode);
    ula->SetDiJetAngle(user.DiJetAngle);
    ula->SetTransversePhiSize(user.TransversePhiSize);

	// Cycle through events
	// --------------------
	vector<PseudoJet> particles;		// for jet finding
	TStarJetVectorContainer<TStarJetVector>* container;		// for underlying event loop
	TStarJetVector* sv; // TLorentzVector* would be sufficient. 
	PseudoJet pj;

	std::vector<EtaPhiPair> TrigLoc2Match;		// trigger of High Tower or Jet Patch

	//Long64_t nEvents=-1; // -1 for all
	//nEvents=10000;	// test
	cout<<"init..."<<endl;
	// DJS: make nEvents an int input
	//reader.Init(nEvents);
	reader.Init(user.nEvents);
    cout<<"initializing " << user.nEvents << " " << user.nEvents << " events. " << endl;
	int count = 0;


        // problematic runs, need future investigation		
        // <--------- Moved to include/pp200Y12_badrun.list
        //const int NoBadRun = 185;
        //int badrun[NoBadRun] = {13044118, 13044123, 13044124, 13044125,
        //13045001, 13045003, 13045005, 13045006, 13045007, 13045012, 13045029,
        //13046002, 13046008, 13046010, 13046029, 13046118, 13046119, 13046120,
        //13047004, 13047014, 13047018, 13047036, 13047037, 13047039, 13047040,
        //13047041, 13047042, 13047043, 13047044, 13047045, 13047046, 13047047,
        //13047048, 13047049, 13047050, 13047051, 13047052, 13047053, 13047054,
        //13047055, 13048007, 13048022, 13048046, 13049004, 13049005, 13049050,
        //13049052, 13049075, 13049086, 13049087, 13049088, 13049089, 13050007,
        //13050025, 13050026, 13050027, 13050033, 13050039, 13050043, 13050044,
        //13050046, 13050047, 13050049, 13050050, 13051068, 13051080, 13051088,
        //13051095, 13051102, 13052021, 13052022, 13052054, 13052063, 13052068,
        //13053010, 13053021, 13054004, 13054005, 13054006, 13054007, 13054008,
        //13054009, 13054011, 13054012, 13054013, 13054014, 13054015, 13054016,
        //13054017, 13054018, 13054019, 13054020, 13054022, 13054042, 13054045,
        //13054046, 13054057, 13055015, 13055072, 13055081, 13055082, 13055086,
        //13055087, 13055088, 13055089, 13055090, 13056011, 13056012, 13056034,
        //13056035, 13056037, 13056038, 13056039, 13057038, 13057039, 13058019,
        //13058030, 13058047, 13058048, 13059003, 13059004, 13059005, 13059006,
        //13059007, 13059008, 13059009, 13059010, 13059019, 13059035, 13059082,
        //13059083, 13059084, 13059085, 13059086, 13059087, 13060001, 13060002,
        //13060003, 13060009, 13060012, 13061026, 13063033, 13064030, 13064057,
        //13064059, 13064074, 13066035, 13066036, 13066101, 13066102, 13066104,
        //13066109, 13066110, 13067001, 13067002, 13067003, 13067004, 13067005,
        //13067006, 13067007, 13067008, 13067009, 13067010, 13067011, 13067012,
        //13067013, 13067014, 13067015, 13067017, 13068017, 13068022, 13068027,
        //13068029, 13068034, 13068036, 13068037, 13069006, 13069009, 13069029,
        //13070030, 13070056, 13071034, 13071037, 13071038, 13071040};
	
	std::set<int>badrun;
	badrun.clear();

	readinbadrunlist(badrun, user);        


	try{
		while ( reader.NextEvent() ) {
			reader.PrintStatus(10);
			if(count%10000==0) cout<<"event "<<count<<endl;
			count++;

            // DJS: Need an option to (1) cut out the run after hitting a maximum number of events
            //                        (2) to save the output periodically
            //           GetNOfCurrentEvents example in src/PicoUnderMcVsEmbed.cxx

			// event info
			// ----------
			//cout<<"load event header"<<endl;
			TStarJetPicoEventHeader* header = reader.GetEvent()->GetHeader();

			// eventid = header->GetEventId();
			int runid   = header->GetRunId();
			if(badrun.count(runid)>0) continue;			// in bad run list
			//if(runid>=13058000&& runid<13061000) continue;          
            //// a dip in TPC primary tracks. problematic runs, need future investigation	
            //<--------- Moved to include/pp200Y12_badrun.list
			//for(int i = 0; i<NoBadRun; i++) {
                        //	if(runid==badrun[i]) continue;
                	//}
			//if(header->GetZdcCoincidenceRate()>6000) continue;		// test
	
			// Load event ht/jetpatch trigger objs
			// ----------
			//std::cout<<"load trigger objs"<<endl;	
			TrigLoc2Match.clear();
			if(ula->GetToMatchJetTrigger()) {
				TClonesArray *trigobj = reader.GetEvent()->GetTrigObjs();
				cout << "B2 : trigobj->GetEntries() " << trigobj->GetEntries() << endl;
				for(int itrg = 0; itrg<trigobj->GetEntries(); itrg++) {
					if( ((TStarJetPicoTriggerInfo *)((*trigobj)[itrg]))->GetTriggerFlag()==user.TrigFlagId )	 
                    { 
						EtaPhiPair itrigloc =std::make_pair(CorrectBemcVzEta(((TStarJetPicoTriggerInfo *)
                                        ((*trigobj)[itrg]))->GetEta(),header->GetPrimaryVertexZ()),
                                        ((TStarJetPicoTriggerInfo *)((*trigobj)[itrg]))->GetPhi()) ;
						TrigLoc2Match.push_back(itrigloc);
			/*TEMP*/ cout << "itrigloc " /* itrigloc */ << endl;
			/*TEMP*/ cout << "itrigloc " << itrigloc.first << " " << itrigloc.second  << endl;
					}
				}
			}


			// Load event particles
			// ----------
			container = reader.GetOutputContainer();

			// Make particle vector
			// --------------------
			particles.clear();

			for (int ip = 0; ip<container->GetEntries() ; ++ip ){
				sv = container->Get(ip);  // Note that TStarJetVector contains more info, such as charge;

				if(fabs(sv->perp())<0.2) continue;		// #ly CHECK!!!!!!!! minimum pT or Et. 
                // --> NOT in use anymore, moved cuts to UnderlyingAna class min_const_pt 
                // for all particles (tracks and towers)

				if (sv->GetCharge()==0 ) (*sv) *= user.fTowScale; // for systematics
				pj=MakePseudoJet( sv );
				pj.set_user_info ( new JetAnalysisUserInfo( 3*sv->GetCharge(), 
                            sv->GetFeatureD(TStarJetVector::_DEDX),
                            sv->GetFeatureD(TStarJetVector::_TOFBETA) ) );
				pj.set_user_index(ip);		
                // #ly	link fastjet::PseudoJet to TStarJetVector class	
                // --> NEED TO FIX THIS, NOT SURE WHY USER_INFO IS NOT PASSED TO 
                //       JAResult.at(0).constituents() in UnderlyingAna.cxx
				//cout<<"input "<<sv->GetCharge() <<" -> "
                //<<pj.user_info<JetAnalysisUserInfo>().GetQuarkCharge()<<endl;	 

				particles.push_back ( pj );

				//}	      
			}    
			// Run analysis
			// ------------
			//cout<<"analyze and fill"<<endl; 	

			ula->AnalyzeAndFill( particles, 
					//*container, 
					reader.GetEvent()->GetHeader()->GetEventId(),
					reader.GetEvent()->GetHeader()->GetRunId(),
					reader.GetEvent()->GetHeader()->GetGReferenceMultiplicity(),
					reader.GetEvent()->GetHeader()->GetPrimaryVertexZ(),
					TrigLoc2Match
			);



		} // while NextEvent
	} catch ( exception& e) {
		cerr << "Caught " << e.what() << endl;
		return -1;
	}
	cout << "##################################################################" << endl;

	//Long64_t nEventsUsed=reader.GetNOfEvents();  

	// Close up shop
	// -------------
	ula->Finish();
	//delete ula;
	cout << "Bye." << endl;
    TString end_echo_file = Form("%s/user.i.final",user.odir.Data());
    user.PrintCurrentVals(end_echo_file, 0);
	return 0;
}

