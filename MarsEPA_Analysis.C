#include <iostream>
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TH1F.h"
#include "TF1.h"
#include "TSpectrum.h"
#include "TMath.h"

//########################################//
//########################################//
//#####        Original Data      ########//
//########################################//
//########################################//

int AnalysePedestal(Char_t *infile){

  TString histname[14] = {"Si1High","Si1Low",
                          "Si2Low","Si2Medium","Si2High",
                          "CsISiPMT1","CsISiPMT2","CsISiPMT3",
                          "CsISiPD1Low","CsISiPD1High",
                          "CsISiPD2Low","CsISiPD2High",
                          "CsISiPD3Low","CsISiPD3High"};

  TH1F *h[14];
  for(int i=0;i<14;i++){
    h[i] = new TH1F(histname[i].Data(),histname[i].Data(),8000,0,16000);
  }

  /*int Si1High[22],Si1Low[22];
  int Si2Low[22],Si2Medium[22],Si2High[22];
  int CsISiPMT1[22],CsISiPMT2[22],CsISiPMT3[22];
  int CsISiPD1Low[22],CsISiPD1High[22],CsISiPD2Low[22],CsISiPD2High[22],CsISiPD3Low[22],CsISiPD3High[22];*/
  
  TFile *f_in = new TFile(infile);
  TTree *tr_in = (TTree *)f_in->Get("tree_Origin");
  for(int j=0;j<14;j++){
    
    tr_in->Project(h[i],histname[i].Data());
    TSpectrum *s;
    h[i]->Fit("")
  }  
  return 0;
}

int AnalyseCalibration(){
  return 0;
}
