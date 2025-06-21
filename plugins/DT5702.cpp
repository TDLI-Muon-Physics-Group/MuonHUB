#include "DT5702.h"

bool DT5702::initialize(const std::string &paramsFile) {
  
  prefix = "DT5702";
    
  // load param
  config = loadParams(paramsFile);
  
  std::cout << "Parameters read from: "<< paramsFile.c_str() << std::endl;
  std::cout << "  dataset : " << config.dataset << std::endl;
  
  if (isTxt(config.dataset)){
    
    // load full list of dataset
    iDataSets.open(config.inputFile);
    std::string dataSet;
    std::cout<<"Will analyze following datasets:\n";
    while(iDataSets>>dataSet){
      //std::string	subdataset = dataPath+dataSet;
      //if (config.subrun != "All" && subdataset.find(config.subrun) == std::string::npos ) continue;
      std::cout<<dataSet<<std::endl;
      
      dataSets.push_back(dataSet);
    }
    
  }
  else{
    dataSets.push_back(config.dataset);
  }
  
  return true; 
}

bool DT5702::process() {
  
  for(auto set : dataSets){
    
    //
    TChain *chain = new TChain ("mppc");
    string infname(set);
    fs::path path(infname);
    string outfname = path.filename();
    outfname.replace(outfname.rfind("mppc"), 4, "mppcPro");
    string directory = path.parent_path();
    
    // prepare output folder
    std::string newFolder = directory + "-"+prefix+"/";
    if(!dirExists(newFolder.c_str()))
      gSystem->mkdir(newFolder.c_str(), kTRUE);
    
    // read file
    TFile* infile = new TFile(TString(infname), "READ");

    // constructor
    
    anaDT5702 ana((TTree*) infile->Get("mppc"));
    ana.Loop(infname, outfname);
    
  }
  
  return true;
}

bool DT5702::finalize() {
  
  std::cout << "test module completed successfully." << std::endl;
  return true;
}
