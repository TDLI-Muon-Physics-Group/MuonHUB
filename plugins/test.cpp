#include "test.h"

bool test::initialize(const std::string &paramsFile , const bool fitCalorimeter=false) {
  
  prefix = "TrackResoCorr";
  
  // load param
  config = loadParams(paramsFile);

  std::cout << "Parameters read from: "<< paramsFile.c_str() << std::endl;
  std::cout << "  minEn: " << config.minEn << std::endl;
  std::cout << "  maxEn: " << config.maxEn << std::endl;
  std::cout << "  binWidth: " << config.binWidth << std::endl;
  std::cout << "  yRange: " << config.yRange << std::endl;
  std::cout << "  adjRange: " << config.adjRange << std::endl;
  std::cout << "  loopTh: " << config.loopTh << std::endl;
  std::cout << "  sig: " << config.sig << std::endl;
  std::cout << "  clTh: " << config.clTh << std::endl;
  std::cout << "  minEnt: " << config.minEnt << std::endl;
  std::cout << "  Reso: " << config.reso << std::endl;
  std::cout << "  subrun: " << config.subrun << std::endl;
  std::cout << "  inList: " << config.inputFile << std::endl;
  std::cout << "  fitCalorimeter : " << fitCalorimeter << std::endl;

  // load full list of dataset
  iDataSets.open(config.inputFile);
  std::string dataSet;
  std::cout<<"Will analyze following datasets:\n";
  while(iDataSets>>dataSet){
    std::string	subdataset = dataPath+dataSet;
    if (config.subrun != "All" && subdataset.find(config.subrun) == std::string::npos ) continue;
    std::cout<<subdataset<<std::endl;
    
    dataSets.push_back(dataPath+dataSet);
  }
  
  // prepare the tracker resolution
  std::ifstream iTrackRes;
  iTrackRes.open(config.reso);
  std::string _enLow;
  std::string _enHigh;
  float _res;
  std::vector<std::string> enLow;
  //std::vector<std::string> enHigh;
  //std::map<std::string, float> resMap;
  while(iTrackRes>>_enLow>>_enHigh>>_res){
    enLow.push_back(_enLow);
    enHigh.push_back(_enHigh);
    resMap[_enHigh] = _res;
  }
  
  return true; 
}

bool test::process() {

  //
  //TFile * f = new TFile();
  
  // loop on datasets exp : 4AB
  for(auto set : dataSets){
    
    // prepare output folder
    std::string newFolder = set + "-"+prefix+"/";
    if(!dirExists(newFolder.c_str()))
      gSystem->mkdir(newFolder.c_str(), kTRUE);
    
    std::vector<std::string> runs = getListOfFiles(set, ".root");
    
    // looping on datasets's file
    for(auto run : runs){
      
      std::cout<<"Analyzing run "<<run<<"\n";
      std::string ofname = newFolder + fs::path(run).stem().string() + "-"+prefix+".root";

      std::unique_ptr<TFile> filein(TFile::Open(run.c_str(), "READ")); // Open the input file
      if (!filein || filein->IsZombie()) {
	std::cerr << "Error opening file: " << run << "\n";
	continue;
      }
      
      std::array<std::string,2> stations= {"12","18"};
      std::vector<TH1F*> hists;
      // hists consists of energy bracket for station 12 and 18
      for(auto st : stations){
        for(auto en : enHigh){
          std::string hname = "hYReco-" + st + "-" + en;
	  TH1F* hist = (TH1F*)gDirectory->Get(hname.c_str());
	  if (hist) hists.push_back(hist);
        }
      }
      
      std::cout<<"output run "<<ofname<<"\n";
      std::unique_ptr<TFile> f(TFile::Open(ofname.c_str(), "RECREATE")); // Open the output file
      if (!f || f->IsZombie()) {
	std::cerr << "Error creating file: " << ofname << "\n";
	continue;
      }
      
      std::cout<<"hists size : " << hists.size() << std::endl;
      for(auto h : hists){
	std::cout<<"here we go"<<std::endl;
	std::cout<<"histogram mean : " << h->GetMean() << std::endl;
	h->Write();
      }
      
      //break;
    } // end of run loop
  } // end of dataset loop
  
  return true;
}

bool test::finalize() {
  
  std::cout << "test module completed successfully." << std::endl;
  return true;
}
