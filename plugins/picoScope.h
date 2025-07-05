#ifndef PICOSCOPE_H
#define PICOSCOPE_H

#include "PluginBase.h"
#include "PluginFactory.h"

#include <set>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include "rapidcsv.h"
#include "TTree.h"
#include "TFile.h"
#include "TString.h"
#include "TMath.h"
#include <filesystem>
#include "TSystem.h"

using namespace std;
namespace fs = std::filesystem;

struct channel_stat{
  double max_v;
  double max_t;
  double min_v;
  double min_t;
  double mean;
  double RMS;
};

class picoScope : public PluginBase {
public:
  bool initialize(const string &paramsFile) override;
  bool process() override;
  bool finalize() override;
  void loader(int rate);
  string clean_filename(const string& filename);
  string copy_and_rename_file(const fs::path& file_path, const fs::path& output_dir);
  fs::path get_parent_folder(const fs::path& file_path, int parent_level);
  string rename_file(const fs::path& file_path);
  string format_filename(const std::string& input);
  void setupChannelBranches(const std::string& channelName, TTree* tree, std::vector<double>* data, channel_stat* stats, std::vector<std::string>& colTitles);
  
private:
  std::string prefix;
  Config config;
  
  std::ifstream iDataSets;
  std::vector<std::string> dataSets;
  std::vector<std::string> subfolder;
  
};

inline void picoScope::loader(int rate){
  
  char proc[22];
  memset(proc, '\0', sizeof(proc));
  
  for (int i = 0; i < rate/5; i++)
    {
      proc[i] = '#';
    }
  
  printf("\r[%-20s] [%d%%]", proc, rate);        //C语言格式控制时默认右对齐，所以要在前面加-变成左对齐                                                                                     
  fflush(stdout);                                 //刷新屏幕打印               
  
}

inline string picoScope::clean_filename(const string& filename) {
  string cleaned = filename;
  
  // Replace whitespace with underscore
  replace(cleaned.begin(), cleaned.end(), ' ', '_');
  
  // Remove parentheses
  cleaned.erase(remove(cleaned.begin(), cleaned.end(), '('), cleaned.end());
  cleaned.erase(remove(cleaned.begin(), cleaned.end(), ')'), cleaned.end());
  
  return cleaned;
}

inline std::string picoScope::copy_and_rename_file(const fs::path& file_path, const fs::path& output_dir) {
    // Get cleaned filename
    std::string cleaned_name = clean_filename(file_path.filename().string());
    fs::path new_path = output_dir / cleaned_name;

    // Early return if source and destination paths are identical
    if (fs::equivalent(file_path, new_path)) {
        return new_path.string();
    }

    try {
        // Create output directory if needed
        fs::create_directories(output_dir);

        // Only copy if paths are different
        fs::copy_file(file_path, new_path, fs::copy_options::overwrite_existing);
        
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Error processing " << file_path << ": " << e.what() << std::endl;
        // Return empty string on failure or consider throwing
        return "";
    }

    return new_path.string();
}

inline fs::path picoScope::get_parent_folder(const fs::path& file_path, int parent_level = 2) {
  
    // Validate parent level argument
    if (parent_level < 1 || parent_level > 2) {
        throw std::invalid_argument("parent_level must be 1 or 2");
    }

    // Get first level parent
    if (!file_path.has_parent_path()) {
        throw std::runtime_error("Path doesn't have even one parent level");
    }
    fs::path parent1 = file_path.parent_path();

    // Return if only first level requested
    if (parent_level == 1) {
        return parent1;
    }

    // Get second level parent if requested
    if (!parent1.has_parent_path()) {
        throw std::runtime_error("Path doesn't have two parent levels");
    }
    return parent1.parent_path();
}

inline string picoScope::rename_file(const fs::path& file_path) {
  
  if (!fs::exists(file_path)) {
    cerr << "Error: File does not exist - " << file_path << endl;
    return file_path.string();
  }
  
  fs::path new_path(clean_filename(file_path.string()));
  
  // Early exit if no renaming is needed
  if (file_path == new_path) {
    return file_path.string();
  }
  
  try {
    // Case 1: Only filename changes (same parent directory)
    if (file_path.parent_path() == new_path.parent_path()) {
      fs::rename(file_path, new_path);
    }
    // Case 2: Parent directory also needs renaming
    else {
      // Step 1: Ensure target directory exists
      fs::create_directories(new_path.parent_path());
      
      // Step 2: Move the file
      fs::rename(file_path, new_path);
      
      // Step 3: Remove old directory if empty
      if (fs::is_empty(file_path.parent_path())) {
	fs::remove(file_path.parent_path());
      }
    }
  } catch (const fs::filesystem_error& e) {
    cerr << "Error renaming " << file_path << ": " << e.what() << endl;
  }

  return new_path.string();
}


inline std::string picoScope::format_filename(const std::string& input) {
  
  // Find the position of the last underscore
  size_t underscore_pos = input.rfind('_');
  
  // Extract the base part before the underscore
  std::string base = input.substr(0, underscore_pos);
  
  // Extract the number after the underscore
  int num = std::stoi(input.substr(underscore_pos + 1));
  
  // Format the number with leading zeros (4 digits in total)
  std::ostringstream formatted_number;
  formatted_number << std::setw(4) << std::setfill('0') << num;
  
  // Create the final filename with the .root extension
  return base + "_" + formatted_number.str() + ".root";
}

void picoScope::setupChannelBranches(const std::string& channelName, 
			  TTree* tree,
			  std::vector<double>* data,
			  channel_stat* stats,
			  std::vector<std::string>& colTitles) {
  std::string prefix = "Ch" + std::string(1, channelName.back()) + "1_";
  
  tree->Branch((prefix + "V").c_str(), data);
  tree->Branch((prefix + "max_v").c_str(), &stats->max_v, (prefix + "max_v/D").c_str());
  tree->Branch((prefix + "max_t").c_str(), &stats->max_t, (prefix + "max_t/D").c_str());
  tree->Branch((prefix + "min_v").c_str(), &stats->min_v, (prefix + "min_v/D").c_str());
  tree->Branch((prefix + "min_t").c_str(), &stats->min_t, (prefix + "min_t/D").c_str());
  tree->Branch((prefix + "mean").c_str(), &stats->mean, (prefix + "mean/D").c_str());
  tree->Branch((prefix + "RMS").c_str(), &stats->RMS, (prefix + "RMS/D").c_str());
  colTitles.push_back(channelName);
}

// Apply the registration macro
REGISTER_PLUGIN("picoScope", picoScope)

#endif
