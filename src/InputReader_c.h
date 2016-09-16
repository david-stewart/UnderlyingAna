
#ifndef INPUTREADER_C_H
#define INPUTREADER_C_H
#include "InputReader.h"
 
InputReader::InputReader(const std::string fin_name) : fin(NULL), fecho(NULL) {
        open_file(fin_name);
        sprintf(tmpstr,"%s.echo",fin_name.c_str());
        fecho = fopen(tmpstr,"w");
        if (fecho == NULL){ fatal_error("Can't open \"input_echo\" file.\n"); }
    }
void InputReader::close_files(){
    if (fin != NULL)   { fclose(fin); fin = NULL; }
    if (fecho != NULL) { fclose(fecho); fecho = NULL; } 
}
InputReader::~InputReader(){
    close_files();
}

void InputReader::open_file(const std::string &name_in) {
    if (name_in != file_name){ 
        if (fin != NULL){ fclose(fin); }
        file_name = name_in;
        fin = fopen(file_name.c_str(),"r");
        if (fin == NULL){
            sprintf(tmpstr,"Can't open input file: %s\n",file_name.c_str());
            fatal_error(tmpstr);
        }
        fprintf(fin, "Echo of the input variables from file \"%s\"\n",
                file_name.c_str());
    }
}

//function to terminate with error msg */
void InputReader::fatal_error(const char* err_msg){
    fprintf(stderr,err_msg);
    exit(1);
}

int InputReader::is_empty(const char *s){
    while (*s != '\0') {
        if (!isspace(*s)){
            return 0;
        }
        s++;
    }
    return 1;
}
void InputReader::check_read_error(const char* varstr, const int &count, int number_read = 1) {
    if (count != number_read){
        sprintf(tmpstr,
        "\n--------------------------------------------------"
        "\n! Terminal Error: could not read input parameter: "
        "\n  parameter: %s"
        "\n  from file: %s"
        "\n--------------------------------------------------\n\n"
        , varstr, file_name.c_str());
        fatal_error(tmpstr);
    } 
}

template <typename T> 
void InputReader::read(T &rval, const char* var_name, const char* read_fmt)
{
	read_to_var_name(var_name);
    sprintf(fmtstr," %s %%%s",var_name,read_fmt);
    int count = sscanf(tmpstr,fmtstr,&rval);
    check_read_error(var_name,count);
    sprintf(fmtstr,"%-25s %+%%s\n",var_name,read_fmt);
    fprintf(fecho,fmtstr,rval);
}

  /*! \brief 
   * Skip all white space and commented lines.
   * 
   * Stops at first non-white space/non-commented line 
   * and errors if that line doesn't start with var_name
   */
void InputReader::read_to_var_name(const char* var_name){
    if ( fgets(tmpstr, MAX_SIZE, fin) == NULL ){
        check_read_error(var_name, 0);
    }
    // skip inputs that are all whitespace

    /* std::cout << "  --  --  -4: " << tmpstr << std::endl; */
    sscanf(tmpstr, " %s", tmpstr2);
    /* std::cout << "  --  --  -3: " << tmpstr << std::endl; */
    while ( (tmpstr2[0] == '/' && tmpstr2[1] == '/') || is_empty(tmpstr))
    {
        if ( fgets(tmpstr, MAX_SIZE, fin) == NULL ){
         check_read_error(var_name, 0);
        }
        sscanf(tmpstr, " %s", tmpstr2);
    }
}
	

const double InputReader::read_double (const char* var_name){
    double rval;
    read(rval, var_name,"lf");
    return rval;
}

const float InputReader::read_float (const char* var_name){
    float rval;
    read(rval, var_name, "f");
    return rval;
}

const int InputReader::read_int (const char* var_name){
    int rval;
    read(rval, var_name,"i");
    return rval;
}

const bool InputReader::read_bool (const char* var_name){
    int rval;
    read(rval, var_name,"i");
    bool retval = rval;
    return retval;
}

const unsigned int InputReader::read_uint (const char* var_name){
    unsigned int rval;
    read(rval, var_name,"ui");
    return rval;
}

const long long InputReader::read_llint (const char* var_name){
    long long int rval;
    read(rval, var_name,"lli");
    return rval;
}

const std::string InputReader::read_string (const char* var_name){
    char new_temp[MAX_SIZE];
    read(new_temp, var_name,"s");

    std::string rval = new_temp;
    return rval;
}
std::pair<float, float> InputReader::read_pair_of_floats(const char* var_name){
		read_to_var_name(var_name);
        sprintf(fmtstr," %s %%f %%f",var_name);
		float first, second;
        int count = sscanf(tmpstr,fmtstr,&first, &second);
        check_read_error(var_name,count,2);
        fprintf(fecho,"%-20s %8.4f %8.4f\n",var_name, first, second);
		return std::make_pair(first, second);
}


TextFileInputs::TextFileInputs(InputReader &din){

    //------------------------------------
    //  First inputs (frequently changed)
    //------------------------------------
    outdir               = din.read_string("outdir");
    nEvents              = din.read_llint("nEvents");
    save_every_nEvents   = din.read_int("save_every_nEvents");
    output_dirname       = din.read_string("output_dirname");
    add_detail_dirname   = din.read_bool("add_detail_dirname");
    sparse_degphi_tranlo = din.read_float("sparse_degphi_tranlo");
    sparse_degphi_tranhi = din.read_float("sparse_degphi_tranhi");
    tran_max_by_n        = din.read_bool("tran_max_by_n");
    no_pdfs              = din.read_bool("no_pdfs");

    announce_every_nevents  = din.read_int("announce_every_nevents ");
    output_rootfile         = din.read_string("output_rootfile        ");
    input_rootfile          = din.read_string("input_rootfile         ");
    triggername             = din.read_string("triggername            ");
    badtower_list_loc    = din.read_string("badtower_list_loc");
    DebugLevel              = din.read_int("DebugLevel             ");
    file_runmsg             = din.read_string("file_runmsg            ");

    //------------------------------------
    //  Main modeling parameters (w/ selector cuts)
    //------------------------------------
    R                    = din.read_float("R");
    efficiency_filename  = din.read_string("efficiency_filename");
    IntTowScale          = din.read_string("IntTowScale        ");
    jet_algo             = din.read_string("jet_algo");
    ghost_repeat         = din.read_int("ghost_repeat");
    ghost_area           = din.read_double("ghost_area");
    VzCut                = din.read_double("VzCut");
    VzDiffCut            = din.read_double("VzDiffCut");
    RefMultCut           = din.read_uint("RefMultCut");
    TrackPileupCut       = din.read_int("TrackPileupCut");
    DcaCut               = din.read_double("DcaCut              ");
    PVRanking            = din.read_int("PVRanking           ");
    NMinFit              = din.read_int("NMinFit             ");
    FitOverMaxPointsCut  = din.read_double("FitOverMaxPointsCut ");

    //------------------------------------
    //  Cuts, event and particle
    //------------------------------------
    evCut_zdc_min        = din.read_int("evCut_zdc_min");
    evCut_zdc_max        = din.read_int("evCut_zdc_max");
    evCut_minjetpt       = din.read_float("evCut_minjetpt");
    evCut_matchtrig      = din.read_bool("evCut_matchtrig");
    evCut_neutptPercMax  = din.read_double("evCut_neutptPercMax");
    cstCut_minpt         = din.read_float("cstCut_minpt");
    cstCut_maxeta        = din.read_double("cstCut_maxeta");
    cstCut_mineta        = din.read_double("cstCut_mineta");
    cstCut_chargedonly   = din.read_bool("cstCut_chargedonly");

    //------------------------------------
    //  PDF plot generation
    //------------------------------------
    pdf_switch_all       = din.read_int("pdf_switch_all");
    pdf_n_pjet           = din.read_bool("pdf_n_pjet");
    pdf_n_pjet_parea     = din.read_bool("pdf_n_pjet_parea");
    pdf_pt_pjet          = din.read_bool("pdf_pt_pjet");
    pdf_pt_pjet_parea    = din.read_bool("pdf_pt_pjet_parea");
    pdf_deta_mcarlo      = din.read_bool("pdf_deta_mcarlo");
    pdf_deta_mcarlo_eff  = din.read_bool("pdf_deta_mcarlo_eff");
    dmc_x1               = din.read_double("dmc_x1");
    dmc_y1               = din.read_double("dmc_y1");
    dmc_x2               = din.read_double("dmc_x2");
    dmc_y2               = din.read_double("dmc_y2");
    pdf_sparse_projs     = din.read_bool("pdf_sparse_projs");

    //------------------------------------
    //  Selections for TH1D histograms of neutron numbers in lead/sub/trans (LST) directions
    //------------------------------------
    LST_nbins            = din.read_int("LST_nbins");
    LTS_xlo              = din.read_double("LTS_xlo");
    LTS_xhi              = din.read_double("LTS_xhi");
    master_runall        = din.read_int("master_runall");
    master_nbins         = din.read_int("master_nbins");
    quiet_run            = din.read_bool("quiet_run");

    //------------------------------------
    //  Histogram options
    //------------------------------------
    integrate2D          = din.read_bool("integrate2D");
    setLogY_1D_axis      = din.read_bool("setLogY_1D_axis");
    setLogY_2D_axis      = din.read_bool("setLogY_2D_axis");
    profile_option       = din.read_string("profile_option");

    //------------------------------------
    //  Selectors for print out statements
    //------------------------------------
    cout_matchtrig       = din.read_bool("cout_matchtrig");
    cout_finalstatcuts   = din.read_bool("cout_finalstatcuts");

    //------------------------------------
    //  read but not used, and rarely used
    //------------------------------------
    program_name          = din.read_string("program_name         ");

    //------------------------------------
    //  THnSparse Inputs
    //------------------------------------
    sparse_use           = din.read_bool("sparse_use");

    sparse_jetpT_nbins   = din.read_int("sparse_jetpT_nbins");
    sparse_jetpT_min     = din.read_float("sparse_jetpT_min");
    sparse_jetpT_max     = din.read_float("sparse_jetpT_max");

    sparse_jeteta_nbins  = din.read_int("sparse_jeteta_nbins");
    sparse_jeteta_min    = din.read_float("sparse_jeteta_min");
    sparse_jeteta_max    = din.read_float("sparse_jeteta_max");

    sparse_deta_nbins    = din.read_int("sparse_deta_nbins");
    sparse_deta_min      = din.read_float("sparse_deta_min");
    sparse_deta_max      = din.read_float("sparse_deta_max");

    sparse_pT_nbins      = din.read_int("sparse_pT_nbins");
    sparse_pT_min        = din.read_float("sparse_pT_min");
    sparse_pT_max        = din.read_float("sparse_pT_max");

    sparse_mc_dartsPbin  = din.read_int("sparse_mc_dartsPbin");
    sparse_mc_eta_nbins  = din.read_int("sparse_mc_eta_nbins");

    n_pT_slices     = din.read_int("n_pT_slices"); // enter 0 for slices
    for (int i = 0; i < n_pT_slices; i++){
        char temp[10];  
        sprintf(temp,"%i:",i+1);
        pT_slice.push_back(din.read_pair_of_floats(temp)); 
    }

  // ************************************
  // Do NOT cut high tracks and towers!
  // Instead, reject the whole event when
  // of these is found
  // ************************************
    MaxEtCut             = din.read_double("MaxEtCut            ");
    MaxTrackPt           = din.read_double("MaxTrackPt          ");

// EVENT rejection cuts
    MaxEventPtCut        = din.read_double("MaxEventPtCut");
    MaxEventEtCut        = din.read_double("MaxEventEtCut");

// Hadronic Selection Values
    ApplyFractionHadronicCorrection  = din.read_bool("ApplyFractionHadronicCorrection ");
    FractionHadronicCorrection       = din.read_double("FractionHadronicCorrection      ");
    RejectTowerElectrons             = din.read_bool("RejectTowerElectrons            ");

  /// Repetitions in the background. Anything other than 1 WILL NOT WORK because
  /// a) we're using explicit ghosts (though we don't have to)
  /// b) more importantly, the background subtractor contains fastjet::SelectorNHardest(2)
  ///    which doesn't work jet-by-jet and throws an error


    fprintf(din.fecho,"\n\n%s\n%s\n%s\n"
            , "-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- "
            , " * The following input names defaulted to the following final values. "
            , "-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- ");

    bool mod = false;
    if (save_every_nEvents < 0){
    	if (nEvents == -1) { 
            save_every_nEvents = 10000000;
            mod = true;
        } else {
            save_every_nEvents = -nEvents/(save_every_nEvents-1);
            mod = true;
        }
    }
    if (mod) { fprintf(din.fecho, "%-25s %i\n", "save_every_nEvents",save_every_nEvents); mod = false; }

    if (sparse_degphi_tranhi == 0){
        sparse_degphi_tranhi = 180. - sparse_degphi_tranlo;
        mod = true;
    }
    if (mod) { fprintf(din.fecho, "%-25s %5.2f\n", "sparse_degphi_tranhi",sparse_degphi_tranhi); mod = false; }

    if (input_rootfile == "MB") { 
        input_rootfile = "/home/fas/caines/ly247/Scratch/run12ppQA/pp200Y12PicoMB_151207_P12id_sum*.root";  mod = true;}
    else if (input_rootfile == "JP2") { 
        input_rootfile = "/home/fas/caines/ly247/Scratch/run12ppQA/pp200Y12PicoJP2_151030_P12id_sum*.root"; 
        mod = true;
    }
    if (mod) { fprintf(din.fecho, "%-25s %s\n", "input_rootfile",input_rootfile.data()); mod = false; }

    if (triggername == "def") { 
	mod = true;
    	TString tstr = input_rootfile;
	if (tstr.Contains("PicoMB")) { triggername = "ppMB"; }
	else if (tstr.Contains("PicoJP2")) { triggername = "ppJP2"; }
    }
    if (mod) { fprintf(din.fecho, "%-25s %s\n", "triggername",triggername.data()); mod = false; }

    if (add_detail_dirname) {
        mod = true;
        TString number;
        if ( nEvents == -1) { number = "all"; }
        else {
            float num = 1.*nEvents / 1000000;
            number = Form("%.1f",num);
            number.ReplaceAll(".","p");
            number.Append("M");
        }

        TString mbjp = "";
    	TString tstr = input_rootfile;
	if (tstr.Contains("PicoMB")) { mbjp = "MB"; }
	else if (tstr.Contains("PicoJP2")) { mbjp = "JP"; }
        
        // add the degrees and number of runs to the dir-name
        //  [name]_45sym_3p45M for 45, 135 degrees and 3.45 million,
        //  [name]_45_130_3p45M is more for same, but non-symmetric input (i.e. 130 != 180-45)
	if (sparse_degphi_tranhi == 180 - sparse_degphi_tranlo){
	    output_dirname = Form("%s_%s_%.0fsym_%s",output_dirname.data(),mbjp.Data(),sparse_degphi_tranlo,
		number.Data());
        } else {
	    output_dirname = Form("%s_%s_%.0f_%.0f_%s",output_dirname.data(),mbjp.Data(),
              sparse_degphi_tranlo, sparse_degphi_tranhi,number.Data());
        }
    }
    if (mod) { fprintf(din.fecho, "%-25s %s\n", "output_dirname",output_dirname.data()); mod = false; }


}
#endif