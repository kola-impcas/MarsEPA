#include "stdio.h"
#include <iostream>
#include "stdlib.h"
#include "string.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"

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

