#!/usr/bin/env python

import sys
import random
import ROOT

fin = ROOT.TFile(sys.argv[2])
rand = float(sys.argv[1])

fout0 = ROOT.TFile(sys.argv[2] + ".0", 'recreate')
fout1 = ROOT.TFile(sys.argv[2] + ".1", 'recreate')

for tree in [fin.Get(x.GetName()) for x in fin.GetListOfKeys()]:
  fout0.cd()
  tree0 = tree.CloneTree(0)
  fout1.cd()
  tree1 = tree.CloneTree(1)
  for en in xrange(tree.GetEntries()):
    tree.GetEntry(en)
    if (random.random() < rand):
      tree0.Fill()
    else:
      tree1.Fill()
  tree0.AutoSave()
  tree1.AutoSave()
  tree0.Write()
  tree1.Write()

fin.Close()
fout0.Close()
fout1.Close()
