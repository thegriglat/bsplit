#include <iostream>
#include <vector>
#include <string>
#include "stdlib.h"

#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"

using namespace std;
using namespace ROOT;

void rebaseSplit(vector<float> &list){
  float sum = 0;
  for (size_t i = 0; i < list.size(); i++) sum += list[i];
  for (size_t i = 0; i < list.size(); i++) list[i] = list[i] / sum;
}

int main(int argv, char **argc){
  string finname  = argc[1];
  /* read splitting */
  string splitstr = argc[2];
  vector<float> splitting;
  string tmp = "";
  for (size_t i = 0; i < splitstr.length(); i++){
    if (splitstr.at(i) == ':'){
      splitting.push_back(atof(tmp.c_str()));
      tmp = "";
      continue;
    }
    tmp += splitstr.at(i);
  }
  splitting.push_back(atof(tmp.c_str()));
  rebaseSplit(splitting);
  for (size_t i = 0; i < splitting.size(); i++)
    cout << "Split block is " << splitting.at(i) << endl;
  cout << "Opening file " << finname << endl;
  TFile *fin = new TFile(finname.c_str());
  vector<TFile *> outfiles;
  for (size_t i = 0; i < splitting.size(); i++)
    outfiles.push_back(new TFile((finname + "." + i), "RECREATE"));
  /* end */
  vector<TTree *> trees;
  TList * dirs = fin->GetListOfKeys();
  cout << dirs->GetEntries() << endl;
  for (size_t i = 0; i <  dirs->GetEntries(); i++){
    TTree *t = (TTree*)dirs->At(i);
    cout << "Found TTree " << t->GetName() << endl;
    trees.push_back(t);
  }
  fin->Close();
  return 0;
}
