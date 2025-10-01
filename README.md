# MuonHUB
Hub contains codes used for converting raw data format into analysis-level format.

## Convertors
- [x] DT5702 : https://github.com/TwinklyStar/DT5702PreAna-notime
- DRS4 : https://github.com/TwinklyStar/DRS4bin2root
- [x] Picoscope : https://github.com/TwinklyStar/picoCSV2root

## How do i use it (demonstration)

### MuonHUB

The framework (MuonHUB) loads the module(convertor DT5702, DRS4, Picoscop), reads the cfg file, and perform conversion.

Meaning:
1. one need to prepare the filelist pointing to the datasets folder
2. specify parameters (convertor specific)
3. run teh framework , by specify the module , and the location of the cfg file.

The framework organize as follow:

- ```src``` contain source code for framework.
- ```plugins``` contain source code for each module.
- ```configs``` contain cfg files for each module.
- ```data``` contain auxillary files or filelist necessary for each module.
- ```misc``` contain helper source code.
- ```scripts``` contain pre-process step (if any) for specific daq raw files.
- ```test``` contain files relevant for test run.

To install, simply
```
git clone https://github.com/TDLI-Muon-Physics-Group/MuonHUB
cd MuonHUB
make
```

### DT5702

1. DT5702 is the CAEN DAQ product.
2. Raw root files are ouput by MULTIFEB.

```
./build/uconvert -c configs/template_dt5702.cfg -m DT5702
```

### DRS4

1. DRS4 is the evaluation board used by PSI
2. Raw root files are ouput by DRS4 GUI OSC


```
./build/uconvert -c configs/template_drs4.cfg -m DRS4
```

### picoScope

1. picoScope
2. Each CSV file contains waveform

```
./build/uconvert -c configs/template_picoscope.cfg -m picoScope
```