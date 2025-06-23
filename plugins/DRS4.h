#ifndef DRS4_H
#define DRS4_H

#include "PluginBase.h"
#include "PluginFactory.h"

// include std libraries 
#include <iostream>
#include <fstream>
#include <regex>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <iomanip>

// include ROOT libraries 
#include "TH1.h"
#include "TH2.h"
#include "TF1.h"
#include "TTree.h"
#include "TChain.h"
#include "TGraph.h"
#include "TStyle.h"
#include "TFolder.h"
#include "TCanvas.h"
#include "TRandom.h"
#include "TMath.h"
#include "TFile.h"
#include "TSystem.h"
#include "TProfile.h"
#include "TSpectrum.h"

using namespace std;

class DRS4 : public PluginBase {
public:
  bool initialize(const std::string &paramsFile) override;
  bool process() override;
  bool finalize() override;
  
private:
  std::string prefix;
  Config config;
  
  std::ifstream iDataSets;
  std::vector<std::string> dataSets;

  int verbose = 0;
  
  struct channel_stat{
    Double_t max_v;
    Double_t max_t;
    Double_t min_v;
    Double_t min_t;
    Double_t mean;
    Double_t RMS;
  };
  
  struct time_info{
    Short_t year;
    Short_t month;
    Short_t day;
    Short_t hour;
    Short_t min;
    Short_t sec;
    Short_t ms;
  };

  //
  char FileHeader[5];
  char TimeHeader[5];
  char EventHeader[5];
  char BoardSerialNumber[5];
  char TriggerCellNumber[5];
  short Date[8];
  short number;
  float EventTime[1024];
  int SerialNumber;
  int ScalerNumber;
  char ChannelHeader[5];
  unsigned short ChannelDataRaw[1024];
  unsigned short ChannelData[1024];
  bool endoffile = false;
  int n=0;
  clock_t start = clock();
  time_t realtime;
  
  channel_stat chstat_manager[8];     // Maximum 8 channels, i.e. 2 boards in chain;                                                                                                        
  std::vector<Double_t> chT_manager[8];     // Maximum 8 channels, i.e. 2 boards in chain;                                                                                                  
  std::vector<Double_t> chV_manager[8];     // Maximum 8 channels, i.e. 2 boards in chain;                                                                                                  
  time_info time_manager;
  UInt_t nevent = 0;
  Int_t  data_length = 1024;
  
  
  
};

// Apply the registration macro
REGISTER_PLUGIN("DRS4", DRS4)

#endif
