
// This is a generic input file based on the running the script 'write_inputs_cfiles.py'
// It outputs all of the required input files with some default (though not necessarily 
// suggested) input values.
//
// Note that:
//   * All lines which start with "//" are comment lines
//   * All words on any line past the first two are comments (are ignored)
//   * All lines must contain at least two words
//   * All bools are input as 0:False, 1:True
        
// First inputs (frequently changed)
    outdir                 odir2 .string.
    nEvents                5000 .llint.  set to -1 to run through all events
    save_every_nEvents        0 .int.    if neg, then will try to save ~every nEvent/abs(val)
    output_dirname            o .string.
    add_detail_dirname        1 .bool.   adds description to the output dirnam
    sparse_degphi_tranlo     45 .float.  dPhi distance away from the jet where trans starts
    sparse_degphi_tranhi      0 .float.  dPhi where Trans ends (if 0, = 180-tranlo)
    tran_max_by_n             0 .bool.   if 0 then maximize by pt, else by n
    no_pdfs                   1 .bool.   suppres pdf outputs (use ./src/post_prod instead)

    announce_every_nevents   10000 .int.    output progress about every this number of events.
    output_rootfile         out.root .string.
    input_rootfile             JP2 .string.  JP2 for JP2, or MB for minbias
    triggername                def .string. ppJP2|ppJP|ppMB, if def-> to ppJP2|ppMB from input filename
    badtower_list_loc    include/pp200Y12_badtower.list .string.
    DebugLevel                   0 .int.   
    file_runmsg             run_msg .string.

// Main modeling parameters (w/ selector cuts)
    R                       0.6 .float.  Jet measure "radius"
    efficiency_filename       0 .string.
    IntTowScale               0 .string.
    jet_algo             antikt .string.
    ghost_repeat              1 .int.   
    ghost_area             0.01 .double.
    VzCut                    30 .double.
    VzDiffCut                 5 .double.
    RefMultCut                0 .uint.  
    TrackPileupCut            0 .int.    0: no req., 1: tpx track matching to bemc or tof
                                  // 2: tof match only
    DcaCut                  1.0 .double.
    PVRanking                 0 .int.   
    NMinFit                  20 .int.   
    FitOverMaxPointsCut    0.52 .double.

  // Jetpatch zdc Quantiles (from 25M events):
  // Quantile : 0.1  val: 6225.32
  // Quantile : 0.2  val: 7029.13
  // Quantile : 0.3  val: 7620.18
  // Quantile : 0.4  val: 8253.05
  // Quantile : 0.5  val: 8871.77
  // Quantile : 0.6  val: 9478.18
  // Quantile : 0.7  val: 10198.1
  // Quantile : 0.8  val: 11025.1
  // Quantile : 0.9  val: 12074.7
  
  // minbias zdc Quantiles (from 25M events):
  // Quantile : 0.1  val: 3780.17
  // Quantile : 0.2  val: 5303.25
  // Quantile : 0.3  val: 5990.74
  // Quantile : 0.4  val: 6535.1
  // Quantile : 0.5  val: 7177.97
  // Quantile : 0.6  val: 7806.57
  // Quantile : 0.7  val: 8485.84
  // Quantile : 0.8  val: 9315.15
  // Quantile : 0.9  val: 10663
  // Quantile : 1  val: 17999

// Cuts, event and particle
    evCut_zdc_min             0 .int.    if not zero, min value for zdc coincidence rate
    evCut_zdc_max             0 .int.    if not zero, max value for zdc coincidence rate
    evCut_minjetpt            0 .float.  if not zero, cut-off for events re main jet pt
    evCut_matchtrig           0 .bool.   If SetToMatchJetTrigger is used
    evCut_neutptPercMax     0.9 .double. if 0 then not used, otherwise ratio of neutral/total perp
    cstCut_minpt            0.2 .float.  min pT to use particle (and jet)
    cstCut_maxeta             1 .double.
    cstCut_mineta            -1 .double. not use used for the selector, but in Ule.h
    cstCut_chargedonly        1 .bool.   to use only charged particles

// PDF plot generation
    pdf_switch_all            1 .int.    -1: turn off all, +1: turn on all, else ignore
    pdf_n_pjet                1 .bool.  
    pdf_n_pjet_parea          1 .bool.  
    pdf_pt_pjet               1 .bool.  
    pdf_pt_pjet_parea         1 .bool.  
    pdf_deta_mcarlo           1 .bool.  
    pdf_deta_mcarlo_eff       1 .bool.  
    dmc_x1                 0.85 .double.
    dmc_y1                 0.85 .double.
    dmc_x2                  0.7 .double.
    dmc_y2                  0.7 .double.
    pdf_sparse_projs          1 .bool.  

// Selections for TH1D histograms of neutron numbers in lead/sub/trans (LST) directions
    LST_nbins               100 .int.   
    LTS_xlo                  -1 .double. will default to evCut_minjetpt if = '-1'
    LTS_xhi                 100 .double.
    master_runall             1 .int.    1:run only sparse, 2: run all but sparse
    master_nbins              0 .int.    if not zero will over-ride all nbins
    quiet_run                 1 .bool.   suppresses printing of histograms

// Histogram options
    integrate2D               1 .bool.   To integrate 2D histogram values
    setLogY_1D_axis           0 .bool.  
    setLogY_2D_axis           0 .bool.  
    profile_option            s .string. to use the spread for profiles (not error on mean)

// Selectors for print out statements
    cout_matchtrig            0 .bool.  
    cout_finalstatcuts        1 .bool.  

// read but not used, and rarely used
    program_name          RunJetFinding .string.

// THnSparse Inputs
    sparse_use                0 .bool.  

    sparse_jetpT_nbins       35 .int.   
    sparse_jetpT_min          0 .float. 
    sparse_jetpT_max         35 .float. 

    sparse_jeteta_nbins      20 .int.   
    sparse_jeteta_min         0 .float.  If '0' then will default to min possible
    sparse_jeteta_max         0 .float.  If '1' then will default to max possible

    sparse_deta_nbins        20 .int.   
    sparse_deta_min           0 .float.  If either min or max (or both) is zero 
    sparse_deta_max           0 .float.   then it will default to that max range

    sparse_pT_nbins          10 .int.   
    sparse_pT_min             0 .float.  pT per area searched (d-eta d-phi
    sparse_pT_max             2 .float.  see above

    sparse_mc_dartsPbin      50 .int.    Monte Carlo, darts per bin
    sparse_mc_eta_nbins      50 .int.    number of monte carlo to fill per jeta_eta bins

    n_pT_slices     5 .int. read how many slices to make; 1 entry to line
// # start finish
   1:    0   0
   2:    0   5
   3:    5  15
   4:   15  25
   5:   25  70
    MaxEtCut               1000 .double.
    MaxTrackPt             1000 .double.

// EVENT rejection cuts
    MaxEventPtCut            20 .double.
    MaxEventEtCut            50 .double.

// Hadronic Selection Values
    ApplyFractionHadronicCorrection       1 .bool.  
    FractionHadronicCorrection       0.9999 .double.
    RejectTowerElectrons                  0 .bool.  
