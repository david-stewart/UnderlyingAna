

#ifndef INPUTREADER_H
#define INPUTREADER_H

#define MAX_SIZE  400

#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<iostream>
#include<vector>
#include<TString.h>

/*! rief List of user input parameters.
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
    const std::string read_string (const char* var_name);
    std::pair<float, float> read_pair_of_floats(const char* var_name);
};

struct TextFileInputs
{
	TextFileInputs(InputReader&);


    //------------------------------------
    //  First inputs (frequently changed)
    //------------------------------------
    std::string   outdir;
    long long int nEvents;
    int            save_every_nEvents;
    std::string   output_dirname;
    bool           add_detail_dirname;
    float          sparse_degphi_tranlo;
    float          sparse_degphi_tranhi;
    bool           tran_max_by_n;
    bool           no_pdfs;

    int            announce_every_nevents ;
    std::string   output_rootfile        ;
    std::string   input_rootfile         ;
    std::string   triggername            ;
    std::string   badtower_list_loc;
    int            DebugLevel             ;
    std::string   file_runmsg            ;

    //------------------------------------
    //  Main modeling parameters (w/ selector cuts)
    //------------------------------------
    float          R;
    std::string   efficiency_filename;
    std::string   IntTowScale        ;
    std::string   jet_algo;
    int            ghost_repeat;
    double         ghost_area;
    double         VzCut;
    double         VzDiffCut;
    unsigned int  RefMultCut;
    int            TrackPileupCut;
    double         DcaCut              ;
    int            PVRanking           ;
    int            NMinFit             ;
    double         FitOverMaxPointsCut ;

    //------------------------------------
    //  Cuts, event and particle
    //------------------------------------
    int            evCut_zdc_min;
    int            evCut_zdc_max;
    float          evCut_minjetpt;
    bool           evCut_matchtrig;
    double         evCut_neutptPercMax;
    float          cstCut_minpt;
    double         cstCut_maxeta;
    double         cstCut_mineta;
    bool           cstCut_chargedonly;

    //------------------------------------
    //  PDF plot generation
    //------------------------------------
    int            pdf_switch_all;
    bool           pdf_n_pjet;
    bool           pdf_n_pjet_parea;
    bool           pdf_pt_pjet;
    bool           pdf_pt_pjet_parea;
    bool           pdf_deta_mcarlo;
    bool           pdf_deta_mcarlo_eff;
    double         dmc_x1;
    double         dmc_y1;
    double         dmc_x2;
    double         dmc_y2;
    bool           pdf_sparse_projs;

    //------------------------------------
    //  Selections for TH1D histograms of neutron numbers in lead/sub/trans (LST) directions
    //------------------------------------
    int            LST_nbins;
    double         LTS_xlo;
    double         LTS_xhi;
    int            master_runall;
    int            master_nbins;
    bool           quiet_run;

    //------------------------------------
    //  Histogram options
    //------------------------------------
    bool           integrate2D;
    bool           setLogY_1D_axis;
    bool           setLogY_2D_axis;
    std::string   profile_option;

    //------------------------------------
    //  Selectors for print out statements
    //------------------------------------
    bool           cout_matchtrig;
    bool           cout_finalstatcuts;

    //------------------------------------
    //  read but not used, and rarely used
    //------------------------------------
    std::string   program_name         ;

    //------------------------------------
    //  THnSparse Inputs
    //------------------------------------
    bool           sparse_use;

    int            sparse_jetpT_nbins;
    float          sparse_jetpT_min;
    float          sparse_jetpT_max;

    int            sparse_jeteta_nbins;
    float          sparse_jeteta_min;
    float          sparse_jeteta_max;

    int            sparse_deta_nbins;
    float          sparse_deta_min;
    float          sparse_deta_max;

    int            sparse_pT_nbins;
    float          sparse_pT_min;
    float          sparse_pT_max;

    int            sparse_mc_dartsPbin;
    int            sparse_mc_eta_nbins;

    int n_pT_slices;
    std::vector<std::pair<float, float>> pT_slice;
    
  // ************************************
  // Do NOT cut high tracks and towers!
  // Instead, reject the whole event when
  // of these is found
  // ************************************
    double         MaxEtCut            ;
    double         MaxTrackPt          ;

// EVENT rejection cuts
    double         MaxEventPtCut;
    double         MaxEventEtCut;

// Hadronic Selection Values
    bool           ApplyFractionHadronicCorrection ;
    double         FractionHadronicCorrection      ;
    bool           RejectTowerElectrons            ;
};
  /// Repetitions in the background. Anything other than 1 WILL NOT WORK because
  /// a) we're using explicit ghosts (though we don't have to)
  /// b) more importantly, the background subtractor contains fastjet::SelectorNHardest(2)
  ///    which doesn't work jet-by-jet and throws an error
#endif // INPUTFILES_H
