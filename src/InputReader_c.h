
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

const TString InputReader::read_string (const char* var_name){
    char new_temp[MAX_SIZE];
    read(new_temp, var_name,"s");

    TString rval = new_temp;
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
    odir                 = din.read_string("odir");
    InputFiles           = din.read_string("InputFiles");
    OutFileName          = din.read_string("OutFileName");
    nEvents              = din.read_int("nEvents");
    JetRes_R             = din.read_float("JetRes_R");
    TrigFlagId           = din.read_int("TrigFlagId");
    TrigId               = din.read_int("TrigId");
    TriggerName          = din.read_string("TriggerName");
    ChainName            = din.read_string("ChainName");
    isAuAu               = din.read_bool("isAuAu");
    IntTowScale          = din.read_int("IntTowScale");
    fTowScale            = din.read_float("fTowScale");
    mEffUn               = din.read_int("mEffUn");
    JetChargeCode        = din.read_int("JetChargeCode");
    UnderlyingChargeCode = din.read_int("UnderlyingChargeCode");
    TrackPileUpCut       = din.read_int("TrackPileUpCut");
    TStarJetDebugLevel   = din.read_int("TStarJetDebugLevel");
    JetAlgorithm         = din.read_string("JetAlgorithm");

    TStarJet_DebugLevel  = din.read_int("TStarJet_DebugLevel");
    UlaToMatchTrig       = din.read_bool("UlaToMatchTrig");
    NeutralJetFractionCut = din.read_bool("NeutralJetFractionCut");
    RefMultCut           = din.read_float("RefMultCut");

    CstMinPt             = din.read_float("CstMinPt");
    DiJetAngle           = din.read_int("DiJetAngle");
    TransversePhiSize    = din.read_float("TransversePhiSize");
    badrun_list          = din.read_string("badrun_list");
    noprint_badrunid     = din.read_bool("noprint_badrunid");

    //------------------------------------
    //  Some of the Aj Parameters
    //------------------------------------
    MaxTrackRap          = din.read_float("MaxTrackRap");
    dPhiCut              = din.read_float("dPhiCut");
}

void TextFileInputs::PrintCurrentVals( const TString &fout_name,
                                       const int& which_write){
    FILE *fout;
    fout = fopen(fout_name.Data(),"w");
    if (fout == NULL){
        std::cout << "WARNING: Cannot open the output file " << fout_name
                  << " to use to write out the members of 'user'.\n";
        exit(1);
    };

    fprintf(fout,"// -- Printing: %i\n\n",which_write);
    fprintf(fout,"// This is an echo of all of the inputs to the module \"user\"\n"
                 "// AFTER they have been modified by internal logic, due in part\n"
                 "// (potentially) to other command line arguments.\n\n"
                 "// This file may be used to inspect the parameters used and also\n"
                 "// as a way to re-run the program with the same arguments without\n"
                 "// (without needing any other command line arguments)\n\n");
    fprintf(fout,"//%-22s  %-20s\n","keyword","value");
    fprintf(fout,"  %-22s  %-20s  \n","odir", odir.Data());
    fprintf(fout,"  %-22s  %-20s  \n","InputFiles", InputFiles.Data());
    fprintf(fout,"  %-22s  %-20s  \n","OutFileName", OutFileName.Data());
    fprintf(fout,"  %-22s  %-20i  \n","nEvents", nEvents);
    fprintf(fout,"  %-22s  %-20.8f\n","JetRes_R", JetRes_R);
    fprintf(fout,"  %-22s  %-20i  \n","TrigFlagId", TrigFlagId);
    fprintf(fout,"  %-22s  %-20i  \n","TrigId", TrigId);
    fprintf(fout,"  %-22s  %-20s  \n","TriggerName", TriggerName.Data());
    fprintf(fout,"  %-22s  %-20s  \n","ChainName", ChainName.Data());
    fprintf(fout,"  %-22s  %-20i  \n","isAuAu", isAuAu);
    fprintf(fout,"  %-22s  %-20i  \n","IntTowScale", IntTowScale);
    fprintf(fout,"  %-22s  %-20.8f\n","fTowScale", fTowScale);
    fprintf(fout,"  %-22s  %-20i  \n","mEffUn", mEffUn);
    fprintf(fout,"  %-22s  %-20i  \n","JetChargeCode", JetChargeCode);
    fprintf(fout,"  %-22s  %-20i  \n","UnderlyingChargeCode", UnderlyingChargeCode);
    fprintf(fout,"  %-22s  %-20i  \n","TrackPileUpCut", TrackPileUpCut);
    fprintf(fout,"  %-22s  %-20i  \n","TStarJetDebugLevel", TStarJetDebugLevel);
    fprintf(fout,"  %-22s  %-20s  \n","JetAlgorithm", JetAlgorithm.Data());
    fprintf(fout,"  %-22s  %-20i  \n","TStarJet_DebugLevel", TStarJet_DebugLevel);
    fprintf(fout,"  %-22s  %-20i  \n","UlaToMatchTrig", UlaToMatchTrig);
    fprintf(fout,"  %-22s  %-20i  \n","NeutralJetFractionCut", NeutralJetFractionCut);
    fprintf(fout,"  %-22s  %-20.8f\n","RefMultCut", RefMultCut);
    fprintf(fout,"  %-22s  %-20.8f\n","CstMinPt", CstMinPt);
    fprintf(fout,"  %-22s  %-20i  \n","DiJetAngle", DiJetAngle);
    fprintf(fout,"  %-22s  %-20.8f\n","TransversePhiSize", TransversePhiSize);
    fprintf(fout,"  %-22s  %-20s  \n","badrun_list", badrun_list.Data());
    fprintf(fout,"  %-22s  %-20i  \n","noprint_badrunid", noprint_badrunid);
    fprintf(fout,"  %-22s  %-20.8f\n","MaxTrackRap", MaxTrackRap);
    fprintf(fout,"  %-22s  %-20.8f\n","dPhiCut", dPhiCut);
}
#endif // INPUTFILES_C
