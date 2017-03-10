#include "stdio.h"
#include <iostream>
#include <fstream>
#include "stdlib.h"
#include "math.h"
#include "string.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"

/*int CreateTreeFile(char* dest,char* src){
    int len,n;
    len = strlen(src);
    n = len - 4;
    printf("%d\n",n);
    strncpy(dest,src,n);
    printf("%s\n",dest);
    return 0;
}*/

int DecodeCalibPacket(char *infile){
    /////////////////////////////////////////////////////////////
    ////////////////    Create Data Tree     ////////////////////
    /////////////////////////////////////////////////////////////
    int idnum;
    int packetlength;
    int datachannel[17];
    int trigcount;

    TFile *fin = new TFile(Form("%s.root",infile),"recreate");
    //TFile *fin = new TFile("test.root","recreate");
    TTree *trin = new TTree("tr_cali","tr_cali");
    trin->Branch("idnum",&idnum,"idnum/I");
    trin->Branch("packetlength",&packetlength,"packetlength/I");
    trin->Branch("datachannel",datachannel,"datachannel[17]/I");
    trin->Branch("trigcount",&trigcount,"trigcount/I");

    TH1F *hchan[17];
    for(int chid=0;chid<17;chid++){
        hchan[chid] = new TH1F(Form("hchannel_%d",chid+1),Form("hchannel_%d",chid+1),17000,-500,16500);
    }

    /////////////////////////////////////////////////////////////
    ////////////////    Define Data Type     ////////////////////
    /////////////////////////////////////////////////////////////
    unsigned char onedata[60];
    char top[2];
    int eventcounts = 0;

    FILE *fp = fopen(Form("%s.dat",infile),"r");
    if(!fp){
        printf("Cannot open input file in Current Directory");
    }

    ////////////////////////////////////////////////////////////
    ////////////////    Decoding Packet    /////////////////////
    ////////////////////////////////////////////////////////////
    while(!feof(fp)){
        //printf("at the beginning of the file! %d \n",eventcounts);
        if(fread(&top,sizeof(char),2,fp)==NULL){
            printf("Hello! I'm Here");
            break;
        }
        if((top[0]&0xff)==0x55 && (top[1]&0xff)==0xaa){
            //printf("top0 and top1 are respectively %d and %d",(top[0]&0xff),(top[1]&0xff));
            if(fread(&top,sizeof(char),2,fp) == NULL) break;
            if(fread(&top,sizeof(char),2,fp) == NULL) break;
            idnum = (top[1]&0xff);
            printf("id number is %d\n",idnum);
            if(fread(&top,sizeof(char),2,fp) == NULL) break;
            packetlength = ((top[0]&0xff)<<8)+(top[1]&0xff)-2;
            //printf("packetlenght is %d\n",packetlength);
            if(packetlength != 38){
                printf("error in packet length and %d th Event\n",eventcounts);
                return -1;
            }
            //cout << packetlength << endl;
            if(fread(&onedata,sizeof(char),packetlength,fp) == NULL) break;
            for(int packetid=0; packetid<packetlength/2; packetid++){
                if(packetid<17){
                    datachannel[packetid] = ((onedata[2*packetid]&0xff)<<8) + (onedata[2*packetid+1]&0xff);
                    //hchan[packetid]->Fill(datachannel[packetid]);
                }
                else if(packetid == 17){
                    trigcount = ((onedata[2*packetid]&0xff)<<8) + (onedata[2*packetid+1]&0xff);
                }
                else{
                    if((onedata[2*packetid]&0xff) != 0x5a || (onedata[2*packetid+1]&0xff) != 0xa5)
                        printf("error end code ! %d and %d \n",onedata[2*packetid]&0xff,onedata[2*packetid+1]&0xff);
                }
            }

            trin->Fill();   
            for(int ii=0;ii<17;ii++){
                hchan[ii]->Fill(datachannel[ii]);
                datachannel[ii] = 0;
            }

            idnum = 0;
            packetlength = 0;
            trigcount = 0;

            eventcounts++;
            printf("event %d th is being processed!\n",eventcounts);
        }
        else{
            fread(&top,sizeof(char),2,fp);
        }
    }

    for(int j=0;j<17;j++){
        hchan[j]->Write();
    }
    trin->Write();
    fclose(fp);
    fin->Close();
    printf(Form("%d Events have been processed!\n",eventcounts));

    return 0;
}

int DecodeOriginPacket(Char_t *infile){
    ////////////////////////////////////////////////////////////
    ////////////////  Decoding Origin Packet  //////////////////
    ////////////////////////////////////////////////////////////

    int packet_type;
    int packet_length;
    int packet_count;
    int second;
    int millisecond;
    int Si1High[22],Si1Low[22];
    int Si2Low[22],Si2Medium[22],Si2High[22];
    int CsISiPMT1[22],CsISiPMT2[22],CsISiPMT3[22];
    int CsISiPD1Low[22],CsISiPD1High[22],CsISiPD2Low[22],CsISiPD2High[22],CsISiPD3Low[22],CsISiPD3High[22];
    int TotalCounts,LPCounts,HPCounts,HETCounts,FracFreq;

    //TH1F *hSi1High = new TH1F("Si1High","Si1High",);

    ////////////////////////////////////////////////////////////
    ////////////////////    Create Tree  ///////////////////////
    ////////////////////////////////////////////////////////////
    TFile *fin = new TFile(Form("%s.root",infile),"recreate");
    TTree *trin = new TTree("tree_Origin","tree_Origin");
    trin->Branch("packet_type",&packet_type,"packet_type/I");
    trin->Branch("packet_length",&packet_length,"packet_length/I");
    trin->Branch("packet_count",&packet_count,"packet_count/I");
    trin->Branch("second",&second,"second/I");
    trin->Branch("millisecond",&millisecond,"millisecond/I");
    trin->Branch("Si1High",Si1High,"Si1High[22]/I");
    trin->Branch("Si1Low",Si1Low,"Si1Low[22]/I");
    trin->Branch("Si2Low",Si2Low,"Si2Low[22]/I");
    trin->Branch("Si2Medium",Si2Medium,"Si2Medium[22]/I");
    trin->Branch("Si2High",Si2High,"Si2High[22]/I");
    trin->Branch("CsISiPMT1",CsISiPMT1,"CsISiPMT1[22]/I");
    trin->Branch("CsISiPMT2",CsISiPMT2,"CsISiPMT2[22]/I");
    trin->Branch("CsISiPMT3",CsISiPMT3,"CsISiPMT3[22]/I");
    trin->Branch("CsISiPD1Low",CsISiPD1Low,"CsISiPD1Low[22]/I");
    trin->Branch("CsISiPD2Low",CsISiPD2Low,"CsISiPD2Low[22]/I");
    trin->Branch("CsISiPD3Low",CsISiPD3Low,"CsISiPD3Low[22]/I");
    /*trin->Branch("CsISiPD1Medium",CsISiPD1Medium,"CsISiPD1Medium[22]/I");
    trin->Branch("CsISiPD2Medium",CsISiPD2Medium,"CsISiPD2Medium[22]/I");
    trin->Branch("CsISiPD3Medium",CsISiPD3Medium,"CsISiPD3Medium[22]/I");*/
    trin->Branch("CsISiPD1High",CsISiPD1High,"CsISiPD1High[22]/I");
    trin->Branch("CsISiPD2High",CsISiPD2High,"CsISiPD2High[22]/I");
    trin->Branch("CsISiPD3High",CsISiPD3High,"CsISiPD3High[22]/I");
    trin->Branch("TotalCounts",&TotalCounts,"TotalCounts/I");
    trin->Branch("LPCounts",&LPCounts,"LPCounts/I");
    trin->Branch("HPCounts",&HPCounts,"HPCounts/I");
    trin->Branch("HETCounts",&HETCounts,"HETCounts/I");
    trin->Branch("FracFreq",&FracFreq,"FracFreq/I");

    FILE *fp = fopen(Form("%s",infile),"r");
    if(!fp){
        printf("error in openning input file ... \n");
        return -1;
    }

    //////////////////////////////////////////////////////////////
    ///////////////////  Decoding Packet  ////////////////////////
    //////////////////////////////////////////////////////////////
    unsigned char onedata[700];
    char top[2];
    char sigle;
    int eventnum = 0;
    //int bytesnum = 0;
    while(!feof(fp)){
        if(fread(&top,sizeof(char),2,fp)==NULL) break;

        if((top[0]&0xff)==0xeb && (top[1]&0xff)==0x90){
            //bytesnum++;
            //printf("I have found %d eb90\n",num);
            if(fread(&sigle,sizeof(char),1,fp) == NULL) break;
            if(fread(&top,sizeof(char),2,fp) == NULL) break;
            packet_type = (top[0]&0xc0)>>6;
            if(packet_type != 0) break;
            packet_length = ((top[0]&0x3f)<<8) + (top[1]&0xff);
            if(packet_length != 639){
                printf("error in packet length %d and %dth Events\n",packet_length,eventnum);
            }
            if(fread(&top,sizeof(char),2,fp)==NULL) break;
            packet_count = ((top[0]&0xff)<<8) + (top[1]&0xff);
            if(fread(&onedata,sizeof(char),packet_length+5,fp)==NULL) break;
            second = ((onedata[0]&0xff)<<24) + ((onedata[1]&0xff)<<16) + ((onedata[2]&0xff)<<8) + (onedata[3]&0xff);
            millisecond = ((onedata[4]&0xff)<<8) + (onedata[5]&0xff);
            for(int i=0;i<22;i++){
                Si1High[i] = ((onedata[i*28+6]&0xff)<<8) + (onedata[i*28+7]&0xff);
                Si1Low[i] = ((onedata[i*28+8]&0xff)<<8) + (onedata[i*28+9]&0xff);
                Si2Low[i] = ((onedata[i*28+10]&0xff)<<8) + (onedata[i*28+11]&0xff);
                Si2Medium[i] = ((onedata[i*28+12]&0xff)<<8) + (onedata[i*28+13]&0xff);
                Si2High[i] = ((onedata[i*28+14]&0xff)<<8) + (onedata[i*28+15]&0xff);
                CsISiPMT1[i] = ((onedata[i*28+16]&0xff)<<8) + (onedata[i*28+17]&0xff);
                CsISiPD1Low[i] = ((onedata[i*28+18]&0xff)<<8) + (onedata[i*28+19]&0xff);
                CsISiPD1High[i] = ((onedata[i*28+20]&0xff)<<8) + (onedata[i*28+21]&0xff);
                CsISiPMT2[i] = ((onedata[i*28+22]&0xff)<<8) + (onedata[i*28+23]&0xff);
                CsISiPD2Low[i] = ((onedata[i*28+24]&0xff)<<8) + (onedata[i*28+25]&0xff);
                CsISiPD2High[i] = ((onedata[i*28+26]&0xff)<<8) + (onedata[i*28+27]&0xff);
                CsISiPMT3[i] = ((onedata[i*28+28]&0xff)<<8) + (onedata[i*28+29]&0xff);
                CsISiPD3Low[i] = ((onedata[i*28+30]&0xff)<<8) + (onedata[i*28+31]&0xff);
                CsISiPD3High[i] = ((onedata[i*28+32]&0xff)<<8) + (onedata[i*28+33]&0xff);
            }
            TotalCounts = ((onedata[631]&0xff)<<8) + (onedata[632]&0xff);
            LPCounts = ((onedata[633]&0xff)<<8) + (onedata[634]&0xff);
            HPCounts = ((onedata[635]&0xff)<<8) + (onedata[636]&0xff);
            HETCounts = ((onedata[637]&0xff)<<8) + (onedata[638]&0xff);
            FracFreq = (onedata[639]&0xff);

            trin->Fill();

            packet_type = 0;
            packet_length = 0;
            packet_count = 0;
            for(int j=0;j<22;j++){
                Si1High[j] = 0; Si1Low[j] = 0;
                Si2Low[j] = 0; Si2Medium[j] = 0; Si2High[j] = 0;
                CsISiPMT1[j] = 0; CsISiPD1Low[j] = 0; CsISiPD1High[j] = 0;
                CsISiPMT2[j] = 0; CsISiPD2Low[j] = 0; CsISiPD2High[j] = 0;
                CsISiPMT3[j] = 0; CsISiPD3Low[j] = 0; CsISiPD3High[j] = 0;
            }
            TotalCounts = 0;
            LPCounts = 0;
            HPCounts = 0;
            HETCounts = 0;
            FracFreq = 0;

            eventnum++;
            if((eventnum*22)%110 == 0){
                printf("Event %d th is being processed!\n",eventnum*22);
            }
            if(eventnum < 5)
              printf("The last 5 bytes in %dth packet are respectively\t%x\t%x\t%x\t%x\t%x\n",eventnum, onedata[637], onedata[638], onedata[639], onedata[640], onedata[641]);
        }
        else{
            //fread(&top,sizeof(char),2,fp);
            continue;
        }
    }
    trin->Write();
    fclose(fp);
    fin->Close();
    printf("Totally %d Events have been processed!\n",eventnum*22);
    printf("Totally %d Packets have been processed!\n",eventnum);

    return 0;
}

int DecodeCompressedPacket(Char_t *infile){
    ////////////////////////////////////////////////////////////
    ///////////////  Decoding Compressed Packet  ///////////////
    ////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////
    /////////// electron\proton\alpha energy spectrum //////////
    ////////////////////////////////////////////////////////////

    TH1F *helectron = new TH1F("helectron","helectron",12,0,12);
    TH1F *hproton = new TH1F("hproton","hproton",30,0,30);
    TH1F *halpha = new TH1F("halpha","halpha",30,0,30);
   // Int_t elecbin,protonbin,alphabin;

    const Int_t Maxnum = 84;

    typedef struct{
        int packet_type;
        int packet_length;
        int packet_count;
        int second;
        int millisecond;
        int Elec[18];
        int Proton[18];
        int Alpha[18];
        int ionevt;
        int Si1Gain[Maxnum],Si1[Maxnum];
        int Si2Gain[Maxnum],Si2[Maxnum];
        int CsIGain[Maxnum],CsI[Maxnum];
        int TotalCounts,LPCounts,HPCounts,HETCounts,FracFreq;
    }CompressedPacket_t;

    ////////////////////////////////////////////////////////////
    ////////////////////    Create Tree  ///////////////////////
    ////////////////////////////////////////////////////////////
    TFile *fin = new TFile(Form("%s.root",infile),"recreate");
    TTree *trin = new TTree("tree_Compressed","tree_Compressed");

    CompressedPacket_t compac;
    trin->Branch("packet_type",&compac.packet_type,"packet_type/I");
    trin->Branch("packet_length",&compac.packet_length,"packet_length/I");
    trin->Branch("packet_count",&compac.packet_count,"normpac.packet_count/I");
    trin->Branch("second",&compac.second,"second/I");
    trin->Branch("millisecond",&compac.millisecond,"millisecond/I");
    trin->Branch("Elec",compac.Elec,"Elec[18]/I");
    trin->Branch("Proton",compac.Proton,"Proton[18]/I");
    trin->Branch("Alpha",compac.Alpha,"Alpha[18]/I");
    trin->Branch("ionevt",&compac.ionevt,"ionevt/I");
    trin->Branch("Si1Gain",compac.Si1Gain,"Si1Gain[ionevt]/I");
    trin->Branch("Si1",compac.Si1,"Si1[ionevt]/I");
    trin->Branch("Si2Gain",compac.Si2Gain,"Si2Gain[ionevt]/I");
    trin->Branch("Si2",compac.Si2,"Si2[ionevt]/I");
    trin->Branch("CsIGain",compac.CsIGain,"CsIGain[ionevt]/I");
    trin->Branch("CsI",compac.CsI,"CsI[ionevt]/I");
    trin->Branch("TotalCounts",&compac.TotalCounts,"TotalCounts/I");
    trin->Branch("LPCounts",&compac.LPCounts,"LPCounts/I");
    trin->Branch("HPCounts",&compac.HPCounts,"HPCounts/I");
    trin->Branch("HETCounts",&compac.HETCounts,"HETCounts/I");
    trin->Branch("FracFreq",&compac.FracFreq,"FracFreq/I");

    FILE *fp = fopen(Form("%s.dat",infile),"r");
    if(!fp){
        printf("Error in openning input file ...\n");
    }

    //////////////////////////////////////////////////////////////
    ///////////////////  Decoding Packet  ////////////////////////
    //////////////////////////////////////////////////////////////
    unsigned char onedata[700];
    unsigned char tmp;
    char top[2];
    char sigle;
    int eventnum = 0;
    while(!feof(fp)){
        if(fread(&top,sizeof(char),2,fp)==NULL){
            printf("This file is empty!\n");
        }
        if((top[0]&0xff)==0xeb && (top[1]&0xff)==0x90){
            if(fread(&sigle,sizeof(char),1,fp) == NULL) break;
            if(fread(&top,sizeof(char),2,fp) == NULL) break;
            compac.packet_type = (top[0]&0xc0)>>6;
            compac.packet_length = ((top[0]&0x3f)<<8) + (top[1]&0xff);
            if(compac.packet_length != 640){
                printf("error in packet length and %dth Events\n",eventnum);
            }
            if(fread(&top,sizeof(char),2,fp)==NULL) break;
            compac.packet_count = ((top[0]&0xff)<<8) + (top[1]&0xff);
            if(fread(&onedata,sizeof(char),compac.packet_length+2,fp)==NULL) break;
            compac.second = ((onedata[0]&0xff)<<24) + ((onedata[1]&0xff)<<16) + ((onedata[2]&0xff)<<8) + (onedata[3]&0xff);
            compac.millisecond = ((onedata[4]&0xff)<<8) + (onedata[5]&0xff);
            for(int pid=0;pid<3;pid++){
                for(int chanNo=0;chanNo<18;chanNo++){
                    switch(pid){
                    case 0:{
                        compac.Elec[chanNo] = ((onedata[pid*chanNo+2*chanNo+6]&0xff)<<8) + (onedata[pid*chanNo+2*chanNo+7]&0xff);
                        helectron->Fill(compac.Elec[chanNo]);
                        break;
                    }
                    case 1:{
                        compac.Proton[chanNo] = ((onedata[pid*chanNo+2*chanNo+6]&0xff)<<8) + (onedata[pid*chanNo+2*chanNo+7]&0xff);
                        hproton->Fill(compac.Proton[chanNo]);
                        break;
                    }
                    case 2:{
                        compac.Alpha[chanNo] = ((onedata[pid*chanNo+2*chanNo+6]&0xff)<<8) + (onedata[pid*chanNo+2*chanNo+7]&0xff);
                        halpha->Fill(compac.Alpha[chanNo]);
                        break;
                    }
                    default:
                        return -1;
                    break;
                    }
                }
            }
            if(fread(&sigle,sizeof(char),1,fp)==NULL) break;
            compac.ionevt = (onedata[114]&0xff);
            for(int i=0;i<compac.ionevt;i++){
                tmp = onedata[i*6+115];
                compac.Si1Gain[i] = ((tmp&0xc0)>>6);
                compac.Si1[i] = ((onedata[i*6+115]&0xff)<<8) + (onedata[i*6+116]);
                tmp = onedata[i*6+117];
                compac.Si2Gain[i] = ((tmp&0xc0)>>6);
                compac.Si2[i] = ((onedata[i*6+117]&0xff)<<8) + (onedata[i*6+118]);
                tmp = onedata[i*6+119];
                compac.CsIGain[i] = ((tmp&0xc0)>>6);
                compac.CsI[i] = ((onedata[i*6+119]&0xff)<<8) + (onedata[i*6+120]);
            }
            compac.TotalCounts = ((onedata[631]&0xff)<<8) + (onedata[632]&0xff);
            compac.LPCounts = ((onedata[633]&0xff)<<8) + (onedata[634]&0xff);
            compac.HPCounts = ((onedata[635]&0xff)<<8) + (onedata[636]&0xff);
            compac.HETCounts = ((onedata[637]&0xff)<<8) + (onedata[638]&0xff);
            compac.FracFreq = (onedata[639]&0xff);

            trin->Fill();

            compac.packet_type = 0;
            compac.packet_length = 0;
            compac.packet_count = 0;
            for(int j=0;j<compac.ionevt;j++){
                compac.Si1Gain[j] = 0;
				compac.Si1[j] = 0;
				compac.Si2Gain[j] = 0;
				compac.Si2[j] = 0;
				compac.CsIGain[j] = 0;
				compac.CsI[j] = 0;
            }
            compac.TotalCounts = 0;
            compac.LPCounts = 0;
            compac.HPCounts = 0;
            compac.HETCounts = 0;
            compac.FracFreq = 0;

            eventnum++;
            if(eventnum%1000 == 0)
                printf("Packet %d th is being processed!\n",eventnum);
        }
        else{
            //fread(&top,sizeof(char),2,fp);
            continue;
        }
    }

    trin->Write();
    helectron->Write();
    hproton->Write();
    halpha->Write();

    fclose(fp);
    fin->Close();
    printf("Totally %d Packets have been processed!\n",eventnum);
    return 0;
}

int DecodeNormalPacket(Char_t *infile){
    ////////////////////////////////////////////////////////////
    /////////////////  Decoding Normal Packet  /////////////////
    ////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////
    /////////// electron\proton\alpha energy spectrum //////////
    ////////////////////////////////////////////////////////////

    TH1F *helectron = new TH1F("helectron","helectron",12,0,12);
    TH1F *hproton = new TH1F("hproton","hproton",30,0,30);
    TH1F *halpha = new TH1F("halpha","halpha",30,0,30);
   // Int_t elecbin,protonbin,alphabin;

    const Int_t Maxnum = 18;

    typedef struct{
        int packet_type;
        int packet_length;
        int packet_count;
        int second;
        int millisecond;
        int Elec[18];
        int Proton[18];
        int Alpha[18];
        int ionevt;
        int Si1High[Maxnum],Si1Low[Maxnum];
        int Si2Low[Maxnum],Si2Medium[Maxnum],Si2High[Maxnum];
        int CsISiPMT1[Maxnum],CsISiPMT2[Maxnum],CsISiPMT3[Maxnum];
        int CsISiPD1Low[Maxnum],CsISiPD1Medium[Maxnum],CsISiPD1High[Maxnum];
        int CsISiPD2Low[Maxnum],CsISiPD2Medium[Maxnum],CsISiPD2High[Maxnum];
        int CsISiPD3Low[Maxnum],CsISiPD3Medium[Maxnum],CsISiPD3High[Maxnum];
        int TotalCounts,LPCounts,HPCounts,HETCounts,FracFreq;
    }NormPacket_t;

    ////////////////////////////////////////////////////////////
    ////////////////////    Create Tree  ///////////////////////
    ////////////////////////////////////////////////////////////

    //char *destFile;
    //CreateTreeFile(destFile,infile);

    TFile *fin = new TFile(Form("%s.root",infile),"recreate");
    TTree *trin = new TTree("tree_Normal","tree_Normal");
    cout << "hello!\n" << endl;
    NormPacket_t normpac;
    trin->Branch("packet_type",&normpac.packet_type,"packet_type/I");
    trin->Branch("packet_length",&normpac.packet_length,"packet_length/I");
    trin->Branch("packet_count",&normpac.packet_count,"normpac.packet_count/I");
    trin->Branch("second",&normpac.second,"second/I");
    trin->Branch("millisecond",&normpac.millisecond,"millisecond/I");
    trin->Branch("Elec",normpac.Elec,"Elec[18]/I");
    trin->Branch("Proton",normpac.Proton,"Proton[18]/I");
    trin->Branch("Alpha",normpac.Alpha,"Alpha[18]/I");
    trin->Branch("ionevt",&normpac.ionevt,"ionevt/I");
    trin->Branch("Si1High",normpac.Si1High,"Si1High[ionevt]/I");
    trin->Branch("Si1Low",normpac.Si1Low,"Si1Low[ionevt]/I");
    trin->Branch("Si2Low",normpac.Si2Low,"Si2Low[ionevt]/I");
    trin->Branch("Si2Medium",normpac.Si2Medium,"Si2Medium[ionevt]/I");
    trin->Branch("Si2High",normpac.Si2High,"Si2High[ionevt]/I");
    trin->Branch("CsISiPMT1",normpac.CsISiPMT1,"CsISiPMT1[ionevt]/I");
    trin->Branch("CsISiPMT2",normpac.CsISiPMT2,"CsISiPMT2[ionevt]/I");
    trin->Branch("CsISiPMT3",normpac.CsISiPMT3,"CsISiPMT3[ionevt]/I");
    trin->Branch("CsISiPD1Low",normpac.CsISiPD1Low,"CsISiPD1Low[ionevt]/I");
    trin->Branch("CsISiPD2Low",normpac.CsISiPD2Low,"CsISiPD2Low[ionevt]/I");
    trin->Branch("CsISiPD3Low",normpac.CsISiPD3Low,"CsISiPD3Low[ionevt]/I");
    trin->Branch("CsISiPD1Medium",normpac.CsISiPD1Medium,"CsISiPD1Medium[ionevt]/I");
    trin->Branch("CsISiPD2Medium",normpac.CsISiPD2Medium,"CsISiPD2Medium[ionevt]/I");
    trin->Branch("CsISiPD3Medium",normpac.CsISiPD3Medium,"CsISiPD3Medium[ionevt]/I");
    trin->Branch("CsISiPD1High",normpac.CsISiPD1High,"CsISiPD1High[ionevt]/I");
    trin->Branch("CsISiPD2High",normpac.CsISiPD2High,"CsISiPD2High[ionevt]/I");
    trin->Branch("CsISiPD3High",normpac.CsISiPD3High,"CsISiPD3High[ionevt]/I");
    trin->Branch("TotalCounts",&normpac.TotalCounts,"TotalCounts/I");
    trin->Branch("LPCounts",&normpac.LPCounts,"LPCounts/I");
    trin->Branch("HPCounts",&normpac.HPCounts,"HPCounts/I");
    trin->Branch("HETCounts",&normpac.HETCounts,"HETCounts/I");
    trin->Branch("FracFreq",&normpac.FracFreq,"FracFreq/I");

    FILE *fp = fopen(Form("%s",infile),"r");
    if(!fp){
        printf("Error in openning input file: %s ...\n",infile);
    }

    //////////////////////////////////////////////////////////////
    ///////////////////  Decoding Packet  ////////////////////////
    //////////////////////////////////////////////////////////////
    unsigned char onedata[700];
    char top[2];
    char sigle;
    int eventnum = 0;
    while(!feof(fp)){
        if(fread(&top,sizeof(char),2,fp)==NULL){
            printf("This file is empty!\n");
        }
        if((top[0]&0xff)==0xeb && (top[1]&0xff)==0x90){
            if(fread(&sigle,sizeof(char),1,fp) == NULL) break;
            if(fread(&top,sizeof(char),2,fp) == NULL) break;
            normpac.packet_type = (top[0]&0xc0)>>6;
            normpac.packet_length = ((top[0]&0x3f)<<8) + (top[1]&0xff);
            if(normpac.packet_length != 640){
                printf("error in packet length %d and %dth Events\n",normpac.packet_length,eventnum);
            }
            if(fread(&top,sizeof(char),2,fp)==NULL) break;
            normpac.packet_count = ((top[0]&0xff)<<8) + (top[1]&0xff);
            if(fread(&onedata,sizeof(char),normpac.packet_length+2,fp)==NULL) break;
            normpac.second = ((onedata[0]&0xff)<<24) + ((onedata[1]&0xff)<<16) + ((onedata[2]&0xff)<<8) + (onedata[3]&0xff);
            normpac.millisecond = ((onedata[4]&0xff)<<8) + (onedata[5]&0xff);
            for(int pid=0;pid<3;pid++){
                for(int chanNo=0;chanNo<18;chanNo++){
                    switch(pid){
                    case 0:{
                        normpac.Elec[chanNo] = ((onedata[pid*chanNo+2*chanNo+6]&0xff)<<8) + (onedata[pid*chanNo+2*chanNo+7]&0xff);
                        helectron->Fill(normpac.Elec[chanNo]);
                        break;
                    }
                    case 1:{
                        normpac.Proton[chanNo] = ((onedata[pid*chanNo+2*chanNo+6]&0xff)<<8) + (onedata[pid*chanNo+2*chanNo+7]&0xff);
                        hproton->Fill(normpac.Proton[chanNo]);
                        break;
                    }
                    case 2:{
                        normpac.Alpha[chanNo] = ((onedata[pid*chanNo+2*chanNo+6]&0xff)<<8) + (onedata[pid*chanNo+2*chanNo+7]&0xff);
                        halpha->Fill(normpac.Alpha[chanNo]);
                        break;
                    }
                    default:
                        return -1;
                    break;
                    }
                }
            }
            if(fread(&sigle,sizeof(char),1,fp)==NULL) break;
            normpac.ionevt = (onedata[114]&0xff);
            for(int i=0;i<normpac.ionevt;i++){
                normpac.Si1High[i] = ((onedata[i*28+115]&0xff)<<8) + (onedata[i*28+116]&0xff);
                normpac.Si1Low[i] = ((onedata[i*28+117]&0xff)<<8) + (onedata[i*28+118]&0xff);
                normpac.Si2Low[i] = ((onedata[i*28+119]&0xff)<<8) + (onedata[i*28+120]&0xff);
                normpac.Si2Medium[i] = ((onedata[i*28+121]&0xff)<<8) + (onedata[i*28+122]&0xff);
                normpac.Si2High[i] = ((onedata[i*28+123]&0xff)<<8) + (onedata[i*28+124]&0xff);
                normpac.CsISiPMT1[i] = ((onedata[i*28+125]&0xff)<<8) + (onedata[i*28+126]&0xff);
                normpac.CsISiPD1Low[i] = ((onedata[i*28+127]&0xff)<<8) + (onedata[i*28+128]&0xff);
                normpac.CsISiPD1High[i] = ((onedata[i*28+129]&0xff)<<8) + (onedata[i*28+130]&0xff);
                normpac.CsISiPMT2[i] = ((onedata[i*28+131]&0xff)<<8) + (onedata[i*28+132]&0xff);
                normpac.CsISiPD2Low[i] = ((onedata[i*28+133]&0xff)<<8) + (onedata[i*28+134]&0xff);
                normpac.CsISiPD2High[i] = ((onedata[i*28+135]&0xff)<<8) + (onedata[i*28+136]&0xff);
                normpac.CsISiPMT3[i] = ((onedata[i*28+137]&0xff)<<8) + (onedata[i*28+138]&0xff);
                normpac.CsISiPD3Low[i] = ((onedata[i*28+139]&0xff)<<8) + (onedata[i*28+140]&0xff);
                normpac.CsISiPD3High[i] = ((onedata[i*28+141]&0xff)<<8) + (onedata[i*28+142]&0xff);
            }
            normpac.TotalCounts = ((onedata[631]&0xff)<<8) + (onedata[632]&0xff);
            normpac.LPCounts = ((onedata[633]&0xff)<<8) + (onedata[634]&0xff);
            normpac.HPCounts = ((onedata[635]&0xff)<<8) + (onedata[636]&0xff);
            normpac.HETCounts = ((onedata[637]&0xff)<<8) + (onedata[638]&0xff);
            normpac.FracFreq = (onedata[639]&0xff);

            trin->Fill();

            normpac.packet_type = 0;
            normpac.packet_length = 0;
            normpac.packet_count = 0;
            for(int j=0;j<normpac.ionevt;j++){
                normpac.Si1High[j] = 0; normpac.Si1Low[j] = 0;
                normpac.Si2Low[j] = 0; normpac.Si2Medium[j] = 0; normpac.Si2High[j] = 0;
                normpac.CsISiPMT1[j] = 0; normpac.CsISiPD1Low[j] = 0; normpac.CsISiPD1High[j] = 0;
                normpac.CsISiPMT2[j] = 0; normpac.CsISiPD2Low[j] = 0; normpac.CsISiPD2High[j] = 0;
                normpac.CsISiPMT3[j] = 0; normpac.CsISiPD3Low[j] = 0; normpac.CsISiPD3High[j] = 0;
            }
            normpac.TotalCounts = 0;
            normpac.LPCounts = 0;
            normpac.HPCounts = 0;
            normpac.HETCounts = 0;
            normpac.FracFreq = 0;

            eventnum++;
            if(eventnum%1000 == 0)
                printf("Packet %d th is being processed!\n",eventnum);
        }
        else{
            //fread(&top,sizeof(char),2,fp);
            continue;
        }
    }

    trin->Write();
    helectron->Write();
    hproton->Write();
    halpha->Write();

    fclose(fp);
    fin->Close();
    printf("Totally %d Packets have been processed!\n",eventnum);

    return 0;
}
