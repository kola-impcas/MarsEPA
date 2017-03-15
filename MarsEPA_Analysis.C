#include "stdio.h"
#include "stdlib.h"
#include <iostream>
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TString.h"
#include "TStyle.h"
#include "TH1F.h"
#include "TF1.h"
#include "TSpectrum.h"
#include "TMath.h"

  //###################################//
  //####     Glob variable       ######//
  //###################################//
    TString trname[14] = {"Si1High","Si1Low",
                          "Si2Low","Si2Medium","Si2High",
                          "CsISiPMT1","CsISiPMT2","CsISiPMT3",
                          "CsISiPD1Low","CsISiPD1High",
                          "CsISiPD2Low","CsISiPD2High",
                          "CsISiPD3Low","CsISiPD3High"};
    const Int_t Maximum = 30;
//########################################//
//########################################//
//#####        Original Data      ########//
//########################################//
//########################################//

int AnalysePedestal(Char_t *infile,Char_t *outfile){

  gStyle->SetOptFit(11111);



  
  TFile *f_in = new TFile(infile);
  if(!f_in){
    printf("Error in opening input File!\n");
    return -1;
  }
  TTree *tr_in = (TTree *)f_in->Get("tree_Origin");

  Float_t pedmean,pedsigma;
  Char_t pedname[20];
  TFile *f_out = new TFile(outfile,"recreate");
  if(!f_out){
    printf("Error in Creating output File!\n");
    return -1;
  }
  TTree *tr_out = new TTree("tree_ped","tree_ped");
  tr_out->Branch("pedmean",&pedmean,"pedmean/F");
  tr_out->Branch("pedsigma",&pedsigma,"pedsigma/F");
  tr_out->Branch("pedname",pedname,"pedname/C");

  FILE *fp = fopen("pedresult.csv","w");
  fprintf(fp,"MarsEPA_ChanNo,MarsEPA_ChanName,Mean,Sigma\n");

  Float_t *xpeaks;
  Float_t mean_tmp,sigma_tmp;
  Float_t xmin,xmax;

  TF1 *f1;
  TF1 *ffit;
  TSpectrum *s;
  Int_t nfounds;
  Float_t SpecSigma[14] = {3.0,3.0,3.0,13.0,3.0,15.0,15.0,15.0,15.0,15.0,15.0,15.0,15.0,15.0};

  TCanvas *can = new TCanvas("can","can",800,600);

  TH1F *h[14];
 
  can->Print("PedResult.pdf[");
  for(int i=0;i<14;i++){
    h[i] = new TH1F(Form("Hist_%s",trname[i].Data()),Form("Hist_%s",trname[i].Data()),300,0,900);
    tr_in->Project(Form("Hist_%s",trname[i].Data()),Form("%s",trname[i].Data()));
    s = new TSpectrum(3);
    nfounds = s->Search(h[i],SpecSigma[i],"",0.20);
    if(nfounds != 1){
      printf("error in finding %d peaks\n",nfounds);
      return -1;
    }
    xpeaks = s->GetPositionX();
    xmin = xpeaks[0] - 200;
    xmax = xpeaks[1] + 200;
    f1 = new TF1("f1","gaus",xmin,xmax);
    h[i]->Fit("f1");
    mean_tmp = f1->GetParameter(1);
    sigma_tmp = f1->GetParameter(2);
    //printf("1th %f   %f\n",mean_tmp,sigma_tmp);
  
    xmin = mean_tmp - 3*sigma_tmp;
    xmax = mean_tmp + 3*sigma_tmp;
    ffit = new TF1("ffit","gaus",xmin,xmax);
    h[i]->Fit("ffit");
    mean_tmp = ffit->GetParameter(1);
    sigma_tmp = ffit->GetParameter(2);
    //printf("2th %f   %f\n",mean_tmp,sigma_tmp);
    pedmean = mean_tmp;
    pedsigma = sigma_tmp;
    sprintf(pedname,"%s",trname[i].Data());
    fprintf(fp,"%d,%s,%.3f,%.3f\n",i+1,trname[i].Data(),mean_tmp,sigma_tmp);

    can->cd();
    h[i]->Draw();
    ffit->Draw("lsame");
    can->Print("PedResult.pdf");

    tr_out->Fill();
  }
  can->Print("PedResult.pdf]");

  tr_out->Write();

  fclose(fp);
  delete f_in;
  delete f_out;
   
  return 0;
}

int AnalyseCalibration(Char_t *infile,Int_t nPoints,Float_t startVol,Float_t stopVol){

  Float_t paceVol = (stopVol - startVol)/(nPoints-1);
  //printf("%f",paceVol);
  Float_t calibVol[Maximum]={0};
  for(int calNo=0;calNo<nPoints;calNo++){
    calibVol[calNo] = startVol + calNo*paceVol;
    printf("%f\n",calibVol[calNo]);
  }

  TFile *f_in = new TFile(infile);
  TTree *tr_in = (TTree *)f_in->Get("tree_Origin");

  TF1 *f1;
  TF1 *ffit;
 
  Int_t nfounds;
  TSpectrum *s;
  Float_t *xpeaks;
  Float_t mean_tmp[Maximum],sigma_tmp[Maximum];
  Float_t xmin,xmax;

  TH1F *hist[14];
  TCanvas *can = new TCanvas("can","can",800,600);

  TF1 *fcalib;
  TGraph *gr;
  TCanvas *can_calib = new TCanvas("can_calib","can_calib",800,600);

  can->Print("calibSpectrum.pdf[");
  can_calib->Print("calibPoint.pdf[");
  for(int i=0;i<14;i++){
    hist[i] = new TH1F(Form("Hist_%s",trname[i].Data()),Form("Hist_%s",trname[i].Data()),1000,0,16000);
    tr_in->Project(Form("Hist_%s",trname[i].Data()),Form("%s",trname[i].Data()));
    s = new TSpectrum(Maximum);
    nfounds = s->Search(hist[i],5.5,"",0.15);
    if(nfounds != nPoints+1){
      printf("Error in finding %d peaks",nfounds);
      //return -1;
    }
    xpeaks = s->GetPositionX();
    for(int j=0;j<nPoints;j++){
      xmin = xpeaks[j] - 30;
      xmax = xpeaks[j] + 30;
      f1 = new TF1("f1","gaus",xmin,xmax);
      hist[i]->Fit("f1");
      
      mean_tmp[j] = f1->GetParameter(1);
      sigma_tmp[j] = f1->GetParameter(2);
      xmin = mean_tmp[j] - 3*sigma_tmp[j];
      xmax = mean_tmp[j] + 3*sigma_tmp[j];
      ffit = new TF1("ffit","gaus",xmin,xmax);
      hist[i]->Fit("ffit");
      
      mean_tmp[j] = ffit->GetParameter(1);
      sigma_tmp[j] = ffit->GetParameter(2);
    }
    
    can->cd();
    hist[i]->Draw();
    can->Print("calibSpectrum.pdf");

    fcalib = new TF1("fcalib","gaus",50,1450);
    gr = new TGraph(nPoints,calibVol,mean_tmp);
    gr->SetMarkerColor(kRed);
    gr->SetMarkerStyle(5);
    can_calib->cd();
    gr->Draw();
    gr->Fit("fcalib");
    can_calib->Print("calibPoint.pdf");

    can->Update();
    can_calib->Update();
  }
  can->Print("calibSpectrum.pdf]");
  can_calib->Print("calibPoint.pdf]");

  delete can;
  delete can_calib;

  delete f_in;

  return 0;
}
