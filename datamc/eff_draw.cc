#include <TFile.h>
#include <TH1F.h>
#include <TCanvas.h>

#include <string>
#include <iostream>

#include "xjjrootuti.h"
#include "fitX.h"
#include "MCefficiency.h"

int n = 1;
void eff_draw(std::string input, std::string output)
{
  TFile* inf = TFile::Open(input.c_str());
  fitX::init(inf);
  std::vector<MCeff::MCefficiency*> mceff_a(n+1), mceff_b(n+1);
  std::vector<std::string> tleg_a(n+1), tleg_b(n+1);
  for(int i=0; i<n+1; i++)
    {
      mceff_a[i] = new MCeff::MCefficiency(inf, Form("_a-%d", i));
      mceff_a[i]->calceff();
      mceff_a[i]->setstyle((i?xjjroot::mycolor_middle[xjjroot::cc[i-1]]:kGray+2), 20, 2, 2);
      tleg_a[i] = mceff_a[i]->heffmc()->GetTitle();
      mceff_b[i] = new MCeff::MCefficiency(inf, Form("_b-%d", i));
      mceff_b[i]->calceff();
      mceff_b[i]->setstyle((i?xjjroot::mycolor_middle[xjjroot::cc[i-1]]:kGray+2), 20, 2, 2);
      tleg_b[i] = mceff_b[i]->heffmc()->GetTitle();
    }

  float ymaxeff = 0.2, ymaxeff_incl = 0.05;
  TH2F* hemptyeff = MCeff::createhempty("hemptyeff", "#alpha #times #epsilon_{reco} #times #epsilon_{sel}", ymaxeff);
  TH2F* hemptyeff_incl = MCeff::createhempty_incl("hemptyeff_incl", "#alpha #times #epsilon_{reco} #times #epsilon_{sel}", ymaxeff_incl);

  xjjroot::setgstyle(1);
  TCanvas* c = new TCanvas("c", "", 1800, 600);
  c->Divide(3, 1);
  c->cd(1);
  hemptyeff->Draw("AXIS");
  for(auto& mceff : mceff_a)
    mceff->greff()->Draw("samelX");
  xjjroot::drawtex(0.24, 0.84, fitX::title_a.c_str(), 0.038, 12, 62);
  fitX::drawkinematics();
  xjjroot::drawCMS();
  c->cd(2);
  hemptyeff->Draw("AXIS");
  for(auto& mceff : mceff_b)
    mceff->greff()->Draw("samelX");
  xjjroot::drawtex(0.24, 0.84, fitX::title_b.c_str(), 0.038, 12, 62);
  fitX::drawkinematics();
  xjjroot::drawCMS();
  c->cd(3);
  hemptyeff_incl->Draw("AXIS");
  for(auto& mceff : mceff_a)
    mceff->greff_incl()->Draw("same ple");
  for(auto& mceff : mceff_b)
    mceff->greff_incl()->Draw("same ple");
  for(int i=0; i<tleg_a.size(); i++)
    xjjroot::drawtex(0.25, 0.84-i*0.04, tleg_a[i].c_str(), 0.036, 13, 62, mceff_a[i]->greff()->GetLineColor());
  fitX::drawkinematics();
  xjjroot::drawCMS();
  c->RedrawAxis();
  std::string outputname = "plots/"+output+"/pt/efficiency/ceffsyst.pdf";
  xjjroot::mkdir(outputname);
  c->SaveAs(outputname.c_str());

  float syst_a = 0, effnom_a = mceff_a[0]->greff_incl()->GetEfficiency(fitX::ibin_a);
  for(auto& mceff : mceff_a)
    {
      float dev = fabs(mceff->greff_incl()->GetEfficiency(fitX::ibin_a)-effnom_a);
      if(dev > syst_a) syst_a = dev;
    }
  syst_a /= effnom_a;
  float syst_b = 0, effnom_b = mceff_b[0]->greff_incl()->GetEfficiency(fitX::ibin_b);
  for(auto& mceff : mceff_b)
    {
      float dev = fabs(mceff->greff_incl()->GetEfficiency(fitX::ibin_b)-effnom_b);
      if(dev > syst_b) syst_b = dev;
    }
  syst_b /= effnom_b;
  std::cout<<"\\pt Shape & "<<Form("%.1f", syst_a*100)<<"\\% & "<<Form("%.1f", syst_b*100)<<"\\% & "<<Form("%.1f", sqrt(syst_a*syst_a+syst_b*syst_b)*100)<<"\\% \\\\"<<std::endl;
    
}

int main(int argc, char* argv[])
{
  if(argc==3) { eff_draw(argv[1], argv[2]); return 0; }
  return 1;
}


