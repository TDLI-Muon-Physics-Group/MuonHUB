#include "DRS4.h"

bool DRS4::initialize(const std::string &paramsFile) {

  //
  prefix = "DRS4_PostProc";
    
  // load param
  config = loadParams(paramsFile);
  
  std::cout << " Parameters read from: "<< paramsFile.c_str() << std::endl;
  std::cout << " dataset : " << config.dataset << std::endl;
  
  if (isTxt(config.dataset)){

    std::cout << " filelist detected : " << config.dataset << std::endl;
    
    // load full list of dataset
    iDataSets.open(config.dataset);
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
    std::cout << " single file detected : " << config.dataset << std::endl;
    dataSets.push_back(config.dataset);
  }
  
  return true; 
}

bool DRS4::process() {

  // loop on multiple files
  // set should be full path directory, and the file name
  for(auto set : dataSets){

    ifstream file;

    file.open (set, ios::in | ios::binary);
    cout << ">> Opening file " << set << " ......" << endl;

    // terminate if the file can't be opened
    if (!file.is_open()){
      cerr << "!! File open error:" << set << endl;
      return false;
    }
    
    // convert fill path into string
    string infname(set);
    fs::path path(infname);
    
    // the file name
    string outfname = path.filename();
    int file_len = strlen(outfname.c_str());
    outfname.replace (file_len - 3, 3, "root");
    outfname = prefix + "_" + outfname;
    
    // residing absolute path
    string directory = path.parent_path();

    // prepare output folder
    std::string newFolder = directory + "/" + prefix +"/";
    if(!dirExists(newFolder.c_str()))
      gSystem->mkdir(newFolder.c_str(), kTRUE);

    cout << ">> Start reading file" << set << " ......" << endl;
    cout << endl;

    // out file
    //TFile *treefile = new TFile( TString(newFolder + outfname) , "recreate" );
    std::unique_ptr<TFile> treefile( new TFile( TString( newFolder + outfname ) , "recreate" ) );
    if (!treefile || treefile->IsZombie()) {
      cerr << "!! Failed to create ROOT file." << endl;
      return false;
    }
    
    cout << ">> Creating rootfile " << set << " ......" << endl;
    cout << endl;
    TTree *tree = new TTree ("wfm", "A tree storing waveform data from DRS4");
    tree->Branch("EvtNum", &nevent, "EvtNum/i");
    tree->Branch("Data_Length", &data_length, "Data_Length/I");
    tree->Branch("EvtTime", &time_manager, "year/S:month/S:day/S:hour/S:min/S:sec/S:ms/S");

    // Read file header                                                                                                                                                                       
    file.read ((char *) &FileHeader, 4);
    FileHeader[4] = '\0';

    cout<<"File header: "<<FileHeader<<endl;

    // Read time header                                                                                                                                                                       
    file.read ((char *) &TimeHeader, 4);
    TimeHeader[4] = '\0';

    cout<<"Time header: "<<TimeHeader<<endl;

    // Read board serial number                                                                                                                                                               
    file.read ((char *) &BoardSerialNumber, 2);
    BoardSerialNumber[2] = '\0';
    file.read ((char *) &number, 2);

    cout<<"Board serial number: "<<BoardSerialNumber << number << endl;

    bool end_of_timeinfo = false;
    int bd_itr=0;   // Board iterator                                                                                                                                                         
    std::vector<short> channel_seq; // Channel iterator                                                                                                                                       

    // Header matching template                                                                                                                                                               
    std::regex bd_serial_template("B#..");
    std::regex ch_header_template("C00.");

    // Identify channel number and create branches
    std::cout << ">> Starting identifying channel and board number..." << std::endl;
    while (! end_of_timeinfo) {

      // Read channel header
      file.read((char*)&ChannelHeader, 4);
      ChannelHeader[4] = '\0';

      if (std::regex_match(ChannelHeader, ch_header_template)) {  // Matching the channel header                                                                                            
	if (strcmp(ChannelHeader, "C001") == 0) {
	  if (bd_itr == 0) {
	    tree->Branch("ChA1_T", &chT_manager[0]);
	    tree->Branch("ChA1_V", &chV_manager[0]);
	    tree->Branch("ChA1_max_v", &chstat_manager[0].max_v, "ChA1_max_v/D");
	    tree->Branch("ChA1_max_t", &chstat_manager[0].max_t, "ChA1_max_t/D");
	    tree->Branch("ChA1_min_v", &chstat_manager[0].min_v, "ChA1_min_v/D");
	    tree->Branch("ChA1_min_t", &chstat_manager[0].min_t, "ChA1_min_t/D");
	    tree->Branch("ChA1_mean",  &chstat_manager[0].mean,  "ChA1_mean/D");
	    tree->Branch("ChA1_RMS",   &chstat_manager[0].RMS,   "ChA1_RMS/D");
	    
	    // Read event times                                                                                                                                                       
	    file.read((char *) &EventTime, 4096);
	    Double_t time_last = 0;
	    for (int i = 0; i < 1024; i++) {
	      chT_manager[0].push_back((Double_t)EventTime[i] + time_last);
	      time_last = chT_manager[0].back();
	    }
	    channel_seq.push_back(0);
	  } else if (bd_itr == 1) {
	    tree->Branch("ChA2_T", &chT_manager[4]);
	    tree->Branch("ChA2_V", &chV_manager[4]);
	    tree->Branch("ChA2_max_v", &chstat_manager[4].max_v, "ChA2_max_v/D");
	    tree->Branch("ChA2_max_t", &chstat_manager[4].max_t, "ChA2_max_t/D");
	    tree->Branch("ChA2_min_v", &chstat_manager[4].min_v, "ChA2_min_v/D");
	    tree->Branch("ChA2_min_t", &chstat_manager[4].min_t, "ChA2_min_t/D");
	    tree->Branch("ChA2_mean",  &chstat_manager[4].mean,  "ChA2_mean/D");
	    tree->Branch("ChA2_RMS",   &chstat_manager[4].RMS,   "ChA2_RMS/D");
	    
	    // Read event times                                                                                                                                                       
	    file.read((char *) &EventTime, 4096);
	    Double_t time_last = 0;
	    for (int i = 0; i < 1024; i++) {
	      chT_manager[4].push_back((Double_t)EventTime[i] + time_last);
	      time_last = chT_manager[4].back();
	    }
	    channel_seq.push_back(4);
	  } else {
	    std::cerr << "ERROR(TimeInfo): More than two DRS4 boards!!!" << std::endl;
	    return false;
	  }
	}
	if (strcmp(ChannelHeader, "C002") == 0) {
	  if (bd_itr == 0) {
	    tree->Branch("ChB1_T", &chT_manager[1]);
	    tree->Branch("ChB1_V", &chV_manager[1]);
	    tree->Branch("ChB1_max_v", &chstat_manager[1].max_v, "ChB1_max_v/D");
	    tree->Branch("ChB1_max_t", &chstat_manager[1].max_t, "ChB1_max_t/D");
	    tree->Branch("ChB1_min_v", &chstat_manager[1].min_v, "ChB1_min_v/D");
	    tree->Branch("ChB1_min_t", &chstat_manager[1].min_t, "ChB1_min_t/D");
	    tree->Branch("ChB1_mean",  &chstat_manager[1].mean,  "ChB1_mean/D");
	    tree->Branch("ChB1_RMS",   &chstat_manager[1].RMS,   "ChB1_RMS/D");
	    
	    // Read event times                                                                                                                                                       
	    file.read((char *) &EventTime, 4096);
	    Double_t time_last = 0;
	    for (int i = 0; i < 1024; i++) {
	      chT_manager[1].push_back((Double_t)EventTime[i] + time_last);
	      time_last = chT_manager[1].back();
	    }
	    channel_seq.push_back(1);
	  } else if (bd_itr == 1) {
	    tree->Branch("ChB2_T", &chT_manager[5]);
	    tree->Branch("ChB2_V", &chV_manager[5]);
	    tree->Branch("ChB2_max_v", &chstat_manager[5].max_v, "ChB2_max_v/D");
	    tree->Branch("ChB2_max_t", &chstat_manager[5].max_t, "ChB2_max_t/D");
	    tree->Branch("ChB2_min_v", &chstat_manager[5].min_v, "ChB2_min_v/D");
	    tree->Branch("ChB2_min_t", &chstat_manager[5].min_t, "ChB2_min_t/D");
	    tree->Branch("ChB2_mean",  &chstat_manager[5].mean,  "ChB2_mean/D");
	    tree->Branch("ChB2_RMS",   &chstat_manager[5].RMS,   "ChB2_RMS/D");
	    
	    // Read event times                                                                                                                                                       
	    file.read((char *) &EventTime, 4096);
	    Double_t time_last = 0;
	    for (int i = 0; i < 1024; i++) {
	      chT_manager[5].push_back((Double_t)EventTime[i] + time_last);
	      time_last = chT_manager[5].back();
	    }
	    channel_seq.push_back(5);
	  } else {
	    std::cerr << "ERROR(TimeInfo): More than two DRS4 boards!!!" << std::endl;
	    return false;
	  }
	}
	if (strcmp(ChannelHeader, "C003") == 0) {
	  if (bd_itr == 0) {
	    tree->Branch("ChC1_T", &chT_manager[2]);
	    tree->Branch("ChC1_V", &chV_manager[2]);
	    tree->Branch("ChC1_max_v", &chstat_manager[2].max_v, "ChC1_max_v/D");
	    tree->Branch("ChC1_max_t", &chstat_manager[2].max_t, "ChC1_max_t/D");
	    tree->Branch("ChC1_min_v", &chstat_manager[2].min_v, "ChC1_min_v/D");
	    tree->Branch("ChC1_min_t", &chstat_manager[2].min_t, "ChC1_min_t/D");
	    tree->Branch("ChC1_mean",  &chstat_manager[2].mean,  "ChC1_mean/D");
	    tree->Branch("ChC1_RMS",   &chstat_manager[2].RMS,   "ChC1_RMS/D");
	    
	    // Read event times                                                                                                                                                       
	    file.read((char *) &EventTime, 4096);
	    Double_t time_last = 0;
	    for (int i = 0; i < 1024; i++) {
	      chT_manager[2].push_back((Double_t)EventTime[i] + time_last);
	      time_last = chT_manager[2].back();
	    }
	    channel_seq.push_back(2);
	  } else if (bd_itr == 1) {
	    tree->Branch("ChC2_T", &chT_manager[6]);
	    tree->Branch("ChC2_V", &chV_manager[6]);
	    tree->Branch("ChC2_max_v", &chstat_manager[6].max_v, "ChC2_max_v/D");
	    tree->Branch("ChC2_max_t", &chstat_manager[6].max_t, "ChC2_max_t/D");
	    tree->Branch("ChC2_min_v", &chstat_manager[6].min_v, "ChC2_min_v/D");
	    tree->Branch("ChC2_min_t", &chstat_manager[6].min_t, "ChC2_min_t/D");
	    tree->Branch("ChC2_mean",  &chstat_manager[6].mean,  "ChC2_mean/D");
	    tree->Branch("ChC2_RMS",   &chstat_manager[6].RMS,   "ChC2_RMS/D");
	    
	    // Read event times                                                                                                                                                       
	    file.read((char *) &EventTime, 4096);
	    Double_t time_last = 0;
	    for (int i = 0; i < 1024; i++) {
	      chT_manager[6].push_back((Double_t)EventTime[i] + time_last);
	      time_last = chT_manager[6].back();
	    }
	    channel_seq.push_back(6);
	  } else {
	    std::cerr << "ERROR(TimeInfo): More than two DRS4 boards!!!" << std::endl;
	    return false;
	  }
	}
	if (strcmp(ChannelHeader, "C004") == 0) {
	  if (bd_itr == 0) {
	    tree->Branch("ChD1_T", &chT_manager[3]);
	    tree->Branch("ChD1_V", &chV_manager[3]);
	    tree->Branch("ChD1_max_v", &chstat_manager[3].max_v, "ChD1_max_v/D");
	    tree->Branch("ChD1_max_t", &chstat_manager[3].max_t, "ChD1_max_t/D");
	    tree->Branch("ChD1_min_v", &chstat_manager[3].min_v, "ChD1_min_v/D");
	    tree->Branch("ChD1_min_t", &chstat_manager[3].min_t, "ChD1_min_t/D");
	    tree->Branch("ChD1_mean",  &chstat_manager[3].mean,  "ChD1_mean/D");
	    tree->Branch("ChD1_RMS",   &chstat_manager[3].RMS,   "ChD1_RMS/D");
	    
	    // Read event times                                                                                                                                                       
	    file.read((char *) &EventTime, 4096);
	    Double_t time_last = 0;
	    for (int i = 0; i < 1024; i++) {
	      chT_manager[3].push_back((Double_t)EventTime[i] + time_last);
	      time_last = chT_manager[3].back();
	    }
	    channel_seq.push_back(3);
	  } else if (bd_itr == 1) {
	    tree->Branch("ChD2_T", &chT_manager[7]);
	    tree->Branch("ChD2_V", &chV_manager[7]);
	    tree->Branch("ChD2_max_v", &chstat_manager[7].max_v, "ChD2_max_v/D");
	    tree->Branch("ChD2_max_t", &chstat_manager[7].max_t, "ChD2_max_t/D");
	    tree->Branch("ChD2_min_v", &chstat_manager[7].min_v, "ChD2_min_v/D");
	    tree->Branch("ChD2_min_t", &chstat_manager[7].min_t, "ChD2_min_t/D");
	    tree->Branch("ChD2_mean",  &chstat_manager[7].mean,  "ChD2_mean/D");
	    tree->Branch("ChD2_RMS",   &chstat_manager[7].RMS,   "ChD2_RMS/D");
	    
	    // Read event times                                                                                                                                                       
	    file.read((char *) &EventTime, 4096);
	    Double_t time_last = 0;
	    for (int i = 0; i < 1024; i++) {
	      chT_manager[7].push_back((Double_t)EventTime[i] + time_last);
	      time_last = chT_manager[7].back();
	    }
	    channel_seq.push_back(7);
	  } else {
	    std::cerr << "ERROR(TimeInfo): More than two DRS4 boards!!!" << std::endl;
	    return false;
	  }
	}
      }
      else if (std::regex_match(ChannelHeader, bd_serial_template)) bd_itr++;
      else if (strcmp(ChannelHeader, "EHDR") == 0){   // End of time info part                                                                                                              
	end_of_timeinfo = true;
      }
      else {
	std::cerr << "ERROR(TimeInfo): Unexpected Channel Header!!!" << std::endl;
	std::cout << "Current Channel Header: " << ChannelHeader << std::endl;
	return false;
      }
    }
    std::cout << "DRS4 board number: " << bd_itr+1 << std::endl << "Channel number: " << channel_seq.size() << std::endl << "Activated channels: ";
    for (auto ch_num : channel_seq) std::cout << ch_num+1 << " ";
    std::cout << std::endl;
    if (channel_seq.at(0) >= 5){
      std::cerr << "ERROR: No channel in the first board is active!!!" << std::endl;
      return false;
    }
    std::cout << "First Event Header: " << ChannelHeader << std::endl;

    //
    while (!endoffile){ // event loop                                                                                                                                                         
      
      // Count Event, show the progress every 1000 events                                                                                                                           
      if (nevent % 100000 == 0){
	time (&realtime);
	cout << ">> Processing event No." << nevent << ", Time elapsed : " <<
	  (double) (clock() - start) / CLOCKS_PER_SEC << " secs, Current time : " << ctime (&realtime) << endl;
	//start = clock();                                                                                                                                                    
      }
      
      if (nevent>0){  // not the first event header                                                                                                                                         
	// Read event header                                                                                                                                                              
	file.read ((char *) &EventHeader, 4);
	EventHeader[4] = '\0';
	if (strcmp(EventHeader, "EHDR") != 0) {
	  cerr << "ERROR: Wrong event header in event " << nevent << endl;
	  return false;
	}
	if (verbose >= 1 && nevent % 1000 == 1) cout << nevent << " th Event header: " << EventHeader << endl;
      }
      
      // Read event serial number                                                                                                                                                   
      file.read ((char *) &SerialNumber, 4);
      
      if (verbose >= 1) cout<<"Event serial number: "<<SerialNumber<<endl;
      
      // Read date (YY/MM/DD/HH/mm/ss/ms/rr)                                                                                                                                        
      file.read ((char *) &Date, 16);
      
      time_manager.year = Date[0];
      time_manager.month = Date[1];
      time_manager.day = Date[2];
      time_manager.hour = Date[3];
      time_manager.min = Date[4];
      time_manager.sec = Date[5];
      time_manager.ms = Date[6];

      // Start channel looping in a single event
      short last_ch_num = -1;
      
      for (auto current_ch_num : channel_seq){
	if ((last_ch_num < 0 && current_ch_num >= 0) || (last_ch_num < 4 && current_ch_num >= 4)){  // First channel of a board                                                           
	  // Read board serial number                                                                                                                                                   
	  file.read ((char *) &BoardSerialNumber, 2);
	  BoardSerialNumber[2] = '\0';
	  file.read((char *) &number, 2);
	  
	  if (verbose >= 1) cout<<"Board serial number: "<<BoardSerialNumber << number << endl;
	  
	  // Read trigger cell number                                                                                                                                                   
	  file.read ((char *) &TriggerCellNumber, 2);
	  file.read((char *) &number, 2);
	  TriggerCellNumber[2] = '\0';
	  
	  if (verbose >= 1) cout<<"Trigger cell number: "<<TriggerCellNumber << number <<endl;
	}
	last_ch_num = current_ch_num;
	
	file.read ((char *) &ChannelHeader, 4);
	ChannelHeader[4] = '\0';
	
	if (verbose >= 1) cout << "Channel Header : " << ChannelHeader << endl;
	
	// Read scaler number                                                                                                                                                             
	file.read ((char *) &ScalerNumber, 4);
	
	if (verbose >= 1) cout<<"Scaler number: "<<ScalerNumber<<endl;
	
	// get amplitude of each channel                                                                                                                                                  
	file.read ((char *) &ChannelDataRaw, 2048);
	
	// apply 5-sample average of waveform smoothing                                                                                                                                   
	// this will decrease the resolution, but ok for very noisy daq                                                                                                                   
	ChannelData[0] = ChannelDataRaw[0];
	ChannelData[1] = ChannelDataRaw[1];
	ChannelData[1022] = ChannelDataRaw[1022];
	ChannelData[1023] = ChannelDataRaw[1023];
	
	for (int i = 2; i < 1022; i++){
	  ChannelData[i] = (ChannelDataRaw[i - 2] + ChannelDataRaw[i - 1] + ChannelDataRaw[i] + ChannelDataRaw[i + 1] + ChannelDataRaw[i + 2]) / 5;
	}
	
	// Unit conversion: 0 -> -500, 65535 -> 500                                                                                                                                       
	for (int i=0; i<1024; i++){
	  chV_manager[current_ch_num].push_back(ChannelData[i] * (1000.0 / 65535.0) + (Date[7] - 500));
	}
	
	// Find Max and Min of the Channel data (Voltage)
	auto itr_min = TMath::LocMin (chV_manager[current_ch_num].begin(), chV_manager[current_ch_num].end());      // return index of the min                                            
	chstat_manager[current_ch_num].min_v = *itr_min;    // return value of the vmin                                                                                                   
	int idx_min = std::distance(chV_manager[current_ch_num].begin(), itr_min);
	chstat_manager[current_ch_num].min_t = chT_manager[current_ch_num].at(idx_min);     // return value of the tmin                                                                   
	
	auto itr_max = TMath::LocMax (chV_manager[current_ch_num].begin(), chV_manager[current_ch_num].end());      // return index of the max                                            
	chstat_manager[current_ch_num].max_v = *itr_max;    // return value of the vmax                                                                                                   
	int idx_max = std::distance(chV_manager[current_ch_num].begin(), itr_max);
	chstat_manager[current_ch_num].max_t = chT_manager[current_ch_num].at(idx_max);     // return value of the tmax                                                                   
	
	// Calculate the mean and RMS of each channel                                                                                                                                     
	chstat_manager[current_ch_num].mean = TMath::Mean(chV_manager[current_ch_num].begin(), chV_manager[current_ch_num].end());
	chstat_manager[current_ch_num].RMS  = TMath::RMS(chV_manager[current_ch_num].begin(), chV_manager[current_ch_num].end());
	
      }
      
      tree->Fill();    
      ++nevent;
      
      for (auto current_ch_num : channel_seq){
	chV_manager[current_ch_num].clear();
	chV_manager[current_ch_num].shrink_to_fit();
      }
      if (file.eof()){
	cout << ">> Reach End of the file .... " << endl;
	cout << ">> Total event number: " << nevent << endl;
	endoffile = true;
	break;
      }
    } // end of event loop                                                                                                                                                                    
    
    cout << "The tree was saved." << endl;
    cout << "Last Event Header: " << EventHeader << endl;

    tree->Write("", TObject::kOverwrite); // Force overwrite
    treefile->Write("", TObject::kOverwrite); // Explicitly write the file
    treefile->Close(); // Properly close
    
    cout << "The treefile was saved." << endl;
    cout << "Time elapsed : " <<
      (double) (clock() - start) / CLOCKS_PER_SEC << " secs, Current time : " << ctime (&realtime) << endl;
    
    file.close();
    
  } // end of set loop
  
  return true;
}

bool DRS4::finalize() {
  
  std::cout << "test module completed successfully." << std::endl;
  return true;
}
