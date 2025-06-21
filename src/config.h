#ifndef CONFIG_H
#define CONFIG_H

// Define the Params struct with fields corresponding to the parameters in the file                                                                                                      
struct Config {
  // global parameter
  std::string dataset;
  std::string inputFile;
  std::string subrun;
  // tracker                                                                                                                                                                             
  std::string reso;
  float minEn;
  float maxEn;
  float binWidth;
  float yRange; // mm                                                                                                                                                                    
  float adjRange; // +/- bins                                                                                                                                                            
  unsigned int loopTh;
  float sig;
  float clTh;
  int minEnt;
  // azimuth
  std::string azShift;
  std::string CaloAcc;
  // fitamps calo
  int runCalo;
  // exclude run
  std::string excluRun;
};

#endif
