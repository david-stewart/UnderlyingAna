
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
        
// First inputs (frequently changed)
    odir                 PictoJetEtc .string. output directory name
    InputFiles              ../input/location/string .string. can use globber names
    OutFileName          PicoJetUnderlyingActivity .string. 
    JetRes_R                 0. .float.  
    TrigFlagId                0 .int.    
    TriggerName           ppJP2 .string. 
    ChainName            JetTree .string. 
    isAuAu                    0 .bool.   
    IntTowScale               0 .int.    
    fTowScale                0. .float.  if zero, defaults to 1+0.02*IntTowScale
    mEffUn                    1 .int.    
    JetChargeCode             2 .int.    0=neutral, 1=charged, 2=fulljet
    UnderlyingChargeCode      2 .int.    same as jet charge code
    TrackPileUpCut            0 .int.    0=no req, 1=tpc track matching, 
                                  // 2=tof match only, 3=tpc track matching to bemc
                                  // <odir-built>: NoTofMatch=0, BemcOrTofMatch=1, default = 2, BemcMatch=3
    TStarJetDebugLevel        0 .int.    
    JetAlgorithm              0 .string. kt or antikt

    TStarJet_DebugLevel       0 .int.    
    UlaToMatchTrig            0 .bool.   
    NeutralJetFractionCut      1 .bool.   
    RefMultCut               0. .float.  this was a hardcoded value

    CstMinPt                 0. .float.  
    DiJetAngle                1 .int.    0 or 1
    TransversePhiSize       60. .float.  

// Some of the Aj Parameters
    MaxTrackRap             1.0 .float.  
    dPhiCut                 0.4 .float.  
