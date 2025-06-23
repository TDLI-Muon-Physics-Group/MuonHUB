//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Mon Mar  6 11:44:00 2023 by ROOT version 6.24/06
// from TTree mppc/mppc
// found on file: mppc_4scint2sipm.root
//////////////////////////////////////////////////////////

#ifndef mppc_h
#define mppc_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.

class mppc {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   UChar_t         mac5;
   UShort_t        chg[32];
   UInt_t          ts0;
   UInt_t          ts1;
   UInt_t          ts0_ref;
   UInt_t          ts1_ref;

   // List of branches
   TBranch        *b_mac5;   //!
   TBranch        *b_chg;   //!
   TBranch        *b_ts0;   //!
   TBranch        *b_ts1;   //!
   TBranch        *b_ts0_ref;   //!
   TBranch        *b_ts1_ref;   //!

   mppc(TTree *tree=0);
   virtual ~mppc();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef mppc_cxx
mppc::mppc(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("mppc_4scint2sipm.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("mppc_4scint2sipm.root");
      }
      f->GetObject("mppc",tree);

   }
   Init(tree);
}

mppc::~mppc()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t mppc::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t mppc::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void mppc::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("mac5", &mac5, &b_mac5);
   fChain->SetBranchAddress("chg", chg, &b_chg);
   fChain->SetBranchAddress("ts0", &ts0, &b_ts0);
   fChain->SetBranchAddress("ts1", &ts1, &b_ts1);
   fChain->SetBranchAddress("ts0_ref", &ts0_ref, &b_ts0_ref);
   fChain->SetBranchAddress("ts1_ref", &ts1_ref, &b_ts1_ref);
   Notify();
}

Bool_t mppc::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void mppc::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t mppc::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef mppc_cxx
