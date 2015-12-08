#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <random>
#include "stdlib.h"

#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"

using namespace std;
using namespace ROOT;

double getSumLS(TFile* fin, const char* treename){
  TTree* tree = (TTree* )fin->Get(treename);
  double sumLS = 0;
  float LS;
  tree->SetBranchStatus("*", 0);
  tree->SetBranchStatus("SingleTop_1__BJet_1__Pt", 1);
  tree->SetBranchAddress("SingleTop_1__BJet_1__Pt", &LS);
  for (int eid = 0; eid < tree->GetEntries(); eid++){
    tree->GetEntry(eid);
    sumLS += LS;
  } 
  return sumLS;
}

int main(int argv, char **argc){
  string finname  = argc[1];
  float trainperc = atof(argc[2]);
  cout << "Split ratio is " << trainperc << " / "<< 1 - trainperc << endl;
  cout << "Opening file " << finname << endl;
  TFile *fin = new TFile(finname.c_str());
  vector<TFile *> outfiles;
  outfiles.push_back(new TFile((finname + ".0").c_str(), "RECREATE"));
  outfiles.push_back(new TFile((finname + ".1").c_str(), "RECREATE"));

  TList * dirs = fin->GetListOfKeys();

  random_device _randomDevice;
  mt19937 _generator (_randomDevice());
  uniform_real_distribution<> _uniformDist (0, 1);

  for (size_t i = 0; i <  dirs->GetEntries(); i++){
    TTree *t = (TTree*)dirs->At(i);
    t = (TTree*)fin->Get(t->GetName());
    t->SetBranchStatus("*",1);
    cout << "Found TTree " << t->GetName() << " " << t << endl;
    long tentries = t->GetEntries();
    cout << "  Entries: " << tentries << endl;
    long estart = 0, eentries;
    outfiles[0]->cd();
    TTree* newt0 = t->CloneTree(0);
    outfiles[1]->cd();
    TTree* newt1 = t->CloneTree(0);
    for (int eidx = 0; eidx < tentries; eidx++){
      t->GetEntry(eidx);
      if (_uniformDist(_generator) < trainperc){
        newt0->Fill();
      } else {
        newt1->Fill();
      }
    }
    newt0->Write();
    newt1->Write();
    newt0 = 0;
    newt1 = 0;
  }

  // Verifying
  cout << "Verifying by sum of SingleTop_1__BJet_1__Pt branch values ..." << endl;
  for (size_t i = 0; i <  dirs->GetEntries(); i++){
    TTree *t = (TTree*)dirs->At(i);
    t = (TTree*)fin->Get(t->GetName());
    double sumLS = 0;
    int trueEntries = t->GetEntries();
    int sumEntries = 0;
    double insumLS = getSumLS(fin, t->GetName());
    cout << "Sum of 'SingleTop_1__BJet_1__Pt' in tree " << t->GetName() << " = " << insumLS  << endl; 
    for (int fid = 0; fid < outfiles.size(); fid++){
      double LS = getSumLS(outfiles[fid], t->GetName());
      cout << "  File: ." << fid << "| Sum of 'SingleTop_1__BJet_1__Pt' in tree " << t->GetName() << " = " << LS << endl; 
      sumLS += LS;
      sumEntries += ((TTree*)outfiles[fid]->Get(t->GetName()))->GetEntries();
    }
    if (sumLS == insumLS and trueEntries == sumEntries){
      cout << "==> OK. Sum of SingleTop_1__BJet_1__Pt is equal." << endl;
      cout << "==> Entries " << trueEntries << " == " << sumEntries << endl;
    } else{
      cout << "==> Something wrong. " << insumLS << " != " << sumLS << endl;
      cout << "==> Entries " << trueEntries << " != " << sumEntries << endl;
      cout << "Exiting ..." << endl;
      fin->Close();
      for (size_t i = 0; i < outfiles.size(); i++)  outfiles[i]->Close();
      return 1;
    }
  }
  
  fin->Close();
  for (size_t i = 0; i < outfiles.size(); i++)  outfiles[i]->Close();
  return 0;
}
