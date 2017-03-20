#include <iostream>
#include <fstream>
#include "stdio.h"
#include "stdlib.h"

#include "TROOT.h"
#include "TStyle.h"
#include "TString.h"
#include "TFile.h"
#include "TH1F.h"
#include "TKey.h"
#include "TCanvas.h"

#include "TApplication.h"
#include "TGClient.h"
#include "TGFrame.h"
#include "TRootEmbeddedCanvas.h"
#include "TGTextView.h"
#include "TGLayout.h"
#include "TGTab.h"
#include "TGTable.h"
#include "TGButton.h"
#include "TGTextEntry.h"
#include "TGColorSelect.h"
#include "TGNumberEntry.h"
#include "TGFSContainer.h"
#include "TGSplitter.h"
#include "TGListTree.h"
#include "TGFileDialog.h"
#include "TGComboBox.h"
//#include "TGFileInfo.h"

#include "TClass.h"
#include "TVirtualX.h"
#include "TSystem.h"
#include "TSystemDirectory.h"

#include "MarsEPA_Decoding.C"

const TString chname[14] = {"Si1High","Si1Low",
                        "Si2Low","Si2Medium","Si2High",
                        "CsISiPMT1","CsISiPMT2","CsISiPMT3",
                        "CsISiPD1Low","CsISiPD1High",
                        "CsISiPD2Low","CsISiPD2High",
                        "CsISiPD3Low","CsISiPD3High"};
 
class MarsEPAMinorFrame : public TGMainFrame{

ClassDef(MarsEPAMinorFrame,0)

private:
  TGMainFrame *fMain;
  TGHorizontalFrame *fUpFrame;
  TGHorizontalFrame *fCenterFrame;
  TGVerticalFrame *fDownFrame;
  TGHorizontalFrame *fDownFrame_Sub1;
  TGHorizontalFrame *fDownFrame_Sub2;

  TRootEmbeddedCanvas *fCanvas;

  TGComboBox *fCombo;
  TGButton *fOpenButton,*fDecodeButton,*fDrawButton;
  TGTextEntry *fTxt;
  TGTextView *fTextView;

  TGLayoutHints *fL1;
  TGLayoutHints *fL2;
  TGLayoutHints *fLDFSub1;
  TGLayoutHints *fLDFSub2;
  TGLayoutHints *fLCombo;

  TGFileInfo *fInput;
  TString infilename;

 
public:
  MarsEPAMinorFrame(const TGWindow *p,UInt_t w,UInt_t h);
  virtual ~MarsEPAMinorFrame();
  void CloseWindow();
  void DoOpen();
  void DoDecode();
  void DoDraw(Int_t id);
  void ShowText(TGText *text);
  void ShowText(const char *text);
  void ClearTextView();

};

MarsEPAMinorFrame::MarsEPAMinorFrame(const TGWindow *p,UInt_t w,UInt_t h):TGMainFrame(p, w, h){

  fMain = new TGMainFrame(p,w,h);

  fMain->SetCleanup(kDeepCleanup);

  fMain->Connect("CloseWindow()","MarsEPAMinorFrame",this,"CloseWindow()");

////////////////// Up Frame for Command Button ////////////////////
  fUpFrame = new TGHorizontalFrame(fMain, 550, 20, kFixedHeight);
  fOpenButton = new TGTextButton(fUpFrame,"&Open...",1);
  fOpenButton->Connect("Clicked()","MarsEPAMinorFrame",this,"DoOpen()");
  fDecodeButton = new TGTextButton(fUpFrame,"Decoding...",2);
  fDecodeButton->Connect("Clicked()","MarsEPAMinorFrame",this,"DoDecode()");
  fTxt = new TGTextEntry(fUpFrame, new TGTextBuffer(150));

  fL1 = new TGLayoutHints(kLHintsTop | kLHintsExpandX | kLHintsLeft, 5, 5, 5, 5);

  fUpFrame->AddFrame(fOpenButton, fL1);
  fUpFrame->AddFrame(fTxt, fL1);
  fUpFrame->AddFrame(fDecodeButton, fL1);

  fUpFrame->Resize(fUpFrame->GetDefaultWidth()+30, fUpFrame->GetDefaultHeight()+20);

///////////////// Center Frame for Checking Channels of MarsEPA /////////////////
  fCenterFrame = new TGHorizontalFrame(fMain, 550, 20, kFixedHeight);

  char tmp[30];
  fCombo = new TGComboBox(fCenterFrame,100);
  for(int i=0;i<14;i++){
    sprintf(tmp,"%s",chname[i].Data());
    fCombo->AddEntry(tmp,i+1);
  }
  fCombo->Resize(200,20);
  fCombo->Select(1);
  
  fDrawButton = new TGTextButton(fCenterFrame, "Draw...",3);
  fDrawButton->Connect("Clicked()","MarsEPAMinorFrame",this,"DoDraw(Int_t)");

  fLCombo = new TGLayoutHints(kLHintsTop | kLHintsExpandX | kLHintsLeft, 5, 5, 5, 5);

  fCenterFrame->AddFrame(fCombo, fLCombo);
  fCenterFrame->AddFrame(fDrawButton, fLCombo);
  fCenterFrame->Resize(fCenterFrame->GetDefaultWidth()+30,fCenterFrame->GetDefaultHeight()+20);
   
///////////////// Down Frame for TxtView displaying Decode Information //////////

  fDownFrame = new TGVerticalFrame(fMain, 550, 650, kFixedWidth);
  fDownFrame_Sub1 = new TGHorizontalFrame(fDownFrame, 550, 400, kFixedHeight);
  fDownFrame_Sub2 = new TGHorizontalFrame(fDownFrame, 550, 250, kFixedHeight);
//------------------ Canvas ------------------//
  fCanvas = new TRootEmbeddedCanvas("Canvas", fDownFrame_Sub1, 550, 400);
  fLDFSub1 = new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5, 5, 5, 5);
  fDownFrame_Sub1->AddFrame(fCanvas, fLDFSub1);
//--------------------------------------------//
//------------------ Splitter ------------------//
  TGHSplitter *hsp_center = new TGHSplitter(fDownFrame, 4, 4);
  hsp_center->SetFrame(fDownFrame_Sub1, kTRUE);
//----------------------------------------------//
//------------------ TextView ------------------//
  fTextView = new TGTextView(fDownFrame_Sub2, 550, 250);
  fLDFSub2 = new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5, 5, 5, 5);
  fDownFrame_Sub2->AddFrame(fTextView, fLDFSub2);
//----------------------------------------------//
  fL2 = new TGLayoutHints(kLHintsTop | kLHintsExpandX, 5, 5, 5, 5);
  
  fDownFrame->AddFrame(fDownFrame_Sub1, fL2);
  fDownFrame->AddFrame(hsp_center, new TGLayoutHints(kLHintsTop | kLHintsExpandX));
  fDownFrame->AddFrame(fDownFrame_Sub2, new TGLayoutHints(kLHintsBottom | kLHintsExpandX | kLHintsExpandY));
  fDownFrame->Resize(fDownFrame->GetDefaultWidth(), fDownFrame->GetDefaultHeight());

///////////////// Main Frame LayoutHints //////////////////
  fMain->AddFrame(fUpFrame, new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 10 ,10, 10, 1));
  fMain->AddFrame(fCenterFrame, new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 10, 10, 1, 5));
  fMain->AddFrame(fDownFrame, new TGLayoutHints(kLHintsBottom | kLHintsLeft | kLHintsExpandX, 10, 10, 1 ,10));
  fMain->MapSubwindows();
  fMain->Resize(fMain->GetDefaultSize());
  fMain->SetWindowName("MarsEPA Origin Data Decoding");
  fMain->MapWindow();

///////////////////////////////////////////////////////////
  fInput = new TGFileInfo();
}

MarsEPAMinorFrame::~MarsEPAMinorFrame(){

  fMain->Cleanup();
  delete fMain;
  delete fInput;
}


void MarsEPAMinorFrame::CloseWindow(){
  
 /* fMain->Cleanup();
  delete fMain;*/
  delete this;

}

void MarsEPAMinorFrame::DoOpen(){

  new TGFileDialog(gClient->GetRoot(),this,kFDOpen,fInput);
  infilename = fInput->fFilename; 
  fTxt->Clear();
  fTxt->SetText(infilename.Data());
  ShowText("Input File:");
  ShowText(infilename.Data());
 
}

void MarsEPAMinorFrame::DoDecode(){
/*
/////////// inputDir and inputName ///////////
  TString inputDirName,inputBaseName;
  inputBaseName = gSystem->BaseName(infilename.Data());
  inputDirName = infilename;
  inputDirName.ReplaceAll(inputDirName,""); 

/////////// outputDir and outputName ///////////
  TString outputDirName,outputBaseName;
  outBaseName*/
//  TGText *txt=0;
  int eventnum=0;
  char inputFname[200];
  sprintf(inputFname,"%s",infilename.Data());
  ShowText("Begin Decoding: ");
  DecodeOriginPacket(inputFname,eventnum);
//  ShowText(txt);
  ShowText(Form("%d events have been processed!\n",eventnum*22));
  ShowText(Form("%d packets have been processed!\n",eventnum));
  ShowText("Decoding End.");
  ShowText("Decode Successfully");
}

void MarsEPAMinorFrame::DoDraw(Int_t id){
  
  char inputFname[200];
  sprintf(inputFname,"%s",infilename.Data());
  TFile *f = new TFile(inputFname);
  TTree *trin = (TTree *)f->Get("tree_Origin");
  TH1F *htmp = 0; 
  id = fCombo->GetSelected();
  ShowText(Form("%s Signal",chname[id].Data()));
  htmp = new TH1F("htmp",Form("%s",chname[id-1].Data()),8000,0,16000);
  //trin->Project("htmp",chname[id].Data());
  
  TCanvas *can = fCanvas->GetCanvas();
  can->cd();
  trin->Draw(Form("%s >> htmp",chname[id-1].Data()));
  can->Update();
  
}

void MarsEPAMinorFrame::ShowText(TGText *text){
  
  ClearTextView();
  fTextView->AddText(text);
  fTextView->Update();
  fTextView->ShowTop();
}

void MarsEPAMinorFrame::ShowText(const char *text){
  
  ClearTextView();
  fTextView->AddLineFast(text);
  fTextView->Update();
  fTextView->ShowTop();
}

void MarsEPAMinorFrame::ClearTextView(){

  if(fTextView->ReturnLineCount()>=70){
    fTextView->GetText()->DelLine(1);    
  }

}

void MarsEPAMinorGUI(){
  
  new MarsEPAMinorFrame(gClient->GetRoot(),600,800);

}
