#ifndef DT5702_H
#define DT5702_H

#include "PluginBase.h"
#include "PluginFactory.h"

//#include <sys/stat.h>
#include <fstream>
#include <TFile.h>
#include <TH1.h>
#include <TF1.h>
#include <TTree.h>
#include <TSystem.h>
#include <TRandom3.h>
#include <iostream>

// all analyzer 
#include "anaDT5702.h"

class DT5702 : public PluginBase {
public:
  // Framework
  bool initialize(const std::string &paramsFile) override;
  bool process() override;
  bool finalize() override;

  //
  
private:
  std::string prefix;
  Config config;
  
  std::ifstream iDataSets;
  std::vector<std::string> dataSets;
  
};

// Apply the registration macro
REGISTER_PLUGIN("DT5702", DT5702)

#endif
