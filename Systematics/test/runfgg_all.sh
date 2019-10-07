#date="20191004"
#year="2016"
#tag="test"
date=$1
year=$2
tag=$3

outdir="preapproval_ws_"${year}"_"${date}"_"${tag}"_v2"
stageDir="/hadoop/cms/store/user/hmei/ttH/"${outdir}"/"
intermediateTar="package_"${year}"_"${tag}".tar.gz"

mkdir -p ${outdir}
cp workspaceStd.py ${outdir}

rm $intermediateTar
XZ_OPT='-9 -T 24' tar -Jvc --exclude='.git' --exclude='my*.root' --exclude='*.tar*' --exclude='merged_ntuple*.root' --exclude='*.out' --exclude='*.err' --exclude='*.log' -f $intermediateTar ../../../../../CMSSW_10_5_0/

mkdir -p ${stageDir}
cp $intermediateTar ${stageDir}"package.tar.gz"
hadoop fs -setrep -R 30 ${stageDir}"package.tar.gz"

#fggRunJobs.py --load wsJSONs/legacy_runII_v1_${year}_${tag}.json -d ${outdir} workspaceStd.py -n 300 --no-copy-proxy -b htcondor --stage-to "gsiftp://gftp.t2.ucsd.edu"${stageDir} -q workday doHTXS=True tthTagsOnly=True doSystematics=True doL1Prefiring=True recalculatePDFWeights=True useParentDataset=True
fggRunJobs.py --load wsJSONs/legacy_runII_v1_${year}_${tag}.json -d ${outdir} workspaceStd.py -n 300 --no-copy-proxy -b htcondor --stage-to "gsiftp://gftp.t2.ucsd.edu"${stageDir} -q workday doHTXS=True tthTagsOnly=True doSystematics=True dumpTrees=True dumpWorkspace=False doL1Prefiring=True

