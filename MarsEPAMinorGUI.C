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

#include "TClass.h"
#include "TVirtualX.h"
#include "TSystem.h"
#include "TSystemDirectory.h"

class MarsEPAMinorFrame : public TGMainFrame{

ClassDef(MarsEPAMinorFrame,0)

private:
  TGMainFrame *fMain;
  TGHorizontalFrame *fUpFrame,fDownFrame;
  TGButton *fOpenButton,*fDecodeButton;
  TGTextEntry *fTxt;
  TGTextView *fTextView;

  TGLayoutHints *fL1;
  TGLayoutHints *fL2;

public:
  MarsEPAMinorFrame(TGWindow *p,UInt_t w,UInt_t h);
  virtual ~MarsEPAMinorFrame();
  void CloseWindow();
  void DoOpen();
  void DoDecode();
}

MarsEPAMinorFrame::MarsEPAMinorFrame(const TGWindow *p,UInt_t w,UInt_t h):TGMainFrame(p, w. h){

  fMain = new TGMainFrame(p,w,h);

  fMain->SetCleanup(kDeepCleanup);

  fMain->Connect("CloseWindow()","MarsEPAMinorFrame",this,"CloseWindow()");

////////////////// Up Frame for Command Button ////////////////////
  fUpFrame = new TGHorizontalFrame(fMain, 550, 20, kFixedHeight);
  fOpenButton = new TGTextButton(fUpFrame,"&Open...",1);
  fOpenButton->Connect("Clicked()","MarsEPAMinorFrame",this,"DoOpen()");
  fDecodeButton = new TGTextButton(fUpFrame,"Decoding...",2);
  fDecodeButton->Connect("Clicked()","MarsEPAMinorFrame",this,"DoDecode()");
  fTxt = new TGTextEntry(fUpFrame, new TGTextBuffer(200));

  fL1 = new TGLayoutHints(kLHintsTop | kLHintsExpandX | kLHintsLeft, 5, 5, 5, 5);
  fL2 = new TGLayoutHints(kLHintsBottom | kLHintsRight, 100, 5, 5, 5);

  fUpFrame->AddFrame(fOpenButton, fL1);
  fUpFrame->AddFrame(fTxt, fL1);
  fUpFrame->AddFrame(fDecodeButton, fL1);

  fUpFrame->Resize(fUpFrame->GetDefaultWidth(), fUpFrame->GetDefaultHeight());
///////////////// Down Frame for TxtView displaying Decode Information //////////

  fDownFrame = new TGHorizontalFrame(fMain, 550, 450, kFixedHeight);
  fTextView = new TGTextView(fDownFrame, 500, 400);
  
  fL2 = new TGLayoutHints(kLHintsBottom | kLHintsRight, 100, 5, 5, 5);
  
  fDownFrame->AddFrame(fTxt, fL2);
  fDownFrame->Resize(fDownFrame->GetDefaultWidth(), fDownFrame->GetDefaultHeight());

///////////////// Main Frame LayoutHints //////////////////
  fMain->AddFrame(fUpFrame, new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX), 10 ,10, 10, 10);
  fMain->AddFrame(fDownFrame, new TGLayoutHints(kLHintsBottom | kLHintsLeft | kLHintsExpandX), 10, 10, 10 ,10);
  fMain->MapSubwindows();
  fMain->Resize(fMain->GetDefaultSize());
  fMain->SetWindowsName("MarsEPA Origin Data Decoding");
  fMain->MapWindow();
}

MarsEPAMinorFrame::~MarsEPAMinorFrame(){

  fMain->Cleanup();
  delete fMain;

}

void MarsEPAMinorFrame::CloseWindow(){
  
  delete this;

}

void MarsEPAMinorFrame::DoOpen(){
}

void MarsEPAMinorFrame::DoDecode(){
}


