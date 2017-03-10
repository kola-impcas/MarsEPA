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

class MarsEPAMainFrame : public TGMainFrame{

ClassDef(MarsEPAMainFrame,0)

private:
// --- Main Frame ---
  TGMainFrame *fMain;
  TGTab *fTab;
  TGCompositeFrame *fFrame1,*tf1, *tf2, *tf3, *tf4;
  TGLayoutHints *fL1, *fL2, *fL3, *fL4 ,*fL5 ,*fL6;
  TGButton *fOpenButton, *fRunButton;
  TGTextEntry *fTxt1;

// --- sub Tab_Origin ---
  TGListView *fLvView;
  TGFileContainer *fContents;
  TRootEmbeddedCanvas *fTab1Canvas;
  TGTextView *fTab1TxtView;

  void DisplayFile(const TString &fname);
  void DisplayDirectory(const TString &fname);
  void DisplayObject(const TString &fname,const TString &name);

// --- sub Tab Calibration ---
  /*TGListView *fTab2LvView;
  TGFileContainer *fTab2Contents;
  TRootEmbeddedCanvas *fTab2Can;
  TGTextView *fTab2TxtView;*/

// --- sub Tab Compressed ---
  /*TGListView *fTab3LvView;
  TGFileContainer *fTab3Contents;
  TRootEmbeddedCanvas *fTab3Can1;
  TRootEmbeddedCanvas *fTab3Can2;
  TRootEmbeddedCanvas *fTab3Can3;
  TRootEmbeddedCanvas *fTab3Can4;
  TGTextView *fTab3TxtView;*/

// --- sub Tab Normal ---
  /*TGListView *fTab4LvView;
  TGFileContainer *fTab4Contents;
  TRootEmbeddedCanvas *fTab4Can1;
  TRootEmbeddedCanvas *fTab4Can2;
  TRootEmbeddedCanvas *fTab4Can3;
  TRootEmbeddedCanvas *fTab4Can4;
  TGTextView *fTab4TxtView;*/

public:
  MarsEPAMainFrame(const TGWindow *p, UInt_t w, UInt_t h);
  virtual ~MarsEPAMainFrame();
  void CloseWindow();
  void DoOpen();
  void DoRun();  
  void DoTab(Int_t);

 // --- sub Tab Origin ---
  void OnDoubleClick(TGLVEntry*, Int_t);
};

MarsEPAMainFrame::MarsEPAMainFrame(const TGWindow *p, UInt_t w, UInt_t h):TGMainFrame(p, w, h){
 
 // Create MarsEPAMainFrame. 
  fMain = new TGMainFrame(p, w, h);

 // use hierarchical cleaning.
  fMain->SetCleanup(kDeepCleanup);
 
  fMain->Connect("CloseWindow()","MarsEPAMainFrame",this,"CloseWindow()");

  // use Horizontal Frame to setup Open and Run Button
  fFrame1 = new TGHorizontalFrame(fMain, 550, 20, kFixedWidth);
  
  fOpenButton = new TGTextButton(fFrame1, "&Open...", 1);
  fOpenButton->Connect("Clicked()", "MarsEPAMainFrame", this, "DoOpen()");
  fRunButton = new TGTextButton(fFrame1, "&Run...", 2);
  fRunButton->Connect("Clicked()", "MarsEPAMainFrame", this, "DoRun()");
  fTxt1 = new TGTextEntry(fFrame1, new TGTextBuffer(150));

  fL1 = new TGLayoutHints(kLHintsTop | kLHintsExpandX | kLHintsLeft,
                          5, 5, 5, 5);
  fL2 = new TGLayoutHints(kLHintsBottom | kLHintsRight,
                          100, 5, 5, 5);

  fFrame1->AddFrame(fOpenButton, fL1);
  fFrame1->AddFrame(fTxt1, fL1);
  fFrame1->AddFrame(fRunButton, fL1);

  fFrame1->Resize(550, fOpenButton->GetDefaultHeight());
  fMain->AddFrame(fFrame1, fL2);

  // ------ create Tab widget and some composite frame
  fTab = new TGTab(fMain, 500, 500);
  fTab->Connect("Selected(Int_t)","MarsEPAMainFrame",this,"DoTab(Int_t)");

  fL3 = new TGLayoutHints(kLHintsTop | kLHintsLeft, 5, 5, 5, 5);

  //------------------------
  // --- sub_tab1 Origin ---
  //------------------------
  TGCompositeFrame *tf = fTab->AddTab("Origin");
  
  TGHorizontalFrame *fTab1_SubHF = new TGHorizontalFrame(tf, 450, 450);
//  TGCompositeFrame *fTab1_SubHF = new TGHorizontalFrame(tf, 450, 450, kHorizontalFrame);
  TGVerticalFrame *fTab1_V1 = new TGVerticalFrame(fTab1_SubHF, 200, 400, kFixedWidth);
  TGVerticalFrame *fTab1_V2 = new TGVerticalFrame(fTab1_SubHF, 200, 400);
  TGHorizontalFrame *fTab1_V2_Top = new TGHorizontalFrame(fTab1_V2, 190, 190, kFixedHeight);
  TGHorizontalFrame *fTab1_V2_Bottom = new TGHorizontalFrame(fTab1_V2, 190, 190);

  //--- Tab1_V1 ---
  fL4 = new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5, 5, 5, 5);  

  fLvView = new TGListView(fTab1_V1, 180, 380, kSunkenFrame);
  fTab1_V1->AddFrame(fLvView, fL4);
  Pixel_t white;
  gClient->GetColorByName("white",white);
  fContents = new TGFileContainer(fLvView, kSunkenFrame, white);
  fContents->Connect("DoubleClicked(TGFrame*, Int_t)", "MarsEPAMainFrame", this, "OnDoubleClick(TGLVEntry*, Int_t)");  
  fContents->SetDefaultHeaders();
  fContents->DisplayDirectory(); 
  fContents->AddFile(".."); 
  fContents->Resize();
 
  //--- Tab1_V2 ---
  fL5 = new TGLayoutHints(kLHintsTop | kLHintsExpandX);
  fL6 = new TGLayoutHints(kLHintsBottom | kLHintsExpandX | kLHintsExpandY);
  
  fTab1Canvas = new TRootEmbeddedCanvas("Tab1Canvas", fTab1_V2_Top, 185, 185);
  fTab1_V2_Top->AddFrame(fTab1Canvas, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5, 5, 5, 5));
  fTab1_V2->AddFrame(fTab1_V2_Top, fL5);
//-----------------------------------------------------
  TGHSplitter *hsp_center = new TGHSplitter(fTab1_V2, 4, 4);
  hsp_center->SetFrame(fTab1_V2_Top, kTRUE);
  fTab1_V2->AddFrame(hsp_center, fL5);
//-----------------------------------------------------
  fTab1TxtView = new TGTextView(fTab1_V2_Bottom, 185, 185);
  fTab1_V2_Bottom->AddFrame(fTab1TxtView, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5, 5, 5, 5));
  fTab1_V2->AddFrame(fTab1_V2_Bottom, fL6);
//------------------------------------------------------
 
  fTab1_V1->Resize(fTab1_V1->GetDefaultWidth()+50, fTab1_V1->GetDefaultHeight());
  fTab1_V2->Resize(fTab1_V1->GetDefaultWidth()+150, fTab1_V1->GetDefaultHeight());



  fTab1_SubHF->AddFrame(fTab1_V1, new TGLayoutHints(kLHintsLeft | kLHintsExpandY));
  TGVSplitter *vsp_center = new TGVSplitter(fTab1_SubHF, 4, 4);
  vsp_center->SetFrame(fTab1_V1, kTRUE);
  fTab1_SubHF->AddFrame(vsp_center, new TGLayoutHints(kLHintsLeft | kLHintsExpandY));
  fTab1_SubHF->AddFrame(fTab1_V2, new TGLayoutHints(kLHintsRight | kLHintsExpandX | kLHintsExpandY));
  tf->AddFrame(fTab1_SubHF, fL3);
  fTab1_SubHF->Resize(fTab1_SubHF->GetDefaultSize());
  
//------------------------------------
//------------sub tab2 calibration----
//------------------------------------
  tf = fTab->AddTab("Calibration");
  
  TGHorizontalFrame *fTab2_SubHF = new TGHorizontalFrame(tf, 450, 450);
  tf->AddFrame(fTab2_SubHF, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5, 5, 5, 5));


//------------------------------------
//------------sub tab3 compressed-----
//------------------------------------

  tf = fTab->AddTab("Compressed");

  TGHorizontalFrame *fTab3_SubHF = new TGHorizontalFrame(tf, 450, 450);
  tf->AddFrame(fTab3_SubHF, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5, 5, 5, 5));


//------------------------------------
//------------sub tab4 Normal---------
//------------------------------------
  tf = fTab->AddTab("Normal");
 
  TGHorizontalFrame *fTab4_SubHF = new TGHorizontalFrame(tf, 450, 450);
  tf->AddFrame(fTab4_SubHF, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5, 5, 5, 5));


// --- finally addFrame ---
  fMain->AddFrame(fTab, new TGLayoutHints(kLHintsBottom | kLHintsExpandX | kLHintsExpandY, 5, 5, 5, 5));

  fMain->SetWindowName("MarsEPA Data Analysis");
  fMain->MapSubwindows();
  fMain->Resize(fMain->GetDefaultSize());
  fMain->MapWindow(); 
}

MarsEPAMainFrame::~MarsEPAMainFrame(){

  fMain->Cleanup();
  delete fMain;

}

void MarsEPAMainFrame::CloseWindow(){
  
  delete this;

}

void MarsEPAMainFrame::DoOpen(){
}

void MarsEPAMainFrame::DoRun(){
}

void MarsEPAMainFrame::DoTab(Int_t tb_id){

  printf("Tab item %d activated", tb_id);  

}

void MarsEPAMainFrame::DisplayFile(const TString &fname){
//Display content of ROOT file and this code is used from "Root Tutorial"

  TFile file(fname);
  fContents->RemoveAll();
  fContents->AddFile(gSystem->WorkingDirectory());
  fContents->SetPagePosition(0,0);
  fContents->SetColHeaders("Name","Title");

  TIter next(file.GetListOfKeys());
  TKey *key;

  while((key=(TKey*)next())){
      TString cname = key->GetClassName();
      TString name = key->GetName();
      TGLVEntry *entry = new TGLVEntry(fContents,name,cname);
      entry->SetSubnames(key->GetTitle());
      fContents->AddItem(entry);

      entry->SetUserData((void*)StrDup(fname));
  }
}

void MarsEPAMainFrame::DisplayDirectory(const TString &fname){
  
  // Display content of directory and this code is from "Root Tutorial"

  fContents->SetDefaultHeaders();
  gSystem->ChangeDirectory(fname);
  fContents->ChangeDirectory(fname);
  fContents->DisplayDirectory();
  fContents->AddFile("..");
  
}

void MarsEPAMainFrame::DisplayObject(const TString &fname,const TString &name){

  // Browse object located in file
  
  TDirectory *sav = gDirectory;
  
  static TFile *file = 0;
  if(file) delete file; // close
  file = new TFile(fname);

  TObject* obj = file->Get(name);
  if(obj) {
      if(!obj->IsFolder()){
          obj->Browse(0);
      } else obj->Print();
  }
  gDirectory = sav;
}

void MarsEPAMainFrame::OnDoubleClick(TGLVEntry* f,Int_t btn){

  if(btn != kButton1) return;

  //set kWatch cursor
  ULong_t cur = gVirtualX->CreateCursor(kWatch);
  gVirtualX->SetCursor(fContents->GetId(), cur);

  TString name(f->GetTitle());
  const char* fname = (const char*)f->GetUserData();

  if(fname){
    DisplayObject(fname,name);
  } else if(name.EndsWith(".root")){
    DisplayFile(name);
  } else{
    DisplayDirectory(name);
  }
  //set kPointer cursor
  cur = gVirtualX->CreateCursor(kPointer);
  gVirtualX->SetCursor(fContents->GetId(), cur);

}

void MarsEPAGUI(){

  new MarsEPAMainFrame(gClient->GetRoot(), 600, 600);

}
