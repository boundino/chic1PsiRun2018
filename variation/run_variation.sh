#!/bin/bash

# varlatex="BDTG"
# variable="ntp->BDTG[j]"
# varname="BDTGcut"
# iscut=1

# varlatex="BDTG"
# variable="ntp->BDTG[j]"
# varname="BDTG"
# iscut=0

# varlatex="#chi^{2} probability"
# variable="ntp->Bchi2cl[j]"
# varname="Bchi2cl"
# iscut=0

varlatex="|p_{T}^{trk1}-p_{T}^{trk2}|/(p_{T}^{trk1}+p_{T}^{trk2})"
variable="TMath::Abs((ntp->Btrk1Pt[j]-ntp->Btrk2Pt[j]) \/ (ntp->Btrk1Pt[j]+ntp->Btrk2Pt[j]))"
varname="Btrkptimba"
iscut=0

# varlatex="p_{T}^{trk}"
# variable="ntp->Btrk2Pt[j]"
# varname="Btrk2Pt"
# iscut=0

input=/export/d00/scratch/jwang/BntupleRun2018/mva_output/ntmix_20190711_Bfinder_20190513_HIDoubleMuon__PsiPeri__HIRun2018A_04Apr2019_v1_1033p1_GoldenJSON_skimBpt10_skimhltBsize_trainX_sideband_tktk0p2_BDT_BDTG_CutsGA_CutsSA_LD_10p0_inf_0-10-1-2-9.root
input_ss=/export/d00/scratch/jwang/BntupleRun2018/mva_output_old/crab_Bfinder_samesign_20190513_HIDoubleMuonPsi_HIRun2018A_04Apr2019_v1_1033p1_GoldenJSON_327123_327564_skimhltBsize_ntmix_Xpt10_trainX_sideband_tktk0p2_BDT_BDTG_CutsGA_CutsSA_LD_10p0_inf_0-10-1-2-9_oldLH.root
inputmc_a_prompt=/export/d00/scratch/jwang/BntupleRun2018/mva_output_old/crab_Bfinder_20190520_Hydjet_Pythia8_Psi2SToJpsiPiPi_prompt_1033p1_pt6tkpt0p7dls0_v3_addSamplePthat_pthatweight_trainX_sideband_tktk0p2_BDT_BDTG_CutsGA_CutsSA_LD_10p0_inf_0-10-1-2-9_oldLH.root
inputmc_b_prompt=/export/d00/scratch/jwang/BntupleRun2018/mva_output/ntmix_20190609_Bfinder_20190520_Hydjet_Pythia8_X3872ToJpsiRho_prompt_1033p1_pt6tkpt0p7dls0_v3_addSamplePthat_pthatweight_trainX_sideband_tktk0p2_BDT_BDTG_CutsGA_CutsSA_LD_10p0_inf_0-10-1-2-9.root
inputmc_a_nonprompt=/export/d00/scratch/jwang/BntupleRun2018/mva_output_old/crab_Bfinder_20190520_Hydjet_Pythia8_Psi2SToJpsiPiPi_nonprompt_1033p1_pt6tkpt0p7dls0_v3_addSamplePthat_pthatweight_trainX_sideband_tktk0p2_BDT_BDTG_CutsGA_CutsSA_LD_10p0_inf_0-10-1-2-9_oldLH.root
inputmc_b_nonprompt=/export/d00/scratch/jwang/BntupleRun2018/mva_output_old/crab_Bfinder_20190520_Hydjet_Pythia8_X3872ToJpsiRho_nonprompt_1033p1_pt6tkpt0p7dls0_v3_addSamplePthat_pthatweight_trainX_sideband_tktk0p2_BDT_BDTG_CutsGA_CutsSA_LD_10p0_inf_0-10-1-2-9_oldLH.root

name=$varname
# mkdir -p plots/$name/idx plots/chmass_idx/$name plots/chmassmc_idx/$name rootfiles/
mkdir -p plots/$name/idx rootfiles/

##
tmp=`date +%y%m%d`

cp variation.C variation_${name}_${tmp}.C
sed -i "s/__INPUT__FROM__OUTSIDE__/$variable/g" variation_${name}_${tmp}.C
cp drawvariation.C drawvariation_${name}_${tmp}.C
sed -i "s/__INPUT__FROM__OUTSIDE__/$variable/g" drawvariation_${name}_${tmp}.C

g++ variation_${name}_${tmp}.C $(root-config --libs --cflags) -g -o variation_${name}_${tmp}.exe || { rm *_${name}_${tmp}.* ; exit 1 ; }
rm variation_${name}_${tmp}.C
g++ drawvariation_${name}_${tmp}.C $(root-config --libs --cflags) -g -o drawvariation_${name}_${tmp}.exe || { rm *_${name}_${tmp}.* ; exit 1 ; }
rm drawvariation_${name}_${tmp}.C

set -x
[[ ${1:-0} -eq 1 ]] && ./variation_${name}_${tmp}.exe $input $inputmc_a_prompt $inputmc_b_prompt $inputmc_a_nonprompt $inputmc_b_nonprompt "$varlatex" $iscut "$name"
[[ ${2:-0} -eq 1 ]] && ./drawvariation_${name}_${tmp}.exe "$name" "$varlatex" $iscut
set +x
rm drawvariation_${name}_${tmp}.exe
rm variation_${name}_${tmp}.exe

