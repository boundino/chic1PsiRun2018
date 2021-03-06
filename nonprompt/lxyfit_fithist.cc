#include <TH1F.h>
#include <TH2F.h>
#include <TGraphAsymmErrors.h>
#include <TTree.h>
#include <TFile.h>
#include <TLegend.h>
#include <TCanvas.h>
#include <RooDataSet.h>
#include <RooRealVar.h>
#include <RooWorkspace.h>
#include <RooBinning.h>

#include <string>
#include <vector>

#include "var.h"
#include "fit.h"

#include "xjjcuti.h"
#include "xjjrootuti.h"

void drawkinematics();
void lxyfit_fithist(std::string input, std::string output, std::string type, int nsmear)
{
  std::cout<<"\e[32;1m -- "<<__FUNCTION__<<"\e[0m"<<std::endl;

  TFile* inf = new TFile(input.c_str());
  fitX::init(inf);
  output += (fitX::tagname()+"/"+type);

  datamc::var* vv = new datamc::var(type.c_str());
  if(!vv->valid()) return;
  // mass
  RooWorkspace* ww = (RooWorkspace*)inf->Get("ww");
  std::vector<RooDataSet*> dsh(vv->n()-1);
  for(int i=0; i<vv->n()-1; i++) { dsh[i] = (RooDataSet*)ww->data(Form("dsh%d", i)); }
  RooDataSet* dshmcp_a = (RooDataSet*)ww->data("dshmcp_a");
  RooDataSet* dshmcp_b = (RooDataSet*)ww->data("dshmcp_b");
  std::vector<TH1F*> h(vv->n()-1);
  for(int i=0; i<vv->n()-1; i++) { h[i] = (TH1F*)inf->Get(Form("h%d", i)); }
  TH1F* hmcp_a = (TH1F*)inf->Get("hmcp_a");
  hmcp_a->Scale(hmcp_a->GetEntries()/hmcp_a->Integral());
  TH1F* hmcp_b = (TH1F*)inf->Get("hmcp_b");
  hmcp_b->Scale(hmcp_b->GetEntries()/hmcp_b->Integral());

  // distribution
  std::vector<TH1F*> hmcpdis_a(nsmear); 
  for(int i=0;i<nsmear;i++) 
    { hmcpdis_a[i] = (TH1F*)inf->Get(Form("hmcpdis_a_%d",i));
      hmcpdis_a[i]->GetXaxis()->SetNdivisions(505); }
  std::vector<TH1F*> hmcpdis_b(nsmear); 
  for(int i=0;i<nsmear;i++) 
    { hmcpdis_b[i] = (TH1F*)inf->Get(Form("hmcpdis_b_%d",i));
      hmcpdis_b[i]->GetXaxis()->SetNdivisions(505); }
  std::vector<TH1F*> hmcnpdis_a(nsmear);
  for(int i=0;i<nsmear;i++) 
    { hmcnpdis_a[i] = (TH1F*)inf->Get(Form("hmcnpdis_a_%d",i));
      hmcnpdis_a[i]->GetXaxis()->SetNdivisions(505); }
  std::vector<TH1F*> hmcnpdis_b(nsmear);
  for(int i=0;i<nsmear;i++) 
    { hmcnpdis_b[i] = (TH1F*)inf->Get(Form("hmcnpdis_b_%d",i));
      hmcnpdis_b[i]->GetXaxis()->SetNdivisions(505); }
  TH1F* hdis_a = new TH1F("hdis_a", Form(";%s %s;Probability", vv->title().c_str(), vv->unit().c_str()), vv->n()-1, vv->vars().data());
  hdis_a->GetXaxis()->SetNdivisions(505);
  TH1F* hdis_b = new TH1F("hdis_b", Form(";%s %s;Probability", vv->title().c_str(), vv->unit().c_str()), vv->n()-1, vv->vars().data());
  hdis_b->GetXaxis()->SetNdivisions(505);
  RooRealVar* varr = new RooRealVar(vv->formula().c_str(), Form("%s %s", vv->title().c_str(), vv->unit().c_str()), vv->vars().front(), vv->vars().back());

  // fit
  std::vector<TF1*> ff(vv->n()-1, 0);
  std::vector<std::string> tt(vv->n()-1);
  std::vector<Color_t> cc(vv->n()-1);
  std::vector<float> ysig_a(vv->n()-1), ysigerr_a(vv->n()-1), ysig_b(vv->n()-1), ysigerr_b(vv->n()-1);
  RooDataSet* dsh_ws;
  RooWorkspace* wws = new RooWorkspace("wws");
  float ymax = 0;
  for(int i=0;i<vv->n()-1;i++)
    {
      std::string label(Form("%s < %s < %s %s", 
                             xjjc::number_remove_zero(vv->vars()[i]).c_str(),
                             vv->title().c_str(),
                             xjjc::number_remove_zero(vv->vars()[i+1]).c_str(),
                             vv->unit().c_str()));
      std::map<std::string, fitX::fitXresult*> result = fitX::fit(h[i], 0, hmcp_a, hmcp_b,
                                                                  dsh[i], dshmcp_a, dshmcp_b,
                                                                  Form("plots/%s/idx", output.c_str()), 0, true, Form("-%d", i), label); // fix mean = false
      ysig_a[i] = result["unbinned"]->ysig_a();
      ysigerr_a[i] = result["unbinned"]->ysigerr_a();
      ysig_b[i] = result["unbinned"]->ysig_b();
      ysigerr_b[i] = result["unbinned"]->ysigerr_b();
      ff[i] = result["unbinned"]->f();
      xjjroot::setthgrstyle(h[i], xjjroot::mycolor_middle[xjjroot::cc[i]], 20, 0.9, xjjroot::mycolor_middle[xjjroot::cc[i]], 1, 1);
      xjjroot::settfstyle(ff[i], xjjroot::mycolor_middle[xjjroot::cc[i]], 7, 2);
      tt[i] = label;
      cc[i] = xjjroot::mycolor_middle[xjjroot::cc[i]];
      if((vv->gt() && i==0) || (!vv->gt() && i==(vv->n()-2)))
        {
          RooWorkspace* w = result["unbinned"]->ww();
          dsh_ws = (RooDataSet*)w->data(Form("%s_ws", dsh[i]->GetName()));
          std::cout<<"\e[33;1m"<<vv->vars()[i]<<"\e[0m"<<std::endl;
        }
      if(h[i]->GetMaximum() > ymax) { ymax = h[i]->GetMaximum(); }
    }
  ymax *= 1.2;

  wws->import(*dsh_ws);
  dsh_ws->Print("v");
  RooDataSet* dsh_ws_par5 = new RooDataSet(dsh_ws->GetName(), dsh_ws->GetTitle(), dsh_ws, *dsh_ws->get(), 0, "par5_sw");
  RooDataSet* dsh_ws_par10 = new RooDataSet(dsh_ws->GetName(), dsh_ws->GetTitle(), dsh_ws, *dsh_ws->get(), 0, "par10_sw");

  for(int i=0; i<vv->n()-1; i++)
    {
      hdis_a->SetBinContent(i+1, ysig_a[i]);
      hdis_a->SetBinError(i+1, ysigerr_a[i]);
      hdis_b->SetBinContent(i+1, ysig_b[i]);
      hdis_b->SetBinError(i+1, ysigerr_b[i]);
    }
  for(auto& hh : hmcpdis_a) hh->Scale(1./hh->Integral(), "width");
  for(auto& hh : hmcpdis_b) hh->Scale(1./hh->Integral(), "width");
  for(auto& hh : hmcnpdis_a) hh->Scale(1./hh->Integral(), "width");
  for(auto& hh : hmcnpdis_b) hh->Scale(1./hh->Integral(), "width");
  hdis_a->Scale(1./hdis_a->Integral(), "width");
  hdis_b->Scale(1./hdis_b->Integral(), "width");

  std::vector<double> xw_a(vv->n()-1), xw_b(vv->n()-1);
  for(int i=0; i<vv->n()-1; i++)
    {
      xw_a[i] = hmcpdis_a[0]->GetBinCenter(i+1);
      xw_b[i] = hmcpdis_b[0]->GetBinCenter(i+1);
    }

  TGraphAsymmErrors* gdis_a = xjjroot::setwcenter(hdis_a, xw_a, "gdis_a");
  TGraphAsymmErrors* gdis_b = xjjroot::setwcenter(hdis_b, xw_b, "gdis_b");

  // Draw
  for(auto& hh : hmcpdis_a) 
    { hh->SetMinimum(1.e-5);
      hh->SetMaximum(std::max(hh->GetMaximum(), hdis_a->GetMaximum())*1.e+2);
      xjjroot::sethempty(hh, 0, 0);
      xjjroot::setthgrstyle(hh, xjjroot::mycolor_middle["red"], 21, 0.5, xjjroot::mycolor_middle["red"], 1, 1, xjjroot::mycolor_middle["red"], 0.6, 3005); }
  for(auto& hh : hmcnpdis_a)
    { xjjroot::sethempty(hh, 0, 0);
      xjjroot::setthgrstyle(hh, xjjroot::mycolor_middle["azure"], 21, 0.5, xjjroot::mycolor_middle["azure"], 1, 1, xjjroot::mycolor_middle["azure"], 0.6, 3004); }
  xjjroot::sethempty(hdis_a, 0, 0);
  xjjroot::setthgrstyle(hdis_a, kBlack, 47, 1.9, kBlack, 1, 1);
  xjjroot::setthgrstyle(gdis_a, kBlack, 47, 1.9, kBlack, 1, 1);

  for(auto& hh : hmcpdis_b) 
    { hh->SetMinimum(1.e-5);
      hh->SetMaximum(std::max(hh->GetMaximum(), hdis_b->GetMaximum())*1.e+2);
      xjjroot::sethempty(hh, 0, 0);
      xjjroot::setthgrstyle(hh, xjjroot::mycolor_middle["red"], 21, 0.5, xjjroot::mycolor_middle["red"], 1, 1, xjjroot::mycolor_middle["red"], 0.6, 3005); }
  for(auto& hh : hmcnpdis_b)
    { xjjroot::sethempty(hh, 0, 0);
      xjjroot::setthgrstyle(hh, xjjroot::mycolor_middle["azure"], 21, 0.5, xjjroot::mycolor_middle["azure"], 1, 1, xjjroot::mycolor_middle["azure"], 0.6, 3004); }
  xjjroot::sethempty(hdis_b, 0, 0);
  xjjroot::setthgrstyle(hdis_b, kBlack, 47, 1.9, kBlack, 1, 1);
  xjjroot::setthgrstyle(gdis_b, kBlack, 47, 1.9, kBlack, 1, 1);

  TLegend* leg_a = new TLegend(0.22, 0.81-0.047*3, 0.64, 0.81);
  xjjroot::setleg(leg_a, 0.042);
  leg_a->AddEntry(gdis_a, "Data signal", "pl");
  leg_a->AddEntry(hmcpdis_a[0], "MC Prompt template", "pl");
  leg_a->AddEntry(hmcnpdis_a[0], "MC Non-prompt template", "pl");
  TLegend* leg_b = new TLegend(0.22, 0.81-0.047*3, 0.64, 0.81);
  xjjroot::setleg(leg_b, 0.042);
  leg_b->AddEntry(gdis_b, "Data signal", "pl");
  leg_b->AddEntry(hmcpdis_b[0], "MC Prompt template", "pl");
  leg_b->AddEntry(hmcnpdis_b[0], "MC Non-prompt template", "pl");

  TH2F* hempty_a = new TH2F("hempty_a", Form(";m_{#mu#mu#pi#pi} (GeV/c^{2});%s", Form("Entries / %.0f MeV", fitX::BIN_WIDTH*1.e+3)), 
                            fitX::NBIN/2, fitX::BIN_MIN, fitX::BIN_MIN+fitX::BIN_WIDTH*fitX::NBIN/2, 10, 0, ymax);
  hempty_a->GetXaxis()->SetNdivisions(505);
  xjjroot::sethempty(hempty_a, 0, 0);
  TH2F* hempty_b = new TH2F("hempty_b", Form(";m_{#mu#mu#pi#pi} (GeV/c^{2});%s", Form("Entries / %.0f MeV", fitX::BIN_WIDTH*1.e+3)), 
                            fitX::NBIN/2, fitX::BIN_MAX-fitX::BIN_WIDTH*fitX::NBIN/2, fitX::BIN_MAX, 10, 0, ymax);
  hempty_b->GetXaxis()->SetNdivisions(505);
  xjjroot::sethempty(hempty_b, 0, 0);
  
  xjjroot::setgstyle(1);

  TCanvas* c_a = new TCanvas("c_a", "", 1200, 600);
  c_a->Divide(2, 1);
  c_a->cd(1);
  hempty_a->Draw();
  for(int i=0;i<vv->n()-1;i++) { h[i]->Draw("pe same"); ff[i]->Draw("same"); }
  xjjroot::drawtexgroup(0.89, 0.86, tt, 1, 0.5, 0.038, 33, 62, cc);
  xjjroot::drawtex(0.24, 0.84, fitX::title_a.c_str(), 0.038, 12, 62, kBlack);
  xjjroot::drawCMS();
  c_a->cd(2);
  gPad->SetLogy();
  hmcpdis_a[0]->Draw("hist e");
  hmcnpdis_a[0]->Draw("hist e same");
  gdis_a->Draw("pe same");
  drawkinematics();
  xjjroot::drawtex(0.24, 0.84, fitX::title_a.c_str(), 0.038, 12, 62, kBlack);
  leg_a->Draw();
  xjjroot::drawCMS();
  std::string outputname_a(Form("plots/%s/cdis_a.pdf", output.c_str()));
  xjjroot::mkdir(outputname_a);
  c_a->SaveAs(outputname_a.c_str());

  TCanvas* c_b = new TCanvas("c_b", "", 1200, 600);
  c_b->Divide(2, 1);
  c_b->cd(1);
  hempty_b->Draw();
  for(int i=0;i<vv->n()-1;i++) { h[i]->Draw("pe same"); ff[i]->Draw("same"); }
  xjjroot::drawtexgroup(0.89, 0.86, tt, 1, 0.5, 0.038, 33, 62, cc);
  xjjroot::drawtex(0.24, 0.84, fitX::title_b.c_str(), 0.038, 12, 62, kBlack);
  xjjroot::drawCMS();
  c_b->cd(2);
  gPad->SetLogy();
  hmcpdis_b[0]->Draw("hist e");
  hmcnpdis_b[0]->Draw("hist e same");
  gdis_b->Draw("pe same");
  drawkinematics();
  xjjroot::drawtex(0.24, 0.84, fitX::title_b.c_str(), 0.038, 12, 62, kBlack);
  leg_b->Draw();
  xjjroot::drawCMS();
  std::string outputname_b(Form("plots/%s/cdis_b.pdf", output.c_str()));
  xjjroot::mkdir(outputname_b);
  c_b->SaveAs(outputname_b.c_str());

  std::string outputname = std::string("rootfiles/"+output+"/datamc_fithist.root");
  xjjroot::mkdir(outputname);
  TFile* outf = new TFile(outputname.c_str(), "recreate");
  outf->cd();
  for(auto& hh : hmcpdis_a) hh->Write();
  for(auto& hh : hmcpdis_b) hh->Write();
  for(auto& hh : hmcnpdis_a) hh->Write();
  for(auto& hh : hmcnpdis_b) hh->Write();
  hdis_a->Write();
  hdis_b->Write();
  gdis_a->Write();
  gdis_b->Write();
  outf->cd();
  gDirectory->Add(wws);
  wws->Write();
  wws->Print();
  outf->cd();
  fitX::write();
  outf->Close();
}

int main(int argc, char* argv[])
{
  if(argc==5) { lxyfit_fithist(argv[1], argv[2], argv[3], atoi(argv[4])); return 0; }
  return 1;
}

void drawkinematics()
{
  xjjroot::drawtex(0.90, 0.84, fitX::pttag().c_str(), 0.038, 32, 62);
  xjjroot::drawtex(0.90, 0.84-0.04, fitX::ytag().c_str(), 0.038, 32, 62);
  xjjroot::drawtex(0.90, 0.84-0.04*2, fitX::centtag().c_str(), 0.038, 32, 62);
}

