#ifndef DRS4_H
#define DRS4_H

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
  
};

// Apply the registration macro
REGISTER_PLUGIN("DRS4", DRS4)

#endif
