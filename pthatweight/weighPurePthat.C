#include <iostream>
#include <iomanip>

#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TDirectory.h"

int weighPurePthat(TString ifname, TString ofname)
{
  bool isInsidebin(float xpthat, int i);
  std::cout<<std::endl<<" -- Checking if input and output files are same"<<std::endl;
  if(ifname==ofname)
    {
      std::cout<<"    Error: Input file will be overwritten."<<std::endl;
      return 1;
    }
  std::cout<<" -- Opening unweighed sample"<<std::endl;
  TFile* inf = TFile::Open(ifname);
  TTree* HiTree = (TTree*)inf->Get("hiEvtAnalyzer/HiTree");
  float pthat; HiTree->SetBranchAddress("pthat",&pthat);
  bool cleanpthat = HiTree->FindBranch("sample");
  float sample = 0; if(cleanpthat) { HiTree->SetBranchAddress("sample",&sample); }

  float weight[nBins],nweight[nBins];
  for(int j=0;j<nBins;j++)
    {
      weight[j]=0;
      nweight[j]=0;
    }
  std::cout<<" -- Checking event number"<<std::endl;
  int nentries = HiTree->GetEntries();
  std::cout<<" -- Calculating weights"<<std::endl;
  for(int i=0;i<nentries;i++)
    {
      HiTree->GetEntry(i);
      if(i%100==0) std::cout<<std::left<<" Processing [ "<<std::setw(10)<<i<<" / "<<nentries<<" ] - "<<std::setw(6)<<(int)(100.*i/nentries)<<"% \r"<<std::flush;

      if(pthat < sample) continue;

      for(int j=0;j<nBins;j++)
        {
          if(isInsidebin(pthat,j)) nweight[j]++;
        }
    }
  std::cout<<std::endl<<" -- Weight results"<<std::endl;
  for(int j=0;j<nBins;j++)
    {
      if(nweight[j]==0)
        {
          std::cout<<"    Error: Weight fails."<<std::endl;
          return 1;
        }
      weight[j] = (crosssec[j]-crosssec[j+1])/nweight[j];
      std::cout<<"    Pthat"<<std::left<<std::setw(3)<<pthatBin[j]<<": "<<weight[j]<<std::endl;
    }

  std::cout<<" -- Building weight branch"<<std::endl;
  TFile* outf = TFile::Open(ofname, "update");
  TTree* HiTree_new = (TTree*)outf->Get("hiEvtAnalyzer/HiTree");
  float pthatweight;
  TBranch* newBr_pthatweight = HiTree_new->Branch("pthatweight", &pthatweight, "pthatweight/F");
  std::cout<<" -- Filling weight branch"<<std::endl;
  for(int i=0;i<nentries;i++)
    {
      HiTree->GetEntry(i);
      if(i%100==0) std::cout<<std::left<<" Processing [ "<<std::setw(10)<<i<<" / "<<nentries<<" ] - "<<std::setw(6)<<(int)(100.*i/nentries)<<"% \r"<<std::flush;

      pthatweight=0;
      if(pthat > sample) 
        {
          for(int j=0;j<nBins;j++)
            {
              if(isInsidebin(pthat,j))
                {
                  pthatweight = weight[j];
                }
            }
        }
      newBr_pthatweight->Fill();
    }
  outf->cd("hiEvtAnalyzer");
  HiTree_new->Write("", TObject::kOverwrite);

  std::cout<<std::endl<<" -- End"<<std::endl<<std::endl;

  return 0;
}

bool isInsidebin(float xpthat, int i)
{
  if(i >= nBins)
    {
      std::cout<<"    Error: invalid input"<<std::endl;
      return false;
    }
  if(i<(nBins-1) && xpthat>pthatBin[i] && xpthat<pthatBin[i+1]) return true;
  else if(i==(nBins-1) && xpthat>pthatBin[i]) return true;
  else return false;
}

int main(int argc, char *argv[])
{
  if(argc==3) { return weighPurePthat(argv[1], argv[2]); }
  std::cout<<"Invalid parameter number"<<std::endl;
  return 1;
}
