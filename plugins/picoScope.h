#ifndef PICOSCOPE_H
#define PICOSCOPE_H

#include "PluginBase.h"
#include "PluginFactory.h"

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include "rapidcsv.h"
#include "TTree.h"
#include "TFile.h"
#include "TString.h"
#include "TMath.h"


using namespace std;

class picoScope : public PluginBase {
public:
  bool initialize(const std::string &paramsFile) override;
  bool process() override;
  bool finalize() override;
  void loader(int rate) override;
  
private:
  std::string prefix;
  Config config;
  
  std::ifstream iDataSets;
  std::vector<std::string> dataSets;

  //
  struct channel_stat{
    Double_t max_v;
    Double_t max_t;
    Double_t min_v;
    Double_t min_t;
    Double_t mean;
    Double_t RMS;
  };
  
};

inline void picoScope::loader(int rate){
  
  char proc[22];
  memset(proc, '\0', sizeof(proc));
  
  for (int i = 0; i < rate/5; i++)
    {
      proc[i] = '#';
    }
  
  printf("\r[%-20s] [%d%%]", proc, rate);        //C语言格式控制时默认右对齐，所以要在前面加-变成左对齐                                                                                     
  fflush(stdout);                                 //刷新屏幕打印               
  
}

// Apply the registration macro
REGISTER_PLUGIN("picoScope", picoScope)

#endif
