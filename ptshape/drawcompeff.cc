#include <TFile.h>
#include <TH2F.h>
#include <TLegend.h>
#include <TCanvas.h>
#include <string>

#include "xjjcuti.h"
#include "MCefficiency.h"
#include "fitX.h"

void drawcompeff(std::string inputname_a, std::string inputname_b, std::string outputname)
{
  TFile* inf_a = new TFile(Form("%s", inputname_a.c_str()));
  fitX::init(inf_a);
  MCeff::MCefficiency mceff_a(inf_a, "");
  MCeff::MCefficiency mceffweight_a(inf_a, "_weight");
  TFile* inf_b = new TFile(Form("%s", inputname_b.c_str()));
  MCeff::MCefficiency mceff_b(inf_b, "", 1);
  MCeff::MCefficiency mceffweight_b(inf_b, "_weight", 1);

  mceff_a.calceff();
  mceffweight_a.calceff();
  mceff_b.calceff();
  mceffweight_b.calceff();

  float ymaxeff = 0.2, ymaxeff_incl = 0.07;
  TH2F* hemptyeff = new TH2F("hemptyeff", ";p_{T} (GeV/c);#alpha #times #epsilon_{reco} #times #epsilon_{sel}", 10, MCeff::ptBins[0], MCeff::ptBins[MCeff::nPtBins], 10, 0, ymaxeff);
  xjjroot::sethempty(hemptyeff, 0, 0.3);
  TH2F* hemptyeff_incl = new TH2F("hemptyeff_incl", ";p_{T} (GeV/c);#alpha #times #epsilon_{reco} #times #epsilon_{sel}", 10, MCeff::ptBins_incl[0], MCeff::ptBins_incl[MCeff::nPtBins_incl], 10, 0, ymaxeff_incl);
  xjjroot::sethempty(hemptyeff_incl, 0, 0.3);
  // hemptyeff_incl->GetXaxis()->SetLabelSize(hemptyeff_incl->GetXaxis()->GetLabelSize()*1.5);

  mceff_a.setstyle(xjjroot::mycolor_middle["azure"], 20, 1);
  mceffweight_a.setstyle(xjjroot::mycolor_middle["red"], 47, 1);
  mceff_b.setstyle(xjjroot::mycolor_middle["azure"], 20, 2);
  mceffweight_b.setstyle(xjjroot::mycolor_middle["red"], 47, 2);

  TLegend* legeff = new TLegend(0.35, 0.20, 0.95, 0.32);
  xjjroot::setleg(legeff, 0.038);
  legeff->SetNColumns(2);
  legeff->AddEntry(mceff_a.greff(), "#psi(2S)", "fl");
  legeff->AddEntry(mceffweight_a.greff(), "#psi(2S) weight p_{T}", "fl");
  legeff->AddEntry(mceff_b.greff(), "X(3872)", "fl");
  legeff->AddEntry(mceffweight_b.greff(), "X(3872) weight p_{T}", "fl");
  xjjroot::setgstyle();
  TCanvas* ceff = new TCanvas("ceff", "", 1200, 600);
  ceff->Divide(2, 1);
  ceff->cd(1);
  hemptyeff->Draw();
  mceff_a.greff()->Draw("same3");
  mceff_a.greff()->Draw("samelX");
  mceff_b.greff()->Draw("same3");
  mceff_b.greff()->Draw("samelX");
  mceffweight_a.greff()->Draw("same3");
  mceffweight_a.greff()->Draw("samelX");
  mceffweight_b.greff()->Draw("same3");
  mceffweight_b.greff()->Draw("samelX");
  legeff->Draw();
  xjjroot::drawtex(0.23, 0.84, "PYTHIA8 + HYDJET", 0.042, 12, 62);
  xjjroot::drawtex(0.23, 0.77, "Prompt", 0.042, 12, 62);
  xjjroot::drawCMS("Simulation");
  ceff->cd(2);
  hemptyeff_incl->Draw();
  mceff_a.greff_incl()->Draw("same ple");
  mceff_b.greff_incl()->Draw("same ple");
  mceffweight_a.greff_incl()->Draw("same ple");
  mceffweight_b.greff_incl()->Draw("same ple");
  xjjroot::drawtex(0.23, 0.84, "PYTHIA8 + HYDJET", 0.042, 12, 62);
  xjjroot::drawtex(0.23, 0.77, "Prompt", 0.042, 12, 62);
  fitX::drawkinematics();
  legeff->Draw();
  xjjroot::drawCMS("Simulation");
  ceff->SaveAs(Form("%s", outputname.c_str()));

  // print
  float per_a = TMath::Abs(mceffweight_a.greff_incl()->GetEfficiency(1) - mceff_a.greff_incl()->GetEfficiency(1)) / mceff_a.greff_incl()->GetEfficiency(1);
  float per_b = TMath::Abs(mceffweight_b.greff_incl()->GetEfficiency(1) - mceff_b.greff_incl()->GetEfficiency(1)) / mceff_b.greff_incl()->GetEfficiency(1);
  float per_ab = TMath::Sqrt(per_a*per_a + per_b*per_b);
  std::cout<<"\\pt Shape & "<<Form("%.1f", per_a*1.e+2)<<"\\% & "<<Form("%.1f", per_b*1.e+2)<<"\\% & "<<Form("%.1f", per_ab*1.e+2)<<"\\% \\\\"<<std::endl;
}

int main(int argc, char* argv[])
{
  if(argc==4) { drawcompeff(argv[1], argv[2], argv[3]); return 0; }
  return 1;
}
