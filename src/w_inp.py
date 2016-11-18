#!/usr/bin/env python

# This script is to write the Input Data File for c-programs. It writes two files:
 # 1. The Input_py.cc file (this is the one which is compiled
 # 2. The inputs      file (this is the one which is read by the computer)


class Words:
    def __init__(self, f_c, f_h, f_user):
        self.final_set = [];
        self.spaces = 4
        self.comment_nspaces = 0;
        self.fc  = f_c
        self.fh  = f_h
        self.fuser = f_user

    def fcfh_write(self, text):
        self.fc.write(text)
        self.fh.write(text)

    def intups(self, header, in_tuples):
        if header:
            self.fcfh_write('\n'+' '*self.spaces+'//'+'-'*(40-self.spaces)+'\n'
                           +' '*self.spaces+'//  ' + header     +'\n'
                           +' '*self.spaces+'//'+'-'*(40-self.spaces)+'\n')
            self.fuser.write('\n// ' + header + '\n')
        #temp_list = []
        for entry in in_tuples:
            if not entry:
                self.fuser.write('\n'),
                self.fcfh_write('\n'),
                continue
            if len(entry) == 1:
                self.fuser.write(entry[0]+'\n')
                self.fcfh_write(entry[0]+'\n')
                continue
            ctype, name = entry[0], entry[1]
            self.final_set.append((ctype,name))
            self.fc.write(' '*self.spaces+'%-20s = din.read_%s("%s");\n'%(name,ctype,name))
            if (ctype == 'llint'):
                self.fh.write(' '*self.spaces + 'long long int ' + name + ';\n')
            elif (ctype == 'uint'):
                self.fh.write(' '*self.spaces + 'unsigned int  ' + name + ';\n')
            elif (ctype == 'string'):
                self.fh.write(' '*self.spaces + 'TString       ' + name + ';\n')
            else:
                self.fh.write(' '*self.spaces + '%-14s %s;\n'%(ctype,name))

            #temp_list.append((ctype,name))
            # fc = 
                
            typstr = '.'+ctype+'.'
            if len(entry) == 2:
                self.fuser.write('    %-20s <!!> %-8s'%(name,typstr))
            else:
                self.fuser.write('    %-20s %6s %-8s'%(name,entry[2],typstr))

            if len(entry) > 3:
                self.fuser.write(' '+entry[3]+'\n')
                for E in tuple(entry[4:]):
                    self.fuser.write(' '*34 + '// ' + E + '\n')
            else: self.fuser.write('\n')

fh   = open('InputReader.h','w')
fc   = open('InputReader_c.h','w')
fuser = open('user.i','w') #'run/RunJetFinding.input','w')
words = Words(fc,fh,fuser)

# Print the beginnings of the all the files
fuser.write("""
// This is a generic input file based on the running the script 'write_inputs_cfiles.py'
// It outputs all of the required input files with some default (though not necessarily 
// suggested) input values.
//
// Note that:
//   * All lines which start with "//" are comment lines
//   * All words on any line past the first two are comments, i.e. they are ignored
//   * Any input line reads only the first two words
//   * Any line with no uncommented words is ignored
//   * whitespace is used only as separators. Otherwise, it is ignored.
//   * All bools are input as 0:False, 1:True
        """)

fh.write("""

#ifndef INPUTREADER_H
#define INPUTREADER_H

#define MAX_SIZE  400

#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<iostream>
#include<vector>
#include<TString.h>

/*! \brief List of user input parameters.
 *
 *  The following are lists of input parameters which are read from the
 *  compiled code at run-time.
 */

class InputReader
{
    public:
    ~InputReader();
    InputReader(const std::string fin_name);
    std::string file_name;
    FILE *fin = NULL;
    FILE *fecho;
    void open_file(const std::string &name_in);
    void fatal_error(const char* err_msg);
    int is_empty(const char *s);
    char tmpstr[MAX_SIZE];
    char tmpstr2[MAX_SIZE];
    char fmtstr[MAX_SIZE];

    void check_read_error(const char* varstr, const int &count, int number_read);

    template <typename T> 
    void read(T &rval, const char* var_name, const char* read_fmt);
    void read_to_var_name(const char* var_name);
    void close_files();
    const double read_double (const char* var_name);
    const float read_float (const char* var_name);
    const int read_int (const char* var_name);
    const bool read_bool (const char* var_name);
    const unsigned int read_uint (const char* var_name);
    const long long read_llint (const char* var_name);
    const TString read_string (const char* var_name);
    std::pair<float, float> read_pair_of_floats(const char* var_name);
};

struct TextFileInputs
{
	TextFileInputs(InputReader&);
        void PrintCurrentVals(const TString&, const int&);
""")

fc.write(r"""
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
""")

##############################################################################################
# Variable inputs
##############################################################################################
words.intups("First inputs (frequently changed)", (
    ('string' , 'odir'            , 'PicoJetOut', 'output directory name'),
    ('string' , 'InputFiles'      , 
        '/home/fas/caines/ly247/scratch/run12ppQA/pp200Y12PicoJP*.root'
                                  , 'can use globber names'),
    ('string' , 'OutFileName'     , 'PicoJetUnderlyingActivity', ''),
    ('int'    , 'nEvents'         , '-1'      , 'if -1, will select all events'),
    ('float'  , 'JetRes_R'        , '0.6'      , ''),
    ('int'    , 'TrigFlagId'      , '0'       , ''),
    ('int'    , 'TrigId'          , '0'       , ''),
    ('string' , 'TriggerName'     , 'ppJP2'   , 'ppMB,ppJP0/1/2'),
    ('string' , 'ChainName'       , 'JetTree' , ''),
    ('bool'   , 'isAuAu'          , '0'       , ''),
    ('int'    , 'IntTowScale'     , '0'       , ''),
    ('float'  , 'fTowScale'       , '0.'      , 'if zero, defaults to 1+0.02*IntTowScale'),
    ('int'    , 'mEffUn'          , '0'       , ''),
    ('int'    , 'JetChargeCode'   , '2'       , '0=neutral, 1=charged, 2=fulljet'),
    ('int'    , 'UnderlyingChargeCode' , '2'  , 'same as jet charge code'),
    ('int'    , 'TrackPileUpCut'       , '0'  , '0=no req, 1=tpc track matching, ',
                           '2=tof match only, 3=tpc track matching to bemc',
                           '<odir-built>: NoTofMatch=0, BemcOrTofMatch=1, default = 2, BemcMatch=3'),
    ('int'    , 'TStarJetDebugLevel'   , '0'  , ''),
    ('string' , 'JetAlgorithm'         , 'antikt'  , 'kt or antikt'),
    False,
    ('int'    , 'TStarJet_DebugLevel'  , '0'  , ''),
    ('bool'   , 'UlaToMatchTrig'       , '0'  , ''),
    ('bool'   , 'NeutralJetFractionCut', '1'  , ''),
    ('float'  , 'RefMultCut'           , '0.' , 'this was a hardcoded value'),
    False,
    ('float'  , 'CstMinPt'             , '0.' , ''),
    ('int'    , 'DiJetAngle'           , '1'  , '0 or 1'),
    ('float'  , 'TransversePhiSize'    , '60.', ''),
    ('string' , 'badrun_list'          , './include/pp200Y12_badrun.list',
                                        'file listing badruns'),
    ('bool'   , 'noprint_badrunid'     , '1'  , ''),
))
words.intups("Some of the Aj Parameters", (
    ('float'  , 'MaxTrackRap'          , '1.0', ''),
    ('float'  , 'dPhiCut'              , '0.4', ''),
))

#  //some of the Aj Parameters
#    ('string'  , 'outdir'                , 'outdir' ),
#    ('llint'   , 'nEvents'               , 5000, 'set to -1 to run through all events'), #2000
#    ('int'     , 'save_every_nEvents'    ,  0, 'if neg, then will try to save ~every nEvent/abs(val)'),
#    ('string'  , 'output_dirname'        , 'o' ),
#    ('bool'    , 'add_detail_dirname'     , 1, 'adds description to the output dirnam'),
#    ('float'   , 'sparse_degphi_tranlo',  45, 'dPhi distance away from the jet where trans starts'),
#    ('float'   , 'sparse_degphi_tranhi',   0, 'dPhi where Trans ends (if 0, = 180-tranlo)'),
#    ('bool'    , 'tran_max_by_n'       ,   0, 'if 0 then maximize by pt, else by n'),
#    ('bool'    , 'no_pdfs'               ,  1, 'suppres pdf outputs (use ./src/post_prod instead)'),
#   False,
#    ('int'     , 'announce_every_nevents ', 10000, 'output progress about every this number of events.'),
#    ('string'  , 'output_rootfile        ', 'out.root' ),
#    ('string'  , 'input_rootfile         ',  'JP2', ' JP2 for JP2, or MB for minbias'),
#    ('string'  , 'triggername            ', 'def', 'ppJP2|ppJP|ppMB, if def-> to ppJP2|ppMB from input filename'),
#    #('string'  , 'TTreeChainName'      ,  'def', 'if def, then default to JetTree and pp12tree'),
#    ('string'  , 'badtower_list_loc', 'include/pp200Y12_badtower.list' ),
#    ('int'     , 'DebugLevel             ', 0 ),
#    ('string'  , 'file_runmsg            ', 'run_msg'),
#    ))
#
#words.intups("Main modeling parameters (w/ selector cuts)", (
#    ('float'   , 'R'            , '0.6', 'Jet measure "radius"'),
#    ('string'  , 'efficiency_filename', 0),
#    ('string'  , 'IntTowScale        ', 0 ),
#    ('string'  , 'jet_algo'          , 'antikt'),
#    ('int'     , 'ghost_repeat'      ,  1    ),
#    ('double'  , 'ghost_area'        ,  0.01 ),
#    ('double'  , 'VzCut'             ,  30   ),
#    ('double'  , 'VzDiffCut'         ,  5    ),
#    ('uint'    , 'RefMultCut'        ,  0    ),
#    ('int'     , 'TrackPileupCut'    ,  0    , '0: no req., 1: tpx track matching to bemc or tof'
#                                             , '2: tof match only'), #lyon is 0
#    ('double'  , 'DcaCut              ',  1.0  ),
#    ('int'     , 'PVRanking           ',  0    ),
#    ('int'     , 'NMinFit             ',  20   ),
#    ('double'  , 'FitOverMaxPointsCut ', 0.52),
#    ))
#
#fuser.write("""
#  // Jetpatch zdc Quantiles (from 25M events):
#  // Quantile : 0.1  val: 6225.32
#  // Quantile : 0.2  val: 7029.13
#  // Quantile : 0.3  val: 7620.18
#  // Quantile : 0.4  val: 8253.05
#  // Quantile : 0.5  val: 8871.77
#  // Quantile : 0.6  val: 9478.18
#  // Quantile : 0.7  val: 10198.1
#  // Quantile : 0.8  val: 11025.1
#  // Quantile : 0.9  val: 12074.7
#  
#  // minbias zdc Quantiles (from 25M events):
#  // Quantile : 0.1  val: 3780.17
#  // Quantile : 0.2  val: 5303.25
#  // Quantile : 0.3  val: 5990.74
#  // Quantile : 0.4  val: 6535.1
#  // Quantile : 0.5  val: 7177.97
#  // Quantile : 0.6  val: 7806.57
#  // Quantile : 0.7  val: 8485.84
#  // Quantile : 0.8  val: 9315.15
#  // Quantile : 0.9  val: 10663
#  // Quantile : 1  val: 17999
#""")
#
#words.intups("Cuts, event and particle", (
#    ('int'     , 'evCut_zdc_min'      ,   0, 'if not zero, min value for zdc coincidence rate'),
#    ('int'     , 'evCut_zdc_max'      ,   0, 'if not zero, max value for zdc coincidence rate'),
#    ('float'   , 'evCut_minjetpt'     ,   0, 'if not zero, cut-off for events re main jet pt'),
#    ('bool'    , 'evCut_matchtrig'    ,   0   , 'If SetToMatchJetTrigger is used'),
#    ('double'  , 'evCut_neutptPercMax',  0.9  , 'if 0 then not used, otherwise ratio of neutral/total perp'),
#    ('float'   , 'cstCut_minpt'       , 0.2, 'min pT to use particle (and jet)'),
#    ('double'  , 'cstCut_maxeta'      , 1 ),
#    ('double'  , 'cstCut_mineta'      , -1, 'not use used for the selector, but in Ule.h'),
#    ('bool'    , 'cstCut_chargedonly' ,   1, 'to use only charged particles'),
#    ))
#
#
#words.intups("PDF plot generation", (
#    ('int'     , 'pdf_switch_all'   ,   1, '-1: turn off all, +1: turn on all, else ignore'),
#    ('bool'    , 'pdf_n_pjet'       ,   1),
#    ('bool'    , 'pdf_n_pjet_parea' ,   1),
#    ('bool'    , 'pdf_pt_pjet'      ,   1),
#    ('bool'    , 'pdf_pt_pjet_parea',   1),
#    ('bool'    , 'pdf_deta_mcarlo'  ,   1),
#    ('bool'    , 'pdf_deta_mcarlo_eff', 1),
#    ('double'  , 'dmc_x1'   , .85),
#    ('double'  , 'dmc_y1'   , .85),
#    ('double'  , 'dmc_x2'   , .7),
#    ('double'  , 'dmc_y2'   , .7),
#    ('bool'    , 'pdf_sparse_projs' ,  1),
#
#    ))
#
#words.intups("Selections for TH1D histograms of neutron numbers in lead/sub/trans (LST) directions",(
#    ('int'     , 'LST_nbins'    ,  100,),
#    ('double'  , 'LTS_xlo'      ,   -1, "will default to evCut_minjetpt if = '-1'"),
#    ('double'  , 'LTS_xhi'      ,  100),
#    ('int'     , 'master_runall',    1, '1:run only sparse, 2: run all but sparse'),
#    ('int'     , 'master_nbins' ,    0, 'if not zero will over-ride all nbins'),
#    ('bool'    , 'quiet_run'    ,    1, 'suppresses printing of histograms'),
#    ))
#
#words.intups("Histogram options",(
#    ('bool'    , 'integrate2D',      1,  'To integrate 2D histogram values'),
#    ('bool'    , 'setLogY_1D_axis' , 0),
#    ('bool'    , 'setLogY_2D_axis' , 0),
#    ('string'    , 'profile_option'  , 's', 'to use the spread for profiles (not error on mean)'),
#    ))
#
#words.intups( "Selectors for print out statements",(
#        ('bool', 'cout_matchtrig'      , 0),
#        ('bool', 'cout_finalstatcuts'  , 1)
#    ))
#
#words.intups("read but not used, and rarely used", (
#    ('string'  , 'program_name         ', 'RunJetFinding' ), #this input is read but not used
#))
#
#words.intups(
#  "THnSparse Inputs",(
#   ('bool'     , 'sparse_use', 0),
#   False,
#   ('int'     , 'sparse_jetpT_nbins' , 35 ),
#   ('float'   , 'sparse_jetpT_min'   , 0  ),
#   ('float'   , 'sparse_jetpT_max'   , 35 ),
#   False,
#   ('int'     , 'sparse_jeteta_nbins',20  ),
#   ('float'   , 'sparse_jeteta_min'  , 0, "If '0' then will default to min possible"),
#   ('float'   , 'sparse_jeteta_max'  , 0, "If '1' then will default to max possible"),
#   False,
#   ('int'     , 'sparse_deta_nbins', 20  ),
#   ('float'   , 'sparse_deta_min'  , 0, 'If either min or max (or both) is zero '),
#   ('float'   , 'sparse_deta_max'  , 0, ' then it will default to that max range'),
#   False,
#   # ('int'     , 'sparse_n_nbins',     5  ),
#   # ('float'   , 'sparse_n_min'  ,    .5, 'n per area searched (deta bin)'),
#   # ('float'   , 'sparse_n_max'  ,    5.5,'see above'),
#   # False,
#   ('int'     , 'sparse_pT_nbins',   10  ),
#   ('float'   , 'sparse_pT_min'  ,    0, 'pT per area searched (d-eta d-phi'),
#   ('float'   , 'sparse_pT_max'  ,    2,'see above'),
#   False,
#   ('int'       , 'sparse_mc_dartsPbin' ,  50, 'Monte Carlo, darts per bin'),
#   ('int'       , 'sparse_mc_eta_nbins' ,  50, 'number of monte carlo to fill per jeta_eta bins'),
#   ))

#fc.write("""
#    n_pT_slices     = din.read_int("n_pT_slices"); // enter 0 for slices
#    for (int i = 0; i < n_pT_slices; i++){
#        char temp[10];  
#        sprintf(temp,"%i:",i+1);
#        pT_slice.push_back(din.read_pair_of_floats(temp)); 
#    }
#""")
#fh.write("""
#    int n_pT_slices;
#    std::vector<std::pair<float, float>> pT_slice;
#    """)
#
#fuser.write("""
#    n_pT_slices     5 .int. read how many slices to make; 1 entry to line
#// # start finish
#   1:    0   0
#   2:    0   5
#   3:    5  15
#   4:   15  25
#   5:   25  70
#""")

## put in the old i_Aj variables
#words.fcfh_write("""
#  // ************************************
#  // Do NOT cut high tracks and towers!
#  // Instead, reject the whole event when
#  // of these is found
#  // ************************************
#""")
#words.intups(False, (
#    ('double'  , 'MaxEtCut            ', 1000         ),
#    ('double'  , 'MaxTrackPt          ', 1000        ),
##fcc.write("         // EVENT rejection cuts\n")
##finp.write("// EVENT rejection cuts\n")
#    False,
#    ('// EVENT rejection cuts',),
#    ('double'  , 'MaxEventPtCut', 20     ),
#    ('double'  , 'MaxEventEtCut', 50     ),
#    False,
#    ('// Hadronic Selection Values',),
##fcc.write("         // Hadronic Selection Values\n")
##finp.write("// Hadronic Selection Values\n")
##words.intups(False, (
#    ('bool'  ,  'ApplyFractionHadronicCorrection ', 1),
#    ('double',  'FractionHadronicCorrection      ', 0.9999),
#    ('bool'  ,  'RejectTowerElectrons            ', 0)))
#
#fh.write("};")
#words.fcfh_write("""
#  /// Repetitions in the background. Anything other than 1 WILL NOT WORK because
#  /// a) we're using explicit ghosts (though we don't have to)
#  /// b) more importantly, the background subtractor contains fastjet::SelectorNHardest(2)
#  ///    which doesn't work jet-by-jet and throws an error
#""")


#fc.write(r"""
#
#    fprintf(din.fecho,"\n\n%s\n%s\n%s\n"
#            , "-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- "
#            , " * The following input names defaulted to the following final values. "
#            , "-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- ");
#
#    bool mod = false;
#    if (save_every_nEvents < 0){
#    	if (nEvents == -1) { 
#            save_every_nEvents = 10000000;
#            mod = true;
#        } else {
#            save_every_nEvents = -nEvents/(save_every_nEvents-1);
#            mod = true;
#        }
#    }
#    if (mod) { fprintf(din.fecho, "%-25s %i\n", "save_every_nEvents",save_every_nEvents); mod = false; }
#
#    if (sparse_degphi_tranhi == 0){
#        sparse_degphi_tranhi = 180. - sparse_degphi_tranlo;
#        mod = true;
#    }
#    if (mod) { fprintf(din.fecho, "%-25s %5.2f\n", "sparse_degphi_tranhi",sparse_degphi_tranhi); mod = false; }
#
#    if (input_rootfile == "MB") { 
#        input_rootfile = "/home/fas/caines/ly247/Scratch/run12ppQA/pp200Y12PicoMB_151207_P12id_sum*.root";  mod = true;}
#    else if (input_rootfile == "JP2") { 
#        input_rootfile = "/home/fas/caines/ly247/Scratch/run12ppQA/pp200Y12PicoJP2_151030_P12id_sum*.root"; 
#        mod = true;
#    }
#    if (mod) { fprintf(din.fecho, "%-25s %s\n", "input_rootfile",input_rootfile.data()); mod = false; }
#
#    if (triggername == "def") { 
#	mod = true;
#    	TString tstr = input_rootfile;
#	if (tstr.Contains("PicoMB")) { triggername = "ppMB"; }
#	else if (tstr.Contains("PicoJP2")) { triggername = "ppJP2"; }
#    }
#    if (mod) { fprintf(din.fecho, "%-25s %s\n", "triggername",triggername.data()); mod = false; }
#
#    if (add_detail_dirname) {
#        mod = true;
#        TString number;
#        if ( nEvents == -1) { number = "all"; }
#        else {
#            float num = 1.*nEvents / 1000000;
#            number = Form("%.1f",num);
#            number.ReplaceAll(".","p");
#            number.Append("M");
#        }
#
#        TString mbjp = "";
#    	TString tstr = input_rootfile;
#	if (tstr.Contains("PicoMB")) { mbjp = "MB"; }
#	else if (tstr.Contains("PicoJP2")) { mbjp = "JP"; }
#        
#        // add the degrees and number of runs to the dir-name
#        //  [name]_45sym_3p45M for 45, 135 degrees and 3.45 million,
#        //  [name]_45_130_3p45M is more for same, but non-symmetric input (i.e. 130 != 180-45)
#	if (sparse_degphi_tranhi == 180 - sparse_degphi_tranlo){
#	    output_dirname = Form("%s_%s_%.0fsym_%s",output_dirname.data(),mbjp.Data(),sparse_degphi_tranlo,
#		number.Data());
#        } else {
#	    output_dirname = Form("%s_%s_%.0f_%.0f_%s",output_dirname.data(),mbjp.Data(),
#              sparse_degphi_tranlo, sparse_degphi_tranhi,number.Data());
#        }
#    }
#    if (mod) { fprintf(din.fecho, "%-25s %s\n", "output_dirname",output_dirname.data()); mod = false; }
#
#
#}
##endif""")
fc.write(r"""}

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
""")
#print the list of tuples
def str_ret(x):
    return {
        'uint'   : '%-20o  \\n\",\"',
        'llint'  : '%-20i  \\n\",\"',
        'int'    : '%-20i  \\n\",\"',
        'bool'   : '%-20i  \\n\",\"',
        'string' : '%-20s  \\n\",\"',
        'float'  : '%-20.8f\\n\",\"',
        'double' : '%-20.8f\\n\",\"',
     }[x]
    
for E in words.final_set:
    temp_string = ('    fprintf(fout,"  %-22s  ' + str_ret(E[0])  
                  + E[1] + '", ' + E[1])
    if E[0] == "string": temp_string += '.Data()'
    temp_string += ");\n"
    fc.write(temp_string)


#need to write the print statements for all of the keys here...
fc.write("}\n")
fc.write("#endif // INPUTFILES_C\n")


fh.write("};\n")
fh.write("#endif // INPUTFILES_H\n")
