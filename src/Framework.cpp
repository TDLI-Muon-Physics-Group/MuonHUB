#include "Framework.h"
#include "PluginFactory.h"
#include <iostream>
#include <vector>
#include <string>

Framework::Framework(const std::string& configInput, const std::vector<std::string>& pluginNames, const bool submitCondor)
  : configInput(configInput), pluginNames(pluginNames), submitCondor(submitCondor) {}

std::string Framework::getCurrentWorkingDirectory() {
  char temp[PATH_MAX];
  if (getcwd(temp, sizeof(temp)) != NULL) {
    return std::string(temp);
  } else {
    // Handle error
    perror("getcwd() error");
    return std::string("");
  }
}

bool Framework::execute() {

  std::string currentPath = Framework::getCurrentWorkingDirectory();
  std::string currentConfigInput= (submitCondor) ? configInput : currentPath + "/" + configInput;
  //string parentPath = (contain(outPath,".root")) ? fs::path(outPath).parent_path().c_str() : outPath;
  //cout<<"Parent path : "<< parentPath.c_str() << endl;

  std::cout<<" read in configuration file : " << currentConfigInput << std::endl;
  std::cout<<" size of plugins : " << pluginNames.size() << std::endl;
  
  for (size_t i=0 ; i<pluginNames.size() ; i++) {

    std::cout<<"Start i : "<< i <<std::endl;
    
    auto plugin = PluginFactory::instance().create(pluginNames.at(i));
    if (!plugin) {
      std::cerr << "Plugin " << pluginNames.at(i) << " could not be created." << std::endl;
      return false;
    }
    
    if (!plugin->initialize( currentConfigInput )) {
      std::cerr << "Plugin " << pluginNames.at(i) << " failed to initialize." << std::endl;
      return false;
    }
    
    if (!plugin->process()) {
      std::cerr << "Plugin " << pluginNames.at(i) << " failed to process." << std::endl;
      return false;
    }
    
    if (!plugin->finalize()) {
      std::cerr << "Plugin " << pluginNames.at(i) << " failed to finalize." << std::endl;
      return false;
    }

    std::cout<<"DONE HERE"<<std::endl;
    // Update the input file for the next plugin
    //currentInputFile = currentOutputFile;
  }

  std::cout<<"DONE HERE TOO"<<std::endl;
  
  // Rename the final output file
  //if (std::rename(currentInputFile.c_str(), outputFileName.c_str()) != 0) {
  //  std::cerr << "Failed to rename final output file to " << outputFileName << std::endl;
  //  return false;
  //}
  
  return true;
}
