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
  cout << dirs->GetEntries() << endl;
  for (size_t i = 0; i <  dirs->GetEntries(); i++){
    TTree *t = (TTree*)dirs->At(i);
    cout << "Found TTree " << t->GetName() << endl;
    //trees.push_back(t);
    int tentries = t->GetEntries();
    string tname = t->GetName();
    TFile *currentfile = outfiles[0];
    size_t currentfile_idx = 0;
    currentfile->cd();
    TTree *newt = new TTree(tname.c_str(), tname.c_str());
    for (int idx = 0; idx < tentries; idx ++){
      // go to the next file if needed
      if (idx / tentries > splitting[currentfile_idx]){
        cout << "Go to file " << currentfile_idx << endl;
        currentfile_idx++;
        newt->Write();
        currentfile = outfiles[currentfile_idx];
        currentfile->cd();
        newt = new TTree(tname.c_str(), tname.c_str());
      }
      
    }
    newt->Write();
    // create tree in output files
    /*
      outfiles[i]->cd();
      TTree *tree = new TTree(tname.c_str(), tname.c_str());
      tree->Write();
    }
    */
  }
  fin->Close();
  for (size_t i = 0; i < outfiles.size(); i++)
    outfiles[i]->Close();
  return 0;
}
