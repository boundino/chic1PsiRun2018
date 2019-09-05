#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>

#include <RooWorkspace.h>
#include <RooRealVar.h>
#include <RooDataSet.h>

#include <string>

#include "xjjrootuti.h"
#include "fit.h"
#include "smear.h"

const int nn = 600; //

void toymc(std::string input, std::string output)
{
  std::cout<<"\e[32;1m -- "<<__FUNCTION__<<"\e[0m"<<std::endl;
  TFile* inf = new TFile(input.c_str());
  fitX::init(inf);
  RooWorkspace* ww = (RooWorkspace*)inf->Get("ww");
  RooDataSet* dsh = (RooDataSet*)ww->data("dsh");
  RooDataSet* dshBenr = (RooDataSet*)ww->data("dshBenr");
  RooDataSet* dshmcp_a = (RooDataSet*)ww->data("dshmcp_a");
  RooDataSet* dshmcp_b = (RooDataSet*)ww->data("dshmcp_b");
  TH1F* h = (TH1F*)inf->Get("h");
  TH1F* hmcp_a = (TH1F*)inf->Get("hmcp_a");
  hmcp_a->Scale(hmcp_a->GetEntries()/hmcp_a->Integral());
  TH1F* hmcp_b = (TH1F*)inf->Get("hmcp_b");
  hmcp_b->Scale(hmcp_b->GetEntries()/hmcp_b->Integral());

  std::map<std::string, fitX::fitXresult*> result = fitX::fit(h, 0, hmcp_a, hmcp_b,
                                                              dsh, dshmcp_a, dshmcp_b,
                                                              Form("plots/%s/idx", output.c_str()), false, true, "nominal", "default", "real-data", true); // fix mean = false
  float ysig_a = result["unbinned"]->ysig_a();
  float ysigerr_a = result["unbinned"]->ysigerr_a();
  float ysig_b = result["unbinned"]->ysig_b();
  float ysigerr_b = result["unbinned"]->ysigerr_b();
  float msig_a = result["unbinned"]->msig_a();
  float msigerr_a = result["unbinned"]->msigerr_a();
  float msig_b = result["unbinned"]->msig_b();
  float msigerr_b = result["unbinned"]->msigerr_b();

  TH1F* hysig_a = new TH1F("hysig_a", "", 40, ysig_a - ysigerr_a*5, ysig_a + ysigerr_a*5);
  TH1F* hysig_b = new TH1F("hysig_b", "", 40, ysig_b - ysigerr_b*5, ysig_b + ysigerr_b*5);
  TH1F* hmsig_a = new TH1F("hmsig_a", "", 40, msig_a - msigerr_a*5, msig_a + msigerr_a*5);
  TH1F* hmsig_b = new TH1F("hmsig_b", "", 40, msig_b - msigerr_b*5, msig_b + msigerr_b*5);

  RooRealVar* mass = new RooRealVar("Bmass", "Bmass", fitX::BIN_MIN, fitX::BIN_MAX);
  RooWorkspace* wo = new RooWorkspace("wo");
  TF1* f = result["unbinned"]->f();
  int nentries = h->GetEntries();
  std::vector<TH1F*> hh(nn);
  std::vector<RooDataSet*> dshh(nn);
  for(int i=0; i<nn; i++)
    {
      dshh[i] = new RooDataSet(Form("dsh_%d", i), "", RooArgSet(*mass));
      // hh[i] = smear::smear(h, Form("_%d", i));
      hh[i] = new TH1F(Form("h_%d", i), ";;", fitX::NBIN, fitX::BIN_MIN, fitX::BIN_MAX);
      for(int j=0; j<nentries; j++)
        {
          float mm = f->GetRandom(fitX::BIN_MIN, fitX::BIN_MAX);
          mass->setVal(mm);
          dshh[i]->add(*mass);
          hh[i]->Fill(mm, 1);
        }
      fitX::printhist(hh[i]);
      dshh[i]->Print();
      wo->import(*dshh[i]);
      std::map<std::string, fitX::fitXresult*> rt = fitX::fit(hh[i], 0, hmcp_a, hmcp_b,
                                                              dshh[i], dshmcp_a, dshmcp_b,
                                                              Form("plots/%s/idx", output.c_str()), false, i%100==0, Form("-%d",i), "default", Form("pseudo-data (%d)", i), true); // fix mean = false
      hysig_a->Fill(rt["unbinned"]->ysig_a());
      hysig_b->Fill(rt["unbinned"]->ysig_b());
      hmsig_a->Fill(rt["unbinned"]->msig_a());
      hmsig_b->Fill(rt["unbinned"]->msig_b());
    }

  std::string outputname = Form("rootfiles/%s/toymc.root", output.c_str());
  xjjroot::mkdir(outputname.c_str());
  TFile* outf = new TFile(outputname.c_str(), "recreate");
  outf->cd();
  for(auto& ih : hh) { ih->Write(); }
  outf->cd();
  gDirectory->Add(wo);
  wo->Write();
  wo->Print();
  outf->cd();
  hysig_a->Write();
  hysig_b->Write();
  hmsig_a->Write();
  hmsig_b->Write();
  outf->Close();

}

int main(int argc, char* argv[])
{
  if(argc==3) { toymc(argv[1], argv[2]); return 0; }
  return 1;
}

