#include <vector>
#include <iostream>
#include <string>
#include <TMath.h>

//============================================================
// type: psi'(type=0), X(type=1), X/psi'(type=2)
// opt: upper("u"), lower("d")
//============================================================

namespace syst
{
  float getsyst(int type, std::string opt);
}

namespace syst
{
  std::vector<float> syst_fit = {4.6, 4.8, 8.0};
  std::vector<float> syst_acc = {2.6, 0.7, 2.7};
  std::vector<float> syst_eff = {27.1, 45.5, 40.3};
  // std::vector<float> syst_ptshape = {12.4, 2.9, 12.8};
  // std::vector<float> syst_ptshape = {24.0, 11.9, 26.8};
  std::vector<float> syst_ptshape = {24.0, 11.9, 24.0};
  std::vector<float> syst_tnp_u = {6.7, 6.2, 0.5};
  std::vector<float> syst_tnp_d = {6.7, 6.2, 0.6};
  std::vector<float> syst_fprompt = {14.8, 7.9, 8.1};
}

float syst::getsyst(int type, std::string opt)
{
  float syst = 0;
  syst += syst_fit[type]*syst_fit[type];
  syst += syst_acc[type]*syst_acc[type];
  syst += syst_eff[type]*syst_eff[type];
  syst += syst_ptshape[type]*syst_ptshape[type];
  syst += syst_fprompt[type]*syst_fprompt[type];
  if(opt.find("u") != std::string::npos)
    syst += syst_tnp_u[type]*syst_tnp_u[type];
  else if(opt.find("d") != std::string::npos)
    syst += syst_tnp_d[type]*syst_tnp_d[type];
  
  syst = TMath::Sqrt(syst)*1.e-2;
  std::cout<<"==> "<<type<<" "<<opt<<" "<<syst<<std::endl;
  return syst;
}
