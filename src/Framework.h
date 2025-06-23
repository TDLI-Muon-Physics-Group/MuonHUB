#ifndef FRAMEWORK_H
#define FRAMEWORK_H

#include <vector>
#include <string>
#include <memory>
#include "PluginBase.h"
#include <filesystem>

#include <unistd.h>
#include <limits.h>

namespace fs = std::filesystem;

class Framework {
public:
  Framework(const std::string& configInput, const std::vector<std::string>& pluginNames, const bool submitCondor);
  std::string getCurrentWorkingDirectory();
  bool execute();
  
private:
  std::string configInput;
  std::vector<std::string> pluginNames;
  std::vector<std::unique_ptr<PluginBase>> plugins;
  bool submitCondor;
};

#endif // FRAMEWORK_H
