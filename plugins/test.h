#ifndef TEST_H
#define TEST_H

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

#include "paths.h"
#define SIGMA3 99.73

class test : public PluginBase {
public:
  bool initialize(const std::string &paramsFile, const bool fitCalorimeter) override;
  bool process() override;
  bool finalize() override;
  
private:
  std::string prefix;
  Config config;
  TFile *input;
  TFile *output;
  
  std::ifstream iDataSets;
  std::vector<std::string> dataSets;
  std::vector<std::string> enHigh;
  std::map<std::string, float> resMap;
  
};

// Apply the registration macro
REGISTER_PLUGIN("test", test)

#endif
