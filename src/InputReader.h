

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
    const TString read_string (const char* var_name);
    std::pair<float, float> read_pair_of_floats(const char* var_name);
};

struct TextFileInputs
{
	TextFileInputs(InputReader&);
        void PrintCurrentVals(const TString&, const int&);

    //------------------------------------
    //  First inputs (frequently changed)
    //------------------------------------
    TString       odir;
    TString       InputFiles;
    TString       OutFileName;
    int            nEvents;
    float          JetRes_R;
    int            TrigFlagId;
    int            TrigId;
    TString       TriggerName;
    TString       ChainName;
    bool           isAuAu;
    int            IntTowScale;
    float          fTowScale;
    int            mEffUn;
    int            JetChargeCode;
    int            UnderlyingChargeCode;
    int            TrackPileUpCut;
    int            TStarJetDebugLevel;
    TString       JetAlgorithm;

    int            TStarJet_DebugLevel;
    bool           UlaToMatchTrig;
    bool           NeutralJetFractionCut;
    float          RefMultCut;

    float          CstMinPt;
    int            DiJetAngle;
    float          TransversePhiSize;
    TString       badrun_list;
    bool           noprint_badrunid;

    //------------------------------------
    //  Some of the Aj Parameters
    //------------------------------------
    float          MaxTrackRap;
    float          dPhiCut;

    //------------------------------------
    //  AjParameters.hh input values
    //------------------------------------
    int             ghost_repeat;
    double         ghost_area;

    double         jet_ptmin;
    double         jet_ptmax;

    double         JetNeutralPertMax;

    double         LeadPtMin;
    double         SubLeadPtMin;

    double         PtConsHi;

    double         VzCut;
    double         VzDiffCut;

    double         DcaCut;
    int            NMinFit;
    double         FitOverMaxPointsCut;

    double         MaxEtCut;
    double         MaxTrackPt;

    //------------------------------------
    //  EVENT rejection cuts
    //------------------------------------
    double         MaxEventPtCut;
    double         MaxEventEtCut;
};
#endif // INPUTFILES_H
