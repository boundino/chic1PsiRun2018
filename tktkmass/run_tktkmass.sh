#!/bin/bash

BDTG=0.7
input=/export/d00/scratch/jwang/BntupleRun2018/mva_output/ntmix_20190711_Bfinder_20190513_HIDoubleMuon__PsiPeri__HIRun2018A_04Apr2019_v1_1033p1_GoldenJSON_skimBpt10_skimhltBsize_trainX_sideband_tktk0p2_BDT_BDTG_CutsGA_CutsSA_LD_10p0_inf_0-10-1-2-9.root
input_ss=/export/d00/scratch/jwang/BntupleRun2018/mva_output_old/crab_Bfinder_samesign_20190513_HIDoubleMuonPsi_HIRun2018A_04Apr2019_v1_1033p1_GoldenJSON_327123_327564_skimhltBsize_ntmix_Xpt10_trainX_sideband_tktk0p2_BDT_BDTG_CutsGA_CutsSA_LD_10p0_inf_0-10-1-2-9_oldLH.root
inputmc_a_prompt=/export/d00/scratch/jwang/BntupleRun2018/mva_output_old/crab_Bfinder_20190520_Hydjet_Pythia8_Psi2SToJpsiPiPi_prompt_1033p1_pt6tkpt0p7dls0_v3_addSamplePthat_pthatweight_trainX_sideband_tktk0p2_BDT_BDTG_CutsGA_CutsSA_LD_10p0_inf_0-10-1-2-9_oldLH.root
inputmc_b_prompt=/export/d00/scratch/jwang/BntupleRun2018/mva_output/ntmix_20190609_Bfinder_20190520_Hydjet_Pythia8_X3872ToJpsiRho_prompt_1033p1_pt6tkpt0p7dls0_v3_addSamplePthat_pthatweight_trainX_sideband_tktk0p2_BDT_BDTG_CutsGA_CutsSA_LD_10p0_inf_0-10-1-2-9.root
inputmc_a_nonprompt=/export/d00/scratch/jwang/BntupleRun2018/mva_output_old/crab_Bfinder_20190520_Hydjet_Pythia8_Psi2SToJpsiPiPi_nonprompt_1033p1_pt6tkpt0p7dls0_v3_addSamplePthat_pthatweight_trainX_sideband_tktk0p2_BDT_BDTG_CutsGA_CutsSA_LD_10p0_inf_0-10-1-2-9_oldLH.root
inputmc_b_nonprompt=/export/d00/scratch/jwang/BntupleRun2018/mva_output_old/crab_Bfinder_20190520_Hydjet_Pythia8_X3872ToJpsiRho_nonprompt_1033p1_pt6tkpt0p7dls0_v3_addSamplePthat_pthatweight_trainX_sideband_tktk0p2_BDT_BDTG_CutsGA_CutsSA_LD_10p0_inf_0-10-1-2-9_oldLH.root

cut="HLT_HIL3Mu0NHitQ10_L2Mu0_MAXdR3p5_M1to5_v1 && pprimaryVertexFilter && phfCoincFilter2Th4 && pclusterCompatibilityFilter && hiBin >=0 && hiBin < 180"
cut="$cut && mvapref && BDTG>${BDTG}"
cutgen="hiBin >=0 && hiBin < 180"

name=trainX_sideband_tktk0p2_10p0_inf_0-10-1-2-9_oldLH
outputdir=rootfiles/$name

mkdir -p $outputdir plots/$name 

g++ tktkmass.C $(root-config --libs --cflags) -g -o tktkmass.exe || exit 1

[[ ${1:-0} -eq 1 ]] && ./tktkmass.exe $input $inputmc_b_prompt "$cut" "$outputdir/tktkmass" "$name"

rm tktkmass.exe

