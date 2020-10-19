##!/bin/bash
#export XRD_NETWORKSTACK=IPv4
#export X509_USER_PROXY=/afs/cern.ch/user/h/hmei/x509up_u47854
#WD=$PWD
#echo
#echo
#echo
#source $VO_CMS_SW_DIR/cmsset_default.sh
#export SCRAM_ARCH=slc7_amd64_gcc700
#scram project CMSSW CMSSW_10_6_8
#cd CMSSW_10_6_8
#tar zxf /afs/cern.ch/work/h/hmei/ttH/CMSSW_10_6_8/src/flashgg/Taggers/test/testDumper/ttH2017_24Sep20/sandbox.tgz -h
#cp src/XGBoostCMSSW/XGBoostInterface/toolbox/*xml config/toolbox/$SCRAM_ARCH/tools/selected/
#scram setup rabit
#scram setup xgboost
#scram b
#eval $(scram runtime -sh)
#cd $WD
#mkdir ttH2017_24Sep20
#echo "ls $X509_USER_PROXY"
#ls $X509_USER_PROXY
#mkdir .dasmaps 
#mv das_maps_dbs_prod.js .dasmaps/ 
#
#declare -a jobIdsMap=()
#cmsRun /afs/cern.ch/work/h/hmei/ttH/CMSSW_10_6_8/src/flashgg/Taggers/test/test_dumper_clean2.py maxEvents=100 runOnZee=False pujidWP=tight dumpJetSysTrees=False campaign=Era2017_RR-31Mar2018_v1 metaConditions=/afs/cern.ch/work/h/hmei/ttH/CMSSW_10_6_8/src/flashgg/MetaData/data/MetaConditions/Era2017_RR-31Mar2018_v1.json useAAA=True copyInputMicroAOD=True processIdMap=/afs/cern.ch/work/h/hmei/ttH/CMSSW_10_6_8/src/flashgg/Taggers/test/testDumper/ttH2017_24Sep20/config.json dataset=/ttHJetToGG_M125_13TeV_amcatnloFXFX_madspin_pythia8 outputFile=ttH2017_24Sep20/output_ttHJetToGG_M125_13TeV_amcatnloFXFX_madspin_pythia8_spigazzi-Era2017_RR-31Mar2018_v2-legacyRun2FullV1-v0-RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1-8898d473234391c75fcfaef6f4012781_USER.root #nJobs=-1 jobId=${jobIdsMap[${1}]} 
cmsRun /afs/cern.ch/work/h/hmei/ttH/CMSSW_10_6_8/src/flashgg/Taggers/test/test_dumper_clean2.py maxEvents=-1 campaign=Era2017_RR-31Mar2018_v1 metaConditions=/afs/cern.ch/work/h/hmei/ttH/CMSSW_10_6_8/src/flashgg/MetaData/data/MetaConditions/Era2017_RR-31Mar2018_v1.json useAAA=True copyInputMicroAOD=True processIdMap=/afs/cern.ch/work/h/hmei/ttH/CMSSW_10_6_8/src/flashgg/Taggers/test/testDumper/ttH2017_24Sep20/config.json dataset=/ttHJetToGG_M125_13TeV_amcatnloFXFX_madspin_pythia8 outputFile=ttH2017_24Sep20/output_ttHJetToGG_M125_13TeV_amcatnloFXFX_madspin_pythia8_spigazzi-Era2017_RR-31Mar2018_v2-legacyRun2FullV1-v0-RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1-8898d473234391c75fcfaef6f4012781_USER.root #nJobs=-1 jobId=${jobIdsMap[${1}]} 

cd ttH2017_24Sep20
python printEff.py
cd -
#retval=$?
#if [[ $retval != 0 ]]; then
#    retval=$(( ${jobIdsMap[${1}]} + 1 )) 
#fi 
#cd ttH2017_24Sep20
#echo
#echo
#echo "Job finished with exit code ${retval}"
#echo "Files in ouput folder"
#ls -ltr
#if [[ $retval == 0 ]]; then
#    errors=""
#    for file in $(find -name '*.root' -or -name '*.xml'); do
#        echo "cp -pv ${file} /afs/cern.ch/work/h/hmei/ttH/CMSSW_10_6_8/src/flashgg/Taggers/test/testDumper/"
#        cp -pv $file /afs/cern.ch/work/h/hmei/ttH/CMSSW_10_6_8/src/flashgg/Taggers/test/testDumper/
#        if [[ $? != 0 ]]; then
#            errors="$errors $file($?)"
#        fi
#    done
#    if [[ -n "$errors" ]]; then
#       echo "Errors while staging files"
#       echo "$errors"
#       exit -2
#    fi
#fi
#
#exit $retval
#
