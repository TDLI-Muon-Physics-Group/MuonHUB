#include "Framework.h"
#include <iostream>
#include <vector>
#include "tclap/CmdLine.h"
#include "string/string_io.h"

int main(int argc, char** argv) {
  
  TCLAP::CmdLine cmd( "uconvert" , ' ' , "2.0" );
  TCLAP::ValueArg<std::string> configIn_     ( "c" , "configIn_"       , "Input configuration file"    , true  , "some.txt"  , "string" , cmd );
  TCLAP::MultiArg<std::string> listPlugin_   ( "m" , "listPlugin_"     , "module list"   , true  , "string" , cmd );
  TCLAP::SwitchArg             submitCondor_ ( "s" , "submitCondor_"   , "submit condor" , cmd   , false );
  //TCLAP::SwitchArg             fitCalo_ ( "f" , "fitCalo_"   , "Fitting Calorimeter amplitude" , cmd   , false );
  
  cmd.parse( argc, argv );
  
  std::string configIn = configIn_.getValue();
  std::vector<std::string> listPlugin = listPlugin_.getValue();
  bool submitCondor = submitCondor_.getValue();
  //bool fitCalorimeter = fitCalo_.getValue();

  // file list in
  std::cout<<"Read in configuration file : " << configIn << std::endl;
  
  // register plugins
  for (size_t i=0 ; i<listPlugin.size() ; i++)
    std::cout<<"Registered module name : " << listPlugin.at(i).c_str() << std::endl;
  
  // Create and run the framework
  //Framework framework( configIn , listPlugin , submitCondor , fitCalorimeter );
  Framework framework( configIn , listPlugin , submitCondor );
  if (!framework.execute()) {
    std::cerr << "Framework execution failed." << std::endl;
    return 1;
  }
  
  std::cout<<"END OF OPERATION"<<std::endl;
  
  return 0;
}
