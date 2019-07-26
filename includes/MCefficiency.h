#include "xjjrootuti.h"

#include <TFile.h>
#include <TH1F.h>
#include <TEfficiency.h>
#include <TGraphAsymmErrors.h>

#include <string>

namespace MCeff
{
  std::vector<float> ptBins = {15, 20, 25, 30, 40, 60, 80, 100};
  const int nPtBins = ptBins.size() - 1;

  class MCefficiency
  {
  public:
    MCefficiency(std::string name);
    MCefficiency(TFile* inf, std::string name);
    TH1F* heffmc;
    TH1F* heffmc_incl;
    TH1F* heffgen;
    TH1F* heffgen_incl;
    TEfficiency* greff;
    TEfficiency* greff_incl;
    TH1F* heff;
    TH1F* heff_incl;

    void calceff();
    void setstyle(Color_t color);
  private:
    std::string fname;
    void createhist();
    void readhist(TFile* inf);
    
  };
}

MCeff::MCefficiency::MCefficiency(std::string name) : fname(name)
{
  createhist();
}

MCeff::MCefficiency::MCefficiency(TFile* inf, std::string name) : fname(name)
{
  readhist(inf);
}

void MCeff::MCefficiency::createhist()
{
  heffmc = new TH1F(Form("heffmc%s", fname.c_str()), ";p_{T} (GeV/c);", MCeff::nPtBins, MCeff::ptBins.data()); heffmc->Sumw2();
  heffmc_incl = new TH1F(Form("heffmc_incl%s", fname.c_str()), "", 5, 0, 5); heffmc_incl->Sumw2();
  heffgen = new TH1F(Form("heffgen%s", fname.c_str()), ";p_{T} (GeV/c);", MCeff::nPtBins, MCeff::ptBins.data());
  heffgen_incl = new TH1F(Form("heffgen_incl%s", fname.c_str()), "", 5, 0, 5); heffgen_incl->Sumw2();
}

void MCeff::MCefficiency::readhist(TFile* inf)
{
  heffmc = (TH1F*)inf->Get(Form("heffmc%s", fname.c_str()));
  heffmc_incl = (TH1F*)inf->Get(Form("heffmc_incl%s", fname.c_str()));
  heffgen = (TH1F*)inf->Get(Form("heffgen%s", fname.c_str()));
  heffgen_incl = (TH1F*)inf->Get(Form("heffgen_incl%s", fname.c_str()));
}

void MCeff::MCefficiency::calceff()
{
  greff = new TEfficiency(*heffmc, *heffgen); greff->SetName(Form("greff%s", fname.c_str()));
  greff_incl = new TEfficiency(*heffmc_incl, *heffgen_incl); greff_incl->SetName(Form("greff_incl%s", fname.c_str()));
  heff = (TH1F*)heffmc->Clone(Form("heff%s", fname.c_str()));
  heff->Divide(heffgen);
  heff_incl = (TH1F*)heffmc_incl->Clone(Form("heff_incl%s", fname.c_str()));
  heff_incl->Divide(heffgen_incl);
}

void MCeff::MCefficiency::setstyle(Color_t color)
{
  xjjroot::setthgrstyle(heff, 0, 0, 0, color, 2, 3, color, 0.1, 1001);
  xjjroot::setthgrstyle(heff_incl, color, 20, 1.2, color, 2, 3, color, 0.1, 1001);
  xjjroot::setthgrstyle(greff, 0, 0, 0, color, 2, 3, color, 0.1, 1001);
  xjjroot::setthgrstyle(greff_incl, color, 20, 1.2, color, 2, 3, color, 0.1, 1001);
}