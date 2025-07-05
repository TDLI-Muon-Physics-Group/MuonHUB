#include "picoScope.h"

bool picoScope::initialize(const std::string &paramsFile) {

  //
  prefix = "picoScope_PostProc";
    
  // load param
  config = loadParams(paramsFile);
  
  std::cout << " Parameters read from: "<< paramsFile.c_str() << std::endl;
  std::cout << " dataset : " << config.dataset << std::endl;
  std::cout << " makeRenameCopy : " << config.makeRenameCopy << std::endl;
 
  // 
  if (isTxt(config.dataset)){
    
    std::cout << " filelist detected : " << config.dataset << std::endl;

    std::ifstream input_file(config.dataset);
    if (!input_file.is_open()) {
      std::cerr << "Failed to open "<< config.dataset << std::endl;
      return 1;
    }
    
    // rename
    std::string line;
    bool firstfile=false;
    string outfolder;
    fs::path containing_folder;
    
    while (std::getline(input_file, line)) {
      if (!line.empty()) {
	
	fs::path file_path(line);
		
	if (fs::exists(file_path)) {
	  if (fs::is_regular_file(file_path)) {

	    // two levels
	    if (!firstfile) {
	      containing_folder = get_parent_folder(file_path);
	      firstfile=true;
	    }
	    
	    if (config.makeRenameCopy >= 1){
	      outfolder = containing_folder.string() + "_renamed";
	      
	      // Create output path by adding "renamed_files" to the original directory
	      fs::path parent1 = file_path.parent_path();
	      string parent_lvl1 = clean_filename(parent1.filename().string());
	      
	      fs::path out_path(outfolder);
	      fs::path output_dir = out_path / parent_lvl1;
	      //cout<<"Making new folder containing renamed folder and data files : "<< output_dir.string() << endl;
	      dataSets.push_back( copy_and_rename_file(file_path, output_dir) );
	    }
	    // in place rename
	    else{
	      //cout<<"In place renaming in folder : "<<  get_parent_folder(file_path).string() << endl;
	      string rename_files = rename_file(file_path);
	      dataSets.push_back(rename_files);
	    }
	  } else {
	    cerr << "Path is not a regular file: " << file_path << endl;
	  }
	} else {
	  cerr << "File not found: " << file_path << endl;
	}
      }
    }
    input_file.close();
  }

  // get subfolder vector
  std::set<std::string> unique_subfolders;
  for(auto set : dataSets){
    fs::path path(set);
    unique_subfolders.insert(path.parent_path().string());
  }
  subfolder.assign(unique_subfolders.begin(), unique_subfolders.end());
  
  return true; 
}

bool picoScope::process() {
  // fnum : total file number
  //
  //           subfolder       fnum output filename event number
  //./CSV2root demo/20231228_7 10 6 20231228 (7)_01.csv 50

  int evt_num=0;
  
  // loop on multiple files
  // set should be full path directory, and the file name
  for(auto set : subfolder){
    
    cout<<"Looking at sub-dataset : "<< set << endl;

    fs::path subfolder_path(set);
    string fname = format_filename(subfolder_path.filename().string());
    string outfname = subfolder_path.parent_path().string() + "/" + prefix + "_" + fname;
    cout<<"The outfname format output : "<< outfname << endl;
    
    TFile *ff = new TFile(outfname.c_str(), "RECREATE");
    TTree *tt = new TTree("wfm", "A tree storing waveform data from Picoscope");

    vector<Double_t> time_vec, ChA1_V, ChB1_V, ChC1_V, ChD1_V;
    channel_stat *ChA1_stat = new channel_stat;
    channel_stat *ChB1_stat = new channel_stat;
    channel_stat *ChC1_stat = new channel_stat;
    channel_stat *ChD1_stat = new channel_stat;
    Int_t smp_num;
    
    // This part is the simplification of saving data
    map<string, vector<Double_t>*> data_map;
    map<string, channel_stat*> ch_map;
    data_map.insert(pair<string, vector<Double_t>*> ("Time", &time_vec));
    data_map.insert(pair<string, vector<Double_t>*> ("Channel A", &ChA1_V));
    data_map.insert(pair<string, vector<Double_t>*> ("Channel B", &ChB1_V));
    data_map.insert(pair<string, vector<Double_t>*> ("Channel C", &ChC1_V));
    data_map.insert(pair<string, vector<Double_t>*> ("Channel D", &ChD1_V));
    ch_map.insert(pair<string, channel_stat*> ("Channel A", ChA1_stat));
    ch_map.insert(pair<string, channel_stat*> ("Channel B", ChB1_stat));
    ch_map.insert(pair<string, channel_stat*> ("Channel C", ChC1_stat));
    ch_map.insert(pair<string, channel_stat*> ("Channel D", ChD1_stat));
    vector<string> col_title;

    // gather the csv files reside in the subfolder
    std::vector<std::string> csvFiles;    
    for (const auto& entry : fs::directory_iterator(set)) {
      // only non-directory files
      if (fs::is_regular_file(entry.status())) {
	csvFiles.push_back(entry.path().string());
      }
    }
    // sorted them in ascending
    std::sort(csvFiles.begin(), csvFiles.end());

    
    int fnum = csvFiles.size();
    int idx = 0;
    for (const auto& full_path : csvFiles) {
      idx++;
      std::cout << "Processing: " << full_path << std::endl;
      
      //
      rapidcsv::Document doc(full_path);
      doc.SetStartLineNum(3);    // for CSV from picoscope, the first three rows are not data


      if (idx == 1){  // register branch according to channel number
	tt->Branch("EvtNum", &evt_num, "EvtNum/i");
	tt->Branch("Data_Length", &smp_num, "Data_Length/I");
	tt->Branch("Time", &time_vec);
	col_title.push_back("Time");

	if (doc.GetColumnIdx("Channel A") >= 0) {
	  setupChannelBranches("Channel A", tt, &ChA1_V, ChA1_stat, col_title);
	}
	if (doc.GetColumnIdx("Channel B") >= 0) {
	  setupChannelBranches("Channel B", tt, &ChB1_V, ChB1_stat, col_title);
	}
	if (doc.GetColumnIdx("Channel C") >= 0) {
	  setupChannelBranches("Channel C", tt, &ChC1_V, ChC1_stat, col_title);
	}
	if (evt_num==0) {
	  std::cout << "Branches in output tree:" << std::endl;
	  for (auto str: col_title) std::cout << str << "\t";
	  std::cout << std::endl;
	}
      }

      // progress bar
      if (fnum >= 100 && idx % (fnum/100) == 0) loader(idx/(fnum/100));

      bool if_exception = false;
      for (auto str : col_title){     // iterate valid data (jump unused channels)                                                                                                        
	auto itr = data_map.find(str);
	try {
	  *itr->second = doc.GetColumn<Double_t>(str);
	}
	catch (exception &e){
	  std::cout << "\rInvalid number (e.g. infinity) in: " << full_path << "--" << str << std::endl;
	  if_exception = true;
	}
      }
      smp_num = time_vec.size();
      
      if (!if_exception) {
	// Preliminary data analysis                                                                                                                                                    
	for (auto str: col_title) {
	  auto itr = data_map.find(str);
	  auto itr_max = TMath::LocMax(itr->second->begin(),
				       itr->second->end());     // return vector iterator                                                                                             
	  auto itr_min = TMath::LocMin(itr->second->begin(),
				       itr->second->end());     // return vector iterator                                                                                             
	  int idx_max = std::distance(itr->second->begin(),
				      itr_max);             // vector iterator to index                                                                                               
	  int idx_min = std::distance(itr->second->begin(), itr_min);
	  
	  auto stat_itr = ch_map.find(str);
	  if (stat_itr == ch_map.end()) continue;
	  stat_itr->second->max_v = *itr_max;
	  stat_itr->second->min_v = *itr_min;
	  stat_itr->second->max_t = time_vec.at(idx_max);
	  stat_itr->second->min_t = time_vec.at(idx_min);
	  stat_itr->second->mean = TMath::Mean(itr->second->begin(), itr->second->end());
	  stat_itr->second->RMS = TMath::RMS(itr->second->begin(), itr->second->end());
	}
	tt->Fill();
      }
      evt_num++;
      
      // Release memory                                                                                                                                                                   
      time_vec.clear();
      time_vec.shrink_to_fit();
      ChA1_V.clear();
      ChA1_V.shrink_to_fit();
      ChB1_V.clear();
      ChB1_V.shrink_to_fit();
      ChC1_V.clear();
      ChC1_V.shrink_to_fit();
      ChD1_V.clear();
      ChD1_V.shrink_to_fit();
    } // csv file loop
    
    ff->Write();
    ff->Close();
    delete ChA1_stat;
    delete ChB1_stat;
    delete ChC1_stat;
    delete ChD1_stat;
    std::cout << "\rSave file as: " << outfname << std::endl;
    
  } // end of set loop
  
  return true;
}

bool picoScope::finalize() {
  
  std::cout << "test module completed successfully." << std::endl;
  return true;
}
