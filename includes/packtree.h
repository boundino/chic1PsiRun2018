#ifndef _XJJC_PACKTREE_H_
#define _XJJC_PACKTREE_H_

#include "ntuple.h"

#include <TFile.h>
#include <TTree.h>
#include <string>

#include "xjjcuti.h"

namespace xjjroot
{
  typedef std::vector<TTree*> trlist;
  class packtree
  {
  public:
    packtree(TFile* inf, std::string treename, std::string type, std::string gentreename = "");
    mytmva::ntuple* ntp() { return fntp; }
    int getentries() { return fentries; }
    std::string getname() { return name; }
    void getentry(int i);
    // TTree* getnt() { return nt; }
    // TTree* gethlt() { return hlt; }
    // TTree* getskim() { return skim; }
    // TTree* gethi() { return hi; }
  private:
    mytmva::ntuple* fntp;
    std::string name;
    TTree* nt;
    TTree* hlt;
    TTree* skim;
    TTree* hi;
    TTree* mva;
    TTree* ntgen;
    TTree* ntmu;
    int fentries;
    bool fgen;
    bool fmutrg;
  };
}

void xjjroot::packtree::getentry(int i)
{
  nt->GetEntry(i);
  hlt->GetEntry(i);
  hi->GetEntry(i);
  skim->GetEntry(i);
  mva->GetEntry(i);
  if(fgen) ntgen->GetEntry(i);
  if(fmutrg) ntmu->GetEntry(i);
}

xjjroot::packtree::packtree(TFile* inf, std::string treename, std::string type, std::string gentreename)
{
  name = type;
  ntgen = 0; 
  fgen = gentreename!="";
  fmutrg = inf->GetListOfKeys()->Contains("muinfo");

  nt = (TTree*)inf->Get(treename.c_str());
  fentries = nt->GetEntries();

  hlt = (TTree*)inf->Get("hltanalysis/HltTree");
  hlt->SetBranchStatus("*", 0);
  hlt->SetBranchStatus("HLT_HIL3Mu0NHitQ10_L2Mu0_MAXdR3p5_M1to5_v1", 1);
  nt->AddFriend(hlt);

  skim = (TTree*)inf->Get("skimanalysis/HltTree");
  nt->AddFriend(skim);

  hi = (TTree*)inf->Get("hiEvtAnalyzer/HiTree");
  nt->AddFriend(hi);

  mva = (TTree*)inf->Get("dataset/mva");
  nt->AddFriend(mva);

  if(fgen) 
    { 
      ntgen = (TTree*)inf->Get(gentreename.c_str());
      ntgen->AddFriend(hlt);
      ntgen->AddFriend(skim);
      ntgen->AddFriend(hi);
    }

  if(fmutrg)
  {
    ntmu = (TTree*)inf->Get("muinfo/mu");
    nt->AddFriend(ntmu);
  }

  fntp = new mytmva::ntuple(nt, ntgen);
}

#endif
