#include <TFile.h>
#include <TTree.h>
#include <TCut.h>
#include <TH1F.h>
#include <RooWorkspace.h>
#include <RooRealVar.h>
#include <RooDataSet.h>

#include <string>
#include <regex>

#include "fit.h"
#include "project.h"
#include "lxydis.h"
#include "MCefficiency.h"
#include "xjjcuti.h"

void fitX_savehist(std::string input, std::string inputmcp_a, std::string inputmcp_b, std::string inputmcnp_a, std::string inputmcnp_b,
                   std::string cut, std::string cutgen, std::string cutgenacc, std::string cutpre, std::string output)
{
  std::cout<<"\e[32;1m -- "<<__FUNCTION__<<"\e[0m"<<std::endl;
  std::cout<<std::endl<<std::string(20, '-')<<std::endl;
  std::cout<<cut<<std::endl;
  std::cout<<cutgen<<std::endl;
  std::cout<<cutgenacc<<std::endl;
  std::cout<<cutpre<<std::endl;
  cut = xjjc::str_replaceall(cut, " ", "");
  std::cout<<std::string(20, '-')<<std::endl<<std::endl;

  std::map<std::string, std::vector<float>> lxyxbins = lxydis::setupbins();
  std::string input_flatten = xjjc::str_replaceall(input, ".root", "_flatten.root");
  std::string inputmcp_a_flatten = xjjc::str_replaceall(inputmcp_a, ".root", "_flatten.root");
  std::string inputmcp_b_flatten = xjjc::str_replaceall(inputmcp_b, ".root", "_flatten.root");

  std::string mcweight = "(pthatweight*Ncoll)";
  RooRealVar* mass = new RooRealVar("Bmass", "Bmass", fitX::BIN_MIN, fitX::BIN_MAX);
  RooRealVar* massmc_a = new RooRealVar("Bmass", "massmc_a", fitX::BIN_MIN_L, fitX::BIN_MAX_L);
  RooRealVar* massmc_b = new RooRealVar("Bmass", "massmc_b", fitX::BIN_MIN_H, fitX::BIN_MAX_H);
  RooRealVar* pthatweight = new RooRealVar("pthatweight", "pthatweight", 0, 10.); // pthatweight range!!

  // TH1 must be defined after TTree declaration (some tricky issue) if no `gDirectory->cd("root:/");`
  TH1F* h = new TH1F("h", Form(";m_{#mu#mu#pi#pi} (GeV/c^{2});Entries / (%.0f MeV/c^{2})", fitX::BIN_WIDTH*1.e+3), fitX::NBIN, fitX::BIN_MIN, fitX::BIN_MAX); h->Sumw2();
  RooDataSet* dsh;
  TH1F* hBenr = new TH1F("hBenr", Form(";m_{#mu#mu#pi#pi} (GeV/c^{2});Entries / (%.0f MeV/c^{2})", fitX::BIN_WIDTH*1.e+3), fitX::NBIN, fitX::BIN_MIN, fitX::BIN_MAX); hBenr->Sumw2();
  RooDataSet* dshBenr;
  TH1F* hmcp_a = new TH1F("hmcp_a", Form(";m_{#mu#mu#pi#pi} (GeV/c^{2});Entries / (%.0f MeV/c^{2})", fitX::BIN_WIDTH_L*1.e+3), fitX::NBIN_L, fitX::BIN_MIN_L, fitX::BIN_MAX_L); hmcp_a->Sumw2();
  RooDataSet* dshmcp_a;
  TH1F* hmcp_b = new TH1F("hmcp_b", Form(";m_{#mu#mu#pi#pi} (GeV/c^{2});Entries / (%.0f MeV/c^{2})", fitX::BIN_WIDTH_H*1.e+3), fitX::NBIN_H, fitX::BIN_MIN_H, fitX::BIN_MAX_H); hmcp_b->Sumw2();
  RooDataSet* dshmcp_b;
  TH1F* hlxymcnp_a = new TH1F("hlxymcnp_a", Form(";%s;Probability", lxydis::vars["lxy"].c_str()), lxyxbins["lxynonprompt"].size()-1, lxyxbins["lxynonprompt"].data()); hlxymcnp_a->Sumw2();
  TH1F* hlxymcnp_b = new TH1F("hlxymcnp_b", Form(";%s;Probability", lxydis::vars["lxy"].c_str()), lxyxbins["lxynonprompt"].size()-1, lxyxbins["lxynonprompt"].data()); hlxymcnp_b->Sumw2();
  TH1F* hlxymcp_a = new TH1F("hlxymcp_a", Form(";%s;Probability", lxydis::vars["lxy"].c_str()), lxyxbins["lxyprompt"].size()-1, lxyxbins["lxyprompt"].data()); hlxymcp_a->Sumw2();
  TH1F* hlxymcp_b = new TH1F("hlxymcp_b", Form(";%s;Probability", lxydis::vars["lxy"].c_str()), lxyxbins["lxyprompt"].size()-1, lxyxbins["lxyprompt"].data()); hlxymcp_b->Sumw2();
  MCeff::MCefficiency* mceff_a = new MCeff::MCefficiency("_a");
  MCeff::MCefficiency* mceff_b = new MCeff::MCefficiency("_b");

  //
  TTree* ntmix = fitX::getnt(input, "Bfinder/ntmix"); if(!ntmix) { return; }
  TTree* ntmixmcp_a = fitX::getnt(inputmcp_a, "Bfinder/ntmix"); if(!ntmixmcp_a) { return; }
  TTree* ntmixmcp_b = fitX::getnt(inputmcp_b, "Bfinder/ntmix"); if(!ntmixmcp_b) { return; }
  TTree* ntGenmcp_a = fitX::getnt(inputmcp_a, "Bfinder/ntGen"); if(!ntGenmcp_a) { return; }
  TTree* ntGenmcp_b = fitX::getnt(inputmcp_b, "Bfinder/ntGen"); if(!ntGenmcp_b) { return; }
  TTree* ntmixmcnp_a = fitX::getnt(inputmcnp_a, "Bfinder/ntmix"); if(!ntmixmcnp_a) { return; }
  TTree* ntmixmcnp_b = fitX::getnt(inputmcnp_b, "Bfinder/ntmix"); if(!ntmixmcnp_b) { return; }
  TTree* ntmix_flatten = fitX::getnt(input_flatten, "ntmix_flatten", false); if(!ntmix_flatten) { return; }
  TTree* ntmixmcp_a_flatten = fitX::getnt(inputmcp_a_flatten, "ntmix_flatten", false); if(!ntmixmcp_a_flatten) { return; }
  TTree* ntmixmcp_b_flatten = fitX::getnt(inputmcp_b_flatten, "ntmix_flatten", false); if(!ntmixmcp_b_flatten) { return; }
  
  gDirectory->cd("root:/");
  RooWorkspace* ww = new RooWorkspace("ww");

  std::string cutreco = Form("(%s) && Bmass >= %f && Bmass < %f && Bpt>%f && Bpt<%f && TMath::Abs(By)>=%f && TMath::Abs(By)<%f && hiBin>=%f && hiBin<=%f", cut.c_str(), 
                             fitX::BIN_MIN, fitX::BIN_MAX, 
                             fitX::ptmincut, fitX::ptmaxcut, 
                             fitX::ymincut, fitX::ymaxcut, 
                             fitX::centmincut*2, fitX::centmaxcut*2);
  std::regex e("&&[(]\\b(Bmass-3.096916-Btktkmass)[)]<[0-9\\.]+"); // remove Q value cut
  std::string cutrecobdt = std::regex_replace(cutreco, e, "");
  std::string cutrecopre = Form("(%s) && Bmass >= %f && Bmass < %f && Bpt>%f && Bpt<%f && TMath::Abs(By)>=%f && TMath::Abs(By)<%f && hiBin>=%f && hiBin<=%f", cutpre.c_str(), 
                                fitX::BIN_MIN, fitX::BIN_MAX, 
                                fitX::ptmincut, fitX::ptmaxcut, 
                                fitX::ymincut, fitX::ymaxcut, 
                                fitX::centmincut*2, fitX::centmaxcut*2);
  std::string cutmcreco = Form("%s && Bgen>=23333 && BgencollisionId==0", cutreco.c_str());
  std::string cutmcpre = Form("%s && Bgen>=23333 && BgencollisionId==0", cutrecopre.c_str());
  std::string cutmcbdt = Form("%s && Bgen>=23333 && BgencollisionId==0", cutrecobdt.c_str());
  std::string cutmcgen = Form("(%s) && Gpt>%f && Gpt<%f && TMath::Abs(Gy)>=%f && TMath::Abs(Gy)<%f && hiBin>=%f && hiBin<=%f && GisSignal==7 && GcollisionId==0", cutgen.c_str(), fitX::ptmincut, fitX::ptmaxcut, fitX::ymincut, fitX::ymaxcut, fitX::centmincut*2, fitX::centmaxcut*2);
  std::string cutmcgenacc = Form("(%s) && Gpt>%f && Gpt<%f && TMath::Abs(Gy)>=%f && TMath::Abs(Gy)<%f && hiBin>=%f && hiBin<=%f && GisSignal==7 && GcollisionId==0", cutgenacc.c_str(), fitX::ptmincut, fitX::ptmaxcut, fitX::ymincut, fitX::ymaxcut, fitX::centmincut*2, fitX::centmaxcut*2);

  //
  std::cout<<" == data ==>"<<std::endl;
  xjjroot::printhist(ntmix_flatten, 17);
  ntmix->Project("h", "Bmass", TCut(cutreco.c_str()));
  xjjroot::printhist(h, 17);
  TTree* ntmix_skimh = (TTree*)ntmix_flatten->CopyTree(TCut(cutreco.c_str())); ntmix_skimh->SetName("ntmix_skimh");
  xjjroot::printhist(ntmix_skimh, 17);
  dsh = new RooDataSet("dsh", "", ntmix_skimh, RooArgSet(*mass));
  ww->import(*dsh);
  ntmix->Project("hBenr", "Bmass", TCut(Form("%s && %s > 0.1", cutreco.c_str(), lxydis::formulas["lxy"].c_str())));
  xjjroot::printhist(hBenr, 17);
  TTree* ntmix_skimhBenr = (TTree*)ntmix_flatten->CopyTree(TCut(Form("%s && %s > 0.1", cutreco.c_str(), lxydis::formulas["lxy"].c_str()))); ntmix_skimhBenr->SetName("ntmix_skimhBenr");
  xjjroot::printhist(ntmix_skimhBenr, 17);
  dshBenr = new RooDataSet("dshBenr", "", ntmix_skimhBenr, RooArgSet(*mass));
  ww->import(*dshBenr);

  //
  std::cout<<" == mcp_a ==>"<<std::endl;
  ntmixmcp_a->Project("hmcp_a", "Bmass", TCut("pthatweight")*TCut(cutmcreco.c_str())); // !! weight
  xjjroot::printhist(hmcp_a, 17);
  TTree* ntmixmcp_a_skim = (TTree*)ntmixmcp_a_flatten->CopyTree(TCut(cutmcreco.c_str())); ntmixmcp_a_skim->SetName("ntmixmcp_a_skim");
  xjjroot::printhist(ntmixmcp_a_skim, 17);
  dshmcp_a = new RooDataSet("dshmcp_a", "", ntmixmcp_a_skim, RooArgSet(*massmc_a, *pthatweight), 0, "pthatweight");
  ww->import(*dshmcp_a);
  ntmixmcp_a->Project(mceff_a->heffmc()->GetName(), "Bpt", TCut(mcweight.c_str())*TCut(cutmcreco.c_str()));
  xjjroot::printhist(mceff_a->heffmc(), 17);
  ntmixmcp_a->Project(mceff_a->heffmc_incl()->GetName(), Form("%d", fitX::ibin_a-1), TCut(mcweight.c_str())*TCut(cutmcreco.c_str()));
  xjjroot::printhist(mceff_a->heffmc_incl(), 17);
  ntmixmcp_a->Project(mceff_a->heffmcpre()->GetName(), "Bpt", TCut(mcweight.c_str())*TCut(cutmcpre.c_str()));
  xjjroot::printhist(mceff_a->heffmcpre(), 17);
  ntmixmcp_a->Project(mceff_a->heffmcpre_incl()->GetName(), Form("%d", fitX::ibin_a-1), TCut(mcweight.c_str())*TCut(cutmcpre.c_str()));
  xjjroot::printhist(mceff_a->heffmcpre_incl(), 17);
  ntmixmcp_a->Project(mceff_a->heffmcbdt()->GetName(), "Bpt", TCut(mcweight.c_str())*TCut(cutmcbdt.c_str()));
  xjjroot::printhist(mceff_a->heffmcbdt(), 17);
  ntmixmcp_a->Project(mceff_a->heffmcbdt_incl()->GetName(), Form("%d", fitX::ibin_a-1), TCut(mcweight.c_str())*TCut(cutmcbdt.c_str()));
  xjjroot::printhist(mceff_a->heffmcbdt_incl(), 17);
  ntmixmcp_a->Project("hlxymcp_a", lxydis::formulas["lxy"].c_str(), TCut(mcweight.c_str())*TCut(cutmcreco.c_str()));
  xjjroot::printhist(hlxymcp_a, 17);

  //
  std::cout<<" == mcp_b ==>"<<std::endl;
  ntmixmcp_b->Project("hmcp_b", "Bmass", TCut("pthatweight")*TCut(cutmcreco.c_str())); // !! weight
  xjjroot::printhist(hmcp_b, 17);
  TTree* ntmixmcp_b_skim = (TTree*)ntmixmcp_b_flatten->CopyTree(TCut(cutmcreco.c_str())); ntmixmcp_b_skim->SetName("ntmixmcp_b_skim");
  xjjroot::printhist(ntmixmcp_b_skim, 17); 
  dshmcp_b = new RooDataSet("dshmcp_b", "", ntmixmcp_b_skim, RooArgSet(*massmc_b, *pthatweight), 0, "pthatweight");
  ww->import(*dshmcp_b);
  ntmixmcp_b->Project(mceff_b->heffmc()->GetName(), "Bpt", TCut(mcweight.c_str())*TCut(cutmcreco.c_str()));
  xjjroot::printhist(mceff_b->heffmc(), 17);
  ntmixmcp_b->Project(mceff_b->heffmc_incl()->GetName(), Form("%d", fitX::ibin_b-1), TCut(mcweight.c_str())*TCut(cutmcreco.c_str()));
  xjjroot::printhist(mceff_b->heffmc_incl(), 17);
  ntmixmcp_b->Project(mceff_b->heffmcpre()->GetName(), "Bpt", TCut(mcweight.c_str())*TCut(cutmcpre.c_str()));
  xjjroot::printhist(mceff_b->heffmcpre(), 17);
  ntmixmcp_b->Project(mceff_b->heffmcpre_incl()->GetName(), Form("%d", fitX::ibin_b-1), TCut(mcweight.c_str())*TCut(cutmcpre.c_str()));
  xjjroot::printhist(mceff_b->heffmcpre_incl(), 17);
  ntmixmcp_b->Project(mceff_b->heffmcbdt()->GetName(), "Bpt", TCut(mcweight.c_str())*TCut(cutmcbdt.c_str()));
  xjjroot::printhist(mceff_b->heffmcbdt(), 17);
  ntmixmcp_b->Project(mceff_b->heffmcbdt_incl()->GetName(), Form("%d", fitX::ibin_b-1), TCut(mcweight.c_str())*TCut(cutmcbdt.c_str()));
  xjjroot::printhist(mceff_b->heffmcbdt_incl(), 17);
  ntmixmcp_b->Project("hlxymcp_b", lxydis::formulas["lxy"].c_str(), TCut(mcweight.c_str())*TCut(cutmcreco.c_str()));
  xjjroot::printhist(hlxymcp_b, 17);

  //
  std::cout<<" == mcgenp_a ==>"<<std::endl;
  ntGenmcp_a->Project(mceff_a->heffgen()->GetName(), "Gpt", TCut(mcweight.c_str())*TCut(cutmcgen.c_str()));
  xjjroot::printhist(mceff_a->heffgen(), 17);
  ntGenmcp_a->Project(mceff_a->heffgen_incl()->GetName(), Form("%d", fitX::ibin_a-1), TCut(mcweight.c_str())*TCut(cutmcgen.c_str()));
  xjjroot::printhist(mceff_a->heffgen_incl(), 17);
  ntGenmcp_a->Project(mceff_a->heffgenacc()->GetName(), "Gpt", TCut(mcweight.c_str())*TCut(cutmcgenacc.c_str()));
  xjjroot::printhist(mceff_a->heffgenacc(), 17);
  ntGenmcp_a->Project(mceff_a->heffgenacc_incl()->GetName(), Form("%d", fitX::ibin_a-1), TCut(mcweight.c_str())*TCut(cutmcgenacc.c_str()));
  xjjroot::printhist(mceff_a->heffgenacc_incl(), 17);

  std::cout<<" == mcgenp_b ==>"<<std::endl;
  ntGenmcp_b->Project(mceff_b->heffgen()->GetName(), "Gpt", TCut(mcweight.c_str())*TCut(cutmcgen.c_str()));
  xjjroot::printhist(mceff_b->heffgen(), 17);
  ntGenmcp_b->Project(mceff_b->heffgen_incl()->GetName(), Form("%d", fitX::ibin_b-1), TCut(mcweight.c_str())*TCut(cutmcgen.c_str()));
  xjjroot::printhist(mceff_b->heffgen_incl(), 17);
  ntGenmcp_b->Project(mceff_b->heffgenacc()->GetName(), "Gpt", TCut(mcweight.c_str())*TCut(cutmcgenacc.c_str()));
  xjjroot::printhist(mceff_b->heffgenacc(), 17);
  ntGenmcp_b->Project(mceff_b->heffgenacc_incl()->GetName(), Form("%d", fitX::ibin_b-1), TCut(mcweight.c_str())*TCut(cutmcgenacc.c_str()));
  xjjroot::printhist(mceff_b->heffgenacc_incl(), 17);

  std::cout<<" == mcnp_a ==>"<<std::endl;
  ntmixmcnp_a->Project("hlxymcnp_a", lxydis::formulas["lxy"].c_str(), TCut(mcweight.c_str())*TCut(cutmcreco.c_str()));
  xjjroot::printhist(hlxymcnp_a, 17);

  std::cout<<" == mcnp_b ==>"<<std::endl;
  ntmixmcnp_b->Project("hlxymcnp_b", lxydis::formulas["lxy"].c_str(), TCut(mcweight.c_str())*TCut(cutmcreco.c_str()));
  xjjroot::printhist(hlxymcnp_b, 17);

  std::string outputname = "rootfiles/" + output + fitX::tagname() + "/fitX_savehist.root";
  xjjroot::mkdir(outputname.c_str());
  TFile* outf = new TFile(outputname.c_str(), "recreate");
  outf->cd();
  h->Write();
  hBenr->Write();
  hmcp_a->Write();
  hmcp_b->Write();
  hlxymcnp_a->Write();
  hlxymcnp_b->Write();
  hlxymcp_a->Write();
  hlxymcp_b->Write();
  mceff_a->heffmc()->Write();
  mceff_a->heffmc_incl()->Write();
  mceff_a->heffgen()->Write();
  mceff_a->heffgen_incl()->Write();
  mceff_a->heffgenacc()->Write();
  mceff_a->heffgenacc_incl()->Write();
  mceff_a->heffmcpre()->Write();
  mceff_a->heffmcpre_incl()->Write();
  mceff_a->heffmcbdt()->Write();
  mceff_a->heffmcbdt_incl()->Write();
  mceff_b->heffmc()->Write();
  mceff_b->heffmc_incl()->Write();
  mceff_b->heffgen()->Write();
  mceff_b->heffgen_incl()->Write();
  mceff_b->heffgenacc()->Write();
  mceff_b->heffgenacc_incl()->Write();
  mceff_b->heffmcpre()->Write();
  mceff_b->heffmcpre_incl()->Write();
  mceff_b->heffmcbdt()->Write();
  mceff_b->heffmcbdt_incl()->Write();
  outf->cd();
  gDirectory->Add(ww);
  ww->Write();
  ww->Print();
  outf->cd();
  TTree* info = new TTree("info", "cut info");
  info->Branch("input", &input);
  info->Branch("inputmcp_a", &inputmcp_a);
  info->Branch("inputmcp_b", &inputmcp_b);
  info->Branch("inputmcnp_a", &inputmcnp_a);
  info->Branch("inputmcnp_b", &inputmcnp_b);
  info->Branch("cutreco", &cutreco);
  info->Branch("cutmcreco", &cutmcreco);
  info->Branch("cutmcpre", &cutmcpre);
  info->Branch("cutmcbdt", &cutmcbdt);
  info->Branch("cutmcgen", &cutmcgen);
  info->Branch("cutmcgenacc", &cutmcgenacc);
  info->Fill();
  info->Write();
  fitX::write();
  outf->Close();
  std::cout<<std::endl;
}

int main(int argc, char* argv[])
{
  if(argc==17) { 
    fitX::init(atof(argv[11]), atof(argv[12]), atof(argv[13]), atof(argv[14]), atof(argv[15]), atof(argv[16]));
    fitX_savehist(argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], argv[7], argv[8], argv[9], argv[10]); return 0; }
  return 1;
}
