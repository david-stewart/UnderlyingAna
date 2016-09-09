//=====================================================================-*-C++-*-
//
//	2016.08.23		Li YI
//	try to unfold underlying event activity vs leading jet pt
//
//==============================================================================
//
//
// File and Version Information:
//      $Id: RooUnfoldTestHarness2D.icc 336 2012-06-12 19:39:44Z T.J.Adye $
//
// Description:
//      Test Harness class for the RooUnfold package using 2D toy MC.
//      Inherits from RooUnfoldTestHarness.
//
// Author: Tim Adye <T.J.Adye@rl.ac.uk>
//
//==============================================================================

#ifndef __UNFOLD2D_CXX
#define __UNFOLD2D_CXX

#include "Unfold2D.hh"
#include "CrossSectionPerpT.h"

#if !defined(__CINT__) || defined(__MAKECINT__)
#include <cmath>
#include <iostream>

#include "TROOT.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TRandom.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TProfile.h"
#include "TVectorD.h"
#include "TLine.h"
#include "TStyle.h"

#include "RooUnfoldErrors.h"
#include "RooUnfoldParms.h"
#include "RooUnfoldResponse.h"
#include "RooUnfold.h"
#endif


#if !defined(__CINT__) || defined(__MAKECINT__)
using std::cerr;
using std::cout;
using std::endl;
using std::sin;
using std::cos;
#endif

//==============================================================================
// Test parameters
//==============================================================================

void Unfold2D::Help() {

	cout<<"For parameters set up:"<<endl;
	cout<<"method = args[0];			// use 1 if not sure"<<endl;
	cout<<"// 0	  no unfolding (output copied from measured input)"<<endl;
	cout<<"// 1	  Bayes"<<endl;
	cout<<"// 2	  SVD"<<endl;
	cout<<"// 3	  bin-by-bin"<<endl;
	cout<<"// 4	  TUnfold"<<endl;
	cout<<"// 5	  matrix inversion"<<endl<<endl;

	cout<<"ntx = args[1];				// x-axis bininng for Mc"<<endl;
	cout<<"nty = args[2];				// y-axis bininng for Mc"<<endl;
	cout<<"nmx = args[3];				// x-axis bininng for Rc"<<endl;
	cout<<"nmy = args[4];				// y-axis bininng for Rc"<<endl;
	cout<<"xlo = args[5];				// x-axis minimum"<<endl;
	cout<<"xhi = args[6];				// x-axis maximum"<<endl;
	cout<<"ylo = args[7];				// y-axis minimum"<<endl;
	cout<<"yhi = args[8];				// y-axis maximum"<<endl;
	cout<<"overflow = args[9];			// use histogram under/overflows if 1 (set from RooUnfoldResponse) "<<endl;
	cout<<"verbose = args[10];			// debug print out level"<<endl<<endl;
	cout<<"doerror = args[11];			// use 1 if not sure"<<endl;
	cout<<"// RooUnfold: "<<endl;
	cout<<"// enum ErrorTreatment {  // Error treatment:"<<endl;
	cout<<"//    kNoError,            //   no error treatment: returns sqrt(N)"<<endl;
	cout<<"//    kErrors,             //   bin-by-bin errors (diagonal covariance matrix)"<<endl;
	cout<<"//    kCovariance,         //   covariance matrix from unfolding"<<endl;
	cout<<"//    kCovToy,             //   covariance matrix from toy MC"<<endl;
	cout<<"//    kDefault=-1          //   not specified"<<endl<<endl;
	cout<<"repgram = args[12];			// regularisation parameter (If not sure use: Bayes niter=3, SVD kterm=ntx/2)"<<endl;
}

void Unfold2D::SetParms (const char* const* argv) 
{
	const int Npar = 13;
	float args[Npar] = {0};
	for(int i = 0; i<Npar; i++) 
	{
		args[i] = atof(argv[i]);
	}
	SetParms(args);

}


int Unfold2D::Float2Int(float aFloat) 
{
	int aInt = 0;
	if(aFloat>=0)	aInt = (int) (aFloat + 0.5);
	else		aInt = (int) (aFloat - 0.5);	
	return aInt;
}


void Unfold2D::SetParms (float* args)
{
	method = args[0];			// use 1 if not sure
	// 0	  no unfolding (output copied from measured input)
	// 1	  Bayes
	// 2	  SVD
	// 3	  bin-by-bin
	// 4	  TUnfold
	// 5	  matrix inversion

	ntx = Float2Int(args[1]);				// x-axis bininng for Mc
	nty = Float2Int(args[2]);				// y-axis bininng for Mc
	nmx = Float2Int(args[3]);				// x-axis bininng for Rc
	nmy = Float2Int(args[4]);				// y-axis bininng for Rc
	xlo = args[5];				// x-axis minimum
	xhi = args[6];				// x-axis maximum
	ylo = args[7];				// y-axis minimum
	yhi = args[8];				// y-axis maximum
	overflow = Float2Int(args[9]);			// use histogram under/overflows if 1 (set from RooUnfoldResponse) 
	verbose = Float2Int(args[10]);			// debug print out level

	doerror = Float2Int(args[11]);			// use 1 if not sure
	// RooUnfold: 
	// enum ErrorTreatment {  // Error treatment:
	//    kNoError,            //   no error treatment: returns sqrt(N)
	//    kErrors,             //   bin-by-bin errors (diagonal covariance matrix)
	//    kCovariance,         //   covariance matrix from unfolding
	//    kCovToy,             //   covariance matrix from toy MC
	//    kDefault=-1          //   not specified
	//  };

	repgram = args[12];			// regularisation parameter (If not sure use: Bayes niter=3, SVD kterm=ntx/2)
}

void Unfold2D::SetDefaultParms() 		// use default parameterization
{
	float args[13] = {1, 100, 20, 100, 20, 0, 100, 0, 20, 0, 1, 1, 3};
	SetParms(args);	
}

void Unfold2D::PrintParms() 
{
	cout<<"method = "<<method<<endl;
	cout<<"ntx = "<<ntx<<endl;
	cout<<"nty = "<<nty<<endl;
	cout<<"nmx = "<<nmx<<endl;
	cout<<"nmy = "<<nmy<<endl;
	cout<<"xlo = "<<xlo<<endl;
	cout<<"xhi = "<<xhi<<endl;
	cout<<"ylo = "<<ylo<<endl;
	cout<<"yhi = "<<yhi<<endl;
	cout<<"overflow = "<<overflow<<endl;
	cout<<"verbose = "<<verbose<<endl;
	cout<<"doerror = "<<doerror<<endl;
	cout<<"repgram = "<<repgram<<endl;
}

//==============================================================================
Int_t Unfold2D::ReadbyXsec4Train (TH2D *hTrainTrue, TH2D *hTrain, TH2D *hTrainFake, RooUnfoldResponse *response, int *Nevents)
{
	Bool_t flagMatch2Lead;
	Bool_t flagMatch2Sub;
	Int_t  McTranMaxNtrk;
	Int_t  McTranMinNtrk;
	Int_t  RcTranMaxNtrk;
	Int_t  RcTranMinNtrk;
	
	for(int i = 0; i<NUMBEROFPT; i++) {

		cout<<"Read in "<<Form("/home/fas/caines/ly247/Scratch/embedPythia/pt%s_underMcVsEmbedMatchTrig.root",PTBINS[i])<<endl;
		ftrain = new TFile(Form("/home/fas/caines/ly247/Scratch/embedPythia/pt%s_underMcVsEmbedMatchTrig.root",PTBINS[i]));
		tree = (TTree*)ftrain->Get("Tree");
		tree->SetBranchAddress("runid",&runid);
		tree->SetBranchAddress("eventid",&eventid);
		tree->SetBranchAddress("flagMatch2Lead",&flagMatch2Lead);
		tree->SetBranchAddress("flagMatch2Sub",&flagMatch2Sub);
		tree->SetBranchAddress("Mcj1pt",&McJet);
		tree->SetBranchAddress("McTranMaxNtrk",&McTranMaxNtrk);
		tree->SetBranchAddress("McTranMinNtrk",&McTranMinNtrk);
		//tree->SetBranchAddress("McPart",&McPart);
		tree->SetBranchAddress("Rcj1pt",&RcJet);
		tree->SetBranchAddress("RcTranMaxNtrk",&RcTranMaxNtrk);
		tree->SetBranchAddress("RcTranMinNtrk",&RcTranMinNtrk);
		//tree->SetBranchAddress("RcPart",&RcPart);
		if(McTranMaxNtrk>McTranMinNtrk) McPart = McTranMaxNtrk;
		else McPart = McTranMinNtrk;
		if(RcTranMaxNtrk>RcTranMinNtrk) RcPart = RcTranMaxNtrk;
		else RcPart = RcTranMinNtrk;
	
		if(Nevents[i]>tree->GetEntries()) {cout<<"Error!! Unfold2D::ReadbyXsec4Train () \n pT bin "<<PTBINS[i]<<" (Nevents["<<i<<"] = "<<Nevents[i]<<" >tree->GetEntries() = "<<tree->GetEntries()<<"\nPlease adjust it and rerun the code."<<endl; exit; }

		// Weight per pT bin
		weight = XSEC[i]/Nevents[i];
		
		for (Int_t i= 0; i<Nevents[i]; i++) {		// loop over entries
			tree->GetEntry(i);

			if(flagMatch2Lead||flagMatch2Sub) flag = 1; 
			else if(McJet>1e-6 && RcJet<1e-6) flag = 0;		// non-zero Mc, zero Rc
			else if(RcJet>1e-6 && McJet<1e-6) flag = -1;		// non-zero Rc, zero Mc

			if(flag==1) {			// one-to-one Mc to Rc matched
				hTrainTrue->Fill(McJet, McPart, weight);	
				hTrain->Fill(RcJet, RcPart, weight);	
				response->Fill(RcJet, RcPart,McJet, McPart, weight);
			}
			else if(flag==0) {		// true Mc, no Rc
				hTrainTrue->Fill(McJet, McPart, weight);
				response->Miss(McJet,McPart, weight);
			}
			else if(flag==-1){				// Fake Rc, no Mc
				hTrain->Fill(RcJet,RcPart, weight);
				hTrainFake->Fill(RcJet,RcPart, weight);
				response->Fake(RcJet,RcPart, weight);
			}
		}
		ftrain->Close();
	}

	return 1;
}


//==============================================================================
Int_t Unfold2D::Read4Train ()
{
	//ftrain = new TFile("EmbedTree.root");		// MC embedding files with response matrix and so on
	ftrain = new TFile(inputname);		// MC embedding files with response matrix and so on
	if(!ftrain->IsOpen()) {
		std::cout<<"cannot open "<<inputname<<std::endl;
		return 0;
	}

	tree = (TTree*)ftrain->Get("Tree");
	tree->SetBranchAddress("runid",&runid);
	tree->SetBranchAddress("eventid",&eventid);
	tree->SetBranchAddress("flag",&flag);
	tree->SetBranchAddress("McJet",&McJet);
	tree->SetBranchAddress("McPart",&McPart);
	tree->SetBranchAddress("RcJet",&RcJet);
	tree->SetBranchAddress("RcPart",&RcPart);
	//tree->SetBranchAddress("weight",&weight);

	for (Int_t i= 0; i<tree->GetEntries()/2.; i++) {		// loop over entries
		tree->GetEntry(i);
		if(flag==1) {			// one-to-one Mc to Rc matched
			hTrainTrue->Fill(McJet, McPart, weight);	
			hTrain->Fill(RcJet, RcPart, weight);	
			response->Fill(RcJet, RcPart,McJet, McPart, weight);
		}
		else if(flag==0) {		// true Mc, no Rc
			hTrainTrue->Fill(McJet, McPart, weight);
			response->Miss(McJet,McPart, weight);
		}
		else {				// Fake Rc, no Mc
			hTrain->Fill(RcJet,RcPart, weight);
			hTrainFake->Fill(RcJet,RcPart, weight);
			response->Fake(RcJet,RcPart, weight);
		}
	}
	
	return 1;
}

//==============================================================================
// Train: create response matrix
//==============================================================================

Int_t Unfold2D::Train ()
{

	hTrainTrue= new TH2D ("traintrue", "Training Truth", ntx, xlo, xhi, nty, ylo, yhi);
	hTrainTrue->SetLineColor(kBlue);
	hTrain= new TH2D ("train", "Training Measured", nmx, xlo, xhi, nmy, ylo, yhi);
	hTrain->SetLineColor(kRed);
	hTrainFake= new TH2D ("trainfake", "Training Fakes", nmx, xlo, xhi, nmy, ylo, yhi);
	hTrainFake->SetLineColor(93);

	response = new RooUnfoldResponse ("response", "");

	response->Setup (hTrain, hTrainTrue);

	int Nevents[NUMBEROFPT] = {1000000,300000,300000,150000,150000,150000,80000,50000,40000,38000,12000};
	Read4Train();

	hTrainFakeX= ProjectionX (hTrainFake, "hTrainFakeX", "Training Fakes X");
	hTrainFakeY= ProjectionY (hTrainFake, "hTrainFakeY", "Training Fakes Y");

	hTrainTrueX= ProjectionX (hTrainTrue, "hTrainTrueX", "Training X");
	hTrainTrueY= ProjectionY (hTrainTrue, "hTrainTrueY", "Training Y");
	hTrainX=     ProjectionX (hTrain,     "hTrainX",     "Training Measured X");
	hTrainY=     ProjectionY (hTrain,     "hTrainY",     "Training Measured Y");

	return 1;
}

void Unfold2D::TrainResults()
{
	setmax (hTrainTrueX, hTrainX, hTrainFakeX);
	setmax (hTrainTrueY, hTrainY, hTrainFakeY);

	TLegend *lTrain;
	TCanvas *ctrainx = new TCanvas();
	hTrainTrueX->Draw();
	hTrainX    ->Draw("SAME");
	if (hTrainFakeX) hTrainFakeX->Draw("SAME");
	Legend (lTrain, hTrainTrueX, hTrainFakeX, hTrainX);
	ctrainx->Update();

	TCanvas *ctrainy = new TCanvas();
	hTrainTrueY->Draw();
	hTrainY    ->Draw("SAME");
	if (hTrainFakeY) hTrainFakeY->Draw("SAME");
	lTrain->Draw();
	ctrainy->Update();
}

//==============================================================================
// Test unfolding algorithm
//==============================================================================

Int_t Unfold2D::TrainAndTest ()
{

	cout<<"Histograms"<<endl;
	hTrainTrue= new TH2D ("traintrue", "Training Truth", ntx, xlo, xhi, nty, ylo, yhi);
	hTrainTrue->SetLineColor(kBlue);
	hTrain= new TH2D ("train", "Training Measured", nmx, xlo, xhi, nmy, ylo, yhi);
	hTrain->SetLineColor(kRed);
	hTrainFake= new TH2D ("trainfake", "Training Fakes", nmx, xlo, xhi, nmy, ylo, yhi);
	hTrainFake->SetLineColor(93);

	cout<<"Set response"<<endl;
	response = new RooUnfoldResponse ("response", "");
	response->Setup (hTrain, hTrainTrue);
	cout<<"?"<<endl;

	int Nevents[NUMBEROFPT] = {1000000,300000,300000,150000,150000,150000,80000,50000,40000,38000,12000};
	for(int i=0; i<NUMBEROFPT; i++) {
		cout<<Nevents[i]<<endl;
	}
	ReadbyXsec4Train(hTrainTrue, hTrain, hTrainFake, response, Nevents);

	hTrainFakeX= ProjectionX (hTrainFake, "hTrainFakeX", "Training Fakes X");
	hTrainFakeY= ProjectionY (hTrainFake, "hTrainFakeY", "Training Fakes Y");

	hTrainTrueX= ProjectionX (hTrainTrue, "hTrainTrueX", "Training X");
	hTrainTrueY= ProjectionY (hTrainTrue, "hTrainTrueY", "Training Y");
	hTrainX=     ProjectionX (hTrain,     "hTrainX",     "Training Measured X");
	hTrainY=     ProjectionY (hTrain,     "hTrainY",     "Training Measured Y");

	TrainResults();

	Test(Nevents);

	return 1;
}

Int_t Unfold2D::Test (int *Nevents)
{
	//TFile *fin = new TFile("UnfoldMatrix_part1.root");			// measured result
	cout<<"I am using parts of simulations for train, rest for test"<<endl;
	cout<<"If this is not what you want, change loop in Unfold2D::Test() and Unfold2D::Train(). "<<endl;
	
	int tflag;
	double tMcJet, tMcPart, tRcJet, tRcPart, tweight;

	Bool_t tflagMatch2Lead;
	Bool_t tflagMatch2Sub;
	Int_t  tMcTranMaxNtrk;
	Int_t  tMcTranMinNtrk;
	Int_t  tRcTranMaxNtrk;
	Int_t  tRcTranMinNtrk;
	
	for(int i = 0; i<NUMBEROFPT; i++) {
		cout<<"Read in "<<Form("/home/fas/caines/ly247/Scratch/embedPythia/pt%s_underMcVsEmbedMatchTrig.root",PTBINS[i])<<endl;
		TFile *fin = new TFile(Form("/home/fas/caines/ly247/Scratch/embedPythia/pt%s_underMcVsEmbedMatchTrig.root",PTBINS[i]));
		TTree *ttree = (TTree*)fin->Get("Tree");

		ttree->SetBranchAddress("Mcj1pt",&tMcJet);
		ttree->SetBranchAddress("McTranMaxNtrk",&tMcTranMaxNtrk);
		ttree->SetBranchAddress("McTranMinNtrk",&tMcTranMinNtrk);
		ttree->SetBranchAddress("Rcj1pt",&tRcJet);
		ttree->SetBranchAddress("RcTranMaxNtrk",&tRcTranMaxNtrk);
		ttree->SetBranchAddress("RcTranMinNtrk",&tRcTranMinNtrk);
		if(tMcTranMaxNtrk>tMcTranMinNtrk) tMcPart = tMcTranMaxNtrk;
		else tMcPart = tMcTranMinNtrk;
		if(tRcTranMaxNtrk>tRcTranMinNtrk) tRcPart = tRcTranMaxNtrk;
		else tRcPart = tRcTranMinNtrk;


		hTrue= new TH2D ("true", "Test Truth", ntx, xlo, xhi, nty, ylo, yhi);
		hTrue->SetLineColor(kBlue);
		hMeas= new TH2D ("meas", "Test Measured", nmx, xlo, xhi, nmy, ylo, yhi);
		hMeas->SetLineColor(kRed);

		if(Nevents[i]>ttree->GetEntries()) {cout<<"Error!! Unfold2D::ReadbyXsec4Train () \n pT bin "<<PTBINS[i]<<" (Nevents["<<i<<"] = "<<Nevents[i]<<" >tree->GetEntries() = "<<tree->GetEntries()<<"\nPlease adjust it and rerun the code."<<endl; exit; }

		// Weight per pT bin
		tweight = XSEC[i]/(ttree->GetEntries()-Nevents[i]);

		for (Int_t j=Nevents[i];j<ttree->GetEntries() ; j++) {			// Caution!!! Need to update loop if needed
			ttree->GetEntry(j);

			if(tflagMatch2Lead||tflagMatch2Sub) tflag = 1; 
			else if(tMcJet>1e-6 && tRcJet<1e-6) tflag = 0;		// non-zero Mc, zero Rc
			else if(tRcJet>1e-6 && tMcJet<1e-6) tflag = -1;		// non-zero Rc, zero Mc

			if(tflag==1) {			// one-to-one tMc to tRc matched
				hTrue->Fill(tMcJet, tMcPart, tweight);	
				hMeas->Fill(tRcJet, tRcPart, tweight);	
			}
			else if(tflag==0) {		// true tMc, no tRc
				hTrue->Fill(tMcJet, tMcPart, tweight);
			}
			else if(tflag==-1){				// Fake tRc, no tMc
				hMeas->Fill(tRcJet,tRcPart, tweight);
				hFake->Fill(tRcJet,tRcPart, tweight);
			}
		}
		fin->Close();
	}
	hFakeX= ProjectionX (hFake, "hFakeX", "Test Fakes X");
	hFakeY= ProjectionY (hFake, "hFakeY", "Test Fakes Y");

	hTrueX= ProjectionX (hTrue, "hTrueX", "Test X");
	hTrueY= ProjectionY (hTrue, "hTrueY", "Test Y");

	hMeasX= ProjectionX (hMeas, "hMeasX", "Test Measured X");
	hMeasY= ProjectionY (hMeas, "hMeasY", "Test Measured Y");

	return 1;
}

//==============================================================================
// Show results
//==============================================================================

void Unfold2D::Results()
{

	if (hReco) {
		hRecoX= ProjectionX (hReco, "hRecoX", "Reconstructed X", "E");
		hRecoY= ProjectionY (hReco, "hRecoY", "Reconstructed Y", "E");

		hRecoX->SetMarkerStyle(kFullDotLarge);
		hRecoY->SetMarkerStyle(kFullDotLarge);
		setmax (hMeasX, hRecoX);
		setmax (hMeasY, hRecoY);

		if(hFakeX&&hTrueX) setmax(hMeasX,hFake,hTrueX,hRecoX);
		if(hFakeY&&hTrueY) setmax(hMeasY,hFake,hTrueY,hRecoY);
	}

	TLegend *lTest;
	TCanvas *ctestx = new TCanvas();
	hMeasX   ->Draw();
	if (hTrueX) hTrueX->Draw("SAME");
	if (hFakeX) hFakeX->Draw("SAME");
	if (hRecoX) hRecoX->Draw("SAME");
	Legend (lTest, hTrueX, hFakeX, hMeasX, hRecoX);
	ctestx->Update();

	TCanvas *ctesty = new TCanvas();
	hMeasY   ->Draw();
	if (hTrueY) hTrueY->Draw("SAME");
	if (hFakeY) hFakeY->Draw("SAME");
	if (hRecoY) hRecoY->Draw("SAME");
	lTest->Draw();
	ctesty->Update();

	if(hTrue) {
		TCanvas *ctesttrue = new TCanvas();
		hTrue->Draw("colz");
	}

	TCanvas *ctestmeas = new TCanvas();
	hMeas->Draw("colz");

	if (!hReco) return;

	TCanvas *ctestreco = new TCanvas();
	hReco->Draw();

	gStyle->SetPalette(1,0);
	TCanvas *ccorr = new TCanvas();
	TH2D* hCorr= CorrelationHist (unfold->Ereco((RooUnfold::ErrorTreatment)doerror),
			"corr", "Unfolded correlation matrix",
			response->Hresponse()->GetYaxis()->GetXmin(),
			response->Hresponse()->GetYaxis()->GetXmax());


	hCorr->Draw("COLZ");
}

//==============================================================================
// Write: record response matrix from training for future unfolding
//==============================================================================

Int_t Unfold2D::WriteTrain() 
{
	ftout = new TFile(Form("ResponseMatrix%d",ftrain->GetName()),"RECREATE");
	hTrainTrue->Write();
	hTrain->Write();
	hTrainFake->Write();
	response->Write();


	return 1;
}

//==============================================================================
// Constructors and destructor
//==============================================================================

Unfold2D::Unfold2D (TString name): inputname(name)
{
	Reset();
	SetDefaultParms();
}

Unfold2D::Unfold2D (const char* name, int argc, const char* const* argv)
: inputname(name)
{
	Reset();
	if(argc<10){ Help(); return; }
	SetParms(argv);
}


Unfold2D::~Unfold2D()
{
	delete response; response= 0;
	delete unfold;   unfold=   0;
	if(ftrain->IsOpen()) ftrain->Close();
	if(ftout->IsOpen()) ftout->Close();
}


//==============================================================================
// Utility routines
//==============================================================================

void Unfold2D::Reset()
{
	response= 0;
	unfold= 0;
	hTrain= hTrainTrue= hTrainFake= hTrue= hMeas= hFake= hReco= 0;
	hCorr= 0;

	hTrainX= hTrainTrueX= hTrueX= hTrainFakeX= hFakeX= hMeasX= hRecoX= 
		hTrainY= hTrainTrueY= hTrueY= hTrainFakeY= hFakeY= hMeasY= hRecoY= 0;
}

void Unfold2D::Init()
{
//*nothing for now
}

Int_t Unfold2D::CheckParms()
{
	int error = 0;
	if (verbose>=0) PrintParms ();
	if (ntx<=0)     {cerr << "Error: ntx ("    << ntx    << ") <= 0"                  << endl; error = 2;}
	if (nmx<=0)     {cerr << "Error: nmx ("    << nmx    << ") <= 0"                  << endl; error = 2;}
	if (xlo >= xhi) {cerr << "Error: xlo ("    << xlo    << ") >= xhi(" << xhi << ")" << endl; error = 2;}
	if (nty<=0)     {cerr << "Error: nty ("    << nty    << ") <= 0"                  << endl; error = 2;}
	if (nmy<=0)     {cerr << "Error: nmy ("    << nmy    << ") <= 0"                  << endl; error = 2;}
	if (ylo >= yhi) {cerr << "Error: ylo ("    << ylo    << ") >= yhi(" << yhi << ")" << endl; error = 2;}
	return error;
}

TH1D* Unfold2D::ProjectionX (const TH1* h, const char* name, const char* title, Option_t* opt)
{
	const TH2* h2= dynamic_cast<const TH2*>(h);
	TH1D* h1= h2->ProjectionX (name, 1, h->GetNbinsY(), opt);
	if (title) h1->SetTitle (title);
	return h1;
}

TH1D* Unfold2D::ProjectionY (const TH1* h, const char* name, const char* title, Option_t* opt)
{
	const TH2* h2= dynamic_cast<const TH2*>(h);
	TH1D* h1= h2->ProjectionY (name, 1, h->GetNbinsX(), opt);
	if (title) h1->SetTitle (title);
	return h1;
}




//==============================================================================
// Set histogram Y-axis display range
//==============================================================================

void Unfold2D::setmax (TH1* h,
		const TH1* h1, const TH1* h2, const TH1* h3,
		const TH1* h4, const TH1* h5, const TH1* h6)
{
	// Get the maximum y value of up to 7 histograms
	// Add 10% to match behaviour of ROOT's automatic scaling
	Double_t maxval= h1 ? h1->GetMaximum() : -DBL_MAX;
	if (h2 && h2->GetMaximum() > maxval) maxval= h2->GetMaximum();
	if (h3 && h3->GetMaximum() > maxval) maxval= h3->GetMaximum();
	if (h4 && h4->GetMaximum() > maxval) maxval= h4->GetMaximum();
	if (h5 && h5->GetMaximum() > maxval) maxval= h5->GetMaximum();
	if (h6 && h6->GetMaximum() > maxval) maxval= h6->GetMaximum();
	h->SetMinimum (0.0);
	if (maxval > h->GetMaximum()) h->SetMaximum (1.1*maxval);
}



void Unfold2D::Legend (TLegend*& legend, TH1* truth, TH1* fake, TH1* meas, TH1* reco, TF1* ff, TF1* tf)
{
	legend= new TLegend (0.79, (tf ? 0.62 : reco ? 0.72 : 0.75), 0.894, 0.89);
	legend->SetTextSize(0.035);
	legend->SetTextFont(42);
	legend->AddEntry (truth, "truth",         "L");
	if (fake)
		legend->AddEntry (fake,  "fakes",         "L");
	legend->AddEntry (meas,  "measured",      "L");
	if (reco)
		legend->AddEntry (reco,  "reconstructed", "P");
	if (ff)
		legend->AddEntry (ff->GetHistogram(), "measured fit", "L");
	if (tf)
		legend->AddEntry (tf,    "truth fit",     "L");
	legend->Draw();
}


TH2D* Unfold2D::CorrelationHist (const TMatrixD& cov,
		const char* name, const char* title,
		Double_t lo, Double_t hi)
{
	Int_t nb= cov.GetNrows();
	TH2D* h= new TH2D (name, title, nb, lo, hi, nb, lo, hi);
	h->SetAxisRange (-1.0, 1.0, "Z");
	for(int i=0; i < nb; i++)
		for(int j=0; j < nb; j++) {
			Double_t Viijj= cov(i,i)*cov(j,j);
			if (Viijj>0.0) h->SetBinContent (i+1, j+1, cov(i,j)/sqrt(Viijj));
		}
	return h;
}


#endif
