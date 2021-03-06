#ifndef _LXYDIS_H_
#define _LXYDIS_H_

#include <TH1F.h>
#include <TEfficiency.h>

#include <vector>
#include <map>
#include <string>

#include "xjjrootuti.h"

namespace lxydis
{
  const int nbin = 60;
  float binmin = -0.5, binmax = 2.5;
  std::map<std::string, std::vector<float>> lxycut = 
    {
      std::pair<std::string, std::vector<float>>("lxy", {0.1, 0.15, 0.2}),
      std::pair<std::string, std::vector<float>>("lxyz", {0.1, 0.15}),
    };
  std::vector<float> mvalist = {-1., 0, 0.4, 0.6, 0.7, 0.76, 0.80}; int thismva = 5;

  std::map<std::string, std::string> formulas = 
    {
      std::pair<std::string, std::string>("lxy", "10*Blxy*Bmass/Bpt"),
      std::pair<std::string, std::string>("lxyz", "10*BsvpvDistance*TMath::Cos(Balpha)*Bmass/(Bpt*TMath::CosH(Beta))"),
      std::pair<std::string, std::string>("dls", "BsvpvDistance/BsvpvDisErr"),
      std::pair<std::string, std::string>("dca", "BsvpvDistance*TMath::Sin(Balpha)")
    };
  std::map<std::string, std::string> vars = 
    {
      std::pair<std::string, std::string>("lxy", "l_{xy} (mm)"),
      std::pair<std::string, std::string>("lxyz", "l_{xyz} (mm)"),
      std::pair<std::string, std::string>("dls", "Decay length significance"),
      std::pair<std::string, std::string>("dca", "DCA (cm)")
    };

  class drawopt 
  {
  public: 
    Color_t mcolor, lcolor;
    Style_t mstyle, lstyle;
    Size_t msize; Width_t lwidth;
    drawopt(Color_t mcc, Style_t msy, Size_t msz, Color_t lcc, Style_t lsy, Width_t lwd) : 
      mcolor(mcc), mstyle(msy), msize(msz), lcolor(lcc), lstyle(lsy), lwidth(lwd)
    { ; }
  };

  std::map<std::string, drawopt*> drawset;
  void setupdraw();
  std::map<std::string, std::vector<float>> setupbins();
  std::vector<double> nplxyfrac(TH1F* hlxynpL, TH1F* hlxynpH, float xdiv=0.1);
  TEfficiency* calclxyfprompt(TH1F* hdata, TH1F* hBenr, TH1F* hlxyfrac, std::string name, TH1F** rhprompt);
}

void lxydis::setupdraw()
{
  drawset[ "SignalRegion_data" ]      = new drawopt(kBlack    , 20 , 0.9 , kBlack    , 1 , 1);
  drawset[ "Sideband_data" ]          = new drawopt(kAzure-3  , 21 , 0.2 , kAzure-3  , 1 , 1);
  drawset[ "SignalRegion_samesign" ]  = new drawopt(kViolet+7 , 21 , 0.2 , kViolet+7 , 1 , 1);
  drawset[ "SignalRegion_prompt" ]    = new drawopt(kGreen+2  , 21 , 0.2 , kGreen+2  , 1 , 1);
  drawset[ "SignalRegion_nonprompt" ] = new drawopt(kRed-3    , 21 , 0.2 , kRed-3    , 1 , 1);
}

std::map<std::string, std::vector<float>> lxydis::setupbins()
{
  std::map<std::string, std::vector<float>> xbins;
  float pos;
  // lxy
  pos = -0.5;
  while(pos < 2.6)
    {
      xbins["lxy"].push_back(pos);
      if(pos < -0.11) { pos += 0.1; }
      else if(pos >= 1.2) { pos += 0.2; }
      else { pos += 0.05; }
    }

  // lxyz
  pos = -1.;
  while(pos < 3.1)
    {
      xbins["lxyz"].push_back(pos);
      pos += 0.05;
      // if(pos < -0.11) { pos += 0.1; }
      // else if(pos >= 1.2) { pos += 0.2; }
      // else { pos += 0.05; }
    }

  // dls
  pos = 0;
  while(pos < 30)
    {
      xbins["dls"].push_back(pos);
      if(pos < 2) { pos += 0.5; }
      else { pos += 2.; }
    }

  // dca
  pos = 0;
  while(pos < 0.08)
    {
      xbins["dca"].push_back(pos);
      float firstBinYWidth = 0.001;
      float binYWidthRatio = 1.27;
      pos += firstBinYWidth * pow(binYWidthRatio, xbins["dca"].size()-1);
    }

  // lxy prompt
  pos = -0.2;
  while(pos < 0.21)
    {
      xbins["lxyprompt"].push_back(pos);
      pos += 0.01;
    }

  // lxy nonprompt
  pos = -0.5;
  while(pos < 2.6)
    {
      xbins["lxynonprompt"].push_back(pos);
      if(pos < 0.5) { pos += 0.05; }
      else if(pos >= 1.2) { pos += 0.2; }
      else { pos += 0.1; }
    }

  // lxyz prompt
  pos = -0.2;
  while(pos < 0.21)
    {
      xbins["lxyzprompt"].push_back(pos);
      pos += 0.01;
    }

  // lxyz nonprompt
  pos = -0.5;
  while(pos < 2.6)
    {
      xbins["lxyznonprompt"].push_back(pos);
      if(pos < 0.5) { pos += 0.05; }
      else if(pos >= 1.2) { pos += 0.2; }
      else { pos += 0.1; }
    }

  return xbins;
}

std::vector<double> lxydis::nplxyfrac(TH1F* hlxynpL, TH1F* hlxynpH, float xdiv)
{
  int nbinnonpromptL = hlxynpL->GetXaxis()->FindBin(xdiv)-1;
  int nbinnonpromptH = hlxynpH->GetXaxis()->FindBin(xdiv)-1;
  double fltLerr, fgtLerr, fltHerr, fgtHerr;
  double fltL = hlxynpL->IntegralAndError(1,                nbinnonpromptL,       fltLerr, "width");
  double fgtL = hlxynpL->IntegralAndError(nbinnonpromptL+1, hlxynpL->GetNbinsX(), fgtLerr, "width");
  double fltH = hlxynpH->IntegralAndError(1,                nbinnonpromptH,       fltHerr, "width");
  double fgtH = hlxynpH->IntegralAndError(nbinnonpromptH+1, hlxynpH->GetNbinsX(), fgtHerr, "width");
  std::vector<double> r = 
    {
      fgtL, fgtLerr,
      fgtH, fgtHerr
    };
  return r;
}

TEfficiency* lxydis::calclxyfprompt(TH1F* hdata, TH1F* hBenr, TH1F* hlxyfrac, std::string name, TH1F** rhprompt)
{
  TH1F* hnonprompt = (TH1F*)hBenr->Clone("hnonprompt");
  hnonprompt->Divide(hlxyfrac);
  TH1F* hprompt = (TH1F*)xjjroot::histMinusCorr(hdata, hnonprompt, Form("hprompt_%s", name.c_str()));
  TEfficiency* grfprompt = new TEfficiency(*hprompt, *hdata); grfprompt->SetName(name.c_str());

  *rhprompt = hprompt;
  return grfprompt;
}

#endif
