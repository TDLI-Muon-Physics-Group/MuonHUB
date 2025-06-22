#ifndef PLUGINBASE_H
#define PLUGINBASE_H

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

#include <filesystem>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>
#include <algorithm> // For std::remove_if
#include <cctype>    // For std::isspace
#include <unordered_set>

#include "config.h"

namespace fs = std::filesystem;

class PluginBase {
public:
  
  virtual ~PluginBase() = default;
  virtual bool initialize(const std::string &paramsFile) = 0;
  //virtual bool initialize(const std::string &paramsFile) = 0;
  virtual bool process() = 0;
  virtual bool finalize() = 0;
  std::string removeWhitespace(const std::string& str);
  Config loadParams(const std::string &paramsFile);
  bool isTxt(const std::string& filename);
  
  std::vector<std::string> getListOfFiles(std::string path, std::string match ) const;
  std::vector<std::string> getListOfFiles(std::string path, std::unordered_set<std::string> excludedRuns) const;
  std::string getCurrentWorkingDirectory() const;
  std::vector<std::string> split(std::string s, char d) const;
  bool dirExists(const char *path) const;
  std::string replaceString(std::string target, std::string toReplace, std::string replacement ) const;
  std::unordered_set<std::string> loadExcludeRunList(const std::string& filename) const;

};

inline std::unordered_set<std::string> PluginBase::loadExcludeRunList(const std::string& filename) const {
  std::unordered_set<std::string> excludeRuns;
  std::ifstream file(filename);
  
  if (!file.is_open()) {
    std::cerr << "Error opening exclude run list file: " << filename << std::endl;
    return excludeRuns;
  }
  
  std::string line;
  while (std::getline(file, line)) {
    excludeRuns.insert(line);
  }
  
  file.close();
  return excludeRuns;
}

inline std::string PluginBase::removeWhitespace(const std::string& str) {
  std::string result = str;
  result.erase(std::remove_if(result.begin(), result.end(), [](unsigned char c) { return std::isspace(c); }), result.end());
  return result;
}

inline Config PluginBase::loadParams(const std::string &paramsFile) {
  Config params_out;
  std::ifstream file(paramsFile);
  if (!file) {
    std::cerr << "Error opening parameters file: " << paramsFile << std::endl;
    return params_out;
  }
  
  std::string line;
  while (std::getline(file, line)) {
    std::istringstream iss(line);
    std::string key_;
    std::string value;

    // ignore comment
    if (line.find('#')!=std::string::npos) continue;

    if (std::getline(iss, key_, ':') && std::getline(iss, value)) {

      // Remove leading whitespace from value
      value.erase(0, value.find_first_not_of(" \t"));
      std::string key = removeWhitespace(key_);
      if (key == "dataset") {
	params_out.dataset = value;
	//params_out.filein = getCurrentWorkingDirectory() + "/" + value;
      }

      /*
      // Remove leading whitespace from value
      value.erase(0, value.find_first_not_of(" \t"));
      std::string key = removeWhitespace(key_);
      if (key == "minEn") {
        params_out.minEn = std::stof(value);
      } else if (key == "maxEn") {
        params_out.maxEn = std::stof(value);
      } else if (key == "binWidth") {
        params_out.binWidth = std::stof(value);
      } else if (key == "yRange") {
        params_out.yRange = std::stof(value);
      } else if (key == "adjRange") {
        params_out.adjRange = std::stof(value);
      } else if (key == "loopTh") {
        params_out.loopTh = std::stof(value);
      } else if (key == "sig") {
        params_out.sig = std::stof(value);
      } else if (key == "clTh") {
        params_out.clTh = std::stof(value);
      } else if (key == "minEnt") {
	params_out.minEnt = std::stoi(value);
      } else if (key == "reso") {
        params_out.reso = getCurrentWorkingDirectory() + "/" + value;
      } else if (key == "subrun") {
        params_out.subrun = value;
      } else if (key == "dataset") {
        params_out.inputFile = value;
      } else if (key == "azShift") {
	params_out.azShift = value;
      } else if (key == "CaloAcc") {
	params_out.CaloAcc = value;
      } else if (key == "runCalo") {
	params_out.runCalo = std::stoi(value);
      } else if (key == "excluRun") {
	params_out.excluRun = value;
      }
      */
    }
  }
  file.close();
  return params_out;
}

inline bool PluginBase::isTxt(const std::string& filename) {
    const std::string extension = ".txt";
    // Check if the string ends with ".txt"
    if (filename.length() >= extension.length()) {
        return filename.compare(filename.length() - extension.length(), extension.length(), extension) == 0;
    }
    return false;
}

inline std::vector<std::string> PluginBase::getListOfFiles(std::string path, std::string match) const {
  
  std::vector<std::string> out;
  try {
    for (const auto &entry : fs::directory_iterator(path)) {
      std::string pathStr = entry.path().string();
      if (pathStr.find(match)==std::string::npos) continue;
      //std::cout<<"accepted root file : " << pathStr << std::endl;
      out.push_back(pathStr);
    }
  } catch (const fs::filesystem_error& err) {
    std::cerr << "Filesystem error: " << err.what() << std::endl;
  }
  std::cout<<"size of list : " << out.size() << std::endl;
  return out;
}

inline std::vector<std::string> PluginBase::getListOfFiles(std::string path, std::unordered_set<std::string> excludedRuns) const {

  std::vector<std::string> out;
  
  try {
    for (const auto &entry : fs::directory_iterator(path)) {
      
      std::string pathStr = entry.path().string();
      std::string fileName_ = entry.path().filename().string();
      fs::path filename(fileName_);

      // Skip the file if the extension is not '.root'
      if (filename.extension() != ".root") continue;
      
      std::string fileName = filename.stem().string();
      std::string runNumber = fileName.substr(fileName.find('-') + 1, fileName.find('-') - fileName.find('-') - 1);
      
      // Skip this file if the run number is in the exclude list 
      if (excludedRuns.find(runNumber) != excludedRuns.end()) continue;
      
      out.push_back(pathStr);
    }
  } catch (const fs::filesystem_error& err) {
    std::cerr << "Filesystem error: " << err.what() << std::endl;
  }
  std::cout<<"size of list : " << out.size() << std::endl;
  return out;

}

inline std::string PluginBase::getCurrentWorkingDirectory() const {
  char temp[PATH_MAX];
  if (getcwd(temp, sizeof(temp)) != NULL) {
    return std::string(temp);
  } else {
    // Handle error
    perror("getcwd() error");
    return std::string("");
  }
}

inline std::vector<std::string> PluginBase::split(std::string s, char d) const {
  
  std::vector<std::string> split_string;
  std::stringstream ss(s);
  std::string token;
  while (std::getline(ss, token, d)) {
    split_string.push_back(token);
  }
  
  return split_string;
}

inline bool PluginBase::dirExists(const char *path) const {
  struct stat info;
  
  if(stat( path, &info ) != 0)
    return 0;
  else if(info.st_mode & S_IFDIR)
    return 1;
  else
    return 0;
}

inline std::string PluginBase::replaceString(std::string target, std::string toReplace, std::string replacement) const {

  // Find the position of the substring to replace
  size_t pos = target.find(toReplace);
  
  if (pos != std::string::npos) {
    // Replace the substring
    target.replace(pos, toReplace.length(), replacement);
  } else {
    std::cerr << "Substring not found!" << std::endl;
  }
  
  return target;
}


#endif
