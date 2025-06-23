#include "anaDT5702.h"
#include <iostream>
#include <cmath>
#include <TROOT.h>
#include <TRandom.h>
#include <TCanvas.h>
#include <string.h>
#include <stdio.h>
#include "TSystem.h"

#include "TGraph.h"

#include <vector>
#include <algorithm>


void anaDT5702::Loop (string infname, string outfname)
{
    //  cout << "Loop()" << endl;

    initialize (infname, outfname);

    if (fChain == 0)
        return;

    Long64_t nentries = fChain->GetEntriesFast ();
    Int_t nbytes = 0, nb = 0;

    std::cout << "Processing " << nentries << " events..." << std::endl;

    for (jentry = 0; jentry < nentries; jentry++)
    {
        if (jentry % (nentries/100) == 0) loader(jentry/(nentries/100));
        Long64_t ientry = LoadTree (jentry);
        if (ientry < 0)
            break;
        nb = fChain->GetEntry (jentry);
        nbytes += nb;

        clear ();
        execute ();

    }

//    add_ts += ts0_ref;
//    rearrange(if_rearr, jentry_start, jentry, add_ts);

    finalize ();

}

void anaDT5702::initialize (string infname, string outfname)
{

    cout << "initialize()" << endl;
    // ********************** Create ROOT variables for analysis *************************
    char *out = (char *) outfname.c_str ();
    m_file = new TFile (out, "recreate");
    mppc_jp = new TTree("mppcjp", "Preprocessing of mppc tree for further analysis");

    //*********************** Initialize *************************************

    mppc_jp->Branch("run_ID", &run_ID, "run_ID/i");

    mppc_jp->Branch("ADC_ch00", &chg[0], "ADC_ch00/s");
    mppc_jp->Branch("ADC_ch01", &chg[1], "ADC_ch01/s");
    mppc_jp->Branch("ADC_ch02", &chg[2], "ADC_ch02/s");
    mppc_jp->Branch("ADC_ch03", &chg[3], "ADC_ch03/s");
    mppc_jp->Branch("ADC_ch04", &chg[4], "ADC_ch04/s");
    mppc_jp->Branch("ADC_ch05", &chg[5], "ADC_ch05/s");
    mppc_jp->Branch("ADC_ch06", &chg[6], "ADC_ch06/s");
    mppc_jp->Branch("ADC_ch07", &chg[7], "ADC_ch07/s");
    mppc_jp->Branch("ADC_ch08", &chg[8], "ADC_ch08/s");
    mppc_jp->Branch("ADC_ch09", &chg[9], "ADC_ch09/s");
    mppc_jp->Branch("ADC_ch10", &chg[10], "ADC_ch10/s");
    mppc_jp->Branch("ADC_ch11", &chg[11], "ADC_ch11/s");
    mppc_jp->Branch("ADC_ch12", &chg[12], "ADC_ch12/s");
    mppc_jp->Branch("ADC_ch13", &chg[13], "ADC_ch13/s");
    mppc_jp->Branch("ADC_ch14", &chg[14], "ADC_ch14/s");
    mppc_jp->Branch("ADC_ch15", &chg[15], "ADC_ch15/s");
    mppc_jp->Branch("ADC_ch16", &chg[16], "ADC_ch16/s");
    mppc_jp->Branch("ADC_ch17", &chg[17], "ADC_ch17/s");
    mppc_jp->Branch("ADC_ch18", &chg[18], "ADC_ch18/s");
    mppc_jp->Branch("ADC_ch19", &chg[19], "ADC_ch19/s");
    mppc_jp->Branch("ADC_ch20", &chg[20], "ADC_ch20/s");
    mppc_jp->Branch("ADC_ch21", &chg[21], "ADC_ch21/s");
    mppc_jp->Branch("ADC_ch22", &chg[22], "ADC_ch22/s");
    mppc_jp->Branch("ADC_ch23", &chg[23], "ADC_ch23/s");
    mppc_jp->Branch("ADC_ch24", &chg[24], "ADC_ch24/s");
    mppc_jp->Branch("ADC_ch25", &chg[25], "ADC_ch25/s");
    mppc_jp->Branch("ADC_ch26", &chg[26], "ADC_ch26/s");
    mppc_jp->Branch("ADC_ch27", &chg[27], "ADC_ch27/s");
    mppc_jp->Branch("ADC_ch28", &chg[28], "ADC_ch28/s");
    mppc_jp->Branch("ADC_ch29", &chg[29], "ADC_ch29/s");
    mppc_jp->Branch("ADC_ch30", &chg[30], "ADC_ch30/s");
    mppc_jp->Branch("ADC_ch31", &chg[31], "ADC_ch31/s");

//    mppc_jp->Branch("ts", &ts0, "ts/i");
//    mppc_jp->Branch("ts_ref", &ts0_ref, "ts_ref/i");
//    mppc_jp->Branch("ts_delta", &ts_delta, "ts_delta/i");
//
//    mppc_jp->Branch("sec_cnt", &sec_cnt, "sec_cnt/i");


}


void anaDT5702::execute ()
{
//    if (ts0 == 0) {     // A pulse comes in
//        add_ts += ts0_ref_last;
//        ts0_ref_last = ts0_ref;
//
//        rearrange(if_rearr, jentry_start, jentry, add_ts);
//        jentry_start = jentry+1;
//        ts_last = ts_last_temp;
//
//        sec_cnt++;
//        if_rearr = false;
//        if_first = true;
//
//        return;
//    }
//
//    if (ts0 < ts_last_temp && !if_first) if_rearr = true;
//    ts_last_temp = ts0;
//    if_first = false;
//    add_ts = 0;
    mppc_jp->Fill();

}


void anaDT5702::finalize ()
{
    cout<<endl<<"Finalizing......"<<endl;
    m_file->cd();
    // **************************** Your finalizing here *********************************

//    std::cout << "Pre-analysis complete. Fix " << err_sec_cnt << " misaligned seconds out of " << sec_cnt << " seconds." << std::endl;
//    mppc_jp->Write();

    // ***********************************************************************************
    m_file->Write ();
    m_file->Close ();
}

void anaDT5702::clear (){}

void anaDT5702::rearrange(bool if_rearr, Long64_t jentry_start, Long64_t jentry_end, UInt_t add_ts)
{
    if (jentry_start >= jentry_end) return;

    if (!if_rearr){
        for (int kentry = jentry_start; kentry<jentry_end; kentry++){
            fChain->GetEntry(kentry);
            run_ID=kentry;
            ts_delta = ts0 + add_ts - ts_last;
            add_ts = 0;
            ts_last = ts0;

            mppc_jp->Fill();
        }
    }
    else {
        for (int kentry = jentry_start; kentry<jentry_end; kentry++){
            fChain->GetEntry(kentry);
            ts_sort_vec.push_back(std::make_pair(kentry, ts0));
        }

        std::sort(ts_sort_vec.begin(), ts_sort_vec.end(), [] (std::pair<Long64_t, UInt_t> x, std::pair<Long64_t, UInt_t> y) {return x.second < y.second;});

        for (auto temp_pair : ts_sort_vec) {
            run_ID=temp_pair.first;
            fChain->GetEntry(run_ID);

            ts_delta = ts0 + add_ts - ts_last;
            add_ts = 0;
            ts_last = ts0;

            mppc_jp->Fill();
        }
        ts_sort_vec.clear();
        ts_sort_vec.shrink_to_fit();
        err_sec_cnt++;
    }

}
void anaDT5702::loader(int rate)
{
    char proc[22];
    memset(proc, '\0', sizeof(proc));

    for (int i = 0; i < rate/5; i++)
    {
        proc[i] = '#';
    }

    printf("\r[%-20s] [%d%%]", proc, rate);        //C语言格式控制时默认右对齐，所以要在前面加-变成左对齐
    fflush(stdout);                                 //刷新屏幕打印
}
