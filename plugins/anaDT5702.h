#ifndef ANADT5792_H
#define ANADT5702_H

#include "mppc.h"
#include "TF1.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include <queue>

using namespace std;

class anaDT5702 : public mppc {
public:
    anaDT5702(TTree *tree=0):mppc(tree){};
    ~anaDT5702() {};
    virtual void Loop(string infname, string outfname);
    void initialize(string infname, string outfname);
    void execute();
    void rearrange(bool if_rearr, Long64_t jentry_start, Long64_t jentry_end, UInt_t add_ts);
    void finalize();
    void clear();
    void loader(int rate);

private:
    int mac_num;
    TFile *m_file;
    std::vector<std::pair<Long64_t, UInt_t>> ts_sort_vec;
    // *************** Variables to be used is registered here ********************

    TTree *mppc_jp;

    UShort_t ADC_ch00;
    UShort_t ADC_ch01;
    UShort_t ADC_ch02;
    UShort_t ADC_ch03;
    UShort_t ADC_ch04;
    UShort_t ADC_ch05;
    UShort_t ADC_ch06;
    UShort_t ADC_ch07;
    UShort_t ADC_ch08;
    UShort_t ADC_ch09;
    UShort_t ADC_ch10;
    UShort_t ADC_ch11;
    UShort_t ADC_ch12;
    UShort_t ADC_ch13;
    UShort_t ADC_ch14;
    UShort_t ADC_ch15;
    UShort_t ADC_ch16;
    UShort_t ADC_ch17;
    UShort_t ADC_ch18;
    UShort_t ADC_ch19;
    UShort_t ADC_ch20;
    UShort_t ADC_ch21;
    UShort_t ADC_ch22;
    UShort_t ADC_ch23;
    UShort_t ADC_ch24;
    UShort_t ADC_ch25;
    UShort_t ADC_ch26;
    UShort_t ADC_ch27;
    UShort_t ADC_ch28;
    UShort_t ADC_ch29;
    UShort_t ADC_ch30;
    UShort_t ADC_ch31;

    UInt_t ts_last=0;
    UInt_t ts_last_temp=0;
    UInt_t ts_delta;
    UInt_t ts0_ref_last=0;

    UInt_t sec_cnt=0;
    UInt_t err_sec_cnt=0;
    UInt_t add_ts=0;

    Long64_t jentry;
    Long64_t run_ID;
    Long64_t jentry_start=0;
    bool if_rearr=false;
    bool if_first=true;

    // ****************************************************************************
};
#endif
