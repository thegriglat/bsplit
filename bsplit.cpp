#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
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
  std::sort(list.begin(), list.end());
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
    cout << "Split ratio is " << splitting.at(i) << endl;
  cout << "Opening file " << finname << endl;
  TFile *fin = new TFile(finname.c_str());
  vector<TFile *> outfiles;
  for (size_t i = 0; i < splitting.size(); i++)
    outfiles.push_back(new TFile((finname + "." + i), "RECREATE"));
  /* end */
//  vector<TTree *> trees;
  TList * dirs = fin->GetListOfKeys();
  for (size_t i = 0; i <  dirs->GetEntries(); i++){
    TTree *t = (TTree*)dirs->At(i);
    t = (TTree*)fin->Get(t->GetName());
    t->SetBranchStatus("*",1);
    cout << "Found TTree " << t->GetName() << " " << t << endl;
    //trees.push_back(t);
    long tentries = t->GetEntries();
    cout << "  Entries: " << tentries << endl;
    long estart = 0, eentries;
    for (int fidx = 0; fidx < outfiles.size(); fidx ++){
      if (fidx == outfiles.size() - 1)
        eentries = tentries - estart;
      else
        eentries = splitting.at(fidx) * tentries;
      outfiles[fidx]->cd();
      TTree* newt = t->CopyTree("", "", eentries, estart);
      cout << "  " << splitting[fidx] << "% | Tree structure cloned to " << finname  <<  "." << fidx << endl;
      cout << "  " << eentries << " entries copied" << endl;
      newt->AutoSave();
      newt->Write();
      newt = 0;
      estart += eentries;
     } 
  }
  fin->Close();
  for (size_t i = 0; i < outfiles.size(); i++)
    outfiles[i]->Close();
  return 0;
}
