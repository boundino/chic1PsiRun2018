#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TCut.h>

#include <string>
#include <iostream>
#include <vector>

#include "fitX.h"
#include "project.h"
#include "xjjcuti.h"
#include "lxydis.h"

void lxydis_vary(std::string input, std::string inputmcp_a, std::string inputmcp_b, std::string inputmcnp_a, std::string inputmcnp_b, 
                 std::string cut, std::string output)
{
  std::string mcweight = "(pthatweight*Ncoll)";
  std::map<std::string, std::vector<float>> lxyxbins = lxydis::setupbins();  

  std::string cutreco = Form("(%s) && Bpt>%f && Bpt<%f && TMath::Abs(By)<%f", cut.c_str(), fitX::ptmincut, fitX::ptmaxcut, fitX::ycut);
  std::string cutmcreco = Form("%s && Bgen>=23333 && BgencollisionId==0", cutreco.c_str());
  std::cout<<cutreco<<std::endl;
  //
  TTree* ntmix = (TTree*)fitX::getnt(input, "ntmix"); if(!ntmix) { return; }
  TTree* ntmixmcp_a = fitX::getnt(inputmcp_a, "ntmix"); if(!ntmixmcp_a) { return; }
  TTree* ntmixmcp_b = fitX::getnt(inputmcp_b, "ntmix"); if(!ntmixmcp_b) { return; }
  TTree* ntmixmcnp_a = fitX::getnt(inputmcnp_a, "ntmix"); if(!ntmixmcnp_a) { return; }
  TTree* ntmixmcnp_b = fitX::getnt(inputmcnp_b, "ntmix"); if(!ntmixmcnp_b) { return; }

  TH1F* h = new TH1F("h", Form(";m_{#mu#mu#pi#pi} (GeV/c^{2});Entries / %.0f MeV", fitX::BIN_WIDTH*1.e+3), fitX::NBIN, fitX::BIN_MIN, fitX::BIN_MAX); h->Sumw2();
  TH1F* hlxymcnp_a = new TH1F("hlxymcnp_a", ";l_{xy} (mm);Probability", lxyxbins["lxynonprompt"].size()-1, lxyxbins["lxynonprompt"].data()); hlxymcnp_a->Sumw2();
  TH1F* hlxymcnp_b = new TH1F("hlxymcnp_b", ";l_{xy} (mm);Probability", lxyxbins["lxynonprompt"].size()-1, lxyxbins["lxynonprompt"].data()); hlxymcnp_b->Sumw2();
  TH1F* hmcp_a = new TH1F("hmcp_a", Form(";m_{#mu#mu#pi#pi} (GeV/c^{2});Entries / %.0f MeV", fitX::BIN_WIDTH_L*1.e+3), fitX::NBIN_L, fitX::BIN_MIN_L, fitX::BIN_MAX_L); hmcp_a->Sumw2();
  TH1F* hmcp_b = new TH1F("hmcp_b", Form(";m_{#mu#mu#pi#pi} (GeV/c^{2});Entries / %.0f MeV", fitX::BIN_WIDTH_H*1.e+3), fitX::NBIN_H, fitX::BIN_MIN_H, fitX::BIN_MAX_H); hmcp_b->Sumw2();
  std::vector<TH1F*> hBenr(lxydis::lxycut.size());
  for(int k=0; k<lxydis::lxycut.size(); k++)
    {
      hBenr[k] = new TH1F(Form("hBenr_%d", k), Form(";m_{#mu#mu#pi#pi} (GeV/c^{2});Entries / %.0f MeV", fitX::BIN_WIDTH*1.e+3), fitX::NBIN, fitX::BIN_MIN, fitX::BIN_MAX);
      hBenr[k]->Sumw2();
    }

  std::cout<<" == data ==>"<<std::endl;
  ntmix->Project("h", "Bmass", TCut(cutreco.c_str()));
  std::cout<<h->GetEntries()<<std::endl;
  for(int k=0; k<lxydis::lxycut.size(); k++)
    {
      ntmix->Project(Form("hBenr_%d", k), "Bmass", TCut(Form("%s && Blxy > %f", cutreco.c_str(), lxydis::lxycut[k])));
      std::cout<<hBenr[k]->GetEntries()<<std::endl;
    }

  std::cout<<" == mcp_a ==>"<<std::endl;
  ntmixmcp_a->Project("hmcp_a", "Bmass", TCut("pthatweight")*TCut(cutmcreco.c_str())); // mass shape weight
  std::cout<<hmcp_a->GetEntries()<<std::endl;
  hmcp_a->Scale(hmcp_a->GetEntries()/hmcp_a->Integral());
  std::cout<<" == mcp_b ==>"<<std::endl;
  ntmixmcp_b->Project("hmcp_b", "Bmass", TCut("pthatweight")*TCut(cutmcreco.c_str())); // mass shape weight
  std::cout<<hmcp_b->GetEntries()<<std::endl;
  hmcp_b->Scale(hmcp_b->GetEntries()/hmcp_b->Integral());

  std::cout<<" == mcnp_a ==>"<<std::endl;
  ntmixmcnp_a->Project("hlxymcnp_a", "Blxy", TCut(mcweight.c_str())*TCut(cutmcreco.c_str()));
  std::cout<<hlxymcnp_a->GetEntries()<<std::endl;
  std::cout<<" == mcnp_b ==>"<<std::endl;
  ntmixmcnp_b->Project("hlxymcnp_b", "Blxy", TCut(mcweight.c_str())*TCut(cutmcreco.c_str()));
  std::cout<<hlxymcnp_b->GetEntries()<<std::endl;

  TFile* outf = new TFile(Form("%s.root", output.c_str()), "recreate");
  outf->cd();
  h->Write();
  for(auto& hh: hBenr) { hh->Write(); }
  hmcp_a->Write();
  hmcp_b->Write();
  hlxymcnp_a->Write();
  hlxymcnp_b->Write();
  outf->Close();

}

int main(int argc, char* argv[])
{
  if(argc==8) { lxydis_vary(argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], argv[7]); return 0; }
  return 1;
}
